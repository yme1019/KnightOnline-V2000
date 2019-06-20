#include "stdafx.h"
#include "KOSocket.h"
#include "packets.h"
#include "version.h"
#include "Compression.h"

KOSocket::KOSocket(uint16_t socketID, SocketMgr * mgr, SOCKET fd, uint32_t sendBufferSize, uint32_t recvBufferSize) 
	: Socket(fd, sendBufferSize, recvBufferSize), 
	m_socketID(socketID), m_remaining(0),  m_usingCrypto(false), 
	m_readTries(0), m_sequence(0), m_lastResponse(0) 
{
	SetSocketMgr(mgr);
}

KOSocket::~KOSocket()
{
}

void KOSocket::OnConnect()
{
	TRACE("Connection received from %s:%d\n", GetRemoteIP().c_str(), GetRemotePort());

	m_remaining = 0;
	m_usingCrypto = false;
	m_readTries = 0;
	m_sequence = 0;
	m_lastResponse = UNIXTIME;
}

void KOSocket::OnRead() 
{
	Packet pkt;

	for (;;) 
	{
		if (m_remaining == 0) 
		{
			if (GetReadBuffer().GetSize() < 5)
				return; //check for opcode as well

			uint16_t header = 0;
			GetReadBuffer().Read(&header, 2);
			if (header != 0x55aa)
			{
				TRACE("%s: Got packet without header 0x55AA, got 0x%X\n", GetRemoteIP().c_str(), header);
				Disconnect();
				return;
			}

			GetReadBuffer().Read(&m_remaining, 2);
			if (m_remaining == 0)
			{
				TRACE("%s: Got packet without an opcode, this should never happen.\n", GetRemoteIP().c_str());
				Disconnect();
				return;
			}
		}

		if (m_remaining > GetReadBuffer().GetAllocatedSize()) 
		{
			TRACE("%s: Packet received which was %u bytes in size, maximum of %u.\n", GetRemoteIP().c_str(), m_remaining, GetReadBuffer().GetAllocatedSize());
			Disconnect();
			return;
		}

		if (m_remaining > GetReadBuffer().GetSize()) 
		{
			if (m_readTries > 4)
			{
				TRACE("%s: packet fragmentation count is over 4, disconnecting as they're probably up to something bad\n", GetRemoteIP().c_str());
				Disconnect();
				return;
			}
			m_readTries++;
			return;
		}

		uint8_t *in_stream = new uint8_t[m_remaining];

		m_readTries = 0;
		GetReadBuffer().Read(in_stream, m_remaining);

		uint16_t footer = 0;
		GetReadBuffer().Read(&footer, 2);

		if (footer != 0xaa55
			|| !DecryptPacket(in_stream, pkt))
		{
			TRACE("%s: Footer invalid (%X) or failed to decrypt.\n", GetRemoteIP().c_str(), footer);
			delete [] in_stream;
			Disconnect();
			return;
		}

		delete [] in_stream;

		// Update the time of the last (valid) response from the client.
		m_lastResponse = UNIXTIME;

		const uint8_t * buffer = pkt.contents();

		// Verify the packet was created successfully.
		// This should never happened as it's guaranteed to contain an opcode as per the above checks.
		if (pkt.wpos() == 0)
		{
			TRACE("%s: Packet has no opcode.\n", GetRemoteIP().c_str());
			Disconnect();
			return;
		}

		if (!HandlePacket(pkt))
		{
			TRACE("%s: Handler for packet %X returned false\n", GetRemoteIP().c_str(), buffer[0]);
#ifndef _DEBUG
			Disconnect();
			return;
#endif
		}

		m_remaining = 0;
	}
}

bool KOSocket::DecryptPacket(uint8_t *in_stream, Packet & pkt)
{
	uint8_t* final_packet = nullptr;

	if (isCryptoEnabled())
	{
		// Invalid packet (all encrypted packets need a CRC32 checksum!)
		if (m_remaining < 4 
			// Invalid checksum 
				|| m_crypto.JvDecryptionWithCRC32(m_remaining, in_stream, in_stream) < 0 
				// Invalid sequence ID
				|| ++m_sequence != *(uint32_t *)(in_stream)) 
				return false;

		m_remaining -= 8; // remove the sequence ID & CRC checksum
		final_packet = &in_stream[4];
	}
	else
	{
		final_packet = in_stream; // for simplicity :P
	}

	pkt = Packet(final_packet[0], (size_t)m_remaining);
	if (m_remaining > 1)
		pkt.append(&final_packet[1], m_remaining - 1);

	return true;
}

bool KOSocket::Send(Packet * pkt) 
{
	if (!IsConnected() || pkt->size() > GetWriteBuffer().GetAllocatedSize())
		return false;

	bool r;

	uint8_t * out_stream = nullptr;
	uint16_t len = (uint16_t)pkt->size();

	if (isCryptoEnabled())
	{
		len += 5; // +5 [1EFC][m_sequence][00]

		out_stream = new uint8_t[len];

		*(uint16_t *)&out_stream[0] = 0x1efc;
		*(uint16_t *)&out_stream[2] = (uint16_t)(m_sequence); // this isn't actually incremented here
		out_stream[4] = 0;
		if (pkt->size() > 0)
			memcpy(&out_stream[5], pkt->contents(), pkt->size());

		m_crypto.JvEncryptionFast(len, out_stream, out_stream);
	}
	else
	{
		out_stream = new uint8_t[len];
		if (pkt->size() > 0)
			memcpy(&out_stream[0], pkt->contents(), pkt->size());
	}

	BurstBegin();

	if (GetWriteBuffer().GetSpace() < size_t(len + 6))
	{
		BurstEnd();
		Disconnect();
		return false;
	}

	r = BurstSend((const uint8_t*)"\xaa\x55", 2);
	if (r) r = BurstSend((const uint8_t*)&len, 2);
	if (r) r = BurstSend((const uint8_t*)out_stream, len);
	if (r) r = BurstSend((const uint8_t*)"\x55\xaa", 2);
	if (r) BurstPush();
	BurstEnd();

	delete [] out_stream;
	return r;
}

bool KOSocket::SendCompressed(Packet * pkt) 
{
	Packet result(WIZ_COMPRESS_PACKET);
	if (!BuildCompressed(pkt, result))
		return Send(pkt);

	return Send(&result);
}

bool KOSocket::BuildCompressed(const Packet * pkt, Packet & result)
{
	uint32_t inLength = pkt->size(), crc = 0;
	uint32_t outLength = 0;

	if(inLength < Compression::MinBytes)
		return false;

	uint8_t * outBuffer = Compression::CompressWithCRC32(pkt->contents(), inLength, &outLength, &crc);

	result << uint16_t(outLength) << uint16_t(inLength) << crc;
	result.append(outBuffer, outLength);

	delete[] outBuffer;
	return true;
}

void KOSocket::OnDisconnect()
{
	TRACE("Connection closed from %s:%d\n", GetRemoteIP().c_str(), GetRemotePort());
}

void KOSocket::EnableCrypto()
{
#ifdef USE_CRYPTION
	m_crypto.Init();
	m_usingCrypto = true;
#endif
}

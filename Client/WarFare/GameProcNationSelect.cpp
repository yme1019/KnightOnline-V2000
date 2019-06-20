#include "PacketDef.h"
#include "GameEng.h"
#include "APISocket.h"

#include "GameProcNationSelect.h"
#include "PlayerMySelf.h"
#include "UINationSelectDlg.h"
#include "UIManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CGameProcNationSelect::CGameProcNationSelect()
{
	m_pUINationSelectDlg = NULL;
	s_pPlayer->m_InfoBase.eNation = NATION_NOTSELECTED;
}

CGameProcNationSelect::~CGameProcNationSelect()
{
	delete m_pUINationSelectDlg; m_pUINationSelectDlg = NULL;
}

void CGameProcNationSelect::Release()
{
	CGameProcedure::Release();

	delete m_pUINationSelectDlg; m_pUINationSelectDlg = NULL;
}

void CGameProcNationSelect::Init()
{
	CGameProcedure::Init();

	std::string szTemp = "UI_US\\Co_Nation_Select_us.uif"; 
	_TBL_TABLE_UI* pTbl = file_Tbl_UI.Find(NATION_KARUS);
	if(pTbl)
	{
		szTemp = pTbl->Co_nationselect_us;
	}
	
	m_pUINationSelectDlg = new CUINationSelectDlg();
	m_pUINationSelectDlg->Init(s_pUIMgr);
	m_pUINationSelectDlg->LoadFromFile(szTemp);
	m_pUINationSelectDlg->m_pProcNationSelectRef = this;

	s_pPlayer->m_InfoBase.eNation = NATION_NOTSELECTED;
}

void CGameProcNationSelect::Tick()
{
	CGameProcedure::Tick();

	if(NATION_KARUS == s_pPlayer->m_InfoBase.eNation || NATION_ELMORAD == s_pPlayer->m_InfoBase.eNation)
		CGameProcedure::ProcActiveSet((CGameProcedure*)s_pProcCharacterSelect);
}

void CGameProcNationSelect::Render()
{
	uint32_t color = 0x00000000;
	s_pEng->Clear(color);
	s_pEng->s_lpD3DDev->BeginScene();

	CGameProcedure::Render();

	s_pEng->s_lpD3DDev->EndScene();
	s_pEng->Present(CN3Base::s_hWndBase);
}


void CGameProcNationSelect::MsgSendNationSelect(e_Nation eNation)
{
	uint8_t byBuff[4];									
	int iOffset=0;									

	CAPISocket::MP_AddByte(byBuff, iOffset, WIZ_SEL_NATION);	
	CAPISocket::MP_AddByte(byBuff, iOffset, (uint8_t)eNation);		
		
	s_pSocket->Send(byBuff, iOffset);								

	s_pUIMgr->EnableOperationSet(false);
}

bool CGameProcNationSelect::ProcessPacket(Packet& pkt)
{
	size_t rpos = pkt.rpos();
	if (CGameProcedure::ProcessPacket(pkt))
		return true;

	pkt.rpos(rpos);

	int iCmd = pkt.read<uint8_t>();
	switch ( iCmd )										
	{
		case WIZ_SEL_NATION:							
		{
			int iNation = pkt.read<uint8_t>();

			if( 0 == iNation ) 	s_pPlayer->m_InfoBase.eNation = NATION_NOTSELECTED;
			else if( 1 == iNation ) s_pPlayer->m_InfoBase.eNation = NATION_KARUS;
			else if( 2 == iNation ) s_pPlayer->m_InfoBase.eNation = NATION_ELMORAD;
		}
		return true;
	}
	
	return false;
}
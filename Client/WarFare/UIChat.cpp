#include "UIChat.h"
#include "PacketDef.h"
#include "GameProcMain.h"
#include "UIMessageWnd.h"
#include "UIManager.h"

#include "N3UIString.h"
#include "N3UIScrollBar.h"
#include "N3UIEdit.h"								

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CUIChat::CUIChat()
{
	m_pChatOut = NULL;
	m_pScrollbar = NULL;
	m_iChatLineCount = 0;
	m_ppUILines = NULL;
	m_iCurContinueMsg = 0;
	ZeroMemory(&m_rcChatOutRegion, sizeof(m_rcChatOutRegion));

	m_eChatMode = N3_CHAT_NORMAL;
	//m_eChatBuffer = CHAT_BUFFER_NORMAL;

	m_pBtn_Normal = NULL;
	m_pBtn_Private = NULL;
	m_pBtn_PartyOrForce = NULL;
	m_pBtn_KnightsOrGuild = NULL;
	m_pBtn_Shout = NULL;
	m_pNoticeTitle = NULL;
	m_pBtn_Fold = NULL;
	m_pEdit = NULL;

	m_bChatNormal	= true;
	m_bChatPrivate	= true;
	m_bChatClan		= true;
	m_bChatShout	= true;
	m_bChatParty	= true;

	m_bKillFocus	= false;
}

CUIChat::~CUIChat()
{
	if (m_ppUILines) 
	{
		delete [] m_ppUILines; m_ppUILines = NULL;
	}

	ChatListItor itor;
//	for(int i = 0; i < CHAT_BUFFER_COUNT; i++)
//	{
//		for(itor = m_ChatBuffers[i].begin(); m_ChatBuffers[i].end() != itor; ++itor)
		for(itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
		{
			__ChatInfo* pChatInfo = (*itor);
			if (pChatInfo) delete pChatInfo;
		}
//		m_ChatBuffers[i].clear();
		m_ChatBuffer.clear();

//		for(itor = m_LineBuffers[i].begin(); m_LineBuffers[i].end() != itor; ++itor)
		for(itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
		{
			__ChatInfo* pChatInfo = (*itor);
			if (pChatInfo) delete pChatInfo;
		}
//		m_LineBuffers[i].clear();
		m_LineBuffer.clear();
//	}

	DeleteContinueMsg();
}

void CUIChat::Release()
{
	CN3UIBase::Release();

	m_pEdit = NULL;
	m_pChatOut = NULL;
	m_pScrollbar = NULL;
	m_iChatLineCount = 0;
	if (m_ppUILines) 
	{
		delete [] m_ppUILines; m_ppUILines = NULL;
	}

	ZeroMemory(&m_rcChatOutRegion, sizeof(m_rcChatOutRegion));

	ChatListItor itor;
//	for(int i = 0; i < CHAT_BUFFER_COUNT; i++)
//	{
//		for(itor = m_ChatBuffers[i].begin(); m_ChatBuffers[i].end() != itor; ++itor)
		for(itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
		{
			__ChatInfo* pChatInfo = (*itor);
			if (pChatInfo) delete pChatInfo;
		}
//		m_ChatBuffers[i].clear();
		m_ChatBuffer.clear();

//		for(itor = m_LineBuffers[i].begin(); m_LineBuffers[i].end() != itor; ++itor)
		for(itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
		{
			__ChatInfo* pChatInfo = (*itor);
			if (pChatInfo) delete pChatInfo;
		}
//		m_LineBuffers[i].clear();
		m_LineBuffer.clear();
//	}

	m_eChatMode = N3_CHAT_NORMAL;
	//m_eChatBuffer = CHAT_BUFFER_NORMAL;

	m_pBtn_Normal = NULL;
	m_pBtn_Private = NULL;
	m_pBtn_PartyOrForce = NULL;
	m_pBtn_KnightsOrGuild = NULL;
	m_pBtn_Shout = NULL;
	m_pBtn_Fold = NULL;
}

bool CUIChat::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if(NULL == pSender) return false;

	if(dwMsg == UIMSG_BUTTON_CLICK)
	{
		e_ChatMode eCM = N3_CHAT_UNKNOWN;

		if(pSender == m_pBtn_Normal) 
			eCM = N3_CHAT_NORMAL;
		else if(pSender == m_pBtn_Private) 
			eCM = N3_CHAT_PRIVATE;
		else if(pSender == m_pBtn_PartyOrForce) 
			eCM = N3_CHAT_PARTY;
		else if(pSender == m_pBtn_KnightsOrGuild) 
			eCM = N3_CHAT_CLAN;
		else if(pSender == m_pBtn_Shout)
			eCM = N3_CHAT_SHOUT;
		else if(pSender == m_pBtn_Check) 
			ChatListenEnable();
		else if (pSender == m_pBtn_Fold)
		{
			CGameProcedure::s_pProcMain->CommandToggleUIChat();
			return true;
		}

		if(N3_CHAT_UNKNOWN != eCM)
			this->ChangeChattingMode(eCM);
	}
	else if (dwMsg == UIMSG_SCROLLBAR_POS)
	{
		int iCurLinePos = m_pScrollbar->GetCurrentPos();
		SetTopLine(iCurLinePos);
		return true;
	}

	else if (dwMsg == UIMSG_EDIT_RETURN)
	{													
		CN3UIEdit* pEdit = (CN3UIEdit*)pSender;
		m_szString = m_pEdit->GetString();
		int iStrLen = m_szString.size();
		if (iStrLen > 0)
		{
			if(iStrLen > 1 && '/' == m_szString[0])
			{
				CGameProcedure::s_pProcMain->ParseChattingCommand(m_szString);
			}
			else if(iStrLen > 1 && '@' == m_szString[0])
			{
				int iTmp = m_szString.find(' ');

				if(iTmp > 0)
				{
					std::string szID = m_szString.substr(1, iTmp-1);
					std::string szMsg = m_szString.substr(iTmp);

					int iTmp2 = szMsg.find_first_not_of(" ");
					szMsg = szMsg.substr(iTmp2);

					CGameProcedure::s_pProcMain->MsgSend_ChatSelectTarget(szID);
					CGameProcedure::s_pProcMain->MsgSend_Chat(N3_CHAT_PRIVATE, szMsg);
				}
			}
			else if(iStrLen > 1 && '#' == m_szString[0])
			{
				CGameProcedure::s_pProcMain->MsgSend_Chat(N3_CHAT_PARTY, &(m_szString[1]));
			}
			else if(iStrLen > 1 && '$' == m_szString[0])
			{
				CGameProcedure::s_pProcMain->MsgSend_Chat(N3_CHAT_CLAN, &(m_szString[1]));
			}
			else if(iStrLen > 1 && '!' == m_szString[0])
			{
				CGameProcedure::s_pProcMain->MsgSend_Chat(N3_CHAT_SHOUT, &(m_szString[1]));
			}
			else
			{
				CGameProcedure::s_pProcMain->MsgSend_Chat(m_eChatMode, m_szString);
				//this->SetFocus();
			}
		}
		//this->SetString("");
		//::SetWindowText(s_hWndEdit, "");
		SetEnableKillFocus(true);
		return true;
	}
	return true;
}

void CUIChat::CreateLines()
{
	int i;
	if (m_ppUILines) {
		for (i=0; i<m_iChatLineCount; ++i)
		{
			if (m_ppUILines[i]) {delete m_ppUILines[i]; m_ppUILines[i] = NULL;}
		}
		delete [] m_ppUILines; m_ppUILines = NULL;
	}
	SIZE size;
	if (m_pChatOut && m_pChatOut->GetTextExtent("가", lstrlen("가"), &size) && size.cy>0)
	{
		m_iChatLineCount = (m_rcChatOutRegion.bottom - m_rcChatOutRegion.top)/size.cy;
	}
	else return;
	const std::string& szFontName = m_pChatOut->GetFontName();
	uint32_t dwFontHeight = m_pChatOut->GetFontHeight();
	uint32_t dwFontFlag = m_pChatOut->GetFontFlags();

	if (m_iChatLineCount<=0 || szFontName.empty()) return;

	m_ppUILines = new CN3UIString*[m_iChatLineCount];
	for (i=0; i<m_iChatLineCount; ++i)
	{
		RECT rc;
		SetRect(&rc, m_rcChatOutRegion.left, m_rcChatOutRegion.top+(i*size.cy),
									m_rcChatOutRegion.right, m_rcChatOutRegion.top+((i+1)*size.cy));
		m_ppUILines[i] = new CN3UIString();
		m_ppUILines[i]->Init(this);
		m_ppUILines[i]->SetRegion(rc);
		m_ppUILines[i]->SetStyle(UISTYLE_STRING_SINGLELINE|UISTYLE_STRING_ALIGNLEFT|UISTYLE_STRING_ALIGNTOP);
		m_ppUILines[i]->SetFont(szFontName, dwFontHeight, dwFontFlag & D3DFONT_BOLD, dwFontFlag & D3DFONT_ITALIC);
	}
}

bool CUIChat::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;
	m_pChatOut		= (CN3UIString*)GetChildByID("text0");				
	__ASSERT(m_pChatOut, "NULL UI Component!!");

	m_pScrollbar	= (CN3UIScrollBar*)GetChildByID("scroll");			
	__ASSERT(m_pScrollbar, "NULL UI Component!!");

	m_pNoticeTitle	= (CN3UIString*)GetChildByID("text_notice_title");	
	__ASSERT(m_pNoticeTitle, "NULL UI Component!!");

	m_rcChatOutRegion = m_pChatOut->GetRegion();
	CreateLines();

	__ASSERT(0<m_iChatLineCount,"채팅창이 너무 작아요");

	m_pEdit = (CN3UIEdit*)GetChildByID("edit0");				
	__ASSERT(m_pEdit, "NULL UI Component!!");

	m_pEdit->SetMaxString(256);

	m_pBtn_Normal			= GetChildByID("btn_normal");			__ASSERT(m_pBtn_Normal, "NULL UI Component!!");
	m_pBtn_Private			= GetChildByID("btn_private");			__ASSERT(m_pBtn_Private, "NULL UI Component!!");
	m_pBtn_PartyOrForce		= GetChildByID("btn_party_force");		__ASSERT(m_pBtn_PartyOrForce, "NULL UI Component!!");
	//m_pBtn_KnightsOrGuild = GetChildByID("btn_knights_guild");	__ASSERT(m_pBtn_KnightsOrGuild, "NULL UI Component!!");
	m_pBtn_KnightsOrGuild	= GetChildByID("btn_knights");			__ASSERT(m_pBtn_KnightsOrGuild, "NULL UI Component!!");
	m_pBtn_Shout			= GetChildByID("btn_shout");			__ASSERT(m_pBtn_Shout, "NULL UI Component!!");
	m_pBtn_Check			= GetChildByID("btn_check_normal");		__ASSERT(m_pBtn_Check, "NULL UI Component!!");
	m_pBtn_Fold				= GetChildByID("btn_off");				__ASSERT(m_pBtn_Fold, "NULL UI Component!!");

	this->ChangeChattingMode(N3_CHAT_NORMAL);

	return true;
}

void CUIChat::AddChatMsg(e_ChatMode eCM, const std::string& szString, D3DCOLOR color)
{
	if(szString.empty()) return;

	switch(eCM)
	{
	case N3_CHAT_NORMAL:
		if(!m_bChatNormal) return;
		break;
	case N3_CHAT_PRIVATE:
		if(!m_bChatPrivate) return;
		break;
	case N3_CHAT_PARTY:
	case N3_CHAT_FORCE:
		if(!m_bChatParty) return;
		break;
	case N3_CHAT_CLAN:
		if(!m_bChatClan) return;
		break;
	case N3_CHAT_SHOUT:
		if(!m_bChatShout) return;
		break;
	}

//	N3_CHAT_NORMAL = 1, N3_CHAT_PRIVATE, N3_CHAT_PARTY, N3_CHAT_FORCE, N3_CHAT_SHOUT, N3_CHAT_KNIGHTS, N3_CHAT_PUBLIC
//	e_ChatBuffer eCB = CHAT_BUFFER_NORMAL; // Chatting Mode Index 0 - Normal, shout, notice | 1 - private | 2 - party force | 3 - Knights
//	switch(eCM)
//	{
//	case N3_CHAT_NORMAL:
//	case N3_CHAT_SHOUT:
//	case N3_CHAT_PUBLIC:
//		eCB = CHAT_BUFFER_NORMAL;
//		break;
//	case N3_CHAT_PRIVATE: 
//		eCB = CHAT_BUFFER_PRIVATE;
//		break;
//	case N3_CHAT_PARTY:
//	case N3_CHAT_FORCE:
//		eCB = CHAT_BUFFER_PARTY;
//		break;
//	case N3_CHAT_KNIGHTS:
//		eCB = CHAT_BUFFER_KNIGHTS;
//		break;
//	}

	// 일반 ChatBuffer에 넣기
//	if(CHAT_BUFFER_NORMAL != eCB)
//	{
		__ChatInfo* pChatInfo = new __ChatInfo(szString, color);
//		m_ChatBuffers[CHAT_BUFFER_NORMAL].push_back(pChatInfo);
		m_ChatBuffer.push_back(pChatInfo);
//		if (m_ChatBuffers[CHAT_BUFFER_NORMAL].size() > 255)	// 255개가 넘으면 앞에서부터 지우기
		if (m_ChatBuffer.size() > 255)	// 255개가 넘으면 앞에서부터 지우기
		{
//			__ChatInfo* pTemp = m_ChatBuffers[CHAT_BUFFER_NORMAL].front();
			__ChatInfo* pTemp = m_ChatBuffer.front();
			if (pTemp) delete pTemp;

//			m_ChatBuffers[CHAT_BUFFER_NORMAL].pop_front();
			m_ChatBuffer.pop_front();
		}
//		this->AddLineBuffer(CHAT_BUFFER_NORMAL, szString, color); // line buffer 에 넣기
		this->AddLineBuffer(szString, color); // line buffer 에 넣기
//	}

	// ChatBuffer에 넣기
//	__ChatInfo* pChatInfo = new __ChatInfo(szString, color);
//	m_ChatBuffers[eCB].push_back(pChatInfo);
//	if (m_ChatBuffers[eCB].size() > 255)	// 255개가 넘으면 앞에서부터 지우기
//	{
//		__ChatInfo* pTemp = m_ChatBuffers[eCB].front();
//		if (pTemp) delete pTemp;
//
//		m_ChatBuffers[eCB].pop_front();
//	}
//	this->AddLineBuffer(eCB, szString, color); // line buffer 에 넣기

	this->AdjustScroll();
}

void CUIChat::AdjustScroll()
{
	// Line buffer 갯수 조절
	int iCurLinePos = m_pScrollbar->GetCurrentPos();	// 현재 scroll bar가 가리키고 있는 line
	BOOL bAutoScroll = (m_pScrollbar->GetMaxPos() == iCurLinePos) ? TRUE : FALSE;

	while (m_LineBuffer.size() > MAX_CHAT_LINES && 0 < iCurLinePos)	
	{
		//__ChatInfo* pTemp = m_LineBuffers[m_eChatBuffer].front();
		__ChatInfo* pTemp = m_LineBuffer.front();
		if (pTemp) delete pTemp;
		//m_LineBuffers[m_eChatBuffer].pop_front();
		m_LineBuffer.pop_front();
		--iCurLinePos;	
	}

//	int iLineBufferSize = m_LineBuffers[m_eChatBuffer].size();
	int iLineBufferSize = m_LineBuffer.size();
	int iMaxScrollPos = iLineBufferSize-m_iChatLineCount;
	if (iMaxScrollPos<0) iMaxScrollPos = 0;
	m_pScrollbar->SetRange(0, iMaxScrollPos);

	if ( bAutoScroll) iCurLinePos = iMaxScrollPos;
	if (iCurLinePos < 0) iCurLinePos = 0;

	m_pScrollbar->SetCurrentPos(iCurLinePos);

	SetTopLine(iCurLinePos);
}

void CUIChat::AddLineBuffer(const std::string& szString, D3DCOLOR color)
{
	if(szString.empty()) return;

	__ASSERT(m_pChatOut, "");
	const int iStrLen = szString.size();

	SIZE size;
	if (FALSE == m_pChatOut->GetTextExtent(szString, iStrLen, &size)) 
	{
		__ASSERT(0,"no device context"); 
		return;
	}

	const int iRegionWidth = m_rcChatOutRegion.right - m_rcChatOutRegion.left;

	int iCX=0;
	int iCount = 0;
	int iLineStart = 0;

	while(iCount<iStrLen)
	{
		if ('\n' == szString[iCount])
		{
			__ChatInfo* pLineInfo = new __ChatInfo;
			//m_LineBuffers[eCB].push_back(pLineInfo);
			m_LineBuffer.push_back(pLineInfo);

			pLineInfo->color = color;
			if ((iCount - iLineStart)>0)
			{
				int iLineLength = iCount - iLineStart + 1;
				std::string szLine;
				pLineInfo->szChat = szString.substr(iLineStart, iLineLength);
			}

			++iCount;
			iLineStart = iCount;
			iCX = 0;
		}
		else
		{
			int iCC=0;
			if (0x80 & szString[iCount])	iCC = 2;
			else							
				iCC = 1;	

			BOOL bFlag = m_pChatOut->GetTextExtent(&(szString[iCount]), iCC, &size);
			__ASSERT(bFlag, "cannot get size of dfont");
			if ((iCX+size.cx) > iRegionWidth)	
			{				
				int iLineLength = iCount - iLineStart;
				if (iLineLength>0)
				{
					__ChatInfo* pLineInfo = new __ChatInfo;
					//m_LineBuffers[eCB].push_back(pLineInfo);
					m_LineBuffer.push_back(pLineInfo);

					pLineInfo->color = color;
					pLineInfo->szChat = szString.substr(iLineStart, iLineLength);
				}
				else
				{
					__ASSERT(iRegionWidth>15, "너무 좁아서 한글자도 찍을 수가 없다");
					break;
				}
				iLineStart = iCount;
				iCX = 0;
			}
			iCount += iCC;
			iCX += size.cx;
		}
	}

	int iLineLength = iStrLen - iLineStart;
	if (iLineLength>0)
	{
		__ChatInfo* pLineInfo = new __ChatInfo;
//		m_LineBuffers[eCB].push_back(pLineInfo);
		m_LineBuffer.push_back(pLineInfo);

		pLineInfo->color = color;
		pLineInfo->szChat = szString.substr(iLineStart, iLineLength);
	}
}

void CUIChat::SetTopLine(int iTopLine)
{
	if (m_iChatLineCount<=0) return;

	const int iLineBufferSize = m_LineBuffer.size();
	if (iTopLine<0) iTopLine = 0;
	else if (iTopLine > iLineBufferSize) iTopLine = iLineBufferSize;
	
	int i;
	__ChatInfo** ppLineInfos  = new __ChatInfo*[m_iChatLineCount];
	ZeroMemory(ppLineInfos, sizeof(__ChatInfo*)*m_iChatLineCount);

	int iCurLine = 0;
	for (i=0; i<m_iChatLineCount; ++i)
	{
		iCurLine = iTopLine + i;
		if (iLineBufferSize <= iCurLine) 
			break;

		//ppLineInfos[i] = m_LineBuffers[m_eChatBuffer][iCurLine];
		ppLineInfos[i] = m_LineBuffer[iCurLine];
	}

	__ASSERT(m_ppUILines, "null pointer");
	int iRealLine = i;	
	int iRealLineCount = 0;
	for (i=0; i<iRealLine; ++i)
	{
		++iRealLineCount;
		if (NULL == m_ppUILines[i]) 
			continue;

		m_ppUILines[i]->SetColor(ppLineInfos[i]->color);
		m_ppUILines[i]->SetString(ppLineInfos[i]->szChat);
	}
	for (i=iRealLineCount; i<m_iChatLineCount; ++i)
	{
		if (NULL == m_ppUILines[i])
			continue;

		m_ppUILines[i]->SetString("");
	}
	delete [] ppLineInfos;
}

void CUIChat::RecalcLineBuffers()
{
	int iMaxScrollPos = 0;
//	for(int i = 0; i < CHAT_BUFFER_COUNT; i++)
//	{
		// line buffer
		ChatListItor itor;
//		for(itor = m_LineBuffers[i].begin(); m_LineBuffers[i].end() != itor; ++itor)
		for(itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
		{
			__ChatInfo* pLineBuff = (*itor);
			if (pLineBuff) delete pLineBuff;
		}
//		m_LineBuffers[i].clear();
		m_LineBuffer.clear();

		// Line buffer
//		for(itor = m_ChatBuffers[i].begin(); m_ChatBuffers[i].end() != itor; ++itor)
		for(itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
		{
			__ChatInfo* pChatBuff = (*itor);
//			if (pChatBuff) AddLineBuffer((e_ChatBuffer)i, pChatBuff->szChat, pChatBuff->color);
			if (pChatBuff) AddLineBuffer(pChatBuff->szChat, pChatBuff->color);
		}

		// Line buffer
//		while (m_LineBuffers[i].size() > MAX_CHAT_LINES)	
		while (m_LineBuffer.size() > MAX_CHAT_LINES)	
		{
			// 한줄 지우기
//			__ChatInfo* pLineBuff = m_LineBuffers[i].front();
			__ChatInfo* pLineBuff = m_LineBuffer.front();
			if (pLineBuff) delete pLineBuff;
//			m_LineBuffers[i].pop_front();
			m_LineBuffer.pop_front();
		}

//		if(i == m_eChatBuffer)
//		{
//			int iLineBufferSize = m_LineBuffers[i].size();
			int iLineBufferSize = m_LineBuffer.size();
			iMaxScrollPos = iLineBufferSize-m_iChatLineCount;
//		}
//	}

	if (iMaxScrollPos<0) iMaxScrollPos = 0;
	m_pScrollbar->SetRange(0, iMaxScrollPos);
	m_pScrollbar->SetCurrentPos(iMaxScrollPos);
	SetTopLine(iMaxScrollPos);
}

void CUIChat::SetFocus()
{
	SetEnableKillFocus(false);
	this->SetString("");
	if (m_pEdit) m_pEdit->SetFocus();
}

void CUIChat::KillFocus()
{
	this->SetString("");
	if (m_pEdit) m_pEdit->KillFocus();
}

BOOL CUIChat::IsChatMode()
{
	return ((m_pEdit && GetFocusedEdit() == m_pEdit) ? TRUE : FALSE);
}

void CUIChat::SetString(const std::string& szChat)
{
	m_szString = szChat;
	if(m_pEdit)
	{
		m_pEdit->SetString(m_szString);
		m_pEdit->SetCaretPos(m_szString.size());
	}
}

void CUIChat::SetCaretPos(int iPos)
{
	if (IsChatMode() && m_pEdit)
	{
		m_pEdit->SetCaretPos(iPos);
	}
}

BOOL CUIChat::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (0 == iOffsetX && 0 == iOffsetY) return FALSE;

	m_rcRegion.left += iOffsetX;		m_rcRegion.top += iOffsetY;
	m_rcRegion.right += iOffsetX;		m_rcRegion.bottom += iOffsetY;

	m_rcMovable.left += iOffsetX;		m_rcMovable.top += iOffsetY;
	m_rcMovable.right += iOffsetX;		m_rcMovable.bottom += iOffsetY;

	CN3UIBase* pCUI = NULL;
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		pCUI = (*itor);
		__ASSERT(pCUI, "child UI pointer is NULL!");
		pCUI->MoveOffset(iOffsetX, iOffsetY);
	}

	/*if(CGameProcedure::s_pProcMain->m_pUIMsgDlg)
	{
	POINT pt = CGameProcedure::s_pProcMain->m_pUIMsgDlg->GetPos();
	RECT rt = this->GetRegion();
	if( (rt.right != pt.x) || (rt.top != pt.y) )
	{
	CGameProcedure::s_pProcMain->m_pUIMsgDlg->SetPos(rt.right, rt.top);
	}
	}*/

	//NOTE: (madpew) Don't stick them together, so chat and info can move freely, but sync with the folded version instead
	POINT pt = this->GetPos();
	RECT rc = this->GetRegion();
	RECT rc2 = CGameProcedure::s_pProcMain->m_pUIChatDlg2->GetRegion();
	CGameProcedure::s_pProcMain->m_pUIChatDlg2->SetPos(pt.x, rc.bottom + (rc2.top - rc2.bottom));

	return true;
}

void CUIChat::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	 //m_rcChatOutRegion = ;	
	//CreateLines();	
	//RecalcLineBuffers();
}

void CUIChat::ChangeChattingMode(e_ChatMode eCM)
{
	m_eChatMode = eCM;
//	e_ChatBuffer eCBPrev = m_eChatBuffer;

	bool bNBDs[5] = { false, false, false, false, false };
	bool bICLs[5] = { m_bChatNormal, m_bChatPrivate, m_bChatParty, m_bChatClan, m_bChatShout };
	CN3UIBase* pBtns[5] = { m_pBtn_Normal, m_pBtn_Private, m_pBtn_PartyOrForce, m_pBtn_KnightsOrGuild, m_pBtn_Shout };

	switch(m_eChatMode)
	{
	case N3_CHAT_NORMAL:
	case N3_CHAT_PUBLIC:
	case N3_CHAT_CONTINUE:
//		m_eChatBuffer = CHAT_BUFFER_NORMAL;
		bNBDs[0] = true;
		break;
	case N3_CHAT_PRIVATE:
//		m_eChatBuffer = CHAT_BUFFER_PRIVATE;
		bNBDs[1] = true;
		break;
	case N3_CHAT_PARTY:
	case N3_CHAT_FORCE:
//		m_eChatBuffer = CHAT_BUFFER_PARTY; 
		bNBDs[2] = true;
		break;
	case N3_CHAT_CLAN:
//		m_eChatBuffer = CHAT_BUFFER_KNIGHTS;
		bNBDs[3] = true;
		break;
	case N3_CHAT_SHOUT:
//		m_eChatBuffer = CHAT_BUFFER_NORMAL;
		bNBDs[4] = true;
		break;
	}

	for(int i = 0; i < 5; i++)
	{
		if(NULL == pBtns[i]) continue;
		
		if(bNBDs[i])
		{
			if(m_pBtn_Check)
			{
				if(bICLs[i])	m_pBtn_Check->SetState(UI_STATE_BUTTON_DOWN);
				else			m_pBtn_Check->SetState(UI_STATE_BUTTON_NORMAL);
			}
			pBtns[i]->SetState(UI_STATE_BUTTON_DOWN);
		}
		else pBtns[i]->SetState(UI_STATE_BUTTON_NORMAL);
	}

//	if(eCBPrev != m_eChatBuffer) this->AdjustScroll(); // 채팅 모드가 달라지면..
}

void CUIChat::ChatListenEnable()
{
	switch(m_eChatMode)
	{
	case N3_CHAT_NORMAL:
		m_bChatNormal = !m_bChatNormal;
		break;
	case N3_CHAT_PRIVATE:
		m_bChatPrivate = !m_bChatPrivate;
		break;
	case N3_CHAT_PARTY:
	case N3_CHAT_FORCE:
		m_bChatParty = !m_bChatParty;
		break;
	case N3_CHAT_CLAN:
		m_bChatClan = !m_bChatClan;
		break;
	case N3_CHAT_SHOUT:
		m_bChatShout = !m_bChatShout;
		break;
	}
}

bool CUIChat::OnKeyPress(int iKey)
{
	switch(iKey)
	{
	case SDL_SCANCODE_ESCAPE://DIK_ESCAPE:
		{	
			CGameProcedure::s_pUIMgr->ReFocusUI();//this_ui
			CN3UIBase* pFocus = CGameProcedure::s_pUIMgr->GetFocusedUI();
			if(pFocus && pFocus != this) pFocus->OnKeyPress(iKey);
		}
		return true;
	}

	return CN3UIBase::OnKeyPress(iKey);
}

void CUIChat::AddContinueMsg(e_ChatMode eCM, const std::string& szString, D3DCOLOR color)
{
	__ChatInfo* pChatInfo = new __ChatInfo(szString, color);
	m_ContinueMsg.push_back(pChatInfo);

	AddChatMsg(eCM, szString, color);	
}

void CUIChat::DeleteContinueMsg()
{
	m_iCurContinueMsg = 0;

	ChatListItor itor;
	for(itor = m_ContinueMsg.begin(); m_ContinueMsg.end() != itor; ++itor)
	{
		__ChatInfo* pChatInfo = (*itor);
		if (pChatInfo) delete pChatInfo;
	}
	m_ContinueMsg.clear();
}

void CUIChat::ShowContinueMsg()
{
	int iSize = m_ContinueMsg.size();
	if( m_iCurContinueMsg > iSize )
		m_iCurContinueMsg = 0;

	int iCnt = 0;
	ChatListItor itor;
	for(itor = m_ContinueMsg.begin(); m_ContinueMsg.end() != itor; ++itor)
	{
		if( iCnt == m_iCurContinueMsg )
		{
			m_iCurContinueMsg++;
			__ChatInfo* pChatInfo = (*itor);
			if(pChatInfo) AddChatMsg(N3_CHAT_CONTINUE, pChatInfo->szChat, pChatInfo->color);	
		}
	}
}

void CUIChat::SetNoticeTitle(const std::string& szString, D3DCOLOR color)
{
	if(m_pNoticeTitle)
	{
		m_pNoticeTitle->SetString(szString);
		m_pNoticeTitle->SetColor(color);
	}
}

CUIChat2::CUIChat2()
{
	m_pBtn_Fold = NULL;
}

bool CUIChat2::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) 
		return false;

	m_pBtn_Fold = GetChildByID("btn_on");	
	__ASSERT(m_pBtn_Fold, "NULL UI Component!!");

	return true;
}

bool CUIChat2::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (NULL == pSender) return false;

	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == m_pBtn_Fold)
		{
			CGameProcedure::s_pProcMain->CommandToggleUIChat();
			return true;
		}
	}

	return false;
}

void CUIChat2::Release()
{
	CN3UIBase::Release();
	m_pBtn_Fold = NULL;
}
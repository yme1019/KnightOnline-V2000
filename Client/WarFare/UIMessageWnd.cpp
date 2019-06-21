#include "stdafx.h"

CUIMessageWnd::CUIMessageWnd()
{
	m_pChatOut = NULL;
	m_pScrollbar = NULL;
	m_pBtn_Fold = NULL;
	m_iChatLineCount = 0;
	m_ppUILines = NULL;
	ZeroMemory(&m_rcChatOutRegion, sizeof(m_rcChatOutRegion));
}

CUIMessageWnd::~CUIMessageWnd()
{
	if (m_ppUILines) {delete [] m_ppUILines; m_ppUILines = NULL;}

	ChatListItor itor;
	for(itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
	{
		__ChatInfo* pChatInfo = (*itor);
		if (pChatInfo) delete pChatInfo;
	}
	m_ChatBuffer.clear();

	for(itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
	{
		__ChatInfo* pChatInfo = (*itor);
		if (pChatInfo) delete pChatInfo;
	}
	m_LineBuffer.clear();
}

void CUIMessageWnd::Release()
{
	CN3UIBase::Release();

	m_pChatOut = NULL;
	m_pScrollbar = NULL;
	m_pBtn_Fold = NULL;
	m_iChatLineCount = 0;
	if (m_ppUILines) {delete [] m_ppUILines; m_ppUILines = NULL;}
	ZeroMemory(&m_rcChatOutRegion, sizeof(m_rcChatOutRegion));

	ChatListItor itor;
	for(itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
	{
		__ChatInfo* pChatInfo = (*itor);
		if (pChatInfo) delete pChatInfo;
	}
	m_ChatBuffer.clear();

	for(itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
	{
		__ChatInfo* pChatInfo = (*itor);
		if (pChatInfo) delete pChatInfo;
	}
	m_LineBuffer.clear();
}

BOOL CUIMessageWnd::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (0 == iOffsetX && 0 == iOffsetY) 
		return FALSE;

	m_rcRegion.left += iOffsetX;		
	m_rcRegion.top += iOffsetY;
	m_rcRegion.right += iOffsetX;		
	m_rcRegion.bottom += iOffsetY;

	m_rcMovable.left += iOffsetX;		
	m_rcMovable.top += iOffsetY;
	m_rcMovable.right += iOffsetX;		
	m_rcMovable.bottom += iOffsetY;

	CN3UIBase* pCUI = NULL; // Child UI...
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		pCUI = (*itor);
		__ASSERT(pCUI, "child UI pointer is NULL!");
		pCUI->MoveOffset(iOffsetX, iOffsetY);
	}

	//NOTE: (madpew) Don't stick them together, so chat and info can move freely, but sync with the folded version instead
	POINT pt = this->GetPos();
	RECT rc = this->GetRegion();
	RECT rc2 = CGameProcedure::s_pProcMain->m_pUIMsgDlg2->GetRegion();
	CGameProcedure::s_pProcMain->m_pUIMsgDlg2->SetPos(pt.x, rc.bottom + (rc2.top - rc2.bottom));
	
	return true;
}

bool CUIMessageWnd::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) 
		return false;

	m_pChatOut = (CN3UIString*)GetChildByID("text_message");	
	__ASSERT(m_pChatOut, "m_pChatOut NULL UI Component!!");

	m_pScrollbar = (CN3UIScrollBar*)GetChildByID("scroll");		
	__ASSERT(m_pScrollbar, "m_pScrollbar NULL UI Component!!");

	m_pBtn_Fold = (CN3UIBase*)GetChildByID("btn_off");	
	__ASSERT(m_pBtn_Fold, "m_pBtn_Fold NULL UI Component!!");

	m_rcChatOutRegion = m_pChatOut->GetRegion();
	CreateLines();

	__ASSERT(0<m_iChatLineCount,"채팅창이 너무 작아요");
	
	return true;
}

bool CUIMessageWnd::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if(NULL == pSender) return false;

	if (dwMsg == UIMSG_SCROLLBAR_POS)
	{
		int iCurLinePos = m_pScrollbar->GetCurrentPos();
		SetTopLine(iCurLinePos);
		return true;
	}
	else if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == m_pBtn_Fold)
		{
			CGameProcedure::s_pProcMain->CommandToggleUIMsgWnd();
			return true;
		}
	}

	return false;
}

void CUIMessageWnd::CreateLines()
{
	int i;
	if (m_ppUILines) 
	{
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

	if (m_iChatLineCount<=0 || szFontName.size() <= 0) return;

	m_ppUILines = new CN3UIString*[m_iChatLineCount];
	for (i=0; i<m_iChatLineCount; ++i)
	{
		RECT rc;
		SetRect(&rc, m_rcChatOutRegion.left, m_rcChatOutRegion.top+(i*size.cy), m_rcChatOutRegion.right, m_rcChatOutRegion.top+((i+1)*size.cy));
		m_ppUILines[i] = new CN3UIString();
		m_ppUILines[i]->Init(this);
		m_ppUILines[i]->SetRegion(rc);
		m_ppUILines[i]->SetStyle(UISTYLE_STRING_SINGLELINE|UISTYLE_STRING_ALIGNLEFT|UISTYLE_STRING_ALIGNTOP);
		m_ppUILines[i]->SetFont(szFontName, dwFontHeight, dwFontFlag & D3DFONT_BOLD, dwFontFlag & D3DFONT_ITALIC);
	}
}

void CUIMessageWnd::AddMsg(const std::string& szString, D3DCOLOR color)
{
	const int iStrLen = szString.size();
	__ASSERT(m_pScrollbar, "");
	if (0 >= iStrLen) return;

	__ChatInfo* pChatInfo = new __ChatInfo;
	pChatInfo->szChat = szString;
	pChatInfo->color = color;
	m_ChatBuffer.push_back(pChatInfo);
	
	if (m_ChatBuffer.size() > 255)	
	{
		__ChatInfo* pTemp = m_ChatBuffer.front();
		if (pTemp) delete pTemp;

		m_ChatBuffer.pop_front();
	}

	AddLineBuffer(szString, color);
	
	int iCurLinePos = m_pScrollbar->GetCurrentPos();
	BOOL bAutoScroll = (m_pScrollbar->GetMaxPos() == iCurLinePos) ? TRUE : FALSE;

	while (m_LineBuffer.size() > MAX_CHAT_LINES && 0 < iCurLinePos)
	{
		__ChatInfo* pTemp = m_LineBuffer.front();
		if (pTemp) delete pTemp;
		m_LineBuffer.pop_front();
		--iCurLinePos;	
	}

	int iLineBufferSize = m_LineBuffer.size();
	int iMaxScrollPos = iLineBufferSize-m_iChatLineCount;

	if (iMaxScrollPos<0)
		iMaxScrollPos = 0;

	m_pScrollbar->SetRange(0, iMaxScrollPos);	

	if ( bAutoScroll) 
		iCurLinePos = iMaxScrollPos;

	if (iCurLinePos < 0) 
		iCurLinePos = 0;

	m_pScrollbar->SetCurrentPos(iCurLinePos);

	SetTopLine(iCurLinePos);
}

void CUIMessageWnd::AddLineBuffer(const std::string& szString, D3DCOLOR color)
{
	if(szString.empty()) return;

	__ASSERT(m_pChatOut, "");
	const int iStrLen = szString.size();

	SIZE size;
	if (FALSE == m_pChatOut->GetTextExtent(szString, iStrLen, &size)) {__ASSERT(0,"no device context"); return;}

	const int iRegionWidth = m_rcChatOutRegion.right - m_rcChatOutRegion.left;

	int iCX=0;
	int iCount = 0;
	int iLineStart = 0;

	while(iCount<iStrLen)
	{
		if ('\n' == szString[iCount])
		{
			__ChatInfo* pLineInfo = new __ChatInfo;
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
			if (0x80 & szString[iCount])	
				iCC = 2;	
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
		m_LineBuffer.push_back(pLineInfo);

		pLineInfo->color = color;
		pLineInfo->szChat = szString.substr(iLineStart, iLineLength);
	}
}

void CUIMessageWnd::SetTopLine(int iTopLine)
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
		if (iLineBufferSize <= iCurLine) break;
		ppLineInfos[i] = m_LineBuffer[iCurLine];
	}

	__ASSERT(m_ppUILines, "null pointer");
	int iRealLine = i;
	int iRealLineCount = 0;
	for (i=0; i<iRealLine; ++i)
	{
		++iRealLineCount;
		if (NULL == m_ppUILines[i]) continue;
		m_ppUILines[i]->SetColor(ppLineInfos[i]->color);
		m_ppUILines[i]->SetString(ppLineInfos[i]->szChat);
	}
	for (i=iRealLineCount; i<m_iChatLineCount; ++i)
	{
		if (NULL == m_ppUILines[i]) continue;
		m_ppUILines[i]->SetString("");
	}
	delete [] ppLineInfos;
}

void CUIMessageWnd::RecalcLineBuffer()
{
	ChatListItor itor;
	for(itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
	{
		__ChatInfo* pLineBuff = (*itor);
		if (pLineBuff) delete pLineBuff;
	}
	m_LineBuffer.clear();

	for(itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
	{
		__ChatInfo* pChatBuff = (*itor);
		if (pChatBuff) AddLineBuffer(pChatBuff->szChat, pChatBuff->color);
	}

	while (m_LineBuffer.size() > MAX_CHAT_LINES)
	{
		__ChatInfo* pLineBuff = m_LineBuffer.front();
		if (pLineBuff) delete pLineBuff;
		m_LineBuffer.pop_front();
	}

	int iLineBufferSize = m_LineBuffer.size();
	int iMaxScrollPos = iLineBufferSize-m_iChatLineCount;
	if (iMaxScrollPos<0) iMaxScrollPos = 0;
	m_pScrollbar->SetRange(0, iMaxScrollPos);	// scroll bar range

	m_pScrollbar->SetCurrentPos(iMaxScrollPos);

	SetTopLine(iMaxScrollPos);
}

void CUIMessageWnd::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	// m_rcChatOutRegion = ;	
	//CreateLines();	
	//RecalcLineBuffer();	
}


bool CUIMessageWnd::OnKeyPress(int iKey)
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

CUIMessageWnd2::CUIMessageWnd2()
{
	m_pBtn_Fold = NULL;
}

bool CUIMessageWnd2::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile))
		return false;

	m_pBtn_Fold = GetChildByID("btn_on");			
	__ASSERT(m_pBtn_Fold, "NULL UI Component!!");

	return true;
}

bool CUIMessageWnd2::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if (NULL == pSender) 
		return false;

	if (dwMsg == UIMSG_BUTTON_CLICK)
	{
		if (pSender == m_pBtn_Fold)
		{
			CGameProcedure::s_pProcMain->CommandToggleUIMsgWnd();
			return true;
		}
	}

	return false;
}

void CUIMessageWnd2::Release()
{
	CN3UIBase::Release();
	m_pBtn_Fold = NULL;
}
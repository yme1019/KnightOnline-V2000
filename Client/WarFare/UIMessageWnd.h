#if !defined(AFX_UIMESSAGEWND_H__F825624B_A51E_4889_9ADC_BEBF022D010B__INCLUDED_)
#define AFX_UIMESSAGEWND_H__F825624B_A51E_4889_9ADC_BEBF022D010B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3UIBase.h"
#include "GameDef.h"
#include "UIChat.h"

class CUIMessageWnd : public CN3UIBase  
{
protected:
	CN3UIString*	m_pChatOut;		
	CN3UIScrollBar* m_pScrollbar;	
	CN3UIBase*		m_pBtn_Fold;

	ChatList		m_ChatBuffer;		
	ChatList		m_LineBuffer;		
	
	int				m_iChatLineCount;	
	RECT			m_rcChatOutRegion;	
	CN3UIString**	m_ppUILines;		

protected:
	void			SetTopLine(int iTopLine);		
	void			AddLineBuffer(const std::string& szString, D3DCOLOR color);
	void			RecalcLineBuffer();		
	void			CreateLines();

// Operations
public:
	bool OnKeyPress(int iKey);
	BOOL	MoveOffset(int iOffsetX, int iOffsetY);	
	bool	ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	void	Release();
	bool	Load(HANDLE hFile);
	void	SetRegion(const RECT& Rect);	

	void	AddMsg(const std::string& szString, D3DCOLOR color = 0xffffffff);		

	CUIMessageWnd();
	virtual ~CUIMessageWnd();
};

class CUIMessageWnd2 : public CN3UIBase
{
protected:
	CN3UIBase* m_pBtn_Fold;
public:
	bool	ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg);
	void	Release();
	bool	Load(HANDLE hFile);

	CUIMessageWnd2();
};
#endif
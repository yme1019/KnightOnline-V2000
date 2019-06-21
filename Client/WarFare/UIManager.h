#if !defined(AFX_UIMANAGER_H__1EED48B7_90A3_4242_99BD_ABC12040CDDE__INCLUDED_)
#define AFX_UIMANAGER_H__1EED48B7_90A3_4242_99BD_ABC12040CDDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3UIBase.h"

struct __IconItemSkill;

class CUIManager : public CN3UIBase  
{
protected:
	struct __RenderStateForUI
	{
		DWORD dwZEnable, dwAlphaBlend, dwSrcBlend, dwDestBlend, dwFog;
		DWORD dwMagFilter, dwMinFilter, dwMipFilter;
#ifdef _DEBUG
#endif
	};
	
	static __RenderStateForUI s_sRSFU;	// RenderStateForUI
	uint32_t		m_dwMouseFlagsCur;
	bool		m_bEnableOperation;		
	CN3UIBase*	m_pUIFocused;

public:
	bool		m_bDoneSomething;		

public:
	void UserMoveHideUIs();
	CN3UIBase* GetEnableFocusTopUI(bool bVisible);
	static void	RenderStateRestore();
	static void	RenderStateSet();

	void	EnableOperationSet(bool bEnable) { m_bEnableOperation = bEnable; } 
	bool	EnableOperation() { return m_bEnableOperation; } 

	void		Release();
	void		Tick();
	uint32_t		MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
	uint32_t		GetCurMouseFlags() { return m_dwMouseFlagsCur; }
	CN3UIBase*	GetFocusedUI() { return m_pUIFocused; } 
	void		SetFocusedUI(CN3UIBase* pUI); 
	CN3UIBase*	GetTopUI(bool bVisible); 
	void		SetVisibleFocusedUI(CN3UIBase* pUI);
	CN3UIBase*	ReFocusUI();
	void		Render();
	void		ReorderChildList();

	bool		BroadcastIconDropMsg(__IconItemSkill* spItem);

	CUIManager();
	virtual ~CUIManager();
};

#endif
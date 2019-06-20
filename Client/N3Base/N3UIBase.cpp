#include "N3UIBase.h"	
#include <vector>
#include "N3UIButton.h"
#include "N3UIProgress.h"
#include "N3UIImage.h"
#include "N3UIScrollBar.h"
#include "N3UIString.h"
#include "N3UITrackBar.h"
#include "N3UIStatic.h"
#include "N3UIEdit.h"
#include "N3UITooltip.h"
#include "N3UIArea.h"
#include "N3UIList.h"
#ifdef _REPENT
#include "N3UIIconSlot.h"
#endif

#include "N3SndMgr.h"
#include "N3SndObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CN3UIEdit* CN3UIBase::s_pFocusedEdit = NULL;
CN3UITooltip* CN3UIBase::s_pTooltipCtrl = NULL;
std::string CN3UIBase::s_szStringTmp;

CN3UIBase::CN3UIBase()
{
	m_eType = UI_TYPE_BASE;
	m_pParent = NULL;
	m_pChildUI	= NULL;
	m_pParentUI = NULL;

	m_iChildID	= -1;

	ZeroMemory(&m_rcRegion, sizeof(m_rcRegion));
	ZeroMemory(&m_rcMovable, sizeof(m_rcMovable));
	m_eState = UI_STATE_COMMON_NONE;
	m_dwStyle = UISTYLE_NONE;

	m_dwReserved = 0;
	m_bVisible = true;
	m_pSnd_OpenUI = NULL;
	m_pSnd_CloseUI = NULL;
}

CN3UIBase::~CN3UIBase()
{
	if(m_pParent) m_pParent->RemoveChild(this);

	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_OpenUI);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_CloseUI);

	while(!m_Children.empty())
	{
		CN3UIBase* pChild = m_Children.front();
		if (pChild) delete pChild;
	}
}

void CN3UIBase::Release()
{
	if(m_pParent) m_pParent->RemoveChild(this);

	ZeroMemory(&m_rcRegion, sizeof(m_rcRegion));
	ZeroMemory(&m_rcMovable, sizeof(m_rcMovable));
	
	m_szID = "";
	m_szToolTip = "";

	m_eState = UI_STATE_COMMON_NONE;
	m_dwStyle = UISTYLE_NONE;
	m_dwReserved = 0;
	m_bVisible = true;
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_OpenUI);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_CloseUI);

	while(!m_Children.empty())
	{
		CN3UIBase* pChild = m_Children.front();
		if (pChild) delete pChild;
	}

	CN3BaseFileAccess::Release();
}

void CN3UIBase::Init(CN3UIBase* pParent)
{
	Release();
	SetParent(pParent);
}

void CN3UIBase::RemoveChild(CN3UIBase* pChild)
{
	if(NULL == pChild) return;

	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor;)
	{
		if((*itor) == pChild)
		{
			itor = m_Children.erase(itor);
			break;
		}
		else
		{
			++itor;
		}
	}
}

void CN3UIBase::SetParent(CN3UIBase* pParent)
{
	if(m_pParent) m_pParent->RemoveChild(this);
	m_pParent = pParent;
	if (m_pParent) m_pParent->AddChild(this);

	if(pParent) m_iFileFormatVersion = pParent->m_iFileFormatVersion;
}

POINT CN3UIBase::GetPos() const
{
	POINT p;
	p.x = m_rcRegion.left;
	p.y = m_rcRegion.top;
	return p;
}

void CN3UIBase::SetPos(int x, int y)
{
	int dx, dy;
	dx = x - m_rcRegion.left;
	dy = y - m_rcRegion.top;

	MoveOffset(dx, dy);
}

void CN3UIBase::SetPosCenter()
{
	POINT pt = this->GetPos();
	RECT rc = this->GetRegion();
	int iW = rc.right - rc.left;
	int iH = rc.bottom - rc.top;
	int iWVP = CN3Base::s_CameraData.vp.Width;
	int iHVP = CN3Base::s_CameraData.vp.Height;

	MoveOffset(((iWVP - iW) / 2) - pt.x, ((iHVP - iH) / 2) - pt.y);
}

BOOL CN3UIBase::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (0 == iOffsetX && 0 == iOffsetY) return FALSE;

	m_rcRegion.left += iOffsetX;		m_rcRegion.top += iOffsetY;
	m_rcRegion.right += iOffsetX;		m_rcRegion.bottom += iOffsetY;

	if(	m_rcMovable.right - m_rcMovable.left != 0 && m_rcMovable.bottom - m_rcMovable.top != 0 )
	{
		m_rcMovable.left += iOffsetX;		m_rcMovable.top += iOffsetY;
		m_rcMovable.right += iOffsetX;		m_rcMovable.bottom += iOffsetY;
	}

	CN3UIBase* pCUI = NULL; // Child UI...
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		pCUI = (*itor);
		__ASSERT(pCUI, "child UI pointer is NULL!");
		pCUI->MoveOffset(iOffsetX, iOffsetY);
	}
	return TRUE;
}

bool CN3UIBase::IsIn(int x, int y)
{
	if(x<m_rcRegion.left || x>m_rcRegion.right || y<m_rcRegion.top || y>m_rcRegion.bottom) return false;
	return true;
}

void CN3UIBase::SetSize(int iWidth, int iHeight)
{
	RECT rc;
	SetRect(&rc, m_rcRegion.left, m_rcRegion.top, m_rcRegion.left + iWidth, m_rcRegion.top + iHeight);
	SetRegion(rc);
}

bool CN3UIBase::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if(m_pParent && pSender) m_pParent->ReceiveMessage(pSender, dwMsg);
	return true;
}

void CN3UIBase::CallBackProc(int iID, uint32_t dwFlag)
{
}

void CN3UIBase::ShowWindow(int iID, CN3UIBase* pParent)
{
	if(pParent)
		pParent->m_pChildUI = this;

	m_pParentUI = pParent;
	m_iChildID = iID;

	SetVisible(true);
}

bool CN3UIBase::Load(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);
	DWORD dwRWC = NULL;

	// children
	int iCC = 0;
	if(m_iFileFormatVersion >= N3FORMAT_VER_1264) 
	{
		//char temp[0xFF];

		int16_t sCC, sIdk0;
		ReadFile(hFile, &sCC, sizeof(int16_t), &dwRWC, NULL); // children count
		ReadFile(hFile, &sIdk0, sizeof(int16_t), &dwRWC, NULL);
		iCC = (int) sCC;

		//sprintf(temp, "sIdk0 = %d\n", sIdk0);
		//OutputDebugString(temp);
	} 
	else 
	{
		ReadFile(hFile, &iCC, sizeof(iCC), &dwRWC, NULL); // children count
	}

	eUI_TYPE eChildUIType;
	for(int i = 0; i < iCC; i++)
	{
		CN3UIBase* pChild = NULL;
		ReadFile(hFile, &eChildUIType, sizeof(eChildUIType), &dwRWC, NULL);

		switch(eChildUIType)
		{
		case UI_TYPE_BASE:			pChild = new CN3UIBase();			break;
		case UI_TYPE_IMAGE:			pChild = new CN3UIImage();			break;
		case UI_TYPE_STRING:		pChild = new CN3UIString();			break;
		case UI_TYPE_BUTTON:		pChild = new CN3UIButton();			break;
		case UI_TYPE_STATIC:		pChild = new CN3UIStatic();			break;
		case UI_TYPE_PROGRESS:		pChild = new CN3UIProgress();		break;
		case UI_TYPE_SCROLLBAR:		pChild = new CN3UIScrollBar();		break;
		case UI_TYPE_TRACKBAR:		pChild = new CN3UITrackBar();		break;
		case UI_TYPE_EDIT:			pChild = new CN3UIEdit();			break;
		case UI_TYPE_AREA:			pChild = new CN3UIArea();			break;
#ifdef _REPENT
		case UI_TYPE_ICONSLOT:		pChild = new CN3UIIconSlot();		break;
#endif
		case UI_TYPE_LIST:			pChild = new CN3UIList();			break;
		}
		__ASSERT(pChild, "Unknown type UserInterface!!!");
		pChild->Init(this);
		pChild->Load(hFile);
	}

	// base ����
	int iIDLen = 0;
	ReadFile(hFile, &iIDLen, sizeof(iIDLen), &dwRWC, NULL);				// ui id length
	if (iIDLen>0)
	{
		std::vector<char> buffer(iIDLen+1, NULL);
		ReadFile(hFile, &buffer[0], iIDLen, &dwRWC, NULL);			// ui id
		m_szID = &buffer[0];
	}
	else
	{
		m_szID = "";
	}
	ReadFile(hFile, &m_rcRegion, sizeof(m_rcRegion), &dwRWC, NULL);		// m_rcRegion
	ReadFile(hFile, &m_rcMovable, sizeof(m_rcMovable), &dwRWC, NULL);	// m_rcMovable
	ReadFile(hFile, &m_dwStyle, sizeof(m_dwStyle), &dwRWC, NULL);		// style
	ReadFile(hFile, &m_dwReserved, sizeof(m_dwReserved), &dwRWC, NULL);	//	m_dwReserved

	int iTooltipLen;
	ReadFile(hFile, &iTooltipLen, sizeof(iTooltipLen), &dwRWC, NULL);		
	if (iTooltipLen>0)
	{
		std::vector<char> buffer(iTooltipLen+1, NULL);
		ReadFile(hFile, &buffer[0], iTooltipLen, &dwRWC, NULL);
		m_szToolTip = &buffer[0];
	}

	int iSndFNLen = 0;
	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwRWC, NULL);		
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, &buffer[0], iSndFNLen, &dwRWC, NULL);

		__ASSERT(NULL == m_pSnd_OpenUI, "memory leak");
		m_pSnd_OpenUI = s_SndMgr.CreateObj(&buffer[0], SNDTYPE_2D);
	}

	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwRWC, NULL);
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, &buffer[0], iSndFNLen, &dwRWC, NULL);

		__ASSERT(NULL == m_pSnd_CloseUI, "memory leak");
		m_pSnd_CloseUI = s_SndMgr.CreateObj(&buffer[0], SNDTYPE_2D);
	}
	return true;
}


void CN3UIBase::Tick()
{
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->Tick();
	}
}

void CN3UIBase::Render()
{
	if (!m_bVisible) 
		return;

	for(UIListReverseItor itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->Render();
		CN3UIBase* pCUI = NULL;
		pCUI = pChild->m_pChildUI;
		while(pCUI)
		{
			pCUI->Render();
			pCUI = pCUI->m_pChildUI;
		}
	}
}

uint32_t CN3UIBase::MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld )
{
	uint32_t dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	if (UI_STATE_COMMON_MOVE == m_eState)
	{
		if (dwFlags&UI_MOUSE_LBCLICKED)
		{
			SetState(UI_STATE_COMMON_NONE);
		}
		else
		{
			MoveOffset(ptCur.x - ptOld.x, ptCur.y - ptOld.y);
		}
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if(false == IsIn(ptCur.x, ptCur.y))
	{
		if(false == IsIn(ptOld.x, ptOld.y))
		{
			return dwRet;
		}
		dwRet |= UI_MOUSEPROC_PREVINREGION;
	}
	else
	{
		if (s_pTooltipCtrl) s_pTooltipCtrl->SetText(m_szToolTip);
	}
	dwRet |= UI_MOUSEPROC_INREGION;

	if(m_pChildUI && m_pChildUI->IsVisible())
		return dwRet;

	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		uint32_t dwChildRet = pChild->MouseProc(dwFlags, ptCur, ptOld);
		if (UI_MOUSEPROC_DONESOMETHING & dwChildRet)
		{	
			//m_Children.erase(itor);			
			//m_Children.push_front(pChild);
			dwRet |= (UI_MOUSEPROC_CHILDDONESOMETHING|UI_MOUSEPROC_DONESOMETHING);
			return dwRet;
		}
	}

	if (UI_STATE_COMMON_MOVE != m_eState && PtInRect(&m_rcMovable, ptCur) && (dwFlags&UI_MOUSE_LBCLICK) )
	{
		SetState(UI_STATE_COMMON_MOVE);
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}
	return dwRet;
}

bool CN3UIBase::EnableTooltip(const std::string& szFN)
{
	delete s_pTooltipCtrl;
	s_pTooltipCtrl = NULL;
	if (szFN.empty()) return false;
	
	s_pTooltipCtrl = new CN3UITooltip();
	s_pTooltipCtrl->Init(NULL);
	s_pTooltipCtrl->LoadFromFile(szFN);
	return true;
}

void CN3UIBase::DestroyTooltip()
{
	if (s_pTooltipCtrl) {delete s_pTooltipCtrl; s_pTooltipCtrl = NULL;}
}

void CN3UIBase::PrintChildIDs(void) 
{
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if(pChild->m_szID == "") continue;
		printf("%s\n", pChild->m_szID.c_str());
	}
}

CN3UIBase* CN3UIBase::GetChildByID(const std::string& szID)
{
	if(szID.empty()) return NULL;

	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		//if(pChild->m_szID == szID) return pChild;
		if(lstrcmpi(pChild->m_szID.c_str(), szID.c_str()) == 0) return pChild;
	}
	return NULL;
}

void CN3UIBase::SetVisible(bool bVisible)
{
	if (bVisible != m_bVisible)
	{
		if (bVisible)
		{ 
			if(m_pSnd_OpenUI) m_pSnd_OpenUI->Play(); 
		}
		else 
		{ 
			if(m_pSnd_CloseUI) m_pSnd_CloseUI->Play(); 
		}	
	}
	m_bVisible = bVisible;
	if(!m_bVisible)
	{
		if(m_pChildUI)
		{
			m_pChildUI->SetVisible(false);
		}
		m_pChildUI	= NULL;
		if(m_pParentUI)
		{
			if(m_pParentUI->m_pChildUI == this)
				m_pParentUI->m_pChildUI = NULL;
		}
		m_pParentUI = NULL;
		m_iChildID	= -1;
	}
}

void CN3UIBase::SetVisibleWithNoSound(bool bVisible, bool bWork, bool bReFocus)
{ 
	m_bVisible = bVisible; 
	if(!m_bVisible)
	{
		if(m_pChildUI)
		{
			m_pChildUI->SetVisible(false);
		}
		m_pChildUI	= NULL;
		if(m_pParentUI)
		{
			if(m_pParentUI->m_pChildUI == this)
				m_pParentUI->m_pChildUI = NULL;
		}
		m_pParentUI = NULL;
		m_iChildID	= -1;
	}
}

#ifndef _N3TOOL

void CN3UIBase::operator = (const CN3UIBase& other)
{
	Init(NULL);	

	UIListItorConst it = other.m_Children.begin();
	UIListItorConst itEnd = other.m_Children.end();
	CN3UIBase* pOtherChild = NULL;
	CN3UIBase* pChild = NULL;
	for(; it != itEnd; it++)
	{
		pOtherChild = *it;

		if(NULL == pOtherChild) continue;

		pChild = NULL;
		switch(pOtherChild->UIType())
		{
		case UI_TYPE_BASE:
			{ 
				pChild = new CN3UIBase();
				*pChild = *pOtherChild;
			}
			break;
		case UI_TYPE_BUTTON:
			{
				CN3UIButton *pUINew = new CN3UIButton();
				*pUINew = *((CN3UIButton*)pOtherChild);
				pChild = pUINew;
			}
			break;	// button
		case UI_TYPE_STATIC:	
			{ 
				CN3UIStatic* pUINew = new CN3UIStatic();		
				*pUINew = *((CN3UIStatic*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// static 
		case UI_TYPE_PROGRESS:	
			{ 
				CN3UIProgress* pUINew = new CN3UIProgress();	
				*pUINew = *((CN3UIProgress*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// progress
		case UI_TYPE_IMAGE:		
			{ 
				CN3UIImage* pUINew = new CN3UIImage();		
				*pUINew = *((CN3UIImage*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// image
		case UI_TYPE_SCROLLBAR:	
			{ 
				CN3UIScrollBar* pUINew = new CN3UIScrollBar();	
				*pUINew = *((CN3UIScrollBar*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// scroll bar
		case UI_TYPE_STRING:	
			{ 
				CN3UIString* pUINew = new CN3UIString();		
				*pUINew = *((CN3UIString*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// string
		case UI_TYPE_TRACKBAR:	
			{ 
				CN3UITrackBar* pUINew = new CN3UITrackBar();	
				*pUINew = *((CN3UITrackBar*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// track bar
		case UI_TYPE_EDIT:		
			{ 
				CN3UIEdit* pUINew = new CN3UIEdit();		
				*pUINew = *((CN3UIEdit*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// edit
		case UI_TYPE_AREA:		
			{ 
				CN3UIArea* pUINew = new CN3UIArea();		
				*pUINew = *((CN3UIArea*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// area
		case UI_TYPE_TOOLTIP:
			{ 
				CN3UITooltip* pUINew = new CN3UITooltip();
				*pUINew = *((CN3UITooltip*)pOtherChild);
				pChild = pUINew;
			} 
			break;	// tooltip
		case UI_TYPE_LIST:
			{ 
				CN3UIList* pUINew = new CN3UIList();
				*pUINew = *((CN3UIList*)pOtherChild);
				pChild = pUINew;
			} 
			break;	// tooltip
		/*case UI_TYPE_ICON:		pUIDest = new CN3UIIcon();		*pUIDest = *((CN3UIBase*)pUISrc); 
			break;	// icon
		case UI_TYPE_ICON_MANAGER:	pUIDest = new CN3UIIconManager();	*pUIDest = *((CN3UIBase*)pUISrc);
			break;	// icon manager.. */

#ifdef _REPENT
		case UI_TYPE_ICONSLOT:
			{
				CN3UIIconSlot* pUINew = new CN3UIIconSlot();
				*pUINew = *((CN3UIIconSlot*)pOtherChild);
				pChild = pUINew;
			}
			break;	// icon slot
#endif
		}
		if(pChild) pChild->SetParent(this);
	}

	m_bVisible = other.m_bVisible;
	m_dwReserved = other.m_dwReserved;
	m_dwStyle = other.m_dwStyle;
	m_eState = other.m_eState;
	m_eType = other.m_eType;

	if(other.m_pSnd_OpenUI)
	{
		CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_OpenUI);
		m_pSnd_OpenUI = s_SndMgr.CreateObj(other.m_pSnd_OpenUI->m_szFileName, SNDTYPE_2D);
	}

	if(other.m_pSnd_CloseUI)
	{
		CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_CloseUI);
		m_pSnd_CloseUI = s_SndMgr.CreateObj(other.m_pSnd_CloseUI->m_szFileName, SNDTYPE_2D);
	}

	m_rcMovable = other.m_rcMovable;
	m_rcRegion = other.m_rcRegion;
	m_szID = other.m_szID;
	m_szToolTip = other.m_szToolTip;
}
#endif

#ifdef _N3TOOL
bool CN3UIBase::Save(HANDLE hFile)
{
	CN3BaseFileAccess::Save(hFile);
	DWORD dwRWC = NULL;

	int iCC = m_Children.size();
	WriteFile(hFile, &iCC, sizeof(iCC), &dwRWC, NULL);
	for(UIListReverseItor itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		eUI_TYPE eUIType = pChild->UIType();

		WriteFile(hFile, &eUIType, sizeof(eUIType), &dwRWC, NULL); 
		pChild->Save(hFile);
	}

	// base ����
	int iIDLen = 0;
	iIDLen = m_szID.size();
	WriteFile(hFile, &iIDLen, sizeof(iIDLen), &dwRWC, NULL);				// id length
	if (iIDLen>0) WriteFile(hFile, m_szID.c_str(), iIDLen, &dwRWC, NULL);			// ui id
	WriteFile(hFile, &m_rcRegion, sizeof(m_rcRegion), &dwRWC, NULL);		// m_rcRegion
	WriteFile(hFile, &m_rcMovable, sizeof(m_rcMovable), &dwRWC, NULL);		// m_rcMovable
	WriteFile(hFile, &m_dwStyle, sizeof(m_dwStyle), &dwRWC, NULL);			// style
	WriteFile(hFile, &m_dwReserved, sizeof(m_dwReserved), &dwRWC, NULL);	//	m_dwReserved

	int iTooltipLen = m_szToolTip.size();
	WriteFile(hFile, &iTooltipLen, sizeof(iTooltipLen), &dwRWC, NULL);		
	if (iTooltipLen>0) WriteFile(hFile, m_szToolTip.c_str(), iTooltipLen, &dwRWC, NULL);

	int iSndFNLen = 0;
	if (m_pSnd_OpenUI) iSndFNLen = m_pSnd_OpenUI->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwRWC, NULL);		
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_OpenUI->m_szFileName.c_str(), iSndFNLen, &dwRWC, NULL);

	iSndFNLen = 0;
	if (m_pSnd_CloseUI) iSndFNLen = m_pSnd_CloseUI->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwRWC, NULL);		
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_CloseUI->m_szFileName.c_str(), iSndFNLen, &dwRWC, NULL);
	
	return true;
}

void CN3UIBase::ChangeImagePath(const std::string& szPathOld, const std::string& szPathNew)
{
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->ChangeImagePath(szPathOld, szPathNew);
	}
}

void CN3UIBase::ChangeFont(const std::string& szFont)
{
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->ChangeFont(szFont);
	}
}

void CN3UIBase::GatherImageFileName(std::set<std::string>& setImgFile)
{
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->GatherImageFileName(setImgFile);
	}
}


void CN3UIBase::ResizeAutomaticalyByChild()
{
	if(m_Children.empty()) return;

	RECT rcMax = { 100000000, 100000000, -100000000, -100000000 };
	int iIndex = 0;
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; itor++, iIndex++)
	{
		CN3UIBase* pChild = (*itor);
		RECT rcTmp = pChild->GetRegion();
		if(rcTmp.left < rcMax.left) rcMax.left = rcTmp.left;
		if(rcTmp.top < rcMax.top) rcMax.top = rcTmp.top;
		if(rcTmp.right > rcMax.right) rcMax.right = rcTmp.right;
		if(rcTmp.bottom > rcMax.bottom) rcMax.bottom = rcTmp.bottom;
	}

	RECT rcCur = this->GetRegion();
	if(rcCur.left > rcMax.left) rcCur.left = rcMax.left;
	if(rcCur.top > rcMax.top) rcCur.top = rcMax.top;
	if(rcCur.right < rcMax.right) rcCur.right = rcMax.right;
	if(rcCur.bottom < rcMax.bottom) rcCur.bottom = rcMax.bottom;
	//this->SetRegion(rcCur);
	m_rcRegion = rcCur;
}

int CN3UIBase::IsMyChild(CN3UIBase* pUI)
{
	int iIndex = 0;
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; itor++, iIndex++)
	{
		CN3UIBase* pChild = (*itor);
		if(pChild == pUI) return iIndex;
	}

	return -1;
}

bool CN3UIBase::SwapChild(CN3UIBase* pChild1, CN3UIBase* pChild2)
{
	if(this->IsMyChild(pChild1) < 0 || IsMyChild(pChild2) < 0) return false;
	
	UIListItor itor1;
	for(itor1 = m_Children.begin(); m_Children.end() != itor1; itor1++)
		if(*itor1 == pChild1) break;
	if(itor1 == m_Children.end()) return false;

	UIListItor itor2;
	for(itor2 = m_Children.begin(); m_Children.end() != itor2; itor2++)
		if(*itor2 == pChild2) break;
	if(itor2 == m_Children.end()) return false;

	swap(*itor1, *itor2);

	return true;
}

bool CN3UIBase::MoveToHighest(CN3UIBase* pChild)
{
	if(this->IsMyChild(pChild) < 0) return false;
	
	for(UIListItor itor1 = m_Children.begin(); m_Children.end() != itor1; itor1++)
	{
		if(*itor1 == pChild) 
		{
			m_Children.erase(itor1);
			m_Children.push_front(pChild);
			return true;
		}
	}

	return false;
}

bool CN3UIBase::MoveToLowest(CN3UIBase* pChild)
{
	if(this->IsMyChild(pChild) < 0) return false;
	
	for(UIListItor itor1 = m_Children.begin(); m_Children.end() != itor1; itor1++)
	{
		if(*itor1 == pChild) 
		{
			m_Children.erase(itor1);
			m_Children.push_back(pChild);
			return true;
		}
	}

	return false;
}

bool CN3UIBase::MoveToLower(CN3UIBase* pChild)
{
	if(this->IsMyChild(pChild) < 0) return false;
	
	for(UIListItor itor1 = m_Children.begin(); itor1 != m_Children.end(); itor1++)
	{
		if(*itor1 == pChild)
		{
			UIListItor itNext = itor1; itNext++;
			if(itNext != m_Children.end())
			{
				swap(*itNext, *itor1);
				return true;
			}
			break;
		}
	}

	return false;
}

bool CN3UIBase::MoveToUpper(CN3UIBase* pChild)
{
	if(this->IsMyChild(pChild) < 0) return false;
	
	for(UIListItor itor1 = m_Children.begin(); itor1 != m_Children.end(); itor1++)
	{
		if(*itor1 == pChild) 
		{
			if(itor1 != m_Children.begin())
			{
				UIListItor itPrev = itor1; itPrev--;
				swap(*itPrev, *itor1);
				return true;
			}
			break;
		}
	}

	return false;
}

void CN3UIBase::ArrangeZOrder()
{
	UIList tempList;
	UIListItor itor;
	for(itor = m_Children.begin(); m_Children.end() != itor;)
	{
		CN3UIBase* pChild = (*itor);
		if(UI_TYPE_IMAGE == pChild->UIType())
		{
			itor = m_Children.erase(itor);
			tempList.push_back(pChild);		
		}
		else ++itor;
	}

	for(itor = tempList.begin(); tempList.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		m_Children.push_back(pChild);		
	}
	tempList.clear();
}

void CN3UIBase::operator = (const CN3UIBase& other)
{
	Init(NULL);	

	UIListItorConst it = other.m_Children.begin();
	UIListItorConst itEnd = other.m_Children.end();
	CN3UIBase* pOtherChild = NULL;
	CN3UIBase* pChild = NULL;
	for(; it != itEnd; it++)
	{
		pOtherChild = *it;

		if(NULL == pOtherChild) continue;

		pChild = NULL;
		switch(pOtherChild->UIType())
		{
		case UI_TYPE_BASE:
			{ 
				pChild = new CN3UIBase();
				*pChild = *pOtherChild;
			}
			break;
		case UI_TYPE_BUTTON:
			{
				CN3UIButton *pUINew = new CN3UIButton();
				*pUINew = *((CN3UIButton*)pOtherChild);
				pChild = pUINew;
			}
			break;	// button
		case UI_TYPE_STATIC:	
			{ 
				CN3UIStatic* pUINew = new CN3UIStatic();		
				*pUINew = *((CN3UIStatic*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// static
		case UI_TYPE_PROGRESS:	
			{ 
				CN3UIProgress* pUINew = new CN3UIProgress();	
				*pUINew = *((CN3UIProgress*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// progress
		case UI_TYPE_IMAGE:		
			{ 
				CN3UIImage* pUINew = new CN3UIImage();		
				*pUINew = *((CN3UIImage*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// image
		case UI_TYPE_SCROLLBAR:	
			{ 
				CN3UIScrollBar* pUINew = new CN3UIScrollBar();	
				*pUINew = *((CN3UIScrollBar*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// scroll bar
		case UI_TYPE_STRING:	
			{ 
				CN3UIString* pUINew = new CN3UIString();		
				*pUINew = *((CN3UIString*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// string
		case UI_TYPE_TRACKBAR:	
			{ 
				CN3UITrackBar* pUINew = new CN3UITrackBar();	
				*pUINew = *((CN3UITrackBar*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// track bar
		case UI_TYPE_EDIT:		
			{ 
				CN3UIEdit* pUINew = new CN3UIEdit();		
				*pUINew = *((CN3UIEdit*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// edit
		case UI_TYPE_AREA:		
			{ 
				CN3UIArea* pUINew = new CN3UIArea();		
				*pUINew = *((CN3UIArea*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// area
		case UI_TYPE_TOOLTIP:
			{ 
				CN3UITooltip* pUINew = new CN3UITooltip();
				*pUINew = *((CN3UITooltip*)pOtherChild);
				pChild = pUINew;
			} 
			break;	// tooltip
		case UI_TYPE_LIST:
			{ 
				CN3UIList* pUINew = new CN3UIList();
				*pUINew = *((CN3UIList*)pOtherChild);
				pChild = pUINew;
			} 
			break;	// tooltip
		/*case UI_TYPE_ICON:		pUIDest = new CN3UIIcon();		*pUIDest = *((CN3UIBase*)pUISrc); 
			break;	// icon
		case UI_TYPE_ICON_MANAGER:	pUIDest = new CN3UIIconManager();	*pUIDest = *((CN3UIBase*)pUISrc); 
			break;	// icon manager.. */
#ifdef _REPENT
		case UI_TYPE_ICONSLOT:
			{
				CN3UIIconSlot* pUINew = new CN3UIIconSlot();
				*pUINew = *((CN3UIIconSlot*)pOtherChild);
				pChild = pUINew;
			}
			break;	// icon slot
#endif
		}
		if(pChild) pChild->SetParent(this);
	}

	m_bVisible = other.m_bVisible;
	m_dwReserved = other.m_dwReserved;
	m_dwStyle = other.m_dwStyle;
	m_eState = other.m_eState;
	m_eType = other.m_eType;

	SetSndOpen(other.GetSndFName_OpenUI());
	SetSndClose(other.GetSndFName_CloseUI());

	m_rcMovable = other.m_rcMovable;
	m_rcRegion = other.m_rcRegion;
	m_szID = other.m_szID;
	m_szToolTip = other.m_szToolTip;
}

void CN3UIBase::SetSndOpen(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_OpenUI);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_OpenUI = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

void CN3UIBase::SetSndClose(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_CloseUI);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_CloseUI = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

std::string CN3UIBase::GetSndFName_OpenUI() const
{
	if (m_pSnd_OpenUI) return m_pSnd_OpenUI->m_szFileName;
	else return std::string("");
}

std::string CN3UIBase::GetSndFName_CloseUI() const
{
	if (m_pSnd_CloseUI) return m_pSnd_CloseUI->m_szFileName;
	else return std::string("");
}

bool CN3UIBase::ReplaceAllTextures(const std::string& strFind, const std::string& strReplace)
{
	if (strFind.size() <= 0 || strReplace.size() <= 0) return false;
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		if (false == (*itor)->ReplaceAllTextures(strFind, strReplace)) return false;
	}
	return true;
}

#endif

#if !defined(AFX_N3SNDMGR_H__9CB531B0_4FEB_4360_8141_D0BF61347BD7__INCLUDED_)
#define AFX_N3SNDMGR_H__9CB531B0_4FEB_4360_8141_D0BF61347BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#pragma warning(disable : 4786)

#include "N3SndDef.h"
#include "N3TableBase.h"

#include <list>
#include <map>

class CN3SndObj;
class CN3SndObjStream;

typedef std::list<CN3SndObj*>::iterator itl_Snd;
typedef std::list<CN3SndObjStream*>::iterator itl_SndStream;
typedef std::map<std::string, CN3SndObj*>::iterator itm_Snd;
typedef std::map<std::string, CN3SndObj*>::value_type val_Snd;

class CN3SndMgr
{
protected:
	CN3TableBase<__TABLE_SOUND>			m_Tbl_Source;

	bool								m_bSndEnable;
	bool								m_bSndDuplicated;
	std::map<std::string, CN3SndObj*>	m_SndObjSrcs;
	std::list<CN3SndObjStream*>			m_SndObjStreams;	
	std::list<CN3SndObj*>				m_SndObjs_Duplicated;
	std::list<CN3SndObj*>				m_SndObjs_PlayOnceAndRelease;
	
public:
	void		ReleaseObj(CN3SndObj** ppObj);
	void		ReleaseStreamObj(CN3SndObjStream** ppObj);
	bool		PlayOnceAndRelease(int iSndID, const D3DVECTOR* pPos = NULL);
		
	void		SetDuplicated(bool bDuplicate) { m_bSndDuplicated = bDuplicate; }
	bool		GetDuplicated() { return m_bSndDuplicated; }
	void		SetEnable(bool bEnable) { m_bSndEnable = bEnable; }
	void		Init(HWND hWnd);
	void		Release();
	void		Tick();	

	CN3SndObj*			CreateObj(const std::string& szFN, e_SndType eType = SNDTYPE_3D);
	CN3SndObj*			CreateObj(int iID, e_SndType eType = SNDTYPE_3D);
	CN3SndObjStream*	CreateStreamObj(const std::string& szFN);
	CN3SndObjStream*	CreateStreamObj(int iID);

	CN3SndMgr();
	virtual ~CN3SndMgr();
};

#endif
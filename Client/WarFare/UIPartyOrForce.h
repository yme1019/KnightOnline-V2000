#if !defined(AFX_UIPartyOrForce_H__7B2732B7_C9CA_46A3_89BC_C59934ED3F13__INCLUDED_)
#define AFX_UIPartyOrForce_H__7B2732B7_C9CA_46A3_89BC_C59934ED3F13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "GameDef.h"
#include "N3UIBase.h"
#include <list>

typedef std::list<__InfoPartyOrForce>::iterator it_PartyOrForce;

class CUIPartyOrForce : public CN3UIBase
{
protected:
	class CN3UIProgress*	m_pProgress_HPs[MAX_PARTY_OR_FORCE];		//  HP Gauge
	class CN3UIProgress*	m_pProgress_HPReduce[MAX_PARTY_OR_FORCE];	//  HP Reduce
	class CN3UIProgress*	m_pProgress_ETC[MAX_PARTY_OR_FORCE];		
	class CN3UIStatic*		m_pStatic_IDs[MAX_PARTY_OR_FORCE];		
	class CN3UIArea*		m_pAreas[MAX_PARTY_OR_FORCE];		

	std::list<__InfoPartyOrForce>	m_Members; 
	size_t		m_iIndexSelected;

public:
	int			m_iPartyOrForce; 

public:
	bool OnKeyPress(int iKey);
	void Tick();
	void		MemberClassChange(int iID, e_Class eClass);
	void		MemberLevelChange(int iID, int iLevel);
	void		MemberHPChange(int iID, int iHP, int iHPMax);
	void		MemberStatusChange(int iID, e_PartyStatus ePS, bool bSuffer);

	void		MemberInfoReInit(); 
	bool		TargetByIndex(size_t iIndex); 

	const __InfoPartyOrForce*	MemberInfoGetByID(int iID, int& iIndexResult);
	const __InfoPartyOrForce*	MemberInfoGetByIndex(size_t iIndex);
	const __InfoPartyOrForce*	MemberInfoGetSelected(); // ÇöÀç ¼±ÅÃµÈ ¸â¹öÀÎµ¦½º..
	const __InfoPartyOrForce*	MemberAdd(int iID, const std::string szID, int iLevel, e_Class eClass, int iHP, int iHPMax);
	class CPlayerOther*			MemberGetByNearst(const __Vector3& vPosPlayer);
	bool						MemberRemove(int iID);
	void						MemberDestroy();
	size_t						MemberCount() { return m_Members.size(); }
	void						MemberSelect(size_t iMemberIndex) { if (iMemberIndex > m_Members.size()) return; m_iIndexSelected = iMemberIndex; }

	bool Load(HANDLE hFile);
	bool ReceiveMessage(class CN3UIBase* pSender, uint32_t dwMsg);
	void Render();
	
	void Release();
	CUIPartyOrForce();
	virtual ~CUIPartyOrForce();
};

#endif 
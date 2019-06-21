#if !defined(AFX_CATAPULT_H__9D404701_4B6D_4DFE_9BDD_7125FA9B3CE8__INCLUDED_)
#define AFX_CATAPULT_H__9D404701_4B6D_4DFE_9BDD_7125FA9B3CE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "MachineBase.h"

class CCatapult : public CMachineBase
{
public:
	CCatapult();
	virtual ~CCatapult();

// Structure
public:
	struct __Thrower
	{
		CN3SPart*	pThrowerPart;			
		float	fLimitRadian;			
		float	fRadianAccel;			
		float	fRadianSpeed2Reload;	
		float	fReloadDelayTime;		
		float	fRecoilRadian;			
		class CN3Shape*	pStone;			
		__Vector3		vStoneOffset;	

		float	fReleaseTime;

		float	fTime;					
		BOOL	bFire;					
		float	fCurRadian;				
		BOOL	bDontRenderStone;		
	};
// Attributes
public:
protected:
	__Thrower		m_Thrower;			// 발사대

// Operations
public:
	virtual void	Release();
	virtual void	Tick(float fFrm);
	virtual void	LoadMachine(FILE* stream);
	virtual void	Render();
protected:
	virtual void	ReCalcMatrix4AnimatedPart();
	void ThrowStone(__Vector3& vPos, __Vector3& vVel, CN3Shape* pShape) const;

public:
	virtual void	Fire();		// 발사
};

#endif

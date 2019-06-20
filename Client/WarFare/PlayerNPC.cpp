#include "PlayerNPC.h"
#include "GameProcMain.h"
#include "N3WorldManager.h"
#include "N3Shape.h"
#include "N3ShapeMgr.h"
#include "N3SndObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif


CPlayerNPC::CPlayerNPC()
{
	m_ePlayerType = PLAYER_NPC;
	m_vPosFromServer = m_Chr.Pos(); 
}

CPlayerNPC::~CPlayerNPC()
{
}

void CPlayerNPC::Tick()
{
	if(m_pShapeExtraRef)
	{
		CPlayerBase::Tick();
		return;
	}

	if(m_fTimeAfterDeath > 0)
	{
		if(m_fTimeAfterDeath > 3.0f)
			this->Action(PSA_DYING, false);
		CPlayerBase::Tick();
		return;
	}

	__Vector3 vPos = m_Chr.Pos();
	if(	m_vPosFromServer.x != vPos.x || m_vPosFromServer.z != vPos.z )
	{
		if(m_fMoveSpeedPerSec == 0)
		{
			__Vector3 vT1(m_vPosFromServer.x, 0, m_vPosFromServer.z), vT2(vPos.x, 0, vPos.z);
			m_fMoveSpeedPerSec = (vT2 - vT1).Magnitude();
		}

		__Vector3 vOffset = m_vPosFromServer - vPos; vOffset.y = 0;
		__Vector3 vDir = vOffset; vDir.Normalize();  

		float fSpeedAbsolute = (m_fMoveSpeedPerSec > 0) ? m_fMoveSpeedPerSec : -m_fMoveSpeedPerSec; 
		float fDist = vOffset.Magnitude();
		if(fDist < fSpeedAbsolute * CN3Base::s_fSecPerFrm) 
		{
			vPos.x = m_vPosFromServer.x;
			vPos.z = m_vPosFromServer.z; 
//			m_fMoveSpeedPerSec = 0;
			this->ActionMove(PSM_STOP);
		}
		else 
		{
			float fYaw = (m_fMoveSpeedPerSec < 0) ? ::_Yaw2D(-vDir.x, -vDir.z) : ::_Yaw2D(vDir.x, vDir.z);
			this->RotateTo(fYaw, false);

			e_StateMove eMove = PSM_STOP;
			float fStandWalk = ((PLAYER_OTHER == m_ePlayerType) ? (MOVE_SPEED_WHEN_WALK * 2.0f) : (MOVE_SPEED_WHEN_WALK * m_Chr.Radius() * 2.0f));
			if(m_fMoveSpeedPerSec < 0) eMove = PSM_WALK_BACKWARD;
			else if(m_fMoveSpeedPerSec < fStandWalk) eMove = PSM_WALK; 
			else eMove = PSM_RUN; // if(fDN > 5.0f) 
			this->ActionMove(eMove);

			vPos += vDir * (fSpeedAbsolute * s_fSecPerFrm); 
		}

		float fYTerrain = ACT_WORLD->GetHeightWithTerrain(vPos.x, vPos.z); 
		float fYMesh = ACT_WORLD->GetHeightNearstPosWithShape(vPos, 1.0f); 
		if(fYMesh != -FLT_MAX && fYMesh > fYTerrain && fYMesh < m_fYNext + 1.0f) m_fYNext = fYMesh; 
		else m_fYNext = fYTerrain;
		this->PositionSet(vPos, false);
	}

	if(PSA_ATTACK == m_eState || m_iSkillStep != 0)
	{
		CPlayerBase* pTarget = this->TargetPointerCheck(false);
		CPlayerBase::ProcessAttack(pTarget); 
	}

	CPlayerBase::Tick(); 
}

void CPlayerNPC::MoveTo(float fPosX, float fPosY, float fPosZ, float fSpeed, int iMoveMode)
{
	m_vPosFromServer.Set(fPosX, fPosY, fPosZ);
	if(m_pShapeExtraRef) return; 

	if(0 == iMoveMode) 
	{
	}
	else if(iMoveMode)
	{
		m_fMoveSpeedPerSec = fSpeed;
		__Vector3 vPos = m_Chr.Pos(); vPos.y = 0;
		__Vector3 vPosS(fPosX, 0, fPosZ);
	
		if(fSpeed) m_fMoveSpeedPerSec *= ((vPosS - vPos).Magnitude() / (fSpeed * PACKET_INTERVAL_MOVE)) * 0.85f;
		else m_fMoveSpeedPerSec = ((vPosS - vPos).Magnitude() / (fSpeed * PACKET_INTERVAL_MOVE)) * 0.85f; 
		if(fSpeed < 0) m_fMoveSpeedPerSec *= -1.0f;
	}
	else
	{
//		__ASSERT(0, "Invalid Move Mode");
	}
}

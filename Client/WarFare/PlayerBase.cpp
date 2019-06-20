#include "PlayerBase.h"
#include "DFont.h"

#include "N3WorldManager.h"
#include "N3FXMgr.h"
#include "N3ShapeExtra.h"
#include "N3SndObj.h"

#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

static const __Vector3 s_vLightOffset = __Vector3(10.0f, 40.0f, 30.0f);
CN3SndObj*	CPlayerBase::m_pSnd_MyMove = NULL;

CPlayerBase::CPlayerBase()
{
	m_ePlayerType = PLAYER_BASE; // Player Type ... Base, NPC, OTher, MySelf

	m_Chr.PartAlloc(PART_POS_COUNT);
	m_Chr.PlugAlloc(PLUG_POS_COUNT);

	m_Chr.PosSet(__Vector3(0.0f, 0.0f, 0.0f));

	m_pLooksRef = NULL;				
	memset(m_pItemPlugBasics, 0, sizeof(m_pItemPlugBasics)); 
	memset(m_pItemPlugExts, 0, sizeof(m_pItemPlugExts));
	memset(m_pItemPartBasics, 0, sizeof(m_pItemPartBasics)); 
	memset(m_pItemPartExts, 0, sizeof(m_pItemPartExts));

	m_iIDTarget	= -1;
	m_iMagicAni = 0;
	m_bGuardSuccess	= false;	
	m_bVisible = true;			

	m_iDroppedItemID				= 0;	

	m_Chr.m_nJointPartStarts[0]		= 1;	
	m_Chr.m_nJointPartEnds[0]		= 15;	

	m_Chr.m_nJointPartStarts[1]		= 16;	
	m_Chr.m_nJointPartEnds[1]		= 23;	

	m_cvDuration.a = m_cvDuration.r = m_cvDuration.g = m_cvDuration.b = 1.0f;
	m_fDurationColorTimeCur = 0; 
	m_fDurationColorTime = 0;

	m_fFlickeringFactor = 1.0f; 
	m_fFlickeringTime = 0;

	m_fTimeAfterDeath = 0; 

	m_eStateNext = m_eState = PSA_BASIC;
	m_eStateMove = PSM_STOP; 
	m_eStateDying = PSD_UNKNOWN; 
	m_fTimeDying = 0; 

	m_fRotRadianPerSec = D3DXToRadian(270.0f);
	m_fMoveSpeedPerSec = 0;
	m_fYawCur = 0;
	m_fYawToReach = 0;

	m_fGravityCur = 0;			
	m_fYNext = 0;				

	m_fScaleToSet = 1.0f;			
	m_fScalePrev = 1.0f;

	m_pSnd_Move = NULL;
	m_pSnd_Attack_0 = NULL;
//	m_pSnd_Attack_1 = NULL;
	m_pSnd_Struck_0 = NULL;
//	m_pSnd_Struck_1 = NULL;
	m_pSnd_Breathe_0 = NULL;
//	m_pSnd_Breathe_1 = NULL;
	m_pSnd_Blow = NULL;
	m_bSoundAllSet = false;

	m_InfoBase.Init();				

	m_pShapeExtraRef = NULL;					

	m_fCastFreezeTime = 0.0f;
	m_iSkillStep = 0;			
	m_fAttackDelta = 1.0f;			
	m_fMoveDelta = 1.0f;			

	m_vDirDying.Set(0,0,1);

	m_pIDFont = NULL;
	m_pClanFont = NULL;
	m_pInfoFont = NULL;
	m_pBalloonFont = NULL;
	m_fTimeBalloon = 0;

	m_bAnimationChanged = false;

	m_pvVertex[0].Set(-SHADOW_PLANE_SIZE, 0.0f,  SHADOW_PLANE_SIZE, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	m_pvVertex[1].Set( SHADOW_PLANE_SIZE, 0.0f,  SHADOW_PLANE_SIZE, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	m_pvVertex[2].Set( SHADOW_PLANE_SIZE, 0.0f, -SHADOW_PLANE_SIZE, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	m_pvVertex[3].Set(-SHADOW_PLANE_SIZE, 0.0f, -SHADOW_PLANE_SIZE, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);

	uint16_t*		pIdx = m_pIndex;

	*pIdx++ = 0;  *pIdx++ = 1;  *pIdx++ = 3;
	*pIdx++ = 2;  *pIdx++ = 3;  *pIdx++ = 1;

	m_N3Tex.Create(SHADOW_SIZE, SHADOW_SIZE, D3DFMT_A4R4G4B4, 0);	
}

CPlayerBase::~CPlayerBase()
{
	delete m_pClanFont; m_pClanFont = NULL;
	delete m_pIDFont; m_pIDFont = NULL;
	delete m_pInfoFont; m_pInfoFont = NULL;
	delete m_pBalloonFont; m_pBalloonFont = NULL;

	m_bSoundAllSet = false;
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Move);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_0);
//	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_0);
//	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_0);
//	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Blow);

	if(m_pShapeExtraRef) m_pShapeExtraRef->m_bVisible = false;
}

void CPlayerBase::Release()
{
	m_AnimationDeque.clear();

	m_Chr.Release();
	m_Chr.PartAlloc(PART_POS_COUNT);
	m_Chr.PlugAlloc(PLUG_POS_COUNT);

	m_bSoundAllSet = false;
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Move);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_0);
//	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_0);
//	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_0);
//	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Blow);

	m_pLooksRef = NULL;				
	memset(m_pItemPlugBasics, 0, sizeof(m_pItemPlugBasics)); 
	memset(m_pItemPlugExts, 0, sizeof(m_pItemPlugExts));
	memset(m_pItemPartBasics, 0, sizeof(m_pItemPartBasics));
	memset(m_pItemPartExts, 0, sizeof(m_pItemPartExts));

	m_iIDTarget	= -1;
	m_bGuardSuccess = false;
	m_bVisible = true;

	m_cvDuration.a = m_cvDuration.r = m_cvDuration.g = m_cvDuration.b = 1.0f;
	m_fDurationColorTimeCur = 0; 
	m_fDurationColorTime = 0; 

	m_fFlickeringFactor = 1.0f;
	m_fFlickeringTime = 0; 

	m_fTimeAfterDeath = 0;

	m_eStateNext = m_eState = PSA_BASIC;
	m_eStateMove = PSM_STOP;
	m_eStateDying = PSD_UNKNOWN;
	m_fTimeDying = 0; 

	m_fRotRadianPerSec = D3DXToRadian(270.0f);
	m_fMoveSpeedPerSec = 0;
	m_fYawCur = 0;
	m_fYawToReach = 0;

	m_fGravityCur = 0;			
	m_fYNext = 0;				

	m_fScaleToSet = 1.0f;			
	m_fScalePrev = 1.0f;

	m_InfoBase.Init();				

	m_pShapeExtraRef = NULL;					

	m_fCastFreezeTime = 0.0f;
	m_iSkillStep = 0;			
	m_fAttackDelta = 1.0f;		
	m_fMoveDelta = 1.0f;			
	m_vDirDying.Set(0,0,1);

	m_bAnimationChanged = false;

	if(m_pShapeExtraRef) m_pShapeExtraRef->m_bVisible = false;

	CGameBase::Release();
}

void CPlayerBase::SetSoundAndInitFont(uint32_t dwFontFlag)
{
	if(!m_pLooksRef) return;
	if(true == m_bSoundAllSet) return;

	m_bSoundAllSet = true;
	
	if(!m_pSnd_Move) m_pSnd_Move = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Move);
	if(!m_pSnd_Struck_0) m_pSnd_Struck_0 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Struck0);
//	if(!m_pSnd_Struck_1) m_pSnd_Struck_1 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Struck1);
	if(!m_pSnd_Breathe_0) m_pSnd_Breathe_0 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Breathe0);
//	if(!m_pSnd_Breathe_1) m_pSnd_Breathe_1 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Breathe1);
	if(!m_pSnd_Blow) m_pSnd_Blow = CN3Base::s_SndMgr.CreateObj(100);

	__TABLE_ITEM_BASIC* pItemBasic = m_pItemPlugBasics[PLUG_POS_RIGHTHAND];
	if(!pItemBasic) pItemBasic = m_pItemPlugBasics[PLUG_POS_LEFTHAND];
	this->SetSoundPlug(pItemBasic);

	// Font
	if(!m_pIDFont) 
	{
		std::string szFontID; ::_LoadStringFromResource(IDS_FONT_ID, szFontID);
		m_pIDFont = new CDFont(szFontID, 12, dwFontFlag);//D3DFONT_BOLD);
		m_pIDFont->InitDeviceObjects( s_lpD3DDev );
		m_pIDFont->RestoreDeviceObjects();

		m_pIDFont->SetText(m_InfoBase.szID.c_str());
		m_pIDFont->SetFontColor(m_InfoBase.crID);
	}
}

void CPlayerBase::SetSoundPlug(__TABLE_ITEM_BASIC* pItemBasic)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_0);
//	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_1);
	
	if(pItemBasic)
	{
		m_pSnd_Attack_0 = CN3Base::s_SndMgr.CreateObj(pItemBasic->dwSoundID0);
//		m_pSnd_Attack_1 = CN3Base::s_SndMgr.CreateObj(pItemBasic->dwSoundID1);
	}
	else
	{
		m_pSnd_Attack_0 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Attack0); 
//		m_pSnd_Attack_1 = CN3Base::s_SndMgr.CreateObj(m_pLooksRef->iSndID_Attack1);
	}
}

void CPlayerBase::ReleaseSoundAndFont()
{
	if(false == m_bSoundAllSet) return;
	m_bSoundAllSet = false;

	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Move);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_0);
//	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Attack_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_0);
//	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Struck_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_0);
//	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Breathe_1);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Blow);

	delete m_pIDFont; m_pIDFont = NULL;
	delete m_pInfoFont; m_pInfoFont = NULL;
	delete m_pClanFont; m_pClanFont = NULL;
	delete m_pBalloonFont; m_pBalloonFont = NULL;
}

void CPlayerBase::InfoStringSet(const std::string& szInfo, D3DCOLOR crFont)
{
	if(szInfo.empty())
	{
		delete m_pInfoFont; m_pInfoFont = NULL;
		return;
	}
	else
	{
		if(NULL == m_pInfoFont)
		{

			std::string szFontInfo; ::_LoadStringFromResource(IDS_FONT_INFO, szFontInfo);
			m_pInfoFont = new CDFont(szFontInfo, 12);
			m_pInfoFont->InitDeviceObjects( s_lpD3DDev );
			m_pInfoFont->RestoreDeviceObjects();
		}
		m_pInfoFont->SetText(szInfo.c_str(), D3DFONT_BOLD);
		m_pInfoFont->SetFontColor(crFont);
	}
}

void CPlayerBase::BalloonStringSet(const std::string& szBalloon, D3DCOLOR crFont)
{
	if(szBalloon.empty())
	{
		delete m_pBalloonFont; m_pBalloonFont = NULL;
		m_fTimeBalloon = 0;
		return;
	}
	else
	{
		if(NULL == m_pBalloonFont)
		{
			std::string szFontBalloon; ::_LoadStringFromResource(IDS_FONT_BALLOON, szFontBalloon);
			m_pBalloonFont = new CDFont(szFontBalloon, 12);
			m_pBalloonFont->InitDeviceObjects( s_lpD3DDev );
			m_pBalloonFont->RestoreDeviceObjects();
		}

		m_fTimeBalloon = szBalloon.size() * 0.2f;
	}

	m_pBalloonFont->SetText(szBalloon.c_str(), 0);
	m_pBalloonFont->SetFontColor(crFont);
}

void CPlayerBase::IDSet(int iID, const std::string& szID, D3DCOLOR crID)
{
	m_InfoBase.iID = iID;
	m_InfoBase.szID = szID;
	m_InfoBase.crID = crID;

#ifdef _DEBUG
	m_Chr.m_szName = szID; 
#endif
}

void CPlayerBase::KnightsInfoSet(int iID, const std::string& szName, int iGrade, int iRank)
{
	char szPlug[128] = "";
	if(iGrade > 0 && iGrade <= 5)
	{
		sprintf(szPlug, "Item\\ClanAddOn_%.3d_%d.n3cplug", m_InfoBase.eRace, iGrade);
	}

	CN3CPlugBase* pPlug = this->PlugSet(PLUG_POS_KNIGHTS_GRADE, szPlug, NULL, NULL);

	if(NULL == pPlug) return;

	CN3CPlug* pCPlug = (CN3CPlug*)pPlug;
	_TBL_TABLE_FX* pFXClanRank = s_pTbl_FXSource.Find(FXID_CLAN_RANK_1);

	std::string szFXClanRank = "";
	std::string szEmpty = "";
	if(pFXClanRank)
	{
		if(iRank<=5 && iRank>=1)
			szFXClanRank = pFXClanRank->fxNamefxb;
	}
	pCPlug->InitFX(szFXClanRank, szEmpty, 0xffffffff);
}

void CPlayerBase::RenderChrInRect(CN3Chr* pChr, const RECT& Rect)
{
	if (NULL == pChr) return;

	RECT rcViewport;
	if (Rect.left < 0 ) rcViewport.left = 0;
	else 
		rcViewport.left = Rect.left;
	if (Rect.top < 0 ) rcViewport.top = 0;
	else 
		rcViewport.top = Rect.top;
	if (Rect.right > (int)(s_CameraData.vp.X + s_CameraData.vp.Width)) rcViewport.right = s_CameraData.vp.X + s_CameraData.vp.Width;
	else 
		rcViewport.right = Rect.right;
	if (Rect.bottom > (int)(s_CameraData.vp.Y + s_CameraData.vp.Height)) rcViewport.bottom = s_CameraData.vp.Y + s_CameraData.vp.Height;
	else 
		rcViewport.bottom = Rect.bottom;

	// set viewport
	D3DVIEWPORT9 vp;
	vp.X = rcViewport.left;
	vp.Y = rcViewport.top;

	vp.Width = rcViewport.right - rcViewport.left;
	vp.Height = rcViewport.bottom - rcViewport.top;

	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	if (vp.Width <= 0 || vp.Height <= 0 ||  vp.Width > s_CameraData.vp.Width || vp.Height > s_CameraData.vp.Height)
		return;

	s_lpD3DDev->SetViewport(&vp);

	// set matrix
	__Matrix44 mtxProj, mtxView;
	float fChrHeight = pChr->Height()+0.2f;
	float fVCenter = fChrHeight*0.5f;

	int iWidth = Rect.right - Rect.left;
	int iHeight = Rect.bottom - Rect.top;

	float fViewVolumeHeight = fChrHeight * vp.Height / iHeight;
	float fViewVolumeWidth = fChrHeight * vp.Width / iHeight;	
																
	D3DXMatrixOrthoLH(&mtxProj, fViewVolumeWidth, fViewVolumeHeight, 0, 20);
	
	float fCameraMoveX = ((fChrHeight*iWidth/iHeight)-fViewVolumeWidth)/2.0f;
	float fCameraMoveY = (fChrHeight-fViewVolumeHeight)/2.0f;

	if (rcViewport.left != Rect.left) 
		fCameraMoveX = -fCameraMoveX;	

	if (rcViewport.top != Rect.top) 
		fCameraMoveY = -fCameraMoveY;			

	const __Vector3& vChrPos = pChr->Pos();
	D3DXMatrixLookAtLH( &mtxView, &D3DXVECTOR3( vChrPos.x + fCameraMoveX, vChrPos.y + fVCenter+2.0f + fCameraMoveY, vChrPos.z + 10.0f ),&D3DXVECTOR3( vChrPos.x + fCameraMoveX, vChrPos.y + fVCenter + fCameraMoveY, vChrPos.z + 0.0f ), &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
	s_lpD3DDev->SetTransform( D3DTS_VIEW, &mtxView );
	s_lpD3DDev->SetTransform( D3DTS_PROJECTION, &mtxProj);

	// backup render state
	DWORD dwFog, dwZEnable;//, dwLighting;
	s_lpD3DDev->GetRenderState( D3DRS_ZENABLE, &dwZEnable );
	//s_lpD3DDev->GetRenderState( D3DRS_LIGHTING, &dwLighting );
	s_lpD3DDev->GetRenderState( D3DRS_FOGENABLE , &dwFog );

	// set render state
	if (D3DZB_TRUE != dwZEnable) 
		s_lpD3DDev->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE);
	/*if (FALSE != dwLighting) 
		s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, FALSE);*/
	if (FALSE != dwFog) 
		s_lpD3DDev->SetRenderState( D3DRS_FOGENABLE, FALSE);

	// render
	D3DRECT rc = { Rect.left, Rect.top, Rect.right, Rect.bottom };
	s_lpD3DDev->Clear(1, &rc, D3DCLEAR_ZBUFFER, 0, 1.0f, 0); // Z Buffer Clear

	int iLODPrev = CN3Chr::LODDelta();
	CN3Chr::LODDeltaSet(0);
	pChr->m_nLOD = 1;
	pChr->Render();
	CN3Chr::LODDeltaSet(iLODPrev);

	// restore
	if (D3DZB_TRUE != dwZEnable) 
		s_lpD3DDev->SetRenderState( D3DRS_ZENABLE, dwZEnable);
	/*if (FALSE != dwLighting) 
		s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, dwLighting);*/
	if (FALSE != dwFog) 
		s_lpD3DDev->SetRenderState( D3DRS_FOGENABLE, dwFog);

    s_lpD3DDev->SetTransform( D3DTS_PROJECTION, &(s_CameraData.mtxProjection));
	s_lpD3DDev->SetTransform( D3DTS_VIEW, &(s_CameraData.mtxView) );
	s_lpD3DDev->SetViewport(&(s_CameraData.vp));
}

void CPlayerBase::DurationColorSet(const _D3DCOLORVALUE &color, float fDurationTime)
{
	m_fDurationColorTime = fDurationTime;
	m_fDurationColorTimeCur = 0;
	m_cvDuration = color;
}

void CPlayerBase::FlickerFactorSet(float fAlpha)
{
	if(fAlpha < 0.1f) fAlpha = 0.1f;
	if(fAlpha > 1.0f) fAlpha = 1.0f;

	m_fFlickeringFactor = fAlpha; 
	m_fFlickeringTime = 0; 
}

void CPlayerBase::RotateTo(float fYaw, bool bImmediately)
{
	int iLot = (int)(fYaw/__PI2);
	if(iLot) fYaw -= iLot * __PI2; // 0 ~ 360

	iLot = (int)(fYaw/__PI);
	if(iLot) fYaw -= iLot * __PI2; // -180 ~ 180

	m_fYawToReach = fYaw;
	if(bImmediately) m_fYawCur = fYaw; 
}

void CPlayerBase::RotateTo(CPlayerBase* pOther)
{
	if(NULL == pOther || pOther == this) return;

	__Vector3 vDir = pOther->Position() - m_Chr.Pos();
	vDir.y = 0.0f;
	vDir.Normalize();

	float fYaw = ::_Yaw2D(vDir.x, vDir.z);
	this->RotateTo(fYaw, false);

}

void CPlayerBase::TickYaw()
{
	if(m_fYawCur != m_fYawToReach && this->IsAlive()) 
	{
		float fYawDiff = m_fYawToReach - m_fYawCur; 
		float fYawDelta = m_fRotRadianPerSec * s_fSecPerFrm; 
		if(T_Abs(fYawDiff) <= fYawDelta)
		{
			m_fYawCur = m_fYawToReach; 
		}
		else
		{
			if(fYawDiff > 0)
			{
				if(fYawDiff < __PI) fYawDelta *= 1.0f;
				else if(fYawDiff >= __PI) fYawDelta *= -1.0f;
			}
			else
			{
				if(fYawDiff > -__PI) fYawDelta *= -1.0f;
				else if(fYawDiff <= -__PI) fYawDelta *= 1.0f;
			}

			m_fYawCur += fYawDelta;

			if(T_Abs(m_fYawCur) > __PI)
			{
				int iLot = (int)(m_fYawCur/__PI);
				if(iLot) m_fYawCur -= iLot * __PI2; // 0 ~ 360.

				iLot = (int)(m_fYawCur/__PI);
				if(iLot) m_fYawCur -= iLot * __PI2; // -180 ~ 180
			}
		}
	}

	__Quaternion qtRot;
	qtRot.RotationAxis(0,1,0,m_fYawCur);
	m_Chr.RotSet(qtRot);
}

void CPlayerBase::TickAnimation()
{
	float fAniSpeedDelta = 1.0f;

	if(PSM_STOP != m_eStateMove) 
		fAniSpeedDelta = m_fMoveDelta; 
	else
		if(PSA_ATTACK == m_eState) fAniSpeedDelta = m_fAttackDelta;

	__ASSERT(fAniSpeedDelta >= 0.1f && fAniSpeedDelta < 10.0f, "Invalid Animation Speed Delta!!!");
	m_Chr.AniSpeedDeltaSet(fAniSpeedDelta);
	m_Chr.Tick(); 

	m_bAnimationChanged = false;	
	if(	m_Chr.IsAnimEnd() )
	{
		m_bAnimationChanged = true;
		if(m_AnimationDeque.empty()) 
		{
			CPlayerBase* pTarget = this->CharacterGetByID(m_iIDTarget, true);
			this->Action(m_eStateNext, true, pTarget); 
		}
		else
		{
			e_Ani eAniToSet = m_AnimationDeque[0];
			m_AnimationDeque.pop_front();
			m_Chr.AniCurSet(eAniToSet);
			printf("      Animation : %d\n", eAniToSet);
		}
	}
}

void CPlayerBase::TickDurationColor()
{
	if(m_fDurationColorTime <= 0) 
		return;

	if(m_fDurationColorTimeCur > m_fDurationColorTime) 
	{
		m_fDurationColorTime = 0;
		m_fDurationColorTimeCur = 0;

		CN3CPart* pPart = NULL;
		int iPC = m_Chr.m_Parts.size();
		for(int i = 0; i < iPC; i++)
		{
			pPart = m_Chr.m_Parts[i];
			pPart->m_Mtl = pPart->m_MtlOrg;
		}
	}
	else
	{
		float fD = m_fDurationColorTimeCur / m_fDurationColorTime;
		CN3CPart* pPart = NULL;
		int iPC = m_Chr.m_Parts.size();
		for(int i = 0; i < iPC; i++)
		{
			pPart = m_Chr.m_Parts[i];

  		  /*pPart->m_Mtl.Ambient.a = pPart->m_MtlOrg.Ambient.a * fD + m_cvDuration.a * (1.0f - fD);*/
			pPart->m_Mtl.Ambient.r = pPart->m_MtlOrg.Ambient.r * fD + m_cvDuration.r * (1.0f - fD);
			pPart->m_Mtl.Ambient.g = pPart->m_MtlOrg.Ambient.g * fD + m_cvDuration.g * (1.0f - fD);
			pPart->m_Mtl.Ambient.b = pPart->m_MtlOrg.Ambient.b * fD + m_cvDuration.b * (1.0f - fD);

			/*pPart->m_Mtl.Diffuse.a = pPart->m_MtlOrg.Diffuse.a * fD + m_cvDuration.a * (1.0f - fD);*/
			pPart->m_Mtl.Diffuse.r = pPart->m_MtlOrg.Diffuse.r * fD + m_cvDuration.r * (1.0f - fD);
			pPart->m_Mtl.Diffuse.g = pPart->m_MtlOrg.Diffuse.g * fD + m_cvDuration.g * (1.0f - fD);
			pPart->m_Mtl.Diffuse.b = pPart->m_MtlOrg.Diffuse.b * fD + m_cvDuration.b * (1.0f - fD);
		}
	}

	m_fDurationColorTimeCur += s_fSecPerFrm;
}

void CPlayerBase::TickSound()
{
	__Vector3 vPos = this->Position();

	if(PSA_ATTACK == m_eState)
	{
		/*if(m_pSnd_Attack_0 && m_Chr.NeedPlaySound0())
			m_pSnd_Attack_0->Play(&vPos); */

		/*if(m_pSnd_Attack_1 && m_Chr.NeedPlaySound1())
			m_pSnd_Attack_1->Play(&vPos);*/

		if(m_pSnd_Attack_0 && (m_Chr.NeedPlaySound0() || m_Chr.NeedPlaySound1()))
			m_pSnd_Attack_0->Play(&vPos);

		/*if(m_pSnd_Attack_1 && m_Chr.NeedPlaySound1())
			m_pSnd_Attack_1->Play(&vPos);*/
	}
	
	if(PSM_STOP == m_eStateMove)
	{
		if(PSA_DYING == m_eState && m_Chr.NeedPlaySound0())
		{
			int DeadSoundID = (rand()%2) ? m_pLooksRef->iSndID_Dead0 : m_pLooksRef->iSndID_Dead1;
			CN3Base::s_SndMgr.PlayOnceAndRelease(DeadSoundID, &vPos);
		}
		else if(PSA_BASIC == m_eState && m_Chr.NeedPlaySound0())
		{
			/*CN3SndObj* pSndBreath = (rand()%2) ? m_pSnd_Breathe_0 : m_pSnd_Breathe_1;
			if(pSndBreath) 
				pSndBreath->Play(&vPos);*/

			if(m_pSnd_Breathe_0)
				m_pSnd_Breathe_0->Play(&vPos);
		}		
	}
	else if(PSM_WALK == m_eStateMove || PSM_RUN == m_eStateMove || PSM_WALK_BACKWARD == m_eStateMove )
	{
		if(m_pSnd_Move && m_pSnd_Move != m_pSnd_MyMove && m_Chr.NeedPlaySound0()) 
			m_pSnd_Move->Play(&vPos);

		if(m_pSnd_Move && m_pSnd_Move != m_pSnd_MyMove && m_Chr.NeedPlaySound1()) 
			m_pSnd_Move->Play(&vPos);
	}
}

void CPlayerBase::Tick()
{
	if(m_pShapeExtraRef) 
	{
		m_pShapeExtraRef->Tick(FRAME_SELFPLAY);
		return;
	}

	__Vector3 vPos = this->Position();
	if(vPos.y > m_fYNext)
	{
		m_fGravityCur += 9.8f * CN3Base::s_fSecPerFrm;
		vPos.y -= m_fGravityCur;
		this->PositionSet(vPos, false);
	}

	if(vPos.y < m_fYNext)
	{
		vPos.y = m_fYNext;
		this->PositionSet(vPos, false);
		m_fGravityCur = 0;
	}

	if(m_fScaleToSet != m_fScalePrev)
	{
		float fScale = m_Chr.Scale().y;
		if(m_fScaleToSet > m_fScalePrev) 
		{
			fScale += (m_fScaleToSet - m_fScalePrev) * s_fSecPerFrm;
			if(fScale > m_fScaleToSet)
			{
				m_fScalePrev = fScale = m_fScaleToSet;
			}
		}
		else
		{
			fScale -= (m_fScalePrev - m_fScaleToSet) * s_fSecPerFrm;
			if(fScale < m_fScaleToSet)
			{
				m_fScalePrev = fScale = m_fScaleToSet;
			}
		}
		m_Chr.ScaleSet(fScale, fScale, fScale);
	}

	this->TickYaw();
	this->TickAnimation();
	this->TickDurationColor();
	this->TickSound();

	if(m_fTimeAfterDeath > 0) m_fTimeAfterDeath += s_fSecPerFrm;
	if(PSA_DYING == m_eState) 
	{
		if(PSD_DISJOINT == m_eStateDying || PSD_KNOCK_DOWN == m_eStateDying)
		{
			float fAD = 0;
			if(m_fTimeDying <= 0.2f) fAD = 5.0f;
			else if(m_fTimeDying < 0.4f) fAD = 5.0f * (0.4f - m_fTimeDying);

			if(fAD > 0) 
			{
				float fDelta = (fAD * (0.3f + 0.7f / m_Chr.Radius()));
				vPos -= m_vDirDying * (fDelta * s_fSecPerFrm);
				vPos.y = ACT_WORLD->GetHeightWithTerrain(vPos.x, vPos.z);
				m_Chr.PosSet(vPos);
			}
		}
		else
		{

		}
		m_fTimeDying += s_fSecPerFrm;
	}

	if(m_fTimeBalloon > 0)
	{
		m_fTimeBalloon -= s_fSecPerFrm;
		if(m_fTimeBalloon < 0)
		{
			m_fTimeBalloon = 0;
			this->BalloonStringSet("", 0);
		}
	}
}

void CPlayerBase::Render(float fSunAngle)
{
	if(m_Chr.m_nLOD < 0 || m_Chr.m_nLOD >= MAX_CHR_LOD) return;

#ifdef _DEBUG
	if(m_pShapeExtraRef)
	{
		m_pShapeExtraRef->RenderCollisionMesh();
		return;
	}
#endif

	float fFactorToApply = 1.0f;
	if(m_fTimeAfterDeath > TIME_CORPSE_REMAIN - TIME_CORPSE_REMOVE)
		fFactorToApply = (TIME_CORPSE_REMAIN - m_fTimeAfterDeath) / TIME_CORPSE_REMOVE;
	else if(m_fFlickeringFactor != 1.0f)
	{
		m_fFlickeringTime += s_fSecPerFrm;
		if(m_fFlickeringTime < 0.5f)
			fFactorToApply = m_fFlickeringFactor;
		else if(m_fFlickeringTime < 1.0f)
			fFactorToApply = m_fFlickeringFactor * 0.5f;
		else
		{
			m_fFlickeringTime = 0;
			fFactorToApply = m_fFlickeringFactor;
		}
	}


	if(fFactorToApply != 0)
	{
		DWORD dwAlphaBlend, dwAlphaOP, dwAlphaArg1, dwTexFactor, dwSrcBlend, dwDestBlend; // , dwZEnable;
		
		// backup state
		s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlend);
		s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
		s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
		//s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
		s_lpD3DDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTexFactor);
		s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOP);
		s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);

		DWORD dwFactorToApply = ((DWORD)(255.0f * fFactorToApply)) << 24;

		// render state
		s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		//s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
		s_lpD3DDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwFactorToApply);
		
		// texture state
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

		m_Chr.Render();

		// restore state
		s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlend);
		s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
		s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
		//s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
		s_lpD3DDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTexFactor);	// alpha factor 설정
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOP);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg1);
	}
	else
	{
		m_Chr.Render();
	}

	if (s_Options.iUseShadow) this->RenderShadow(fSunAngle);

#ifdef _DEBUG
	if(m_Chr.CollisionMesh())
	{
		s_lpD3DDev->SetTransform(D3DTS_WORLD, &(m_Chr.m_Matrix));
		m_Chr.CollisionMesh()->Render(0xffff0000);
	}

	__Vector3 vLine[3];
	vLine[0] = m_Chr.Pos(); vLine[0].y += 1.3f;
	vLine[1] = m_vPosFromServer; vLine[1].y += 1.3f;
	vLine[2] = vLine[1]; vLine[2].y += 3.0f;
	__Matrix44 mtx; mtx.Identity();
	CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtx);
	CN3Base::RenderLines(vLine, 2, 0xff00ffff);
#endif

	if(m_InfoBase.bRenderID && m_pIDFont)
	{
		float fDist = (m_Chr.Pos() - s_CameraData.vEye).Magnitude();

		// NOTE(srmeier): this will prevent the NPC text from displaying past a certain distance
		if(fDist < 48.0f)
		{
			__Vector3 vHead = this->HeadPosition();
			vHead.y += this->Height() / 10.0f;
			if(PSA_SITDOWN == m_eState) vHead.y += this->RootPosition().y - this->Height()/2.0f;
			POINT pt = ::_Convert3D_To_2DCoordinate(vHead, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp.Width, s_CameraData.vp.Height);

			SIZE size = m_pIDFont->GetSize();
			pt.y -= size.cy + 5;
			D3DCOLOR crFont = m_pIDFont->GetFontColor();

			m_pIDFont->DrawText(pt.x - (size.cx / 2) - 1.0f, pt.y - 1.0f, 0xff000000, 0);
			m_pIDFont->DrawText(pt.x - (size.cx / 2) + 1.0f, pt.y + 1.0f, 0xff000000, 0);
			m_pIDFont->DrawText(pt.x - (size.cx / 2) + 0.0f, pt.y + 0.0f, crFont, 0);

			if(m_pClanFont && m_pClanFont->IsSetText())
			{
				size = m_pClanFont->GetSize();
				pt.y -= size.cy + 5;
				crFont = m_pClanFont->GetFontColor();
				m_pClanFont->DrawText(pt.x - (size.cx / 2) - 1.0f, pt.y - 1.0f, 0xff000000, 0);
				m_pClanFont->DrawText(pt.x - (size.cx / 2) + 1.0f, pt.y + 1.0f, 0xff000000, 0);
				m_pClanFont->DrawText(pt.x - (size.cx / 2) + 0.0f, pt.y + 0.0f, crFont, 0);
			}
			
			if(m_pInfoFont && m_pInfoFont->IsSetText())//->GetFontHeight() > 0)
			{
				size = m_pInfoFont->GetSize();
				pt.y -= size.cy + 5;
				crFont = m_pInfoFont->GetFontColor();
				m_pInfoFont->DrawText(pt.x - (size.cx / 2) - 1.0f, pt.y - 1.0f, 0xff000000, 0);
				m_pInfoFont->DrawText(pt.x - (size.cx / 2) + 1.0f, pt.y + 1.0f, 0xff000000, 0);
				m_pInfoFont->DrawText(pt.x - (size.cx / 2) + 0.0f, pt.y + 0.0f, crFont, 0);
			}

			if(m_pBalloonFont && m_pBalloonFont->IsSetText())//->GetFontHeight())
			{
				crFont = m_pBalloonFont->GetFontColor();
				if(m_fTimeBalloon < 2.0f)
				{
					uint32_t crFont = m_pBalloonFont->GetFontColor();
					crFont = (crFont & 0x00ffffff) | ((uint32_t)(255 * (m_fTimeBalloon/2.0f))<<24);
					m_pBalloonFont->SetFontColor(crFont);
				}

				size = m_pBalloonFont->GetSize();
				pt.y -= size.cy + 5;
				m_pBalloonFont->DrawText(pt.x - (size.cx / 2) - 1.0f, pt.y - 1.0f, 0xff000000, 0);
				m_pBalloonFont->DrawText(pt.x - (size.cx / 2) + 1.0f, pt.y + 1.0f, 0xff000000, 0);
				m_pBalloonFont->DrawText(pt.x - (size.cx / 2) + 0.0f, pt.y + 0.0f, crFont, D3DFONT_BOLD);
			}
		}
	}
}

__Vector3 CPlayerBase::HeadPosition()
{
	__Vector3 vHead = m_Chr.Pos();
	vHead.y += m_Chr.Height();

	return vHead;
}

float CPlayerBase::Height()
{
	if(m_pShapeExtraRef) 
	{
		return (m_pShapeExtraRef->Max().y - m_pShapeExtraRef->Min().y) * m_pShapeExtraRef->Scale().y;
	}
	else
	{
		return m_Chr.Height();
	}
}

float CPlayerBase::Radius()
{
	if(m_pShapeExtraRef) 
	{
		return m_pShapeExtraRef->Radius();
	}
	else
	{
		return m_Chr.Radius();
	}
}

__Vector3 CPlayerBase::Direction()
{
	__Vector3 vDir(0,0,1);
	__Matrix44 mtxRot = m_Chr.Rot();
	vDir *= mtxRot;

	return vDir;
}

bool CPlayerBase::Action(e_StateAction eState, bool bLooping, CPlayerBase* pTarget, bool bForceSet)
{
	if(false == bForceSet)
	{
		// State Table Action
		static BOOL sTableAction[PSA_COUNT][PSA_COUNT] =
		{
			//---------------------------------------------------------------------------------------------------------------------------
			//	BASIC,	   ATTACK,		GUARD,	   STRUCK, 		DYING,		DEATH, SPELL_MAGIC,	  SITDOWN
			//---------------------------------------------------------------------------------------------------------------------------
			{		1,			1,			1,			1,			1,			0,			1,			1 }, // PSA_BASIC		-> ??
			//---------------------------------------------------------------------------------------------------------------------------
			{		1,			1,			0,			0, 			1,			0,			1,			0 }, // PSA_ATTACK		-> ??
			//---------------------------------------------------------------------------------------------------------------------------
			{		1,			1,			1,			0, 			1,			0,			1,			0 }, // PSA_GUARD		-> ??
			//---------------------------------------------------------------------------------------------------------------------------
			{		1,			1,			1,			1,			1,			0,			1,			0 }, // PSA_STRUCK		-> ??
			//---------------------------------------------------------------------------------------------------------------------------
			{		0,			0,			0,			0,			0,			1,			0,			0 }, // PSA_DYING		-> ??
			//---------------------------------------------------------------------------------------------------------------------------
			{		0,			0,			0,			0,			0,			0,			0,			0 }, // PSA_DEATH		-> ??
			//---------------------------------------------------------------------------------------------------------------------------
			{		1,			0,			0,			0,			1,			1,			1,			0 }, // PSA_SPELL_MAGIC	-> ??
			//---------------------------------------------------------------------------------------------------------------------------
			{		1,			0,			0,			0,			1,			0,			0,			0 } // PSA_SITDOWN		-> ??
			//---------------------------------------------------------------------------------------------------------------------------
		}; 

		if(FALSE == sTableAction[m_eState][eState])
			return false;
	}

	bool bNPC = (RACE_NPC == m_InfoBase.eRace ? true : false);
	bool bNeedUpperAnimationOnly = false;
	bool bOnceAndFreeze = false;
	e_Ani eAni = ANI_UNKNOWN;
	e_Ani eAniToRestore = ANI_UNKNOWN;
	float fFreezeTime = 0.0f;
	e_StateAction eStatePrev = m_eState;
	m_eStateNext = m_eState = eState;
	
	switch(eState) 
	{
	case PSA_BASIC:
	{
		if(PSM_STOP == m_eStateMove)
		{
			if(PSA_SITDOWN == eStatePrev) 
			{
				eAni = ANI_STANDUP; 
				if(PSM_STOP == m_eStateMove) eAniToRestore = this->JudgeAnimationBreath(); 
				bOnceAndFreeze = true;
			}
			else 
				eAni = this->JudgeAnimationBreath(); 
		}
		else 
			if(PSM_WALK == m_eStateMove) 
				eAni = this->JudgeAnimationWalk(); 
		else 
			if(PSM_RUN == m_eStateMove) 
				eAni = this->JudgeAnimationRun(); 
		else 
			if(PSM_WALK_BACKWARD == m_eStateMove) 
				eAni = this->JudgeAnimationWalkBackward(); 
		else 
			eAni = this->JudgeAnimationBreath();
	}
	break;
	case PSA_ATTACK:
		if(pTarget)
		{
			if(	pTarget->m_InfoBase.eNation == m_InfoBase.eNation) 
				return false; 
			
			if(	!bNPC && IsMovingNow() )	
			{
				if(m_pItemPlugBasics[0]) 
					eAni = ANI_ATTACK_WITH_WEAPON_WHEN_MOVE;		
				else 
					eAni = ANI_ATTACK_WITH_NAKED_WHEN_MOVE;						
				bNeedUpperAnimationOnly = true;									
			}
			else
			{
				eAni = this->JudgeAnimationAttack();			
			}

			if(!bLooping)
			{
				m_eStateNext = PSA_BASIC;
			}
		}
		break;
	case PSA_GUARD:
		{
			m_eStateNext = PSA_BASIC;
			eAni = this->JudgeAnimationGuard();

			if(!bNPC) bNeedUpperAnimationOnly = true;
			fFreezeTime = 1.5f;
		}
		break;
	case PSA_STRUCK:
		{
			m_eStateNext = PSA_BASIC;
			eAni = this->JudgeAnimationStruck();
			if(!bNPC) bNeedUpperAnimationOnly = true;
		}
		break;
	case PSA_DYING:
		m_eStateNext = PSA_DEATH;
		m_eStateMove = PSM_STOP;
		eAni = this->JudgeAnimationDying();
		bOnceAndFreeze = true; 
		break;

	case PSA_DEATH:
		m_eStateNext = PSA_DEATH;
		if(true == bForceSet)
		{
			eAni = this->JudgeAnimationDying();
			m_Chr.AniFixToLastFrame(eAni);
		}
		this->RegenerateCollisionMesh();
		return true; 
	case PSA_SITDOWN:
		eAni = ANI_SITDOWN;
		eAniToRestore = ANI_SITDOWN_BREATH;
		bOnceAndFreeze = true;
		break;
	case PSA_SPELLMAGIC:
		m_eStateNext = PSA_BASIC;
		fFreezeTime = m_fCastFreezeTime;
		eAni = this->JudgetAnimationSpellMagic();
		bOnceAndFreeze = true;
		break;
	default: 
		return false;
	}

	float fBlendTime = FLT_MIN;
	if(bForceSet)
	{
		bNeedUpperAnimationOnly = false;
		fBlendTime = 0;
		m_Chr.m_FrmCtrl.Init();
	}

	this->AnimationClear(); 
	if(ANI_UNKNOWN != eAniToRestore) this->AnimationAdd(eAniToRestore, false);

	if(bNeedUpperAnimationOnly) 
		m_Chr.AniUpperSet(eAni, fFreezeTime);
	else 
	{
		m_Chr.AniCurSet(eAni, bOnceAndFreeze, fBlendTime, fFreezeTime);
	}

	if(bForceSet && m_Chr.m_FrmCtrl.pAniData) 
	{
		m_Chr.m_FrmCtrl.fFrmCur = m_Chr.m_FrmCtrl.pAniData->fFrmEnd; 
	}

	return true;
}

bool CPlayerBase::ActionMove(e_StateMove eMove)
{
	if(this->IsDead()) return false;
	
	static int sStateTableMove[PSM_COUNT][PSM_COUNT] = // State Table Move
	{
		//---------------------------------------------------------------------------------------------------------------------------------------
		//	STOP,	WALK,	RUN,	WALK_BACKWARD
		//---------------------------------------------------------------------------------------------------------------------------------------
		{	0,		1,		1,		1 }, // PSM_STOP
		//---------------------------------------------------------------------------------------------------------------------------------------
		{	1,		0,		1,		1 }, // PSM_WALK
		//---------------------------------------------------------------------------------------------------------------------------------------
		{	1,		1,		0,		1 }, // PSM_RUN
		//---------------------------------------------------------------------------------------------------------------------------------------
		{	1,		1,		1,		0 } // PSM_WALK_BACKWARD
		//---------------------------------------------------------------------------------------------------------------------------------------
	};

	if(FALSE == sStateTableMove[m_eStateMove][eMove]) return false;
	
	m_eStateNext = m_eState;
	m_eStateMove = eMove;

	e_Ani eAni = ANI_UNKNOWN;
	switch(eMove)
	{
		case PSM_STOP:
		eAni = this->JudgeAnimationBreath();
		break;

		case PSM_WALK:
		eAni = this->JudgeAnimationWalk();
		break;

		case PSM_RUN:
		eAni = this->JudgeAnimationRun();
		break;

		case PSM_WALK_BACKWARD:
		eAni = this->JudgeAnimationWalkBackward();
		break;

		default:
			return false;
	}

	this->Action(PSA_BASIC, true); 
	this->AnimationClear();
	m_Chr.AniCurSet(eAni, false, FLT_MIN, 0, false);
	return true;
}

void CPlayerBase::ActionDying(e_StateDying eSD, const __Vector3& vDir)
{
	this->ActionMove(PSM_STOP);
	this->Action(PSA_DYING, false);
	m_eStateDying = eSD;
	m_vDirDying = vDir;

	e_Ani eAni = ANI_DEAD_NEATLY;
	if(eSD == PSD_KEEP_POSITION)
	{
		if(RACE_NPC == m_InfoBase.eRace) 
			eAni = ANI_NPC_DEAD0;
		else 
			eAni = ANI_DEAD_NEATLY;
	}
	else if(eSD == PSD_DISJOINT)
	{
		if(RACE_NPC == m_InfoBase.eRace) 
			eAni = ANI_NPC_DEAD1;
		else 
			eAni = ANI_DEAD_ROLL;
	}
	else if(eSD == PSD_KNOCK_DOWN)
	{
		if(RACE_NPC == m_InfoBase.eRace)
			eAni = ANI_NPC_DEAD1;
		else 
			eAni = ANI_DEAD_KNOCKDOWN;
	}
	else
	{
		if(RACE_NPC == m_InfoBase.eRace) 
			eAni = ANI_NPC_DEAD0;
		else 
			eAni = ANI_DEAD_NEATLY;
	}

	m_Chr.AniCurSet(eAni, true, FLT_MIN, 0, true);
}

bool CPlayerBase::ProcessAttack(CPlayerBase* pTarget)
{
	if(PSA_ATTACK != m_eState && m_iSkillStep == 0) 
		return false;

	if(NULL == pTarget) 
		return false;

	bool bAttackSuccess = false;
	bool bStrike = false;
	if( m_Chr.NeedStrike0() || m_Chr.NeedStrike1() )
		bStrike = true;

	if(bStrike)
	{
		__Vector3 vCol(0,0,0);
		CPlayerBase* pTarget = TargetPointerCheck(false);
		if(NULL == pTarget) return false;

		if(false == this->CheckCollisionToTargetByPlug(pTarget, 0, &vCol))
		{
			if(false == this->CheckCollisionToTargetByPlug(pTarget, 1, &vCol)) 
			{
				const __Matrix44* pMtxMine = m_Chr.MatrixGet(0); 
				const __Matrix44* pMtxTarget = pTarget->m_Chr.MatrixGet(0);

				__Vector3 v0, v1;
				if(pMtxMine)
				{
					CN3CPlug* pPlug = m_Chr.Plug(0);
					if(pPlug)
					{
						__Matrix44 mtx = *(m_Chr.MatrixGet(pPlug->m_nJointIndex));
						v0.Set( 0.0f, pPlug->m_fTrace1, 0.0f );
						v0 *= pPlug->m_Matrix;
						v0 *= mtx;
						v0 *= m_Chr.m_Matrix;
					}
					else v0 = m_Chr.Pos() + pMtxMine->Pos();
				}
				else
				{
					v0 = m_Chr.Pos(); v0.y += m_Chr.Height() / 2.0f;
				}

				if(pMtxTarget)
				{
					v1 = pMtxTarget->Pos() + pTarget->Position();
					if(false == pTarget->CheckCollisionByBox(v0, v1, &vCol, NULL))
					{
						__Vector3 vDir = v0 - v1; vDir.Normalize();
						vCol = v1 + vDir * (pTarget->Height() / 3.0f);
					}
				}
				else if(pTarget->m_pShapeExtraRef && pTarget->m_pShapeExtraRef->CollisionMesh())
				{
					__Vector3 vDir = this->Direction(); vDir.Normalize();
					v1 = v0 + (vDir * 256.0f);

					CN3VMesh* pVMesh = pTarget->m_pShapeExtraRef->CollisionMesh();
					if(NULL == pVMesh || false == pVMesh->CheckCollision(pTarget->m_pShapeExtraRef->m_Matrix, v0, v1, &vCol)) 
					{
						v1 = pTarget->m_pShapeExtraRef->Pos(); v1.y += pTarget->Height() / 2.0f;
						vDir = v1 - v0; vDir.Normalize();
						vCol = v0 + vDir * Radius();
					}
				}
				else
				{
					return false;
				}
			}
		}

		if(pTarget->m_fTimeAfterDeath > 0 && false == pTarget->IsDead())
		{
			e_ItemClass eICR = this->ItemClass_RightHand();		
			e_StateDying eSD = PSD_KEEP_POSITION;
			if(ITEM_CLASS_SWORD_2H == eICR || ITEM_CLASS_AXE_2H == eICR || ITEM_CLASS_MACE_2H == eICR || ITEM_CLASS_POLEARM == eICR) eSD = PSD_DISJOINT; 
			else 
				if(ITEM_CLASS_SWORD == eICR || ITEM_CLASS_AXE == eICR || ITEM_CLASS_MACE == eICR || ITEM_CLASS_SPEAR == eICR) eSD = PSD_KNOCK_DOWN;
			
			__Vector3 vTarget = pTarget->Position();
			if(pTarget->m_pSnd_Blow) pTarget->m_pSnd_Blow->Play(&vTarget); 

			bool bAffected = false;
			if(m_pItemPlugExts[PLUG_POS_RIGHTHAND])
			{
				int iFXID = -1;
				if((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageFire > 0) || (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageFire >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_FIRE_TARGET;			// 불
				else 
					if((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageIce > 0) || (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageIce >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_ICE_TARGET;			// 냉기
				else 
					if((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamagePoison > 0) || (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamagePoison >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_POISON_TARGET;		// 독
				else 
					if((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageThuner > 0) || (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageThuner >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_LIGHTNING_TARGET;		// 전격

				if(iFXID >= 0)
				{
					bAffected = true;
					CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, iFXID, vCol);
				}
			}
			if(m_pItemPlugExts[PLUG_POS_LEFTHAND] && !bAffected)
			{
				int iFXID = -1;
				if((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageFire > 0) || (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageFire >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_FIRE_TARGET;			
				else 
					if((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageIce > 0) || (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageIce >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_ICE_TARGET;		
				else 
					if((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamagePoison > 0) || (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamagePoison >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_POISON_TARGET;	
				else 
					if((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageThuner > 0) || (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageThuner >= LIMIT_FX_DAMAGE)) iFXID = FXID_SWORD_LIGHTNING_TARGET;	

				if(iFXID >= 0)
				{
					bAffected = true;
					CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, iFXID, vCol);
				}
			}			
			if(!bAffected) CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_BLOOD, vCol);

			__Vector3 vDirDeath = this->Position() - vTarget;
			vDirDeath.Normalize();
			pTarget->ActionDying(eSD, vDirDeath);
		}
		else if(pTarget->IsAlive())
		{
			if(false == pTarget->m_bGuardSuccess) 
			{
				__Vector3 vTarget = pTarget->Position();

				if(pTarget->m_pSnd_Blow) pTarget->m_pSnd_Blow->Play(&vTarget);

				bool bAffected = false;
				if(m_pItemPlugExts[PLUG_POS_RIGHTHAND])
				{
					if((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageFire > 0) || (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageFire >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_FIRE_TARGET, vCol);
					}
					else if((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageIce > 0) || (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageIce >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_ICE_TARGET, vCol);
					}
					else if((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamagePoison > 0) || (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamagePoison >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_POISON_TARGET, vCol);
					}
					else if((m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageThuner > 0) || (m_pItemPlugExts[PLUG_POS_RIGHTHAND]->byDamageThuner >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_LIGHTNING_TARGET, vCol);	
					}
				}
				if(m_pItemPlugExts[PLUG_POS_LEFTHAND] && !bAffected)
				{
					if((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageFire > 0) || (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageFire >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_FIRE_TARGET, vCol);	
					}
					else if((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageIce > 0) || (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageIce >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_ICE_TARGET, vCol);	
					}
					else if((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamagePoison > 0) || (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamagePoison >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_POISON_TARGET, vCol);
					}
					else if((m_pItemPlugExts[PLUG_POS_LEFTHAND]->byMagicOrRare==ITEM_ATTRIB_UNIQUE && m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageThuner > 0) || (m_pItemPlugExts[PLUG_POS_LEFTHAND]->byDamageThuner >= LIMIT_FX_DAMAGE))
					{
						bAffected = true;
						CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_SWORD_LIGHTNING_TARGET, vCol);
					}
				}			
				if (!bAffected) 
				{
					CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, FXID_BLOOD, vCol);
					//CGameProcedure::s_pFX->TriggerBundle(pTarget->IDNumber(), 0, 13000, vCol, 0, -1, N3FORMAT_VER_2062);
				}

				D3DCOLORVALUE crHit = { 1.0f, 0.2f, 0.2f, 1.0f };
				pTarget->DurationColorSet(crHit, 0.3f);

				//int iRand = rand()%2;
				//if(iRand == 0) { if(pTarget->m_pSnd_Struck_0) pTarget->m_pSnd_Struck_0->Play(&vTarget); }
				//else if(iRand == 1) { if(pTarget->m_pSnd_Struck_1) pTarget->m_pSnd_Struck_1->Play(&vTarget); }
				if(pTarget->m_pSnd_Struck_0) pTarget->m_pSnd_Struck_0->Play(&vTarget);

				if(0 == pTarget->m_iSkillStep)
					pTarget->Action(PSA_STRUCK, false);
			}
			else
			{
				//if(0 == pTarget->m_iSkillStep)
					//pTarget->Action(PSA_GUARD, false);
			}
		}
	}

	return bAttackSuccess;
}

e_Ani CPlayerBase::JudgeAnimationAttack()
{
	e_Ani eAni = ANI_BREATH;
	if(RACE_NPC == m_InfoBase.eRace) 
	{
		eAni = (e_Ani)(ANI_NPC_ATTACK0 + rand()%2);
	}
	else 
	{
		if(-1 != m_iIDTarget)
		{
			e_ItemClass eICR = this->ItemClass_RightHand(); 
			if(ITEM_CLASS_STAFF == eICR)
			{
				eAni = ANI_DAGGER_ATTACK_A0;
			}
			else
			{
				eAni = (e_Ani)(this->JudgeAnimationBreath() + 1 + rand()%2);
			}
		}
	}

	return eAni;
}

e_Ani CPlayerBase::JudgeAnimationBreath()
{
	e_Ani eAni = ANI_BREATH;

	if(RACE_NPC == m_InfoBase.eRace)
	{
		if(rand()%10 != 0) eAni = ANI_NPC_BREATH;
		else eAni = (e_Ani)(ANI_NPC_TALK0 + rand()%4);
	}
	else
	{
		CPlayerBase* pTarget = TargetPointerCheck(false);

		if(pTarget && pTarget->m_InfoBase.eNation != m_InfoBase.eNation) 
		{
			e_ItemClass eICR = this->ItemClass_RightHand();
			e_ItemClass eICL = this->ItemClass_LeftHand();

			float fIWR = 0; // , fIWL = 0; // Item Weight RightHand, LeftHand
			if(m_pItemPlugBasics[PLUG_POS_RIGHTHAND]) 
				fIWR = m_pItemPlugBasics[PLUG_POS_RIGHTHAND]->siWeight / 10.f;

			/*if(m_pItemPlugBasics[PLUG_POS_LEFTHAND]) 
				fIWL = m_pItemPlugBasics[PLUG_POS_LEFTHAND]->siWeight / 10.f;*/

			if(	(ITEM_CLASS_SWORD == eICR && ITEM_CLASS_SWORD == eICL) || (ITEM_CLASS_AXE == eICR && ITEM_CLASS_AXE == eICL) || (ITEM_CLASS_SWORD == eICR && ITEM_CLASS_AXE == eICL) || (ITEM_CLASS_AXE == eICR && ITEM_CLASS_SWORD == eICL) )
			{
				if(ITEM_CLASS_SWORD == eICR)
				{
					if(fIWR < WEAPON_WEIGHT_STAND_SWORD) eAni = ANI_DUAL_BREATH_A;
					else eAni = ANI_DUAL_BREATH_B;
				}
				else // if(ITEM_CLASS_AXE == eICR)
				{
					if(fIWR < WEAPON_WEIGHT_STAND_AXE) eAni = ANI_DUAL_BREATH_A;
					else eAni = ANI_DUAL_BREATH_B;
				}
			}
			else if(ITEM_CLASS_DAGGER == eICR)
				eAni = ANI_DAGGER_BREATH_A;
			else if(ITEM_CLASS_SWORD == eICR)
			{
				if(fIWR < WEAPON_WEIGHT_STAND_SWORD) eAni = ANI_SWORD_BREATH_A;
				else eAni = ANI_SWORD_BREATH_B;
			}
			else if(ITEM_CLASS_SWORD_2H == eICR) 
				eAni = ANI_SWORD2H_BREATH_A;
			else if(ITEM_CLASS_AXE == eICR)
			{
				if(fIWR < WEAPON_WEIGHT_STAND_AXE) eAni = ANI_AXE_BREATH_A;
				else eAni = ANI_AXE_BREATH_B;
			}
			else if(ITEM_CLASS_AXE_2H == eICR || ITEM_CLASS_MACE_2H == eICR) 
				eAni = ANI_BLUNT2H_BREATH_A; 
			else if(ITEM_CLASS_MACE == eICR)
			{
				if(fIWR < WEAPON_WEIGHT_STAND_BLUNT) eAni = ANI_BLUNT_BREATH_A;
				else eAni = ANI_BLUNT_BREATH_B;
			}
			else if(ITEM_CLASS_SPEAR == eICR)
				eAni = ANI_SPEAR_BREATH_A;
			else if(ITEM_CLASS_POLEARM == eICR)
				eAni = ANI_POLEARM_BREATH_A; 
			else if(eICR == ITEM_CLASS_UNKNOWN && eICL == ITEM_CLASS_BOW) 
				eAni = ANI_BOW_BREATH;
			else if(eICR == ITEM_CLASS_BOW_CROSS && eICL == ITEM_CLASS_UNKNOWN) 
				eAni = ANI_CROSS_BOW_BREATH;
			else if(eICR == ITEM_CLASS_LAUNCHER && eICL >= ITEM_CLASS_UNKNOWN) 
				eAni = ANI_LAUNCHER_BREATH;
			else if(eICR == ITEM_CLASS_UNKNOWN && eICL >= ITEM_CLASS_SHIELD)
			{
				eAni = ANI_SHIELD_BREATH_A;
			}
			else if(eICR == ITEM_CLASS_STAFF)
			{
				eAni = ANI_BREATH;
			}
			else
			{
				eAni = ANI_NAKED_BREATH_A;
			}
		}
		else
		{
			eAni = ANI_BREATH;
		}
	}

	return eAni;
}

e_Ani CPlayerBase::JudgeAnimationWalk()
{
	e_Ani eAni = ANI_WALK;

	if(RACE_NPC == m_InfoBase.eRace) 
	{
		eAni = ANI_NPC_WALK; 
	}
	else 
	{
		eAni = ANI_WALK; 
	}

	return eAni;
}

e_Ani CPlayerBase::JudgeAnimationRun() 
{
	e_Ani eAni = ANI_RUN;

	if(RACE_NPC == m_InfoBase.eRace)
	{
		eAni = ANI_NPC_RUN; 
	}
	else 
	{
		eAni = ANI_RUN;
	}

	return eAni;
}

e_Ani CPlayerBase::JudgeAnimationWalkBackward()
{
	e_Ani eAni = ANI_WALK_BACKWARD;

	if(RACE_NPC == m_InfoBase.eRace) 
	{
		eAni = ANI_NPC_WALK_BACKWARD; 
	}
	else 
	{
		eAni = ANI_WALK_BACKWARD;
	}

	return eAni;
}

e_Ani CPlayerBase::JudgeAnimationStruck()
{
	if(RACE_NPC == m_InfoBase.eRace)
		return (e_Ani)(ANI_NPC_STRUCK0 + rand()%3);
	else
		return (e_Ani)(ANI_STRUCK0 + rand()%3);
}

e_Ani CPlayerBase::JudgeAnimationGuard()
{
	if(RACE_NPC == m_InfoBase.eRace)
		return ANI_NPC_GUARD; 
	else
		return ANI_GUARD; 
}

e_Ani CPlayerBase::JudgeAnimationDying()
{
	if(RACE_NPC == m_InfoBase.eRace) 
		return (e_Ani)(ANI_NPC_DEAD0);
	else 
		return (e_Ani)(ANI_DEAD_NEATLY + rand()%3); 
}

e_Ani CPlayerBase::JudgetAnimationSpellMagic()
{
	if(RACE_NPC == m_InfoBase.eRace) 
		return (e_Ani)(m_iMagicAni);
	else 
		return (e_Ani)(m_iMagicAni);
}

bool CPlayerBase::CheckCollisionByBox(const __Vector3& v0, const __Vector3& v1, __Vector3* pVCol, __Vector3* pVNormal)
{
	CN3VMesh* pvMesh = m_Chr.CollisionMesh();
	if(NULL == pvMesh) 
		return false;

	return pvMesh->CheckCollision(m_Chr.m_Matrix, v0, v1, pVCol, pVNormal);
}

bool CPlayerBase::CheckCollisionToTargetByPlug(CPlayerBase* pTarget, int nPlug, __Vector3* pVCol)
{
	if(NULL == pTarget) 
		return false;

	CN3CPlug* pPlug = m_Chr.Plug(nPlug);
	if(NULL == pPlug) 
		return false;

	if(pPlug->m_fTrace0 >= pPlug->m_fTrace1) 
		return false; 

	if(PSA_DYING == pTarget->State() || PSA_DEATH == pTarget->State()) 
		return false;
	__Vector3 v1, v2, v3;
	__Matrix44 mtx = *(m_Chr.MatrixGet(pPlug->m_nJointIndex));
	
	v1.Set( 0.0f, pPlug->m_fTrace0, 0.0f );
	v2.Set( 0.0f, pPlug->m_fTrace1, 0.0f );

	v1 *= pPlug->m_Matrix;
	v1 *= mtx;
	v1 *= m_Chr.m_Matrix;

	v2 *= pPlug->m_Matrix;
	v2 *= mtx;
	v2 *= m_Chr.m_Matrix;

	v2 += (v2 - v1)*1.0f;

#ifdef _DEBUG
	CN3Base::s_lpD3DDev->BeginScene();

	__Vector3 vLines[2] = { v1, v2 };
	__Matrix44 mtxTmp; mtxTmp.Identity();
	CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtxTmp);
	CN3Base::RenderLines(vLines, 1, (D3DCOLOR)0xffff8080);

	if(m_pShapeExtraRef && m_pShapeExtraRef->CollisionMesh())
	{
		CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &(m_pShapeExtraRef->m_Matrix));
		m_pShapeExtraRef->CollisionMesh()->Render((D3DCOLOR)0xffff0000);
	}
	CN3Base::s_lpD3DDev->EndScene();
	CN3Base::s_lpD3DDev->Present(NULL, NULL, s_hWndBase, NULL);
#endif

	if(m_pShapeExtraRef && m_pShapeExtraRef->CollisionMesh())
	{
		CN3VMesh* pVMesh = m_pShapeExtraRef->CollisionMesh();
		if(pVMesh)
		{
			return pVMesh->CheckCollision(m_pShapeExtraRef->m_Matrix, v1, v2, pVCol);
		}
	}
	return pTarget->CheckCollisionByBox(v1, v2, pVCol, NULL);
}

CN3CPlugBase* CPlayerBase::PlugSet(e_PlugPosition ePos, const std::string& szFN, __TABLE_ITEM_BASIC* pItemBasic, __TABLE_ITEM_EXT* pItemExt)
{
	if(ePos < PLUG_POS_RIGHTHAND || ePos >= PLUG_POS_COUNT)
	{
		__ASSERT(0, "Invalid Plug Position");
		return NULL;
	}

	int iJoint = 0;
	if(PLUG_POS_RIGHTHAND == ePos)
	{
		m_pItemPlugBasics[ePos] = pItemBasic;
		m_pItemPlugExts[ePos] = pItemExt;
		iJoint = m_pLooksRef->iJointRH;
	}
	else if(PLUG_POS_LEFTHAND == ePos)
	{
		m_pItemPlugBasics[ePos] = pItemBasic;
		m_pItemPlugExts[ePos] = pItemExt;
		if(pItemBasic)
		{
			if(pItemBasic->byClass == ITEM_CLASS_SHIELD) iJoint = m_pLooksRef->iJointLH2;
			else 
				iJoint = m_pLooksRef->iJointLH; 		
		}
	}
	else if(PLUG_POS_KNIGHTS_GRADE == ePos)
	{
		m_pItemPlugBasics[ePos] = pItemBasic;
		m_pItemPlugExts[ePos] = pItemExt;
	}
	else if (PLUG_POS_BACK == ePos)
	{
		//m_pItemPlugBasics[PLUG_POS_BACK] = pItem;
	}
	else 
	{ 
		__ASSERT(0, "Invalid Plug Item position"); 
	}


	CN3CPlugBase* pPlug = m_Chr.PlugSet(ePos, szFN);
	if(NULL == pPlug) return NULL;

	if (PLUG_POS_LEFTHAND == ePos || PLUG_POS_RIGHTHAND == ePos)
	{
		float fScale = m_Chr.Height() / 1.8f;  
		fScale *= pPlug->Scale().y / m_Chr.Scale().y;
		pPlug->ScaleSet(__Vector3(fScale, fScale, fScale));
		pPlug->m_nJointIndex = iJoint;
	}
	/*else if (PLUG_POS_BACK == ePos)
	{
		CN3CPlug_Cloak *pPlugCloak = (CN3CPlug_Cloak*)pPlug;
		pPlugCloak->GetCloak()->SetPlayerBase(this);
	}*/

	if(pPlug && NULL == pItemBasic && NULL == pItemExt) pPlug->TexOverlapSet(""); 
	if(pItemExt)
	{
		if((pItemExt->byMagicOrRare==ITEM_ATTRIB_UNIQUE && pItemExt->byDamageFire > 0) || (pItemExt->byDamageFire >= LIMIT_FX_DAMAGE))
		{
			CN3CPlug* pCPlug = (CN3CPlug*)pPlug;
			_TBL_TABLE_FX* pFXMain = s_pTbl_FXSource.Find(FXID_SWORD_FIRE_MAIN);
			_TBL_TABLE_FX* pFXTail = s_pTbl_FXSource.Find(FXID_SWORD_FIRE_TAIL);
			
			std::string szFXMain, szFXTail;
			if(pFXMain) szFXMain = pFXMain->fxNamefxb;
			else szFXMain = "";
			if(pFXTail) szFXTail = pFXTail->fxNamefxb;
			else szFXTail = "";
			pCPlug->InitFX(szFXMain, szFXTail, 0xffffff00);
		}
		else if((pItemExt->byMagicOrRare==ITEM_ATTRIB_UNIQUE && pItemExt->byDamageIce > 0) || (pItemExt->byDamageIce >= LIMIT_FX_DAMAGE))
		{
			CN3CPlug* pCPlug = (CN3CPlug*)pPlug;
			_TBL_TABLE_FX* pFXMain = s_pTbl_FXSource.Find(FXID_SWORD_ICE_MAIN);
			_TBL_TABLE_FX* pFXTail = s_pTbl_FXSource.Find(FXID_SWORD_ICE_TAIL);
			
			std::string szFXMain, szFXTail;
			if(pFXMain) szFXMain = pFXMain->fxNamefxb;
			else szFXMain = "";
			if(pFXTail) szFXTail = pFXTail->fxNamefxb;
			else szFXTail = "";

			pCPlug->InitFX(szFXMain, szFXTail, 0xff0000ff);
		}
		else if((pItemExt->byMagicOrRare==ITEM_ATTRIB_UNIQUE && pItemExt->byDamageThuner > 0) || (pItemExt->byDamageThuner >= LIMIT_FX_DAMAGE))
		{
			CN3CPlug* pCPlug = (CN3CPlug*)pPlug;
			_TBL_TABLE_FX* pFXMain = s_pTbl_FXSource.Find(FXID_SWORD_LIGHTNING_MAIN);
			_TBL_TABLE_FX* pFXTail = s_pTbl_FXSource.Find(FXID_SWORD_LIGHTNING_TAIL);
			
			std::string szFXMain, szFXTail;
			if(pFXMain) szFXMain = pFXMain->fxNamefxb;
			else szFXMain = "";
			if(pFXTail) szFXTail = pFXTail->fxNamefxb;
			else szFXTail = "";
			
			pCPlug->InitFX(szFXMain, szFXTail, 0xffffffff);
		}
		else if((pItemExt->byMagicOrRare==ITEM_ATTRIB_UNIQUE && pItemExt->byDamagePoison > 0) || (pItemExt->byDamagePoison >= LIMIT_FX_DAMAGE))	
		{
			CN3CPlug* pCPlug = (CN3CPlug*)pPlug;
			_TBL_TABLE_FX* pFXMain = s_pTbl_FXSource.Find(FXID_SWORD_POISON_MAIN);
			_TBL_TABLE_FX* pFXTail = s_pTbl_FXSource.Find(FXID_SWORD_POISON_TAIL);

			std::string szFXMain, szFXTail;
			if(pFXMain) szFXMain = pFXMain->fxNamefxb;
			else szFXMain = "";
			if(pFXTail) szFXTail = pFXTail->fxNamefxb;
			else szFXTail = "";
			
			pCPlug->InitFX(szFXMain, szFXTail, 0xffff00ff);
		}
	}
	return pPlug;
}

CN3CPart* CPlayerBase::PartSet(e_PartPosition ePos, const std::string& szFN, __TABLE_ITEM_BASIC* pItemBasic, __TABLE_ITEM_EXT* pItemExt)
{
	if(ePos < PART_POS_UPPER || ePos >= PART_POS_COUNT)
	{
		__ASSERT(0, "Invalid Item Position");
		return NULL;
	}

	if(PART_POS_UPPER == ePos) 
	{
		if(pItemBasic)
		{
			if(pItemBasic->byIsRobeType && m_Chr.Part(PART_POS_LOWER)) 
			{
				this->PartSet(PART_POS_LOWER, "", m_pItemPartBasics[PART_POS_LOWER], m_pItemPartExts[PART_POS_LOWER]);
			}
		}
		else 
		{
			if(m_pItemPartBasics[ePos] && m_pItemPartBasics[ePos]->byIsRobeType)
			{
				if(m_pItemPartBasics[PART_POS_LOWER])
				{
					std::string szFN2;
					e_PartPosition ePartPos2 = PART_POS_UNKNOWN;
					e_PlugPosition ePlugPos2 = PLUG_POS_UNKNOWN;

					CGameProcedure::MakeResrcFileNameForUPC(m_pItemPartBasics[PART_POS_LOWER], &szFN2, NULL, ePartPos2, ePlugPos2, m_InfoBase.eRace);
					this->PartSet(PART_POS_LOWER, szFN2, m_pItemPartBasics[PART_POS_LOWER], m_pItemPartExts[PART_POS_LOWER]);
				}
				else
				{
					__TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks.Find(m_InfoBase.eRace);	// User Player Character Skin
					this->PartSet(PART_POS_LOWER, pLooks->szPartFNs[PART_POS_LOWER], NULL, NULL); 
				}
			}
		}
	}
	else if(PART_POS_LOWER == ePos)
	{
		if(pItemBasic)
		{
			if(m_pItemPartBasics[PART_POS_UPPER] && m_pItemPartBasics[PART_POS_UPPER]->byIsRobeType)
			{
				m_pItemPartBasics[ePos] = pItemBasic;
				m_pItemPartExts[ePos] = pItemExt;
				return m_Chr.PartSet(ePos, "");
			}
		}
	}

	CN3CPart* pPart = NULL;
	if(szFN.empty())
	{
		if(PART_POS_HAIR_HELMET == ePos)
		{
			this->InitHair();
			pPart = m_Chr.Part(ePos);
		}
		else if(PART_POS_FACE == ePos)
		{
			this->InitFace();
			pPart = m_Chr.Part(ePos);
		}
		else
		{
			__TABLE_PLAYER_LOOKS* pLooks = s_pTbl_UPC_Looks.Find(m_InfoBase.eRace);	// Player Character Skin
			if(pLooks)
			{
				pPart = m_Chr.PartSet(ePos, pLooks->szPartFNs[ePos]);
				if(pPart) pPart->TexOverlapSet("");
			}
		}
	}
	else
	{
		pPart = m_Chr.PartSet(ePos, szFN);
	}

	m_pItemPartBasics[ePos] = pItemBasic;
	m_pItemPartExts[ePos] = pItemExt;

	if(pPart && NULL == pItemBasic && NULL == pItemExt) pPart->TexOverlapSet("");

	return pPart;
}

void CPlayerBase::DurabilitySet(e_ItemSlot eSlot, int iDurability)
{
	e_PartPosition ePartPos = PART_POS_UNKNOWN;

	if(ITEM_SLOT_HAND_RIGHT == eSlot || ITEM_SLOT_HAND_LEFT == eSlot)
	{
		e_PlugPosition ePos = PLUG_POS_LEFTHAND;
		if(ITEM_SLOT_HAND_RIGHT == eSlot) ePos = PLUG_POS_RIGHTHAND;
		if(NULL == m_pItemPlugBasics[ePos] || NULL == m_pItemPlugExts[ePos])
		{
			if(0 != iDurability)
			{
				__ASSERT(0, "NULL Item");
			}
			return;
		}

		int iDuMax = m_pItemPlugBasics[ePos]->siMaxDurability + m_pItemPlugExts[ePos]->siMaxDurability;
		if(iDuMax <= 0)
		{
			__ASSERT(0, "최대 내구력 없음");
			return;
		}

		CN3CPlug* pPlug = m_Chr.Plug(ePos);
		if(NULL == pPlug) 
		{
			__ASSERT(0, "NULL Plug");
			return;
		}

		int iPercentage = iDurability * 100 / iDuMax;
		std::string szFN;
		if(iPercentage <= 30) szFN = "Misc\\Dust_Hard.dxt";
		else if(iPercentage <= 70) szFN = "Misc\\Dust_Soft.dxt";

		pPlug->TexOverlapSet(szFN);
	}
	else if(ITEM_SLOT_UPPER == eSlot) 
		ePartPos = PART_POS_UPPER;
	else if(ITEM_SLOT_LOWER == eSlot) 
		ePartPos = PART_POS_LOWER;
	else if(ITEM_SLOT_GLOVES == eSlot) 
		ePartPos = PART_POS_HANDS;
	else if(ITEM_SLOT_SHOES == eSlot)
		ePartPos = PART_POS_FEET;
	else if(ITEM_SLOT_HEAD == eSlot) 
		ePartPos = PART_POS_HAIR_HELMET;

	if(PART_POS_UNKNOWN != ePartPos)
	{
		CN3CPart* pPart = m_Chr.Part(ePartPos);
		if(pPart)
		{
			if(m_pItemPartBasics[ePartPos] && m_pItemPartExts[ePartPos])
			{
				int iDuMax = m_pItemPartBasics[ePartPos]->siMaxDurability + m_pItemPartExts[ePartPos]->siMaxDurability;
				int iPercentage = iDurability * 100 / iDuMax;
			
				std::string szFN;
				if(iPercentage <= 30) szFN = "Misc\\Dust_Hard.dxt";
				else 
					if(iPercentage <= 70) szFN = "Misc\\Dust_Soft.dxt";
				pPart->TexOverlapSet(szFN);
			}
			else
			{
				pPart->TexOverlapSet("");
			}
		}
		else
		{
			__ASSERT(0, "Invalid Item Position");
		}
	}
}

bool CPlayerBase::InitChr(__TABLE_PLAYER_LOOKS* pTbl)
{
	if(NULL == pTbl) return false;

	m_pLooksRef = pTbl;

	m_Chr.JointSet(pTbl->szJointFN);
	m_Chr.AniCtrlSet(pTbl->szAniFN);

	if(RACE_NPC != m_InfoBase.eRace) 
	{
		m_Chr.JointPartSet(0, 16, 23); 
		m_Chr.JointPartSet(1, 1, 15);
	}
	return true;
}


void CPlayerBase::RegenerateCollisionMesh()
{
	m_Chr.FindMinMax();
	__Matrix44 mtxInverse;
	D3DXMatrixInverse(&mtxInverse, 0, &(m_Chr.m_Matrix));
	if(m_Chr.CollisionMesh()) m_Chr.CollisionMesh()->CreateCube(m_Chr.Min() * mtxInverse, m_Chr.Max() * mtxInverse);
}

CPlayerBase* CPlayerBase::TargetPointerCheck(bool bMustAlive)
{
	CPlayerBase* pTarget = this->CharacterGetByID(m_iIDTarget, bMustAlive);
	//if(pTarget && (PSA_DEATH == pTarget->State())) pTarget = NULL; //픽킹에 의해서 선택된 캐릭을 중간의 임의적으로 수정 못하게 막음
	//if(NULL == pTarget) { m_iIDTarget = -1; }
	return pTarget;
}

bool CPlayerBase::JointPosGet(int nJointIdx, __Vector3& vPos)
{
	const __Matrix44* pMtx = m_Chr.MatrixGet(nJointIdx);
	if(!pMtx) return false;

	vPos = pMtx->Pos();
	vPos *= m_Chr.m_Matrix;

	return true;
}

void CPlayerBase::AnimationAdd(e_Ani eAni, bool bImmediately)
{
	if(bImmediately)
	{
		this->AnimationClear();
		m_Chr.AniCurSet(eAni);
	}
	else
	{
		m_AnimationDeque.push_back(eAni);
	}
}

const __Matrix44 CPlayerBase::CalcShadowMtxBasicPlane(__Vector3 vOffs)
{
	__Matrix44	mtx;	
	mtx.Identity();

	mtx._21 = -vOffs.x/vOffs.y;
	mtx._22 = 0.0f;
	mtx._23 = -vOffs.z/vOffs.y;

	mtx._41	= vOffs.x/vOffs.y;
	mtx._42	= 1.0f;
	mtx._43	= vOffs.z/vOffs.y;

	return mtx;
}

void CPlayerBase::RenderShadow(float fAngle)
{
	if(NULL == m_Chr.Joint()) 
		return;

	if(m_Chr.m_nLOD < 0 || m_Chr.m_nLOD > MAX_CHR_LOD) 
		return;
	
	int iLODTemp = m_Chr.m_nLOD;
	iLODTemp += 2;
	if(iLODTemp >= MAX_CHR_LOD) 
		return;

	__Matrix44 mV, mVvar, mVBack, mtxRotX, mtxRotZ;	mV.Identity(); 
	__Vector3 vPosBack, vNom;
	CN3Base::s_lpD3DDev->GetTransform(D3DTS_WORLD, (_D3DMATRIX* )&mVBack); 
	vPosBack = m_Chr.m_Matrix.Pos();	
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &mV);
	ACT_WORLD->GetNormalWithTerrain(vPosBack.x, vPosBack.z, vNom);	vNom.Normalize();

	mV.PosSet(0.0f, 0.0f, 0.0f);
	float fXRad, fZRad;
	fZRad = asin(vNom.z);
	mtxRotZ.RotationX(fZRad);
	fXRad = asin(-vNom.x);
	mtxRotX.RotationZ(fXRad);
	mV *= mtxRotX;
	mV *= mtxRotZ;
	vPosBack.y += 0.1f;

	mV.PosSet(vPosBack);
	mVvar = mV;

	for( int i = 0; i < SHADOW_SIZE; i++ )
		m_bitset[i].Resize(SHADOW_SIZE);

	for ( int i = 0; i < 4; i++ )
	{
		m_vTVertex[i]  = m_pvVertex[i];	
		m_vTVertex[i] *= mV;
	}

	float fScale = Height() * Radius();
	m_fShaScale = 1.0f;
	__Matrix44 mtPos;	mtPos.Identity(); mtPos.PosSet(mV.Pos());
	if (fScale > 6.6f)
	{
		m_fShaScale = 2.2f;
		for ( int i = 0; i < 4; i++ )
		{
			m_vTVertex[i]  = m_pvVertex[i];	
			m_vTVertex[i] *= m_fShaScale;
			m_vTVertex[i] *= mtPos;
		}
	}
	else
	{
		for ( int i = 0; i < 4; i++ )
		{
			m_vTVertex[i]  = m_pvVertex[i];	
			m_vTVertex[i] *= mtPos;
		}
	}

	if ( (fAngle < 4.0f) || (fAngle > 6.1f) )
	{
		if (fAngle > 3.14f)
		{
			fAngle -= 3.14f;
			fAngle = 3.14f - fAngle;
		}
	}
	float fAngleDeg = D3DXToDegree(fAngle);

	float zVal = s_vLightOffset.Magnitude();
	int iDiv = (int)((int)fAngleDeg)%((int)(180));
	fAngleDeg = (float)iDiv;

	if ( (fAngleDeg >= 0.0f) && (fAngleDeg < 50.0f) )
		fAngleDeg = 50.0f;
	else if ( (fAngleDeg > 130.0f) && (fAngleDeg <= 180.0f) )
		fAngleDeg = 130.0f;

	__Matrix44 mtxRZ; mtxRZ.RotationZ(D3DXToRadian(fAngleDeg));
	__Vector3 vLP; vLP.Set(-zVal, 0.0f, 0.0f );	vLP *= mtxRZ;	vLP.Normalize();

	int iPC = m_Chr.PartCount();
	for( int i = 0; i < iPC; i++)
	{
		CalcPart(m_Chr.Part(i), iLODTemp, vLP);
	}

	iPC = m_Chr.PlugCount();
	for(int i = 0; i < iPC; i++)
	{
		CalcPlug(m_Chr.Plug(i), m_Chr.MatrixGet(m_Chr.Plug(i)->m_nJointIndex), mVvar, vLP);
	}

	if (fScale > 6.6f)
	{
		for ( int i = 0; i < 4; i++ )
		{
			m_vTVertex[i]  = m_pvVertex[i];	
			m_vTVertex[i] *= 0.82f;	
			m_vTVertex[i] *= m_fShaScale;
			m_vTVertex[i] *= mV;
		}
	}
	else
	{
		for ( int i = 0; i < 4; i++ )
		{
			m_vTVertex[i]  = m_pvVertex[i];	
			m_vTVertex[i] *= 0.5f;	
			m_vTVertex[i] *= mV;
		}
	}

	D3DLOCKED_RECT LR;
	if (!m_N3Tex.Get()) return;
	HRESULT hr = m_N3Tex.Get()->LockRect(0, &LR, 0, 0); 
	if (hr != D3D_OK)
		return;

	LPWORD pDst16 = (LPWORD)LR.pBits;
	uint16_t dwColor = SHADOW_COLOR;
	dwColor = dwColor << 12;
	for( int i = 0; i < SHADOW_SIZE; i++ )
	{
		for(int j = 0; j < SHADOW_SIZE; j++ )
		{
			if (m_bitset[i].On(j))
				pDst16[SHADOW_SIZE*i+j] = dwColor;
			else
				pDst16[SHADOW_SIZE*i+j] = 0x0000;
		}
	}

	m_N3Tex.Get()->UnlockRect(0);
 
	DWORD dwAlpha, dwFog, dwCull, dwColorVertex, dwMaterial, dwZWrite,
		dwColorOp0, dwColorArg01, dwColorArg02, dwColorOp1, dwColorArg11, dwColorArg12, 
		dwAlphaOp0, dwAlphaArg01, dwAlphaArg02, dwAlphaOp1, dwAlphaArg11, dwAlphaArg12, dwSrcBlend, dwDestBlend, dwBlendOp,
		dwMagFilter0, dwMinFilter0, dwMagFilter1, dwMinFilter1;

	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_COLORVERTEX, &dwColorVertex);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &dwMaterial);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWrite);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp0);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg01);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg02);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_COLOROP, &dwColorOp1);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_COLORARG1, &dwColorArg11);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_COLORARG2, &dwColorArg12);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp0);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg01);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg02);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_ALPHAOP, &dwAlphaOp1);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_ALPHAARG1, &dwAlphaArg11);
	CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_ALPHAARG2, &dwAlphaArg12);
	/*
		CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_MAGFILTER, &dwMagFilter0);
		CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_MINFILTER, &dwMinFilter0);
		CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_MAGFILTER, &dwMagFilter1);
		CN3Base::s_lpD3DDev->GetTextureStageState(1, D3DTSS_MINFILTER, &dwMinFilter1);
	*/
	CN3Base::s_lpD3DDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter0);
	CN3Base::s_lpD3DDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter0);
	CN3Base::s_lpD3DDev->GetSamplerState(1, D3DSAMP_MAGFILTER, &dwMagFilter1);
	CN3Base::s_lpD3DDev->GetSamplerState(1, D3DSAMP_MINFILTER, &dwMinFilter1);

	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_BLENDOP, &dwBlendOp);

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);	
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_MODULATE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_MODULATE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	CN3Base::s_lpD3DDev->SetTexture(0, m_N3Tex.Get());

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	/*
		CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	*/
	CN3Base::s_lpD3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	CN3Base::s_lpD3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	CN3Base::s_lpD3DDev->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	CN3Base::s_lpD3DDev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	CN3Base::s_lpD3DDev->SetFVF(FVF_VNT1);
	CN3Base::s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, m_pIndex, D3DFMT_INDEX16, m_vTVertex, sizeof(__VertexT1) );

	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp0);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg01);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorArg02);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, dwColorOp1);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, dwColorArg11);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, dwColorArg12);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp0);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg01);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwAlphaArg02);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, dwAlphaOp1);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, dwAlphaArg11);
	CN3Base::s_lpD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG2, dwAlphaArg12);

	CN3Base::s_lpD3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMagFilter0);
	CN3Base::s_lpD3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMinFilter0);
	CN3Base::s_lpD3DDev->SetSamplerState(1, D3DSAMP_MAGFILTER, dwMagFilter1);
	CN3Base::s_lpD3DDev->SetSamplerState(1, D3DSAMP_MINFILTER, dwMinFilter1);

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_COLORVERTEX, dwColorVertex);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, dwMaterial);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWrite);

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog); 
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, dwCull);

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_BLENDOP, dwBlendOp);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);

	CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, (_D3DMATRIX *)&mVBack); 
}

void CPlayerBase::CalcPart(CN3CPart* pPart, int nLOD, __Vector3 vDir)
{
	if(NULL == pPart) 
		return;

	if(nLOD < 0 || nLOD > MAX_CHR_LOD) 
		return;

	if(NULL == pPart->Skins() || NULL == pPart->Skin(nLOD) || pPart->Skin(nLOD)->VertexCount() <= 0) 
		return;

#ifdef _DEBUG
	CN3Base::s_RenderInfo.nChr_Part++; // Rendering Information Update...
	CN3Base::s_RenderInfo.nChr_Polygon += pPart->Skin(nLOD)->FaceCount();
#endif

	int iTotalCount = pPart->Skin(nLOD)->VertexCount();	
	if (iTotalCount < 0 || iTotalCount >  10000 ) return;

	__Vector3 vec, A, B, C, vPick;
	CN3Skin* part00 = pPart->Skin(nLOD);
	__VertexXyzNormal* pVDest = part00->m_pVertices;
	float t, u, v, fx, fz;

	for ( int i = 0; i < iTotalCount; i++ )
	{
		if (pVDest)
		{
			vec = pVDest[i];	
			vec *= m_Chr.m_Matrix;
			int iX, iZ;
			
			A = m_vTVertex[m_pIndex[0]];			
			B = m_vTVertex[m_pIndex[1]];			
			C = m_vTVertex[m_pIndex[2]];
			if (::_IntersectTriangle(vec, vDir, A, B, C, t, u, v, &vPick))
			{
				fx = (vPick.x - C.x) / (SHADOW_PLANE_SIZE*2.0f*m_fShaScale);
				fz = (vPick.z - C.z) / (SHADOW_PLANE_SIZE*2.0f*m_fShaScale);
				iX = (int)(fx*SHADOW_SIZE);
				iZ = (int)(fz*SHADOW_SIZE);
				if ( (iX > 0) && (iX < SHADOW_SIZE) && (iZ > 0) && (iZ < SHADOW_SIZE) )
					m_bitset[iZ].Set(iX);
			}

			A = m_vTVertex[m_pIndex[3]];			
			B = m_vTVertex[m_pIndex[4]];			
			C = m_vTVertex[m_pIndex[5]];
			if (::_IntersectTriangle(vec, vDir, A, B, C, t, u, v, &vPick))
			{
				fx = (vPick.x - B.x) / (SHADOW_PLANE_SIZE*2.0f*m_fShaScale);
				fz = (vPick.z - B.z) / (SHADOW_PLANE_SIZE*2.0f*m_fShaScale);
				iX = (int)(fx*SHADOW_SIZE);
				iZ = (int)(fz*SHADOW_SIZE);
				if ( (iX > 0) && (iX < SHADOW_SIZE) && (iZ > 0) && (iZ < SHADOW_SIZE) )
					m_bitset[iZ].Set(iX);
			}
		}
	}
}

void CPlayerBase::CalcPlug(CN3CPlugBase* pPlug, const __Matrix44* pmtxJoint, __Matrix44& mtxMV, __Vector3 vDir)
{
	if(NULL == pPlug || NULL == pPlug->PMeshInst()) 
		return;

	if(pPlug->PMeshInst()->GetNumVertices() <= 0) 
		return;

#ifdef _DEBUG
	CN3Base::s_RenderInfo.nChr_Plug++; // Rendering Information Update...
	CN3Base::s_RenderInfo.nChr_Plug_Polygon += pPlug->PMeshInst()->GetNumIndices() / 3; // Rendering Information Update...
#endif

	static __Matrix44 mtx, mtxBack;
	__Vector3 vOf;
	mtx = pPlug->m_Matrix;
	mtx *= (*pmtxJoint);
	mtx *= m_Chr.m_Matrix;
	
	int iTotalCount = pPlug->PMeshInst()->GetNumVertices();	
	if (iTotalCount < 0 || iTotalCount >  10000 ) 
		return;

	__Vector3 vec, A, B, C, vPick;
	float t, u, v, fx, fz;

#ifdef _USE_VERTEXBUFFER
	LPDIRECT3DVERTEXBUFFER8	pBuf = NULL;
	__VertexT1*	pVerT1 = NULL;
	pBuf = pPlug->PMeshInst()->GetVertexBuffer();
	if (pBuf)
		pBuf->Lock(0, 0, (uint8_t**)(&pVerT1), 0);
#else 
	__VertexT1*	pVerT1 = NULL;
	pVerT1 = pPlug->PMeshInst()->GetVertices();
#endif

	for ( int i = 0; i < iTotalCount; i++ )
	{
		if (pVerT1)
		{
			vec = pVerT1[i];
			vec *= mtx;

			int iX, iZ;			

			A = m_vTVertex[m_pIndex[0]];			
			B = m_vTVertex[m_pIndex[1]];			
			C = m_vTVertex[m_pIndex[2]];
			if (::_IntersectTriangle(vec, vDir, A, B, C, t, u, v, &vPick))
			{
				fx = (vPick.x - C.x) / (SHADOW_PLANE_SIZE*2.0f*m_fShaScale);
				fz = (vPick.z - C.z) / (SHADOW_PLANE_SIZE*2.0f*m_fShaScale);

				iX = (int)(fx*SHADOW_SIZE);
				iZ = (int)(fz*SHADOW_SIZE);

				if ( (iX > 0) && (iX < SHADOW_SIZE) && (iZ > 0) && (iZ < SHADOW_SIZE) )
					m_bitset[iZ].Set(iX);
			}

			A = m_vTVertex[m_pIndex[3]];			
			B = m_vTVertex[m_pIndex[4]];			
			C = m_vTVertex[m_pIndex[5]];
			if (::_IntersectTriangle(vec, vDir, A, B, C, t, u, v, &vPick))
			{
				fx = (vPick.x - B.x) / (SHADOW_PLANE_SIZE*2.0f*m_fShaScale);
				fz = (vPick.z - B.z) / (SHADOW_PLANE_SIZE*2.0f*m_fShaScale);

				iX = (int)(fx*SHADOW_SIZE);
				iZ = (int)(fz*SHADOW_SIZE);

				if ( (iX > 0) && (iX < SHADOW_SIZE) && (iZ > 0) && (iZ < SHADOW_SIZE) )
					m_bitset[iZ].Set(iX);
			}
		}
	}

#ifdef _USE_VERTEXBUFFER
	if (pBuf)
		pBuf->Unlock();
#endif
}

__Vector3	CPlayerBase::Max()
{
	if(m_pShapeExtraRef) 
		return m_pShapeExtraRef->Max();

	return m_Chr.Max(); 
}

__Vector3	CPlayerBase::Min()
{
	if(m_pShapeExtraRef)
		return m_pShapeExtraRef->Min();
	
	return m_Chr.Min();
}

__Vector3	CPlayerBase::Center()
{
	__Vector3 vCenter;

	if(m_pShapeExtraRef)
	{
		vCenter = m_pShapeExtraRef->Min() + (m_pShapeExtraRef->Max()-m_pShapeExtraRef->Min())*0.5f;
		return vCenter;
	}

	return (m_Chr.Min() + (m_Chr.Max()-m_Chr.Min())*0.5f);
}

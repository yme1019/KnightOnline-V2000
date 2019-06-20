#include "stdafx.h"

CPlayerOtherMgr::CPlayerOtherMgr()
{
	m_iChrCountToRender = 0;
}

CPlayerOtherMgr::~CPlayerOtherMgr()
{
	ReleaseUPCs();
	ReleaseNPCs();
	ReleaseCorpses();
}

void CPlayerOtherMgr::ReleaseUPCs()
{
	it_UPC it = m_UPCs.begin(), itEnd = m_UPCs.end();

	for(; it != itEnd; it++) 
		T_Delete(it->second);

	m_UPCs.clear();
}

void CPlayerOtherMgr::ReleaseNPCs()
{
	it_NPC it = m_NPCs.begin(), itEnd = m_NPCs.end();
	for(; it != itEnd; it++) 
		T_Delete(it->second);

	m_NPCs.clear();
}

void CPlayerOtherMgr::ReleaseCorpses()
{
	it_NPC it = m_Corpses.begin(), itEnd = m_Corpses.end();

	for(; it != itEnd; it++) 
		T_Delete(it->second);

	m_Corpses.clear();
}

void CPlayerOtherMgr::Release()
{
	ReleaseUPCs();
	ReleaseNPCs();
	ReleaseCorpses();
}

void CPlayerOtherMgr::Tick(const __Vector3& vPosPlayer)
{
	int iLOD = 0;
	int iLODTotal = 0;

	it_UPC it = m_UPCs.begin(), itEnd = m_UPCs.end();
	CPlayerNPC* pNPC = NULL;
	for(; it != itEnd; )
	{
		pNPC = it->second;
		
		pNPC->Tick();
		iLOD = pNPC->LODLevel();

		if(iLOD >= 0 && iLOD < MAX_CHR_LOD) 
			iLODTotal += MAX_CHR_LOD - iLOD;

		float fDist = pNPC->Distance(vPosPlayer);
		if(fDist < SOUND_RANGE_TO_SET) 
			pNPC->SetSoundAndInitFont(); // SOUND_RANGE
		else 
			if(fDist > SOUND_RANGE_TO_RELEASE) pNPC->ReleaseSoundAndFont();
		it++;
	}

	it_NPC it2 = m_NPCs.begin(), itEnd2 = m_NPCs.end();
	for(; it2 != itEnd2; )
	{
		pNPC = it2->second;

		pNPC->Tick();
		iLOD = pNPC->LODLevel();
		if(iLOD >= 0 && iLOD < MAX_CHR_LOD) iLODTotal += MAX_CHR_LOD - iLOD; 

		if(PSA_DEATH == pNPC->State())
		{
			it2 = m_NPCs.erase(it2);
			this->CorpseAdd(pNPC);
		}
		else if (pNPC->m_InfoBase.eNation==NATION_KARUS || pNPC->m_InfoBase.eNation==NATION_ELMORAD)
		{
			// TODO(srmeier): in 1299 the NPCs seem to have their own distance parameters
			// keeping them the same for now but will probably want to look into this

			// NOTE(srmeier): CPlayerBase::Tick() places a limit to the distance for which the names
			// get displayed

			float fDist = pNPC->Distance(vPosPlayer);
			if (fDist < SOUND_RANGE_TO_SET) pNPC->SetSoundAndInitFont(D3DFONT_BOLD); // SOUND_RANGE
			else if (fDist > SOUND_RANGE_TO_RELEASE) pNPC->ReleaseSoundAndFont();
			it2++;
		}
		else
		{
			float fDist = pNPC->Distance(vPosPlayer);
			if(fDist < SOUND_RANGE_TO_SET) pNPC->SetSoundAndInitFont(D3DFONT_BOLD); // SOUND_RANGE
			else if(fDist > SOUND_RANGE_TO_RELEASE) pNPC->ReleaseSoundAndFont();
			it2++;
		}
	}

	CPlayerBase* pCorpse = NULL;
	it_NPC it3 = m_Corpses.begin(), itEnd3 = m_Corpses.end();
	for(; it3 != itEnd3; )
	{
		pCorpse = it3->second;
		pCorpse->ReleaseSoundAndFont();

		pCorpse->Tick();
		iLOD = pCorpse->LODLevel();
		if (iLOD >= 0 && iLOD < MAX_CHR_LOD)
			iLODTotal += MAX_CHR_LOD - iLOD;

		if(pCorpse->m_fTimeAfterDeath >= TIME_CORPSE_REMAIN) 
		{
			T_Delete(pCorpse);
			it3 = m_Corpses.erase(it3);
		}
		else
		{
			it3++;
		}
	}

	int iLODDelta = 0;

	if(iLODTotal >= 100) 
		iLODDelta = 3;
	else 
		if(iLODTotal >= 60) 
			iLODDelta = 2;
	else 
		if(iLODTotal >= 30) 
			iLODDelta = 1;

	CN3Chr::LODDeltaSet(iLODDelta);
}



void CPlayerOtherMgr::Render(float fSunAngle)
{
	int iUPCSize = m_UPCs.size();
	if(iUPCSize > 0)
	{
		std::vector<CPlayerOther*> UPCs;
		UPCs.reserve(iUPCSize);
		it_UPC it = m_UPCs.begin(), itEnd = m_UPCs.end();
		for(; it != itEnd; it++) UPCs.push_back(it->second);
		qsort(&(UPCs[0]), UPCs.size(), 4, SortByCameraDistance);

		for(int i = iUPCSize - 1; i >= 0; i--)
		{
			if (!UPCs[i]->m_bVisible) continue;
			/*
			if(UPCs[i]->m_InfoBase.iAuthority == AUTHORITY_MANAGER)
				continue;//this_authority
			*/

			UPCs[i]->Render(fSunAngle);
		}
	}

	int iNPCSize = m_NPCs.size();
	if(iNPCSize > 0)
	{
		std::vector<CPlayerNPC*> NPCs;
		NPCs.reserve(iNPCSize);
		it_NPC it = m_NPCs.begin(), itEnd = m_NPCs.end();
		for(; it != itEnd; it++) NPCs.push_back(it->second);
		qsort(&(NPCs[0]), NPCs.size(), 4, SortByCameraDistance);

		for(int i = iNPCSize - 1; i >= 0; i--)
			NPCs[i]->Render(fSunAngle);
	}

	int iCorpseSize = m_Corpses.size();
	if(iCorpseSize > 0)
	{
		std::vector<CPlayerNPC*> Corpses;
		Corpses.reserve(iCorpseSize);
		it_NPC it = m_Corpses.begin(), itEnd = m_Corpses.end();
		for(; it != itEnd; it++) Corpses.push_back(it->second);
		qsort(&(Corpses[0]), iCorpseSize, 4, SortByCameraDistance);

		for(int i = iCorpseSize - 1; i >= 0; i--)
		{
			Corpses[i]->m_InfoBase.bRenderID = false;
			Corpses[i]->Render(fSunAngle);
		}
	}
}

CPlayerNPC*	CPlayerOtherMgr::NPCGetByPos(const __Vector3& vPos)
{
	CPlayerNPC*	pNPC = NULL;
	it_NPC it = m_NPCs.begin(), itEnd = m_NPCs.end();
	for(; it != itEnd; it++)
	{
		pNPC = it->second;

		if(pNPC)
		{
			if(pNPC->m_vPosFromServer.x == vPos.x && pNPC->m_vPosFromServer.z == vPos.z)
				return pNPC;
		}
	}

	return NULL;
}

CPlayerNPC* CPlayerOtherMgr::Pick(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick)
{
	CPlayerNPC* pNPC = this->PickNPC(ixScreen, iyScreen, iIDResult, pvPick);
	if(pNPC) return pNPC;

	CPlayerOther* pUPC = this->PickUPC(ixScreen, iyScreen, iIDResult, pvPick);
	if(pUPC) return pUPC;

	return NULL;
}

CPlayerOther* CPlayerOtherMgr::PickUPC(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick)
{
	iIDResult = -1;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);

	if(!m_UPCs.empty())
	{
		std::vector<CPlayerOther*> UPCs;
		UPCs.reserve(m_UPCs.size());
		it_UPC it = m_UPCs.begin(), itEnd = m_UPCs.end();
		for(; it != itEnd; it++) UPCs.push_back(it->second);
		qsort(&(UPCs[0]), UPCs.size(), 4, SortByCameraDistance);

		for (auto itr = UPCs.begin(); itr != UPCs.end(); ++itr)
		{
			auto pUPC = *itr;
			if(pUPC->LODLevel() < 0 || pUPC->LODLevel() >= MAX_CHR_LOD) 
				continue; // Level Of Detail

			CN3VMesh* pvMesh = pUPC->m_Chr.CollisionMesh();
			if(NULL != pvMesh && pvMesh->Pick(pUPC->m_Chr.m_Matrix, vPos, vDir, pvPick)) 
			{
				iIDResult = pUPC->IDNumber();
				return pUPC;
			}
		}
	}

	return NULL;
}

CPlayerNPC* CPlayerOtherMgr::PickNPC(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick)
{
	iIDResult = -1;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);
	
	if(!m_NPCs.empty())
	{
		std::vector<CPlayerNPC*> NPCs;
		it_NPC it = m_NPCs.begin(), itEnd = m_NPCs.end();
		NPCs.reserve(m_NPCs.size());
		for(; it != itEnd; it++) NPCs.push_back(it->second);
		qsort(&(NPCs[0]), NPCs.size(), 4, SortByCameraDistance);

		for (auto itr = NPCs.begin(); itr != NPCs.end(); ++itr)
		{
			auto pNPC = *itr;
			if(pNPC->LODLevel() < 0 || pNPC->LODLevel() >= MAX_CHR_LOD)
				continue; // Level Of Detail

			CN3VMesh* pvMesh = NULL;
			__Matrix44* pMtx = NULL;
			if(pNPC->m_pShapeExtraRef && pNPC->m_pShapeExtraRef->m_bVisible)
			{
				pvMesh = pNPC->m_pShapeExtraRef->CollisionMesh();
				pMtx = &(pNPC->m_pShapeExtraRef->m_Matrix);
			}
			if(NULL == pvMesh)
			{
				pvMesh = pNPC->m_Chr.CollisionMesh();
				pMtx = &(pNPC->m_Chr.m_Matrix);
			}

			if(NULL == pvMesh) 
				continue;

			bool bPick = pvMesh->Pick(*pMtx, vPos, vDir, pvPick);
			if(bPick)
			{
				iIDResult = pNPC->IDNumber();
				return pNPC;
			}
		}
	}

	return NULL;
}

CPlayerNPC* CPlayerOtherMgr::PickCorpse(int ixScreen, int iyScreen, int& iIDResult)
{
	iIDResult = -1;
	if(m_Corpses.empty()) return NULL;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);

	std::vector<CPlayerNPC*> Corpses;
	Corpses.reserve(m_Corpses.size());
	it_NPC it = m_Corpses.begin(), itEnd = m_Corpses.end();
	for(; it != itEnd; it++) Corpses.push_back(it->second);
	qsort(&(Corpses[0]), Corpses.size(), 4, SortByCameraDistance);

	for (auto itr = Corpses.begin(); itr != Corpses.end(); ++itr)
	{
		auto pCorpse = *itr;
		if(pCorpse->LODLevel() < 0 || pCorpse->LODLevel() >= MAX_CHR_LOD) 
			continue; // Level Of Detail

		CN3VMesh* pvMesh = pCorpse->m_Chr.CollisionMesh();
		if(NULL != pvMesh && pvMesh->Pick(pCorpse->m_Chr.m_Matrix, vPos, vDir)) 
		{
			iIDResult = pCorpse->IDNumber();
			return pCorpse;
		}
	}

	return NULL;
}

bool CPlayerOtherMgr::IsValidCharacter(CPlayerBase *pCharacter)
{
	CPlayerOther*	pUPC = NULL;
	it_UPC it = m_UPCs.begin(), itEnd = m_UPCs.end();
	for(; it != itEnd; it++)
	{
		pUPC = it->second;
		if(pCharacter == pUPC) 
			return true;
	}

	CPlayerNPC*	pNPC = NULL;
	it_NPC it2 = m_NPCs.begin(), itEnd2 = m_NPCs.end();
	for(; it2 != itEnd2; it2++)
	{
		pNPC = it2->second;
		if(pCharacter == pNPC) 
			return true;
	}

	CPlayerBase* pCorpse = NULL;
	it_NPC it3 = m_Corpses.begin(), itEnd3 = m_Corpses.end();
	for(; it3 != itEnd3; it3++)
	{
		pCorpse = it3->second;
		if(pCharacter == pCorpse) return true;
	}

	return false;
}


void CPlayerOtherMgr::CorpseRemove(CPlayerNPC *pCorpse, bool bRemoveImmediately)
{
	if(NULL == pCorpse) return;
	if(pCorpse->m_fTimeAfterDeath >= TIME_CORPSE_REMAIN - TIME_CORPSE_REMOVE) return;

	if(bRemoveImmediately)
		pCorpse->m_fTimeAfterDeath = TIME_CORPSE_REMAIN;
	else
		pCorpse->m_fTimeAfterDeath = TIME_CORPSE_REMAIN - TIME_CORPSE_REMOVE;
}

void CPlayerOtherMgr::CorpseAdd(CPlayerNPC* pNPC)
{
	if(NULL == pNPC) return;
	std::pair<it_NPC, bool> result = m_Corpses.insert(val_NPC(pNPC->IDNumber(), pNPC));
	if(false == result.second)
	{
		T_Delete(result.first->second); 
		result.first->second = pNPC; 
	}
}

CPlayerNPC*	CPlayerOtherMgr::CorpseGetNearstNPC(bool bMustHaveItem, e_Nation eNation, const __Vector3& vPosPlayer)
{
	CPlayerNPC* pTarget = NULL;
	float fDistMin = FLT_MAX, fDistTmp = 0;

	it_NPC it2 = m_NPCs.begin(), itEnd2 = m_NPCs.end();
	for(; it2 != itEnd2; it2++)
	{
		CPlayerNPC* pNPC = it2->second;
		if(eNation == pNPC->m_InfoBase.eNation) continue;
		if(bMustHaveItem && pNPC->m_iDroppedItemID <= 0) continue;
		
		fDistTmp = pNPC->Distance(vPosPlayer);
		if(fDistTmp < fDistMin)
		{
			pTarget = pNPC;
			fDistMin = fDistTmp;
		}
	}

	return pTarget;
}

void CPlayerOtherMgr::MoveToCorpsesForcely(CPlayerNPC* pNPC, bool bErase)
{
	if(NULL == pNPC) return;

	int iID = pNPC->IDNumber();
	pNPC->Action(PSA_DEATH, false, NULL, true); 
	if(bErase) pNPC->m_fTimeAfterDeath = TIME_CORPSE_REMAIN - 10.0f;
	else pNPC->m_fTimeAfterDeath = 0.1f;

	it_UPC it = m_UPCs.find(iID); // User를 찾아보고...
	if(it != m_UPCs.end())
	{
		if( bErase )
		{
			CPlayerOther* pUPC = it->second;
			this->CorpseAdd(pUPC); 
			m_UPCs.erase(it); 
		}

//		CPlayerOther* pUPC = it->second;
//		this->CorpseAdd(pUPC);
//		m_UPCs.erase(it);
	}
	else
	{
		it_NPC it2 = m_NPCs.find(iID);
		if(it2 != m_NPCs.end())
		{
			CPlayerNPC* pNPC = it2->second; 
			this->CorpseAdd(pNPC);
			m_NPCs.erase(it2);
		}
	}
}

CPlayerNPC*	CPlayerOtherMgr::CharacterGetByNearstEnemy(e_Nation eNation, const __Vector3& vPosPlayer) // 가장 가까운 적 가져오기..
{
	CPlayerNPC* pTarget = NULL;
	float fDistMin = FLT_MAX, fDistTmp = 0;

	it_UPC it = m_UPCs.begin(), itEnd = m_UPCs.end();
	CPlayerNPC* pNPC = NULL;
	for(; it != itEnd; it++)
	{
		pNPC = it->second;
		if(eNation == pNPC->m_InfoBase.eNation) 
			continue;

		fDistTmp = pNPC->Distance(vPosPlayer);
		if(fDistTmp < fDistMin)
		{
			pTarget = pNPC;
			fDistMin = fDistTmp;
		}
	}

	it_NPC it2 = m_NPCs.begin(), itEnd2 = m_NPCs.end();
	for(; it2 != itEnd2; it2++)
	{
		pNPC = it2->second;
		if(eNation == pNPC->m_InfoBase.eNation) 
			continue;

		fDistTmp = pNPC->Distance(vPosPlayer);
		if(fDistTmp < fDistMin)
		{
			pTarget = pNPC;
			fDistMin = fDistTmp;
		}
	}

	return pTarget;
}

bool CPlayerOtherMgr::CharacterDelete(int iID) // User, NPC
{
	it_UPC it = m_UPCs.find(iID); // User�
	if(it != m_UPCs.end())
	{
		CPlayerOther* pUPC = it->second;
		delete pUPC;
		m_UPCs.erase(it); 
		return true;
	}

	it_NPC it2 = m_NPCs.find(iID);
	if(it2 != m_NPCs.end())
	{
		CPlayerNPC* pNPC = it2->second; 
		delete pNPC;
		m_NPCs.erase(it2); 
		return true;
	}

	return false;
}


int CPlayerOtherMgr::SortByCameraDistance(const void* pArg1, const void* pArg2)
{
	CPlayerBase* pPlayer1 = *((CPlayerBase**)pArg1);
	CPlayerBase* pPlayer2 = *((CPlayerBase**)pArg2);

	float fDist1 = 0.0f, fDist2 = 0.0f;
	
	if(pPlayer1) fDist1 = (CN3Base::s_CameraData.vEye - pPlayer1->Position()).Magnitude();
	if(pPlayer2) fDist2 = (CN3Base::s_CameraData.vEye - pPlayer2->Position()).Magnitude();

	if(fDist1 < fDist2) return -1;
	else if(fDist1 > fDist2) return 1;
	else return 0;
}

void CPlayerOtherMgr::CorpseAdd(int iID)
{
	it_UPC it = m_UPCs.find(iID); // User
	if(it != m_UPCs.end())
	{
		CPlayerOther* pUPC = it->second;

		pUPC->Action(PSA_DEATH, false, NULL, true);
		pUPC->m_fTimeAfterDeath = TIME_CORPSE_REMAIN - 10.0f;

		this->CorpseAdd(pUPC); 
		m_UPCs.erase(it);
	}
}

CPlayerNPC* CPlayerOtherMgr::PickAllPrecisely(int ixScreen, int iyScreen, int &iIDResult, __Vector3* pvPick)
{
	iIDResult = -1;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);


	CPlayerNPC* pNPC = NULL;

	std::vector<CPlayerNPC*> NPCs;
	std::vector<CPlayerNPC*> NUPCBufs;
	if(!m_NPCs.empty() || !m_UPCs.empty())
	{
		it_NPC it = m_NPCs.begin(), itEnd = m_NPCs.end();
		it_UPC it_u = m_UPCs.begin(), itEnd_u = m_UPCs.end();
		NPCs.reserve(m_NPCs.size()+m_UPCs.size());
		for(; it != itEnd; it++) NPCs.push_back(it->second);
		for(; it_u != itEnd_u; it_u++) NPCs.push_back(it_u->second);
		qsort(&(NPCs[0]), NPCs.size(), 4, SortByCameraDistance);
	}
	else
	{
		return NULL;
	}


	for (auto itr = NPCs.begin(); itr != NPCs.end(); ++itr)
	{
		pNPC = *itr;
		if(pNPC == NULL) continue;
		if(pNPC->LODLevel() < 0 || pNPC->LODLevel() >= MAX_CHR_LOD) 
			continue; // Level Of Detail

		CN3VMesh* pvMesh = NULL;
		__Matrix44* pMtx = NULL;
		if(pNPC->m_pShapeExtraRef && pNPC->m_pShapeExtraRef->m_bVisible)
		{
			pvMesh = pNPC->m_pShapeExtraRef->CollisionMesh();
			pMtx = &(pNPC->m_pShapeExtraRef->m_Matrix);
			if(NULL == pvMesh)
			{
				pvMesh = pNPC->m_Chr.CollisionMesh();
				pMtx = &(pNPC->m_Chr.m_Matrix);
			}

			if(NULL == pvMesh) 
				continue;
			bool bPick = pvMesh->Pick(*pMtx, vPos, vDir, pvPick);
			if(bPick)
			{
				NUPCBufs.push_back(pNPC);
			}
			continue;
		}
		else
		{
			pvMesh = pNPC->m_Chr.CollisionMesh();
			pMtx = &(pNPC->m_Chr.m_Matrix);

			if(NULL == pvMesh) 
				continue;
			bool bPick = pvMesh->Pick(*pMtx, vPos, vDir, pvPick);
			if(bPick)
			{
				NUPCBufs.push_back(pNPC);
			}
			continue;
		}
	}

	if(NUPCBufs.size() == 1)
	{
		pNPC = NUPCBufs[0];
		if(pNPC)
		{
			iIDResult = pNPC->IDNumber();
			return pNPC;
		}
	}

	for (auto itr = NUPCBufs.begin(); itr != NUPCBufs.end(); ++itr)
	{
		pNPC = *itr;
		if(pNPC->LODLevel() < 0 || pNPC->LODLevel() >= MAX_CHR_LOD) continue; // Level Of Detail

		CN3VMesh* pvMesh = NULL;
		__Matrix44* pMtx = NULL;
		if(pNPC->m_pShapeExtraRef && pNPC->m_pShapeExtraRef->m_bVisible)
		{
			iIDResult = pNPC->IDNumber();
			return pNPC;
		}

		if(pNPC->m_Chr.CheckCollisionPrecisely(ixScreen ,iyScreen, pvPick) != -1)
		{
			iIDResult = pNPC->IDNumber();
			return pNPC;
		}
	}

	return NULL;
}

CPlayerNPC* CPlayerOtherMgr::PickNPCPrecisely(int ixScreen, int iyScreen, int &iIDResult, __Vector3* pvPick)
{
	iIDResult = -1;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);
	
	if(!m_NPCs.empty())
	{
		std::vector<CPlayerNPC*> NPCs;
		it_NPC it = m_NPCs.begin(), itEnd = m_NPCs.end();
		NPCs.reserve(m_NPCs.size());
		for(; it != itEnd; it++) NPCs.push_back(it->second);
		qsort(&(NPCs[0]), NPCs.size(), 4, SortByCameraDistance);

		CPlayerNPC* pNPC = NULL;
		for (auto itr = NPCs.begin(); itr != NPCs.end(); ++itr)
		{
			pNPC = *itr;
			if(pNPC->LODLevel() < 0 || pNPC->LODLevel() >= MAX_CHR_LOD) continue; // Level Of Detail

			CN3VMesh* pvMesh = NULL;
			__Matrix44* pMtx = NULL;
			if(pNPC->m_pShapeExtraRef && pNPC->m_pShapeExtraRef->m_bVisible)
			{
				pvMesh = pNPC->m_pShapeExtraRef->CollisionMesh();
				pMtx = &(pNPC->m_pShapeExtraRef->m_Matrix);
				if(NULL == pvMesh)
				{
					pvMesh = pNPC->m_Chr.CollisionMesh();
					pMtx = &(pNPC->m_Chr.m_Matrix);
				}

				if(NULL == pvMesh) continue;
				bool bPick = pvMesh->Pick(*pMtx, vPos, vDir, pvPick);
				if(bPick)
				{
					iIDResult = pNPC->IDNumber();
					return pNPC;
				}
				continue;
			}

			if(pNPC->m_Chr.CheckCollisionPrecisely(ixScreen ,iyScreen, pvPick) != -1)
			{
				iIDResult = pNPC->IDNumber();
				return pNPC;
			}
		}
	}

	return NULL;
}

CPlayerOther* CPlayerOtherMgr::PickUPCPrecisely(int ixScreen, int iyScreen, int &iIDResult, __Vector3* pvPick)
{
	iIDResult = -1;

	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(ixScreen, iyScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);

	if(!m_UPCs.empty())
	{
		std::vector<CPlayerOther*> UPCs;
		UPCs.reserve(m_UPCs.size());
		it_UPC it = m_UPCs.begin(), itEnd = m_UPCs.end();
		for(; it != itEnd; it++) UPCs.push_back(it->second);
		qsort(&(UPCs[0]), UPCs.size(), 4, SortByCameraDistance);

		CPlayerOther*	pUPC = NULL;
		for (auto itr = UPCs.begin(); itr != UPCs.end(); ++itr)
		{
			pUPC = *itr;
			if(pUPC->LODLevel() < 0 || pUPC->LODLevel() >= MAX_CHR_LOD) continue; // Level Of Detail

			if(pUPC->m_Chr.CheckCollisionPrecisely(ixScreen ,iyScreen, pvPick) != -1)
			{
				iIDResult = pUPC->IDNumber();
				return pUPC;
			}
		}
	}

	return NULL;
}

CPlayerNPC* CPlayerOtherMgr::PickPrecisely(int ixScreen, int iyScreen, int& iIDResult, __Vector3* pvPick)
{
	CPlayerNPC* pNPC = this->PickAllPrecisely(ixScreen, iyScreen, iIDResult, pvPick);
	if(pNPC) return pNPC;

	return NULL;
}

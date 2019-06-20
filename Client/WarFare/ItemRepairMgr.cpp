#include "resource.h"
#include "ItemRepairMgr.h"

#include "PacketDef.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "UIInventory.h"
#include "GameProcMain.h"
#include "PlayerMySelf.h"

#include "UIRepairTooltipDlg.h"

#include "N3UIString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CItemRepairMgr::CItemRepairMgr()
{
	m_pspItemBack = NULL;	
}

CItemRepairMgr::~CItemRepairMgr()
{

}

void CItemRepairMgr::Tick()
{
	CUIInventory *pInv	= CGameProcedure::s_pProcMain->m_pUIInventory;
	if (!pInv) return;
	if (!pInv->IsVisible())	return;
	if (pInv->GetInvState() != INV_STATE_REPAIR) return;
	if (CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer) return;

	CUIRepairTooltipDlg *pDlg = CGameProcedure::s_pProcMain->m_pUIRepairTooltip;
	if (pDlg)	pDlg->m_bBRender = false;

	POINT ptCur			= CGameProcedure::s_pLocalInput->MouseGetPos();

	// 위치를 구해서 
	int i;	int iArm = 0x00; int iOrder = -1; __IconItemSkill* spItem = NULL;
	for (i = 0; i < ITEM_SLOT_COUNT; i++)
	{
		if (spItem) break;
		if (pInv->m_pMySlot[i])
		{
			if (pInv->m_pMySlot[i]->pUIIcon->IsIn(ptCur.x, ptCur.y))
			{
				iArm = 0x01;
				spItem = pInv->m_pMySlot[i];
				iOrder = i;
			}
		}
	}

	if (!spItem)
	{
		for (i = 0; i < MAX_ITEM_INVENTORY; i++)
		{
			if (spItem) break;
			if (pInv->m_pMyInvWnd[i])
			{
				if (pInv->m_pMyInvWnd[i]->pUIIcon->IsIn(ptCur.x, ptCur.y))
				{
					iArm = 0x02;
					spItem = pInv->m_pMyInvWnd[i];
					iOrder = i;
				}
			}
		}
	}

	int iRepairGold = 0;
	if (spItem)
	{
		iRepairGold = CalcRepairGold(spItem);
		if (pDlg)
		{
			pDlg->m_bBRender		= true;
			pDlg->m_iBxpos			= ptCur.x;
			pDlg->m_iBypos			= ptCur.y;
			pDlg->m_pBspItem		= spItem;
			pDlg->m_iBRequiredGold	= iRepairGold;
		}

		if (iRepairGold > s_pPlayer->m_InfoExt.iGold)
		{
			if (pDlg)
				pDlg->m_bBHaveEnough = false;
		}
		else
		{
			if (pDlg)
				pDlg->m_bBHaveEnough = true;
		}
	}

	uint32_t dwMouseFlags	= CGameProcedure::s_pLocalInput->MouseGetFlag();
	if (dwMouseFlags & MOUSE_LBCLICK)	
	{
		m_pspItemBack	= spItem;
		m_iArm			= iArm;
		m_iiOrder		= iOrder;
	}
	else if(dwMouseFlags & MOUSE_LBCLICKED)
	{
		if (m_pspItemBack && spItem && (m_pspItemBack == spItem) )
		{
			// Send To Server..
			if (iRepairGold > 0)
			{
				if (iRepairGold > s_pPlayer->m_InfoExt.iGold)
				{
					std::string szMsg; ::_LoadStringFromResource(IDS_REPAIR_LACK_GOLD, szMsg);
					CGameProcedure::s_pProcMain->MsgOutput(szMsg, 0xffff00ff);
				}
				else
				{
					uint8_t byBuff[8];															
					int iOffset=0;															

					CAPISocket::MP_AddByte(byBuff, iOffset,  WIZ_ITEM_REPAIR);			
					CAPISocket::MP_AddByte(byBuff, iOffset,  iArm);							
					CAPISocket::MP_AddByte(byBuff, iOffset,  iOrder);							
					CAPISocket::MP_AddDword(byBuff, iOffset, spItem->pItemBasic->dwID+spItem->pItemExt->ItemIndex);

					CGameProcedure::s_pSocket->Send(byBuff, iOffset);	
					CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer = true;
					// Change To Cursor..
					CGameProcedure::SetGameCursor(CGameProcedure::s_hCursorNowRepair, true);
				}
			}
		}
	}
}

void CItemRepairMgr::ReceiveResultFromServer(int iResult, int iUserGold)
{
	CUIRepairTooltipDlg *pDlg = CGameProcedure::s_pProcMain->m_pUIRepairTooltip;
	CUIInventory *pInv	= CGameProcedure::s_pProcMain->m_pUIInventory;

	if (!pInv)
		return;
	if (!m_pspItemBack) 
		return;

	if(iResult == 0x01)
	{
		m_pspItemBack->iDurability = m_pspItemBack->pItemBasic->siMaxDurability+m_pspItemBack->pItemExt->siMaxDurability;

		switch (m_iArm)
		{
			case 0x01:
				pInv->m_pMySlot[m_iiOrder] = m_pspItemBack;
				s_pPlayer->DurabilitySet((e_ItemSlot)m_iiOrder, m_pspItemBack->iDurability);
				break;

			case 0x02:
				pInv->m_pMyInvWnd[m_iiOrder] = m_pspItemBack;
				break;
		}

		m_pspItemBack->pUIIcon->SetStyle(m_pspItemBack->pUIIcon->GetStyle() & (~UISTYLE_DURABILITY_EXHAUST));
		
		if (pDlg)	pDlg->m_iBRequiredGold	= 0;
		pInv->PlayRepairSound();
	}

	UpdateUserTotalGold(iUserGold);
	CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer = false;
	// Change To Cursor..
	CGameProcedure::SetGameCursor(CGameProcedure::s_hCursorPreRepair, true);
}

void CItemRepairMgr::UpdateUserTotalGold(int iGold)
{
	char szGold[32];
	s_pPlayer->m_InfoExt.iGold = iGold;
	sprintf(szGold, "%d", iGold);
	CGameProcedure::s_pProcMain->m_pUIInventory->GoldUpdate();	
}

int CItemRepairMgr::CalcRepairGold(__IconItemSkill* spItem)
{
	int iMaxDurability = spItem->pItemBasic->siMaxDurability + spItem->pItemExt->siMaxDurability;
	if (iMaxDurability <= 0) return 0;
	float fAllPrice = (float)spItem->pItemBasic->iPrice*(float)spItem->pItemExt->siPriceMultiply;
	float fTemp = ((fAllPrice-10.0f)/(10000.0f))+pow(fAllPrice, 0.75f);
	float fValue = fTemp*( (float)(iMaxDurability - spItem->iDurability) / (float)(iMaxDurability) );

	return (int)fValue;
}

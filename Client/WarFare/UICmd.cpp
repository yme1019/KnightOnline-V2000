#include "resource.h"
#include "UICmd.h"
#include "GameProcMain.h"
#include "PlayerOtherMgr.h"
#include "PlayerMyself.h"
#include "UITransactionDlg.h"
#include "N3UIButton.h"
#include "UIManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CUICmd::CUICmd()
{
	m_pBtn_Exit = NULL;			

	m_pBtn_Act = NULL;			
	m_pBtn_Act_Walk = NULL;	
	m_pBtn_Act_Run = NULL;		
	m_pBtn_Act_Stop = NULL;	
	m_pBtn_Act_Attack = NULL;	
	
	m_pBtn_Act_StandUp = NULL; 
	m_pBtn_Act_SitDown = NULL;	

	m_pBtn_Camera = NULL;			
	m_pBtn_Inventory = NULL;		
	m_pBtn_Party_Invite = NULL;	
	m_pBtn_Party_Disband = NULL;	
	m_pBtn_CmdList = NULL;			
	m_pBtn_Quest = NULL;			
	m_pBtn_Character = NULL;		
	m_pBtn_Skill = NULL;			
	m_pBtn_Belong = NULL;			
	m_pBtn_Map = NULL;
}

CUICmd::~CUICmd()
{
	DestroyTooltip();
}

bool CUICmd::Load(HANDLE hFile)
{
	if(CN3UIBase::Load(hFile)==false) return false;

	m_pBtn_Act =			GetChildByID("btn_control");	//__ASSERT(m_pBtn_Act, "NULL UI Component!!");
	m_pBtn_Act_Walk =		GetChildByID("btn_walk");		//__ASSERT(m_pBtn_Act_Walk, "NULL UI Component!!");
	m_pBtn_Act_Run =		GetChildByID("btn_run");		//__ASSERT(m_pBtn_Act_Run, "NULL UI Component!!");
	m_pBtn_Act_Stop =		GetChildByID("btn_stop");		//__ASSERT(m_pBtn_Act_Stop, "NULL UI Component!!");
	m_pBtn_Act_Attack =		GetChildByID("btn_attack");		//__ASSERT(m_pBtn_Act_Attack, "NULL UI Component!!");
	m_pBtn_Act_StandUp =	GetChildByID("btn_stand");		//__ASSERT(m_pBtn_Act_StandUp, "NULL UI Component!!");
	m_pBtn_Act_SitDown =	GetChildByID("btn_sit");		//__ASSERT(m_pBtn_Act_SitDown, "NULL UI Component!!");

	if(m_pBtn_Act_StandUp) m_pBtn_Act_StandUp->SetVisible(false); // 일어서기 버튼은 미리 죽여놓는다..
	
	m_pBtn_Character =		GetChildByID("btn_character");	//__ASSERT(m_pBtn_Character, "NULL UI Component!!");
	m_pBtn_Inventory =		GetChildByID("btn_inventory");	//__ASSERT(m_pBtn_Inventory, "NULL UI Component!!");

	m_pBtn_CmdList =			GetChildByID("btn_option");		//__ASSERT(m_pBtn_Option, "NULL UI Component!!");
	m_pBtn_Camera =			GetChildByID("btn_camera");		//__ASSERT(m_pBtn_Camera, "NULL UI Component!!");
	m_pBtn_Party_Invite =	GetChildByID("btn_invite");		//__ASSERT(m_pBtn_Party_Invite, "NULL UI Component!!");
	m_pBtn_Party_Disband =	GetChildByID("btn_disband");	//__ASSERT(m_pBtn_Party_Disband, "NULL UI Component!!");
	m_pBtn_Skill =			GetChildByID("btn_skill");		//__ASSERT(m_pBtn_Skill, "NULL UI Component!!");
	m_pBtn_Exit	=			GetChildByID("btn_Exit");		//__ASSERT(m_pBtn_Exit, "NULL UI Component!!");

	m_pBtn_Quest =		GetChildByID("btn_quest");			//__ASSERT(m_pBtn_Quest, "NULL UI Component!!");
	m_pBtn_Belong =		GetChildByID("btn_knight");			//__ASSERT(m_pBtn_Belong, "NULL UI Component!!");

	m_pBtn_Map =		GetChildByID("btn_map");			//__ASSERT(m_pBtn_Map, "NULL UI Component!!");

//	this->SetVisibleActButtons(true);
//	this->SetVisibleOptButtons(false);

	return true;
}

bool CUICmd::ReceiveMessage(CN3UIBase* pSender, uint32_t dwMsg)
{
	if ( CGameProcedure::s_pProcMain->m_pUITransactionDlg->IsVisible() )	
			return true;

	if (dwMsg == UIMSG_BUTTON_CLICK)					
	{
		if(pSender == m_pBtn_CmdList)
		{
			CGameProcedure::s_pProcMain->CommandToggleCmdList();
		}

		if(pSender == m_pBtn_Act)
		{
//			this->SetVisibleActButtons(true);
//			this->SetVisibleOptButtons(false);
		}

		else if(pSender == m_pBtn_Act_Walk)
		{
			CGameProcedure::s_pProcMain->CommandToggleWalkRun();
		}
			
		else if(pSender == m_pBtn_Act_Run)
		{
			CGameProcedure::s_pProcMain->CommandToggleWalkRun();
		}

		else if(pSender == m_pBtn_Act_Attack)
		{
			CGameProcedure::s_pProcMain->CommandToggleAttackContinous();
		}

		else if(pSender == m_pBtn_Inventory)
		{
			CGameProcedure::s_pProcMain->CommandToggleUIInventory();
		}
		
		else if(pSender == m_pBtn_Character)
		{
			CGameProcedure::s_pProcMain->CommandToggleUIState();
		}

		else if(pSender == m_pBtn_Exit) 
		{
			std::string szMsg = "Are you sure you want to exit?"; //Once again lack of descrpiton(IDS_EXIT, szMsg);
			CGameProcedure::MessageBoxPost(szMsg, "", MB_YESNO, BEHAVIOR_RESTART_GAME); 
			CGameProcedure::MessageBoxPost(szMsg, "", MB_YESNO, BEHAVIOR_EXIT); 
		}

		else if(pSender == m_pBtn_Camera)
		{
			CGameProcedure::s_pProcMain->CommandCameraChange(); 
		}

		else if(pSender == m_pBtn_Party_Invite)
		{
			CPlayerOther* pUPC = CGameProcedure::s_pOPMgr->UPCGetByID(CGameProcedure::s_pPlayer->m_iIDTarget, true);
			if(pUPC && pUPC->m_InfoBase.eNation == CGameProcedure::s_pPlayer->m_InfoBase.eNation) 
			{
				CGameProcedure::s_pProcMain->MsgSend_PartyOrForceCreate(0, pUPC->IDString()); 
			}
		}

		else if(pSender == m_pBtn_Party_Disband)
		{
			CGameProcMain* pMain = CGameProcedure::s_pProcMain;
			CPlayerMySelf* pPlayer = CGameBase::s_pPlayer;

			bool bIAmLeader = false, bIAmMemberOfParty = false;
			int iMemberIndex = -1;
			CPlayerBase* pTarget = NULL;
			pMain->PartyOrForceConditionGet(bIAmLeader, bIAmMemberOfParty, iMemberIndex, pTarget);
			
			std::string szMsg;
			if(bIAmLeader) 
			{
				if(iMemberIndex > 0)
				{
					::_LoadStringFromResource(IDS_PARTY_CONFIRM_DISCHARGE, szMsg);
					szMsg = pTarget->IDString() + szMsg;
				}
				else ::_LoadStringFromResource(IDS_PARTY_CONFIRM_DESTROY, szMsg);
			}
			else if(bIAmMemberOfParty)
			{
				::_LoadStringFromResource(IDS_PARTY_CONFIRM_LEAVE, szMsg);
			}

			if(!szMsg.empty()) CGameProcedure::MessageBoxPost(szMsg, "", MB_YESNO, BEHAVIOR_PARTY_DISBAND); 
		}

		else if(pSender == m_pBtn_Act_SitDown)
		{
			CGameProcedure::s_pProcMain->CommandSitDown(true, true);
		}
		
		else if(pSender == m_pBtn_Act_StandUp)
		{
			CGameProcedure::s_pProcMain->CommandSitDown(true, false);
		}

		else if(pSender == m_pBtn_Skill)
		{
			CGameProcedure::s_pProcMain->CommandToggleUISkillTree();
		}

		else if(pSender == m_pBtn_Map)
		{
			CGameProcedure::s_pProcMain->CommandToggleUIMiniMap();
		}
	}

	return true;
}

/*
void CUICmd::SetVisibleActButtons(bool bVisible)
{
	//행동
	if(m_pBtn_Act_Walk) m_pBtn_Act_Walk->SetVisible(bVisible); 
	if(m_pBtn_Act_Run) m_pBtn_Act_Run->SetVisible(bVisible);
	if(m_pBtn_Act_Stop) m_pBtn_Act_Stop->SetVisible(bVisible);
	if(m_pBtn_Act_StandUp) m_pBtn_Act_StandUp->SetVisible(bVisible);
	if(m_pBtn_Act_SitDown) m_pBtn_Act_SitDown->SetVisible(bVisible);
	if(m_pBtn_Act_Attack) m_pBtn_Act_Attack->SetVisible(bVisible);
}
*/

/*
void CUICmd::SetVisibleOptButtons(bool bVisible)
{
	//옵션
	if(m_pBtn_Opt_Quest) m_pBtn_Opt_Quest->SetVisible(bVisible);
	if(m_pBtn_Character) m_pBtn_Character->SetVisible(bVisible);	
	if(m_pBtn_Skill) m_pBtn_Skill->SetVisible(bVisible);
	if(m_pBtn_Opt_Knight) m_pBtn_Opt_Knight->SetVisible(bVisible);
	if(m_pBtn_Inventory) m_pBtn_Inventory->SetVisible(bVisible);	
}
*/

void CUICmd::UpdatePartyButtons(bool bIAmLeader, bool bIAmMemberOfParty, int iMemberIndex, const CPlayerBase* pTarget)
{
	bool bInvite = true;
	if(bIAmLeader) 
	{
		if(pTarget) 
		{
			if(iMemberIndex > 0) bInvite = false; 
			else bInvite = true;
		}
		else
		{
			bInvite = false; 
		}
	}
	else
	{
		if(bIAmMemberOfParty) bInvite = false; 
		else bInvite = true; 
	}

	if(m_pBtn_Party_Invite) m_pBtn_Party_Invite->SetVisible(bInvite);
	if(m_pBtn_Party_Disband) m_pBtn_Party_Disband->SetVisible(!bInvite);
}

bool CUICmd::OnKeyPress(int iKey)
{
	switch(iKey)
	{
	case SDL_SCANCODE_ESCAPE://DIK_ESCAPE:
		{	
			CGameProcedure::s_pUIMgr->ReFocusUI();//this_ui
			CN3UIBase* pFocus = CGameProcedure::s_pUIMgr->GetFocusedUI();
			if(pFocus && pFocus != this) pFocus->OnKeyPress(iKey);
		}
		return true;
	}

	return CN3UIBase::OnKeyPress(iKey);
}

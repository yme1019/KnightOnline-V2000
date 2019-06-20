#if !defined(AFX_N3GAMEBASE_H__5F0720A7_AFF6_4AB5_B469_701AA2935158__INCLUDED_)
#define AFX_N3GAMEBASE_H__5F0720A7_AFF6_4AB5_B469_701AA2935158__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Base.h"
#include "GameDef.h"
#include "N3TableBase.h"

class CGameBase : public CN3Base
{
#define ACT_WORLD s_pWorldMgr->GetActiveWorld()

public:
	static bool s_bRunning;

	static CN3TableBase<_TBL_TABLE_TEXTS>			file_Tbl_Texts;
	static CN3TableBase<_TBL_TABLE_ZONE>			file_Tbl_Zones;
	static CN3TableBase<_TBL_TABLE_UI>				file_Tbl_UI;
	static CN3TableBase<__TABLE_ITEM_BASIC>			s_pTbl_Items_Basic;		
	static CN3TableBase<__TABLE_ITEM_EXT>			s_pTbl_Items_Exts[MAX_ITEM_EXTENSION];
	static CN3TableBase<__TABLE_PLAYER_LOOKS>		s_pTbl_UPC_Looks;		
	static CN3TableBase<__TABLE_PLAYER_LOOKS>		s_pTbl_NPC_Looks;		
	static CN3TableBase<_TB_TABLE_UPC_SKILL>		file_Tbl_Skill;
	static CN3TableBase<__TABLE_EXCHANGE_QUEST>		s_pTbl_Exchange_Quest;	
	static CN3TableBase<_TBL_TABLE_FX>				s_pTbl_FXSource;
	static CN3TableBase<__TABLE_QUEST_MENU>			s_pTbl_QuestMenu;		
	static CN3TableBase<_TBL_TABLE_QUEST_TALK>		file_Tbl_QuestTalk;

	static class CN3WorldManager*		s_pWorldMgr;							
	static class CPlayerOtherMgr*		s_pOPMgr;									
	static class CPlayerMySelf*			s_pPlayer;			
	
protected:
	static void StaticMemberInit(); 
	static void StaticMemberRelease();

public:
	static bool	GetTextByAttrib(e_ItemAttrib eAttrib, std::string& szAttrib);
	static bool GetTextByClass(e_Class eClass, std::string& szText);
	static bool GetTextByItemClass(e_ItemClass eItemClass, std::string& szText);
	static bool GetTextByKnightsDuty(e_KnightsDuty eDuty, std::string& szText);
	static bool GetTextByNation(e_Nation eNation, std::string& szText);
	static bool GetTextByRace(e_Race eRace, std::string& szText);
	static D3DCOLOR				GetIDColorByLevelDifference(int iLevelDiff); // 레벨 차이에 따른 ID 색 돌려준다.
	static e_Class_Represent	GetRepresentClass(e_Class eClass); // 세부직업을 넣어주면 대표되는 직업을 돌려준다.
	static e_ItemType MakeResrcFileNameForUPC(	__TABLE_ITEM_BASIC* pItem,		// 아이템 데이터...
												std::string* szResrcFN,			// Resource FileName
												std::string* szIconFN,			// Icon FileName
												e_PartPosition& ePartPosition,	// Part 일경우 Index
												e_PlugPosition& ePlugPosition,  // Plug 일경우 Index
												e_Race eRace = RACE_UNKNOWN);

	class CPlayerBase*	CharacterGetByID(int iID, bool bFromAlive);
	bool				IsValidCharacter(CPlayerBase* pCharacter);
	
	CGameBase();
	virtual ~CGameBase();
};

#endif // !defined(AFX_N3GAMEBASE_H__5F0720A7_AFF6_4AB5_B469_701AA2935158__INCLUDED_)

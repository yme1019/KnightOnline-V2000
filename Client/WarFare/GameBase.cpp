#include "stdafx.h"

bool CGameBase::s_bRunning = false;

CN3TableBase<_TBL_TABLE_TEXTS>			CGameBase::file_Tbl_Texts;
CN3TableBase<_TBL_TABLE_ZONE>			CGameBase::file_Tbl_Zones;
CN3TableBase<_TBL_TABLE_UI>				CGameBase::file_Tbl_UI;
CN3TableBase<__TABLE_ITEM_BASIC>		CGameBase::s_pTbl_Items_Basic;
CN3TableBase<__TABLE_ITEM_EXT>			CGameBase::s_pTbl_Items_Exts[MAX_ITEM_EXTENSION];
CN3TableBase<__TABLE_PLAYER_LOOKS>		CGameBase::s_pTbl_UPC_Looks;
CN3TableBase<__TABLE_PLAYER_LOOKS>		CGameBase::s_pTbl_NPC_Looks;
CN3TableBase<_TB_TABLE_UPC_SKILL>		CGameBase::file_Tbl_Skill;
CN3TableBase<__TABLE_EXCHANGE_QUEST>	CGameBase::s_pTbl_Exchange_Quest;
CN3TableBase<_TBL_TABLE_FX>				CGameBase::s_pTbl_FXSource;
CN3TableBase<__TABLE_QUEST_MENU>		CGameBase::s_pTbl_QuestMenu;
CN3TableBase<_TBL_TABLE_QUEST_TALK>		CGameBase::file_Tbl_QuestTalk;


CN3WorldManager*	CGameBase::s_pWorldMgr = NULL;
CPlayerOtherMgr*	CGameBase::s_pOPMgr = NULL;
CPlayerMySelf*		CGameBase::s_pPlayer = NULL;
	
CGameBase::CGameBase()
{
}

CGameBase::~CGameBase()
{
}

void _LoadStringFromResource(uint32_t dwID, std::string& szText)
{
	auto pText = CGameBase::file_Tbl_Texts.Find(dwID);
	if (pText != nullptr)
		szText = pText->szText;
}

void CGameBase::StaticMemberInit()
{
	std::string szLangTail = "_us.tbl";
	int iLangID = ::GetUserDefaultLangID();
	if(0x0404 == iLangID) szLangTail = "_tr.tbl"; // Taiwan Language

	std::string szFN;
	szFN = "Data\\Texts" + szLangTail;		file_Tbl_Texts.LoadFromFile(szFN.c_str());
	szFN = "Data\\Zones.tbl";				file_Tbl_Zones.LoadFromFile(szFN.c_str());
	szFN = "Data\\UIs" + szLangTail;		file_Tbl_UI.LoadFromFile(szFN.c_str());
	szFN = "Data\\UPC_DefaultLooks.tbl";	s_pTbl_UPC_Looks.LoadFromFile(szFN.c_str());	
	szFN = "Data\\Item_Org" + szLangTail;	s_pTbl_Items_Basic.LoadFromFile(szFN.c_str());

	szFN = "Data\\Quest_Menu" + szLangTail;	s_pTbl_QuestMenu.LoadFromFile(szFN.c_str());	
	szFN = "Data\\Quest_Talk" + szLangTail;	file_Tbl_QuestTalk.LoadFromFile(szFN.c_str());

	for(int i = 0; i < MAX_ITEM_EXTENSION; i++)
	{
		char szFNTmp[256] = "";
		sprintf(szFNTmp, "Data\\Item_Ext_%d", i);
		szFN = szFNTmp + szLangTail;
		s_pTbl_Items_Exts[i].LoadFromFile(szFN.c_str());
	}

	szFN = "Data\\NPC_Looks.tbl";					s_pTbl_NPC_Looks.LoadFromFile(szFN.c_str());
	szFN = "Data\\skill_magic_main" + szLangTail;	file_Tbl_Skill.LoadFromFile(szFN.c_str());
	szFN = "Data\\Exchange_Quest.tbl";				s_pTbl_Exchange_Quest.LoadFromFile(szFN.c_str());
	szFN = "Data\\fx.tbl";							s_pTbl_FXSource.LoadFromFile(szFN.c_str());

	s_pWorldMgr = new CN3WorldManager();
	s_pOPMgr = new CPlayerOtherMgr();
	s_pPlayer = new CPlayerMySelf();
}

void CGameBase::StaticMemberRelease()
{
	delete s_pPlayer;	
	s_pPlayer = NULL;

	delete s_pOPMgr;	
	s_pOPMgr = NULL;

	delete s_pWorldMgr;	
	s_pWorldMgr = NULL;
}

bool CGameBase::GetTextByClass(e_Class eClass, std::string& szText)
{
	switch(eClass)
	{
		case CLASS_KINDOF_WARRIOR:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_WARRIOR, szText);
			break;
		case CLASS_KINDOF_ROGUE:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_ROGUE, szText);
			break;
		case CLASS_KINDOF_WIZARD:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_WIZARD, szText);
			break;
		case CLASS_KINDOF_PRIEST:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_PRIEST, szText);
			break;
		case CLASS_KINDOF_ATTACK_WARRIOR:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_ATTACK_WARRIOR, szText);
			break;
		case CLASS_KINDOF_DEFEND_WARRIOR:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_DEFEND_WARRIOR, szText);
			break;
		case CLASS_KINDOF_ARCHER:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_ARCHER, szText);
			break;
		case CLASS_KINDOF_ASSASSIN:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_ASSASSIN, szText);
			break;
		case CLASS_KINDOF_ATTACK_WIZARD:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_ATTACK_WIZARD, szText);
			break;
		case CLASS_KINDOF_PET_WIZARD:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_PET_WIZARD, szText);
			break;
		case CLASS_KINDOF_HEAL_PRIEST:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_HEAL_PRIEST, szText);
			break;
		case CLASS_KINDOF_CURSE_PRIEST:
			::_LoadStringFromResource(IDS_CLASS_KINDOF_CURSE_PRIEST, szText);
			break;

		case CLASS_ELMORAD_WARRIOR:
		case CLASS_KARUS_WARRIOR:
			::_LoadStringFromResource(IDS_CLASS_WARRIOR, szText);
			break;
		case CLASS_ELMORAD_ROGUE:
		case CLASS_KARUS_ROGUE:
			::_LoadStringFromResource(IDS_CLASS_ROGUE, szText);
			break;
		case CLASS_ELMORAD_WIZARD:
		case CLASS_KARUS_WIZARD:
			::_LoadStringFromResource(IDS_CLASS_WIZARD, szText);
			break;
		case CLASS_ELMORAD_PRIEST:
		case CLASS_KARUS_PRIEST:
			::_LoadStringFromResource(IDS_CLASS_PRIEST, szText);
			break;
		
		case CLASS_KARUS_BERSERKER:
			::_LoadStringFromResource(IDS_CLASS_KARUS_BERSERKER, szText);
			break;
		case CLASS_KARUS_GUARDIAN:
			::_LoadStringFromResource(IDS_CLASS_KARUS_GUARDIAN, szText);
			break;
		case CLASS_KARUS_HUNTER:
			::_LoadStringFromResource(IDS_CLASS_KARUS_HUNTER, szText);
			break;
		case CLASS_KARUS_PENETRATOR:
			::_LoadStringFromResource(IDS_CLASS_KARUS_PENETRATOR, szText);
			break;
		case CLASS_KARUS_SORCERER:
			::_LoadStringFromResource(IDS_CLASS_KARUS_SORCERER, szText);
			break;
		case CLASS_KARUS_NECROMANCER:
			::_LoadStringFromResource(IDS_CLASS_KARUS_NECROMANCER, szText);
			break;
		case CLASS_KARUS_SHAMAN:
			::_LoadStringFromResource(IDS_CLASS_KARUS_SHAMAN, szText);
			break;
		case CLASS_KARUS_DARKPRIEST:
			::_LoadStringFromResource(IDS_CLASS_KARUS_DARKPRIEST, szText);
			break;
		
		case CLASS_ELMORAD_BLADE:
			::_LoadStringFromResource(IDS_CLASS_ELMORAD_BLADE, szText);
			break;
		case CLASS_ELMORAD_PROTECTOR:
			::_LoadStringFromResource(IDS_CLASS_EL_PROTECTOR, szText);
			break;
		case CLASS_ELMORAD_RANGER:
			::_LoadStringFromResource(IDS_CLASS_ELMORAD_RANGER, szText);
			break;
		case CLASS_ELMORAD_ASSASIN:
			::_LoadStringFromResource(IDS_CLASS_ELMORAD_ASSASIN, szText);
			break;
		case CLASS_ELMORAD_MAGE:
			::_LoadStringFromResource(IDS_CLASS_ELMORAD_MAGE, szText);
			break;
		case CLASS_ELMORAD_ENCHANTER:
			::_LoadStringFromResource(IDS_CLASS_ELMORAD_ENCHANTER, szText);
			break;
		case CLASS_ELMORAD_CLERIC:
			::_LoadStringFromResource(IDS_CLASS_ELMORAD_CLERIC, szText);
			break;
		case CLASS_ELMORAD_DRUID:
			::_LoadStringFromResource(IDS_CLASS_ELMORAD_DRUID, szText);
			break;
		
		default:
			__ASSERT(0, "Invalid Class");
			szText = "Unknown Class";
			return false;
	}

	return true;
}

bool CGameBase::GetTextByKnightsDuty(e_KnightsDuty eDuty, std::string& szText)
{
	switch(eDuty)
	{
		case KNIGHTS_DUTY_UNKNOWN:		::_LoadStringFromResource(IDS_KNIGHTS_DUTY_UNKNOWN, szText); 
			break;
		case KNIGHTS_DUTY_PUNISH:		::_LoadStringFromResource(IDS_KNIGHTS_DUTY_PUNISH, szText); 
			break;
		case KNIGHTS_DUTY_TRAINEE:		::_LoadStringFromResource(IDS_KNIGHTS_DUTY_TRAINEE, szText);
			break;
		case KNIGHTS_DUTY_KNIGHT:		::_LoadStringFromResource(IDS_KNIGHTS_DUTY_KNIGHT, szText);
			break;
		case KNIGHTS_DUTY_OFFICER:		::_LoadStringFromResource(IDS_KNIGHTS_DUTY_OFFICER, szText); 
			break;
		case KNIGHTS_DUTY_VICECHIEF:	::_LoadStringFromResource(IDS_KNIGHTS_DUTY_VICECHIEF, szText); 
			break;
		case KNIGHTS_DUTY_CHIEF:		::_LoadStringFromResource(IDS_KNIGHTS_DUTY_CHIEF, szText); 
			break;
		default: __ASSERT(0, "Invalid Knights Duty"); szText = "Unknown Duty"; 
			return false;
	}
	return true;
}

bool CGameBase::GetTextByItemClass(e_ItemClass eItemClass, std::string& szText)
{
	switch(eItemClass)
	{
		case ITEM_CLASS_DAGGER:
			::_LoadStringFromResource(IDS_ITEM_CLASS_DAGGER, szText);
			break; //(dagger)
		case ITEM_CLASS_SWORD:
			::_LoadStringFromResource(IDS_ITEM_CLASS_SWORD, szText);
			break; //(onehandsword)
		case ITEM_CLASS_SWORD_2H:
			::_LoadStringFromResource(IDS_ITEM_CLASS_SWORD_2H, szText);
			break; //(twohandsword)
		case ITEM_CLASS_AXE:
			::_LoadStringFromResource(IDS_ITEM_CLASS_AXE, szText);
			break; //(onehandaxe)
		case ITEM_CLASS_AXE_2H:
			::_LoadStringFromResource(IDS_ITEM_CLASS_AXE_2H, szText);
			break; //(twohandaxe)
		case ITEM_CLASS_MACE:
			::_LoadStringFromResource(IDS_ITEM_CLASS_MACE, szText);
			break; //(mace)
		case ITEM_CLASS_MACE_2H:
			::_LoadStringFromResource(IDS_ITEM_CLASS_MACE_2H, szText);
			break; //(twohandmace)
		case ITEM_CLASS_SPEAR:
			::_LoadStringFromResource(IDS_ITEM_CLASS_SPEAR, szText);
			break; //(spear)
		case ITEM_CLASS_POLEARM:
			::_LoadStringFromResource(IDS_ITEM_CLASS_POLEARM, szText);
			break; //(polearm)

		case ITEM_CLASS_SHIELD:
			::_LoadStringFromResource(IDS_ITEM_CLASS_SHIELD, szText);
			break; //(shield)

		case ITEM_CLASS_BOW:
			::_LoadStringFromResource(IDS_ITEM_CLASS_BOW, szText);
			break; //(Shortbow)
		case ITEM_CLASS_BOW_CROSS:
			::_LoadStringFromResource(IDS_ITEM_CLASS_BOW_CROSS, szText);
			break; //(crossbow)
		case ITEM_CLASS_BOW_LONG:
			::_LoadStringFromResource(IDS_ITEM_CLASS_BOW_LONG, szText);
			break; //(longbow)

		case ITEM_CLASS_EARRING:
			::_LoadStringFromResource(IDS_ITEM_CLASS_EARRING, szText);
			break; 
		case ITEM_CLASS_AMULET:
			::_LoadStringFromResource(IDS_ITEM_CLASS_AMULET, szText);
			break; 
		case ITEM_CLASS_RING:
			::_LoadStringFromResource(IDS_ITEM_CLASS_RING, szText);
			break; 
		case ITEM_CLASS_BELT:
			::_LoadStringFromResource(IDS_ITEM_CLASS_BELT, szText);
			break; 
		case ITEM_CLASS_CHARM:
			::_LoadStringFromResource(IDS_ITEM_CLASS_CHARM, szText);
			break;
		case ITEM_CLASS_JEWEL:
			::_LoadStringFromResource(IDS_ITEM_CLASS_JEWEL, szText);
			break;
		case ITEM_CLASS_POTION:
			::_LoadStringFromResource(IDS_ITEM_CLASS_POTION, szText);
			break;
		case ITEM_CLASS_SCROLL:
			::_LoadStringFromResource(IDS_ITEM_CLASS_SCROLL, szText);
			break;

		case ITEM_CLASS_LAUNCHER:
			::_LoadStringFromResource(IDS_ITEM_CLASS_LAUNCHER, szText);
			break; 
						
		case ITEM_CLASS_STAFF:
			::_LoadStringFromResource(IDS_ITEM_CLASS_STAFF, szText);
			break; //(staff)
		case ITEM_CLASS_ARROW:
			::_LoadStringFromResource(IDS_ITEM_CLASS_ARROW, szText);
			break; //(Arrow)
		case ITEM_CLASS_JAVELIN:
			::_LoadStringFromResource(IDS_ITEM_CLASS_JAVELIN, szText);
			break;	
		case ITEM_CLASS_ARMOR_WARRIOR:
			::_LoadStringFromResource(IDS_ITEM_CLASS_ARMOR_WARRIOR, szText);
			break;
		case ITEM_CLASS_ARMOR_ROGUE:
			::_LoadStringFromResource(IDS_ITEM_CLASS_ARMOR_ROGUE, szText);
			break;
		case ITEM_CLASS_ARMOR_MAGE:
			::_LoadStringFromResource(IDS_ITEM_CLASS_ARMOR_MAGE, szText);
			break;
		case ITEM_CLASS_ARMOR_PRIEST:
			::_LoadStringFromResource(IDS_ITEM_CLASS_ARMOR_PRIEST, szText); 
			break; 
		default:
			__ASSERT(0, "Invalid Item Class"); szText = "Unknonw Item Class";
			return false;
	}

	return true;
}

bool CGameBase::GetTextByAttrib(e_ItemAttrib eAttrib, std::string& szAttrib)
{
	switch(eAttrib)
	{
		case ITEM_ATTRIB_GENERAL:		::_LoadStringFromResource(IDS_ITEM_ATTRIB_GENERAL, szAttrib); 
			break; //(dagger)
		case ITEM_ATTRIB_MAGIC:			::_LoadStringFromResource(IDS_ITEM_ATTRIB_MAGIC, szAttrib);
			break; //(onehandsword)
		case ITEM_ATTRIB_LAIR:			::_LoadStringFromResource(IDS_ITEM_ATTRIB_LAIR, szAttrib); 
			break; //(twohandsword)
		case ITEM_ATTRIB_CRAFT:			::_LoadStringFromResource(IDS_ITEM_ATTRIB_CRAFT, szAttrib); 
			break; //(onehandaxe)
		case ITEM_ATTRIB_UNIQUE:		::_LoadStringFromResource(IDS_ITEM_ATTRIB_UNIQUE, szAttrib); 
			break; //(twohandaxe)
		case ITEM_ATTRIB_UPGRADE:		::_LoadStringFromResource(IDS_ITEM_ATTRIB_UPGRADE, szAttrib); 
			break; //(mace)
		default:
			return false;
	}
	return true;
}

e_Class_Represent CGameBase::GetRepresentClass(e_Class eClass)
{
	switch(eClass)
	{
		case CLASS_KARUS_WARRIOR:
		case CLASS_KARUS_BERSERKER:
		case CLASS_KARUS_GUARDIAN:

		case CLASS_ELMORAD_WARRIOR:
		case CLASS_ELMORAD_BLADE:
		case CLASS_ELMORAD_PROTECTOR:

			return CLASS_REPRESENT_WARRIOR;

		case CLASS_KARUS_ROGUE:
		case CLASS_KARUS_HUNTER:
		case CLASS_KARUS_PENETRATOR:

		case CLASS_ELMORAD_ROGUE:
		case CLASS_ELMORAD_RANGER:
		case CLASS_ELMORAD_ASSASIN:

			return CLASS_REPRESENT_ROGUE;

		case CLASS_KARUS_WIZARD:
		case CLASS_KARUS_SORCERER:
		case CLASS_KARUS_NECROMANCER:

		case CLASS_ELMORAD_WIZARD:
		case CLASS_ELMORAD_MAGE:
		case CLASS_ELMORAD_ENCHANTER:

			return CLASS_REPRESENT_WIZARD;

		case CLASS_KARUS_PRIEST:
		case CLASS_KARUS_SHAMAN:
		case CLASS_KARUS_DARKPRIEST:

		case CLASS_ELMORAD_PRIEST:
		case CLASS_ELMORAD_CLERIC:
		case CLASS_ELMORAD_DRUID:

			return CLASS_REPRESENT_PRIEST;
	}

	return CLASS_REPRESENT_UNKNOWN;
}


bool CGameBase::GetTextByNation(e_Nation eNation, std::string& szText)
{
	switch(eNation)
	{
		case NATION_ELMORAD:	::_LoadStringFromResource(IDS_NATION_ELMORAD, szText); 
			break;
		case NATION_KARUS:		::_LoadStringFromResource(IDS_NATION_KARUS, szText); 
			break;
		default: ::_LoadStringFromResource(IDS_NATION_UNKNOWN, szText); 
			return false;
	}
	return true;
}

bool CGameBase::GetTextByRace(e_Race eRace, std::string& szText)
{
	switch(eRace)
	{
		case RACE_ELMORAD_BABARIAN:
			::_LoadStringFromResource(IDS_RACE_ELMORAD_BABARIAN, szText);
			break;
		case RACE_ELMORAD_MAN:
			::_LoadStringFromResource(IDS_RACE_ELMORAD_MAN, szText);
			break;
		case RACE_ELMORAD_WOMEN:
			::_LoadStringFromResource(IDS_RACE_ELMORAD_WOMEN, szText);
			break;

		case RACE_KARUS_ARKTUAREK:
			::_LoadStringFromResource(IDS_RACE_KARUS_ARKTUAREK, szText);
			break;
		case RACE_KARUS_TUAREK:
			::_LoadStringFromResource(IDS_RACE_KARUS_TUAREK, szText);
			break;
		case RACE_KARUS_WRINKLETUAREK:
			::_LoadStringFromResource(IDS_RACE_KARUS_WRINKLETUAREK, szText);
			break;
		case RACE_KARUS_PURITUAREK:
			::_LoadStringFromResource(IDS_RACE_KARUS_PURITUAREK, szText);
			break;
			
		default:
			::_LoadStringFromResource(IDS_NATION_UNKNOWN, szText); 
			return false;
	}

	return true;
}

D3DCOLOR CGameBase::GetIDColorByLevelDifference(int iLevelDiff)
{
	D3DCOLOR crID = 0xffffffff;

	if(iLevelDiff >= 8)			
		crID = D3DCOLOR_ARGB(255, 255, 0, 255);
	else if(iLevelDiff >= 5)	
		crID = D3DCOLOR_ARGB(255, 255, 0, 0);
	else if(iLevelDiff >= 2)	
		crID = D3DCOLOR_ARGB(255, 255, 255, 0);
	else if(iLevelDiff >= -1)	
		crID = D3DCOLOR_ARGB(255, 255, 255, 255);
	else if(iLevelDiff >= -4)	
		crID = D3DCOLOR_ARGB(255, 0, 0, 255);
	else if(iLevelDiff >= -7)	
		crID = D3DCOLOR_ARGB(255, 0, 255, 0);
	else 
		crID = D3DCOLOR_ARGB(255, 0, 255, 255);

	return crID;
}

e_ItemType CGameBase::MakeResrcFileNameForUPC(__TABLE_ITEM_BASIC* pItem, std::string* pszResrcFN, std::string* pszIconFN, e_PartPosition& ePartPosition, e_PlugPosition& ePlugPosition, e_Race eRace /*= RACE_UNKNOWN*/)
{	
	ePartPosition = PART_POS_UNKNOWN;
	ePlugPosition = PLUG_POS_UNKNOWN;
	if(pszResrcFN) *pszResrcFN = "";
	if(pszIconFN) *pszIconFN = "";

	if(NULL == pItem) 
		return ITEM_TYPE_UNKNOWN;

	e_ItemType eType	= ITEM_TYPE_UNKNOWN;
	e_ItemPosition ePos	= (e_ItemPosition)pItem->byAttachPoint;	

	int iPos = 0;
	std::string szExt;
	
	if(ePos >= ITEM_POS_DUAL && ePos <= ITEM_POS_TWOHANDLEFT)
	{
		if(ITEM_POS_DUAL == ePos || ITEM_POS_RIGHTHAND == ePos || ITEM_POS_TWOHANDRIGHT == ePos) ePlugPosition = PLUG_POS_RIGHTHAND;
		else
			if(ITEM_POS_LEFTHAND == ePos || ITEM_POS_TWOHANDLEFT == ePos) ePlugPosition = PLUG_POS_LEFTHAND;

		eType = ITEM_TYPE_PLUG;
		szExt = ".n3cplug";
	}
	else if(ePos >= ITEM_POS_UPPER && ePos <= ITEM_POS_SHOES)
	{
		if(ITEM_POS_UPPER == ePos)			
			ePartPosition = PART_POS_UPPER;
		else if(ITEM_POS_LOWER == ePos)		
			ePartPosition = PART_POS_LOWER; 
		else if(ITEM_POS_HEAD == ePos)		
			ePartPosition = PART_POS_HAIR_HELMET;
		else if(ITEM_POS_GLOVES == ePos)	
			ePartPosition = PART_POS_HANDS;
		else if(ITEM_POS_SHOES == ePos)		
			ePartPosition = PART_POS_FEET;
		else { __ASSERT(0, "lll"); }
		
		eType = ITEM_TYPE_PART;
		szExt = ".n3cpart";
		iPos = ePartPosition + 1;
	}
	else if(ePos >= ITEM_POS_EAR && ePos <= ITEM_POS_INVENTORY)
	{
		eType = ITEM_TYPE_ICONONLY;
		szExt = ".dxt";
	}
	else if(ePos == ITEM_POS_GOLD)
	{
		eType = ITEM_TYPE_GOLD;
		szExt = ".dxt";
	}
	else if(ePos == ITEM_POS_SONGPYUN)
	{
		eType = ITEM_TYPE_SONGPYUN;
		szExt = ".dxt";
	}
	else
	{
		__ASSERT(0, "Invalid Item Position");
	}

	std::vector<char> buffer(256, NULL);
	if(pszResrcFN)
	{
		if(pItem->dwIDResrc) 
		{
			if(eRace != RACE_UNKNOWN && ePos >= /*ITEM_POS_DUAL*/ITEM_POS_UPPER && ePos <= ITEM_POS_SHOES) {
				// NOTE: no idea but perhaps this will work for now
				sprintf(&buffer[0], "Item\\%.1d_%.4d_%.2d_%.1d%s",
					(pItem->dwIDResrc / 10000000),
					((pItem->dwIDResrc / 1000) % 10000) + eRace,
					(pItem->dwIDResrc / 10) % 100,
					pItem->dwIDResrc % 10,
					szExt.c_str());
			} 
			else 
			{
				sprintf(&buffer[0], "Item\\%.1d_%.4d_%.2d_%.1d%s",
					(pItem->dwIDResrc / 10000000),
					(pItem->dwIDResrc / 1000) % 10000,
					(pItem->dwIDResrc / 10) % 100,
					pItem->dwIDResrc % 10,
					szExt.c_str());
			}

			*pszResrcFN = &buffer[0];
		}
		else
		{
			*pszResrcFN = "";
		}
	}
	if(pszIconFN)
	{
		sprintf(&buffer[0],	"UI\\ItemIcon_%.1d_%.4d_%.2d_%.1d.dxt",
			(pItem->dwIDIcon / 10000000), 
			(pItem->dwIDIcon / 1000) % 10000, 
			(pItem->dwIDIcon / 10) % 100, 
			pItem->dwIDIcon % 10);
		*pszIconFN = &buffer[0];
	}
	
	return eType;
}


bool CGameBase::IsValidCharacter(CPlayerBase* pCharacter)
{
	if(NULL == pCharacter) 
		return false;

	if(pCharacter == s_pPlayer) 
		return true;

	return s_pOPMgr->IsValidCharacter(pCharacter); 
}

CPlayerBase* CGameBase::CharacterGetByID(int iID, bool bFromAlive)
{
	if(iID < 0) 
		return NULL;

	if(iID == s_pPlayer->IDNumber()) 
		return s_pPlayer;

	return s_pOPMgr->CharacterGetByID(iID, bFromAlive);
}


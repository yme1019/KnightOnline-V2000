#ifndef __GAME_DEF_H_
#define __GAME_DEF_H_

#include <string>

#include "shared/types.h"
#include "shared/version.h"

#include "SDL2/SDL.h"

// TODO: Shift this logic into a separate header and generally clean this shared logic up
#ifndef ASSERT
#if defined(_DEBUG)
#define ASSERT assert
#include <assert.h>
#else
#define ASSERT
#endif
#endif
#include "shared/Packet.h"

const int CURRENT_VERSION = 1298;

const float PACKET_INTERVAL_MOVE = 1.5f;				
const float PACKET_INTERVAL_ROTATE = 4.0f;
const float PACKET_INTERVAL_REQUEST_TARGET_HP = 2.0f;


#define N3_FORMAT_VER_1068 0x00000001
#define N3_FORMAT_VER_1298 0x00000002

enum ZoneFlags
{
	ZF_TRADE_OTHER_NATION = (1 << 0),
	ZF_TALK_OTHER_NATION = (1 << 1),
	ZF_ATTACK_OTHER_NATION = (1 << 2),
	ZF_ATTACK_SAME_NATION = (1 << 3),
	ZF_FRIENDLY_NPCS = (1 << 4),
	ZF_WAR_ZONE = (1 << 5),
	ZF_CLAN_UPDATE = (1 << 6)  // Joining, disbanding, creating etc.
};

enum ZoneAbilityType
{
	// these control neutrality-related settings client-side, 
	// including whether collision is enabled for other players.
	ZoneAbilityNeutral = 0, // Players cannot attack each other, or NPCs. Can walk through players.
	ZoneAbilityPVP = 1, // Players can attack each other, and only NPCs from the opposite nation. Cannot walk through players.
	ZoneAbilitySpectator = 2, // player is spectating a 1v1 match (ZoneAbilityPVP is sent for the attacker)
	ZoneAbilitySiege1 = 3, // siege state 1 (unknown)
	ZoneAbilitySiege2 = 4, // siege state 2/4: if they have 0 NP & this is set, it will not show the message telling them to buy more.
	ZoneAbilitySiege3 = 5, // siege state 3 (unknown)
	ZoneAbilitySiegeDisabled = 6, // CSW not running
	ZoneAbilityCaitharosArena = 7, // Players can attack each other (don't seem to be able to anymore?), but not NPCs. Can walk through players.
	ZoneAbilityPVPNeutralNPCs = 8 // Players can attack each other, but not NPCs. Cannot walk through players.
};

enum eKeyMap 
{	
	KM_HOTKEY1 = SDL_SCANCODE_1,
	KM_HOTKEY2 = SDL_SCANCODE_2,
	KM_HOTKEY3 = SDL_SCANCODE_3,
	KM_HOTKEY4 = SDL_SCANCODE_4,
	KM_HOTKEY5 = SDL_SCANCODE_5,
	KM_HOTKEY6 = SDL_SCANCODE_6,
	KM_HOTKEY7 = SDL_SCANCODE_7,
	KM_HOTKEY8 = SDL_SCANCODE_8,
	KM_TOGGLE_RUN = SDL_SCANCODE_T,
	KM_TOGGLE_MOVE_CONTINOUS = SDL_SCANCODE_E,
	KM_TOGGLE_ATTACK = SDL_SCANCODE_R,
	KM_TOGGLE_SITDOWN = SDL_SCANCODE_C,
	KM_TOGGLE_INVENTORY = SDL_SCANCODE_I,
	KM_TOGGLE_SKILL = SDL_SCANCODE_K,
	KM_TOGGLE_STATE = SDL_SCANCODE_U,
	KM_TOGGLE_MINIMAP = SDL_SCANCODE_M,
	KM_TOGGLE_HELP = SDL_SCANCODE_F10,
	KM_TOGGLE_CMDLIST = SDL_SCANCODE_H,
	KM_CAMERA_CHANGE = SDL_SCANCODE_F9,
	KM_DROPPED_ITEM_OPEN = SDL_SCANCODE_F,
	KM_MOVE_FOWARD = SDL_SCANCODE_W,
	KM_MOVE_BACKWARD = SDL_SCANCODE_S,
	KM_ROTATE_LEFT = SDL_SCANCODE_A,
	KM_ROTATE_RIGHT = SDL_SCANCODE_D,
	KM_TARGET_NEARST_ENEMY = SDL_SCANCODE_Z,
	KM_TARGET_NEARST_PARTY = SDL_SCANCODE_X,
	KM_TARGET_NEARST_FRIEND = SDL_SCANCODE_V,
	KM_SKILL_PAGE_1 = SDL_SCANCODE_F1,
	KM_SKILL_PAGE_2 = SDL_SCANCODE_F2,
	KM_SKILL_PAGE_3 = SDL_SCANCODE_F3,
	KM_SKILL_PAGE_4 = SDL_SCANCODE_F4,
	KM_SKILL_PAGE_5 = SDL_SCANCODE_F5,
	KM_SKILL_PAGE_6 = SDL_SCANCODE_F6,
	KM_SKILL_PAGE_7 = SDL_SCANCODE_F7,
	KM_SKILL_PAGE_8 = SDL_SCANCODE_F8
};

enum e_PlayerType 
{ 
	PLAYER_BASE = 0,
	PLAYER_NPC = 1, 
	PLAYER_OTHER = 2,
	PLAYER_MYSELF = 3 
};

enum e_Race 
{	
	RACE_ALL = 0,

	RACE_KARUS_ARKTUAREK = 1, 
	RACE_KARUS_TUAREK = 2, 
	RACE_KARUS_WRINKLETUAREK = 3, 
	RACE_KARUS_PURITUAREK = 4,

	RACE_ELMORAD_BABARIAN = 11, 
	RACE_ELMORAD_MAN = 12, 
	RACE_ELMORAD_WOMEN = 13,

	RACE_NPC = 100,
	RACE_UNKNOWN = 0xffffffff
};

enum e_Class 
{	
	CLASS_KINDOF_WARRIOR = 1, CLASS_KINDOF_ROGUE, CLASS_KINDOF_WIZARD, CLASS_KINDOF_PRIEST,
	CLASS_KINDOF_ATTACK_WARRIOR, CLASS_KINDOF_DEFEND_WARRIOR, CLASS_KINDOF_ARCHER, CLASS_KINDOF_ASSASSIN,
	CLASS_KINDOF_ATTACK_WIZARD, CLASS_KINDOF_PET_WIZARD, CLASS_KINDOF_HEAL_PRIEST, CLASS_KINDOF_CURSE_PRIEST,

	CLASS_KARUS_WARRIOR = 101, 
	CLASS_KARUS_ROGUE, 
	CLASS_KARUS_WIZARD, 
	CLASS_KARUS_PRIEST,
	CLASS_KARUS_BERSERKER = 105, 
	CLASS_KARUS_GUARDIAN, 
	CLASS_KARUS_HUNTER = 107, 
	CLASS_KARUS_PENETRATOR,
	CLASS_KARUS_SORCERER = 109, 
	CLASS_KARUS_NECROMANCER, 
	CLASS_KARUS_SHAMAN = 111, 
	CLASS_KARUS_DARKPRIEST,

	CLASS_ELMORAD_WARRIOR = 201, 
	CLASS_ELMORAD_ROGUE, 
	CLASS_ELMORAD_WIZARD, 
	CLASS_ELMORAD_PRIEST,
	CLASS_ELMORAD_BLADE = 205, 
	CLASS_ELMORAD_PROTECTOR, 
	CLASS_ELMORAD_RANGER = 207, 
	CLASS_ELMORAD_ASSASIN,
	CLASS_ELMORAD_MAGE = 209, 
	CLASS_ELMORAD_ENCHANTER, 
	CLASS_ELMORAD_CLERIC = 211, 
	CLASS_ELMORAD_DRUID,

	CLASS_UNKNOWN = 0xffffffff
};

enum e_Class_Represent 
{ 
	CLASS_REPRESENT_WARRIOR = 0, 
	CLASS_REPRESENT_ROGUE, 
	CLASS_REPRESENT_WIZARD, 
	CLASS_REPRESENT_PRIEST, 
	CLASS_REPRESENT_UNKNOWN = 0xffffffff 
};

const float WEAPON_WEIGHT_STAND_SWORD = 5.0f; 
const float WEAPON_WEIGHT_STAND_AXE = 5.0f; 
const float WEAPON_WEIGHT_STAND_BLUNT = 8.0f; 

enum e_Ani 
{	
	ANI_BREATH = 0, ANI_WALK, ANI_RUN, ANI_WALK_BACKWARD, ANI_STRUCK0, ANI_STRUCK1, ANI_STRUCK2, ANI_GUARD,
	ANI_DEAD_NEATLY = 8, ANI_DEAD_KNOCKDOWN, ANI_DEAD_ROLL, ANI_SITDOWN, ANI_SITDOWN_BREATH, ANI_STANDUP,
	ANI_ATTACK_WITH_WEAPON_WHEN_MOVE = 14, ANI_ATTACK_WITH_NAKED_WHEN_MOVE, 

	ANI_SPELLMAGIC0_A = 16, ANI_SPELLMAGIC0_B, 
	ANI_SPELLMAGIC1_A = 18, ANI_SPELLMAGIC1_B, 
	ANI_SPELLMAGIC2_A = 20, ANI_SPELLMAGIC2_B, 
	ANI_SPELLMAGIC3_A = 22, ANI_SPELLMAGIC3_B, 
	ANI_SPELLMAGIC4_A = 24, ANI_SPELLMAGIC4_B, 
				
	ANI_SHOOT_ARROW_A = 26, ANI_SHOOT_ARROW_B, 
	ANI_SHOOT_QUARREL_A = 28, ANI_SHOOT_QUARREL_B, 
	ANI_SHOOT_JAVELIN_A = 30, ANI_SHOOT_JAVELIN_B, 
				
	ANI_SWORD_BREATH_A = 32,	ANI_SWORD_ATTACK_A0, ANI_SWORD_ATTACK_A1,
	ANI_SWORD_BREATH_B,			ANI_SWORD_ATTACK_B0, ANI_SWORD_ATTACK_B1,	
				
	ANI_DAGGER_BREATH_A = 38,	ANI_DAGGER_ATTACK_A0, ANI_DAGGER_ATTACK_A1,
	ANI_DAGGER_BREATH_B,		ANI_DAGGER_ATTACK_B0, ANI_DAGGER_ATTACK_B1,		
				
	ANI_DUAL_BREATH_A = 44,		ANI_DUAL_ATTACK_A0, ANI_DUAL_ATTACK_A1, 
	ANI_DUAL_BREATH_B,			ANI_DUAL_ATTACK_B0, ANI_DUAL_ATTACK_B1,			
				
	ANI_SWORD2H_BREATH_A = 50,	ANI_SWORD2H_ATTACK_A0, ANI_SWORD2H_ATTACK_A1, 
	ANI_SWORD2H_BREATH_B,		ANI_SWORD2H_ATTACK_B0, ANI_SWORD2H_ATTACK_B1,	
				
	ANI_BLUNT_BREATH_A = 56,	ANI_BLUNT_ATTACK_A0, ANI_BLUNT_ATTACK_A1, 
	ANI_BLUNT_BREATH_B,			ANI_BLUNT_ATTACK_B0, ANI_BLUNT_ATTACK_B1,		
				
	ANI_BLUNT2H_BREATH_A = 62,	ANI_BLUNT2H_ATTACK_A0, ANI_BLUNT2H_ATTACK_A1, 
	ANI_BLUNT2H_BREATH_B,		ANI_BLUNT2H_ATTACK_B0, ANI_BLUNT2H_ATTACK_B1,	
				
	ANI_AXE_BREATH_A = 68,		ANI_AXE_ATTACK_A0, ANI_AXE_ATTACK_A1, 
	ANI_AXE_BREATH_B,			ANI_AXE_ATTACK_B0, ANI_AXE_ATTACK_B1,			
				
	ANI_SPEAR_BREATH_A = 74,	ANI_SPEAR_ATTACK_A0, ANI_SPEAR_ATTACK_A1, 
	ANI_SPEAR_BREATH_B,			ANI_SPEAR_ATTACK_B0, ANI_SPEAR_ATTACK_B1,		
				
	ANI_POLEARM_BREATH_A = 80,	ANI_POLEARM_ATTACK_A0, ANI_POLEARM_ATTACK_A1, 
	ANI_POLEARM_BREATH_B,		ANI_POLEARM_ATTACK_B0, ANI_POLEARM_ATTACK_B1,	
				
	ANI_NAKED_BREATH_A = 86,	ANI_NAKED_ATTACK_A0, ANI_NAKED_ATTACK_A1, 
	ANI_NAKED_BREATH_B,			ANI_NAKED_ATTACK_B0, ANI_NAKED_ATTACK_B1,		
				
	ANI_BOW_BREATH = 92,		ANI_CROSS_BOW_BREATH, ANI_LAUNCHER_BREATH, 
	ANI_BOW_BREATH_B,			ANI_BOW_ATTACK_B0, ANI_BOW_ATTACK_B1,			
				
	ANI_SHIELD_BREATH_A = 98,	ANI_SHIELD_ATTACK_A0, ANI_SHIELD_ATTACK_A1, 
	ANI_SHIELD_BREATH_B,		ANI_SHIELD_ATTACK_B0, ANI_SHIELD_ATTACK_B1,		

	ANI_GREETING0 = 104, ANI_GREETING1, ANI_GREETING2, 
	ANI_WAR_CRY0 = 107, ANI_WAR_CRY1, ANI_WAR_CRY2, ANI_WAR_CRY3, ANI_WAR_CRY4, 

	ANI_SKILL_AXE0 = 112, ANI_SKILL_AXE1, ANI_SKILL_AXE2, ANI_SKILL_AXE3, 
	ANI_SKILL_DAGGER0 = 116, ANI_SKILL_DAGGER1,
	ANI_SKILL_DUAL0 = 118, ANI_SKILL_DUAL1,
	ANI_SKILL_BLUNT0 = 120, ANI_SKILL_BLUNT1, ANI_SKILL_BLUNT2, ANI_SKILL_BLUNT3, 
	ANI_SKILL_POLEARM0 = 124, ANI_SKILL_POLEARM1,
	ANI_SKILL_SPEAR0 = 126, ANI_SKILL_SPEAR1,
	ANI_SKILL_SWORD0 = 128, ANI_SKILL_SWORD1, ANI_SKILL_SWORD2, ANI_SKILL_SWORD3, 
	ANI_SKILL_AXE2H0 = 132, ANI_SKILL_AXE2H1,
	ANI_SKILL_SWORD2H0 = 134, ANI_SKILL_SWORD2H1,

	// 여기부터는 NPC Animation
	ANI_NPC_BREATH = 0, ANI_NPC_WALK, ANI_NPC_RUN, ANI_NPC_WALK_BACKWARD,
	ANI_NPC_ATTACK0 = 4, ANI_NPC_ATTACK1, ANI_NPC_STRUCK0, ANI_NPC_STRUCK1, ANI_NPC_STRUCK2, ANI_NPC_GUARD, 
	ANI_NPC_DEAD0 = 10, ANI_NPC_DEAD1, ANI_NPC_TALK0, ANI_NPC_TALK1, ANI_NPC_TALK2, ANI_NPC_TALK3, 
	ANI_NPC_SPELLMAGIC0 = 16, ANI_NPC_SPELLMAGIC1, 

	ANI_UNKNOWN = 0xffffffff 
};


const	float MAX_INCLINE_CLIMB = 0.6430f;

enum e_MoveDirection { MD_STOP, MD_FOWARD, MD_BACKWARD, MD_UNKNOWN = 0xffffffff };

const float MOVE_DELTA_WHEN_RUNNING = 3.0f;
const float MOVE_SPEED_WHEN_WALK = 1.5f; 

enum e_StateMove
{	
	PSM_STOP = 0,
	PSM_WALK,
	PSM_RUN,
	PSM_WALK_BACKWARD,
	PSM_COUNT
};

enum e_StateAction
{	
	PSA_BASIC = 0,
	PSA_ATTACK,			
	PSA_GUARD,			
	PSA_STRUCK,			
	PSA_DYING,			
	PSA_DEATH,			
	PSA_SPELLMAGIC,		
	PSA_SITDOWN, 		
	PSA_COUNT 
}; 

enum e_StateDying 
{		
	PSD_DISJOINT = 0,	
	PSD_KNOCK_DOWN,		
	PSD_KEEP_POSITION,	
	PSD_COUNT,
	PSD_UNKNOWN = 0xffffffff 
};

enum e_StateParty
{
	PSP_NORMAL = 0,
	PSP_POISONING = 1,
	PSP_CURSED = 2,
	PSP_MAGIC_TAKEN = 4,
	PSP_BLESSED = 8,
	PSP_UNKNOWN = 0xffffffff
};

enum e_PartPosition
{
	PART_POS_UPPER = 0,
	PART_POS_LOWER,
	PART_POS_FACE,
	PART_POS_HANDS,
	PART_POS_FEET,
	PART_POS_HAIR_HELMET,
	PART_POS_COUNT,
	PART_POS_UNKNOWN = 0xffffffff
};

enum e_PlugPosition
{
	PLUG_POS_RIGHTHAND = 0,
	PLUG_POS_LEFTHAND,
	PLUG_POS_BACK,
	PLUG_POS_KNIGHTS_GRADE,
	PLUG_POS_COUNT,
	PLUG_POS_UNKNOWN = 0xffffffff
};

enum e_ItemAttrib
{
	ITEM_ATTRIB_GENERAL = 0,
	ITEM_ATTRIB_MAGIC = 1,
	ITEM_ATTRIB_LAIR = 2,
	ITEM_ATTRIB_CRAFT = 3,
	ITEM_ATTRIB_UNIQUE = 4,
	ITEM_ATTRIB_UPGRADE = 5,
	ITEM_ATTRIB_UNIQUE_REVERSE = 11,
	ITEM_ATTRIB_UPGRADE_REVERSE = 12,
	ITEM_ATTRIB_UNKNOWN = 0xffffffff
};

enum e_ItemClass	
{
	ITEM_CLASS_DAGGER = 11, // (dagger)
	ITEM_CLASS_SWORD = 21, // (onehandsword)
	ITEM_CLASS_SWORD_2H = 22, //(twohandsword)
	ITEM_CLASS_AXE = 31, // (onehandaxe)
	ITEM_CLASS_AXE_2H = 32, // (twohandaxe)
	ITEM_CLASS_MACE = 41, // (mace)
	ITEM_CLASS_MACE_2H = 42, //(twohandmace)
	ITEM_CLASS_SPEAR = 51, // (spear)
	ITEM_CLASS_POLEARM = 52, //(polearm)
						
	ITEM_CLASS_SHIELD = 60,

	ITEM_CLASS_BOW = 70, 
	ITEM_CLASS_BOW_CROSS = 71,
	ITEM_CLASS_BOW_LONG = 80,

	ITEM_CLASS_EARRING = 91,
	ITEM_CLASS_AMULET = 92, 
	ITEM_CLASS_RING = 93,
	ITEM_CLASS_BELT = 94, 
	ITEM_CLASS_CHARM = 95, 
	ITEM_CLASS_JEWEL = 96, 
	ITEM_CLASS_POTION = 97, 
	ITEM_CLASS_SCROLL = 98, 

	ITEM_CLASS_LAUNCHER = 100, 
						
	ITEM_CLASS_STAFF = 110, //(staff)
	ITEM_CLASS_ARROW = 120, // (Arrow)
	ITEM_CLASS_JAVELIN = 130,
						
	ITEM_CLASS_ARMOR_WARRIOR = 210, 
	ITEM_CLASS_ARMOR_ROGUE = 220, 
	ITEM_CLASS_ARMOR_MAGE = 230, 
	ITEM_CLASS_ARMOR_PRIEST = 240,

	ITEM_CLASS_ETC = 251, 

	ITEM_CLASS_UNKNOWN = 0xffffffff 
}; 

enum e_Nation 
{
	NATION_NOTSELECTED = 0,
	NATION_KARUS, 
	NATION_ELMORAD, 
	NATION_UNKNOWN = 0xffffffff 
};

struct __TABLE_ITEM_BASIC;
struct __TABLE_ITEM_EXT;
struct __TABLE_PLAYER;

enum e_Authority 
{
	AUTHORITY_MANAGER = 0,
	AUTHORITY_USER,
	AUTHORITY_NOCHAT,
	AUTHORITY_NPC,
	AUTHORITY_BLOCK_USER = 0xff
};

struct __InfoPlayerOther
{
	int			iFace;
	int			iHair;	

	int			iCity;
	int			iKnightsID;
	std::string szKnights;
	int			iKnightsGrade;
	int			iKnightsRank;

	int			iRank;
	int			iTitle;

	void Init()
	{
		iFace = 0;			
		iHair = 0;			
		iCity;				
		iKnightsID = 0;		
		szKnights = "";		
		iKnightsGrade = 0;
		iKnightsRank = 0;	
		iTitle = 0;
	}
};

enum e_KnightsDuty 
{	
	KNIGHTS_DUTY_UNKNOWN	 = 0,		
	KNIGHTS_DUTY_CHIEF		 = 1,			
	KNIGHTS_DUTY_VICECHIEF	 = 2,		
	KNIGHTS_DUTY_PUNISH		 = 3,		
	KNIGHTS_DUTY_TRAINEE	 = 4,		
	KNIGHTS_DUTY_KNIGHT		 = 5,		
	KNIGHTS_DUTY_OFFICER	 = 6		
};

#define VICTORY_ABSENCE		0
#define VICTORY_KARUS		1
#define VICTORY_ELMORAD		2

struct __InfoPlayerMySelf : public __InfoPlayerOther
{
	int					iBonusPointRemain;
	int					iLevelPrev; 

	int					iMSPMax; 
	int					iMSP; 
			
	int					iTargetHPPercent;
	int					iGold;
	Uint64				iExpNext;
	Uint64				iExp;
	int					iRealmPoint;		
	int					iRealmPointMonthly;	
	e_KnightsDuty		eKnightsDuty;		
	int					iWeightMax;		
	int					iWeight;			
	int					iStrength;
	int					iStrength_Delta;		
	int					iStamina;			
	int					iStamina_Delta;		
	int					iDexterity;
	int					iDexterity_Delta;	
	int					iIntelligence;		
	int					iIntelligence_Delta;
	int 				iMagicAttak;		
	int 				iMagicAttak_Delta;	
	
	int 				iAttack;		
	int 				iAttack_Delta;	
	int 				iGuard;			
	int 				iGuard_Delta;

	int 				iRegistFire;			
	int 				iRegistFire_Delta;		
	int 				iRegistCold;			
	int 				iRegistCold_Delta;		
	int 				iRegistLight;			
	int 				iRegistLight_Delta;		
	int 				iRegistMagic;			
	int 				iRegistMagic_Delta;		
	int 				iRegistCurse;			
	int 				iRegistCurse_Delta;		
	int 				iRegistPoison;			
	int 				iRegistPoison_Delta;	

	int					iZoneInit;
	int					iZoneCur;
	int					iVictoryNation;

	void Init()
	{
		__InfoPlayerOther::Init();

		iBonusPointRemain = 0; 
		iLevelPrev = 0;

		iMSPMax = 0; 
		iMSP = 0; 
		
		iTargetHPPercent = 0;
		iGold = 0;
		iExpNext = 0;
		iExp = 0; 
		iRealmPoint = 0;		
		iRealmPointMonthly = 0;
		eKnightsDuty = KNIGHTS_DUTY_UNKNOWN;		
		iWeightMax = 0;			
		iWeight = 0;			
		iStrength = 0;			
		iStrength_Delta = 0;	
		iStamina = 0;			
		iStamina_Delta = 0;		
		iDexterity = 0;			
		iDexterity_Delta = 0;	
		iIntelligence = 0;		
		iIntelligence_Delta = 0;
		iMagicAttak = 0;		
		iMagicAttak_Delta = 0;	
		
		iAttack = 0;		
		iAttack_Delta = 0;
		iGuard = 0;			
		iGuard_Delta = 0;	

		iRegistFire = 0;			
		iRegistFire_Delta = 0;		
		iRegistCold = 0;			
		iRegistCold_Delta = 0;		
		iRegistLight = 0;			
		iRegistLight_Delta = 0;		
		iRegistMagic = 0;			
		iRegistMagic_Delta = 0;		
		iRegistCurse = 0;			
		iRegistCurse_Delta = 0;		
		iRegistPoison = 0;			
		iRegistPoison_Delta = 0;	

		iZoneInit = 0x01;		
		iZoneCur = 0;			
		iVictoryNation = -1;
	}
};

const int MAX_PARTY_OR_FORCE = 8;

struct __InfoPartyOrForce
{
	int			iID;			
	int			iLevel;			
	e_Class		eClass;			
	int			iHP;			
	int			iHPMax;			
	bool		bSufferDown_HP;			
	bool		bSufferDown_Etc;		
	std::string szID;		

	void Init()
	{
		iID = -1;
		iLevel = 0;
		eClass = CLASS_UNKNOWN;
		iHP = 0;
		iHPMax = 0;
		szID = "";

		bSufferDown_HP = false;	
		bSufferDown_Etc = false;	
	};

	__InfoPartyOrForce()
	{
		this->Init();
	}
};

enum e_PartyStatus { PARTY_STATUS_DOWN_HP = 1, PARTY_STATUS_DOWN_ETC = 2 };

struct __InfoPartyBBS // 파티 지원 게시판 구조체..
{
	std::string szID;			// 파티 이름 문자열
	int			iID;			// 파티원 ID
	int			iLevel;			// Level
	e_Class		eClass;			// 직업
	int			iMemberCount;

	void Init()
	{
		szID = "";
		iID = -1;
		iLevel = 0;
		eClass = CLASS_UNKNOWN;
		iMemberCount = 0;
	};

	__InfoPartyBBS()
	{
		this->Init();
	}
};

enum {	RESRC_UI_LOGIN = 0, RESRC_UI_CHARACTERSELECT, RESRC_UI_CHARACTERCREATE, RESRC_UI_INVENTORY, RESRC_UI_CHAT,
		RESRC_UI_MYINFORMATION, RESRC_UI_TARGET, RESRC_UI_HOTKEY, RESRC_UI_FUNCTION };

typedef struct _TBL_TABLE_TEXTS
{
	uint32_t		dwID;
	std::string		szText;

} TABLE_TEXTS;

typedef struct _TBL_TABLE_ZONE
{
	uint32_t        dwID;				  // zone ID
	std::string		szTerrainFN;          // GTD

	std::string		szName;
	std::string		szColorMapFN;         // TCT
	std::string		szLightMapFN;         // TLT
	std::string		szObjectPostDataFN;   // OPD

#if __VERSION > 1264
	std::string		szReserved;
#endif

	std::string		szMiniMapFN;          // DXT
	std::string		szSkySetting;         // N3Sky
	int				bIndicateEnemyPlayer; // SByte (now Int32)
	int				iFixedSundDirection;  // Int32
	std::string		szLightObjFN;         // GLO

	std::string		szGEVFN;              // GEV
	int				iIdk0;                // idk
	std::string		szENSFN;              // ENS
	float			fIdk1;                // idk
	std::string		szFLAGFN;             // FLAG
	uint32_t        iIdk2;
	uint32_t        iIdk3;
	uint32_t        iIdk4;
	uint32_t        iIdk5;
	std::string		szOPDSUBFN;           // OPDSUB
	int				iIdk6;
	std::string		szEVTSUB;             // EVTSUB
	std::string		sMob;
} 
TABLE_ZONE;

typedef struct _TBL_TABLE_UI
{
	uint32_t	UIndex;							// 0 (Karus/Human)
	std::string login_intro_us;					// 1 
	std::string Co_Cmd_us;						// 2 
	std::string co_chatting_box_us;				// 3 
	std::string co_information_box_us;			// 4 
	std::string Co_StateBar_us;					// 5 
	std::string Co_Various_frame_us;			// 6 
	std::string Co_Page_State_us;				// 7 
	std::string Co_Page_clan_us;				// 8 
	std::string Co_Page_Quest_us;				// 9 
	std::string Co_Page_Friends_us;				// 10 
	std::string Co_Inventory_us;				// 11
	std::string Co_Transaction_us;				// 12
	std::string co_DroppedItem_us;				// 13
	std::string co_TargetBar_us;				// 14
	std::string Co_TargetSymbol;				// 15
	std::string Co_SkillTree_us;				// 16
	std::string co_HotKey_us;					// 17
	std::string Ka_MiniMap_us;					// 18
	std::string Co_Party_us;					// 19
	std::string Co_PartyBoard_us;				// 20
	std::string Co_Help_us;						// 21
	std::string Co_Notice_us;					// 22
	std::string Co_CharacterCreate_us;			// 23
	std::string Co_CharacterSelect_us;			// 24
	std::string Co_ToolTip_us;					// 25
	std::string Co_MessageBox_us;				// 26
	std::string Co_Loading_us;					// 27
	std::string Co_ItemInfo_us;					// 28
	std::string Co_PersonalTrade_us;			// 29
	std::string Co_PersonalTradeEdit_us;		// 30
	std::string co_smith_us;					// 31
	std::string Co_Warp_us;						// 32
	std::string Co_change_us;					// 33
	std::string Co_tooltop_repair_us;			// 34
	std::string co_talk_us;						// 35
	std::string co_list2_us;					// 36
	std::string co_KnightsOperation_us;			// 37
	std::string co_changeclass_us;				// 38
	std::string Co_EndMessage_us;				// 39
	std::string Co_WareHouse_us;				// 40
	std::string co_change_us;					// 41
	std::string co_change_bill_us;				// 42
	std::string co_inn_us;						// 43
	std::string co_creat_clan_us;				// 44
	std::string Co_saleboard_us;				// 45
	std::string co_saleboardselection_us;		// 46
	std::string co_saleboardmemo_us;			// 47
	std::string co_saleboardmemolist_us;		// 48
	std::string co_QuestMenu_us;				// 49
	std::string co_QuestTalk_us;				// 50
	std::string co_Quest_Edit_us;				// 51
	std::string co_Dead_us;						// 52
	std::string Co_Elmorad_loading_us;			// 53
	std::string Co_Karus_loading_us;			// 54
	std::string Co_nationselect_us;				// 55
	std::string Co_chat_small_us;				// 56
	std::string Co_msgoutput_small_us;			// 57
	std::string co_ItemUpgrade_us;				// 58
	std::string co_DuelCreate_us;				// 59
	std::string co_DuelList_us;					// 60
	std::string co_DuelMessageBox_us;			// 61
	std::string co_DuelEditMsg_us;				// 62
	std::string co_DuelLobby_us;				// 63
	std::string co_QuestContent_us;				// 64
	std::string co_DuelItemCount_us;			// 65
	std::string co_TradeInventory_us;			// 66
	std::string co_TradeBuyInventory_us;		// 67
	std::string co_TradeItemDisplay_us;			// 68
	std::string co_TradePrice_us;				// 69
	std::string co_TradeCount_us;				// 70
	std::string co_TradeMessageBox_us;			// 71
	std::string Co_page_knights_us;				// 72
	std::string Co_page_knights_Union_main_us;	// 73
	std::string Co_page_knights_Union_sub_us;	// 74
	std::string Co_CmdList_us;					// 75
	std::string co_CmdEdit_us;					// 76
	std::string co_knights_crest_us;			// 77
	std::string co_ShoppingMall_us;				// 78
	std::string Co_levelguide_us;				// 79
	std::string co_WarfareNpc_us;				// 80
	std::string co_WarfarePetition_us;			// 81
	std::string co_CastleUnion_us;				// 82
	std::string co_WarfareSchedule_us;			// 83
	std::string co_ExitMenu_us;					// 84
	std::string co_resurrection_us;				// 85
	std::string co_idchange_us;					// 86
	std::string co_id_us;						// 87
	std::string co_id_check_us;					// 88
	std::string co_WarfareAdministrationNpc_us;	// 89
	std::string co_WarfareTaxTariff_us;			// 90
	std::string Co_WarfareMantleList_us;		// 91
	std::string co_KnightsMantleShop_us;		// 92
	std::string co_WarfareTaxCollection_us;		// 93
	std::string co_WarfareTaxRate_us;			// 94
	std::string co_WarfareTaxMsg_us;			// 95
	std::string Co_catapult_us;					// 96
	std::string co_disguiseRing_us;				// 97
	std::string co_MsgBoxOk_us;					// 98
	std::string co_MsgBoxOkCancel_us;			// 99
	std::string co_whisper_open_us;				// 100
	std::string co_whisper_close_us;			// 101
	std::string co_knights_crest_chr_us;		// 102
	std::string co_warning_us;					// 103
	std::string co_conversation_us;				// 104
	std::string co_blog_us;						// 105
	std::string co_inn_password_us;				// 106
	std::string co_rookietip_us;				// 107
	std::string co_WebPage_us;					// 108
	std::string co_partyboard_message_us;		// 109
	std::string co_symbol_us;					// 110
	std::string co_rental_npc_us;				// 111
	std::string co_rental_Transaction_us;		// 112
	std::string co_rental_entry_us;				// 113
	std::string co_rental_item_us;				// 114
	std::string co_rental_message_us;			// 115
	std::string co_rental_Count_us;				// 116
	std::string co_netdio_us;					// 117
	std::string Co_login_intro_us;				// 118
	std::string co_login_intro_sub_us;			// 119
	std::string co_CharacterSelect_us;			// 120
	std::string co_characterCreate_us;			// 121
	std::string co_other_state_us;				// 122
	std::string co_ppcard_begin_us;				// 123
	std::string co_ppcard_list_us;				// 125
	std::string co_ppcard_regist_us;			// 126
	std::string co_ppcard_message_us;			// 127
	std::string co_ppcard_buylist_us;			// 128
	std::string co_ppcard_myinfo_us;			// 129
	std::string co_nation_Select_us;			// 130
	std::string co_usa_logo_us;					// 131
	std::string co_monster_us;					// 132
	std::string co_NationTaxNpc_us;				// 133
	std::string co_NationTaxRate_us;			// 134
	std::string co_MsgBoxOk_King_us;			// 135
	std::string co_MsgBoxOkCancel_King_us;		// 136
	std::string co_King_ElectionBoard_us;		// 137
	std::string co_King_ElectionList_us;		// 138
	std::string co_King_ElectionMain_us;		// 139
	std::string co_King_MemberRecommend_us;		// 140
	std::string co_King_ElectionRegist_us;		// 141
	std::string co_ringUpgrade_us;				// 142
	std::string co_upgradeselect_us;			// 143
	std::string co_tradeMessage_us;				// 144
	std::string co_piecechange_us;				// 145
	std::string co_time_us;						// 146
	std::string co_map1024_us;					// 147
	std::string co_map1280_us;					// 148
	std::string co_map1600_us;					// 149
	std::string co_mailsystem_alarm_us;			// 150
	std::string co_mailsystem_us;				// 151
	std::string co_mailsystem_read_us;			// 152
	std::string co_mailsystem_list_us;			// 153
	std::string co_show_icon_us;				// 154
	std::string co_changename_us;				// 155
	std::string co_buffer_us;					// 156
	std::string co_taskbar_main_us;				// 157
	std::string co_taskbar_sub_us;				// 158
	std::string co_hpbar_us;					// 159
	std::string co_minimap_us;					// 160
	std::string co_Seed_Helper_us;				// 161
	std::string co_Quest_Seed_us;				// 162
	std::string co_Quest_Npc_Talk_us;			// 163
	std::string co_Quest_Npc_Menu_us;			// 164
	std::string co_Quest_Viewer_us;				// 165
	std::string co_Quest_Map_us;				// 166
	std::string co_Quest_Requital_us;			// 167
	std::string co_Quest_Completed_us;			// 168
	std::string co_Quest_MainStory_1_us;		// 169
	std::string co_Quest_MainStory_2_us;		// 170
	std::string co_Quest_MainStory_3_us;		// 171
	std::string Agreement_tw_us;				// 172
	std::string co_EventPremium_us;				// 173
	std::string co_EventNotice_us;				// 174
	std::string co_EventQuest_us;				// 175
	std::string co_EventContent_us;				// 176
	std::string co_MainTopNotice_us;			// 177
	std::string co_minimenu_us;					// 178
	std::string co_loading_event_us;			// 179
	std::string co_durability_us;				// 180
	std::string co_TradeItemDisplaySpecial_us;	// 181
	std::string co_check;						// 182
	std::string co_item_Seal_us;				// 183
	std::string co_pet_various_frame_us;		// 184
	std::string co_pet_page_state_us;			// 185
	std::string co_pet_page_inventory_us;		// 186
	std::string co_pet_page_skil_us;			// 187
	std::string co_pet_hotkey_us;				// 189
	std::string co_pet_quest_us;				// 190
	std::string co_pet_hpbar_us;				// 191
	std::string co_cos_2_us;					// 192
	std::string co_partyboard_message_2_us;		// 193
	std::string co_party_access_close_us;		// 194
	std::string co_party_access_open_us;		// 195
	std::string co_character_seal_us;			// 196
	std::string co_character_seal_trans_us;		// 197
	std::string co_character_seal_tooltip_us;	// 198
	std::string co_introduction_us;				// 199
	std::string co_InsertID_us;					// 200
	std::string co_page_force_king_us;			// 201
	std::string co_page_force_officer_us;		// 202
	std::string co_page_force_member_us;		// 203
	std::string co_force_join_us;				// 204
	std::string co_palette_us;					// 205
	std::string co_clan_ClanPoint_us;			// 206
	std::string co_clan_RealmPointSave_us;		// 204
	std::string co_clan_LeaderTransfer_us;		// 208
	std::string co_pet_balloon_us;				// 209
	std::string co_pointList_us;				// 210
	std::string co_pointShow_us;				// 211
	std::string co_Radder_us;					// 212
	std::string co_quest_mini_tip_us;			// 213
	std::string co_check_button_us;				// 214
	std::string co_voteboard_us;				// 215
	std::string co_King_LunaMap_us;				// 216
	std::string co_loading_luna_us;				// 217
	std::string co_chat_creat_us;				// 218
	std::string co_caht_main_us;				// 219
	std::string co_chat_into_us;				// 220
	std::string co_chat_name_us;				// 221
	std::string co_fortune_us;					// 222
	std::string co_chat_minimenu_us;			// 223
	std::string Co_Movie_Save_us;				// 224
	std::string Co_Movie_Resolution_us;			// 225
	std::string Co_Movie_Result_us;				// 226
	std::string co_file_select_us;				// 227
}
_TBL_TABLE_UI;

typedef struct __TABLE_ITEM_BASIC
{
	uint32_t		dwID;				// 00 
	uint8_t 		byExtIndex;			// 01
	std::string		szName;				// 02
	std::string		szRemark;			// 03 

	uint32_t		dwIDK0;				// 04
	uint8_t			byIDK1;				// 05 

	uint32_t		dwIDResrc;			// 06
	uint32_t		dwIDIcon;			// 07 
	uint32_t		dwSoundID0;			// 08 
	uint32_t		dwSoundID1;			// 09 

	uint8_t			byClass;			// 10
	uint8_t			byIsRobeType;		// 11 
	uint8_t			byAttachPoint;		// 12 
	uint8_t			byNeedRace;			// 13
	uint8_t			byNeedClass;		// 14 

	int16_t			siDamage;			// 15 
	int16_t			siAttackInterval;	// 15 
	int16_t			siAttackRange;		// 16 
	int16_t			siWeight;			// 17 
	int16_t			siMaxDurability;	// 18 
	int				iPrice;				// 19
	int				iPriceSale;			// 20 
	int16_t			siDefense;			// 21 
	uint8_t			byContable;			// 22

	uint32_t		dwEffectID1;		// 22 ID1
	uint32_t		dwEffectID2;		// 23 ID1

	char			cNeedLevel;			// 24 
	char			cIDK2;				// 25

	uint8_t			byNeedRank;			// 26 iRank
	uint8_t			byNeedTitle;		// 27 iTitle
	uint8_t			byNeedStrength;		// 28 iStrength
	uint8_t			byNeedStamina;		// 29 iStamina
	uint8_t			byNeedDexterity;	// 30 iDexterity
	uint8_t			byNeedInteli;		// 31 iIntelligence
	uint8_t			byNeedMagicAttack;	// 32 iMagicAttak
	uint8_t			bySellGroup;		// 33

	uint8_t			byIDK3;				// 34
	int				byIDK4;				// 35
	uint32_t		byIDK5;				// 36

} 
TABLE_ITEM_BASIC;

const int MAX_ITEM_EXTENSION = 23;
const int LIMIT_FX_DAMAGE = 64;
const int ITEM_LIMITED_EXHAUST = 17;

typedef struct __TABLE_ITEM_EXT
{
	uint32_t		ItemIndex;			// 00 ItemIndex
	std::string		szHeader;			// 01 

	uint32_t		dwBaseID;

	std::string		szRemark;			// 02

	uint32_t		dwIDK0; 
	uint32_t		dwIDResrc;
	uint32_t		dwIDIcon;

	uint8_t			byMagicOrRare;		// 03

	int16_t	siDamage;				// 04
	int16_t	siAttackIntervalPercentage;		// 05
	int16_t	siHitRate;				// 06
	int16_t	siEvationRate;			// 07

	int16_t	siMaxDurability;		// 08
	int16_t	siPriceMultiply;		// 09
	int16_t	siDefense;				// 10
	
	int16_t	siDefenseRateDagger;	// 11
	int16_t	siDefenseRateSword;		// 12 
	int16_t	siDefenseRateBlow;		// 13
	int16_t	siDefenseRateAxe;		// 14 
	int16_t	siDefenseRateSpear;		// 15 
	int16_t	siDefenseRateArrow;		// 16
	
	uint8_t	byDamageFire;			// 17
	uint8_t	byDamageIce;			// 18
	uint8_t	byDamageThuner;			// 19
	uint8_t	byDamagePoison;			// 20

	uint8_t	byStillHP;				// 21 HP
	uint8_t	byDamageMP;				// 22 MP Damage
	uint8_t	byStillMP;				// 23 MP
	uint8_t	byReturnPhysicalDamage;	// 24

	uint8_t	bySoulBind;				// 25
	
	int16_t	siBonusStr;				// 26
	int16_t	siBonusSta;				// 27
	int16_t	siBonusDex;				// 28
	int16_t	siBonusInt;				// 29 
	int16_t	siBonusMagicAttak;		// 30
	int16_t	siBonusHP;				// 31 HP
	int16_t	siBonusMSP;				// 32 MSP

	int16_t	siRegistFire;			// 33
	int16_t	siRegistIce;			// 34
	int16_t	siRegistElec;			// 35
	int16_t	siRegistMagic;			// 36
	int16_t	siRegistPoison;			// 37
	int16_t	siRegistCurse;			// 38
	
	uint32_t	dwEffectID1;		// 39
	uint32_t	dwEffectID2;		// 40

	int16_t	siNeedLevel;			// 41 iLeve
	int16_t	siNeedRank;				// 42 iRank
	int16_t	siNeedTitle;			// 43 iTitle
	int16_t	siNeedStrength;			// 44 iStrength
	int16_t	siNeedStamina;			// 45 iStamina
	int16_t	siNeedDexterity;		// 46 iDexterity
	int16_t	siNeedInteli;			// 47 iIntelligence
	int16_t	siNeedMagicAttack;		// 48 iMagicAttak
} 
TABLE_ITEM_EXT;

const int MAX_NPC_SHOP_ITEM = 30;

typedef struct __TABLE_NPC_SHOP
{
	uint32_t		dwNPCID;
	std::string		szName;
	uint32_t		dwItems[MAX_NPC_SHOP_ITEM];
} 
TABLE_NPC_SHOP;

enum e_ItemType 
{ 
	ITEM_TYPE_PLUG = 1, 
	ITEM_TYPE_PART, 
	ITEM_TYPE_ICONONLY, 
	ITEM_TYPE_GOLD = 9, 
	ITEM_TYPE_SONGPYUN = 10, 
	ITEM_TYPE_UNKNOWN = 0xffffffff 
};

enum e_ItemPosition 
{	
	ITEM_POS_DUAL			= 0,
	ITEM_POS_RIGHTHAND		= 1,
	ITEM_POS_LEFTHAND		= 2,
	ITEM_POS_TWOHANDRIGHT	= 3,
	ITEM_POS_TWOHANDLEFT	= 4,
	ITEM_POS_UPPER			= 5, 
	ITEM_POS_LOWER			= 6,
	ITEM_POS_HEAD			= 7,
	ITEM_POS_GLOVES			= 8,
	ITEM_POS_SHOES			= 9,
	ITEM_POS_EAR			= 10, 
	ITEM_POS_NECK			= 11,
	ITEM_POS_FINGER			= 12,
	ITEM_POS_SHOULDER		= 13,
	ITEM_POS_BELT			= 14,
	ITEM_POS_INVENTORY		= 15,
	ITEM_POS_GOLD			= 16,
	ITEM_POS_SONGPYUN		= 17,
	ITEM_POS_UNKNOWN = 0xffffffff
};
					
enum e_ItemSlot 
{	
	ITEM_SLOT_EAR_RIGHT = 0, 
	ITEM_SLOT_HEAD = 1, 
	ITEM_SLOT_EAR_LEFT = 2,
	ITEM_SLOT_NECK = 3, 
	ITEM_SLOT_UPPER = 4, 
	ITEM_SLOT_SHOULDER = 5,
	ITEM_SLOT_HAND_RIGHT = 6, 
	ITEM_SLOT_BELT = 7, 
	ITEM_SLOT_HAND_LEFT = 8,
	ITEM_SLOT_RING_RIGHT = 9, 
	ITEM_SLOT_LOWER = 10, 
	ITEM_SLOT_RING_LEFT = 11,
	ITEM_SLOT_GLOVES = 12, 
	ITEM_SLOT_SHOES = 13,
	ITEM_SLOT_COUNT = 14, 
	ITEM_SLOT_UNKNOWN = 0xffffffff
};


typedef struct __TABLE_PLAYER_LOOKS 
{
	uint32_t		iNum;
	std::string		szName; 
	std::string		szJointFN; 
	std::string		szAniFN; 
	std::string		szPartFNs[13]; 

	int				iIdk1;

	int				iJointRH;			
	int				iJointLH;			
	int				iJointLH2;			
	int				iJointCloak;		
	
	int				iSndID_Move;
	int				iSndID_Attack0;
	int				iSndID_Attack1;
	int				iSndID_Struck0;
	int				iSndID_Struck1;
	int				iSndID_Dead0;
	int				iSndID_Dead1;
	int				iSndID_Breathe0;
	int				iSndID_Breathe1;
	int				iSndID_Reserved0;
	int				iSndID_Reserved1;

	int				iIdk2;
	int				iIdk3;
	uint8_t			bIdk4;
	uint8_t			bIdk5;
	uint8_t			bIdk6;
} 
TABLE_PLAYER;

typedef struct __TABLE_EXCHANGE_QUEST
{
	uint32_t		QuestIndex;				//0
	uint32_t		dwNpcNum;				//1
	std::string		szDesc;					//2
	int				iCondition0;			//3
	int				iCondition1;			//4
	int				iCondition2;			//5
	int				iCondition3;			//6
	int				iNeedGold;				//7	
	uint8_t			bNeedLevel;				//8
	uint8_t			bNeedClass;				//9
	uint8_t			bNeedRank;				//10	
	uint8_t			bNeedExtra1;			//11	
	uint8_t			bNeedExtra2;			//12
	uint8_t			bCreatePercentage;		//13
	int				iArkTuarek;				//14	
	int				iTuarek;				//15
	int				iRinkleTuarek;			//16
	int				iBabarian;				//17
	int				iMan;					//18
	int				iWoman;					//19
} 
TABLE_EXCHANGE_QUEST;

typedef struct _TB_TABLE_UPC_SKILL
{
	uint32_t		SkllNum;			//1
	std::string		szEngName;			//2
	std::string		szName;				//3
	std::string		szDesc;				//4
	int				iSelfAnimID1;		//5
	int				iSelfAnimID2;		//6

	int				idwTargetAnimID;	//7
	int				iSelfFX1;			//8
	int				iSelfPart1;			//9
	int				iSelfFX2;			//10
	int				iSelfPart2;			//11
	int				iFlyingFX;			//12
	int				iTargetFX;			//13

	int				iTargetPart;		//14
	int				iTarget;			//15
	int				iNeedLevel;			//16
	int				iNeedSkill;			//17
	int				iExhaustMSP;		//18

	int				iExhaustHP;			//19
	uint32_t		dwNeedItem;			//20
	uint32_t		dwExhaustItem;
	int				iCastTime;			//21
	int				iReCastTime;		//22

	float			fIDK0;				//23
	float			fIDK1;				//24

	int				iPercentSuccess;	//25
	uint32_t		dw1stTableType;		//26
	uint32_t		dw2ndTableType;		//27
	int				iValidDist;			//28

	int				iIDK2;				//29
	int				iIDK3;				//30
	int				iIDK4;				//31
	int				iIDK5;				//32

} 
TABLE_UPC_ATTACK_B;

typedef struct __TABLE_UPC_SKILL_TYPE_1
{
	uint32_t		SkllNum;			
	int				iSuccessType;	
	int				iSuccessRatio;	
	int				iPower;			
	int				iDelay;			
	int				iComboType;		
	int				iNumCombo;		
	int				iComboDamage;				
	int				iValidAngle;	
	int				iAct[3];
} 
TABLE_UPC_SKILL_TYPE_1;

typedef struct __TABLE_UPC_SKILL_TYPE_2
{
	uint32_t		SkllNum;			
	int				iSuccessType;	
	int				iPower;			
	int				iIdk1;
	int				iAddDist;		
	int				iNumArrow;		
} 
TABLE_UPC_SKILL_TYPE_2;

typedef struct __TABLE_UPC_SKILL_TYPE_3
{
	uint32_t		SkllNum;
	int				iIdk1;
	int				iDDType;
	int				iStartDamage;
	int				iDuraDamage;
	int				iDurationTime;
	int				iAttribute;
} 
TABLE_UPC_SKILL_TYPE_3;

typedef struct __TABLE_UPC_SKILL_TYPE_4
{
	uint32_t	SkllNum;
	int			iBuffType;		
	int			iIDK1;
	int			iDuration;
	int			iAttackSpeed;	
	int			iMoveSpeed;		
	int			iAC;			
	int			iAttack;		
	int			iMaxHP;		
	int			iStr;			
	int			iSta;			
	int			iDex;			
	int			iInt;			
	int			iMAP;			
	int			iFireResist;
	int			iColdResist;	
	int			iLightningResist;
	int			iMagicResist;	
	int			iDeseaseResist;	
	int			iPoisonResist;	

	int			iIdk0;
	int			iIdk2;
	int			iIdk3;
	int			iIdk4;
	int			iIdk5;
	int			iIdk6;
} 
TABLE_UPC_SKILL_TYPE_4;

typedef struct __TABLE_UPC_SKILL_TYPE_5
{
	uint32_t		SkllNum;
	uint32_t		dwTarget;		
	int				iSuccessRatio;	
	int				iValidDist;		
	int				iRadius;		
	float			fCastTime;		
	float			fRecastTime;	
	int				iDurationTime;
	uint32_t		dwExhaustItem;	
	uint32_t		dwFX;			
} 
TABLE_UPC_SKILL_TYPE_5;

typedef struct __TABLE_UPC_SKILL_TYPE_6
{
	uint32_t		SkllNum;
	uint32_t		dwTarget;		
	int				iSuccessRatio;	
	int				iValidDist;		
	int				iRadius;		
	float			fCastTime;		
	float			fRecastTime;	
	int				iDurationTime;	
	uint32_t		dwExhaustItem;	
	uint32_t		dwFX;			
	uint32_t		dwTranform;		
} 
TABLE_UPC_SKILL_TYPE_6;

typedef struct __TABLE_UPC_SKILL_TYPE_7
{
	uint32_t		SkllNum;
	uint32_t		dwTarget;		
	uint32_t		dwValidGroup;	
	int				iSuccessRatio;	
	int				iValidDist;		
	int				iRadius;	
	float			fCastTime;		
	float			fRecastTime;	
	int				iDurationTime;	
	uint32_t		dwExhaustItem;	
	uint32_t		dwFX;		
}
TABLE_UPC_SKILL_TYPE_7;

typedef struct __TABLE_UPC_SKILL_TYPE_8
{
	uint32_t		SkllNum;
	uint32_t		dwTarget;		
	int				iRadius;		
	uint32_t		dwWarpType;		
	float			fRefillEXP;		
	uint32_t		dwZone1;		
	uint32_t		dwZone2;		
	uint32_t		dwZone3;		
	uint32_t		dwZone4;		
	uint32_t		dwZone5;
} 
TABLE_UPC_SKILL_TYPE_8;

typedef struct __TABLE_UPC_SKILL_TYPE_9
{
	uint32_t		SkllNum;
	uint32_t		dwTarget;		
	int				iSuccessRatio;	
	int				iValidDist;		
	int				iRadius;		
	float			fCastTime;		
	float			fRecastTime;	
	int				iDurationTime;	
	uint32_t		dwExhaustItem;	
	uint32_t		dwAttr;			
	int				iDamage;		
} 
TABLE_UPC_SKILL_TYPE_9;

typedef struct __TABLE_UPC_SKILL_TYPE_10
{
	uint32_t		SkllNum;
	uint32_t		dwTarget;		
	int				iSuccessRatio;	
	int				iValidDist;		
	int				iRadius;		
	float			fCastTime;		
	float			fRecastTime;	
	uint32_t		dwExhaustItem;	
	uint32_t		dwRemoveAttr;
} 
TABLE_UPC_SKILL_TYPE_10;

typedef struct __TABLE_QUEST_MENU
{
	uint32_t		dwID;		
	std::string szMenu;
} 
TABLE_QUEST_MENU;

typedef struct _TBL_TABLE_QUEST_TALK
{
	uint32_t		dwID;	
	std::string		szTalk;
	uint32_t		No1;
	uint32_t		No2;
} 
TABLE_QUEST_TALK;

const int MAX_ITEM_SLOT_OPC			= 8;
const int MAX_ITEM_INVENTORY		= 28;		
const int MAX_ITEM_TRADE			= 24;	
const int MAX_ITEM_TRADE_PAGE		= 12;
const int MAX_ITEM_WARE_PAGE		= 8;
const int MAX_ITEM_PER_TRADE		= 12;
const int MAX_ITEM_BUNDLE_DROP_PIECE = 6;
const int MAX_ITEM_EX_RE_NPC		 = 4; 

const int MAX_SKILL_FROM_SERVER		= 9;	
const int MAX_SKILL_KIND_OF			= 5;			
const int MAX_SKILL_IN_PAGE			= 6;				
const int MAX_SKILL_PAGE_NUM		= 3;			
const int MAX_SKILL_HOTKEY_PAGE		= 8;				
const int MAX_SKILL_IN_HOTKEY		= 8;				

const int MAX_AVAILABLE_CHARACTER	= 3;		

const int ID_SOUND_ITEM_ETC_IN_INVENTORY	= 2000;
const int ID_SOUND_ITEM_IN_REPAIR			= 2001;
const int ID_SOUND_ITEM_WEAPON_IN_INVENTORY = 2002;
const int ID_SOUND_ITEM_ARMOR_IN_INVENTORY	= 2003;
const int ID_SOUND_GOLD_IN_INVENTORY		= 3000;
const int ID_SOUND_SKILL_THROW_ARROW		= 5500;
const int ID_SOUND_BGM_TOWN					= 20000;
const int ID_SOUND_BGM_KA_BATTLE			= 20002;
const int ID_SOUND_BGM_EL_BATTLE			= 20003;
const int ID_SOUND_CHR_SELECT_ROTATE		= 2501;

const float SOUND_RANGE_TO_SET = 10.0f;
const float SOUND_RANGE_TO_RELEASE = 20.0f;

const float STUN_TIME = 3.0f;

enum e_Behavior 
{	
	BEHAVIOR_NOTHING = 0,
	BEHAVIOR_EXIT,			
	BEHAVIOR_RESTART_GAME,	
	BEHAVIOR_REGENERATION,	
	BEHAVIOR_CANCEL,		

	BEHAVIOR_PARTY_PERMIT,	
	BEHAVIOR_PARTY_DISBAND, 
	BEHAVIOR_FORCE_PERMIT,	
	BEHAVIOR_FORCE_DISBAND, 

	BEHAVIOR_REQUEST_BINDPOINT,

	BEHAVIOR_DELETE_CHR,

	BEHAVIOR_KNIGHTS_CREATE,
	BEHAVIOR_KNIGHTS_DESTROY, 
	BEHAVIOR_KNIGHTS_WITHDRAW, 

	BEHAVIOR_PERSONAL_TRADE_FMT_WAIT,	
	BEHAVIOR_PERSONAL_TRADE_PERMIT,	

	BEHAVIOR_MGAME_LOGIN,
					
	BEHAVIOR_CLAN_JOIN,
	BEHAVIOR_PARTY_BBS_REGISTER,		
	BEHAVIOR_PARTY_BBS_REGISTER_CANCEL,

	BEHAVIOR_EXECUTE_OPTION,
				
	BEHAVIOR_UNKNOWN = 0xffffffff
};

enum e_SkillMagicTaget	
{	
	SKILLMAGIC_TARGET_SELF = 1,				
	SKILLMAGIC_TARGET_FRIEND_WITHME = 2,	
	SKILLMAGIC_TARGET_FRIEND_ONLY = 3,		
	SKILLMAGIC_TARGET_PARTY = 4,			
	SKILLMAGIC_TARGET_NPC_ONLY = 5,			
	SKILLMAGIC_TARGET_PARTY_ALL = 6,		
	SKILLMAGIC_TARGET_ENEMY_ONLY = 7,		
	SKILLMAGIC_TARGET_ALL = 8,				
							
	SKILLMAGIC_TARGET_AREA_ENEMY = 10,		
	SKILLMAGIC_TARGET_AREA_FRIEND = 11,		
	SKILLMAGIC_TARGET_AREA_ALL = 12,		
	SKILLMAGIC_TARGET_AREA = 13,			
	SKILLMAGIC_TARGET_DEAD_FRIEND_ONLY = 25,
							
	SKILLMAGIC_TARGET_UNKNOWN = 0xffffffff
};

typedef struct _TBL_TABLE_FX
{
	uint32_t		EffectNum;	// Effect ID
	std::string		szName;		// NOTE: adding the name of the FX
	std::string		fxNamefxb;	// Fxb file name
	uint32_t		dwSoundID;	// sound id
#if __VERSION > 1264
	uint8_t			byAOE;		// AOE ??
#endif
} 
TABLE_FX;

const int	MAX_COMBO = 3;

const int   FXID_CLASS_CHANGE			= 603;
const int	FXID_BLOOD					= 10002;
const int	FXID_LEVELUP_KARUS			= 10012;
const int	FXID_LEVELUP_ELMORAD		= 10018;
const int	FXID_REGEN_ELMORAD			= 10019;
const int	FXID_REGEN_KARUS			= 10020;
const int	FXID_SWORD_FIRE_MAIN		= 10021;
const int	FXID_SWORD_FIRE_TAIL		= 10022;
const int	FXID_SWORD_FIRE_TARGET		= 10031;
const int	FXID_SWORD_ICE_MAIN			= 10023;
const int	FXID_SWORD_ICE_TAIL			= 10024;
const int	FXID_SWORD_ICE_TARGET		= 10032;
const int	FXID_SWORD_LIGHTNING_MAIN	= 10025;
const int	FXID_SWORD_LIGHTNING_TAIL	= 10026;
const int	FXID_SWORD_LIGHTNING_TARGET = 10033;
const int	FXID_SWORD_POISON_MAIN		= 10027;
const int	FXID_SWORD_POISON_TAIL		= 10028;
const int	FXID_SWORD_POISON_TARGET	= 10034;
//const int	FXID_GROUND_TARGET = 10035;
const int	FXID_REGION_TARGET_EL_ROGUE		= 10035;
const int	FXID_REGION_TARGET_EL_WIZARD	= 10036;
const int	FXID_REGION_TARGET_EL_PRIEST	= 10037;
const int	FXID_REGION_TARGET_KA_ROGUE		= 10038;
const int	FXID_REGION_TARGET_KA_WIZARD	= 10039;
const int	FXID_REGION_TARGET_KA_PRIEST	= 10040;
const int	FXID_CLAN_RANK_1				= 10041;
const int	FXID_WARP_KARUS					= 10046;
const int	FXID_WARP_ELMORAD				= 10047;
const int	FXID_REGION_POISON				= 10100;

enum e_SkillMagicType4	
{	
	BUFFTYPE_MAXHP = 1,				
	BUFFTYPE_AC = 2,				
	BUFFTYPE_RESIZE = 3,			
	BUFFTYPE_ATTACK = 4,			
	BUFFTYPE_ATTACKSPEED = 5,		
	BUFFTYPE_SPEED = 6,				
	BUFFTYPE_ABILITY = 7,			
	BUFFTYPE_RESIST = 8,			
	BUFFTYPE_HITRATE_AVOIDRATE = 9,	
	BUFFTYPE_TRANS = 10,			
	BUFFTYPE_SLEEP = 11,			
	BUFFTYPE_EYE = 12									
};

enum e_SkillMagicType3	
{	
	DDTYPE_TYPE3_DUR_OUR	= 100,
	DDTYPE_TYPE3_DUR_ENEMY  = 200
};

enum e_ObjectType	
{	
	OBJECT_TYPE_BINDPOINT,
	OBJECT_TYPE_DOOR_LEFTRIGHT,
	OBJECT_TYPE_DOOR_TOPDOWN,
	OBJECT_TYPE_LEVER_TOPDOWN,
	OBJECT_TYPE_FLAG,
	OBJECT_TYPE_WARP_POINT,
	OBJECT_TYPE_UNKNOWN = 0xffffffff
};

//definitions related clan....
const int	CLAN_LEVEL_LIMIT	= 20;
const int	CLAN_COST			= 500000;
const uint32_t KNIGHTS_FONT_COLOR	= 0xffff0000; // 기사단(클랜)이름 폰트 컬러..

enum e_Cursor		
{	
	CURSOR_ATTACK,
	CURSOR_EL_NORMAL,
	CURSOR_EL_CLICK,
	CURSOR_KA_NORMAL,
	CURSOR_KA_CLICK,
	CURSOR_PRE_REPAIR,
	CURSOR_NOW_REPAIR,
	CURSOR_COUNT,
	CURSOR_UNKNOWN = 0xffffffff
};

#endif // end of #define __GAME_DEF_H_


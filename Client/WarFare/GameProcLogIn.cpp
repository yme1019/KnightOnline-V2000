#include "resource.h"
#include "GameEng.h"
#include "GameProcLogIn.h"
#include "UILogIn.h"
#include "PlayerMySelf.h"
#include "UIManager.h"
#include "LocalInput.h"
#include "APISocket.h"
#include "PacketDef.h"

#include "N3Camera.h"
#include "N3Light.h"
#include "N3Chr.h"
#include "N3SndObj.h"
#include "N3SndObjStream.h"
#include "N3SndMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CGameProcLogIn::CGameProcLogIn()
{
	m_pUILogIn	= NULL;
	m_pChr			= NULL;
	m_pTexBkg		= NULL;

	m_pCamera = NULL;
	for(int i = 0; i < 3; i++) m_pLights[i] = NULL;

	m_bLogIn = false;
}

CGameProcLogIn::~CGameProcLogIn()
{
	delete m_pUILogIn;
	delete m_pChr;
	delete m_pTexBkg;

	delete m_pCamera;
	for(int i = 0; i < 3; i++) delete m_pLights[i];
}

void CGameProcLogIn::Release()
{
	CGameProcedure::Release();

	delete m_pUILogIn; m_pUILogIn = NULL;
	delete m_pChr; m_pChr = NULL;
	delete m_pTexBkg; m_pTexBkg = NULL;

	delete m_pCamera; m_pCamera = NULL;
	for(int i = 0; i < 3; i++) { delete m_pLights[i]; m_pLights[i] = NULL; }
}

void CGameProcLogIn::Init()
{
	CGameProcedure::Init();

	m_pTexBkg = new CN3Texture();
	m_pTexBkg->LoadFromFile("Intro\\Moon.dxt");

	m_pChr = new CN3Chr();
	m_pChr->LoadFromFile("Intro\\Intro.N3Chr");
	m_pChr->AniCurSet(0);
	
	m_pCamera = new CN3Camera();
	m_pCamera->EyePosSet(0.22f, 0.91f, -1.63f);
	m_pCamera->AtPosSet(-0.19f, 1.1f, 0.09f);
	m_pCamera->m_Data.fNP = 0.1f;
	m_pCamera->m_Data.fFP = 32.0f;
	m_pCamera->m_bFogUse = false;

	for(int i = 0; i < 3; i++) m_pLights[i] = new CN3Light();
	m_pLights[0]->LoadFromFile("Intro\\0.N3Light");
	m_pLights[1]->LoadFromFile("Intro\\1.N3Light");
	m_pLights[2]->LoadFromFile("Intro\\2.N3Light");

	s_pEng->s_SndMgr.ReleaseStreamObj(&(CGameProcedure::s_pSnd_BGM));
	CGameProcedure::s_pSnd_BGM = s_pEng->s_SndMgr.CreateStreamObj(35);	

	m_pUILogIn = new CUILogIn();
	m_pUILogIn->Init(s_pUIMgr);
	
	_TBL_TABLE_UI* pTbl = file_Tbl_UI.GetIndexedData(0);
	//TODO(srmeier) : the UIE saves in the old format...
	if (pTbl) m_pUILogIn->LoadFromFile(pTbl->Co_login_intro_us);

	RECT rc = m_pUILogIn->GetRegion();
	int iX = (CN3Base::s_CameraData.vp.Width - (rc.right - rc.left))/2;
	int iY = CN3Base::s_CameraData.vp.Height - (rc.bottom - rc.top);
	m_pUILogIn->SetPos(iX, iY);
	m_pUILogIn->RecalcGradePos();
	rc.left = 0; rc.top = 0; rc.right = CN3Base::s_CameraData.vp.Width; rc.bottom = CN3Base::s_CameraData.vp.Height;
	m_pUILogIn->SetRegion(rc);
	s_pUIMgr->SetFocusedUI((CN3UIBase*)m_pUILogIn);

	char szIniPath[_MAX_PATH] = "";
	lstrcpy(szIniPath, CN3Base::PathGet().c_str());
	lstrcat(szIniPath, "Server.Ini");

	char szRegistrationSite[_MAX_PATH];
	memset(szRegistrationSite, 0, sizeof(szRegistrationSite));

	GetPrivateProfileString("Join", "Registration site", "", szRegistrationSite, _MAX_PATH, szIniPath);
	m_szRegistrationSite = std::string(szRegistrationSite);

	int iServerCount = GetPrivateProfileInt("Server", "Count", 0, szIniPath);

	char szIPs[256][32]; memset(szIPs, 0, sizeof(szIPs));
	for(int i = 0; i < iServerCount; i++)
	{
		char szKey[32] = "";
		sprintf(szKey, "IP%d", i);
		GetPrivateProfileString("Server", szKey, "", szIPs[i], 32, szIniPath);
	}
	int iServer = -1;
	if(iServerCount > 0) iServer = rand()%iServerCount;
	
	if(	iServer >= 0 && lstrlen(szIPs[iServer]) )
	{
		s_bNeedReportConnectionClosed = false; 
		int iErr = s_pSocket->Connect(s_hWndBase, szIPs[iServer], SOCKET_PORT_LOGIN);
		s_bNeedReportConnectionClosed = true; 
		
		if(iErr) this->ReportServerConnectionFailed("LogIn Server", iErr, true);
		else
		{
			m_pUILogIn->FocusToID();			
			int iOffset = 0;
			uint8_t byBuffs[4];
			CAPISocket::MP_AddByte(byBuffs, iOffset, N3_GAMESERVER_GROUP_LIST);				
			s_pSocket->Send(byBuffs, iOffset);											
		}
	}
	else
	{
		this->MessageBoxPost("No server list", "LogIn Server fail", MB_OK, BEHAVIOR_EXIT); 
	}

	if(LIC_KNIGHTONLINE != s_eLogInClassification)
	{
		this->MsgSend_AccountLogIn(s_eLogInClassification);
	}
}

void CGameProcLogIn::Tick() 
{
	CGameProcedure::Tick();	

	for(int i = 0; i < 3; i++) 	m_pLights[i]->Tick();
	m_pChr->Tick();

	static float fTmp	= 0;
	if(fTmp == 0)
	{
		if(CGameProcedure::s_pSnd_BGM) CGameProcedure::s_pSnd_BGM->Play(); 
	}
	fTmp += CN3Base::s_fSecPerFrm;
	if(fTmp > 21.66f)
	{
		fTmp = 0;
		if(CGameProcedure::s_pSnd_BGM) CGameProcedure::s_pSnd_BGM->Stop();		
	}
}

void CGameProcLogIn::Render()
{
	D3DCOLOR crEnv = 0x00000000;
	s_pEng->Clear(crEnv); 
	s_pEng->s_lpD3DDev->BeginScene();	

//	__Vector3 vEye(0.22f, 0.91f, -1.63f), vAt(-0.19f, 1.1048f, 0.0975f), vUp(0,1,0);
//	__Matrix44 mtxView, mtxPrj, mtxWorld;
//	mtxWorld.Identity();

	m_pCamera->Tick();
	m_pCamera->Apply();
/*	D3DVIEWPORT8 vp;
	CN3Base::s_lpD3DDev->GetViewport(&vp);
	float fLens = D3DXToRadian(55.0f);
	float fAspect = (float)vp.Width / (float)vp.Height;
	float fNear = 0.1f;
	float fFar = 100.0f;

	::D3DXMatrixLookAtLH(&mtxView, &vEye, &vAt, &vUp);
	::D3DXMatrixPerspectiveFovLH(&mtxPrj, fLens, fAspect, fNear, fFar);
	CN3Base::s_lpD3DDev->SetTransform(D3DTS_VIEW, &mtxView); 
	CN3Base::s_lpD3DDev->SetTransform(D3DTS_PROJECTION, &mtxPrj); 
	CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtxWorld); 
*/

	for(int i = 0; i < 8; i++) 	CN3Base::s_lpD3DDev->LightEnable(i, FALSE);
	for(int i = 0; i < 3; i++) 	m_pLights[i]->Apply();

/*	D3DLIGHT8 lgt0, lgt1, lgt2;
	
	memset(&lgt0, 0, sizeof(D3DLIGHT8));
	lgt0.Type = D3DLIGHT_POINT;
	lgt0.Attenuation0 = 0.5f;
	lgt0.Range = 100000.0f;
	lgt0.Position = __Vector3(-500, 100, -50);
	lgt0.Diffuse.r = 232/255.0f; lgt0.Diffuse.g = 226/255.0f; lgt0.Diffuse.b = 215/255.0f;

	memset(&lgt1, 0, sizeof(D3DLIGHT8));
	lgt1.Type = D3DLIGHT_POINT;
	lgt1.Attenuation0 = 1.0f;
	lgt1.Range = 100000.0f;
	lgt1.Position = __Vector3(1000, -300, -50);
//	lgt1.Ambient.r = 56/255.0f; lgt1.Ambient.g = 58/255.0f; lgt1.Ambient.b = 129/255.0f;
	lgt1.Diffuse.r = 66/255.0f; lgt1.Diffuse.g = 68/255.0f; lgt1.Diffuse.b = 168/255.0f;

	memset(&lgt2, 0, sizeof(D3DLIGHT8));
	lgt2.Type = D3DLIGHT_POINT;
	lgt2.Attenuation0 = 1.0f;
	lgt2.Range = 100000.0f;
	lgt2.Position = __Vector3(-200, -800, 350);
//	lgt2.Ambient.r = 52/255.0f; lgt2.Ambient.g = 56/255.0f; lgt2.Ambient.b = 107/255.0f;
	lgt2.Diffuse.r = 124/255.0f; lgt2.Diffuse.g = 45/255.0f; lgt2.Diffuse.b = 31/255.0f;

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	for(int i = 0; i < 8; i++)

	CN3Base::s_lpD3DDev->LightEnable(0, FALSE);
	CN3Base::s_lpD3DDev->LightEnable(0, TRUE);
	CN3Base::s_lpD3DDev->LightEnable(1, TRUE);
	CN3Base::s_lpD3DDev->LightEnable(2, TRUE);
	CN3Base::s_lpD3DDev->SetLight(0, &lgt0);
	CN3Base::s_lpD3DDev->SetLight(1, &lgt1);
	CN3Base::s_lpD3DDev->SetLight(2, &lgt2);
*/
	D3DVIEWPORT9 vp;
	CN3Base::s_lpD3DDev->GetViewport(&vp);

	float fMW = (m_pTexBkg->Width() * vp.Width / 1024.0f)*1.3f;
	float fMH = (m_pTexBkg->Height() * vp.Height / 768.0f)*1.3f;
	float fX = 100.0f * vp.Width / 1024.0f;
	float fY = 50.0f * vp.Height / 768.0f;

	float fRHW = 1.0f;
	__VertexTransformed vMoon[4];
	vMoon[0].Set(fX,     fY,     0, fRHW, 0xffffffff, 0.0f, 0.0f);
	vMoon[1].Set(fX+fMW, fY,     0, fRHW, 0xffffffff, 1.0f, 0.0f);
	vMoon[2].Set(fX+fMW, fY+fMH, 0, fRHW, 0xffffffff, 1.0f, 1.0f);
	vMoon[3].Set(fX,     fY+fMH, 0, fRHW, 0xffffffff, 0.0f, 1.0f);
	
	DWORD dwZWrite;
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWrite);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	CN3Base::s_lpD3DDev->SetTexture(0, m_pTexBkg->Get());
	CN3Base::s_lpD3DDev->SetFVF(FVF_TRANSFORMED);
	CN3Base::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vMoon, sizeof(__VertexTransformed));

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWrite);
	m_pChr->Render(); 

	CGameProcedure::Render(); 

	s_pEng->s_lpD3DDev->EndScene();			
	s_pEng->Present(CN3Base::s_hWndBase);
}

bool CGameProcLogIn::MsgSend_AccountLogIn(e_LogInClassification eLIC)
{
	if(LIC_KNIGHTONLINE == eLIC) 
	{
		m_pUILogIn->AccountIDGet(s_szAccount); 
		m_pUILogIn->AccountPWGet(s_szPassWord); 
	}
	if(	s_szAccount.empty() || s_szPassWord.empty() || s_szAccount.size() >= 20 || s_szPassWord.size() >= 12) return false;

	m_pUILogIn->SetVisibleLogInUIs(false);
	m_pUILogIn->SetRequestedLogIn(true);
	m_bLogIn = true;

	uint8_t byBuff[256];										
	int iOffset=0;										

	uint8_t byCmd = N3_ACCOUNT_LOGIN;
	if(LIC_KNIGHTONLINE == eLIC) byCmd = N3_ACCOUNT_LOGIN;
	else if(LIC_MGAME == eLIC) byCmd = N3_ACCOUNT_LOGIN_MGAME;
//	else if(LIC_DAUM == eLIC) byCmd = N3_ACCOUNT_LOGIN_DAUM;

	CAPISocket::MP_AddByte(byBuff, iOffset, byCmd);				
	CAPISocket::MP_AddShort(byBuff, iOffset, (int16_t)s_szAccount.size());	
	CAPISocket::MP_AddString(byBuff, iOffset, s_szAccount);		
	CAPISocket::MP_AddShort(byBuff, iOffset, (int16_t)s_szPassWord.size());	
	CAPISocket::MP_AddString(byBuff, iOffset, s_szPassWord);		
		
	s_pSocket->Send(byBuff, iOffset);								

	return true;
}

void CGameProcLogIn::MsgRecv_GameServerGroupList(Packet& pkt)
{
	int iServerCount = pkt.read<uint8_t>();
	for(int i = 0; i < iServerCount; i++)
	{
		int iLen = 0;
		__GameServerInfo GSI;
		iLen = pkt.read<int16_t>();
		pkt.readString(GSI.szIP, iLen);
		iLen = pkt.read<int16_t>();
		pkt.readString(GSI.szName, iLen);
		GSI.iConcurrentUserCount = pkt.read<int16_t>();	
		m_pUILogIn->ServerInfoAdd(GSI); // ServerList
	}
	m_pUILogIn->ServerInfoUpdate();
}

void CGameProcLogIn::MsgRecv_AccountLogIn(int iCmd, Packet& pkt)
{
	int iResult = pkt.read<uint8_t>(); // Recv
	if(1 == iResult)
	{
		this->MessageBoxClose(-1);
		m_pUILogIn->OpenServerList(); 
	}
	else if(2 == iResult) // ID 
	{
		if(N3_ACCOUNT_LOGIN == iCmd)
		{
			std::string szMsg;
			std::string szTmp;
			::_LoadStringFromResource(IDS_NOACCOUNT_RETRY_MGAMEID, szMsg);
			::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);

			this->MessageBoxPost(szMsg, szTmp, MB_YESNO, BEHAVIOR_MGAME_LOGIN); // MGame ID 
		}
		else
		{
			std::string szMsg;
			std::string szTmp;
			::_LoadStringFromResource(IDS_NO_MGAME_ACCOUNT, szMsg);
			::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);

			this->MessageBoxPost(szMsg, szTmp, MB_OK); // MGame ID 
		}
	}
	else if(3 == iResult)
	{
		std::string szMsg;
		std::string szTmp;
		::_LoadStringFromResource(IDS_WRONG_PASSWORD, szMsg);
		::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);
		this->MessageBoxPost(szMsg, szTmp, MB_OK); 
	}
	else if(4 == iResult) 
	{
		std::string szMsg;
		std::string szTmp;
		::_LoadStringFromResource(IDS_SERVER_CONNECT_FAIL, szMsg);
		::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);
		this->MessageBoxPost(szMsg, szTmp, MB_OK); 
	}
	else if(5 == iResult) 
	{
		int iLen = pkt.read<int16_t>();
		if (iLen > 0)
		{
			std::string szIP;
			pkt.readString(szIP, iLen);
			uint32_t dwPort = pkt.read<int16_t>();

			CAPISocket socketTmp;
			s_bNeedReportConnectionClosed = false;
			if(0 == socketTmp.Connect(s_hWndBase, szIP.c_str(), dwPort))
			{
				int iOffset2 = 0;
				uint8_t Buff[32];
				CAPISocket::MP_AddByte(Buff, iOffset2, WIZ_KICKOUT); // Recv s1, str1(IP) s1(port) | Send s1, str1(ID)
				CAPISocket::MP_AddShort(Buff, iOffset2, (int16_t)s_szAccount.size());
				CAPISocket::MP_AddString(Buff, iOffset2, s_szAccount); // Recv s1, str1(IP) s1(port) | Send s1, str1(ID)
				
				socketTmp.Send(Buff, iOffset2);
				socketTmp.Disconnect(); 
			}
			s_bNeedReportConnectionClosed = true; 

			std::string szMsg;
			std::string szTmp;
			::_LoadStringFromResource(IDS_LOGIN_ERR_ALREADY_CONNECTED_ACCOUNT, szMsg);
			::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);
			this->MessageBoxPost(szMsg, szTmp, MB_OK); 
		}
	}
	else
	{
		std::string szMsg;
		std::string szTmp;
		::_LoadStringFromResource(IDS_CURRENT_SERVER_ERROR, szMsg);
		::_LoadStringFromResource(IDS_CONNECT_FAIL, szTmp);
		this->MessageBoxPost(szMsg, szTmp, MB_OK); 
	}

	if(1 != iResult) 
	{
		m_pUILogIn->SetVisibleLogInUIs(true); 
		m_pUILogIn->SetRequestedLogIn(false);
		m_bLogIn = false;
	}
}

int CGameProcLogIn::MsgRecv_VersionCheck(Packet& pkt) // virtual
{
	int iVersion = CGameProcedure::MsgRecv_VersionCheck(pkt);
	if(iVersion == CURRENT_VERSION)
	{
		CGameProcedure::MsgSend_GameServerLogIn();
		m_pUILogIn->ConnectButtonSetEnable(false);
	}

	return iVersion;
}

int CGameProcLogIn::MsgRecv_GameServerLogIn(Packet& pkt) 
{
	int iNation = CGameProcedure::MsgRecv_GameServerLogIn(pkt); 

	if( 0xff == iNation )
	{
		__GameServerInfo GSI;
		std::string szFmt;
		::_LoadStringFromResource(IDS_FMT_GAME_SERVER_LOGIN_ERROR, szFmt);
		m_pUILogIn->ServerInfoGetCur(GSI);
		char szErr[256];
		sprintf(szErr, szFmt.c_str(), GSI.szName.c_str(), iNation);
		this->MessageBoxPost(szErr, "", MB_OK);
		m_pUILogIn->ConnectButtonSetEnable(true); 
	}
	else
	{
		if(0 == iNation) s_pPlayer->m_InfoBase.eNation = NATION_NOTSELECTED;
		else if(1 == iNation) s_pPlayer->m_InfoBase.eNation = NATION_KARUS;
		else if(2 == iNation) s_pPlayer->m_InfoBase.eNation = NATION_ELMORAD;
	}

	if ( NATION_NOTSELECTED == s_pPlayer->m_InfoBase.eNation )
	{
		s_pEng->s_SndMgr.ReleaseStreamObj(&(CGameProcedure::s_pSnd_BGM));
		CGameProcedure::s_pSnd_BGM = s_pEng->s_SndMgr.CreateStreamObj(ID_SOUND_BGM_EL_BATTLE);
		if(CGameProcedure::s_pSnd_BGM)
		{
			CGameProcedure::s_pSnd_BGM->Looping(true);
			CGameProcedure::s_pSnd_BGM->Play();
		}
		CGameProcedure::ProcActiveSet((CGameProcedure*)s_pProcNationSelect);
	}
	else if( NATION_KARUS == s_pPlayer->m_InfoBase.eNation || NATION_ELMORAD == s_pPlayer->m_InfoBase.eNation)
	{
		s_pEng->s_SndMgr.ReleaseStreamObj(&(CGameProcedure::s_pSnd_BGM));
		CGameProcedure::s_pSnd_BGM = s_pEng->s_SndMgr.CreateStreamObj(ID_SOUND_BGM_EL_BATTLE);
		if(CGameProcedure::s_pSnd_BGM)
		{
			CGameProcedure::s_pSnd_BGM->Looping(true);
			CGameProcedure::s_pSnd_BGM->Play();
		}
		CGameProcedure::ProcActiveSet((CGameProcedure*)s_pProcCharacterSelect);
	}

	return iNation;
}

bool CGameProcLogIn::ProcessPacket(Packet& pkt)
{
	size_t rpos = pkt.rpos();
	if (CGameProcedure::ProcessPacket(pkt))
		return true;

	pkt.rpos(rpos);

	s_pPlayer->m_InfoBase.eNation = NATION_UNKNOWN;
	int iCmd = pkt.read<uint8_t>();
	s_pPlayer->m_InfoBase.eNation = NATION_UNKNOWN;
	switch ( iCmd )										
	{
		case N3_GAMESERVER_GROUP_LIST: 
			this->MsgRecv_GameServerGroupList(pkt);
			return true;

		case N3_ACCOUNT_LOGIN: 
		case N3_ACCOUNT_LOGIN_MGAME: 
			this->MsgRecv_AccountLogIn(iCmd, pkt);
			return true;
	}

	return false;
}

void CGameProcLogIn::ConnectToGameServer()
{
	__GameServerInfo GSI;
	if(false == m_pUILogIn->ServerInfoGetCur(GSI)) 
		return; 

	s_bNeedReportConnectionClosed = false; 
	int iErr = s_pSocket->Connect(s_hWndBase, GSI.szIP.c_str(), SOCKET_PORT_GAME);
	s_bNeedReportConnectionClosed = true;
	
	if(iErr)
	{
		this->ReportServerConnectionFailed(GSI.szName, iErr, false);
		m_pUILogIn->ConnectButtonSetEnable(true);
	}
	else
	{
		s_szServer = GSI.szName;
		this->MsgSend_VersionCheck();
	}
}

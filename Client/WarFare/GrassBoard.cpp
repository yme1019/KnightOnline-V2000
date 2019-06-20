#include "GrassBoard.h"
#include "N3Terrain.h"
#include "N3Texture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CGrassBoard::CGrassBoard()
{
	m_dwType |= OBJ_BOARD;
	CGrassBoard::Release();
}

CGrassBoard::~CGrassBoard()
{
	this->Release();
}

void CGrassBoard::Release()
{
//	memset(m_vRects, 0, sizeof(m_vRects));

	m_nTexIndex = -1;
	m_ucTexIndex = 0;
	m_ucTexNum = 0;

	m_vCenterPo.Zero();

	m_dwBoardType = BOARD_Y;

	m_fBrightmin = 1.0f;
	m_fBrightmax = 1.0f;
	m_bCamOut = FALSE;

	CN3Transform::Release();
}

void CGrassBoard::ReCalcMatrix()
{
	Grass_Info* pGrass;
	for(int i=0;i<m_ucTexNum;++i)
	{
		pGrass = &m_sGrassInfo[i];

		pGrass->mtxWorld.Scale(m_vScale);
		if(m_qRot.w != 0)
		{
			static __Matrix44 mtxRot;
			D3DXMatrixRotationQuaternion(&mtxRot, &m_qRot);
			pGrass->mtxWorld *= mtxRot;
		}
		pGrass->mtxWorld.PosSet(m_vPos);
	}
}

void CGrassBoard::Tick(CN3Terrain* pTerrain)
{
	static __Vector3 vBakCam;
	if(vBakCam == s_CameraData.vEye) return;	
	vBakCam = s_CameraData.vEye;

	uint32_t dwAlpha;
	__Vector3 vDir;
	Grass_Info* pGrass;
	for(int i=0;i<m_ucTexNum;++i)
	{
		pGrass = &m_sGrassInfo[i];

		if(pGrass->vPos.y==0.0f)	
			pGrass->vPos.y = pTerrain->GetHeight(pGrass->vPos.x,pGrass->vPos.z);

		vDir = s_CameraData.vEye - pGrass->vPos;

		if(vDir.x > 0.0f) pGrass->mtxWorld.RotationY(-atanf(vDir.z/vDir.x) - (D3DX_PI * 0.5f));
		else pGrass->mtxWorld.RotationY(-atanf(vDir.z/vDir.x) + (D3DX_PI * 0.5f));
		pGrass->mtxWorld.PosSet(m_sGrassInfo[i].vPos);

		dwAlpha = SetBrightLevel(vDir.Magnitude());
		if(dwAlpha!=0x00000000) pGrass->dwAlpColor = dwAlpha;
	}
}

void CGrassBoard::Render(CN3Texture** ppTex)
{
	if(m_bCamOut==TRUE)
		return;	

	DWORD dwColorop, dwColorA1, dwColorA2;

	s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLOROP,   &dwColorop);
	s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorA1);
	s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorA2);

	static Grass_Info* pGrass;
	for(int i=0;i<m_ucTexNum;++i)
	{
		pGrass = &m_sGrassInfo[i];
		if(ppTex[pGrass->iTexIndex]==NULL) continue;

		s_lpD3DDev->SetTransform(D3DTS_WORLD, &pGrass->mtxWorld);

		s_lpD3DDev->SetTexture(0,ppTex[pGrass->iTexIndex]->Get());
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		s_lpD3DDev->SetFVF(FVF_XYZCOLORT1);

		static __VertexXyzColorT1 vRects[4];
		vRects[0].Set((-1.0f/2.0f), 1.0f, 0,  pGrass->dwAlpColor, 0.0f, 0.0f);
		vRects[1].Set( (1.0f/2.0f), 1.0f, 0,  pGrass->dwAlpColor, 1.0f, 0.0f);
		vRects[2].Set( (1.0f/2.0f),  0	, 0,  pGrass->dwAlpColor, 1.0f, 1.0f);
		vRects[3].Set((-1.0f/2.0f),  0	, 0,  pGrass->dwAlpColor, 0.0f, 1.0f);

		s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vRects, sizeof(__VertexXyzColorT1));
	}

	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   dwColorop);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorA1);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorA2);

}

void CGrassBoard::Init(__Vector3 vPos, uint32_t dwBoardType)
{
	m_vPos = vPos;
	m_dwBoardType = dwBoardType;
}

bool CGrassBoard::Load(HANDLE hFile)
{
	return CN3Transform::Load(hFile);
}

#ifdef _N3TOOL
bool CGrassBoard::Save(HANDLE hFile)
{
	return CN3Transform::Save(hFile);
}
#endif

void CGrassBoard::LoadFromFile(int iTexIndex,uint8_t ucTexOrgIndex,__Vector3 vPos)
{
	Release();

//	m_nTexIndex = iTexIndex;
//	m_usTexIndex = ucTexOrgIndex;
//	m_dwBoardType = BOARD_Y;
//	Init(vPos, m_dwBoardType, 0.7f, 0.7f);
//	Init(vPos, m_dwBoardType, 1.0f, 1.0f);
}

uint32_t CGrassBoard::SetBrightLevel(float Level)
{
	static float fLevelbak;
	if(fLevelbak == Level) return 0x00000000;	
	fLevelbak = Level;

	if(Level<0.0f) return 0x00ffffff;
	if(Level>m_fBrightmin+m_fBrightmax)	return 0x00ffffff;	
	
	uint32_t Color=0x00ffffff;
	if(Level>m_fBrightmin)	
	{
		float brightper = (Level-m_fBrightmin)/m_fBrightmax;
		uint32_t alphaColor = (uint32_t)(255 - 255*brightper);
		Color = (alphaColor << 24) | 0x00ffffff;
	}
	else Color = 0xffffffff;	

	return Color;
}

void CGrassBoard::SetInfo(__Vector3 vBoardPosion,uint16_t usData)
{
	m_vCenterPo.Set(vBoardPosion.x+2.0f , vBoardPosion.y , vBoardPosion.z+2.0f);	
	m_ucTexIndex = (uint8_t)((usData & 0xff00)>>8);	
	m_ucTexNum = (uint8_t)(usData&0x00ff);

	if(m_ucTexNum>20) m_ucTexNum = 20;

	int GrassInputCount;
	int InputGrass[8];
	FindGrassIndex(m_ucTexIndex,InputGrass,GrassInputCount);

	int iCount=0;
	Grass_Info* pGrass;
	int mRand;
	for(int i=0;i<m_ucTexNum;++i)
	{
		pGrass = &m_sGrassInfo[i];

		mRand = rand();	pGrass->vPos.x = (mRand%40)/10.0f + vBoardPosion.x;

		pGrass->dwAlpColor = 0x00ffffff;
		pGrass->iTexIndex = InputGrass[iCount];
		if(++iCount>=GrassInputCount) iCount=0;

		mRand = rand();	pGrass->vPos.z = (mRand%40)/10.0f+ vBoardPosion.z;
		pGrass->vPos.y = 0.0f;
	}
}

void CGrassBoard::FindGrassIndex(const uint8_t uCGrassMngOrder,int* pnInputGrass,int& nGrassTotNum)
{
	nGrassTotNum=0;
	if(uCGrassMngOrder & 0x01)  { pnInputGrass[nGrassTotNum] = 0 ; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x02)  { pnInputGrass[nGrassTotNum] = 1 ; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x04)  { pnInputGrass[nGrassTotNum] = 2 ; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x08)  { pnInputGrass[nGrassTotNum] = 3 ; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x10)  { pnInputGrass[nGrassTotNum] = 4 ; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x20)  { pnInputGrass[nGrassTotNum] = 5 ; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x40)  { pnInputGrass[nGrassTotNum] = 6 ; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x80)  { pnInputGrass[nGrassTotNum] = 7 ; nGrassTotNum++; }
}


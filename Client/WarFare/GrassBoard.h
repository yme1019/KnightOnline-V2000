#if !defined(AFX_GRASSBOARD_H__D53F0EC4_B777_49CD_BEE8_071AD4A1680E__INCLUDED_)
#define AFX_GRASSBOARD_H__D53F0EC4_B777_49CD_BEE8_071AD4A1680E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Transform.h"

const uint32_t BOARD_X				= 0;
const uint32_t BOARD_Y				= 1;
const uint32_t BOARD_Z				= 2;
const uint32_t BOARD_XYZ			= 3;

class CN3Terrain;

class CGrassBoard : public CN3Transform
{
protected:
	typedef struct Grass_Info
	{
		__Matrix44	mtxWorld;
		__Vector3	vPos;	
		uint32_t		dwAlpColor;	
		int			iTexIndex;	
	}Grass_Info;
	Grass_Info		m_sGrassInfo[20];

	int				m_nTexIndex;
	uint8_t	m_ucTexIndex;	
	uint8_t	m_ucTexNum;	

	__Vector3		m_vCenterPo;

public:
	uint32_t m_dwBoardType; 

	float	m_fBrightmin;	
	float	m_fBrightmax;	

	BOOL	m_bCamOut;	

	BOOL	m_bGroundInfo;	

public:

	void Init(__Vector3 vPos, uint32_t dwBoardType);
	void Tick(CN3Terrain* pTerrain);
	void Render(CN3Texture** ppTex);

	void ReCalcMatrix();

	bool Load(HANDLE hFile);
	void LoadFromFile(int iTexIndex,uint8_t ucTexOrgIndex,__Vector3 vPos);

	void SetBrightInit(float min,float max) {m_fBrightmin = min,m_fBrightmax = max - min;};
	uint32_t SetBrightLevel(float Level);
	
	void SetInfo(__Vector3 vBoardPosion,uint16_t usData);
	__Vector3	GetPo() {return m_vCenterPo;}

#ifdef _N3TOOL
	bool Save(HANDLE hFile);
#endif // end of #ifdef _N3TOOL

	void Release();

	CGrassBoard();
	virtual ~CGrassBoard();

protected:
	void FindGrassIndex(const uint8_t uCGrassMngOrder,int* pnInputGrass,int& nGrassTotNum);

};

#endif 
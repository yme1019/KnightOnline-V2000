#pragma warning(disable:4786)

#if !defined(AFX_DFONT_H__B1A14901_0027_40BC_8A6C_7FC78DE38686__INCLUDED_)
#define AFX_DFONT_H__B1A14901_0027_40BC_8A6C_7FC78DE38686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "N3Base.h"

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_3D		    0x0008		// 3D text

// set text flag
#define D3DFONT_CENTERED    0x0004		

#define D3DFONT_TWOSIDED    0x0010		
#define D3DFONT_FILTERED    0x0020		


class CDFont  : public CN3Base
{
public:
	CDFont(const std::string& szFontName, uint32_t dwHeight, uint32_t dwFlags=0L);
	virtual ~CDFont();

// Attributes
public:
	const std::string& GetFontName() { return m_szFontName; }
	uint32_t	GetFontHeight() const {return m_dwFontHeight;}
	int		GetFontHeightInLogicalUnit() const {return -MulDiv(m_dwFontHeight, GetDeviceCaps(s_hDC, LOGPIXELSY), 72);}
	uint32_t	GetFontFlags() const {return m_dwFontFlags;}
	SIZE	GetSize() const {return m_Size;}
	uint32_t	GetFontColor() const {return m_dwFontColor;}

protected:
	static HDC s_hDC;						// DC handle
	static int s_iInstanceCount;			// Class Instance Count
	static HFONT s_hFontOld;				// default font


	std::string	m_szFontName;				// Font properties
    uint32_t		m_dwFontHeight;			// Font Size
    uint32_t		m_dwFontFlags;

    LPDIRECT3DDEVICE9       m_pd3dDevice;	// A D3DDevice used for rendering
    LPDIRECT3DTEXTURE9      m_pTexture;		// The d3d texture for this font
    LPDIRECT3DVERTEXBUFFER9 m_pVB;			// VertexBuffer for rendering text
    uint32_t	m_dwTexWidth;				// Texture dimensions
    uint32_t	m_dwTexHeight;				// Texture dimensions
    FLOAT	m_fTextScale;					
											
											

//	HDC			m_hDC;							// DC handle
	HFONT		m_hFont;						// Font handle
	UINT		m_iPrimitiveCount;				
	D3DXVECTOR2	m_PrevLeftTop;				
	uint32_t		m_dwFontColor;				
	BOOL		m_Is2D;						
	SIZE		m_Size;						

// Operations
public:
	bool	IsSetText() { if(m_pTexture) return true; return false; }
	void	AddToAlphaManager(uint32_t dwColor, float fDist, __Matrix44& mtxWorld, uint32_t dwFlags);
	HRESULT	SetFontColor(uint32_t dwColor);						
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects();								
	HRESULT InvalidateDeviceObjects();							
	HRESULT DeleteDeviceObjects();							

	HRESULT SetText(const std::string& szText, uint32_t dwFlags = 0L);	
	HRESULT DrawText( FLOAT sx, FLOAT sy, uint32_t dwColor, uint32_t dwFlags, FLOAT fZ = 1.0f);
	HRESULT DrawText3D(uint32_t dwColor, uint32_t dwFlags );		

	HRESULT	SetFont(const std::string& szFontName, uint32_t dwHeight, uint32_t dwFlags=0L);	
	BOOL	GetTextExtent(const std::string& szString, int iStrLen, SIZE* pSize );
protected:
	void Make2DVertex(const int iFontHeight, const std::string& szText);				
	void Make3DVertex(const int iFontHeight, const std::string& szText, uint32_t dwFlags);

};

#endif // !defined(AFX_DFONT_H__B1A14901_0027_40BC_8A6C_7FC78DE38686__INCLUDED_)

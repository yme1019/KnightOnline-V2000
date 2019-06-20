#include "GameProcOption.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CGameProcOption::CGameProcOption()
{
}

CGameProcOption::~CGameProcOption()
{
}

void CGameProcOption::Init()
{
	CGameProcedure::Init();
}

void CGameProcOption::Release()
{
	CGameProcedure::Release();
}

void CGameProcOption::Render()
{
	CGameProcedure::Render();
}

void CGameProcOption::Tick()
{
	CGameProcedure::Tick();	
}

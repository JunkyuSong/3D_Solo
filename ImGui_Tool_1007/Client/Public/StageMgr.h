#pragma once
#include "C:\Users\송준규\Desktop\갠폴관련\00.portfolio\3D_Persnal_Project_1010\ImGui_Tool_1007\Reference\Headers\Base.h"
class CStageMgr :
	public CBase
{
	DECLARE_SINGLETON(CStageMgr)
private:
	CStageMgr();
	virtual ~CStageMgr() = default;

public:
	const _uint& Add_Mob() { return ++m_iMob; }

private:
	_uint m_iMob = 0;

public:
	// CBase을(를) 통해 상속됨
	virtual void Free() override;

};


#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_Mgr :
	public CBase
{
	DECLARE_SINGLETON(CEffect_Mgr)
public:
	enum EFFECT_TYPE {
		EFFECT_CLAW,
		EFFECT_BLOOD,
		EFFECT_END
	};

private:
	CEffect_Mgr() = default;
	virtual ~CEffect_Mgr() = default;

public:
	void Tick(_float fTimeDelta);
	
	HRESULT Add_Effect();

	void Clear_Level(LEVEL _eLv);

private:
	list<CEffect*>				m_pEffects[EFFECT_END];
	list<CEffect*>				m_pDeadEffects[EFFECT_END];

public:
	// CBase을(를) 통해 상속됨
	virtual void Free() override;
};

END
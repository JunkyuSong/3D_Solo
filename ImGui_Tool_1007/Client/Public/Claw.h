#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CNonAnimModel;
END

BEGIN(Client)
class CClaw :
	public CEffect
{
private:
	CClaw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CClaw(const CEffect& rhs);
	virtual ~CClaw();
	
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CNonAnimModel*	m_pModelCom = nullptr;

private:
	HRESULT Ready_Components();

public:
	static CClaw* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END
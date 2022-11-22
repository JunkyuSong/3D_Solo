#pragma once

#include "Client_Defines.h"
#include "Weapon.h"


BEGIN(Client)

class CLence final : public CWeapon
{
private:
	enum COLLIDERTYPE {
		COLLILDERTYPE_OBB, COLLILDERTYPE_END
	};
private:
	CLence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLence(const CLence& rhs);
	virtual ~CLence() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta, CGameObject* _pUser);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();





private:
	HRESULT Ready_Components();

public:
	static CLence* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
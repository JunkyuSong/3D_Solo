#pragma once

#include "Client_Defines.h"
#include "Weapon.h"


BEGIN(Client)

class CMonsterAxe final : public CWeapon
{
private:
	enum COLLIDERTYPE {
		COLLILDERTYPE_OBB, COLLILDERTYPE_END
	};
private:
	CMonsterAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterAxe(const CMonsterAxe& rhs);
	virtual ~CMonsterAxe() = default;

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
	static CMonsterAxe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
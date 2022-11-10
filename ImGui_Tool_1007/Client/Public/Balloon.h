#pragma once

#include "Client_Defines.h"
#include "Monster.h"


BEGIN(Engine)
class CNonAnimModel;
class CCollider;
END

BEGIN(Client)

class CBalloon final : public CMonster
{
public:
	enum COLLISION { COLLISION_PUSH, COLLISION_BODY, COLLISION_END };
private:
	CBalloon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBalloon(const CBalloon& rhs);
	virtual ~CBalloon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick( _float fTimeDelta);
	virtual void LateTick( _float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();

	void Update_Collider();

	void Die();

private:
	CCollider*		m_pColliderCom[COLLISION_END] = { nullptr };
	_bool			m_bCollision[COLLISION_END] = { false };

	_bool			m_bHit = false; //�����Ҵ��ؾ��ϰ�
	_float			m_fShakeTime = 0.5f;
	_float			m_fShakeCurTime = 0.f; //�����Ҵ��ؾ��ϰ�

	_float4			m_vOriginPos; //�����Ҵ��ؾ��ϰ�, �Ź� �ʾ�� ������ϰ�

	CNonAnimModel*	m_pNonAnimModelCom = nullptr;

	_bool			m_bDead = false;

	_uint			m_iPass = 8;

	_uint			m_iPointIndex = 0;

public:
	static CBalloon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
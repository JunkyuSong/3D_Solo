#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CHierarchyNode;
END

BEGIN(Client)

/*
�÷��̾ ���� ���� ���� �ȿ� ������ �� ���ϰ�
���� ���� �ȿ� ������ �� ���� �ȿ� ���ö����� ���Ѵ�
*/

class CExtra01_Last final : public CMonster
{
public:
	enum EXTRA01COLLIDER { COLLIDERTYPE_BODY, COLLIDERTYPE_PUSH, COLLILDERTYPE_END };
	enum STATE {
		LV1Villager_M_Attack01,
		LV1Villager_M_Attack02,
		LV1Villager_M_Attack03,
		LV1Villager_M_Die01,
		LV1Villager_M_HurtCounter,
		LV1Villager_M_HurtL_F,
		LV1Villager_M_Sit_Idle,
		LV1Villager_M_SP_Idle1,
		LV1Villager_M_VSTakeExecution,
		LV1Villager_M_WalkF,
		LV1Villager_M_IdleGeneral,
		LV1Villager_M_HurtS_FL,
		LV1Villager_M_HurtS_FR,
		LV1Villager_M_SP_Idle,
		LV1Villager_M_HurtStunStart,
		LV1Villager_M_HurtStunLoop,
		LV1Villager_M_HurtStunEnd,
		STATE_END
	};
private:
	enum DIRECT { DIR_F, DIR_B, DIR_R, DIR_L, DIR_FR, DIR_BR, DIR_FL, DIR_BL, DIR_END };

private:
	CExtra01_Last(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExtra01_Last(const CExtra01_Last& rhs);
	virtual ~CExtra01_Last() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	void PlayAnimation(_float fTimeDelta);

	_bool	Get_Battle() { return m_bPreStateAtt; }

public:
	void		Set_AnimState(STATE	_eState) { m_eCurState = _eState; }
	void		Set_AnimReserveState(STATE	_eState) { m_eReserveState = _eState; }
	STATE*		Get_AnimState() { return &m_eCurState; }
	STATE*		Get_AnimReserveState() { return &m_eReserveState; }

private:
	void CheckEndAnim();
	void Set_Anim(STATE _eState);
	void CheckAnim();
	void CheckState(_float fTimeDelta);

	void CheckLimit();

	void Get_AnimMat();

	void RenderGroup();
	_bool Collision(_float fTimeDelta);

	void On_Collider(EXTRA01COLLIDER _eCollider, _bool _bCollision);

	void Look_Move_Player(_float _fPosX, _float _fPosZ);
	void Look_Player();

	_bool InRange();
	void Pattern();

	_float	m_fIdleTime = 0.f;

	_bool	m_bAgainAnim = false;

	_bool	m_bDead = false;

private:
	STATE					m_eReserveState = STATE_END;
	STATE					m_eCurState = LV1Villager_M_SP_Idle1;
	STATE					m_ePreState = STATE_END;

	_float4					m_AnimPos;
	_float4					m_PreAnimPos;

	_bool					m_bAnimStop = false;
	_float					m_fPlayTime = 0.f;
	_float					m_fPlaySpeed = 1.f;
	vector<_float>			m_vecLimitTime[STATE_END];

	CCollider*				m_pColliderCom[COLLILDERTYPE_END] = { nullptr };
	_bool					m_bCollision[COLLILDERTYPE_END] = { false };

	DIRECT					m_eDir = DIR_END;

	_float					m_fMove = 0.f;
	_bool					m_bLine = false;

	_float3					m_vNextLook;
	_bool					m_bPreStateAtt = false;


private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();

	void	Ready_LimitTime();

	void	Update_Collider();

	void	Check_Stun();

private:
	HRESULT Ready_Weapon();

	HRESULT Update_Weapon();

	class CWeapon*				m_pParts = nullptr;
	class CHierarchyNode*		m_pSockets = nullptr;



public:
	static CExtra01_Last* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMonster* Clone(void* pArg);
	virtual void Free() override;
};

END
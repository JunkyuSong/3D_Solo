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

class CPuppet final : public CMonster
{
public:
	enum EXTRA01COLLIDER { COLLILDERTYPE_END = 1 };
	enum STATE {
		Puppet_AttackF_A,
		Puppet_Combo_F2R,
		Puppet_Combo_F_A_Start,
		Puppet_Combo_R_Attack01,
		Puppet_Combo_R_Attack02,
		Puppet_Idle_F,
		Puppet_IdleGeneral,
		Puppet_Step0_FightStart,
		Puppet_Step0_OffLineIdle,
		Puppet_Step1_1F_Attack01,
		Puppet_Step1_1F_Pose,
		Puppet_Step2_1FTO1R,
		Puppet_Step2_1R_Attack01,
		Puppet_Step3_2R_Attack01,
		Puppet_Step3_2R_Attack02,
		Puppet_Step3_2R_Attack03,
		Puppet_Step3_2R_Attack04, // ���������
		Puppet_Step3_2R_Pose1,
		Puppet_Step4_2L_Attack01,
		Puppet_Step4_2L_Attack02,
		Puppet_Step4_2L_Pose,
		Puppet_Step4_2RTO2L,
		Puppet_Step5_2B_Attack01,
		Puppet_Step5_2LTO2B,
		Puppet_VS_TakeExecution_Attack,
		Puppet_VS_TakeExecution_End,
		STATE_END
	};
private:
	enum DIRECT { DIR_F, DIR_B, DIR_R, DIR_L, DIR_FR, DIR_BR, DIR_FL, DIR_BL, DIR_END };

private:
	CPuppet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPuppet(const CPuppet& rhs);
	virtual ~CPuppet() = default;

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

	void Slow_Look_Player();
	void Look_Player();

	_bool InRange();
	void Pattern();



	_bool	m_bAgainAnim = false;

	_bool	m_bDead = false;

private:
	STATE					m_eReserveState = STATE_END;
	STATE					m_eCurState = Puppet_Step0_FightStart;
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
	_float4					m_ListNextLook[STATE_END];
	_float3					m_vAttPos;
	_float3					m_vLocalPos;
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
	static CPuppet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMonster* Clone(void* pArg);
	virtual void Free() override;

private:
	void Tick_Imgui();
};

END
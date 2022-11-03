#pragma once

#include "Client_Defines.h"
#include "Monster.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CAnimModel;
class CHierarchyNode;
END

BEGIN(Client)

class CMagician final : public CMonster
{
public:
	enum MAGICIANCOLLIDER { COLLIDERTYPE_BODY, COLLIDERTYPE_FOOT_R, COLLIDERTYPE_FOOT_L, COLLIDERTYPE_PUSH, COLLIDERTYPE_PARRY, COLLIDERTYPE_CARD, COLLILDERTYPE_END };
	enum STATE {
		Magician_Idle, Magician_Idle2,
		Hurt_Short, Hurt_Long,
		Boss_Enter,
		Cane_Att1, Cane_Att2,
		SP_Att1_Start, SP_Att1_Suc,
		SP_Att2_Start, SP_Att2_Loop, SP_Att2_Suc,
		Appear_L, Appear_R, Appear_B, Appear_F,
		Cane_Att3,
		Kick_Combo,
		Walk_B, Walk_Disappear_B, Walk_F, Walk_Disappear_F, Walk_L, Walk_Disappear_L, Walk_R, Walk_Disappear_R,

		Magician_Parry01,
		Magician_ParryAttack01,
		Magician_ParryAttack02,
		Magician_ParryJump,
		Magician_Shoot1,
		Magician_Shoot2,
		Magician_Shoot2_Slow,
		Magician_ShotJump,
		Magician_Disappear,
		Magician_Stage2_Attakc01,
		Magician_Stage2_Attakc04,
		Magician_Stage2_SwrodAttackCombo,
		Magician_Stage2_JumpAppear,
		Magician_StunStart_Sword,
		Magician_StunLoop_Sword,
		Magician_StunEnd_Sword,
		Magician_StunStart_Cane,
		Magician_StunLoop_Cane,
		Magician_StunEnd_Cane,
		Magician_SwordAttack1,
		Magician_SwordAttack2,
		Magician_SwordAttack2_V2,
		Magician_VSCorvus_TakeExecution,
		Magician_VSCorvus_TakeExecutionDisappear,
		Magician_Walk2F,
		Magician_Walk2L,
		Magician_Walk2R,
		Magician_HurtFL,
		Magician_HurtFR,
		Magician_HurtSL,
		Magician_HurtSR,
		Magician_DisppearIdle,
		Magician_JumpAppear,
		Magician_Sprinkle,
		Magician_ParrySword,
		STATE_END
	};
private:
	enum DIRECT { DIR_F, DIR_B, DIR_R, DIR_L, DIR_FR, DIR_BR, DIR_FL, DIR_BL, DIR_END };
	enum PART { PART_CANE, PART_CANESWORD, PART_END };
	enum CANESWORD { CANESWORD_R, CANESWORD_L, CANESWORD_END };

private:
	CMagician(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMagician(const CMagician& rhs);
	virtual ~CMagician() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	void PlayAnimation(_float fTimeDelta);

public:
	void		Set_AnimState(STATE	_eState) { m_eCurState = _eState; }
	void		Set_AnimReserveState(STATE	_eState) { m_eReserveState = _eState; }
	STATE*		Get_AnimState() { return &m_eCurState; }
	STATE*		Get_AnimReserveState() { return &m_eReserveState; }
	_float*		Get_Alpha() { return &m_fAppear; }

	void		Set_Card(CGameObject* pCard) { m_CutSceneCard = pCard; }

private:
	void CheckEndAnim();
	void Set_Anim(STATE _eState);
	void CheckAnim();
	void CheckState(_float fTimeDelta);

	void ChangeCanesword(CANESWORD _eCanesword);

	void CheckLimit();

	void Get_AnimMat();

	void RenderGroup();
	_bool Collision(_float fTimeDelta);

	void On_Collider(MAGICIANCOLLIDER _eCollider, _bool _bCollision);

	void Look_Move_Player(_float _fPosX, _float _fPosZ);
	void Look_Player();

	_float	m_fAppear = 1.f;

	_bool	m_bAgainAnim = false;

	_bool	m_bCutScene = false;
	_bool	m_bDead = false;

private:
	STATE					m_eReserveState = STATE_END;
	STATE					m_eCurState = Magician_Idle2;
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

	typedef vector<class CWeapon*>		PARTS;
	PARTS								m_pParts;
	vector<class CHierarchyNode*>		m_pSockets;
	vector<class CHierarchyNode*>		m_pCanesword;

	CGameObject*						m_CutSceneCard = nullptr;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();

	void	Ready_LimitTime();

	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();
	HRESULT Update_Weapon();

	void	Update_Collider();

	HRESULT Shoot();
	void	Check_Stun();

private:
	_float	m_fStay = 0.f;
	void Pattern(_float fTimeDelta);
	void Pattern_Appear();
	void Pattern_Short(_float fDis);
	void Pattern_Long(_float fDis);

public:
	static CMagician* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMonster* Clone(void* pArg);
	virtual void Free() override;
};

END
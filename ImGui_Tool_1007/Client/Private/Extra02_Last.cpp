#include "stdafx.h"
#include "..\Public\Extra02_Last.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"
#include "HierarchyNode.h"
#include "Weapon.h"
#include "CollisionMgr.h"
#include "Player.h"
#include "Status.h"
#include "Animation.h"
#include "StageMgr.h"
#include "CameraMgr.h"
#include "Camera.h"
#include "Camera_CutScene_Enter.h"

CExtra02_Last::CExtra02_Last(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CExtra02_Last::CExtra02_Last(const CExtra02_Last & rhs)
	: CMonster(rhs)
	, m_AnimPos(rhs.m_AnimPos)
	, m_PreAnimPos(rhs.m_PreAnimPos)
{
}

HRESULT CExtra02_Last::Initialize_Prototype()
{
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;

	m_bAnimStop = true;

	return S_OK;
}

HRESULT CExtra02_Last::Initialize(void * pArg)
{
	AUTOINSTANCE(CGameInstance, _pInstance);

	m_eMonsterType = MONSTER_EXTRA02;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	Ready_LimitTime();


	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (pArg)
	{
		MONSTERINFO _tInfo = *static_cast<MONSTERINFO*>(pArg);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _tInfo._vPos);
		_bool		isMove = true;
		Get_AnimMat();
		_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		if (nullptr != m_pNavigationCom)
			isMove = m_pNavigationCom->isMove(_tInfo._vPos, &vNormal);

		if (true == isMove)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _tInfo._vPos);

		m_pNavigationCom->Set_Index(_tInfo._iIndex);
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(51.979f, 0.115f, -7.650f, 1.f));
		_bool		isMove = true;
		Get_AnimMat();
		_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		if (nullptr != m_pNavigationCom)
			isMove = m_pNavigationCom->isMove(XMVectorSet(30.672f, 2.402f, 50.622f, 1.f), &vNormal);

		if (true == isMove)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(30.672f, 2.402f, 50.622f, 1.f));
	}

	_float fRot = _pInstance->Rand_Float(0.f, 359.f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRot));

	m_eCurState = LV1Villager_M_SP_Idle;
	On_Collider(COLLIDERTYPE_BODY, true);
	//m_fIdleTime = _pInstance->Rand_Float(20.f, 200.f);
	m_pModelCom->DirectAnim(LV1Villager_M_SP_Idle);
	return S_OK;
}

void CExtra02_Last::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		m_eMonsterState = ATTACK_DEAD;
		return;
	}
		
	AUTOINSTANCE(CGameInstance, _Instance);
	
	if (m_Coll_Target == true)
	{
		m_Coll_Target = false;
		if (m_eCurState == LV1Villager_M_Attack01 || m_eCurState == LV1Villager_M_Attack02)
		{
			m_eReserveState = LV1Villager_M_Attack03;
		}
		else if (m_eCurState == LV2Villager01_M_ComboA01)
		{
			m_eReserveState = LV2Villager01_M_ComboA02;
		}
	}

	if (m_bDead)
		return;

	if (m_pModelCom != nullptr)
	{
		Check_Stun();
		CheckAnim();


		if (m_eCurState != LV1Villager_M_SP_Idle)
			PlayAnimation(fTimeDelta);
		CheckState(fTimeDelta);
	}

	Update_Weapon();

	if (m_pParts != nullptr)
		m_pParts->Tick(fTimeDelta, this);

	Update_Collider();
}

void CExtra02_Last::LateTick(_float fTimeDelta)
{
	if (m_bDead)
	{
		
		RenderGroup();
		return;
	}
		
	if (Collision(fTimeDelta))
	{
		CheckAnim();
		CheckState(fTimeDelta);
		PlayAnimation(fTimeDelta);
	}

	AUTOINSTANCE(CGameInstance, _pInstance);
	if (Collision(fTimeDelta))
	{
		CheckAnim();
		CheckState(fTimeDelta);
		PlayAnimation(fTimeDelta);
	}
	_bool		isDraw = _pInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
	if (isDraw)
		RenderGroup();
}

HRESULT CExtra02_Last::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;



	SetUp_ShaderResources();

	_uint		iNumMeshes = m_pModelCom->Get_NumMesh();//�޽� ������ �˰� �޽� ������ŭ ������ �� ����. ���⼭!

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, 0, i)))
			return E_FAIL;
	}

	for (_uint i = 0; i < COLLILDERTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}
	m_pNavigationCom->Render();

	return S_OK;
}

void CExtra02_Last::PlayAnimation(_float fTimeDelta)
{
	if (m_bAnimStop)
		return;
	_float4 _vAnim;
	XMStoreFloat4(&_vAnim, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	if (m_pModelCom->Play_Animation(fTimeDelta * m_fPlaySpeed, &_vAnim, &m_fPlayTime, m_bAgainAnim))
	{
		CheckEndAnim();
	}
	CheckLimit();
	XMStoreFloat4(&m_AnimPos, (XMLoadFloat4(&_vAnim) - XMLoadFloat4(&m_PreAnimPos)));
	m_PreAnimPos = _vAnim;
}


void CExtra02_Last::CheckEndAnim()
{
	m_fPlaySpeed = 1.f;
	AUTOINSTANCE(CGameInstance, _pInstance);
	
	switch (m_eCurState)
	{
	case Client::CExtra02_Last::LV1Villager_M_Attack01:
		m_eCurState = LV1Villager_M_IdleGeneral;
		break;
	case Client::CExtra02_Last::LV1Villager_M_Attack02:
		m_eCurState = LV1Villager_M_IdleGeneral;
		break;
	case Client::CExtra02_Last::LV1Villager_M_Attack03:
		m_eCurState = LV1Villager_M_IdleGeneral;
		break;
	case Client::CExtra02_Last::LV1Villager_M_Die01:		
		m_bDead = true;
		m_eMonsterState = ATTACK_DEAD;
		break;
	case Client::CExtra02_Last::LV1Villager_M_HurtCounter:
		m_eCurState = LV1Villager_M_IdleGeneral;
		break;
	case Client::CExtra02_Last::LV1Villager_M_HurtL_F:
		m_eCurState = LV1Villager_M_IdleGeneral;
		break;
	case Client::CExtra02_Last::LV2Villager01_M_VS_TakeExecution_01:
		m_bDead = true;
		break;
	case Client::CExtra02_Last::LV1Villager_M_WalkF:
		m_eCurState = LV1Villager_M_WalkF;
		break;
	case Client::CExtra02_Last::LV2Villager01_M_ComboA01:
		m_eCurState = LV1Villager_M_IdleGeneral;
		break;
	case Client::CExtra02_Last::LV2Villager01_M_ComboA02:
		m_eCurState = LV1Villager_M_IdleGeneral;
		break;
	case Client::CExtra02_Last::LV1Villager_M_IdleGeneral:
		
		//m_eCurState = LV1Villager_M_IdleGeneral;

		break;

	}

	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
}

void CExtra02_Last::CheckState(_float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, _pInstance);
	switch (m_eCurState)
	{
	case Client::CExtra02_Last::LV1Villager_M_Attack01:
		break;
	case Client::CExtra02_Last::LV1Villager_M_Attack02:
		break;
	case Client::CExtra02_Last::LV1Villager_M_Attack03:
		break;
	case Client::CExtra02_Last::LV1Villager_M_Die01:
		break;
	case Client::CExtra02_Last::LV1Villager_M_HurtCounter:
		break;
	case Client::CExtra02_Last::LV1Villager_M_HurtL_F:
		break;
	/*case Client::CExtra02::LV1Villager_M_Sit_Idle:
		break;
	case Client::CExtra02::LV1Villager_M_SP_Idle1:
		break;*/
	case Client::CExtra02_Last::LV1Villager_M_VSTakeExecution:
		break;
	case Client::CExtra02_Last::LV1Villager_M_WalkF:
		{
			_vector _PlayerPos = static_cast<CTransform*>(_pInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->LookAt_ForLandObject(_PlayerPos);

			_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

			Pattern();
		}
		break;
	case Client::CExtra02_Last::LV1Villager_M_IdleGeneral:
		m_eCurState = LV1Villager_M_WalkF;
		break;
	case LV1Villager_M_SP_Idle:
		{
			_vector _PlayerPos = static_cast<CTransform*>(_pInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			if (fabs(XMVector3Length(_PlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)).m128_f32[0]) > 5.f)
			{
				m_eCurState = LV1Villager_M_WalkF;
			}
		}
		break;
	}
	Get_AnimMat();
}

void CExtra02_Last::CheckLimit()
{
	switch (m_eCurState)
	{
	case Client::CExtra02_Last::LV1Villager_M_Attack01:
		if (m_vecLimitTime[LV1Villager_M_Attack01][2] < m_fPlayTime)
		{
			m_pParts->Set_CollisionOn(false);
		}
		else if (m_vecLimitTime[LV1Villager_M_Attack01][1] < m_fPlayTime)
		{
			if (m_eReserveState == LV1Villager_M_Attack02)
			{
				m_eCurState = LV1Villager_M_Attack02;
			}
			else if (m_eReserveState == LV1Villager_M_Attack03)
			{
				m_eCurState = LV1Villager_M_Attack03;
				m_eReserveState = STATE_END;
			}
		}
		else if (m_vecLimitTime[LV1Villager_M_Attack01][0] < m_fPlayTime)
		{
			m_pParts->Set_CollisionOn(true);
		}

		break;
	case Client::CExtra02_Last::LV1Villager_M_Attack02:
		break;
	case Client::CExtra02_Last::LV1Villager_M_Attack03:
		if (m_vecLimitTime[LV1Villager_M_Attack03][0] < m_fPlayTime)
		{
			m_pParts->Set_CollisionOn(false);
		}
		break;
	case Client::CExtra02_Last::LV1Villager_M_Die01:
		break;
	case Client::CExtra02_Last::LV1Villager_M_HurtCounter:
		break;
	case Client::CExtra02_Last::LV1Villager_M_HurtL_F:
		if (m_fPlayTime > 10.f)
		{
			On_Collider(COLLIDERTYPE_BODY, true);
		}
		break;
	case Client::CExtra02_Last::LV1Villager_M_VSTakeExecution:
		break;
	case Client::CExtra02_Last::LV1Villager_M_WalkF:
		break;
	case Client::CExtra02_Last::LV2Villager01_M_ComboA01:
		if (m_vecLimitTime[LV2Villager01_M_ComboA01][2] < m_fPlayTime)
		{
			On_Collider(COLLIDERTYPE_ARM, false);
		}
		else if (m_vecLimitTime[LV2Villager01_M_ComboA01][1] < m_fPlayTime)
		{
			if (m_eReserveState == LV1Villager_M_Attack02)
			{
				m_eCurState = LV1Villager_M_Attack02;
			}
			else if (m_eReserveState == LV2Villager01_M_ComboA02)
			{
				m_eCurState = LV2Villager01_M_ComboA02;
				m_eReserveState = STATE_END;
			}
		}
		else if (m_vecLimitTime[LV2Villager01_M_ComboA01][0] < m_fPlayTime)
		{
			On_Collider(COLLIDERTYPE_ARM, true);
		}
		break;
	case Client::CExtra02_Last::LV2Villager01_M_ComboA02:
		if (m_vecLimitTime[LV2Villager01_M_ComboA02][0] < m_fPlayTime)
		{
			On_Collider(COLLIDERTYPE_ARM, false);
		}
		break;
	case Client::CExtra02_Last::LV1Villager_M_IdleGeneral:
		break;
	}
}

void CExtra02_Last::Set_Anim(STATE _eState)
{
	m_eCurState = _eState;
	XMStoreFloat4(&m_AnimPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_PreAnimPos = m_AnimPos;
	if (m_pStatusCom->Get_Hp() <= 0.f)
		m_eMonsterState = CMonster::ATTACK_DEAD;
	else
		m_eMonsterState = CMonster::ATTACK_IDLE;
	m_pModelCom->Set_AnimationIndex(m_eCurState);
	m_fPlayTime = 0.f;
}

void CExtra02_Last::CheckAnim()
{
	if (m_ePreState != m_eCurState)
	{
		Set_Anim(m_eCurState);
		m_ePreState = m_eCurState;
	}
}

void CExtra02_Last::Get_AnimMat()
{
	if (m_bAnimStop)
		return;
	_fmatrix _World = m_pTransformCom->Get_WorldMatrix();
	_vector _vPos;
	_vPos = XMVector3TransformCoord(XMLoadFloat4(&m_AnimPos), _World);

	_bool		isMove = true;

	if (nullptr != m_pNavigationCom)
		isMove = m_pNavigationCom->isMove(_vPos, nullptr);

	if (true == isMove)
	{
		_vPos.m128_f32[1] = m_pNavigationCom->Get_PosY(_vPos);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
	}
}

void CExtra02_Last::RenderGroup()
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_pParts);
}

_bool CExtra02_Last::Collision(_float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, _instance);

	CGameObject* _pTarget = nullptr;

	if (_pTarget = m_pColliderCom[COLLIDERTYPE_PUSH]->Get_Target())
	{
		if (_pTarget->Get_ObjType() == TYPE_PLAYER)
		{
			_vector _vDir = XMLoadFloat3(&(static_cast<CCapsule*>(m_pColliderCom[COLLIDERTYPE_PUSH])->Get_Dir()));
			_float	_vDis = (static_cast<CCapsule*>(m_pColliderCom[COLLIDERTYPE_PUSH])->Get_Dis());
			_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(_vDir) * _vDis;
			_bool		isMove = true;

			_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			if (nullptr != m_pNavigationCom)
				isMove = m_pNavigationCom->isMove(_vPos, &vNormal);

			if (true == isMove)
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
		}

	}
	



	if ((_pTarget = m_pColliderCom[COLLIDERTYPE_BODY]->Get_Target()) && (CPlayer::ParryL != *static_cast<CPlayer*>(_pTarget)->Get_AnimState()))
	{
		CPlayer* _pPlayer = static_cast<CPlayer*>(_pTarget);

		CTransform* _Trans = static_cast<CTransform*>(_instance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));
		m_pTransformCom->LookAt_ForLandObject(_Trans->Get_State(CTransform::STATE_POSITION));

		if (m_eCurState == LV1Villager_M_HurtL_F)
		{
			if (m_bAgainAnim == false)
			{
				m_bAgainAnim = true;
				Set_Anim(m_eCurState);
			}
		}
		else
		{
			m_eCurState = LV1Villager_M_HurtL_F;
		}
		On_Collider(COLLIDERTYPE_BODY, false);
		m_bPreStateAtt = true;
		if (false == m_pStatusCom->Damage(static_cast<CStatus*>(_pTarget->Get_ComponentPtr(TEXT("Com_Status")))->Get_Attack()))
		{
			m_eCurState = LV1Villager_M_Die01;
		}
		return true;
	}

	return false;
}

void CExtra02_Last::On_Collider(EXTRA02COLLIDER _eCollider, _bool _bCollision)
{
	switch (_eCollider)
	{
	case Client::CExtra02_Last::COLLIDERTYPE_BODY:
		if (m_bCollision[COLLIDERTYPE_BODY] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
		break;
	case Client::CExtra02_Last::COLLIDERTYPE_ARM:
		if (m_bCollision[COLLIDERTYPE_ARM] = _bCollision)
			CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_WEAPON, m_pColliderCom[COLLIDERTYPE_ARM], this);
		break;
	}
}

void CExtra02_Last::Look_Move_Player(_float _fPosX, _float _fPosZ)
{
	// �ִϸ��̼� ���� �߿� ���ϴ� �Լ�

	AUTOINSTANCE(CGameInstance, _Intance);
	CTransform* _Trans =
		static_cast<CTransform*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));

	CNavigation* _pNavi =
		static_cast<CNavigation*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Navigation")));
	m_pNavigationCom->Set_Index(_pNavi->Get_Index());
	_vector _vTargetPos = _Trans->Get_State(CTransform::STATE_POSITION);
	_vTargetPos.m128_f32[0] += _fPosX;
	_vTargetPos.m128_f32[2] += _fPosZ;

	_bool		isMove = true;
	_vector		vNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (nullptr != m_pNavigationCom)
		isMove = m_pNavigationCom->isMove(_vTargetPos, &vNormal);

	if (true == isMove)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vTargetPos);

	m_pTransformCom->LookAt_ForLandObject(_Trans->Get_State(CTransform::STATE_POSITION));
}

void CExtra02_Last::Look_Player()
{
	//�ѹ��� �ٶ󺸴°�
	AUTOINSTANCE(CGameInstance, _Intance);
	CTransform* _Trans =
		static_cast<CTransform*>(_Intance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));

	m_pTransformCom->LookAt_ForLandObject(_Trans->Get_State(CTransform::STATE_POSITION));
}

_bool CExtra02_Last::InRange()
{

	AUTOINSTANCE(CGameInstance, _pInstance);
	CPlayer* _pPlayer = static_cast<CPlayer*>(_pInstance->Get_Player());
	CTransform* _pTransform = static_cast<CTransform*>(_pPlayer->Get_ComponentPtr(TEXT("Com_Transform")));
	_vector _vToPlayer = _pTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float _fLimitAngle = sqrtf(3.f) / 2.f;
	_float _fLimitDis = 7.f;

	if (m_bPreStateAtt)
	{
		_fLimitDis = 20.f;
		if (XMVector3Length(_vToPlayer).m128_f32[0] > _fLimitDis)
		{
			return false;
		}
		_vector _vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_vector _vDestLook = XMVector3Normalize(_vToPlayer);

		_float _fAngle = XMVector3Dot(_vLook, _vDestLook).m128_f32[0];

		Pattern();

		return true;
	}
	else
	{
		if (XMVector3Length(_vToPlayer).m128_f32[0] > _fLimitDis)
		{
			return false;
		}

		_vector _vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_vector _vDestLook = XMVector3Normalize(_vToPlayer);

		_float _fAngle = XMVector3Dot(_vLook, _vDestLook).m128_f32[0];


		if (_fAngle > _fLimitAngle)//�ȿ� ������ �׸��� ... �پ�ߵǴµ�?
		{
			Pattern();
			return true;
		}

	}
	return false;
}

void CExtra02_Last::Pattern()
{
	//�Ÿ��� ���� Ȯ�������� �����ϰ� �������
	// �Ÿ� ª���� �� �׷��� ��Ű��Ű
	AUTOINSTANCE(CGameInstance, _pInstance);

	CTransform* _pPlayerTransform = static_cast<CTransform*>(_pInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));
	_vector		_pLook = _pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float _fDistance = XMVector3Length(_pLook).m128_f32[0];



	// ���� ���� ¥�� ����
	//�÷��̾���� �Ÿ�

	if (_fDistance < 3.f)
	{
		_uint _iRand = _pInstance->Rand_Int(0,1);

		switch (_iRand)
		{
		case 0:
			m_eCurState = LV1Villager_M_Attack01;
			break;
		case 1:
			m_eCurState = LV2Villager01_M_ComboA01;
			break;
		}

		
	}
	else
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), _pLook, 0.8f);
		m_eCurState = LV1Villager_M_WalkF;
	}
}

HRESULT CExtra02_Last::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC	_Desc;
	_Desc.fRotationPerSec = XMConvertToRadians(90.f);
	//_Desc.fSpeedPerSec = 1.5f;
	_Desc.fSpeedPerSec = 2.5f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &_Desc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Model_Monster_Extra2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Status */
	CStatus::STATUS _tStatus;
	_tStatus.fMaxHp = 200.f;
	_tStatus.fAttack = 7.f;
	_tStatus.fHp = _tStatus.fMaxHp;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &_tStatus)))
		return E_FAIL;


	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 1.8f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_BODY], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 0.3f, 0.3f);
	_float4x4 _mat = m_pModelCom->Get_HierarchyNode("lowerarm_l")->Get_Trans();

	ColliderDesc.vCenter = _float3(_mat._41,
		_mat._42,
		_mat._43);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Arm"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_ARM], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 1.8f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Capsule"), TEXT("Com_Capsule"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_PUSH], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 112;

	if (FAILED(__super::Add_Component(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Navigation_Stage_Last"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CExtra02_Last::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CExtra02_Last::Ready_LimitTime()
{
	m_vecLimitTime[LV1Villager_M_Attack01].push_back(100.f);//���� �ݶ��̴� on
	m_vecLimitTime[LV1Villager_M_Attack01].push_back(110.f);
	m_vecLimitTime[LV1Villager_M_Attack01].push_back(165.f);//���� �ݶ��̴� off

	m_vecLimitTime[LV1Villager_M_Attack03].push_back(60.f);//���� �ݶ��̴� off


	// lowerarm
	m_vecLimitTime[LV2Villager01_M_ComboA01].push_back(70.f);//���� �ݶ��̴� on
	m_vecLimitTime[LV2Villager01_M_ComboA01].push_back(90.f);
	m_vecLimitTime[LV2Villager01_M_ComboA01].push_back(100.f);//���� �ݶ��̴� off

	m_vecLimitTime[LV2Villager01_M_ComboA02].push_back(60.f);//���� �ݶ��̴� off
}

CExtra02_Last * CExtra02_Last::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CExtra02_Last*		pInstance = new CExtra02_Last(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CExtra02_Last"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMonster * CExtra02_Last::Clone(void * pArg)
{
	CExtra02_Last*		pInstance = new CExtra02_Last(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CExtra02_Last"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExtra02_Last::Free()
{
	__super::Free();

	for (auto& _Collider : m_pColliderCom)
	{
		if (_Collider)
			Safe_Release(_Collider);
	}
	Safe_Release(m_pParts);
	Safe_Release(m_pSockets);
}

void CExtra02_Last::Update_Collider()
{
	//if (m_bCollision[COLLIDERTYPE_BODY])
	m_pColliderCom[COLLIDERTYPE_PUSH]->Update(m_pTransformCom->Get_WorldMatrix());
	CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_PUSH, m_pColliderCom[COLLIDERTYPE_PUSH], this);

	if (m_bCollision[COLLIDERTYPE_BODY])
	{
		m_pColliderCom[COLLIDERTYPE_BODY]->Update(m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLIDERTYPE_BODY], this);
	}
	if (m_bCollision[COLLIDERTYPE_ARM])
	{
		m_pColliderCom[COLLIDERTYPE_ARM]->Update(m_pModelCom->Get_HierarchyNode("lowerarm_l")->Get_CombinedTransformation()*XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())*m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_WEAPON, m_pColliderCom[COLLIDERTYPE_ARM], this);
	}
}

void CExtra02_Last::Check_Stun()
{
	if (m_eMonsterState == CMonster::ATTACK_STUN)
	{
		m_eCurState = LV1Villager_M_HurtCounter;
		m_eMonsterState = CMonster::ATTACK_IDLE;
	}
}

HRESULT CExtra02_Last::Ready_Weapon()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("weapon_r_IK");
	if (nullptr == pWeaponSocket)
		return E_FAIL;
	m_pSockets = pWeaponSocket;
	Safe_AddRef(m_pSockets);



	/* For.Sword */
	CWeapon*		pGameObject = static_cast<CWeapon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_MonsterAxe"), this));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pParts = pGameObject;

	return S_OK;
}

HRESULT CExtra02_Last::Update_Weapon()
{
	if (nullptr == m_pSockets)
		return E_FAIL;

	/* ���. */
	/*_matrix			WeaponMatrix = ���� �����̽� ��ȯ(OffsetMatrix)
	* ���� ���(CombinedTransformation)
	* ���� PivotMatrix * �����̾��ǿ������. ;*/
	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Player */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	_matrix WeaponMatrix = /*m_pSockets[PART_CANE]->Get_OffSetMatrix()**/
		m_pSockets->Get_CombinedTransformation()
		* XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix())
		* m_pTransformCom->Get_WorldMatrix();

	m_pParts->SetUp_State(WeaponMatrix);

	return S_OK;
}

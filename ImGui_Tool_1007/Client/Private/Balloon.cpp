#include "stdafx.h"
#include "Balloon.h"
#include "Obj_NonAnim.h"
#include "Renderer.h"
#include "Shader.h"
#include "NonAnimModel.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Status.h"
#include "CollisionMgr.h"
#include "ImGuiMgr.h"

CBalloon::CBalloon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CBalloon::CBalloon(const CBalloon & rhs)
	: CMonster(rhs)
{
}

HRESULT CBalloon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBalloon::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	AUTOINSTANCE(CGameInstance, _pInstance);

	m_pTransformCom->Set_Scale(XMVectorSet(0.015f, 0.015f, 0.015f, 0.f));

	if (pArg)
	{
		MONSTERINFO _tInfo = *static_cast<MONSTERINFO*>(pArg);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _tInfo._vPos);
		
		memcpy(&m_vOriginPos, pArg, sizeof(_float4));
	}

	m_bCollision[COLLISION_PUSH] = true;
	m_bCollision[COLLISION_BODY] = true;

	return S_OK;
}

void CBalloon::Tick( _float fTimeDelta)
{
	if (!m_bDead && m_bHit)
	{
		m_fShakeCurTime += fTimeDelta;

		//흔들려
		if (m_fShakeCurTime > m_fShakeTime)
		{
			m_bHit = false;
			m_fShakeCurTime = 0.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vOriginPos));
			m_bCollision[COLLISION_BODY] = true;
		}
		else
		{
			if ((_uint(m_fShakeCurTime*20.f) % 2) == 0)
			{
				_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_vPos -= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT) + m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fTimeDelta;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
			}
			else
			{
				_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_vPos += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT) + m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fTimeDelta * 1.5f;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
			}		
		}
	}

	Update_Collider();
}

void CBalloon::LateTick( _float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	AUTOINSTANCE(CGameInstance, _pInstance);

	CGameObject* _pTarget = nullptr;

	if (_pTarget = m_pColliderCom[COLLISION_PUSH]->Get_Target())
	{
		_vector _vDir = XMLoadFloat3(&(static_cast<CCapsule*>(m_pColliderCom[COLLISION_PUSH])->Get_Dir()));
		
		_float	_vDis = (static_cast<CCapsule*>(m_pColliderCom[COLLISION_PUSH])->Get_Dis());
		CTransform* _pTargetTrans = static_cast<CTransform*>(_pTarget->Get_ComponentPtr(TEXT("Com_Transform")));
		//_vector _vDir = XMVector3Normalize(_pTargetTrans->Get_State(CTransform::STATE_LOOK));
		_vector _vPos = _pTargetTrans->Get_State(CTransform::STATE_POSITION) - XMVector3Normalize(_vDir) */* fabs*/(_vDis);
		_bool		isMove = true;

		

		isMove = static_cast<CNavigation*>(_pTarget->Get_ComponentPtr(TEXT("Com_Navigation")))->isMove(_vPos, nullptr);

		if (true == isMove)
			_pTargetTrans->Set_State(CTransform::STATE_POSITION, _vPos);
		else
		{
			_vDir = XMVector3Normalize(_pTargetTrans->Get_State(CTransform::STATE_LOOK));
			_vPos = _pTargetTrans->Get_State(CTransform::STATE_POSITION) - XMVector3Normalize(_vDir) * fabs(_vDis);
			_pTargetTrans->Set_State(CTransform::STATE_POSITION, _vPos);
		}
	}
	if (_pTarget = m_pColliderCom[COLLISION_BODY]->Get_Target())
	{
		//흔들리고
		if (false == m_pStatusCom->Damage(static_cast<CStatus*>(_pTarget->Get_ComponentPtr(TEXT("Com_Status")))->Get_Attack()))
		{
			Safe_Release(m_pNonAnimModelCom);
			Safe_Release(m_Components[TEXT("Com_Model")]);
			m_pNonAnimModelCom = static_cast<CNonAnimModel*>(_pInstance->Clone_Component(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_balloonDamaged01")));
			m_Components[TEXT("Com_Model")] = m_pNonAnimModelCom;
			Safe_AddRef(m_Components[TEXT("Com_Model")]);
			m_bDead = true;
			m_bCollision[COLLISION_PUSH] = false;
			m_bCollision[COLLISION_BODY] = false;
			_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vPos .m128_f32[1] += 3.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
			m_iPass = 0;
		}
		m_bHit = true;
		m_bCollision[COLLISION_BODY] = false;
	}
	
	_bool		isDraw = _pInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f);
	if (isDraw)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	
}

HRESULT CBalloon::Render()
{
	if (nullptr == m_pNonAnimModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;



	SetUp_ShaderResources();

	_uint		iNumMeshes = m_pNonAnimModelCom->Get_NumMesh();//메쉬 갯수를 알고 메쉬 갯수만큼 렌더를 할 것임. 여기서!

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pNonAnimModelCom->SetUp_OnShader(m_pShaderCom, m_pNonAnimModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		if (FAILED(m_pNonAnimModelCom->SetUp_OnShader(m_pShaderCom, m_pNonAnimModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pNonAnimModelCom->SetUp_OnShader(m_pShaderCom, m_pNonAnimModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Begin(m_iPass)))
			return E_FAIL;

		if (FAILED(m_pNonAnimModelCom->Render(i)))
			return E_FAIL;
	}

	//if (nullptr != m_pColliderCom[COLLISION_BODY])
		m_pColliderCom[COLLISION_BODY]->Render();


	return S_OK;
}


HRESULT CBalloon::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC	_Desc;
	_Desc.fRotationPerSec = XMConvertToRadians(90.f);
	//_Desc.fSpeedPerSec = 1.5f;
	_Desc.fSpeedPerSec =0.5f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &_Desc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	AUTOINSTANCE(CGameInstance, _pInstance);
	if (_pInstance->Rand_Int(0, 1) == 1)
	{
		if (FAILED(__super::Add_Component(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Balloon01"), TEXT("Com_Model"), (CComponent**)&m_pNonAnimModelCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Balloon02"), TEXT("Com_Model"), (CComponent**)&m_pNonAnimModelCom)))
			return E_FAIL;
	}

	/* For.Com_Status */
	CStatus::STATUS _tStatus;
	_tStatus.fMaxHp = 100.f;
	_tStatus.fAttack = 0.f;
	_tStatus.fHp = _tStatus.fMaxHp;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &_tStatus)))
		return E_FAIL;


	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(600.f, 300.f, 600.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f + 200.f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLISION_BODY], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(6.5f, 6.5f, 6.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f + 1.8f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Capsule"), TEXT("Com_Capsule"), (CComponent**)&m_pColliderCom[COLLISION_PUSH], &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBalloon::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fY", &(m_pTransformCom->Get_WorldMatrix().r[3].m128_f32[1]), sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBalloon::Update_Collider()
{
	if (m_bCollision[COLLISION_PUSH])
	{
		m_pColliderCom[COLLISION_PUSH]->Update(m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_PUSH, m_pColliderCom[COLLISION_PUSH], this);
	}
	if (m_bCollision[COLLISION_BODY])
	{
 		m_pColliderCom[COLLISION_BODY]->Update(m_pTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_MONSTER_BODY, m_pColliderCom[COLLISION_BODY], this);
	}
}

CBalloon * CBalloon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBalloon*		pInstance = new CBalloon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBalloon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBalloon::Clone(void * pArg)
{
	CBalloon*		pInstance = new CBalloon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBalloon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBalloon::Free()
{
	__super::Free();

	Safe_Release(m_pNonAnimModelCom);

	Safe_Release(m_pColliderCom[COLLISION_BODY]);
	Safe_Release(m_pColliderCom[COLLISION_PUSH]);
}
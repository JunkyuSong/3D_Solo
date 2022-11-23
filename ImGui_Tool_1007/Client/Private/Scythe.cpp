#include "stdafx.h"
#include "..\Public\Scythe.h"
#include "GameInstance.h"


#include "Trail_Obj.h"

#include "Effect_Mgr.h"
#include "Fire.h"

CScythe::CScythe(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CWeapon(pDevice, pContext)
{
}

CScythe::CScythe(const CScythe & rhs)
	: CWeapon(rhs)
{
}

HRESULT CScythe::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CScythe::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.0f));





	return S_OK;
}

void CScythe::Tick(_float fTimeDelta)
{
	m_pTrailCom->Tick(fTimeDelta, m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());

	if (m_bColliderOn)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
	
}

void CScythe::Tick(_float fTimeDelta, CGameObject * _pUser)
{
	m_pTrailCom->Tick(fTimeDelta, m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());

	if (m_bColliderOn)
	{
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
		CCollisionMgr::Get_Instance()->Add_CollisoinList(CCollisionMgr::TYPE_PLAYER_WEAPON, m_pColliderCom, _pUser);
	}

	//high지점에 불 하나 놓고
	// high - low 방향으로 나간다
	//if (m_pTrailCom->Get_On())
	//{
	//	
	//	TRAILPOS HL = m_pTrailCom->Get_HighAndLow();
	//	CFire::FIRE_DESC _tInfo;
	//	XMStoreFloat4(&_tInfo.vPos, XMVectorSetW(XMLoadFloat3(&HL.vHigh), 1.f));
	//	_tInfo.vColor = CLIENT_RGB(100.f, 255.f, 100.f);
	//	_tInfo.vSize = { 1.f, 1.f };
	//	_tInfo.iChance = 1;
	//	XMStoreFloat3(&_tInfo.vDirect, XMVector3Normalize(XMLoadFloat3(&HL.vHigh) - XMLoadFloat3(&HL.vLow)));
	//	AUTOINSTANCE(CGameInstance, _pInstance);
	//	_float _fTerm = _pInstance->Rand_Float(0.01f, 0.5f);
	//	if (fabs(XMVector3Length(XMLoadFloat4(&m_vPreFirePos) - XMLoadFloat4(&_tInfo.vPos)).m128_f32[0]) > _fTerm)
	//		CEffect_Mgr::Get_Instance()->Add_Effect(CEffect_Mgr::EFFECT_FIRE, &_tInfo);
	//	_float4 _vPos = _tInfo.vPos;
	//	_float3 _vDir = _tInfo.vDirect;
	//	//if (fabs(XMVector3Length(XMLoadFloat4(&m_vPreFirePos) - XMLoadFloat4(&_tInfo.vPos)).m128_f32[0]) < 2.5f)
	//	/*{
	//		for (_float _fRatio = 0.f; _fRatio < 1.f ; _fRatio += 0.25f)
	//		{
	//			XMStoreFloat4(&_tInfo.vPos, XMVectorSetW(XMVectorLerp(XMLoadFloat4(&m_vPreFirePos), XMLoadFloat4(&_vPos), _fRatio), 1.f));
	//			XMStoreFloat3(&_tInfo.vDirect,XMVector3Normalize(XMVectorLerp(XMLoadFloat3(&m_vPreFireDir), XMLoadFloat3(&_vDir), _fRatio)));
	//			CEffect_Mgr::Get_Instance()->Add_Effect(CEffect_Mgr::EFFECT_FIRE, &_tInfo);
	//		}
	//	}*/
	//	m_vPreFirePos = _tInfo.vPos;
	//	m_vPreFireDir = _tInfo.vDirect;
	//}
	
}

void CScythe::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


}

HRESULT CScythe::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_fColor", &CLIENT_RGB(119.f, 245.f, 200.f), sizeof(_float4))))
		return E_FAIL;

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	_uint		iNumMeshes = m_pModelCom->Get_NumMesh();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	m_pTrailCom->Render();

#ifdef _DEBUG
	if (nullptr != m_pColliderCom && m_bColliderOn)
		m_pColliderCom->Render();
#endif

	return S_OK;

}



HRESULT CScythe::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC	_Desc;
	_Desc.fRotationPerSec = XMConvertToRadians(90.f);
	//_Desc.fSpeedPerSec = 1.5f;
	_Desc.fSpeedPerSec = 2.5f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &_Desc)))
		return E_FAIL;

	/* For.Com_ParentTransform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_ParentTransform"), (CComponent**)&m_pParentTransformCom)))
		return E_FAIL;



	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Effect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Scythe"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Trail */
	CTrail::TRAILINFO _tInfo;
	_tInfo._Color = CLIENT_RGB(0.f,0.f,0.f);
	_tInfo._HighAndLow.vHigh = _float3(120.0f, 0.f, 0.f);
	_tInfo._HighAndLow.vLow = _float3(-135.f, 0.f, 0.f);
	//_tInfo._eOption = CTrail::TRAILOPTION_TEXTURE;
	//_tInfo._szTexture = TEXT("../Bin/Resources/Textures/Trail_Flame.png");
	AUTOINSTANCE(CGameInstance, _pInstance);
	m_pTrailCom = static_cast<CTrail_Obj*>(_pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail"), &_tInfo));
	if (m_pTrailCom == nullptr)
		return E_FAIL;


	/* For.Com_OBB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(50.0f, 50.f, 20.f);
	ColliderDesc.vCenter = _float3(65.f, ColliderDesc.vSize.y * 0.5f - 30.f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.f), 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

CScythe * CScythe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CScythe*		pInstance = new CScythe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CScythe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CScythe::Clone(void * pArg)
{
	CScythe*		pInstance = new CScythe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScythe::Free()
{
	__super::Free();
}

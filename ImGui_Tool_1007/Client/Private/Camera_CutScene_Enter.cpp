#include "stdafx.h"
#include "..\Public\Camera_CutScene_Enter.h"
#include "GameInstance.h"
#include "Cell.h"
#include "HierarchyNode.h"

CCamera_CutScene_Enter::CCamera_CutScene_Enter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CClient_Camere(pDevice, pContext)
	, m_pTarget(nullptr)
{
}

CCamera_CutScene_Enter::CCamera_CutScene_Enter(const CCamera_CutScene_Enter & rhs, CTransform::TRANSFORMDESC * pArg)
	: CClient_Camere(rhs, pArg)
	, m_pTarget(rhs.m_pTarget)
{

}

HRESULT CCamera_CutScene_Enter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CCamera_CutScene_Enter::Initialize(void * pArg)
{
	/*if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;*/
	memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	AUTOINSTANCE(CGameInstance, pGameInstance);
	//m_pTarget = static_cast<CPlayer*>(pGameInstance->Get_Player());
	m_pTarget = static_cast<CTransform*>(pGameInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Transform")));
	Safe_AddRef(m_pTarget);
	m_pModel = static_cast<CAnimModel*>(pGameInstance->Get_Player()->Get_ComponentPtr(TEXT("Com_Model")));
	Safe_AddRef(m_pModel);
	m_pCameraBone = m_pModel->Get_HierarchyNode("camera");
	Safe_AddRef(m_pCameraBone);
	return S_OK;
}

void CCamera_CutScene_Enter::Tick(_float fTimeDelta)
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	switch (g_eCurLevel)
	{
	case Client::LEVEL_GAMEPLAY:
	{
		_float4x4	_matWorld;
		XMStoreFloat4x4(&_matWorld, m_pCameraBone->Get_OffSetMatrix()* m_pCameraBone->Get_CombinedTransformation()*XMLoadFloat4x4(&m_pModel->Get_PivotMatrix())*m_pTarget->Get_WorldMatrix());
		m_pTransformCom->Set_WorldFloat4x4(_matWorld);
		m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));
		_vector _vTargetPos = XMLoadFloat4(&m_vLookAt);
		m_CameraDesc.fNear = 0.05f;
		XMStoreFloat4(&m_CameraDesc.vAt, _vTargetPos);
		m_pTransformCom->LookAt(_vTargetPos);
		__super::Tick(fTimeDelta);
	}
		break;
	default:
		break;
	}

	
}

void CCamera_CutScene_Enter::LateTick(_float fTimeDelta)
{
	
}

HRESULT CCamera_CutScene_Enter::Render()
{
	return S_OK;
}

void CCamera_CutScene_Enter::Set_Target(CGameObject * _pTarget)
{
	Safe_Release(m_pTarget);
	Safe_Release(m_pModel);
	Safe_Release(m_pCameraBone);


	m_pTarget = static_cast<CTransform*>(_pTarget->Get_ComponentPtr(TEXT("Com_Transform")));
	Safe_AddRef(m_pTarget);
	m_pModel = static_cast<CAnimModel*>(_pTarget->Get_ComponentPtr(TEXT("Com_Model")));
	Safe_AddRef(m_pModel);
	m_pCameraBone = m_pModel->Get_HierarchyNode("camera");
	Safe_AddRef(m_pCameraBone);
}

CCamera_CutScene_Enter * CCamera_CutScene_Enter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_CutScene_Enter*		pInstance = new CCamera_CutScene_Enter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_CutScene_Enter"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_CutScene_Enter::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera_CutScene_Enter*		pInstance = new CCamera_CutScene_Enter(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_CutScene_Enter::Free()
{
	__super::Free();
	
	Safe_Release(m_pTarget);
	Safe_Release(m_pModel);
	Safe_Release(m_pCameraBone);
}

#include "stdafx.h"
#include "Balloon.h"
#include "Obj_NonAnim.h"
#include "Renderer.h"
#include "Shader.h"
#include "NonAnimModel.h"
#include "Transform.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"

CBalloon::CBalloon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObj_NonAnim(pDevice, pContext)
{
}

CBalloon::CBalloon(const CBalloon & rhs)
	: CObj_NonAnim(rhs)
{
}

HRESULT CBalloon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBalloon::Initialize(void * pArg)
{

	ZeroMemory(&m_tInfo, sizeof(OBJ_DESC));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg)
	{
		OBJ_DESC _tInfo = *static_cast<OBJ_DESC*>(pArg);
		m_tInfo.matWorld = _tInfo.matWorld;
		m_pTransformCom->Set_WorldFloat4x4(m_tInfo.matWorld);
	}
	

	AUTOINSTANCE(CGameInstance, _pInstance);


	return S_OK;
}

void CBalloon::Tick( _float fTimeDelta)
{
	ImGuiTick();

}

void CBalloon::LateTick( _float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	AUTOINSTANCE(CGameInstance, _pInstance);

	_bool		isDraw = _pInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f);
	if (isDraw)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	
}

HRESULT CBalloon::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;



	SetUp_ShaderResources();

	_uint		iNumMeshes = m_pModelCom->Get_NumMesh();//메쉬 갯수를 알고 메쉬 갯수만큼 렌더를 할 것임. 여기서!

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_ePass)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	

	return S_OK;
}


HRESULT CBalloon::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC	_Desc;
	_Desc.fRotationPerSec = XMConvertToRadians(90.f);
	//_Desc.fSpeedPerSec = 1.5f;
	_Desc.fSpeedPerSec =2.5f;
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
		if (FAILED(__super::Add_Component(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Balloon01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		lstrcpy(m_tInfo.szModelTag, TEXT("Prototype_Component_Model_Balloon01"));
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Balloon02"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		lstrcpy(m_tInfo.szModelTag, TEXT("Prototype_Component_Model_Balloon02"));
	}


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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
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

	
}
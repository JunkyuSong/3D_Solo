#include "stdafx.h"
#include "..\Public\Obj_NonAnim.h"
#include "GameInstance.h"
#include "ImGuiMgr.h"

CObj_NonAnim::CObj_NonAnim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObj_Plus(pDevice, pContext)
{
}

CObj_NonAnim::CObj_NonAnim(const CObj_NonAnim & rhs)
	: CObj_Plus(rhs)
{
}

HRESULT CObj_NonAnim::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CObj_NonAnim::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	ZeroMemory(&m_tInfo, sizeof(OBJ_DESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CObj_NonAnim::Tick( _float fTimeDelta)
{
	ImGuiTick();

}

void CObj_NonAnim::LateTick( _float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	//AUTOINSTANCE(CGameInstance, _pInstance);
	//_bool		isDraw = _pInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 15.f);
	//if (isDraw)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CObj_NonAnim::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;



	SetUp_ShaderResources();
	//119 215 223
	if (FAILED(m_pShaderCom->Set_RawValue("g_fColor", &CLIENT_RGB(119.f,235.f,223.f), sizeof(_float4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMesh();//�޽� ������ �˰� �޽� ������ŭ ������ �� ����. ���⼭!

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CObj_NonAnim::Set_Info(OBJ_DESC _tInfo)
{
	m_tInfo.eLevel = _tInfo.eLevel;

	if (m_pModelCom == nullptr)
	{
		lstrcpy(m_tInfo.szModelTag, _tInfo.szModelTag);
		
		if (__super::Add_Component(g_eCurLevel, m_tInfo.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom))
			return E_FAIL;

	}
	else if (lstrcmp(m_tInfo.szModelTag, _tInfo.szModelTag))
	{
		//Safe_Delete_Array(m_tInfo.szModelTag);
		Safe_Release(m_pModelCom);

		auto& iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(TEXT("Com_Model")));
		Safe_Release(iter->second);
		m_Components.erase(iter);

		lstrcpy(m_tInfo.szModelTag, _tInfo.szModelTag);
		if (__super::Add_Component(g_eCurLevel, m_tInfo.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom))
			return E_FAIL;
	}

	m_tInfo.matWorld = _tInfo.matWorld;
	m_pTransformCom->Set_WorldFloat4x4(m_tInfo.matWorld);
	return S_OK;
}

void CObj_NonAnim::ImGuiTick()
{

}

_bool CObj_NonAnim::Picking(_float3 & _vPos)
{
	_bool			_bPick;
	_vector			vPickPos;
	if (_bPick = m_pModelCom->Picking(m_pTransformCom, vPickPos))
	{
		XMStoreFloat3(&_vPos, vPickPos);
	}
	return _bPick;
}

HRESULT CObj_NonAnim::Ready_Components()
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
	/*if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CObj_NonAnim::SetUp_ShaderResources()
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

CObj_NonAnim * CObj_NonAnim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CObj_NonAnim*		pInstance = new CObj_NonAnim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CObj_NonAnim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CObj_NonAnim * CObj_NonAnim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<OBJ_DESC>* _tInfo)
{
	CObj_NonAnim*		pInstance = new CObj_NonAnim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CObj_NonAnim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CObj_NonAnim::Clone(void * pArg)
{
	CObj_NonAnim*		pInstance = new CObj_NonAnim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CObj_NonAnim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObj_NonAnim::Free()
{
	__super::Free();

	if (m_tInfo.szModelTag != nullptr)
	{
		//Safe_Delete_Array(m_tInfo.szModelTag);
	}
	Safe_Release(m_pModelCom);
}
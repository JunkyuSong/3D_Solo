#include "..\Public\DirLight.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Frustum.h"

CDirLight::CDirLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CDirLight::Initialize(const DIRLIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CDirLight::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer, CFrustum* _pFrustum)
{
	

	_uint		iPassIndex = 1;

	if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
		return E_FAIL;
	

	if (FAILED(pShader->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Render();

	return S_OK;
}

CDirLight * CDirLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const DIRLIGHTDESC & LightDesc)
{
	CDirLight*			pInstance = new CDirLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CPointLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDirLight::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

#include "..\Public\Light_Manager.h"
#include "Light.h"
#include "Frustum.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
	: m_pFrustum(CFrustum::Get_Instance())
{
}

HRESULT CLight_Manager::Initialize(_uint iLv)
{
	m_vecLights = new vector<class CLight*>[iLv];
	m_vecDeadLights = new vector<class CLight*>[iLv];
	m_iNumLevels = iLv;
	return S_OK;
}

LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iLv,_uint iIndex)
{
	if (iIndex >= m_vecLights[iLv].size() || m_vecLights[iLv][iIndex] == nullptr)
		return nullptr;
	return m_vecLights[iLv][iIndex]->Get_LightDesc();
}

HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iLv, const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(pDevice, pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_vecLights[iLv].push_back(nullptr);
	m_vecDeadLights[iLv].push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pLight : m_vecLights[i])
		{
			if (pLight)
				pLight->Render(pShader, pVIBuffer, m_pFrustum);
		}
	}

	return S_OK;
}

HRESULT CLight_Manager::Light_On(_uint iLv, _uint _iIndex)
{
	if (_iIndex >= m_vecDeadLights[iLv].size() || iLv >= m_iNumLevels)
		return E_FAIL;
	if (m_vecLights[iLv][_iIndex] == nullptr)
	{
		m_vecLights[iLv][_iIndex] = m_vecDeadLights[iLv][_iIndex];
		m_vecDeadLights[iLv][_iIndex] = nullptr;
	}
	return S_OK;
}

HRESULT CLight_Manager::Light_Off(_uint iLv, _uint _iIndex)
{
	if (_iIndex >= m_vecLights[iLv].size() || iLv >= m_iNumLevels)
		return E_FAIL;
	if (m_vecDeadLights[iLv][_iIndex] == nullptr)
	{
		m_vecDeadLights[iLv][_iIndex] = m_vecLights[iLv][_iIndex];
		m_vecLights[iLv][_iIndex] = nullptr;
	}

	return S_OK;
}

HRESULT CLight_Manager::Clear(_uint iLv)
{
	for (auto& pLight : m_vecLights[iLv])
		Safe_Release(pLight);
	m_vecLights[iLv].clear();
	return S_OK;
}

void CLight_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pLight : m_vecLights[i])
			Safe_Release(pLight);
		m_vecLights[i].clear();
	}
	Safe_Delete_Array(m_vecLights);

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pLight : m_vecDeadLights[i])
			Safe_Release(pLight);
		m_vecDeadLights[i].clear();
	}
	Safe_Delete_Array(m_vecDeadLights);
}

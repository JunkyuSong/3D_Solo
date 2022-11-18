#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
}



HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4 * pClearColor)
{
	if (nullptr != Find_RenderTarget(pTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pContext, iSizeX, iSizeY, eFormat, pClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);
	if(nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;

		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_SRV(const _tchar * pTargetTag, CShader * pShader, const char * pConstantName)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_SRV(pShader, pConstantName);
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext * pContext, const _tchar * pMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	_uint		iNumViews = 8;
	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);
		
	_uint			iNumRTVs = 0;

	ID3D11RenderTargetView*			RTVs[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{		
		//pRenderTarget->Clear();
		m_UsingTargets.push_back(pRenderTarget);
		Safe_AddRef(pRenderTarget);
		RTVs[iNumRTVs++] = pRenderTarget->Get_RTV();
		//m_pClearRenderTargets.push_back(RTVs[iNumRTVs-1]);
	}


	pContext->OMSetRenderTargets(iNumRTVs, RTVs, m_pOldDepthStencil);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext * pContext, _uint iDepthStencil)
{
	_uint		iNumRTVs = 8;	

	pContext->OMSetRenderTargets(iNumRTVs, m_pOldRenderTargets, m_pOldDepthStencil);

	for (_uint i = 0; i < 8; ++i)
		Safe_Release(m_pOldRenderTargets[i]);

	/*for (_uint i = 0; i < iDepthStencil; ++i)
		m_pOldDepthStencil->Release();*/

	Safe_Release(m_pOldDepthStencil);

	return S_OK;
}

HRESULT CTarget_Manager::AddBinding_RTV(ID3D11DeviceContext * pContext, const _tchar * pRTVTag, _uint _iIndex)
{
	CRenderTarget*		pRTV = Find_RenderTarget(pRTVTag);
	if (nullptr == pRTV)
		return E_FAIL;

	_uint		iNumViews = 8;
	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);

	_uint			iNumRTVs = 0;
	
	ID3D11RenderTargetView*			RTVs[8] = { nullptr };

	for (_uint i = 0 ; i < 8 ; ++i)
	{
		RTVs[i] = m_pOldRenderTargets[i];
		
		if (m_pOldRenderTargets[i] != nullptr)
			++iNumRTVs;
	}
	if(RTVs[_iIndex] == nullptr)
		++iNumRTVs;
	m_UsingTargets.push_back(pRTV);
	Safe_AddRef(pRTV);
	RTVs[_iIndex] = pRTV->Get_RTV();

	if (iNumRTVs > 2)
	{
		m_pOldRenderTargets[0]->Release();
		Safe_Release(m_pOldRenderTargets[0]);

		Safe_Release(m_pOldRenderTargets[1]);

		for (_uint i = 0 ; i < iNumRTVs -2 ; ++i)
			m_pClearRenderTargets.push_back(m_pOldRenderTargets[i]);

		for (_uint i = 0; i < iNumRTVs - 1; ++i)
			m_pClearRenderTargets.push_back(m_pOldRenderTargets[i]);
	}
		
	pContext->OMSetRenderTargets(iNumRTVs, RTVs, m_pOldDepthStencil);
	
	return S_OK;
}

HRESULT CTarget_Manager::Clear_RTVs()
{
	for (auto& _RTV : m_UsingTargets)
	{
		_RTV->Clear();
		Safe_Release(_RTV);
	}
	m_UsingTargets.clear();

	for (auto& RT : m_pClearRenderTargets)
	{
		Safe_Release(RT);
	}
	m_pClearRenderTargets.clear();

	return S_OK;
}

HRESULT CTarget_Manager::BackBuffer_Start(ID3D11DeviceContext * pContext, const _tchar * pBackBufferTag, ID3D11RenderTargetView ** _pBackBuffer)
{
	CRenderTarget*		pRTV = Find_RenderTarget(pBackBufferTag);
	if (nullptr == pRTV)
		return E_FAIL;

	_uint		iNumViews = 8;
	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);

	*_pBackBuffer = m_pOldRenderTargets[0];
	
	Safe_AddRef(*_pBackBuffer);

	m_UsingTargets.push_back(pRTV);
	Safe_AddRef(pRTV);

	m_pOldRenderTargets[0] = pRTV->Get_RTV();

	pContext->OMSetRenderTargets(1, m_pOldRenderTargets, m_pOldDepthStencil);

	Safe_Release(m_pOldDepthStencil);
	//m_pOldDepthStencil->Release();
	return S_OK;
}

HRESULT CTarget_Manager::BackBuffer_End(ID3D11DeviceContext * pContext, ID3D11RenderTargetView ** _pBackBuffer)
{
	_uint		iNumViews = 8;

	pContext->OMGetRenderTargets(iNumViews, m_pOldRenderTargets, &m_pOldDepthStencil);

	Safe_Release(m_pOldRenderTargets[0]);

	m_pOldRenderTargets[0] = *_pBackBuffer;

	Safe_Release(*_pBackBuffer);

	pContext->OMSetRenderTargets(1, m_pOldRenderTargets, m_pOldDepthStencil);

	for (_uint i = 0; i < 8; ++i)
		Safe_Release(m_pOldRenderTargets[i]);

	Safe_Release(m_pOldDepthStencil);


	return S_OK;
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Initialize_Debug(const _tchar * pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*		pRenderTarget =  Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Initialize_Debug(fX, fY, fSizeX, fSizeY);	
}

HRESULT CTarget_Manager::Render_Debug(const _tchar * pMRTTag, class CVIBuffer* pVIBuffer, class CShader* pShader)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;


	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render_Debug(pShader, pVIBuffer);
	}

	return S_OK;
}

#endif // _DEBUG

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(pTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;	
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(pMRTTag));
	if(iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{


	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}

	for (auto& iter : m_UsingTargets)
		Safe_Release(iter);

	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();

}

//#include "..\Public\RenderTarget.h"
//#include "VIBuffer_Rect.h"
//#include "Shader.h"
//
//CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
//	: m_pDevice(pDevice)
//	, m_pContext(pContext)
//{
//	Safe_AddRef(m_pDevice);
//	Safe_AddRef(m_pContext);
//}
//
//HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4 * pClearColor)
//{
//	m_vClearColor = *pClearColor;
//
//	D3D11_TEXTURE2D_DESC	TextureDesc;
//	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
//
//	TextureDesc.Width = iSizeX;
//	TextureDesc.Height = iSizeY;
//	TextureDesc.MipLevels = 1;
//	TextureDesc.ArraySize = 1;
//	TextureDesc.Format = eFormat;
//
//	TextureDesc.SampleDesc.Quality = 0;
//	TextureDesc.SampleDesc.Count = 1;
//
//	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
//	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
//	TextureDesc.CPUAccessFlags = 0;
//	TextureDesc.MiscFlags = 0;
//
//	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
//		return E_FAIL;
//
//	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
//		return E_FAIL;
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
//
//	tSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
//	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
//	tSRVDesc.Buffer.NumElements = iWidth;
//
//	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, &tSRVDesc, &m_pSRV)))
//		return E_FAIL;
//
//
//	return S_OK;
//}
//
//HRESULT CRenderTarget::Clear()
//{
//	if (nullptr == m_pContext)
//		return E_FAIL;
//
//	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);
//
//	return S_OK;
//}
//
//HRESULT CRenderTarget::Bind_SRV(CShader * pShader, const char * pConstantName)
//{
//	return pShader->Set_ShaderResourceView(pConstantName, m_pSRV);
//}
//
//#ifdef _DEBUG
//
//HRESULT CRenderTarget::Initialize_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
//{
//	_uint		iNumViewport = 1;
//	D3D11_VIEWPORT		ViewportDesc;
//	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);
//
//	XMStoreFloat4x4(&m_WorldMatrix,
//		XMMatrixTranspose(XMMatrixScaling(fSizeX, fSizeY, 0.f) * XMMatrixTranslation(fX - ViewportDesc.Width * 0.5f, -fY + ViewportDesc.Height * 0.5f, 0.f)));
//
//	return S_OK;
//}
//
//
//HRESULT CRenderTarget::Render_Debug(CShader* pShader, CVIBuffer * pVIBuffer)
//{
//	pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4));
//
//	pShader->Set_ShaderResourceView("g_DiffuseTexture", m_pSRV);
//
//	pShader->Begin(0);
//
//	return pVIBuffer->Render();
//}
//#endif // _DEBUG
//
//CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4 * pClearColor)
//{
//	CRenderTarget*		pInstance = new CRenderTarget(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, eFormat, pClearColor)))
//	{
//		MSG_BOX(TEXT("Failed To Created : CRenderTarget"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//void CRenderTarget::Free()
//{
//	Safe_Release(m_pTexture2D);
//	Safe_Release(m_pSRV);
//	Safe_Release(m_pRTV);
//
//	Safe_Release(m_pDevice);
//	Safe_Release(m_pContext);
//}

#include "HDR_Mgr.h"
#include "Shader.h"
#include "RenderTarget.h"
#

IMPLEMENT_SINGLETON(CHDR_Mgr)

//extern ID3D11Device* g_pDevice;
//extern ID3D11SamplerState* g_pSampPoint;
//
//HRESULT CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile, DWORD dwShaderFlags, ID3DBlob** ppVertexShaderBuffer);




HRESULT CHDR_Mgr::Initialize(const _uint& width, const _uint& height, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// Find the amount of thread groups needed for the downscale operation
	m_iWidth = width;
	m_iHeight = height;
	m_iDownScaleGroups = (_uint)ceil((float)(m_iWidth * m_iHeight / 16) / 1024.f);
	
	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);

	m_pContext = pContext;
	Safe_AddRef(m_pContext);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate down scaled luminance buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = sizeof(float);
	bufferDesc.ByteWidth = m_iDownScaleGroups * bufferDesc.StructureByteStride;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	
	if (FAILED(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pDownScale1DBuffer)))
		return E_FAIL;

	

	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory(&DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	DescUAV.Format = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DescUAV.Buffer.NumElements = m_iDownScaleGroups;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pDownScale1DBuffer, &DescUAV, &m_pDownScale1DUAV)))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate average luminance buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRV_Desc;
	ZeroMemory(&tSRV_Desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	tSRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
	tSRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	tSRV_Desc.Buffer.NumElements = m_iDownScaleGroups;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pDownScale1DBuffer, &tSRV_Desc, &m_pDownScale1DSRV)))
		return E_FAIL;

	bufferDesc.ByteWidth = sizeof(float);

	if (FAILED(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pAvgLumBuffer)))
		return E_FAIL;

	DescUAV.Buffer.NumElements = 1;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pAvgLumBuffer, &DescUAV, &m_pAvgLumUAV)))
		return E_FAIL;

	tSRV_Desc.Buffer.NumElements = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pAvgLumBuffer, &tSRV_Desc, &m_pAvgLumSRV)))
		return E_FAIL;

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(TDOWNSCALE_DESC);

	if (FAILED(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pDownScale)))
		return E_FAIL;

	bufferDesc.ByteWidth = sizeof(TFinalPass_DESC);

	if (FAILED(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pFinalPass)))
		return E_FAIL;

	// Compile the shaders
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT hr;

	hr = CreateComputeShader(TEXT("../Bin/shaderFiles/ComputeShader_DownScale.hlsl"), NULL, "DownScaleFirstPass", "cs_5_0", dwShaderFlags, &m_pDownScaleFirstPassCS);

	if (FAILED(hr))
		return E_FAIL;

	hr = CreateComputeShader(TEXT("../Bin/shaderFiles/ComputeShader_DownScale.hlsl"), NULL, "DownScaleSecondPass", "cs_5_0", dwShaderFlags, &m_pDownScaleSecondPassCS);
	
	if (FAILED(hr))
		return E_FAIL;

	m_pFullScreenShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/shaderFiles/Shader_HDR.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);

	if (m_pFullScreenShaderCom == nullptr)
	{
		return E_FAIL;
	}

	return S_OK;
}


void CHDR_Mgr::PostProcessing(CRenderTarget* _pRenderTarget, _float4x4* _Matrix)
{
	// Down scale the HDR image
	ID3D11ShaderResourceView* SRV = _pRenderTarget->Get_SRV();

	ID3D11RenderTargetView* RTVs[1] = { nullptr };
	m_pContext->OMSetRenderTargets(1, RTVs, nullptr);
	DownScale(SRV);

	// Do the final pass

	if (FAILED(m_pFullScreenShaderCom->Set_RawValue("g_WorldMatrix", &(_Matrix[0]), sizeof(_float4x4))))
		return;
	if (FAILED(m_pFullScreenShaderCom->Set_RawValue("g_ViewMatrix", &(_Matrix[1]), sizeof(_float4x4))))
		return;
	if (FAILED(m_pFullScreenShaderCom->Set_RawValue("g_ProjMatrix", &(_Matrix[2]), sizeof(_float4x4))))
		return;

	RTVs[0] = _pRenderTarget->Get_RTV();
	m_pContext->OMSetRenderTargets(1, RTVs, nullptr);
	FinalPass(m_pContext, SRV);
}

void CHDR_Mgr::PostProcessing(ID3D11ShaderResourceView * _pSRV, ID3D11RenderTargetView * _pRTV, _float4x4 * _Matrix)
{
	// Down scale the HDR image
	ID3D11ShaderResourceView* SRV = _pSRV;

	ID3D11RenderTargetView* RTVs[1] = { nullptr };
	m_pContext->OMSetRenderTargets(1, RTVs, nullptr);
	DownScale(SRV);

	// Do the final pass

	if (FAILED(m_pFullScreenShaderCom->Set_RawValue("g_WorldMatrix", &_Matrix[0], sizeof(_float4x4))))
		return;
	if (FAILED(m_pFullScreenShaderCom->Set_RawValue("g_ViewMatrix", &_Matrix[1], sizeof(_float4x4))))
		return;
	if (FAILED(m_pFullScreenShaderCom->Set_RawValue("g_ProjMatrix", &_Matrix[2], sizeof(_float4x4))))
		return;

	RTVs[0] = _pRTV;
	m_pContext->OMSetRenderTargets(1, RTVs, nullptr);
	FinalPass(m_pContext, SRV);
}

void CHDR_Mgr::DownScale(ID3D11ShaderResourceView* pHDRSRV)
{
	// Output
	ID3D11UnorderedAccessView* UAVs[1] = { m_pDownScale1DUAV };
	m_pContext->CSSetUnorderedAccessViews(0, 1, UAVs, (UINT*)(&UAVs));

	// Input
	ID3D11ShaderResourceView* SRVs[1] = { pHDRSRV };
	m_pContext->CSSetShaderResources(0, 1, SRVs);

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pDownScale, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	TDOWNSCALE_DESC* pDownScale = (TDOWNSCALE_DESC*)MappedResource.pData;
	pDownScale->iWidth = m_iWidth >> 2;
	pDownScale->iHeight = m_iHeight >> 2;
	pDownScale->iTotalPixels = pDownScale->iWidth * pDownScale->iHeight;
	pDownScale->iGroupSize = m_iDownScaleGroups;
	m_pContext->Unmap(m_pDownScale, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pDownScale };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);


	// Shader
	m_pContext->CSSetShader(m_pDownScaleFirstPassCS, nullptr, 0);

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	m_pContext->Dispatch(m_iDownScaleGroups, 1, 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Second pass - reduce to a single pixel

	// Outoput
	ZeroMemory(UAVs, sizeof(UAVs));
	UAVs[0] = m_pAvgLumUAV;
	m_pContext->CSSetUnorderedAccessViews(0, 1, UAVs, (_uint*)(&UAVs));

	// Input
	SRVs[0] = m_pDownScale1DSRV;
	m_pContext->CSSetShaderResources(0, 1, SRVs);

	// Constants
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Shader
	m_pContext->CSSetShader(m_pDownScaleSecondPassCS, nullptr, 0);

	// Excute with a single group - this group has enough threads to process all the pixels
	m_pContext->Dispatch(1, 1, 1);

	// Cleanup
	m_pContext->CSSetShader(nullptr, nullptr, 0);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
	ZeroMemory(SRVs, sizeof(SRVs));
	m_pContext->CSSetShaderResources(0, 1, SRVs);
	ZeroMemory(UAVs, sizeof(UAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 1, UAVs, (UINT*)(&UAVs));
}

void CHDR_Mgr::FinalPass(ID3D11DeviceContext* pd3dImmediateContext, ID3D11ShaderResourceView* pHDRSRV)
{
	ID3D11ShaderResourceView* arrViews[2] = { pHDRSRV, m_pAvgLumSRV };
	m_pContext->PSSetShaderResources(0, 2, arrViews);

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pFinalPass, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	TFinalPass_DESC* pFinalPass = (TFinalPass_DESC*)MappedResource.pData;
	pFinalPass->fMiddleGrey = m_fMiddleGrey;
	pFinalPass->fLumWhiteSqr = m_fWhite;
	pFinalPass->fLumWhiteSqr *= pFinalPass->fMiddleGrey; // Scale by the middle grey value
	pFinalPass->fLumWhiteSqr *= pFinalPass->fLumWhiteSqr; // Squre
	m_pContext->Unmap(m_pFinalPass, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pFinalPass };
	m_pContext->PSSetConstantBuffers(0, 1, arrConstBuffers);

	m_pContext->IASetInputLayout(nullptr);
	m_pContext->IASetVertexBuffers(NULL, NULL, nullptr, nullptr, nullptr);
	m_pContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ID3D11SamplerState* arrSamplers[1] = { m_pContext };
	//m_pContext->PSSetSamplers(0, 1, arrSamplers);

	// Set the shaders
	m_pFullScreenShaderCom->Begin(0);

	m_pContext->Draw(4, 0);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->PSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->PSSetConstantBuffers(0, 1, arrConstBuffers);
	m_pContext->VSSetShader(nullptr, nullptr, 0);
	m_pContext->PSSetShader(nullptr, nullptr, 0);
}

void CHDR_Mgr::Free()
{
	Safe_Release(m_pDownScale1DBuffer);
	Safe_Release(m_pDownScale1DUAV);
	Safe_Release(m_pDownScale1DSRV);
	Safe_Release(m_pDownScale);
	Safe_Release(m_pFinalPass);
	Safe_Release(m_pAvgLumBuffer);
	Safe_Release(m_pAvgLumUAV);
	Safe_Release(m_pAvgLumSRV);
	Safe_Release(m_pDownScaleFirstPassCS);
	Safe_Release(m_pDownScaleSecondPassCS);
	Safe_Release(m_pFullScreenShaderCom);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

HRESULT CHDR_Mgr::CreateComputeShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile, DWORD dwShaderFlags, ID3D11ComputeShader** _ppComputeShader)
{
	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(strPath, pMacros, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		strEntryPoint, strProfile,
		dwShaderFlags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			Safe_Release(errorBlob);
		}

		Safe_Release(shaderBlob);

		return hr;
	}

	//m_pBlob = shaderBlob;

	hr = m_pDevice->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, _ppComputeShader);
	return hr;
}
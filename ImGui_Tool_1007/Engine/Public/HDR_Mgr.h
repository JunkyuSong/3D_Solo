//#pragma once
//
//#include "Base.h"
//
//BEGIN(Engine)
//
//class CRenderTarget final : public CBase
//{
//private:
//	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual ~CRenderTarget() = default;
//
//public:
//	ID3D11RenderTargetView* Get_RTV() const {
//		return m_pRTV;
//	}
//
//	ID3D11Texture2D* Get_Texture2D() const {
//		return m_pTexture2D;
//	}
//
//public:
//	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor);
//	HRESULT Clear();
//	HRESULT Bind_SRV(class CShader* pShader, const char* pConstantName);
//#ifdef _DEBUG
//public:
//	HRESULT Initialize_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
//#endif // _DEBUG
//
//#ifdef _DEBUG
//public:
//	HRESULT Render_Debug(class CShader* pShader, class CVIBuffer* pVIBuffer);
//#endif // _DEBUG
//
//
//private:
//	ID3D11Device*				m_pDevice = nullptr;
//	ID3D11DeviceContext*		m_pContext = nullptr;
//
//	ID3D11Texture2D*			m_pTexture2D = nullptr;
//	ID3D11RenderTargetView*		m_pRTV = nullptr;
//	ID3D11ShaderResourceView*	m_pSRV = nullptr;
//
//	_float4						m_vClearColor = _float4(0.f, 0.f, 0.f, 1.f);
//
//
//#ifdef _DEBUG
//private:
//	_float4x4					m_WorldMatrix;
//#endif // _DEBUG
//
//public:
//	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor);
//	virtual void Free() override;
//};
//
//END

#pragma once

#include "Base.h"

BEGIN(Engine)
class CHDR_Mgr : public CBase
{

	DECLARE_SINGLETON(CHDR_Mgr)

private:
	CHDR_Mgr() = default;

	~CHDR_Mgr() = default;

public:
	HRESULT Initialize(const _uint& width, const _uint& height, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	// Entry point for post processing
	void PostProcessing(class CRenderTarget* _pRenderTarget, _float4x4* _Matrix);
	void PostProcessing(ID3D11ShaderResourceView* _pSRV, ID3D11RenderTargetView* _pRTV, _float4x4* _Matrix);

	void SetParameters(float fMiddleGrey, float fWhite) { m_fMiddleGrey = fMiddleGrey; m_fWhite = fWhite; }

private:
	HRESULT CreateComputeShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile, DWORD dwShaderFlags, ID3D11ComputeShader** _ppComputeShader);
	
private:
	// Down scale the full size HDR image
	void DownScale(ID3D11ShaderResourceView* pHDRSRV);

	// Final pass that composites all the post processing calculations
	void FinalPass(ID3D11DeviceContext* pd3dImmediateContext, ID3D11ShaderResourceView* pHDRSRV);

	// 1D intermediate storage for the down scale operation
	ID3D11Buffer* m_pDownScale1DBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pDownScale1DUAV = nullptr;
	ID3D11ShaderResourceView* m_pDownScale1DSRV = nullptr;

private:// Average luminance
	ID3D11Buffer* m_pAvgLumBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pAvgLumUAV = nullptr;
	ID3D11ShaderResourceView* m_pAvgLumSRV = nullptr;

	_uint m_iWidth = 0;
	_uint m_iHeight = 0;
	_uint m_iDownScaleGroups = 0;
	_float m_fMiddleGrey = 0.5f;
	_float m_fWhite = 5.f;
	
	typedef struct
	{
		_uint iWidth;
		_uint iHeight;
		_uint iTotalPixels;
		_uint iGroupSize;
	} TDOWNSCALE_DESC;

	ID3D11Buffer* m_pDownScale = nullptr;

	typedef struct
	{
		_float fMiddleGrey;
		_float fLumWhiteSqr;
		_uint pad[2];
	} TFinalPass_DESC;

	ID3D11Buffer* m_pFinalPass = nullptr;

private:// Shaders
	ID3D11ComputeShader* m_pDownScaleFirstPassCS = nullptr;
	ID3D11ComputeShader* m_pDownScaleSecondPassCS = nullptr;
	class CShader*		m_pFullScreenShaderCom = nullptr;

	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

public:
	// CBase을(를) 통해 상속됨
	virtual void Free() override;
};

END
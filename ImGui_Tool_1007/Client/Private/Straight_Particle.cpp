#include "stdafx.h"
#include "..\Public\Straight_Particle.h"
#include "GameInstance.h"

CStraight_Particle::CStraight_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Particle(pDevice, pContext)
{
}

CStraight_Particle::CStraight_Particle(const CEffect_Particle & rhs)
	: CEffect_Particle(rhs)
{
}


HRESULT CStraight_Particle::Initialize_Prototype(_tchar * szTextureTag)
{
	return S_OK;
}

HRESULT CStraight_Particle::Initialize(void * pArg)
{


	return S_OK;
}


const _bool & CStraight_Particle::Update(_float _fTimeDelta)
{	
	//현재 위치에서 센터까지의 디스턴스를 구해놓고 그걸 넘어가면 도달한 것.
	_vector _vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (m_fDistane < fabs((XMVectorSetW(XMLoadFloat3(&m_tOption.Center), 1.f) - _vPos).m128_f32[0]))
	{
		return false;
	}

	if (!MoveFrame(_fTimeDelta))
	{
		if (m_pTextureCom->Get_Type() == CTexture::TYPE_SINGLE)
			return false;
	}

	_vPos += XMLoadFloat3(&m_vDirect);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	return true;
}

HRESULT CStraight_Particle::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	AUTOINSTANCE(CGameInstance, pGameInstance);


	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4x4))))
		return E_FAIL;

	_uint _iMaxY, _iMaxX;

	m_pTextureCom->Get_HeightWidth(_iMaxY, _iMaxX);

	m_iCurSpriteY = m_iSpriteNum / _iMaxY;
	m_iCurSpriteX = m_iSpriteNum % _iMaxY;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iMaxY", &_iMaxY, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iMaxX", &_iMaxX, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iCurY", &m_iCurSpriteY, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iCurX", &m_iCurSpriteX, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Size", &(m_tOption.Size), sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (m_pMaskTextureCom)
	{
		if (FAILED(m_pMaskTextureCom->Set_SRV(m_pShaderCom, "g_NoiseTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CStraight_Particle::Init_Again(OPTION _tOption, CTexture* _pTextureCom, CTexture* _pMaskCom, CVIBuffer_Point* _pBufferCom)
{
	AUTOINSTANCE(CGameInstance, _pInstance);

	m_tOption = _tOption;

	m_pVIBufferCom = _pBufferCom;
	m_pTextureCom = _pTextureCom;
	m_pMaskTextureCom = _pMaskCom;
	Safe_AddRef(m_pVIBufferCom);
	Safe_AddRef(m_pTextureCom);
	Safe_AddRef(m_pMaskTextureCom);

	_vector _vPos;
	_vPos.m128_f32[3] = 1.f;
	_vPos.m128_f32[0] = _pInstance->Rand_Float(m_fMinMaxX.x, m_fMinMaxX.y);
	_vPos.m128_f32[1] = _pInstance->Rand_Float(m_fMinMaxY.x, m_fMinMaxY.y);
	_vPos.m128_f32[2] = _pInstance->Rand_Float(m_fMinMaxZ.x, m_fMinMaxZ.y);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);

	m_fDistane = fabs((XMVectorSetW(XMLoadFloat3(&m_tOption.Center), 1.f) - _vPos).m128_f32[0]);

	switch (m_tOption.Spread)
	{
	case SPREAD_CENTER:
		XMStoreFloat3(&m_vDirect, XMLoadFloat3(&m_tOption.Center) - _vPos);
		break;
	case SPREAD_EDGE:
		XMStoreFloat3(&m_vDirect, (XMLoadFloat3(&m_tOption.Center) - _vPos) * -1.f);
		break;
	}
}

void CStraight_Particle::Recycle()
{
	AUTOINSTANCE(CGameInstance, _pInstance);
	_vector _vPos;
	_vPos.m128_f32[3] = 1.f;
	_vPos.m128_f32[0] = _pInstance->Rand_Float(m_fMinMaxX.x, m_fMinMaxX.y);
	_vPos.m128_f32[1] = _pInstance->Rand_Float(m_fMinMaxY.x, m_fMinMaxY.y);
	_vPos.m128_f32[2] = _pInstance->Rand_Float(m_fMinMaxZ.x, m_fMinMaxZ.y);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);

	m_fDistane = fabs((XMVectorSetW(XMLoadFloat3(&m_tOption.Center), 1.f) - _vPos).m128_f32[0]);

	switch (m_tOption.Spread)
	{
	case SPREAD_CENTER:
		XMStoreFloat3(&m_vDirect, XMLoadFloat3(&m_tOption.Center) - _vPos);
		break;
	case SPREAD_EDGE:
		XMStoreFloat3(&m_vDirect, (XMLoadFloat3(&m_tOption.Center) - _vPos) * -1.f);
		break;
	}
}

CEffect_Particle * CStraight_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _tchar * szTextureTag)
{
	CStraight_Particle*		pInstance = new CStraight_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(szTextureTag)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStraight_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStraight_Particle::Clone(void * pArg)
{
	CStraight_Particle*		pInstance = new CStraight_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStraight_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStraight_Particle::Free()
{
	__super::Free();
}
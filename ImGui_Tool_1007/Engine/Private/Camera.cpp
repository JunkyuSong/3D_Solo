#include "..\Public\Camera.h"
#include "PipeLine.h"
#include "Timer_Manager.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera & rhs, CTransform::TRANSFORMDESC* pArg)
	: CGameObject(rhs)
	, m_pTransformCom((CTransform*)rhs.m_pTransformCom->Clone(pArg))
{

}

HRESULT CCamera::Initialize_Prototype()
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CameraDesc.vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
	fTimeDelta /= CTimer_Manager::Get_Instance()->Get_TimeSpeed(TEXT("Timer_Main"));
	if (m_bZoomIn)
	{
		Cam_ZoomIn(fTimeDelta);
	}
	else if (m_bZoomOut)
	{
		Cam_ZoomOut(fTimeDelta);
	}
	else
	{
		if (fabs(m_CameraDesc.fFovy - XMConvertToRadians(60.f)) < XMConvertToRadians(3.f))
		{
			m_CameraDesc.fFovy = XMConvertToRadians(60.f);
		}
		else
		{
			if (m_CameraDesc.fFovy > XMConvertToRadians(60.f))
				m_CameraDesc.fFovy -= XMConvertToRadians(m_fZoomSpeed * fTimeDelta);
			else if (m_CameraDesc.fFovy < XMConvertToRadians(60.f))
				m_CameraDesc.fFovy += XMConvertToRadians(m_fZoomSpeed * fTimeDelta);
		}
	}

	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInverse());
	pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

	RELEASE_INSTANCE(CPipeLine);
}

void CCamera::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}


void CCamera::ZoomOut(_float fFoV, _float fSpeed)
{
	m_bZoomOut = true;
	m_bZoomIn = false;
	m_fZoomFOV = XMConvertToRadians( fFoV);
	m_fZoomSpeed = fSpeed;
}

void CCamera::ZoomOff(_float fSpeed)
{
	m_bZoomOut = false;
	m_bZoomIn = false;
	m_fZoomSpeed = fSpeed;
}

void CCamera::Set_FOV(_float fFOV)
{
	m_CameraDesc.fFovy = XMConvertToRadians(fFOV);
}

void CCamera::ZoomIn(_float fFoV, _float fSpeed)
{
	m_bZoomOut = false;
	m_bZoomIn = true;
	m_fZoomFOV = XMConvertToRadians(fFoV);
	m_fZoomSpeed = fSpeed;
}

void CCamera::Cam_ZoomOut(_float fTimeDelta)
{
	//°¢µµ°¡ ´Ã¸é ÁÜ¾Æ¿ô
	if (fabs(m_CameraDesc.fFovy - m_fZoomFOV) < XMConvertToRadians(3.f))
	{
		m_CameraDesc.fFovy = m_fZoomFOV;
	}
	else
	{
		m_CameraDesc.fFovy += XMConvertToRadians( m_fZoomSpeed * fTimeDelta);
	}

}

void CCamera::Cam_ZoomIn(_float fTimeDelta)
{
	//°¢µµ°¡ ÁÙ¸é ÁÜÀÎ
	if (fabs(m_CameraDesc.fFovy - m_fZoomFOV) < XMConvertToRadians(3.f))
	{
		m_CameraDesc.fFovy = m_fZoomFOV;
	}
	else
	{
		m_CameraDesc.fFovy -= XMConvertToRadians(m_fZoomSpeed * fTimeDelta);
	}
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}

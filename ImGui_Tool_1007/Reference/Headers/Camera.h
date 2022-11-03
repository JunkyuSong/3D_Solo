#pragma once

/* Ŭ�󿡼� ������ ī�޶���� �θ�. */
/* ��� ī�޶� �������� �⺻���� ����� �����Ѵ�. */
#include "GameObject.h"
#include "Transform.h"


BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;

		CTransform::TRANSFORMDESC		TransformDesc;
	}CAMERADESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	class CTransform*			Get_Transform() { return m_pTransformCom; }

	void ZoomIn(_float fFoV, _float fSpeed);
	void ZoomOut(_float fFoV, _float fSpeed);
	void ZoomOff(_float fSpeed);
	void Set_FOV(_float fFOV);

protected:
	void Cam_ZoomIn(_float fTimeDelta);
	void Cam_ZoomOut(_float fTimeDelta);

protected:
	class CTransform*			m_pTransformCom = nullptr;
	CAMERADESC					m_CameraDesc;

	_float			m_fZoomSpeed = 0.f;

	_bool			m_bZoomIn = false;
	_float			m_fZoomFOV = 0.f;
	_bool			m_bZoomOut = false;


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();
};

END
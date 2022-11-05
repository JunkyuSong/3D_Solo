#pragma once

#include "Light.h"

BEGIN(Engine)
/* �� ������ �����Ѵ�. */

class CPointLight : public CLight
{
protected:
	CPointLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPointLight() = default;

public:
	POINTLIGHTDESC* Get_LightDesc() {
		return &m_LightDesc;
	}

public:
	HRESULT Initialize();
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, class CFrustum* _pFrustum);

protected:
	POINTLIGHTDESC				m_LightDesc;

public:
	virtual void Free() override;
};

END
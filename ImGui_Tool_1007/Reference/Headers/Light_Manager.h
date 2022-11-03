#pragma once

#include "Base.h"

BEGIN(Engine)

/* 빛들을 보관한다. */

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
public:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	HRESULT	Initialize(_uint iLv);

public:
	LIGHTDESC* Get_LightDesc(_uint iLv, _uint iIndex);
	

public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,  _uint iLv, const LIGHTDESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
	HRESULT	Light_On(_uint iLv, _uint _iIndex);
	HRESULT	Light_Off(_uint iLv, _uint _iIndex);
	HRESULT Clear(_uint iLv);

private:
	_uint							m_iNumLevels = 0;
	class CFrustum*					m_pFrustum = nullptr;

	vector<class CLight*>*			m_vecLights = nullptr;
	vector<class CLight*>*			m_vecDeadLights = nullptr;
	typedef vector<class CLight*>	LIGHTS;

public:
	virtual void Free() override;
};

END
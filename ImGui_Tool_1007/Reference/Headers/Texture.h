#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TEXTURE_TYPE {TYPE_SINGLE, TYPE_MULTI, TYPE_END};
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures, _float _fSpeed);
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iWidthNum, _uint iHighNum, _float _fSpeed);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Set_SRV(class CShader* pShader, const char* pConstantName, _uint iTextureIndex = 0);

public:
	_tchar*					Get_Name() { return m_szName; }
	const _uint&			Get_MaxTexture() { return m_iNumTextures; }

	void					Get_HeightWidth(_uint& _iHeight, _uint& _iWidth) { _iHeight = m_iHighNum; _iWidth = m_iWidthNum; }

	const TEXTURE_TYPE&		Get_Type() { return m_eType; }

	const _float&		Get_Speed() { return m_fSpriteSpeed; }

private:	
	/* ���̴��� ���� ���ε� �� �� �ִ� ��ü */
	vector<ID3D11ShaderResourceView*>				m_SRVs;
	typedef vector<ID3D11ShaderResourceView*>		SRVS;

	_tchar			m_szName[MAX_PATH];

	_uint			m_iNumTextures = 0;
	_uint			m_iHighNum = 1;
	_uint			m_iWidthNum = 1;

	_float			m_fSpriteSpeed = 0.f;

	TEXTURE_TYPE	m_eType = TYPE_END;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1, _float _fSpeed = 0.f);
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iWidthNum, _uint iHighNum, _float _fSpeed = 0.f);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};

END
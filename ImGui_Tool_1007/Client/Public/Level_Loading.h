#pragma once

#include "Client_Defines.h"
#include "Level_Client.h"


BEGIN(Client)

class CLevel_Loading final : public CLevel_Client
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(const _uint& _eNextLevel);

	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT	Ready_Component();

private:
	LEVEL				m_eNextLevel = LEVEL_END;
	class CLoader*		m_pLoader = nullptr;


public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END
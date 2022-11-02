#pragma once

/* Ư�� ������ ���Ǵ� ��ü������ ��Ƽ� �����صд�. */
/* �����ϰ� �մ� ��ü���� Tick�� ȣ�����ش�. */

#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iLayerIndex);


public:
	HRESULT Reserve_Container(_uint iNumLevels);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr, CGameObject** pOut = nullptr);
	HRESULT Add_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	map<const _tchar*, class CLayer*>  Get_Layers(_uint iLevelIndex);

public:
	void Tick(const _float& fTimeDelta);
	void LateTick(const _float& fTimeDelta);
	void Clear(const _uint& iLevelIndex);
	
public: //Tool
	class CLayer*									Get_Layer(_uint iLevelIndex, _tchar* _pLayerTag);


private: /* ������ü���� ������ �����̳� */
	typedef map<const _tchar*, class CGameObject*>	PROTOTYPES;
	PROTOTYPES										m_Prototypes;


private: /* �纻��ü���� ������ �����̳� */
	_uint											m_iNumLevels = 0;
	typedef map<const _tchar*, class CLayer*>		LAYERS;
	LAYERS*											m_pLayers = nullptr;

private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END
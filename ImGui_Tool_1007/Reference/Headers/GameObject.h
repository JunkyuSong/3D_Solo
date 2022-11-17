#pragma	 once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum OBJTYPE { TYPE_PLAYER, TYPE_MONSTER, TYPE_CAMERA, TYPE_STATIC, TYPE_END };

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag);
	const _float& Get_CamDistance() const {
		return m_fCamDistance;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick( _float fTimeDelta);
	virtual void LateTick( _float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) { return S_OK; }
	HRESULT Compute_CamZ(_fvector vWorldPos);
	OBJTYPE Get_ObjType() { return m_eTypeObj; }

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

protected: /* ��ü���� �߰��� ������Ʈ���� Ű�� �з��Ͽ� �����Ѵ�. */
	map<const _tchar*, class CComponent*>			m_Components;
	typedef map<const _tchar*, class CComponent*>	COMPONENTS;

protected:
	_float				m_fCamDistance = 0.f;
	OBJTYPE				m_eTypeObj = TYPE_END;

protected:
	HRESULT Add_Component(const _uint& iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	

private:
	class CComponent* Find_Component(const _tchar* pComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
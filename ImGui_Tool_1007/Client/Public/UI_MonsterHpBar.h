#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

//Ÿ������ �ٲ�� �׿� �°� �ٲ�����.
// ������ ü�¹ٴ� �̰ɷ� �ϰ�,
// �Ϲ� ���� ü�¹ٴ� ���� ��������!

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
class CRenderer;
class CVIBuffer_Rect;
class CVIBuffer_Point;
END

BEGIN(Client)

class CUI_MonsterHpBar :
	public CGameObject
{
private:
	enum HPBAR { BAR_LEFTEDGE, BAR_BACK, BAR_RIGHTEDGE, BAR_HP, BAR_DISAPPEAR, BAR_END	};
	struct RECT
	{
		_float3		vCenter;
		_float2		fSize;
		_float		fAlpha;
	};

public:
	CUI_MonsterHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MonsterHpBar(const CUI_MonsterHpBar& rhs);
	virtual ~CUI_MonsterHpBar() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Set_Target(class CStatus* _pTarget);



private:
	class CStatus*			m_pTarget = nullptr;//�������ͽ� �����ñ��
	_float					m_fPreHP;
	_float					m_fMaxHP;

	_float					m_fBarSize;

	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;

	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransCom[BAR_END] = { nullptr };
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom =  nullptr;
	CTexture*				m_pTextureCom[BAR_END] = { nullptr };

	CVIBuffer_Point*		m_pVIBufferCom_Point = nullptr;
	CShader*				m_pShaderCom_Point = nullptr;

	//��Ʈ�� ������? 
	list<RECT>			m_tRects;

private:
	HRESULT Ready_Components();

public:
	static CUI_MonsterHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg) override;
	virtual void Free() override;

};

END
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSkillSlot final : public CGameObject
{
public:
	enum SKILL {
		SKILL_1,
		SKILL_2,
		SKILL_3,
		SKILL_4,
		SKILL_5,
		SKILL_6,
		SKILL_CLAW,
		SKILL_END
	};
	enum SKILLSLOT {
		SKILLSLOT_GREEN,
		SKILLSLOT_MINI,
		SKILLSLOT_GRAY,
		SKILLSLOT_CLAW,
		SKILLSLOT_END
	};
private:
	CSkillSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillSlot(const CSkillSlot& rhs);
	virtual ~CSkillSlot() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	_bool Check_CoolDown(SKILL _eSkillNum);

private:
	CShader*				m_pShaderCom = nullptr;
	
	CRenderer*				m_pRendererCom = nullptr;
	
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

	CTransform*				m_pTransformCom_SkillSlot[SKILL_END] = { nullptr };
	CTexture*				m_pTextureCom_SkillSlot[SKILLSLOT_END] = { nullptr };

	CTransform*				m_pTransformCom_Skill[SKILL_END] = { nullptr };
	CTransform*				m_pTransformCom_Claw = nullptr;
	CTexture*				m_pTextureCom_Skill[SKILL_END] = { nullptr };

	_float					m_fMaxCoolDown = 5.f;
	_float					m_fSkillCoolDown[SKILL_CLAW] = { 0.f };

private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Transform();
	HRESULT Ready_Texture();

	HRESULT Render_Slot();

public:
	static CSkillSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
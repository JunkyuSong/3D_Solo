#pragma once
#include "Effect.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Point_Instancing;
class CVIBuffer_Point;
END

BEGIN(Client)
class CEffect_Particle :
	public CEffect
{
public:
	enum PARTICLETYPE { TYPE_STRIGHT, TYPE_EDGE, TYPE_INTANCE, TYPE_END };
	enum SPREAD { SPREAD_CENTER, SPREAD_EDGE, SPREAD_END };
	struct OPTION
	{
		_tchar*							szTextureTag = nullptr;
		_tchar*							szMaskTag = nullptr;
		PARTICLETYPE					eType = TYPE_END;
		_float3							Center;
		SPREAD							Spread = SPREAD_EDGE;
		_uint							iNumParticles = 0;
		_float							fGravity = 0.f;
		_float							fAccSpeed = 0.f;
		_float							fLifeTime = 0.f;
		_float2							Size;
		_float3							fRange;
	};

protected:
	CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Particle(const CEffect_Particle& rhs);
	virtual ~CEffect_Particle() = default;

public:
	HRESULT Initialize_Prototype(_tchar * szTextureTag);
	virtual HRESULT Initialize(void* pArg);
	virtual const _bool & Update(_float _fTimeDelta) override;
	virtual HRESULT Render();

public:
	virtual void	Init_Again(OPTION _tOption, CTexture* _pTextureCom, CTexture* _pMaskCom, CVIBuffer_Point* _pBufferCom) {}
	virtual void	Recycle() {}
	void			Set_Option(OPTION _tOption) { m_tOption = _tOption; }

protected:
	_bool			MoveFrame(_float _fDeltaTime);

private:
	HRESULT			Ready_Components();

protected:
	CTexture*						m_pTextureCom = nullptr;
	CTexture*						m_pMaskTextureCom = nullptr;
	CVIBuffer_Point*				m_pVIBufferCom = nullptr;
	OPTION							m_tOption;
	_uint							m_iSpriteNum = 0;
	_uint							m_iCurSpriteY = 0;
	_uint							m_iCurSpriteX = 0;

	_float							m_fSpriteTime = 0.f;

	_float2							m_fMinMaxX;
	_float2							m_fMinMaxY;
	_float2							m_fMinMaxZ;

private:
	list<CEffect_Particle*>			m_Particles;
	_float							m_fCurLifeTime = 0.f;



	

public:	
	static CEffect_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _tchar* szTextureTag = nullptr);
	virtual CGameObject * Clone(void * pArg) override;
	virtual void Free() override;

};

END

/*
��ƼŬ �ȿ� ��ƼŬ ����Ʈ�� �ְ�
�װ� �����鼭 ���
�� Ŭ������ �뵵��
�ܼ� �θ� �ƴ϶� �ڽĵ��� �����ϴ� Ŭ�����̴�
�� ��ƼŬ�� ��ȸ��Ű�� update��Ų��
�ڽ��� �ؽ��Ŀ� ���۸� �ڽ�
*/
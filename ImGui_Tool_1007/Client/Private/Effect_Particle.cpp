#include "stdafx.h"
#include "..\Public\Effect_Particle.h"

#include "GameInstance.h"
#include "DeadParticle_Mgr.h"


CEffect_Particle::CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEffect_Particle::CEffect_Particle(const CEffect_Particle& rhs)
	: CEffect(rhs)
	, m_pVIBufferCom(rhs.m_pVIBufferCom)
	, m_pTextureCom(rhs.m_pTextureCom)
{
}

HRESULT CEffect_Particle::Initialize_Prototype(_tchar * szTextureTag)
{
	return S_OK;
}

HRESULT CEffect_Particle::Initialize(void * pArg)
{
	if (!pArg)
	{
		MSG_BOX(TEXT("no Option"));
		return E_FAIL;
	}
		

	m_tOption = *(OPTION*)pArg;

	m_fMinMaxX = { m_tOption.Center.x - (m_tOption.fRange.x * 0.5f), m_tOption.Center.x + (m_tOption.fRange.x * 0.5f) };
	m_fMinMaxY = { m_tOption.Center.y - (m_tOption.fRange.y * 0.5f), m_tOption.Center.y + (m_tOption.fRange.y * 0.5f) };
	m_fMinMaxZ = { m_tOption.Center.z - (m_tOption.fRange.z * 0.5f), m_tOption.Center.z + (m_tOption.fRange.z * 0.5f) };

	CEffect_Particle* _pParticle = nullptr;
	for (_uint i = 0; i < m_tOption.iNumParticles; ++i)
	{
		_pParticle = CDeadParticle_Mgr::Get_Instance()->Get_Straight_Particle();
		if (nullptr == _pParticle)
			return E_FAIL;
		_pParticle->Init_Again(m_tOption, m_pTextureCom, m_pVIBufferCom);
		m_Particles.push_back(_pParticle);
	}

	return S_OK;
}

const _bool & CEffect_Particle::Update(_float _fTimeDelta)
{
	m_fCurLifeTime += _fTimeDelta;

	AUTOINSTANCE(CDeadParticle_Mgr, _pInstance);
	if (m_Particles.size() == 0)
	{
		//갖고 있는 입자들 싹 초기화 해주고 파티클 생성관리자에 넘겨준다 -> 이미 다 넘겨주고 삭제된다
		//파티클 생성 관리자는 이미 사용한 파티클만 갖고있는다.
		return false;
	}

	for (auto& iter = m_Particles.begin(); iter != m_Particles.end(); )
	{
		if ((*iter)->Update(_fTimeDelta))
		{
			if (m_fCurLifeTime > m_tOption.fLifeTime)
			{
				_pInstance->Dead_Straight_Particle();
				iter = m_Particles.erase(iter);
				//그냥 이레이즈 하면 안되지

			}
			else
			{
				(*iter)->Recycle();
			}
		}
		else
		{
			++iter;
		}
	}
		
	return true;
}

HRESULT CEffect_Particle::Render()
{
	return S_OK;
}

_bool CEffect_Particle::MoveFrame(_float _fDeltaTime)
{
	m_fSpriteTime += _fDeltaTime;

	if (m_fSpriteTime > m_pTextureCom->Get_Speed())
	{
		m_fSpriteTime -= m_pTextureCom->Get_Speed();
		++m_iSpriteNum;
	}

	if (m_iSpriteNum >= m_pTextureCom->Get_MaxTexture()) // 벡터 사이즈 - 1
	{
		m_iSpriteNum = 0;
		return true;
	}

	return false;
}



CEffect_Particle * CEffect_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _tchar * szTextureTag)
{
	CEffect_Particle*		pInstance = new CEffect_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(szTextureTag)))
	{
		MSG_BOX(TEXT("Failed To Create : CEffect_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Particle::Clone(void * pArg)
{
	CEffect_Particle*		pInstance = new CEffect_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CEffect_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Particle::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
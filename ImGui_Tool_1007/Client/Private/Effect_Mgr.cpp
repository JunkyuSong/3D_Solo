#include "stdafx.h"
#include "..\Public\Effect_Mgr.h"

#include "GameInstance.h"

#include "Straight_Particle.h"

IMPLEMENT_SINGLETON(CEffect_Mgr)

void CEffect_Mgr::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < EFFECT_END; ++i)
	{
		for (auto& _iter = m_pEffects[i].begin(); _iter != m_pEffects[i].end();)
		{
			if (false == (*_iter)->Update(fTimeDelta))
			{
				m_pDeadEffects[i].push_back(*_iter);
				_iter = m_pEffects[i].erase(_iter);
			}
			else
			{
				++_iter;
			}
		}
	}
}

CEffect* CEffect_Mgr::Add_Effect(EFFECT_TYPE _eType, void * pArg)
{
	AUTOINSTANCE(CGameInstance, _pGameInstance);
	CEffect* _pEffect = nullptr;
	switch (_eType)
	{
	case Client::CEffect_Mgr::EFFECT_CLAW:
		if (m_pDeadEffects[EFFECT_CLAW].size() > 0)
		{
			_pEffect = m_pDeadEffects[EFFECT_CLAW].back();
			m_pDeadEffects[EFFECT_CLAW].pop_back();
			_pEffect->Initialize(pArg);
		}
		else
		{
			_pEffect = static_cast<CEffect*>(_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Claw_Effect"), pArg));
			if (_pEffect == nullptr)
			{
				MSG_BOX(TEXT("fail to clone effect(particle)"));
				Safe_Release(_pEffect);
				return nullptr;
			}

		}
		m_pEffects[EFFECT_CLAW].push_back(_pEffect);
		break;
	case Client::CEffect_Mgr::EFFECT_FIRE:
		if (m_pDeadEffects[EFFECT_FIRE].size() > 0)
		{
			_pEffect = m_pDeadEffects[EFFECT_FIRE].back();
			m_pDeadEffects[EFFECT_FIRE].pop_back();
			_pEffect->Initialize(pArg);
		}
		else
		{
			_pEffect = static_cast<CEffect*>(_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Fire_Effect"), pArg));
			if (_pEffect == nullptr)
			{
				MSG_BOX(TEXT("fail to clone effect(EFFECT_FIRE)"));
				Safe_Release(_pEffect);
				return nullptr;
			}

		}
		m_pEffects[EFFECT_FIRE].push_back(_pEffect);
		break;
	case Client::CEffect_Mgr::EFFECT_PARTICLE:
		if (m_pDeadEffects[EFFECT_PARTICLE].size() > 0)
		{
			_pEffect = m_pDeadEffects[EFFECT_PARTICLE].back();
			m_pDeadEffects[EFFECT_PARTICLE].pop_back();
			_pEffect->Initialize(pArg);
		}
		else
		{
			_pEffect = static_cast<CEffect*>(_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleEffect"), pArg));
			if (_pEffect == nullptr)
			{
				MSG_BOX(TEXT("fail to clone effect(particle)"));
				Safe_Release(_pEffect);
				return nullptr;
			}
			
		}
		m_pEffects[EFFECT_PARTICLE].push_back(_pEffect);
		break;
	case Client::CEffect_Mgr::EFFECT_CROSSTRAIL:
		if (m_pDeadEffects[EFFECT_PARTICLE].size() > 0)
		{
			_pEffect = m_pDeadEffects[EFFECT_CROSSTRAIL].back();
			m_pDeadEffects[EFFECT_CROSSTRAIL].pop_back();
			_pEffect->Initialize(pArg);
		}
		else
		{
			_pEffect = static_cast<CEffect*>(_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_CrossTrail"), pArg));
			if (_pEffect == nullptr)
			{
				MSG_BOX(TEXT("fail to clone effect(CrossTrail)"));
				Safe_Release(_pEffect);
				return nullptr;
			}

		}
		m_pEffects[EFFECT_CROSSTRAIL].push_back(_pEffect);
		break;
	}

	return _pEffect;
}

HRESULT CEffect_Mgr::Add_ParticleEffect(CEffect_Particle::PARTICLETYPE _eParticleType, void * pArg)
{
	
	switch (_eParticleType)
	{
	case Client::CEffect_Particle::TYPE_STRIGHT:
		
		break;
	case Client::CEffect_Particle::TYPE_EDGE:
		break;
	case Client::CEffect_Particle::TYPE_INTANCE:
		break;
	}



	return S_OK;
}

void CEffect_Mgr::Clear_Level(LEVEL _eLv)
{
}

void CEffect_Mgr::Free()
{

	for (_uint i = 0; i < EFFECT_END; ++i)
	{
		for (auto& iter : m_pEffects[i])
			Safe_Release(iter);
		for (auto& iter : m_pDeadEffects[i])
			Safe_Release(iter);

		m_pEffects[i].clear();
		m_pDeadEffects[i].clear();
	}
}

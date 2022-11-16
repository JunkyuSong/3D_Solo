#include "stdafx.h"
#include "DeadParticle_Mgr.h"
#include "Straight_Particle.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CDeadParticle_Mgr)


CDeadParticle_Mgr::CDeadParticle_Mgr()
{
}

CStraight_Particle * CDeadParticle_Mgr::Get_Straight_Particle(CEffect_Particle::OPTION _tOption)
{
	CStraight_Particle* _pParticle = nullptr;
	if (m_Dead_Straight_Particles.size() > 0)
	{
		_pParticle = m_Dead_Straight_Particles.back();
		_pParticle->Init_Again(_tOption);
		m_Dead_Straight_Particles.pop_back();
	}
	else
	{
		AUTOINSTANCE(CGameInstance, _pGameInstance);
		_pParticle = static_cast<CStraight_Particle*>(_pGameInstance->Clone_GameObject(TEXT("")));
	}

	return _pParticle;
}

void CDeadParticle_Mgr::Dead_Straight_Particle()
{
}

void CDeadParticle_Mgr::Free()
{
}

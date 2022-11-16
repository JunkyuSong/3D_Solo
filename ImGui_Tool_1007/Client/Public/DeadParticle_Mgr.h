#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Effect_Particle.h"

BEGIN(Client)
class CDeadParticle_Mgr :
	public CBase
{
	DECLARE_SINGLETON(CDeadParticle_Mgr)
public:
	enum PARTICLETYPE { TYPE_Straight, TYPE_CURVE, TYPE_END };
private:
	CDeadParticle_Mgr();
	virtual ~CDeadParticle_Mgr() = default;

public:
	CStraight_Particle*				Get_Straight_Particle(CEffect_Particle::OPTION _tOption);
	void							Dead_Straight_Particle();

private:
	list<class CStraight_Particle*> m_Dead_Straight_Particles;
	//list<class CInstance_Particle*> m_Straight_Particles;
	//list<class CCurve_Particle*> m_Straight_Particles;


public:
	// CBase을(를) 통해 상속됨
	virtual void Free() override;
};

END
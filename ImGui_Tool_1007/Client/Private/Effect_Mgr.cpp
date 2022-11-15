#include "stdafx.h"
#include "..\Public\Effect_Mgr.h"


IMPLEMENT_SINGLETON(CEffect_Mgr)

void CEffect_Mgr::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < EFFECT_END; ++i)
	{
		for (auto& _iter = m_pEffects[i].begin(); _iter != m_pEffects[i].end();)
		{
			if (false == (*_iter)->Update(fTimeDelta))
			{
				_iter = m_pEffects[i].erase(_iter);
			}
			else
			{
				++_iter;
			}
		}
	}
}

HRESULT CEffect_Mgr::Add_Effect()
{
	return S_OK;
}

void CEffect_Mgr::Clear_Level(LEVEL _eLv)
{
}

void CEffect_Mgr::Free()
{
}

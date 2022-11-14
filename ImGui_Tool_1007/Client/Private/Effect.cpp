#include "stdafx.h"
#include "..\Public\Effect.h"

CEffect::CEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{
}

void CEffect::Free()
{
	__super::Free();
}

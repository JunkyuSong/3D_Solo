#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instancing final : public CVIBuffer
{
private:
	CVIBuffer_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& rhs);
	virtual ~CVIBuffer_Point_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance, vector<_float4x4>* _WorldMatrix, vector<_float3> _vecDirect);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT	Render() override;

private:
	HRESULT			Ready_VertexBuffer();
	HRESULT			Ready_IndexBuffer(_uint iNumInstance);
	HRESULT			Ready_InstancingBuffer(_uint iNumInstance, vector<_float4x4>* _WorldMatrix, vector<_float3> _vecDirect);

private:
	_uint					m_iNumInstance = 0; // �ν��Ͻ� ����
	_uint					m_iInstanceStride = 0; // �ν��Ͻ� byte���ε�..?
	ID3D11Buffer*			m_pVBInstance = nullptr; // �ν��Ͻ� ����(������� �� ����)
	vector<_float4x4>		m_vecWorldMatrix;
	vector<_float3>			m_vecDirect;

public:
	static CVIBuffer_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance, vector<_float4x4>* _WorldMatrix, vector<_float3> _vecDirect);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END
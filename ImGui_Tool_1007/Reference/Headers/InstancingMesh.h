#pragma once


#include "MeshContainer.h"
#include "Engine_Binary.h"

/* ���� �����ϴ� �ϳ��� �޽�. */
/* �� �޽ø� �����ϴ� ����, �ε��� ���۸� �����Ѵ�. */

BEGIN(Engine)

class CInstancingMesh : public CMeshContainer
{
public:
	CInstancingMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstancingMesh(const CInstancingMesh& rhs);
	virtual ~CInstancingMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, TCONTAINER*	_pOut, _uint iNumInstance, vector<_float4x4>* _matWorld);
	virtual HRESULT Initialize_Prototype(TCONTAINER	tIn, _uint iNumInstance, vector<_float4x4>* _matWorld);
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Render();

	HRESULT			Ready_IndexBuffer(const aiMesh* pAIMesh, TCONTAINER* _pOut, _uint iNumInstance);
	HRESULT			Ready_IndexBuffer(TCONTAINER _tIn, _uint iNumInstance);
	HRESULT			Ready_InstancingBuffer(_uint iNumInstance, vector<_float4x4>* _WorldMatrix);

	_bool			Picking(class CTransform* pTransform, _vector& pOut);

	void			Culling(_float fSize);

private:
	_uint					m_iNumInstance = 0;
	_uint					m_iInstanceStride = 0;
	ID3D11Buffer*			m_pVBInstance = nullptr;
	vector<_float4x4>		m_vecWorldMatrix;
	_float4x4*				m_vecCullWorldMatrix = nullptr;

public:
	static CInstancingMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, TCONTAINER*	_pOut, _uint iNumInstance, vector<_float4x4>* matWorld);
	static CInstancingMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TCONTAINER	_tIn, _uint iNumInstance, vector<_float4x4>* matWorld);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
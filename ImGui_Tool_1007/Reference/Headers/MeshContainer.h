#pragma once


#include "VIBuffer.h"
#include "Engine_Binary.h"

/* ���� �����ϴ� �ϳ��� �޽�. */
/* �� �޽ø� �����ϴ� ����, �ε��� ���۸� �����Ѵ�. */

BEGIN(Engine)

class CMeshContainer : public CVIBuffer
{
public:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, TCONTAINER*	_pOut);
	virtual HRESULT Initialize_Prototype(TCONTAINER	tIn);
	virtual HRESULT Initialize(void* pArg);

	HRESULT			Ready_IndexBuffer(const aiMesh* pAIMesh, TCONTAINER* _pOut);
	HRESULT			Ready_IndexBuffer(const aiMesh* pAIMesh, TANIMCONTAINER* _pOut);
	HRESULT			Ready_IndexBuffer(TANIMCONTAINER _tIn);
	HRESULT			Ready_IndexBuffer(TCONTAINER _tIn);

	_bool			Picking(class CTransform* pTransform, _vector& pOut);
	
protected:
	_uint				m_iMaterialIndex = 0;
	char				m_szName[MAX_PATH]; //���� �޽��� �̸�
	TCONTAINER			m_in;

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, TCONTAINER*	_pOut);
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TCONTAINER	_tIn);

	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
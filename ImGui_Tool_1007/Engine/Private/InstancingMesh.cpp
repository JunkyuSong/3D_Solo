#include "InstancingMesh.h"
#include "Transform.h"
#include "Picking.h"
#include "PipeLine.h"
#include "Frustum.h"

CInstancingMesh::CInstancingMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMeshContainer(pDevice, pContext)
{
}

CInstancingMesh::CInstancingMesh(const CInstancingMesh & rhs)
	: CMeshContainer(rhs)
{
}

HRESULT CInstancingMesh::Initialize_Prototype(const aiMesh * pAIMesh, TCONTAINER* _pOut, _uint iNumInstance, vector<_float4x4>* _matWorld)
{
	m_iNumInstance = iNumInstance;
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	//�ش� �޽��� �ε���
	_pOut->iIndex = m_iMaterialIndex;
#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 2;
	m_iNumVertices = pAIMesh->mNumVertices;
	_pOut->NumVertices = m_iNumVertices;

	_pOut->pVertices = new VTXMODEL[m_iNumVertices];

	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL*		pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexture, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&(_pOut->pVertices[i]), &(pVertices[i]), sizeof(VTXMODEL));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);



#pragma endregion

#pragma region INDEXBUFFER
	Ready_IndexBuffer(pAIMesh, _pOut, iNumInstance);

	if (FAILED(Ready_InstancingBuffer(iNumInstance, _matWorld)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CInstancingMesh::Initialize_Prototype(TCONTAINER tIn, _uint iNumInstance, vector<_float4x4>* _matWorld)
{
	m_in = tIn;
	m_iMaterialIndex = tIn.iIndex;
#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 2;
	m_iNumVertices = tIn.NumVertices;

	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL*		pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&(pVertices[i]), &(tIn.pVertices[i]), sizeof(VTXMODEL));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	Ready_IndexBuffer(tIn, iNumInstance);
	if (FAILED(Ready_InstancingBuffer(iNumInstance, _matWorld)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CInstancingMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CInstancingMesh::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;
	//m_pVBInstance �̰� �ʾ���ؼ� ������� ��... ���⼭ ���ָ� �ȵȴ�-> �ν��Ͻ� ������ ��ü���� �ø��ؾ���.
	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint			iStrides[] = {
		m_iStride,
		m_iInstanceStride
	};

	_uint			iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);
	//m_iNumInstance �ø������� �ٲ�� ������
	m_pContext->DrawIndexedInstanced(m_iNumPrimitives * m_iNumIndicesofPrimitive, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CInstancingMesh::Ready_IndexBuffer(const aiMesh* pAIMesh, TCONTAINER* _pOut, _uint iNumInstance) // Ŭ�󿡼� �ʿ���� �κ�.
{
	m_iNumPrimitives = pAIMesh->mNumFaces;
	_pOut->NumFaces = m_iNumPrimitives;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	_pOut->pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);
	ZeroMemory(_pOut->pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
		memcpy(&(_pOut->pIndices[i]), &(pIndices[i]), sizeof(FACEINDICES32));
	}

	m_in = *_pOut;
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	

	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CInstancingMesh::Ready_IndexBuffer(TCONTAINER _tIn, _uint iNumInstance)
{
	//����ü�ø� ���ؼ� ���� ���̳������� ���߿� ��ĥ ��.
	m_iNumPrimitives = _tIn.NumFaces;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives * iNumInstance];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives * iNumInstance);

	for (_uint j = 0; j < iNumInstance; ++j)
	{
		for (_uint i = 0; i < m_iNumPrimitives; ++i)
		{
			memcpy(&(pIndices[i]), &(_tIn.pIndices[i]), sizeof(FACEINDICES32));
		}

	}

	

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CInstancingMesh::Ready_InstancingBuffer(_uint iNumInstance, vector<_float4x4>* _WorldMatrix)
{
	if (iNumInstance > 200)
	{
		return E_FAIL;
	}
	m_iInstanceStride = sizeof(VTXINSTANCE);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	VTXINSTANCE*		pInstanceVtx = new VTXINSTANCE[200];
	ZeroMemory(pInstanceVtx, sizeof(VTXINSTANCE) * 200);
	m_vecCullWorldMatrix = new _float4x4[m_iNumInstance];
	for (_uint i = 0; i < iNumInstance; ++i)
	{
		memcpy(&(pInstanceVtx[i]), &((*_WorldMatrix)[i]), sizeof(_float4x4));
		m_vecWorldMatrix.push_back((*_WorldMatrix)[i]);
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pInstanceVtx;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVtx);


	return S_OK;
}

_bool CInstancingMesh::Picking(CTransform * pTransform, _vector & pOut)
{
	CPicking*		pPicking = CPicking::Get_Instance();
	_bool _bPicking = false;
	Safe_AddRef(pPicking);

	_vector			vRayDir, vRayPos;

	pPicking->Compute_LocalRayInfo(vRayDir, vRayPos, pTransform);
	vRayDir = XMVector3Normalize(vRayDir);

	_vector		_pVerticesPos_0;
	_vector		_pVerticesPos_1;
	_vector		_pVerticesPos_2;
	_float		fDist;
	_matrix		WorldMatrix = pTransform->Get_WorldMatrix();


	_uint		iIndices[3] = { NULL };
	_float		_fFinalDistToCam = 0.f;
	_bool		_bOne = false;
	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{
		iIndices[0] = m_in.pIndices[i]._0;
		iIndices[1] = m_in.pIndices[i]._1;
		iIndices[2] = m_in.pIndices[i]._2;
		
		_pVerticesPos_0 = XMLoadFloat3(&(m_in.pVertices[iIndices[0]].vPosition));
		_pVerticesPos_1 = XMLoadFloat3(&(m_in.pVertices[iIndices[1]].vPosition));
		_pVerticesPos_2 = XMLoadFloat3(&(m_in.pVertices[iIndices[2]].vPosition));

		if (true == TriangleTests::Intersects(vRayPos, vRayDir, _pVerticesPos_0, _pVerticesPos_1, _pVerticesPos_2, fDist))
		{
			_vector	vPickPos = vRayPos + (vRayDir * fDist);

			if (_bOne == false)
			{
				pOut = XMVector3TransformCoord(vPickPos, WorldMatrix);
				_fFinalDistToCam = XMVectorGetX(XMVector3Length(pOut - XMLoadFloat4(&(CPipeLine::Get_Instance()->Get_CamPosition()))));
				
				_bOne = true;
			}

			vPickPos = XMVector3TransformCoord(vPickPos, WorldMatrix);
			_float _fCurDistToCam = XMVectorGetX(XMVector3Length(vPickPos - XMLoadFloat4(&(CPipeLine::Get_Instance()->Get_CamPosition()))));

			if (_fFinalDistToCam > _fCurDistToCam)
			{
				pOut = vPickPos;
				_fFinalDistToCam = _fCurDistToCam;
			}
			
			_bPicking = true;
		}
	}

	Safe_Release(pPicking);

	return _bPicking;
}

void CInstancingMesh::Culling(_float fSize)
{
	
	CFrustum* _pFrustum = CFrustum::Get_Instance();
	m_iNumInstance = 0;
	for (auto& _world : m_vecWorldMatrix)
	{

		_vector _vPos;
		memcpy(&_vPos, _world.m[3], sizeof(_float4));
		_vPos.m128_f32[3] = 1.f;
		if (_pFrustum->isIn_WorldSpace(_vPos, fSize))
		{
			m_vecCullWorldMatrix[m_iNumInstance] = _world;
			++m_iNumInstance;			
		}
	}
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, m_vecCullWorldMatrix, sizeof(_float4x4) * m_iNumInstance);
	m_pContext->Unmap(m_pVBInstance, 0);
}

CInstancingMesh * CInstancingMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const aiMesh * pAIMesh, TCONTAINER*	_pOut, _uint iNumInstance, vector<_float4x4>* matWorld)
{
	CInstancingMesh*			pInstance = new CInstancingMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pAIMesh, _pOut, iNumInstance, matWorld)))
	{
		MSG_BOX(TEXT("Failed To Created : CInstancingMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CInstancingMesh * CInstancingMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TCONTAINER _tIn, _uint iNumInstance, vector<_float4x4>* matWorld)
{
	CInstancingMesh*			pInstance = new CInstancingMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(_tIn, iNumInstance, matWorld)))
	{
		MSG_BOX(TEXT("Failed To Created : CInstancingMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CInstancingMesh::Clone(void * pArg)
{
	CInstancingMesh*			pInstance = new CInstancingMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CInstancingMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstancingMesh::Free()
{
	__super::Free();
	if (!m_isCloned)
	{
		Safe_Release(m_pVBInstance);
		Safe_Delete_Array(m_vecCullWorldMatrix);
		m_vecWorldMatrix.clear();
	}

}

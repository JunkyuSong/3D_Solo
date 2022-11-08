#include "..\Public\Model.h"
#include "MeshContainer.h"

#include "Texture.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_datLoad(rhs.m_datLoad)
{
	strcpy_s(m_pModelFilePath, rhs.m_pModelFilePath);
	strcpy_s(m_pModelFileName, rhs.m_pModelFileName);

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}
}


HRESULT CModel::SetUp_OnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eType, const char * pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (m_Materials[iMaterialIndex].pTexture[eType] == nullptr)
		return S_OK;

	return m_Materials[iMaterialIndex].pTexture[eType]->Set_SRV(pShader, pConstantName);	
}

HRESULT CModel::Ready_Materials(TMATERIALS _tIn)
{
	m_iNumMaterials = _tIn.NumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		_MaterialDesc;
		ZeroMemory(&_MaterialDesc, sizeof(MATERIALDESC));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			if (!lstrcmp(_tIn.tMaterial[i].szPath[j], TEXT("")))
				continue;
			_MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, _tIn.tMaterial[i].szPath[j]);
			//�ؽ��� �����ؼ� ���׸��� �迭�� �ִ´�

			if (nullptr == _MaterialDesc.pTexture[j])
				return E_FAIL;
		}
		m_Materials.push_back(_MaterialDesc);
	}
	return S_OK;
}

HRESULT CModel::Ready_Materials(const char * pModelFilePath, TMATERIALS* _pOut)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;
	
	m_iNumMaterials = m_pAIScene->mNumMaterials;
	_pOut->NumMaterials = m_iNumMaterials;
	_pOut->tMaterial = new TMATERIAL[m_iNumMaterials];
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		_MaterialDesc;
		ZeroMemory(&_MaterialDesc, sizeof(MATERIALDESC));

		aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];
		//�� ��ü���� ���׸��� ��������
		//�װ� ���� ���׸����� �迭�� ä���ִ´�

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		strPath;
			//aiTextureType : �̳Ѱ�
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				//strPath�� �ش� fbx�� ����ִ� �ؽ����� �ּ�(������ ��ǻ�Ϳ� �ִ� �ּҷ� ��)�� ��´�
			{
				lstrcpy(_pOut->tMaterial[i].szPath[j], TEXT(""));
				continue;
			}
				
			//������ ��������

			char szFullPath[MAX_PATH] = ""; //���⿡ ���� ����
			char szFileName[MAX_PATH] = ""; //���� �̸�
			char szExt[MAX_PATH] = ""; //Ȯ����

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
			//strPath�� ���� ��� ���� ���� �̸��̶� Ȯ���� ����
			// ex) abc / .png

			strcpy_s(szFullPath, pModelFilePath);
			//���� ��� �ְ�
			strcat_s(szFullPath, szFileName);
			//���� �̸� �ְ�
			strcat_s(szFullPath, szExt);
			//��� �ְ�

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");
			
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);
			//_tchar�� ��ȯ _ szFullPath��ŭ�� �������� szFullPath�� szWideFullPath�� ��´�
			
			lstrcpy(_pOut->tMaterial[i].szPath[j], szWideFullPath);

			_MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath);
			//�ؽ��� �����ؼ� ���׸��� �迭�� �ִ´�

			if (nullptr == _MaterialDesc.pTexture[j])
				return E_FAIL;
			//����°� �����ϸ� ����
		}

		m_Materials.push_back(_MaterialDesc);
	}
	return S_OK;
}

void CModel::Free()
{
	__super::Free();

	for (auto& _Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(_Material.pTexture[i]);
	}
	m_Materials.clear();



	m_Importer.FreeScene();
}

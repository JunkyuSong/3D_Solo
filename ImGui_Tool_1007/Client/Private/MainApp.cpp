#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "ImGuiMgr.h"
#include "TerrainMgr.h"
#include "ReleaseMgr.h"
#include "StageMgr.h"
#include "CameraMgr.h"
#include "CollisionMgr.h"
#include "UI_Mgr.h"
#include "Status.h"
#include "VIBuffer_Point.h"
#include "Effect_Mgr.h"
#include "DeadParticle_Mgr.h"

using namespace Client;

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	//D3D11_SAMPLER_DESC
	// D3D11_SAMPLER_DESC
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	/*
	D3D11_RASTERIZER_DESC
	D3D11_BLEND_DESC
	D3D11_DEPTH_STENCIL_DESC

	*/


	GRAPHICDESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = GRAPHICDESC::MODE_WIN;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;
	
	m_pGameInstance->Loading_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice,m_pContext, LEVEL_END));

	if (FAILED(m_pGameInstance->Reserve_Level(LEVEL_LOGO)))
		return E_FAIL;
	CImGui::Get_Instance()->Initailize(m_pDevice, m_pContext);

	m_pGameInstance->Add_Layer(LEVEL_STATIC,TEXT("Layer_UI"));

	//Ready_Gara();
	return S_OK;
}

void CMainApp::Tick(const _float& fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return ;

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta / m_pGameInstance->Get_TimeSpeed(TEXT("Timer_Main"));
	++m_iNumDraw;
	
	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("fps : %d"), m_iNumDraw);
		m_iFps = m_iNumDraw;
		m_iNumDraw = 0;
		m_fTimeAcc = 0.f;
	}
	
#endif // _DEBUG
	CImGui::Get_Instance()->Tick();
	CImGui::Get_Instance()->Begin("FPS : ");
	CImGui::Get_Instance()->Intcheck(&m_iFps, "FPS");
	CImGui::Get_Instance()->End();
	m_pGameInstance->Tick_Engine(fTimeDelta);
	CEffect_Mgr::Get_Instance()->Tick(fTimeDelta);
	CCameraMgr::Get_Instance()->Tick(fTimeDelta);
	CPipeLine::Get_Instance()->Update();
	
}

HRESULT CMainApp::Render()
{

	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
	
	m_pRenderer->Draw();
	m_pGameInstance->Render_Level();
	CImGui::Get_Instance()->Render();

	m_pGameInstance->Present();

#ifdef _DEBUG
	/*++m_iNumDraw;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("fps : %d"), m_iNumDraw);
		m_iNumDraw = 0;
		m_fTimeAcc = 0.f;
	}*/

	//SetWindowText(g_hWnd, m_szFPS);
	
#endif // _DEBUG

	
	CCollisionMgr::Get_Instance()->Clear_CollisoinList();
	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* Ư�� ������ �Ҵ��ϱ� ���� �ε������� ���� �Ҵ�Ǵ� ���·� ����. */

	/* �Ҵ��� ������ �����Ŵ����� ������ �� �ֶǷ�. gksek. */
	//if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/NavigationData.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3];

	/**/
	ZeroMemory(vPoints, sizeof(_float3) * 3);
	vPoints[0] = _float3(0.0f, 0.f, 5.f);
	vPoints[1] = _float3(5.0f, 0.f, 0.f);
	vPoints[2] = _float3(0.0f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoints, sizeof(_float3) * 3);
	vPoints[0] = _float3(0.0f, 0.f, 5.f);
	vPoints[1] = _float3(5.0f, 0.f, 5.f);
	vPoints[2] = _float3(5.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoints, sizeof(_float3) * 3);
	vPoints[0] = _float3(0.0f, 0.f, 10.f);
	vPoints[1] = _float3(5.0f, 0.f, 5.f);
	vPoints[2] = _float3(0.0f, 0.f, 5.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoints, sizeof(_float3) * 3);
	vPoints[0] = _float3(5.f, 0.f, 5.f);
	vPoints[1] = _float3(10.0f, 0.f, 0.f);
	vPoints[2] = _float3(5.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
	//	return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;
	
	Safe_AddRef(m_pRenderer);


	/* For.Prototype_Component_Status */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Status"),
		CStatus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{

	return S_OK;
}

CMainApp * CMainApp::Create()
{	
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CMainApp"));
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	CDeadParticle_Mgr::Destroy_Instance();
	CEffect_Mgr::Destroy_Instance();
	CReleaseMgr::Destroy_Instance();
	CStageMgr::Destroy_Instance();
	CTerrainMgr::Destroy_Instance();
	CCameraMgr::Destroy_Instance();
	CCollisionMgr::Destroy_Instance();
	CUI_Mgr::Destroy_Instance();
	CGameInstance::Release_Engine();
	CImGui::Destroy_Instance();
	


}

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../Default/stdafx.h"
#include "..\Public\ImGuiMgr.h"
#include "GameInstance.h"
#include "Obj_Tool.h"
#include "Navigation_Tool.h"

IMPLEMENT_SINGLETON(CImGui)

CImGui::CImGui()
	: m_eCurToolType(TOOL_END)
	, m_bTool_UI_Pick(false)
	, m_bTool_UI_Add(false)
	, m_pLayer(nullptr)
	, m_pPick(nullptr)
	, m_iPickIndex(NULL)
	, m_bTexture(false)
	, m_iSelectTex(0)
	, m_vPos(0.f,0.f,0.f)
	, m_vScale(100.f, 100.f, 1.f)
	, show_demo_window(false)
	, m_bToolOn(false)
{
	ZeroMemory(m_szTemp, 256);
	ZeroMemory(m_szSour, 256);
}

CImGui::~CImGui()
{
	for (auto& iter : m_pTextures)
	{
		Safe_Delete_Array(iter);
	}
	m_pTextures.clear();
}

HRESULT CImGui::Initailize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);


	
	return S_OK;
}

void CImGui::Free()
{
	// Cleanup

	CObj_Tool::Destroy_Instance();
	CNavigation_Tool::Destroy_Instance();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


_uint CImGui::Tick()
{	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (CGameInstance::Get_Instance()->KeyDown(DIK_F11))
	{
		show_demo_window = !show_demo_window;
		m_bToolOn = !m_bToolOn;
	}
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	if (!m_bToolOn)
	{
		return TRUE;
	}


	
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	
	//Help

	
	

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImGui::Begin("Tool");
	// Create a window called "Hello, world!" and append into it.
	
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Tool", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("UI_Tool"))
		{
			m_eCurToolType = TOOL_UI;
			Tool_UI();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Obj_Tool"))
		{
			m_eCurToolType = TOOL_OBJ;
			//Tool_UI();
			CObj_Tool::Get_Instance()->Tool_Obj();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Navi_Tool"))
		{
			m_eCurToolType = TOOL_NAVI;
			//Tool_UI();
			CNavigation_Tool::Get_Instance()->Tick();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	
	
	ImGui::Separator();

	ImGui::End();


	//ImGui::Text("Just Test.");               // Display some text (you can use a format strings too)
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_another_window);

	//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//	counter++;
	//ImGui::SameLine();
	//ImGui::Text("counter = %d", counter);

	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


	return TRUE;
}

HRESULT CImGui::Render()
{

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	return S_OK;
}
 
void CImGui::ClientTick()
{
	ImGui::Begin("Client");

	ImGui::End();
}

_tchar * CImGui::ConvertCtoWC(const char * str)
{
	_tchar* pStr;

	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

	pStr = new _tchar[strSize];

	MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str) + 1, pStr, strSize);

	return pStr;
}

char* CImGui::ConvertWCtoC(const _tchar * _tchar)
{
	// ��ȯ�� char* ���� ����
	char* _char;

	// �Է¹��� _tchar ������ ���̸� ����
	int _tcharSize = WideCharToMultiByte(CP_ACP, 0, _tchar, -1, NULL, 0, NULL, NULL);

	// char8 �޸� �Ҵ�
	_char = new char[_tcharSize];

	// ����ȯ
	WideCharToMultiByte(CP_ACP, 0, _tchar, -1, _char, _tcharSize, 0, 0);

	return _char;
}

void CImGui::Begin(const char * Label)
{
	ImGui::Begin(Label);
}

void CImGui::Intcheck(_int* _iInt,const char* Label)
{
	ImGui::InputInt(Label, _iInt);
}
void CImGui::floatcheck(_float* _fFloat, const char* Label)
{
	ImGui::InputFloat(Label, _fFloat);
}

_bool CImGui::Button(const char * Label)
{
	return ImGui::Button(Label);
}

void CImGui::End()
{
	ImGui::End();
}

HRESULT CImGui::Tool_UI()
{
	if (ImGui::Button("Add UI"))
	{
		if (m_pPick != nullptr)
		{
			static_cast<CUI_Plus*>(m_pPick)->Set_Pass(CUI_Plus::PASS_NONPICK);
		}
		m_pPick = nullptr;
		m_bTool_UI_Add = true;
		m_bTool_UI_Pick = false;
	}
	ImGui::SameLine();
	
	if (ImGui::Button("Load UI_Layer"))
	{
		//������ ���̾� �޾ƿ���
		m_pLayer = CGameInstance::Get_Instance()->Get_Layer(LEVEL_STATIC,TEXT("Layer_UI"));
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Texture"))
	{
		//������ ���̾� �޾ƿ���
		Load_Texture();
	}
	if (ImGui::Button("Save UI.dat"))
	{

	}
	ImGui::SameLine();
	if (ImGui::Button("Load UI.dat"))
	{

	}

	if (m_pLayer != nullptr)
	{
		ImGui::SliderInt("slider int", &m_iPickIndex, 0, (*m_pLayer->Get_ListFromLayer()).size() - 1);
		if (ImGui::Button("Pick"))
		{
			if (m_pPick != nullptr)
			{
				static_cast<CUI_Plus*>(m_pPick)->Set_Pass(CUI_Plus::PASS_NONPICK);
			}
			list<CGameObject*>* _list = m_pLayer->Get_ListFromLayer();
			auto& iter = (*_list).begin();
			for (int i = 0; i < m_iPickIndex; ++iter,++i);
			m_pPick = *iter;
			static_cast<CUI_Plus*>(m_pPick)->Set_Pass(CUI_Plus::PASS_PICK);
			m_pPick_Trans = static_cast<CTransform*>(m_pPick->Get_ComponentPtr(TEXT("Com_Transform")));
			m_bTool_UI_Pick = true;
			m_bTool_UI_Add = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			if (m_pPick != nullptr)
			{
				static_cast<CObj_Plus*>(m_pPick)->Set_Pass(CObj_Plus::PASS_NONPICK);
			}
			list<CGameObject*>* _list = m_pLayer->Get_ListFromLayer();
			auto& iter = (*_list).begin();
			for (int i = 0; i < m_iPickIndex; ++iter, ++i);
			Safe_Release<CGameObject*>(*iter);
			(*_list).erase(iter);
			iter = (*_list).begin();
			m_iPickIndex = 0;
			m_pPick = nullptr;
			//m_pPick_Trans = static_cast<CTransform*>(m_pPick->Get_ComponentPtr(TEXT("Com_Transform")));
		}
	}
	
	if (m_bTool_UI_Pick)
	{
		Tool_UI_Pick();
	}
	if (m_bTool_UI_Add)
	{
		Tool_UI_Add();
	}	
	
	return S_OK;
}

HRESULT CImGui::Tool_UI_Pick()
{	
	
	ImGui::Begin("Tool_UI_Pick", &m_bTool_UI_Pick);

	//�ϳ��� ���� ä��������
	XMStoreFloat3(&m_vPos, m_pPick_Trans->Get_State(CTransform::STATE_POSITION));
	m_vScale = m_pPick_Trans->Get_Scale();

	Key_Input();

	ImGui::InputFloat("Pos.x", &m_vPos.x);
	ImGui::InputFloat("Pos.y", &m_vPos.y);
	ImGui::InputFloat("Pos.z", &m_vPos.z);

	ImGui::InputFloat("Scale.x", &m_vScale.x);
	ImGui::InputFloat("Scale.y", &m_vScale.y);
	ImGui::InputFloat("Scale.z", &m_vScale.z);

	m_pPick_Trans->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.x, m_vPos.y, m_vPos.z, 1.f));
	m_pPick_Trans->Set_Scale(XMVectorSet(m_vScale.x, m_vScale.y, m_vScale.z, 0.f));

	if (m_bTexture)
	{
		if (ImGui::BeginCombo("UI", m_pTextures[m_iSelectTex], 1))
		{
			for (int i = 0; i < m_pTextures.size(); ++i)
			{
				const bool isSelected = (m_iSelectTex == i);
				if (ImGui::Selectable(m_pTextures[i], isSelected))
				{
					m_iSelectTex = i;
				}
			}
			ImGui::EndCombo();
		}
	}

	if (ImGui::Button("Set_Info"))
	{
		m_tUI_Desc.vPos = m_vPos;
		m_tUI_Desc.vScale = m_vScale;
		m_tUI_Desc.szTextureTag = ConvertCtoWC(m_pTextures[m_iSelectTex]);
		static_cast<CUI_Plus*>(m_pPick)->Set_Info(m_tUI_Desc);
	}

	ImGui::SameLine();
	if (ImGui::Button("Close"))
	{
		m_bTool_UI_Pick = false;
	}
	ImGui::End();
	
	return S_OK;
}

HRESULT CImGui::Tool_UI_Add()
{
	ImGui::Begin("Tool_UI_Add", &m_bTool_UI_Add);

	ImGui::InputText("UI_Name", m_szTemp, 256, ImGuiInputTextFlags_CharsNoBlank);
	ImGui::InputFloat("Pos.x", &m_vPos.x);
	ImGui::InputFloat("Pos.y", &m_vPos.y);
	ImGui::InputFloat("Pos.z", &m_vPos.z);

	ImGui::InputFloat("Scale.x", &m_vScale.x);
	ImGui::InputFloat("Scale.y", &m_vScale.y);
	ImGui::InputFloat("Scale.z", &m_vScale.z);
	
	
	if (m_bTexture)
	{
		if (ImGui::BeginCombo("UI", m_pTextures[m_iSelectTex], 1))
		{
			for (int i = 0; i < m_pTextures.size(); ++i)
			{
				const bool isSelected = (m_iSelectTex == i);
				if (ImGui::Selectable(m_pTextures[i], isSelected))
				{
					m_iSelectTex = i;
				}
			}
			ImGui::EndCombo();
		}
	}
	

	if (ImGui::Button("Add"))
	{
		m_tUI_Desc.szUIName = ConvertCtoWC(m_szTemp);
		m_tUI_Desc.szTextureTag = ConvertCtoWC(m_pTextures[m_iSelectTex]);
		m_tUI_Desc.vPos = m_vPos;
		m_tUI_Desc.vScale = m_vScale;
		if (FAILED(CGameInstance::Get_Instance()->Add_GameObjectToLayer(TEXT("Prototype_GameObject_UI_Plus"), LEVEL_STATIC, TEXT("Layer_UI"), &m_tUI_Desc)))
			return E_FAIL;
	}

	ImGui::SameLine();
	if (ImGui::Button("Close"))
	{
		m_bTool_UI_Add = false;
	}
	ImGui::End();

	return S_OK;
}

HRESULT CImGui::Save_Tag(const _uint _iLevel, const _tchar * _pLayerTag)
{
	//CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

	//Safe_AddRef<CGameInstance*>(pGameInstance);
	//_tchar*		_szFileName = nullptr;



	//// 2. ���� ����
	////�������� ����Ʈ�� ���´�


	//for (int i = 0; i < LEVEL_END; ++i)
	//{
	//	switch (i)
	//	{
	//	case LEVEL_GAMEPLAY:
	//		//continue;
	//		_szFileName = TEXT("../Bin/Data/GamePlay.dat");
	//		break;
	//	default:
	//		continue;
	//	}

	//	HANDLE		hFile = CreateFile(_szFileName,			// ���� ��ο� �̸� ���
	//		GENERIC_WRITE,				// ���� ���� ��� (GENERIC_WRITE ���� ����, GENERIC_READ �б� ����)
	//		NULL,						// �������, ������ �����ִ� ���¿��� �ٸ� ���μ����� ������ �� ����� ���ΰ�, NULL�� ��� �������� �ʴ´�
	//		NULL,						// ���� �Ӽ�, �⺻��	
	//		CREATE_ALWAYS,				// ���� ���, CREATE_ALWAYS�� ������ ���ٸ� ����, �ִٸ� ���� ����, OPEN_EXISTING ������ ���� ��쿡�� ����
	//		FILE_ATTRIBUTE_NORMAL,		// ���� �Ӽ�(�б� ����, ���� ��), FILE_ATTRIBUTE_NORMAL �ƹ��� �Ӽ��� ���� �Ϲ� ���� ����
	//		NULL);						// �������� ������ �Ӽ��� ������ ���ø� ����, �츮�� ������� �ʾƼ� NULL

	//	list<CGameObject*> _ObjList = *(m_pLayer->Get_ListFromLayer());
	//	
	//	CUI_Plus::UI_DESC _UIDESC = 

	//	CGameObj::GAMEOBJ_INFO _GameInfo;
	//	DWORD		dwByte = 0;


	//	for (CGameObj* _Obj : _ObjList)
	//	{
	//		_tchar _sztchar[128] = { ' ' };
	//		memset(&_sztchar, NULL, sizeof(_tchar) * 128);
	//		const _tchar* _ProtoKey = nullptr;

	//		_GameInfo = _Obj->Get_Info();

	//		_ProtoKey = _Obj->Get_PrototypeKey();

	//		for (int j = 0; _ProtoKey[j] != NULL; ++j)
	//		{
	//			_sztchar[j] = _ProtoKey[j];
	//		}
	//		if (_GameInfo.iStageNum > 10)
	//		{
	//			int uiuiuiui = 91;
	//		}
	//		_GameInfo.vPos =
	//			static_cast<CTransform*>(_Obj->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION)
	//			- (CStageMgr::Get_Instance()->Get_StageInfo(i, _GameInfo.iStageNum));

	//		WriteFile(hFile, &(_sztchar), 128, &dwByte, nullptr); //������Ű��
	//		WriteFile(hFile, &(_GameInfo.iLevel), sizeof(_int), &dwByte, nullptr);
	//		WriteFile(hFile, &(_GameInfo.iStageNum), sizeof(_int), &dwByte, nullptr);
	//		_float3 _vScale = static_cast<CTransform*>(_Obj->Find_Component(TEXT("Com_Transform")))->Get_Scaled();
	//		WriteFile(hFile, &(_GameInfo.fReverse), sizeof(_float), &dwByte, nullptr);
	//		WriteFile(hFile, &(_vScale.x), sizeof(_float), &dwByte, nullptr);
	//		WriteFile(hFile, &(_vScale.y), sizeof(_float), &dwByte, nullptr);
	//		WriteFile(hFile, &(_vScale.z), sizeof(_float), &dwByte, nullptr);

	//		WriteFile(hFile, &(_GameInfo.vPos), sizeof(_float3), &dwByte, nullptr);
	//		WriteFile(hFile, &(_GameInfo.iTextureNumber), sizeof(_int), &dwByte, nullptr);
	//	}

	//	CloseHandle(hFile);
	//}

	//// 3. ���� �Ҹ�


	//Safe_Release<CGameInstance*>(pGameInstance);
	return S_OK;
}

HRESULT CImGui::Load_Tag(const _uint _iLevel, const _tchar * _pLayerTag)
{
	//LEVEL _eLv = LEVEL_TUTORIAL;

	//_tchar* _szLevel = TEXT("../Bin/Data/Tutorial_Taro.dat");

	//CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	//HANDLE		hFile = CreateFile(_szLevel,
	//	GENERIC_READ,
	//	NULL,
	//	NULL,
	//	OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL,
	//	NULL);


	//// 2. ���� ����

	//DWORD		dwByte = 0;

	//CGameObj::GAMEOBJ_INFO _tInfo;
	//_uint _iNum = 0;
	//while (true)
	//{
	//	_tchar _szTemp[128] = {};
	//	memset(&_szTemp, NULL, sizeof(_tchar) * 128);

	//	ReadFile(hFile, &_szTemp, 128, &dwByte, nullptr);

	//	_tchar* _szProto;
	//	//_int i = 0;
	//	//for (i = 0; _szTemp[i] != NULL; ++i)
	//	//{
	//	//	_szProto[i] = _szTemp[i];
	//	//}
	//	//_szProto[i + 1] = NULL;
	//	_szProto = _szTemp;
	//	ReadFile(hFile, &(_tInfo.iLevel), sizeof(_int), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.iStageNum), sizeof(_int), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.fReverse), sizeof(_float), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.fScaleX), sizeof(_float), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.fScaleY), sizeof(_float), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.fScaleZ), sizeof(_float), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.vPos), sizeof(_float3), &dwByte, nullptr);
	//	ReadFile(hFile, &(_tInfo.iTextureNumber), sizeof(_int), &dwByte, nullptr);



	//	if (_tInfo.iStageNum == 0)
	//	{
	//		_tInfo.vPos.x += 1.5f;
	//		_tInfo.vPos.z += 1.35f;
	//	}

	//	_tInfo.iStageNum = 4;

	//	if (0 == dwByte)	// ���̻� ���� �����Ͱ� ���� ���
	//	{
	//		break;
	//	}

	//	_tInfo.vWorldPos = _tInfo.vPos + CStageMgr::Get_Instance()->Get_StageInfo(_eLv, _tInfo.iStageNum);



	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(_szProto, _eLv, TEXT("Layer_MapObj"), &_tInfo)))
	//	{
	//		MSG_BOX(TEXT("Load Error"));
	//		return E_FAIL;
	//	}


	//}

	//// 3. ���� �Ҹ�
	//CloseHandle(hFile);
	return S_OK;
}

HRESULT CImGui::Load_Texture()
{
	for (auto& iter : m_pTextures)
	{
		Safe_Delete_Array(iter);
	}

	CGameInstance* _pGameInstance = GET_INSTANCE(CGameInstance);
	map<const _tchar*, class CComponent*> _mapTemp = _pGameInstance->Get_Map(LEVEL_STATIC);

	auto& iter = _mapTemp.begin();
	_uint _iTemp = 0;
	char* _szTemp = nullptr;

	for (_uint i = 0; i < _mapTemp.size(); ++iter,++i)
	{
		_szTemp = ConvertWCtoC(iter->first);
		if (strstr(_szTemp, "Texture_UI"))
		{
			m_pTextures.push_back(_szTemp);
		}
		else
		{
			Safe_Delete_Array(_szTemp);
		}
	}

	m_bTexture = true;
	Safe_Release(_pGameInstance);
	return S_OK;
}

_bool CImGui::OnMouse()
{
	//1. Ŭ���� �� �浹ó��? ���� ���� �����ѰŸ� Ŭ���Ͽ� �̵�
	//2. ���콺���� �ϴ°�? �̵�, ũ�� ����

	return _bool();
}

HRESULT CImGui::Key_Input()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	ImGui::InputFloat("PressingSpeed", &_PressingSpeed);

	_long	_lMouseMove = 0;
	if (_lMouseMove = pGameInstance->Get_DIMMoveState(DIMM_WHEEL))
	{
		_float MouseMove = (_float)_lMouseMove / (abs(_lMouseMove)*10.f);
/*		if (pGameInstance->KeyPressing(DIK_LCONTROL))
		{
			if (pGameInstance->KeyPressing(DIK_Z))
			{
				m_vAngle.z += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_X))
			{
				m_vAngle.x += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_Y))
			{
				m_vAngle.y += _PressingSpeed * (_float)MouseMove;
			}
		}
		else */if (pGameInstance->KeyPressing(DIK_LSHIFT))
		{
			if (pGameInstance->KeyPressing(DIK_Z))
			{
				m_vScale.z += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_X))
			{
				m_vScale.x += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_Y))
			{
				m_vScale.y += _PressingSpeed * (_float)MouseMove;
			}
		}
		else
		{
			if (pGameInstance->KeyPressing(DIK_Z))
			{
				m_vPos.z += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_X))
			{
				m_vPos.x += _PressingSpeed * (_float)MouseMove;
			}
			else if (pGameInstance->KeyPressing(DIK_Y))
			{
				m_vPos.y += _PressingSpeed * (_float)MouseMove;
			}
		}
	}

	Safe_Release<CGameInstance*>(pGameInstance);
	return S_OK;
}

#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Player.h"
#include "Camera_CutScene.h"
#include "Camera_CutScene_Enter.h"
#include "Saber.h"
#include "Dagger.h"
#include "BackGround.h"
#include "Terrain.h"
//#include "Monster.h"
//#include "Player.h"
//#include "Effect.h"
//#include "Sky.h"
#include "UI_Plus.h"
#include "Obj_Anim.h"
#include "Obj_NonAnim.h"
#include "InstancingObj.h"
#include "Player.h"
#include "Trail.h"
#include "Magician.h"
#include "Cane.h"
#include "Cane_Sword.h"
#include "Axe.h"
#include "Knife.h"
#include "UI_PlayerHpBar.h"
#include "UI_MonsterHpBar.h"
#include "UI_Targeting.h"
#include "Card.h"
#include "Navigation.h"
#include "Stage_01.h"
#include "Stage_02.h"
#include "Stage_02_1.h"
#include "Stage_Lobby.h"
#include "Stage_Last.h"
#include "Sky.h"
#include "Boss_Bat.h"
#include "MonsterAxe.h"
#include "Extra01.h"
#include "Extra02.h"
#include "Extra01_Last.h"
#include "Extra02_Last.h"
#include "Stage_Test.h"
#include "Puppet.h"
#include "Lamp.h"
#include "StreetLight.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CS());

	pLoader->Loading();

	LeaveCriticalSection(&pLoader->Get_CS());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	switch (Get_NextLevelID())
	{
	case LEVEL_LOGO:
		if (m_bOneLoad[LEVEL_LOGO])
		{
			m_isFinished = true;
		}
		else
		{
			Loading_ForLogoLevel();
		}
		
		break;
	case LEVEL_GAMEPLAY:
		if (m_bOneLoad[LEVEL_LOGO])
		{
			m_isFinished = true;
		}
		else
		{
			Loading_ForGamePlayLevel();

		}
		break;
	case LEVEL_STAGE_02:
		if (m_bOneLoad[LEVEL_LOGO])
		{
			m_isFinished = true;
		}
		else
		{
			Loading_ForLevel_Stage02();

		}
		break;
	case LEVEL_STAGE_02_1:
		if (m_bOneLoad[LEVEL_LOGO])
		{
			m_isFinished = true;
		}
		else
		{
			Loading_ForLevel_Stage02_1();

		}
		break;
	case LEVEL_STAGE_LAST:
		if (m_bOneLoad[LEVEL_LOGO])
		{
			m_isFinished = true;
		}
		else
		{
			Loading_ForLevel_StageLast();

		}
		break;
	case LEVEL_STAGE_LOBBY:
		if (m_bOneLoad[LEVEL_LOGO])
		{
			m_isFinished = true;
		}
		else
		{
			Loading_ForLevel_StageLobby();
		}
		
		break;
	}

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("��ü������ �ε����Դϴ�."));

	AUTOINSTANCE(CGameInstance, pGameInstance);

	/* ������ü���� �ε��Ѵ�. */

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Plus"), CUI_Plus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_CutScene"),
		CCamera_CutScene::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_CutScene_Enter"),
		CCamera_CutScene_Enter::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
	/* �ؽ��ĸ� �ε��Ѵ�. */

	/* For.Prototype_Component_Texture_Default */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Default.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Skill.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Inven"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven.png")))))
		return E_FAIL;

	//���� UI �ؽ���
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Dialog"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/Dialog.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ItemBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/TexUI_ItemBackground.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SquareFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/TexUI_SquareFrame_Hover.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Slot_Gray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlagueWeapon/TexUI_PW_DefaultSlotFrame_02.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Slot_Green"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlagueWeapon/TexUI_PW_Frame_Active.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Slot_Claw"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlagueWeapon/TexUI_PW_PlunderSlotFrame_02.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_MiniSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlagueWeapon/TexUI_PW_DefaultSlotFrame.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Edge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlayerHPBar/TexUI_HPBar_1Border.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Edge2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlayerHPBar/TexUI_HPBar_2BG.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_HpBar_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/PlayerHPBar/TexUI_HPBar_3MainBar.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Monster_HpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/MonsterHPBar/TexUI_HPBar_4BarBorderElite.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Monster_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/MonsterHPBar/TexUI_HPBar_3BarBorder.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Monster_Center"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/MonsterHPBar/TexUI_HPBar_6BarTrack.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MainUI_Monster_Disappear"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GameUI/MonsterHPBar/TexUI_HPBar_NewPreSTBar.png")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("���� �ε����Դϴ�."));
	/* �𵨸� �ε��Ѵ�. */



	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("��ü������ �ε����Դϴ�."));

	/* ��ü���� �ε��Ѵ�. */
	/* For.Prototype_GameObject_Terrain*/
	

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Magician"),
		CMagician::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Cane"),
		CCane::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Cane_Sword"),
		CCane_Sword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Card"),
		CCard::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_01"),
		CStage_01::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	///* For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
	/* �ؽ��ĸ� �ε��Ѵ�. */
	/* For.Prototype_Component_Texture_Terrain */




	lstrcpy(m_szLoadingText, TEXT("���� �ε����Դϴ�."));
	/* �𵨸� �ε��Ѵ�. */

	/* For.Prototype_Component_VIBuffer_Terrain */


	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Magician"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Monster/", "Magician.fbx"))))
	//	return E_FAIL;
	_matrix		PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Player */

	/*PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Test"),
	CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Test/", "RL_Lower.fbx", PivotMatrix))))
	return E_FAIL;*/

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Magician2"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster/", "magician2.dat", PivotMatrix))))
		return E_FAIL;

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Magician"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster/", "magician1_All.dat", PivotMatrix))))
		return E_FAIL;

	/*PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
	CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Player/", "Anim_Etc.fbx", PivotMatrix))))
	return E_FAIL;*/

	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Cane"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster/", "Cane.dat"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Cane_Sword"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster/", "Cane_Sword.dat"))))
		return E_FAIL;



	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Card"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster/", "Card.dat"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stage_01"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_01/", "BossRoom.dat"))))
		return E_FAIL;



	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stage_02"),
	//	CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_02/church/", "church.fbx"))))
	//	return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* For.Prototype_Component_Shader_Terrain */
	

	/* For.Prototype_Component_Shader_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
	//	CShader::Create(m_pGraphic_Device, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl")))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�浹ü�� �ε����Դϴ�. "));



	lstrcpy(m_szLoadingText, TEXT("�׺���̼ǵ����͸� �����ϴ� ���Դϴ�."));

	/* For.Prototype_Component_Navigation */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/NavigationData.dat")))))
	//	return E_FAIL;



	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_GamePlay"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navi_GamePlay.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForLevel_Stage02_1()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("��ü������ �ε����Դϴ�."));

	/* ��ü���� �ε��Ѵ�. */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_02_1"),
		CStage_02_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_MonsterAxe"),
		CMonsterAxe::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Extra01"),
		CExtra01::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Extra02"),
		CExtra02::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
	/* �ؽ��ĸ� �ε��Ѵ�. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Texture_Terrain_02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Fire_tiling_floor.png")))))//Grass_%d.dds
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("���� �ε����Դϴ�."));
	/* ���� �ε��Ѵ�. */

	_matrix		PivotMatrix = XMMatrixIdentity();
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Model_Stage_GreenHouse_floor"),
	//	CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/GreenHouse/", "TEST.fbx"))))
	//	return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Model_Stage_GreenHouse"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/GreenHouse/", "GreenHouse.dat"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Model_Monster_Weapon_Axe"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Item/MonsterWeapon/", "Axe.dat"))))
		return E_FAIL;
	

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Model_Monster_Extra01"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster_List/Extra01/", "Extra01.dat", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Model_Monster_Extra2"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster_List/Extra02/", "Extra2.dat", PivotMatrix))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Model_Monster_TEST01"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster_List/Extra01/", "Test1.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Model_Monster_TEST2"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster_List/Extra02/", "Test2.fbx", PivotMatrix))))
		return E_FAIL;*/

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Model_Monster_anim"),
	//	CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster_List/Extra02/", "anim.fbx", PivotMatrix))))
	//	return E_FAIL;


	/* For.Prototype_Component_Shader_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
	//	CShader::Create(m_pGraphic_Device, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl")))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�浹ü�� �ε����Դϴ�. "));

	lstrcpy(m_szLoadingText, TEXT("�׺���̼ǵ����͸� �����ϴ� ���Դϴ�."));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02_1, TEXT("Prototype_Component_Navigation_Stage_02_1"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Stage_02_1.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;

	return S_OK;
	return S_OK;
}

HRESULT CLoader::Loading_ForLevel_Stage02()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("��ü������ �ε����Դϴ�."));

	/* ��ü���� �ε��Ѵ�. */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_02"),
		CStage_02::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Bat"),
		CBoss_Bat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Extra01_Last"),
		CExtra01_Last::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Extra02_Last"),
		CExtra02_Last::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
	/* �ؽ��ĸ� �ε��Ѵ�. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Texture_Terrain_02"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Fire_tiling_floor.png")))))//Grass_%d.dds
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("���� �ε����Դϴ�."));
	/* �𵨸� �ε��Ѵ�. */

	_matrix		PivotMatrix = XMMatrixIdentity();

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Model_Stage_Cage"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_02/Cage/", "Cage.fbx"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Model_Stage2_Spike01"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_02/Spike/", "Spike01.fbx"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Model_Stage2_Spike03"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_02/Spike/", "Spike03.fbx"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Model_Stage2_Spike05"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_02/Spike/", "Spike05.fbx"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Model_Stage2_Spike06"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_02/Spike/", "Spike06.fbx"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Model_Stage2_Spike07"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_02/Spike/", "Spike07.fbx"))))
		return E_FAIL;

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Model_Monster_bat"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster_List/Bat/", "Bat.dat", PivotMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
	//	CShader::Create(m_pGraphic_Device, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl")))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�浹ü�� �ε����Դϴ�. "));

	lstrcpy(m_szLoadingText, TEXT("�׺���̼ǵ����͸� �����ϴ� ���Դϴ�."));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Navigation_Stage_02"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Stage_02.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForLevel_StageLast()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("��ü������ �ε����Դϴ�."));

	/* ��ü���� �ε��Ѵ�. */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_Last"),
		CStage_Last::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_Test"),
		CStage_Test::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Puppet"),
		CPuppet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_StreetLight"),
		CStreetLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_Lamp"),
		CLamp::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
	/* �ؽ��ĸ� �ε��Ѵ�. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_02, TEXT("Prototype_Component_Texture_Terrain_02"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Fire_tiling_floor.png")))))//Grass_%d.dds
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("���� �ε����Դϴ�."));
	/* �𵨸� �ε��Ѵ�. */

	_matrix		PivotMatrix = XMMatrixIdentity();
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Stage_PuppetPlace"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/PuppetPlace/", "PuppetPlace.fbx"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Stage_Last"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/", "Stage_Last.dat"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_bridge"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Object/", "bridge.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_bridgePiece"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Object/", "bridgePiece.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_bridgePiece02"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Object/", "bridgePiece02.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_bridgePiece03"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Object/", "bridgePiece03.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_bridgePiece04"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Object/", "bridgePiece04.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_fence01"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Object/", "fence01.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_fence02"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Object/", "fence02.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Tower01"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Object/", "Tower01.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Tower02"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Object/", "Tower02.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Tower03"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Object/", "Tower03.fbx"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Lamp"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Light/", "Lamp.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Lantern"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Light/", "Lantern.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Light01"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Light/", "Light01.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Light02"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/Light/", "Light02.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Balloon01"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/balloon/", "balloon01.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Balloon02"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/balloon/", "balloon02.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_BalloonBranch01"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/balloon/", "balloonBranch01.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_balloonDamaged02"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/balloon/", "balloonDamaged02.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_balloonDamaged01"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Last/balloon/", "balloonDamaged01.fbx"))))
		return E_FAIL;

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Model_Monster_Puppet"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Monster_List/puppet/", "puppet.dat", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
	//	CShader::Create(m_pGraphic_Device, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl")))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�浹ü�� �ε����Դϴ�. "));

	lstrcpy(m_szLoadingText, TEXT("�׺���̼ǵ����͸� �����ϴ� ���Դϴ�."));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LAST, TEXT("Prototype_Component_Navigation_Stage_Last"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Stage_Last.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForLevel_StageLobby()
{
	AUTOINSTANCE(CGameInstance, pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("��ü������ �ε����Դϴ�."));

	/* ��ü���� �ε��Ѵ�. */

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Axe"),
		CAxe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_Lobby"),
		CStage_Lobby::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Obj_Anim"),
		CObj_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Obj_NonAnim"),
		CObj_NonAnim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Obj_Instancing"),
		CInstancingObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Saber"),
		CSaber::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Dagger"),
		CDagger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Knife"),
		CKnife::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PlayerHpBar"),
		CUI_PlayerHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHpBar"),
		CUI_MonsterHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Targeting"),
		CUI_Targeting::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));
	/* �ؽ��ĸ� �ε��Ѵ�. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LOBBY, TEXT("Prototype_Component_Texture_Terrain_02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/T_WoodTiles01_BC.dds")))))//Grass_%d.dds
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/T_WoodTiles01_BC.dds")))))//Grass_%d.dds
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("���� �ε����Դϴ�."));
	/* �𵨸� �ε��Ѵ�. */

	_matrix		PivotMatrix = XMMatrixIdentity();


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LOBBY, TEXT("Prototype_Component_Model_Stage_Lobby"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Stage/Stage_Lobby/church/", "church.dat"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Trail */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Trail"),
		CTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Player/", "player_test.dat", PivotMatrix))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LOBBY, TEXT("Prototype_Component_Model_Player_mmm"),
	//	CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Player/", "fdsfsd.fbx", PivotMatrix))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LOBBY, TEXT("Prototype_Component_Model_Test"),
	//	CAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Player/", "amamam.fbx", PivotMatrix))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Saber"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Item/Weapon/Saber/", "Saber.dat"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Dagger"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Item/Weapon/Dagger/", "Dagger.dat"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Axe"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Player/SkillWeapon/", "Axe.dat"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Knife"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Player/SkillWeapon/", "Knife.dat"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sky"),
		CNonAnimModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Sky/", "Sky.fbx"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Trail */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), VTXTRAIL_DECLARATION::Elements, VTXTRAIL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Point */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Point.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_InstancingModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTXMODELINTANCE_DECLARATION::Elements, VTXMODELINTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�浹ü�� �ε����Դϴ�. "));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CAABB::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		COBB::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CSphere::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Capsule */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Capsule"),
		CCapsule::Create(m_pDevice, m_pContext, CCollider::TYPE_CAPSULE))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("�׺���̼ǵ����͸� �����ϴ� ���Դϴ�."));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Ampty"),
		CNavigation::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STAGE_LOBBY, TEXT("Prototype_Component_Navigation_Stage_Lobby"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Stage_Lobby.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);


}

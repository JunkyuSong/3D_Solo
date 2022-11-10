#pragma once

#define CLIENT_RGB(r,g,b) _float4(r / 255.f, g / 255.f, b / 255.f, 1.f)
#define DEFAULTTIME		1.5f
namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY,LEVEL_STAGE_02, LEVEL_STAGE_02_1, LEVEL_STAGE_LAST, LEVEL_STAGE_LOBBY, LEVEL_END };

}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern Client::LEVEL g_eCurLevel;

using namespace Client;

#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	virtual void			Free();

public:
	_char						Get_DIKState(const _uchar&	_eKeyID)
	{
		return m_byCurKeyState[_eKeyID];
	}

	_bool						KeyDown(const _uchar& _eKeyID) // ������ �ȴ����� ���� �������� ���ѹ� ����
	{
		if (!(m_byPreKeyState[_eKeyID] & 0X80) && (m_byCurKeyState[_eKeyID] & 0X80))
		{
			return true;
		}
		return false;
	}

	_bool						KeyPressing(const _uchar& _eKeyID) // ���� ����
	{
		if ((m_byPreKeyState[_eKeyID] & 0X80) && (m_byCurKeyState[_eKeyID] & 0X80))
		{
			return true;
		}
		return false;
	}



	_bool						KeyUp(const _uchar& _eKeyID)
	{
		if ((m_byPreKeyState[_eKeyID] & 0X80) && !(m_byCurKeyState[_eKeyID] & 0X80))
		{
			return true;
		}
		return false;
	}


	_bool						MouseDown(const DIMK&	_eMouseKeyID) // ������ �ȴ����� ���� �������� ���ѹ� ����
	{
		if (!(m_PreMouse.rgbButtons[_eMouseKeyID]) && (m_MouseState.rgbButtons[_eMouseKeyID]))
		{
			{
				return true;
			}
		}
		return false;
	}

	_bool						MousePressing(const DIMK&	_eMouseKeyID) // ���� ����
	{
		if ((m_PreMouse.rgbButtons[_eMouseKeyID]) && (m_MouseState.rgbButtons[_eMouseKeyID]))
		{
			return true;
		}
		return false;
	}

	_bool						MouseUp(const DIMK&	_eMouseKeyID) //������ ������ ������ ����
	{
		if ((m_PreMouse.rgbButtons[_eMouseKeyID]) && !(m_MouseState.rgbButtons[_eMouseKeyID]))
		{
			return true;
		}
		return false;
	}

	_char						Get_DIMKeyState(const DIMK&	_eMouseKeyID)
	{
		return m_MouseState.rgbButtons[_eMouseKeyID];
	}

	_long						Get_DIMMoveState(const DIMM&	_eMouseMoveID)
	{
		return ((_long*)&m_MouseState)[_eMouseMoveID];
	}

public:
	HRESULT						Initialize(HINSTANCE _hInstance, HWND _hWnd);
	void						Update();

private:
	LPDIRECTINPUT8				m_pInputSDK;
	LPDIRECTINPUTDEVICE8		m_pKeyboard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

	_char						m_byPreKeyState[256];
	_char						m_byCurKeyState[256];
	
	DIMOUSESTATE				m_MouseState;
	DIMOUSESTATE				m_PreMouse;
};

END

/*
typedef struct _DIMOUSESTATE {
LONG    lX;
LONG    lY;
LONG    lZ;
BYTE    rgbButtons[4];
} DIMOUSESTATE, *LPDIMOUSESTATE;

enum DIMK { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_X, DIMK_END };
enum DIMM { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };
*/

/*
DIRECT INPUT�� DIK�� ����

DIK_ESCAPE  - ESCŰ
DIK_0 ~ 9       - Ű���� ����� ����Ű 0 ���� 9 ����
DIK_NUMPAD0 ~ 9 - Ű���� �����ʿ� �ִ� ���� Ű�е� 0 ���� 9 ����
DIK_A ~ Z      - A ���� Z ������ ���� Ű
DIK_RETURN  - Enter Ű
DIK_LCONTROL - ���� Ctrl Ű
DIK_RCONTROL - ������ Ctrl Ű
DIK_SPACE -  �����̽� ��
DIK_F1 ~ F12 - Ű���� ��� F1 ���� F12���� ��� Ű
DIK_UP        - �� ȭ��ǥ Ű
DIK_DOWN  - �Ʒ� ȭ��ǥ Ű
DIK_LEFT    - ���� ȭ��ǥ Ű
DIK_RIGHT  - ������ ȭ��ǥ Ű
DIK_TAB     - Tab Ű
DIK_NUMPADENTER -  ���� Ű�е� Enter Ű
DIK_PRIOR - Page Up Ű
DIK_NEXT - Page Down Ű
*/
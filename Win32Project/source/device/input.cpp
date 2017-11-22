//=============================================================================
//
// 入力処理 [input.cpp]
// Author : AKIRA TANAKA
//
//=============================================================================
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	COUNT_WAIT_REPEAT	(20)	// リピート待ち時間

//*****************************************************************************
// 静的変数
//*****************************************************************************
LPDIRECTINPUT8	Input::m_pDInput = NULL;	// DirectInputオブジェクト

InputKeyboard* InputKeyboard::m_pInputKeyboard = NULL;

InputMouse* InputMouse::m_pInputMouse = NULL;

//=============================================================================
// CInputコンストラスタ
//=============================================================================
Input::Input()
{
	m_pDIDevice = NULL;
}

//=============================================================================
// CInputデストラスタ
//=============================================================================
Input::~Input()
{
}

//=============================================================================
// 入力処理の初期化処理
//=============================================================================
HRESULT Input::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr = S_FALSE;

	// DirectInputオブジェクトの作成
	if(!m_pDInput)
	{
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&m_pDInput, NULL);
	}

	return hr;
}

//=============================================================================
// 入力処理の更新処理
//=============================================================================
#if 0	// ---> 純粋仮想関数化
HRESULT CInput::Update(void)
{
	return S_OK;
}
#endif

//=============================================================================
// 入力処理の終了処理
//=============================================================================
void Input::Uninit(void)
{
	if(m_pDIDevice)
	{// デバイスオブジェクトの開放
		m_pDIDevice->Unacquire();

		m_pDIDevice->Release();
		m_pDIDevice = NULL;
	}

	if(m_pDInput)
	{// DirectInputオブジェクトの開放
		m_pDInput->Release();
		m_pDInput = NULL;
	}
}

//=============================================================================
// CInputKeyboardコンストラスタ
//=============================================================================
InputKeyboard::InputKeyboard()
{
	// 各ワークのクリア
	ZeroMemory(m_aKeyState, sizeof m_aKeyState);
	ZeroMemory(m_aKeyStateTrigger, sizeof m_aKeyStateTrigger);
	ZeroMemory(m_aKeyStateRelease, sizeof m_aKeyStateRelease);
	ZeroMemory(m_aKeyStateRepeat, sizeof m_aKeyStateRepeat);
	ZeroMemory(m_aKeyStateRepeatCnt, sizeof m_aKeyStateRepeatCnt);
}

//=============================================================================
// CInputKeyboardデストラスタ
//=============================================================================
InputKeyboard::~InputKeyboard()
{
}

///////////////////////////////////////////////////////////////////////////////
//インスタンス生成
///////////////////////////////////////////////////////////////////////////////
void InputKeyboard::CreateInstance(void)
{
	if (m_pInputKeyboard != NULL) { return; }
	m_pInputKeyboard = new InputKeyboard();
}

///////////////////////////////////////////////////////////////////////////////
//インスタンス取得
///////////////////////////////////////////////////////////////////////////////
InputKeyboard* InputKeyboard::GetInstance(void)
{
	return m_pInputKeyboard;
}

//=============================================================================
// キーボードの初期化処理
//=============================================================================
HRESULT InputKeyboard::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;
	// 入力処理の初期化
	Input::Init(hInst, hWnd);

	// デバイスオブジェクトを作成
	hr = m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, NULL);
	if(FAILED(hr) || m_pDIDevice == NULL)
	{
		MessageBox(hWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = m_pDIDevice->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// m_pDIDeviceアクセス権を獲得(入力制御開始)
	m_pDIDevice->Acquire();

	return hr;
}

//=============================================================================
// キーボード入力処理の開放
//=============================================================================
void InputKeyboard::Uninit(void)
{
	// 入力処理の開放
	Input::Uninit();
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
void InputKeyboard::Update(void)
{
	HRESULT hr;
	BYTE aKeyState[NUM_KEY_MAX];

	if(!m_pDIDevice)
	{
		return;
	}

	// デバイスからデータを取得
	hr = m_pDIDevice->GetDeviceState(sizeof(aKeyState), aKeyState);
	if(SUCCEEDED(hr))
	{
		for(int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			m_aKeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & aKeyState[nCntKey];
			m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & ~aKeyState[nCntKey];
			m_aKeyStateRepeat[nCntKey] = m_aKeyStateTrigger[nCntKey];

			if(aKeyState[nCntKey])
			{
				m_aKeyStateRepeatCnt[nCntKey]++;
				if(m_aKeyStateRepeatCnt[nCntKey] >= COUNT_WAIT_REPEAT)
				{
					m_aKeyStateRepeat[nCntKey] = aKeyState[nCntKey];
				}
			}
			else
			{
				m_aKeyStateRepeatCnt[nCntKey] = 0;
				m_aKeyStateRepeat[nCntKey] = 0;
			}

			m_aKeyState[nCntKey] = aKeyState[nCntKey];
		}
	}
	else
	{
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// キーボードデータ取得(プレス)
//=============================================================================
BOOL InputKeyboard::GetKeyPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// キーボードデータ取得(トリガー)
//=============================================================================
BOOL InputKeyboard::GetKeyTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// キーボードデータ取得(リリース)
//=============================================================================
BOOL InputKeyboard::GetKeyRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// キーボードデータ取得(リピート)
//=============================================================================
BOOL InputKeyboard::GetKeyRepeat(int nKey)
{
	return (m_aKeyStateRepeat[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// キーボードデータ削除(トリガー)
//=============================================================================
void InputKeyboard::FlushKeyTrigger(int nKey)
{
	m_aKeyStateTrigger[nKey] = 0;
}

//=============================================================================
// Mouseコンストラスタ
//=============================================================================
InputMouse::InputMouse()
{
}

//=============================================================================
// Mouseデストラスタ
//=============================================================================
InputMouse::~InputMouse()
{
}

///////////////////////////////////////////////////////////////////////////////
//インスタンス生成
///////////////////////////////////////////////////////////////////////////////
void InputMouse::CreateInstance(void)
{
	if (m_pInputMouse != NULL) { return; }
	m_pInputMouse = new InputMouse();
}

///////////////////////////////////////////////////////////////////////////////
//インスタンス取得
///////////////////////////////////////////////////////////////////////////////
InputMouse* InputMouse::GetInstance(void)
{
	return m_pInputMouse;
}

//=============================================================================
// マウスの初期化処理
//=============================================================================
HRESULT InputMouse::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// 入力処理の初期化
	Input::Init(hInst, hWnd);

	// デバイスオブジェクトを作成
	hr = m_pDInput->CreateDevice(GUID_SysMouse, &m_pDIDevice, NULL);
	if (FAILED(hr) || m_pDIDevice == NULL)
	{
		MessageBox(hWnd, "マウスがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = m_pDIDevice->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "マウスのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(hr))
	{
		MessageBox(hWnd, "マウスの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	//デバイスの設定
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_REL;

	hr = m_pDIDevice->SetProperty(DIPROP_AXISMODE, &dipdw.diph);

	// m_pDIDeviceアクセス権を獲得(入力制御開始)
	m_pDIDevice->Acquire();

	return hr;
}

//=============================================================================
// マウス入力処理の開放
//=============================================================================
void InputMouse::Uninit(void)
{
	// 入力処理の開放
	Input::Uninit();
}

//=============================================================================
// マウスの更新処理
//=============================================================================
void InputMouse::Update(void)
{
	HRESULT hr;
	DIMOUSESTATE2 mouseState;

	if (!m_pDIDevice)
	{
		return;
	}

	// デバイスからデータを取得
	hr = m_pDIDevice->GetDeviceState(sizeof(mouseState), &mouseState);

	if (SUCCEEDED(hr))
	{
		m_mouseStateTrigger.lX = ((m_mouseState.lX ^ mouseState.lX) & mouseState.lX);
		m_mouseStateTrigger.lY = ((m_mouseState.lY ^ mouseState.lY) & mouseState.lY);
		m_mouseStateTrigger.lZ = ((m_mouseState.lZ ^ mouseState.lZ) & mouseState.lZ);

		for (int nCntKey = 0; nCntKey < 8; nCntKey++)
		{
			m_mouseStateTrigger.rgbButtons[nCntKey] = ((m_mouseState.rgbButtons[nCntKey] ^ mouseState.rgbButtons[nCntKey]) & mouseState.rgbButtons[nCntKey]);
		}

		m_mouseState = mouseState;

		//スクリーン座標を取得
		GetCursorPos(&m_posMouseWorld);
	}
	else
	{
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// マウスデータ取得(プレス)
//=============================================================================
BOOL InputMouse::GetLeftPress(void)
{
	return (m_mouseState.rgbButtons[0] & 0X80) ? TRUE : FALSE;
}

//=============================================================================
// マウスデータ取得(トリガー)
//=============================================================================
BOOL InputMouse::GetLeftTrigger(void)
{
	return (m_mouseStateTrigger.rgbButtons[0] & 0X80) ? TRUE : FALSE;
}

//=============================================================================
// マウスデータ取得(プレス)
//=============================================================================
BOOL InputMouse::GetRightPress(void)
{
	return (m_mouseState.rgbButtons[1] & 0X80) ? TRUE : FALSE;
}

//=============================================================================
// マウスデータ取得(トリガー)
//=============================================================================
BOOL InputMouse::GetRightTrigger(void)
{
	return (m_mouseStateTrigger.rgbButtons[1] & 0X80) ? TRUE : FALSE;
}

//=============================================================================
// マウスデータ取得(プレス)
//=============================================================================
BOOL InputMouse::GetCenterPress(void)
{
	return (m_mouseState.rgbButtons[2] & 0X80) ? TRUE : FALSE;
}

//=============================================================================
// キーボードデータ取得(トリガー)
//=============================================================================
BOOL InputMouse::GetCenterTrigger(void)
{
	return (m_mouseStateTrigger.rgbButtons[2] & 0X80) ? TRUE : FALSE;
}

LONG InputMouse::GetAxisX(void)
{
	return m_mouseState.lX;
}

LONG InputMouse::GetAxisY(void)
{
	return m_mouseState.lY;
}

LONG InputMouse::GetAxisZ(void)
{
	return m_mouseState.lZ;
}
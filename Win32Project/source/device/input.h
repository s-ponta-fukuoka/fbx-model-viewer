//=============================================================================
//
// 入力処理 [input.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _INPUT_H_
#define _INPUT_H_

#include "../renderer/app_renderer.h"

#include <XInput.h>

#pragma comment(lib,"Xinput.lib")
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	NUM_KEY_MAX		(256)	// キー最大数

//*********************************************************
// 入力クラス
//*********************************************************
class Input
{
public:
	Input();
	virtual ~Input();

	virtual HRESULT Init(HINSTANCE hInst, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;		// ---> 純粋仮想関数化

protected:
	static LPDIRECTINPUT8 m_pDInput;	// DirectInputオブジェクト
	LPDIRECTINPUTDEVICE8 m_pDIDevice;	// Deviceオブジェクト(入力に必要)
};

//*********************************************************
// キーボード入力クラス
//*********************************************************
class InputKeyboard : public Input
{
public:
	virtual ~InputKeyboard();

	HRESULT Init(HINSTANCE hInst, HWND hWnd);
	void Uninit(void);
	void Update(void);

	BOOL GetKeyPress(int nKey);
	BOOL GetKeyTrigger(int nKey);
	BOOL GetKeyRelease(int nKey);
	BOOL GetKeyRepeat(int nKey);
	void FlushKeyTrigger(int nKey);

	//インスタンス生成
	static void CreateInstance(void);

	//インスタンス取得
	static InputKeyboard* GetInstance(void);

private:
	InputKeyboard();

	static InputKeyboard* m_pInputKeyboard;

	BYTE	m_aKeyState[NUM_KEY_MAX];			// キーボードの状態を受け取るワーク
	BYTE	m_aKeyStateTrigger[NUM_KEY_MAX];	// トリガーワーク
	BYTE	m_aKeyStateRelease[NUM_KEY_MAX];	// リリースワーク
	BYTE	m_aKeyStateRepeat[NUM_KEY_MAX];		// リピートワーク
	int		m_aKeyStateRepeatCnt[NUM_KEY_MAX];	// リピートカウンタ
};

//*********************************************************
// マウス入力クラス
//*********************************************************
class InputMouse : public Input
{
public:
	~InputMouse();

	HRESULT Init(HINSTANCE hInst, HWND hWnd);
	void Uninit(void);
	void Update(void);

	BOOL GetLeftPress(void);
	BOOL GetLeftTrigger(void);

	BOOL GetRightPress(void);
	BOOL GetRightTrigger(void);

	BOOL GetCenterPress(void);
	BOOL GetCenterTrigger(void);

	LONG GetAxisX(void);
	LONG GetAxisY(void);
	LONG GetAxisZ(void);

	POINT* GetPosWorld(void) { return &m_posMouseWorld; }

	//インスタンス生成
	static void CreateInstance(void);

	//インスタンス取得
	static InputMouse* GetInstance(void);

private:
	InputMouse();

	static InputMouse* m_pInputMouse;

	DIMOUSESTATE2 m_mouseState;
	DIMOUSESTATE2 m_mouseStateTrigger;
	POINT m_posMouseWorld;
};

#endif
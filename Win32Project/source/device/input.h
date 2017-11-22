//=============================================================================
//
// ���͏��� [input.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _INPUT_H_
#define _INPUT_H_

#include "../renderer/app_renderer.h"

#include <XInput.h>

#pragma comment(lib,"Xinput.lib")
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	NUM_KEY_MAX		(256)	// �L�[�ő吔

//*********************************************************
// ���̓N���X
//*********************************************************
class Input
{
public:
	Input();
	virtual ~Input();

	virtual HRESULT Init(HINSTANCE hInst, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;		// ---> �������z�֐���

protected:
	static LPDIRECTINPUT8 m_pDInput;	// DirectInput�I�u�W�F�N�g
	LPDIRECTINPUTDEVICE8 m_pDIDevice;	// Device�I�u�W�F�N�g(���͂ɕK�v)
};

//*********************************************************
// �L�[�{�[�h���̓N���X
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

	//�C���X�^���X����
	static void CreateInstance(void);

	//�C���X�^���X�擾
	static InputKeyboard* GetInstance(void);

private:
	InputKeyboard();

	static InputKeyboard* m_pInputKeyboard;

	BYTE	m_aKeyState[NUM_KEY_MAX];			// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
	BYTE	m_aKeyStateTrigger[NUM_KEY_MAX];	// �g���K�[���[�N
	BYTE	m_aKeyStateRelease[NUM_KEY_MAX];	// �����[�X���[�N
	BYTE	m_aKeyStateRepeat[NUM_KEY_MAX];		// ���s�[�g���[�N
	int		m_aKeyStateRepeatCnt[NUM_KEY_MAX];	// ���s�[�g�J�E���^
};

//*********************************************************
// �}�E�X���̓N���X
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

	//�C���X�^���X����
	static void CreateInstance(void);

	//�C���X�^���X�擾
	static InputMouse* GetInstance(void);

private:
	InputMouse();

	static InputMouse* m_pInputMouse;

	DIMOUSESTATE2 m_mouseState;
	DIMOUSESTATE2 m_mouseStateTrigger;
	POINT m_posMouseWorld;
};

#endif
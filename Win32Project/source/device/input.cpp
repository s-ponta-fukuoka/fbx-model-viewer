//=============================================================================
//
// ���͏��� [input.cpp]
// Author : AKIRA TANAKA
//
//=============================================================================
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	COUNT_WAIT_REPEAT	(20)	// ���s�[�g�҂�����

//*****************************************************************************
// �ÓI�ϐ�
//*****************************************************************************
LPDIRECTINPUT8	Input::m_pDInput = NULL;	// DirectInput�I�u�W�F�N�g

InputKeyboard* InputKeyboard::m_pInputKeyboard = NULL;

InputMouse* InputMouse::m_pInputMouse = NULL;

//=============================================================================
// CInput�R���X�g���X�^
//=============================================================================
Input::Input()
{
	m_pDIDevice = NULL;
}

//=============================================================================
// CInput�f�X�g���X�^
//=============================================================================
Input::~Input()
{
}

//=============================================================================
// ���͏����̏���������
//=============================================================================
HRESULT Input::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr = S_FALSE;

	// DirectInput�I�u�W�F�N�g�̍쐬
	if(!m_pDInput)
	{
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&m_pDInput, NULL);
	}

	return hr;
}

//=============================================================================
// ���͏����̍X�V����
//=============================================================================
#if 0	// ---> �������z�֐���
HRESULT CInput::Update(void)
{
	return S_OK;
}
#endif

//=============================================================================
// ���͏����̏I������
//=============================================================================
void Input::Uninit(void)
{
	if(m_pDIDevice)
	{// �f�o�C�X�I�u�W�F�N�g�̊J��
		m_pDIDevice->Unacquire();

		m_pDIDevice->Release();
		m_pDIDevice = NULL;
	}

	if(m_pDInput)
	{// DirectInput�I�u�W�F�N�g�̊J��
		m_pDInput->Release();
		m_pDInput = NULL;
	}
}

//=============================================================================
// CInputKeyboard�R���X�g���X�^
//=============================================================================
InputKeyboard::InputKeyboard()
{
	// �e���[�N�̃N���A
	ZeroMemory(m_aKeyState, sizeof m_aKeyState);
	ZeroMemory(m_aKeyStateTrigger, sizeof m_aKeyStateTrigger);
	ZeroMemory(m_aKeyStateRelease, sizeof m_aKeyStateRelease);
	ZeroMemory(m_aKeyStateRepeat, sizeof m_aKeyStateRepeat);
	ZeroMemory(m_aKeyStateRepeatCnt, sizeof m_aKeyStateRepeatCnt);
}

//=============================================================================
// CInputKeyboard�f�X�g���X�^
//=============================================================================
InputKeyboard::~InputKeyboard()
{
}

///////////////////////////////////////////////////////////////////////////////
//�C���X�^���X����
///////////////////////////////////////////////////////////////////////////////
void InputKeyboard::CreateInstance(void)
{
	if (m_pInputKeyboard != NULL) { return; }
	m_pInputKeyboard = new InputKeyboard();
}

///////////////////////////////////////////////////////////////////////////////
//�C���X�^���X�擾
///////////////////////////////////////////////////////////////////////////////
InputKeyboard* InputKeyboard::GetInstance(void)
{
	return m_pInputKeyboard;
}

//=============================================================================
// �L�[�{�[�h�̏���������
//=============================================================================
HRESULT InputKeyboard::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;
	// ���͏����̏�����
	Input::Init(hInst, hWnd);

	// �f�o�C�X�I�u�W�F�N�g���쐬
	hr = m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, NULL);
	if(FAILED(hr) || m_pDIDevice == NULL)
	{
		MessageBox(hWnd, "�L�[�{�[�h���˂��I", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = m_pDIDevice->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// m_pDIDevice�A�N�Z�X�����l��(���͐���J�n)
	m_pDIDevice->Acquire();

	return hr;
}

//=============================================================================
// �L�[�{�[�h���͏����̊J��
//=============================================================================
void InputKeyboard::Uninit(void)
{
	// ���͏����̊J��
	Input::Uninit();
}

//=============================================================================
// �L�[�{�[�h�̍X�V����
//=============================================================================
void InputKeyboard::Update(void)
{
	HRESULT hr;
	BYTE aKeyState[NUM_KEY_MAX];

	if(!m_pDIDevice)
	{
		return;
	}

	// �f�o�C�X����f�[�^���擾
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
// �L�[�{�[�h�f�[�^�擾(�v���X)
//=============================================================================
BOOL InputKeyboard::GetKeyPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(�g���K�[)
//=============================================================================
BOOL InputKeyboard::GetKeyTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(�����[�X)
//=============================================================================
BOOL InputKeyboard::GetKeyRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(���s�[�g)
//=============================================================================
BOOL InputKeyboard::GetKeyRepeat(int nKey)
{
	return (m_aKeyStateRepeat[nKey] & 0x80) ? TRUE: FALSE;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�폜(�g���K�[)
//=============================================================================
void InputKeyboard::FlushKeyTrigger(int nKey)
{
	m_aKeyStateTrigger[nKey] = 0;
}

//=============================================================================
// Mouse�R���X�g���X�^
//=============================================================================
InputMouse::InputMouse()
{
}

//=============================================================================
// Mouse�f�X�g���X�^
//=============================================================================
InputMouse::~InputMouse()
{
}

///////////////////////////////////////////////////////////////////////////////
//�C���X�^���X����
///////////////////////////////////////////////////////////////////////////////
void InputMouse::CreateInstance(void)
{
	if (m_pInputMouse != NULL) { return; }
	m_pInputMouse = new InputMouse();
}

///////////////////////////////////////////////////////////////////////////////
//�C���X�^���X�擾
///////////////////////////////////////////////////////////////////////////////
InputMouse* InputMouse::GetInstance(void)
{
	return m_pInputMouse;
}

//=============================================================================
// �}�E�X�̏���������
//=============================================================================
HRESULT InputMouse::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// ���͏����̏�����
	Input::Init(hInst, hWnd);

	// �f�o�C�X�I�u�W�F�N�g���쐬
	hr = m_pDInput->CreateDevice(GUID_SysMouse, &m_pDIDevice, NULL);
	if (FAILED(hr) || m_pDIDevice == NULL)
	{
		MessageBox(hWnd, "�}�E�X���˂��I", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = m_pDIDevice->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�}�E�X�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�}�E�X�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	//�f�o�C�X�̐ݒ�
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_REL;

	hr = m_pDIDevice->SetProperty(DIPROP_AXISMODE, &dipdw.diph);

	// m_pDIDevice�A�N�Z�X�����l��(���͐���J�n)
	m_pDIDevice->Acquire();

	return hr;
}

//=============================================================================
// �}�E�X���͏����̊J��
//=============================================================================
void InputMouse::Uninit(void)
{
	// ���͏����̊J��
	Input::Uninit();
}

//=============================================================================
// �}�E�X�̍X�V����
//=============================================================================
void InputMouse::Update(void)
{
	HRESULT hr;
	DIMOUSESTATE2 mouseState;

	if (!m_pDIDevice)
	{
		return;
	}

	// �f�o�C�X����f�[�^���擾
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

		//�X�N���[�����W���擾
		GetCursorPos(&m_posMouseWorld);
	}
	else
	{
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// �}�E�X�f�[�^�擾(�v���X)
//=============================================================================
BOOL InputMouse::GetLeftPress(void)
{
	return (m_mouseState.rgbButtons[0] & 0X80) ? TRUE : FALSE;
}

//=============================================================================
// �}�E�X�f�[�^�擾(�g���K�[)
//=============================================================================
BOOL InputMouse::GetLeftTrigger(void)
{
	return (m_mouseStateTrigger.rgbButtons[0] & 0X80) ? TRUE : FALSE;
}

//=============================================================================
// �}�E�X�f�[�^�擾(�v���X)
//=============================================================================
BOOL InputMouse::GetRightPress(void)
{
	return (m_mouseState.rgbButtons[1] & 0X80) ? TRUE : FALSE;
}

//=============================================================================
// �}�E�X�f�[�^�擾(�g���K�[)
//=============================================================================
BOOL InputMouse::GetRightTrigger(void)
{
	return (m_mouseStateTrigger.rgbButtons[1] & 0X80) ? TRUE : FALSE;
}

//=============================================================================
// �}�E�X�f�[�^�擾(�v���X)
//=============================================================================
BOOL InputMouse::GetCenterPress(void)
{
	return (m_mouseState.rgbButtons[2] & 0X80) ? TRUE : FALSE;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(�g���K�[)
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
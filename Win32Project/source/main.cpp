//=============================================================================
//
// main.cpp
// Author : SHOTA FUKUOKA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "../resource.h"
#include "app/app.h"
#include "gui/imgui.h"
#include "gui/imgui_impl_dx11.h"
#include "object\character\player\player.h"

#pragma comment (lib, "winmm.lib")

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"TASO"	// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

char g_FileName[MAX_PATH] = { 0 };

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		LoadIcon(hInstance , TEXT("KITTY")),
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		MAKEINTRESOURCE(IDR_MENU1),
		CLASS_NAME,
		NULL
	};

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	HWND hWnd;
	MSG msg;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;

	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �w�肵���N���C�A���g�̈���m�ۂ��邽�߂ɕK�v�ȃE�B���h�E���W���v�Z
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(rect.right - rect.left),
		(rect.bottom - rect.top),
		NULL,
		NULL,
		hInstance,
		NULL);

	// ����\��ݒ�
	timeBeginPeriod(1);

	// �t���[���J�E���g������
	dwCurrentTime =
		dwFrameCount = 0;
	dwExecLastTime =
		dwFPSLastTime = timeGetTime();

	// �E�C���h�E�̕\��
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	App* pApp = new App();
	pApp->Init(hWnd, hInstance);

	// ���b�Z�[�W���[�v
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - dwFPSLastTime) >= 500)	// 0.5�b���ƂɎ��s
			{
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}
			
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;

				// �X�V����
				pApp->Update();

				// �`�揈��
				pApp->Draw();

				dwFrameCount++;
			}
		}
	}

	pApp->Release();
	delete[] pApp;

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// ����\��߂�
	timeEndPeriod(1);

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg)
	{

	case WM_CREATE:
		break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_40001://�J��
		{
			OPENFILENAME of = { 0 };
			of.lStructSize = sizeof(OPENFILENAME);
			of.hwndOwner = hWnd;
			of.lpstrFilter = "�o�C�i���t�@�C��(*.TASO)\0*.TASO\0";
			of.lpstrFile = g_FileName;
			of.nMaxFile = MAX_PATH;
			of.lpstrDefExt = "taso";
			if (GetOpenFileName(&of))
			{
				Player* pPlayer = Player::GetInstance();
				pPlayer->LoadFile(g_FileName);
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		case ID_40002://�㏑���ۑ�
		case ID_40003://�㏑���ۑ�
		{
			OPENFILENAME of = { 0 };
			of.lStructSize = sizeof(OPENFILENAME);
			of.hwndOwner = hWnd;
			of.lpstrFilter = "�o�C�i���t�@�C��(*.TASO)\0*.TASO\0";
			of.lpstrFile = g_FileName;
			of.nMaxFile = MAX_PATH;
			of.lpstrDefExt = "taso";
			if (GetSaveFileName(&of))
			{
				Player* pPlayer = Player::GetInstance();
				pPlayer->SaveFile(g_FileName);
			}
			break;
		}
		case ID_40005:// �I���������ꂽ��
		{
			DestroyWindow(hWnd);		// �E�B���h�E��j������悤�w������
			break;
		}
		}
		break;
	}

	case WM_DROPFILES:
	{
		char buf[MAX_PATH + 1];
		int fileCount = DragQueryFile((HDROP)wParam, -1, buf, MAX_PATH);

		for (int i = 0; i < fileCount; i++)
		{
			DragQueryFile((HDROP)wParam, i, buf, MAX_PATH);
			//�t�@�C������
			MessageBox(hWnd, "�ǂݍ��݂܂���", "", MB_ICONWARNING);
		}
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:					// [ESC]�L�[�������ꂽ
			DestroyWindow(hWnd);		// �E�B���h�E��j������悤�w������
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
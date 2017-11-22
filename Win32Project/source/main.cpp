//=============================================================================
//
// main.cpp
// Author : SHOTA FUKUOKA
//
//=============================================================================

//*****************************************************************************
// インクルード
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
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"TASO"	// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

char g_FileName[MAX_PATH] = { 0 };

//=============================================================================
// メイン関数
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

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// 指定したクライアント領域を確保するために必要なウィンドウ座標を計算
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの作成
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

	// 分解能を設定
	timeBeginPeriod(1);

	// フレームカウント初期化
	dwCurrentTime =
		dwFrameCount = 0;
	dwExecLastTime =
		dwFPSLastTime = timeGetTime();

	// ウインドウの表示
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	App* pApp = new App();
	pApp->Init(hWnd, hInstance);

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - dwFPSLastTime) >= 500)	// 0.5秒ごとに実行
			{
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}
			
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;

				// 更新処理
				pApp->Update();

				// 描画処理
				pApp->Draw();

				dwFrameCount++;
			}
		}
	}

	pApp->Release();
	delete[] pApp;

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 分解能を戻す
	timeEndPeriod(1);

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
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
		case ID_40001://開く
		{
			OPENFILENAME of = { 0 };
			of.lStructSize = sizeof(OPENFILENAME);
			of.hwndOwner = hWnd;
			of.lpstrFilter = "バイナリファイル(*.TASO)\0*.TASO\0";
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
		case ID_40002://上書き保存
		case ID_40003://上書き保存
		{
			OPENFILENAME of = { 0 };
			of.lStructSize = sizeof(OPENFILENAME);
			of.hwndOwner = hWnd;
			of.lpstrFilter = "バイナリファイル(*.TASO)\0*.TASO\0";
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
		case ID_40005:// 終了が押されたら
		{
			DestroyWindow(hWnd);		// ウィンドウを破棄するよう指示する
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
			//ファイル処理
			MessageBox(hWnd, "読み込みました", "", MB_ICONWARNING);
		}
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:					// [ESC]キーが押された
			DestroyWindow(hWnd);		// ウィンドウを破棄するよう指示する
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
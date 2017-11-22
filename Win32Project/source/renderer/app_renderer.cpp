//=============================================================================
//
// directxレンダリング処理 [rendereDX.cpp]
// Author : SHOTA FUKUOKA
//
//=============================================================================
#include "app_renderer.h"
#include "../app/app.h"
#include "render_manager.h"
#include "../gui/imgui.h"
#include "../gui/imgui_impl_dx11.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
AppRenderer* AppRenderer::m_pAppRenderer = NULL;

///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
///////////////////////////////////////////////////////////////////////////////
AppRenderer::AppRenderer( )
	: m_pDevice( NULL )
	, m_pDeviceContext( NULL )
	, m_pSwapChain( NULL )
	, m_pRenderTargetView( NULL )
	, m_pDepthStencilView( NULL )
{
	ConfigViewPort();
}

///////////////////////////////////////////////////////////////////////////////
// デストラクタ
///////////////////////////////////////////////////////////////////////////////
AppRenderer::~AppRenderer( )
{
	Release();
}

///////////////////////////////////////////////////////////////////////////////
//インスタンス生成
///////////////////////////////////////////////////////////////////////////////
void AppRenderer::CreateInstance(void)
{
	if (m_pAppRenderer != NULL) { return; }
	m_pAppRenderer = new AppRenderer();
}

///////////////////////////////////////////////////////////////////////////////
//インスタンス取得
///////////////////////////////////////////////////////////////////////////////
AppRenderer* AppRenderer::GetInstance(void)
{
	return m_pAppRenderer;
}

///////////////////////////////////////////////////////////////////////////////
// 初期化
///////////////////////////////////////////////////////////////////////////////
HRESULT AppRenderer::Init(HWND hWnd)
{
	ConfigDeviceAndSwapChain(hWnd);

	ConfigRenderTargets();

	ConfigViewPort();

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 終了処理
///////////////////////////////////////////////////////////////////////////////
void AppRenderer::Release(void)
{
	SAFE_RELEASE(m_pDevice);

	SAFE_RELEASE(m_pDeviceContext);

	SAFE_RELEASE(m_pSwapChain);

	SAFE_RELEASE(m_pRenderTargetView);

	SAFE_RELEASE(m_pDepthStencilView);
}

///////////////////////////////////////////////////////////////////////////////
// 描画処理
///////////////////////////////////////////////////////////////////////////////
void AppRenderer::Draw(RenderManager* pRenderManager)
{
	pRenderManager->ShadowDrawAll();

	float ClearColor[4] = { 0,0,1,1 }; //消去色
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, ClearColor);//画面クリア 

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pDeviceContext->RSSetViewports(1, &m_ViewPort);

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	pRenderManager->DrawAll();

	ImGui::Render();

	m_pSwapChain->Present(1, 0);//フリップ
}

///////////////////////////////////////////////////////////////////////////////
//デバイス取得
///////////////////////////////////////////////////////////////////////////////
ID3D11Device* AppRenderer::GetDevice(void) const
{
	return m_pDevice;
}

///////////////////////////////////////////////////////////////////////////////
//デバイスコンテキスト取得
///////////////////////////////////////////////////////////////////////////////
ID3D11DeviceContext* AppRenderer::GetDeviceContex(void) const
{
	return m_pDeviceContext;
}

///////////////////////////////////////////////////////////////////////////////
//レンダーターゲット取得
///////////////////////////////////////////////////////////////////////////////
ID3D11RenderTargetView* AppRenderer::GetRenderTargetView(void) const
{
	return m_pRenderTargetView;
}

///////////////////////////////////////////////////////////////////////////////
//ステンシルビュー取得
///////////////////////////////////////////////////////////////////////////////
ID3D11DepthStencilView* AppRenderer::GetDepthStencilView(void) const
{
	return m_pDepthStencilView;
}

///////////////////////////////////////////////////////////////////////////////
// デバイスとスワップチェーン設定
///////////////////////////////////////////////////////////////////////////////
void AppRenderer::ConfigDeviceAndSwapChain(const HWND hWnd)
{
	// デバイスとスワップチェーンの作成
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;						//バックバッファの数
	sd.BufferDesc.Width = SCREEN_WIDTH;		//バッファの幅
	sd.BufferDesc.Height = SCREEN_HEIGHT;    //バッファの高さ
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //バッファのフォーマット
	sd.BufferDesc.RefreshRate.Numerator = 60;   //リフレッシュレート
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL  FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
		&FeatureLevel, 1, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext)))
	{
		MessageBox(hWnd, "生成できませんでした", "デバイスとスワップチェーン", 0);
		return;
	}

	ConfigDepthStencilView(sd);
}

///////////////////////////////////////////////////////////////////////////////
//ステンシルターゲット設定
///////////////////////////////////////////////////////////////////////////////
void AppRenderer::ConfigDepthStencilView(const DXGI_SWAP_CHAIN_DESC sd)
{
	//ステンシル用テクスチャー作成
	ID3D11Texture2D* hpTexture2dDepth = NULL;
	D3D11_TEXTURE2D_DESC hTexture2dDesc;
	hTexture2dDesc.Width = SCREEN_WIDTH;
	hTexture2dDesc.Height = SCREEN_WIDTH;
	hTexture2dDesc.MipLevels = 1;
	hTexture2dDesc.ArraySize = 1;
	hTexture2dDesc.Format = DXGI_FORMAT_D32_FLOAT;
	hTexture2dDesc.SampleDesc = sd.SampleDesc;
	hTexture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	hTexture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hTexture2dDesc.CPUAccessFlags = 0;
	hTexture2dDesc.MiscFlags = 0;
	m_pDevice->CreateTexture2D(&hTexture2dDesc, NULL, &hpTexture2dDepth);

	//ステンシルターゲット作成
	D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
	hDepthStencilViewDesc.Format = hTexture2dDesc.Format;
	hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hDepthStencilViewDesc.Flags = 0;
	m_pDevice->CreateDepthStencilView(hpTexture2dDepth, &hDepthStencilViewDesc, &m_pDepthStencilView);

}

///////////////////////////////////////////////////////////////////////////////
//レンダーターゲットビュー設定
///////////////////////////////////////////////////////////////////////////////
void AppRenderer::ConfigRenderTargets(void)
{
	//レンダーターゲットビューの作成
	ID3D11Texture2D *BackBuffer = NULL;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
	m_pDevice->CreateRenderTargetView(BackBuffer, NULL, &m_pRenderTargetView);
	BackBuffer->Release();
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
}

///////////////////////////////////////////////////////////////////////////////
//ビューポート設定
///////////////////////////////////////////////////////////////////////////////
void AppRenderer::ConfigViewPort(void)
{
	//ビューポートの設定
	m_ViewPort.Width = SCREEN_WIDTH;
	m_ViewPort.Height = SCREEN_HEIGHT;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
}
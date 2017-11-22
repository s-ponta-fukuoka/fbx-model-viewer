//=============================================================================
//
// directxレンダリング処理 [rendere.h]
// Author : SHOTA FUKUOKA
//
//=============================================================================
#ifndef _APP_RENDERER_H_
#define _APP_RENDERER_H_

//*****************************************************************************
// ヘッダファイル
//*****************************************************************************
#include <d3d11.h>
#include <xnamath.h>
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

#include "../lib/vector.h"
#include "render_manager.h" 

//*****************************************************************************
// ライブラリファイル
// [構成プロパティ]->[リンカー]->[入力]->[追加の依存ファイル]に記述しても可能
//*****************************************************************************
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

#ifdef _DEBUG
#pragma comment(lib, "Effekseer.Debug.lib" )
#pragma comment(lib, "EffekseerRendererDX11.Debug.lib" )
#else
#pragma comment(lib, "Effekseer.Release.lib" )
#pragma comment(lib, "EffekseerRendererDX11.Release.lib" )
#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define D3D_PI ((FLOAT) 3.141592654f) 
#define D3DToRadian( degree ) ((degree) * (D3D_PI / 180.0f))
#define D3DToDegree( radian ) ((radian) * (180.0f / D3D_PI))

//*****************************************************************************
//構造体定義
//*****************************************************************************
class RenderManager;

//*****************************************************************************
// クラス定義
//*****************************************************************************
class AppRenderer
{
public:
	//頂点3D
	struct Vertex3D
	{
		VECTOR3 position;
		VECTOR3 normal;
		VECTOR4 color;
		VECTOR2 tex;
	};

	//頂点2D
	struct Vertex2D
	{
		VECTOR3 position;
		VECTOR4 color;
		VECTOR2 tex;
	};

	//定数
	struct Constant
	{
		XMMATRIX world;
		XMMATRIX view;			
		XMMATRIX projection;
		XMMATRIX lightView;		
		XMMATRIX lightProjection;
		XMVECTOR light;
	};

	//デストラクタ
	virtual ~AppRenderer( );

	//インスタンス生成
	static void CreateInstance(void);

	//インスタンス取得
	static AppRenderer* GetInstance(void);

	//初期化
	HRESULT Init( HWND hWnd );

	//終了
	void Release( void );

	//描画
	void Draw( RenderManager* pRenderManager );

	//デバイス取得
	ID3D11Device* GetDevice(void) const;

	//デバイスコンテキスト取得
	ID3D11DeviceContext* GetDeviceContex(void) const;

	//レンダーターゲット取得
	ID3D11RenderTargetView* GetRenderTargetView(void) const;

	//ステンシルビュー取得
	ID3D11DepthStencilView* GetDepthStencilView(void) const;

private:
	//コンストラクタ
	AppRenderer();

	// デバイスとスワップチェーン設定
	void ConfigDeviceAndSwapChain(const HWND hWnd);

	//ステンシルターゲット設定
	void ConfigDepthStencilView(const DXGI_SWAP_CHAIN_DESC sd);

	//レンダーターゲットビュー設定
	void ConfigRenderTargets(void);

	//ビューポート設定
	void ConfigViewPort(void);

	static AppRenderer*				m_pAppRenderer;

	ID3D11Device*					m_pDevice;

	ID3D11DeviceContext*			m_pDeviceContext;

	IDXGISwapChain*					m_pSwapChain;

	ID3D11RenderTargetView*			m_pRenderTargetView;

	ID3D11DepthStencilView*			m_pDepthStencilView;

	D3D11_VIEWPORT					m_ViewPort;
};

#endif
//=============================================================================
//
// [scene2D.cpp]
// Author : shota fukuoka
//
//=============================================================================
#include "shadow_map.h"
#include "../../../app/app.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SHADOW_MAP_VERTEX (4)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数:
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
///////////////////////////////////////////////////////////////////////////////
ShadowMap::ShadowMap(RenderManager* pRenderManager,
	ShaderManager* pShaderManager,
	TextureManager* pTextureManager)
{
	m_pVertexBuffer = NULL;

	m_pTexture = NULL;

	m_vertex.position = VECTOR3( 0.5f, 0.5f, 0.0f );

	m_vertex.color = VECTOR4(1.0f, 1.0f, 1.0f, 1.0f);			// ポリゴンの位置

	m_size = VECTOR3( 0.3f, 0.3f, 0.0f );

	AppRenderer* pAppRenderer = AppRenderer::GetInstance();

	ID3D11Device* pDevice = pAppRenderer->GetDevice();

	MakeVertex(pDevice);

	ConfigTexture2D(pDevice);

	ConfigRenderTargetView(pDevice);

	ConfigDepthStencilView(pDevice);

	ConfigShaderResourceView(pDevice);

	ConfigViewPort();

	pRenderManager->SetShadowViewPort(m_pView);

	pRenderManager->SetShadowDepthStencilView(m_pDepthStencilView);

	pRenderManager->SetShadowRenderTargetVie(m_pRenderTargetView);

	pRenderManager->SetShadowTexture(m_pTexture);

	pRenderManager->AddRenderer(new CanvasRenderer(m_pVertexBuffer,
		NULL,
		pShaderManager,
		m_pTexture,
		SHADOW_MAP_VERTEX,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		VertexShader::VS_2D,
		PixelShader::PS_2D));
}

///////////////////////////////////////////////////////////////////////////////
// デストラクタ
///////////////////////////////////////////////////////////////////////////////
ShadowMap::~ShadowMap()
{
}

///////////////////////////////////////////////////////////////////////////////
// 終了
///////////////////////////////////////////////////////////////////////////////
void ShadowMap::Release(void)
{
	Canvas::Release();
}

///////////////////////////////////////////////////////////////////////////////
// 更新
///////////////////////////////////////////////////////////////////////////////
void ShadowMap::Update( void )
{
}

///////////////////////////////////////////////////////////////////////////////
//頂点バッファ
///////////////////////////////////////////////////////////////////////////////
void ShadowMap::MakeVertex(ID3D11Device* pDevice)
{
	//四角形
	AppRenderer::Vertex2D vertices[] =
	{
		VECTOR3(m_vertex.position.x,m_vertex.position.y,m_vertex.position.z), VECTOR4(m_vertex.color.x,m_vertex.color.y,m_vertex.color.z,m_vertex.color.w), VECTOR2(0,0),
		VECTOR3(m_vertex.position.x + m_size.x,m_vertex.position.y,m_vertex.position.z), VECTOR4(m_vertex.color.x,m_vertex.color.y,m_vertex.color.z,m_vertex.color.w), VECTOR2(1,0),
		VECTOR3(m_vertex.position.x,m_vertex.position.y - m_size.y,m_vertex.position.z), VECTOR4(m_vertex.color.x,m_vertex.color.y,m_vertex.color.z,m_vertex.color.w), VECTOR2(0,1),
		VECTOR3(m_vertex.position.x + m_size.x,m_vertex.position.y - m_size.y,m_vertex.position.z), VECTOR4(m_vertex.color.x,m_vertex.color.y,m_vertex.color.z,m_vertex.color.w), VECTOR2(1,1),
	};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(AppRenderer::Vertex2D) * SHADOW_MAP_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer)))
	{
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
//テクスチャ2D設定
///////////////////////////////////////////////////////////////////////////////
void ShadowMap::ConfigTexture2D(ID3D11Device* pDevice)
{

	// 2次元テクスチャの設定
	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.Width = 2048;
	texDesc.Height = 2048;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// 2次元テクスチャの生成
	HRESULT  hr = pDevice->CreateTexture2D(&texDesc, NULL, &m_pTexture2D);
	if (FAILED(hr))
	{
		return;
	}

	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	// 2次元テクスチャの生成
	hr = pDevice->CreateTexture2D(&texDesc, NULL, &m_pStencilTexture2D);
	if (FAILED(hr))
	{
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
//レンダーターゲット設定
///////////////////////////////////////////////////////////////////////////////
void ShadowMap::ConfigRenderTargetView(ID3D11Device* pDevice)
{
	// レンダーターゲットビューの設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	HRESULT hr = pDevice->CreateRenderTargetView(m_pTexture2D, &rtvDesc, &m_pRenderTargetView);
	if (FAILED(hr))
	{
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
//ステンシルターゲット設定
///////////////////////////////////////////////////////////////////////////////
void ShadowMap::ConfigDepthStencilView(ID3D11Device* pDevice)
{
	//ステンシルターゲット作成
	D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
	hDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hDepthStencilViewDesc.Flags = 0;
	hDepthStencilViewDesc.Texture2D.MipSlice = 0;
	HRESULT hr = pDevice->CreateDepthStencilView(m_pStencilTexture2D, &hDepthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(hr))
	{
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
//シェーダーリソースビュー設定
///////////////////////////////////////////////////////////////////////////////
void ShadowMap::ConfigShaderResourceView(ID3D11Device* pDevice)
{
	// シェーダ リソース ビューの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // フォーマット
	srDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;  // 2Dテクスチャ
	srDesc.Texture2D.MostDetailedMip = 0;   // 最初のミップマップ レベル
	srDesc.Texture2D.MipLevels = -1;  // すべてのミップマップ レベル
	HRESULT hr = pDevice->CreateShaderResourceView(m_pTexture2D, &srDesc, &m_pTexture);  // 受け取る変数
	if (FAILED(hr))
	{
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
//ビューポート設定
///////////////////////////////////////////////////////////////////////////////
void ShadowMap::ConfigViewPort(void)
{
	// ビューポートの設定
	m_pView.TopLeftX = 0.0f;		// ビューポート領域の左上X座標。
	m_pView.TopLeftY = 0.0f;		// ビューポート領域の左上Y座標。
	m_pView.Width = 2048;			// ビューポート領域の幅
	m_pView.Height = 2048;			// ビューポート領域の高さ
	m_pView.MinDepth = 0.0f;		// ビューポート領域の深度値の最小値
	m_pView.MaxDepth = 1.0f;		// ビューポート領域の深度値の最大値
}
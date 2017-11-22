//=============================================================================
//
// [ShadowMap.h]
// Author : shota fukuoka
//
//=============================================================================
#ifndef _SHADOW_MAP_H_
#define _SHADOW_MAP_H_

//*****************************************************************************
// ヘッダファイル
//*****************************************************************************
#include <Windows.h>
#include "../../canvas/canvas.h"
#include "../../../renderer/app_renderer.h"

//*****************************************************************************
// ライブラリファイル
// [構成プロパティ]->[リンカー]->[入力]->[追加の依存ファイル]に記述しても可能
//*****************************************************************************

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class ShadowMap : public Canvas
{
public:
	//コンストラクタ
	ShadowMap(RenderManager* pRenderManager,
		ShaderManager* pShaderManager,
		TextureManager* pTextureManager);

	//デストラクタ
	virtual ~ShadowMap( );

	//終了
	void Release( void );

	//更新
	void Update( void );

	//バッファ
	void MakeVertex(ID3D11Device* pDevice);

	//テクスチャ2D設定
	void ConfigTexture2D(ID3D11Device* pDevice);

	//レンダーターゲット設定
	void ConfigRenderTargetView(ID3D11Device* pDevice);

	//ステンシルターゲット設定
	void ConfigDepthStencilView(ID3D11Device* pDevice);

	//シェーダーリソースビュー設定
	void ConfigShaderResourceView(ID3D11Device* pDevice);

	//ビューポート設定
	void ConfigViewPort(void);

private:
	VECTOR3							m_size;

	ID3D11BlendState*				m_pBlendState;

	D3D11_VIEWPORT					m_pView;

	ID3D11DepthStencilView*			m_pDepthStencilView;

	ID3D11RenderTargetView*			m_pRenderTargetView;

	ID3D11ShaderResourceView*		m_pTexture;

	ID3D11Texture2D*				m_pTexture2D;

	ID3D11Texture2D*				m_pStencilTexture2D;
};

#endif
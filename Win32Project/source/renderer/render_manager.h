//=============================================================================
//
// render_manager.h
// Author : shoto fukuoka
//
//=============================================================================
#ifndef _RENDER_MANAGER_H_
#define _RENDER_MANAGER_H_

//*****************************************************************************
// ヘッダファイル
//*****************************************************************************
#include <list>

#include "app_renderer.h"

class Renderer;

//*****************************************************************************
// クラス定義
//*****************************************************************************
class RenderManager
{
	std::list<Renderer*>		m_listRenderer;

	std::list<Renderer*>		m_listShadowRenderer;

	D3D11_VIEWPORT					m_ShadowViewPort;

	ID3D11DepthStencilView*			m_pShadowDepthStencilView;

	ID3D11RenderTargetView*			m_pShadowRenderTargetView;

	ID3D11ShaderResourceView*		m_pShadowTexture;
public:

	//コンストラクタ
	RenderManager();

	//デストラクタ
	virtual ~RenderManager();

	//追加
	void AddRenderer(Renderer* renderer);

	//削除
	void DeleteRenderer(const Renderer* renderer);

	//全描画
	void DrawAll(void);

	//追加
	void AddShadowRenderer(Renderer* renderer);

	//削除
	void DeleteShadowRenderer(const Renderer* renderer);

	//シャドウマップ用描画
	void ShadowDrawAll(void);

	//全消去
	void ReleaseAll(void);

	//影ビューポート設定
	void SetShadowViewPort(D3D11_VIEWPORT ShadowViewPort);

	//影ステンシルビュー設定
	void SetShadowDepthStencilView(ID3D11DepthStencilView* pShadowDepthStencilView);

	//影レンダーターゲットビュー設定
	void SetShadowRenderTargetVie(ID3D11RenderTargetView* pShadowRenderTargetView);

	//シャドウマップ設定
	void SetShadowTexture(ID3D11ShaderResourceView* pShadowTexture);

	//シャドウマップ取得
	ID3D11ShaderResourceView* GetShadowTexture(void) const;
};

#endif

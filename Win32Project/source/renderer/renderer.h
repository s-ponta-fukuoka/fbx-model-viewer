//=============================================================================
//
// directxレンダリング処理 [rendere.h]
// Author : SHOTA FUKUOKA
//
//=============================================================================
#ifndef _RENDERER_H_
#define _RENDERER_H_

//*****************************************************************************
// ヘッダファイル
//*****************************************************************************
#include "app_renderer.h"
#include "../object/object.h"
#include "../shader/shader.h"
#include "../texture/texture_manager.h"
#include "../model/model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
//構造体定義
//*****************************************************************************

//*****************************************************************************
// クラス定義
//*****************************************************************************
class Renderer
{
public:
	//コンストラクタ
	Renderer();

	//デストラクタ
	virtual ~Renderer();

	//終了
	void Release(void);

	//描画
	virtual void Draw(void) = 0;

protected:
	//定数バッファ設定
	void ConfigConstantBuffer(UINT ByteWidth);

	//サンプラーステート設定
	void ConfigSamplerState(void);

	ID3D11BlendState*					m_pBlendState;

	ID3D11SamplerState*					m_pSampleLinear;

	ID3D11ShaderResourceView*			m_pTexture;

	ID3D11Buffer*						m_pVertexBuffer;

	ID3D11Buffer*						m_pConstantBuffer;

	ID3D11Buffer*						m_pIndexBuffer;

	VertexShader*						m_pVertexShader;

	PixelShader*						m_pPixelShader;

	Object::Transform*					m_pTransform;

};

//-----------------------------------------------------------------------------
// メッシュレンダラー
//-----------------------------------------------------------------------------
class MeshRenderer : public Renderer
{
public:
	//コンストラクタ
	MeshRenderer(ID3D11Buffer* pVertexBuffer,
		ID3D11Buffer* pIndexBuffer,
		ShaderManager* pShaderManager,
		ID3D11ShaderResourceView* pTexture,
		ID3D11ShaderResourceView* pShadowMap,
		Object::Transform* pTransform,
		AppRenderer::Constant* pConstant,
		AppRenderer::Constant* pLightConstant,
		int	nNumVertexPolygon,
		D3D_PRIMITIVE_TOPOLOGY ePolygon,
		VertexShader::VERTEX_TYPE eVsType,
		PixelShader::PIXEL_TYPE ePsType);

	//デストラクタ
	virtual ~MeshRenderer();

	//終了
	void Release(void);

	//描画
	void Draw(void);

private:
	AppRenderer::Constant*				m_pConstant;

	AppRenderer::Constant*				m_pLightConstant;

	int									m_nNumVertexPolygon;

	D3D_PRIMITIVE_TOPOLOGY				m_ePolygon;

	ID3D11ShaderResourceView*			m_pShadowMap;
};

//-----------------------------------------------------------------------------
// スキニードレンダラー
//-----------------------------------------------------------------------------
class SkinnedMeshRenderer : public Renderer
{
public:
	//コンストラクタ
	SkinnedMeshRenderer(ID3D11Buffer* pVertexBuffer,
		ID3D11Buffer* pIndexBuffer,
		ShaderManager* pShaderManager,
		ID3D11ShaderResourceView* pTexture,
		ID3D11ShaderResourceView* pShadowMap,
		Object::Transform* pTransform,
		AppRenderer::Constant* pConstant,
		AppRenderer::Constant* pLightConstant,
		int	nNumVertexPolygon,
		int* pFrame,
		int* pAnimeNumber,
		D3D_PRIMITIVE_TOPOLOGY ePolygon,
		VertexShader::VERTEX_TYPE eVsType,
		PixelShader::PIXEL_TYPE ePsType,
		SkinMeshModel::Cluster*	pCluster,
		SkinMeshModel::Mesh mesh);

	//デストラクタ
	virtual ~SkinnedMeshRenderer();

	//終了
	void Release(void);

	//描画
	void Draw(void);

private:
	AppRenderer::Constant*				m_pConstant;

	AppRenderer::Constant*				m_pLightConstant;

	int									m_nNumVertexPolygon;

	int*								m_pFrame;

	int*								m_pAnimeNumber;

	D3D_PRIMITIVE_TOPOLOGY				m_ePolygon;

	ID3D11ShaderResourceView*			m_pShadowMap;

	SkinMeshModel::Cluster*				m_pCluster;

	SkinMeshModel::Mesh					m_mesh;
};

//-----------------------------------------------------------------------------
// キャンバスレンダラー
//-----------------------------------------------------------------------------
class CanvasRenderer : public Renderer
{
public:
	//コンストラクタ
	CanvasRenderer(ID3D11Buffer* pVertexBuffer,
		ID3D11Buffer* pIndexBuffer,
		ShaderManager* pShaderManager,
		ID3D11ShaderResourceView* pTexture,
		int	nNumVertexPolygon,
		D3D_PRIMITIVE_TOPOLOGY ePolygon,
		VertexShader::VERTEX_TYPE eVsType,
		PixelShader::PIXEL_TYPE ePsType);

	//デストラクタ
	virtual ~CanvasRenderer();

	//終了
	void Release(void);

	//描画
	void Draw(void);

private:
	int									m_nNumVertexPolygon;

	D3D_PRIMITIVE_TOPOLOGY				m_ePolygon;
};

#endif
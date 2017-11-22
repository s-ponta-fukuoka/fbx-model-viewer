//=============================================================================
//
// mesh_filed.h
// Author : SHOTA FUKUOKA
//
//=============================================================================
#ifndef _MESH_FIELD_H_
#define _MESH_FIELD_H_

//*****************************************************************************
// ヘッダファイル
//*****************************************************************************
#include "../mesh.h"

//*****************************************************************************
// ライブラリファイル
// [構成プロパティ]->[リンカー]->[入力]->[追加の依存ファイル]に記述しても可能
//*****************************************************************************

//*****************************************************************************
// マクロ定義
//*****************************************************************************
class ShaderManager;
class RenderManager;
class TextureManager;

//*****************************************************************************
// クラス定義
//*****************************************************************************
class MeshField : public Mesh
{
public:
	MeshField( RenderManager* pRenderManager,
				ShaderManager* pShaderManager,
				TextureManager* pTextureManager,
				AppRenderer::Constant* pConstant,
				AppRenderer::Constant* pLightCameraConstant);			//コンストラクタ

	~MeshField( );			//デストラクタ

	HRESULT Init( void );	//初期化処理

	void Release( void );	//終了処理

	void Update( void );	//更新処理

	void MakeVertex(void);

	float GetHeight(VECTOR3 Position);

private:
	VECTOR3				m_size;
	int					m_nNumVertex;
	int					m_nNumAllPolygon;
	VECTOR3				m_NumPolygon;
	VECTOR3				m_Polygon[15000];
	int					m_NumVertexPolygon;
};

#endif
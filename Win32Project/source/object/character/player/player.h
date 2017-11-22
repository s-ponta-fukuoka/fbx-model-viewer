//=============================================================================
//
// player.h
// Author : SHOTA FUKUOKA
//
//=============================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

//*****************************************************************************
// インクルード
//*****************************************************************************
#include <Windows.h>
#include "../character.h"
#include "../../../model/model_manager.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
class ShaderManager;
class RenderManager;
class TextureManager;
class ModelManager;

//*****************************************************************************
// クラス定義
//*****************************************************************************
class Player : public Character
{
public:

	//デストラクタ
	virtual ~Player();

	//インスタンス生成
	static void CreateInstance(RenderManager* pRenderManager,
		ShaderManager* pShaderManager,
		TextureManager* pTextureManager,
		ModelManager* pModelManager,
		AppRenderer::Constant* pConstant,
		AppRenderer::Constant* pLightCameraConstant);

	//インスタンス取得
	static Player* GetInstance(void);

	//初期化
	HRESULT Init(void);

	//終了
	void Release(void);

	//更新
	void Update(void);

	//バッファ作成
	void MakeVertex(int nMeshCount, SkinMeshModel::Mesh* pMesh);

	//保存
	void SaveFile(char* FileName);

	void LoadFile(char* FileName);

	typedef struct
	{
		int					nStartTime;

		int					nEndTime;

		bool				bStop;
	}ANIME_CLIP;

private:
	//コンストラクタ
	Player(RenderManager* pRenderManager,
		ShaderManager* pShaderManager,
		TextureManager* pTextureManager,
		ModelManager* pModelManager,
		AppRenderer::Constant* pConstant,
		AppRenderer::Constant* pLightCameraConstant);

	static Player*		m_pPlayer;

	ANIME_CLIP*			m_pAnimeClip;

	SkinMeshModel*		m_pModel;

	int*				m_pFrame;

	int*				m_pAnimeNumber;

	int					m_nOldStartTime;

	int					m_nOldEndTime;

	int					m_nNumClip;
};

#endif
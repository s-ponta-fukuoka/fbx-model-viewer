//=============================================================================
//
// model_manager.h
// Author : SHOTA FUKUOKA
//
//=============================================================================
#ifndef _MODEL_MANAGER_H_
#define _MODEL_MANAGER_H_

//*****************************************************************************
// ヘッダファイル
//*****************************************************************************
#include "model.h"

//*****************************************************************************
// クラス定義
//*****************************************************************************
class ModelManager
{
	std::list<SkinMeshModel*>  m_SkinMeshList;
public:

	//追加
	void AddSkinMeshModel(SkinMeshModel* model);

	//モデルデータ探索
	SkinMeshModel* SeekSkinMeshModel(SkinMeshModel* model);
	
	//スキンメッシュ削除
	void ReleasekSkinMeshModel(void);

	//全削除
	void ReleaseAll(void);
};

#endif

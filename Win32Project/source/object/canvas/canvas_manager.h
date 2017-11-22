//=============================================================================
//
// canvas_manager.h
// Author : shoto fukuoka
//
//=============================================================================
#ifndef _CANVAS_MANAGER_H_
#define _CANVAS_MANAGER_H_

//*****************************************************************************
// ヘッダファイル
//*****************************************************************************
#include <list>
#include "canvas.h"

//*****************************************************************************
// クラス定義
//*****************************************************************************
class CanvasManager
{
	std::list<Canvas*>		m_list;
public:

	//コンストラクタ
	CanvasManager();

	//デストラクタ
	virtual ~CanvasManager();

	//追加
	void AddCanvas(Canvas* canvas);

	//削除
	void DeleteCanvas(const Canvas* canvas);

	//全更新
	void UpdateAll(void);

	//全消去
	void ReleaseAll(void);
};

#endif

//=============================================================================
//
// 判定処理 [collider.h]
// Author : SHOTA FUKUOKA
//
//=============================================================================
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../renderer/app_renderer.h"

class TextureManager;

//*********************************************************
//クラス
//*********************************************************
class Texture
{
public:
	Texture(const char* name, TextureManager* m_pTextureManager);


	void LoadTexture(const char* name);
	void CreateTextureResource(void);
	const char *GetName();
	unsigned char *GetPixels();
	ID3D11ShaderResourceView *GetTexture(void);

private:
	ID3D11ShaderResourceView* m_pTexture;//テクスチャー
	const char* m_name;
	unsigned char* m_Pixels;
	int m_Width;
	int m_Height;
};

#endif
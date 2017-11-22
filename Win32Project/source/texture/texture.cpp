//=============================================================================
//
// [manager.h]
// Author : shota fukuoka
//
//=============================================================================
#include "texture.h"
#include "texture_manager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数:
//*****************************************************************************

Texture::Texture(const char* name, TextureManager* m_pTextureManager)
{
	m_name = name;
	m_Pixels = m_pTextureManager->GetTexture(this);
	if (m_Pixels == NULL)
	{
		LoadTexture(m_name);
		CreateTextureResource();
	}
	else
	{
		CreateTextureResource();
	}
}

const char *Texture::GetName(void)
{
	return m_name;
}

unsigned char *Texture::GetPixels(void)
{
	return m_Pixels;
}


ID3D11ShaderResourceView *Texture::GetTexture(void)
{
	return m_pTexture;
}

void Texture::CreateTextureResource(void)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();

	ID3D11Texture2D* pTex2D = NULL;

	D3D11_TEXTURE2D_DESC tex2D;
	memset(&tex2D, 0, sizeof(tex2D));
	tex2D.Width = m_Width;
	tex2D.Height = m_Height;
	tex2D.MipLevels = 1;
	tex2D.ArraySize = 1;
	tex2D.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	tex2D.SampleDesc.Count = 1;
	tex2D.SampleDesc.Quality = 0;
	tex2D.Usage = D3D11_USAGE_DEFAULT;
	tex2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex2D.CPUAccessFlags = 0;
	tex2D.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_Pixels;
	initData.SysMemPitch = m_Width * 4;
	initData.SysMemSlicePitch = 0;

	auto hr = pDevice->CreateTexture2D(&tex2D, &initData, &pTex2D);
	if (FAILED(hr)) {
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(pTex2D, &SRVDesc, &m_pTexture);
	if (FAILED(hr))
	{
		return;
	}
}

void Texture::LoadTexture(const char* name)
{
	m_Pixels = stbi_load(name, &m_Width, &m_Height, 0, STBI_rgb_alpha);
}
//=============================================================================
//
// [manager.h]
// Author : shota fukuoka
//
//=============================================================================
#include "shader.h"
#include "shader_manager.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数:
//*****************************************************************************

Shader::Shader()
{
}

VertexShader::VertexShader(TCHAR* csoName, VERTEX_TYPE vsType, ShaderManager* pShaderManager)
{
	m_csoName = csoName;
	m_eVsType = vsType;
	CreateVertexShader(m_csoName);
	pShaderManager->VSAdd(this);
}

VertexShader::VERTEX_TYPE VertexShader::GetType(void)
{
	return m_eVsType;
}

ID3D11VertexShader *VertexShader::GetVertexShader(void)
{
	return m_pVertexShader;
}

ID3D11InputLayout *VertexShader::GetVertexLayout(void)
{
	return m_pVertexLayout;
}

void VertexShader::CreateVertexShader(TCHAR* csoName)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();

	//バイナリファイルを読み込む//
	FILE* fp = _tfopen(csoName, _T("rb"));
	if (fp == NULL) { return; }
	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unsigned char* cso_data = new unsigned char[cso_sz];
	fread(cso_data, cso_sz, 1, fp);
	fclose(fp);

	//この時点でコンパイル後のバイナリはcso_dataに格納されている//

	// 頂点シェーダーオブジェクトの作成//
	HRESULT hr = pDevice->CreateVertexShader(cso_data, cso_sz, NULL, &m_pVertexShader);

	switch (m_eVsType)
	{
	case VS_NORMAL:
	case VS_3D:
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3 + 4 * 4, D3D11_INPUT_PER_VERTEX_DATA, 0 } };

		UINT numElements = sizeof(layout) / sizeof(layout[0]);

		//頂点インプットレイアウトを作成
		if (FAILED(pDevice->CreateInputLayout(layout, numElements, cso_data, cso_sz, &m_pVertexLayout))) { return; };

		break;
	}
	case VS_2D:
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 4, D3D11_INPUT_PER_VERTEX_DATA, 0 } };

		UINT numElements = sizeof(layout) / sizeof(layout[0]);

		//頂点インプットレイアウトを作成
		if (FAILED(pDevice->CreateInputLayout(layout, numElements, cso_data, cso_sz, &m_pVertexLayout))) { return; };

		break;
	}
	case VS_TOON:
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12 + 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12 + 12 + 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12 + 12 + 16 + 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12 + 12 + 16 + 8 + 16, D3D11_INPUT_PER_VERTEX_DATA, 0 } };

		UINT numElements = sizeof(layout) / sizeof(layout[0]);

		//頂点インプットレイアウトを作成
		if (FAILED(pDevice->CreateInputLayout(layout, numElements, cso_data, cso_sz, &m_pVertexLayout))) { return; };

		break;
	}
	default:
	{
		break;
	}
	}

	delete[] cso_data;
}

PixelShader::PixelShader(TCHAR* csoName, PIXEL_TYPE psType, ShaderManager* pShaderManager)
{
	m_ePsType = psType;
	m_csoName = csoName;
	CreatePixelShader(m_csoName);
	pShaderManager->PSAdd(this);
}

PixelShader::PIXEL_TYPE PixelShader::GetType(void)
{
	return m_ePsType;
}

ID3D11PixelShader *PixelShader::GetPixelShader(void)
{
	return m_pPixelShader;
}

void PixelShader::CreatePixelShader(TCHAR* csoName)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();

	//バイナリファイルを読み込む//
	FILE* fp = _tfopen(csoName, _T("rb"));
	if (fp == NULL) { return; }
	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unsigned char* cso_data = new unsigned char[cso_sz];
	fread(cso_data, cso_sz, 1, fp);
	fclose(fp);

	// シェーダーオブジェクトの作成//
	HRESULT hr = pDevice->CreatePixelShader(cso_data, cso_sz, NULL, &m_pPixelShader);

	delete[] cso_data;
}
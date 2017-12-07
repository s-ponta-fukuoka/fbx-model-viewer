//=============================================================================
//
// directxレンダリング処理 [rendereDX.cpp]
// Author : SHOTA FUKUOKA
//
//=============================================================================
#include "renderer.h"
#include "../shader/shader.h"
#include "../shader/shader_manager.h"
#include "../texture/texture_manager.h"
#include "../model/model.h"
#include "../app/app.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数:
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
///////////////////////////////////////////////////////////////////////////////
Renderer::Renderer()
:m_pBlendState(NULL)
,m_pSampleLinear(NULL)
,m_pTexture(NULL)
,m_pVertexBuffer(NULL)
,m_pConstantBuffer(NULL)
,m_pIndexBuffer(NULL)
,m_pTransform(NULL)
{
	;
}

MeshRenderer::MeshRenderer(ID3D11Buffer* pVertexBuffer,
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
							PixelShader::PIXEL_TYPE ePsType)
{
	m_ePolygon = ePolygon;

	m_pVertexBuffer = pVertexBuffer;

	m_pIndexBuffer = pIndexBuffer;

	m_pVertexShader = pShaderManager->GetVertexShader(eVsType);

	m_pPixelShader = pShaderManager->GetPixelShader(ePsType);

	m_pTransform = pTransform;

	m_pConstant = pConstant;

	m_pLightConstant = pLightConstant;

	m_nNumVertexPolygon = nNumVertexPolygon;

	m_pTexture = pTexture;

	m_pShadowMap = pShadowMap;

	ConfigConstantBuffer(sizeof(AppRenderer::Constant));

	ConfigSamplerState();
}

SkinnedMeshRenderer::SkinnedMeshRenderer(ID3D11Buffer* pVertexBuffer,
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
	SkinMeshModel::Mesh mesh)
{
	m_ePolygon = ePolygon;

	m_pVertexBuffer = pVertexBuffer;

	m_pIndexBuffer = pIndexBuffer;

	m_pVertexShader = pShaderManager->GetVertexShader(eVsType);

	m_pPixelShader = pShaderManager->GetPixelShader(ePsType);

	m_pTransform = pTransform;

	m_pConstant = pConstant;

	m_pLightConstant = pLightConstant;

	m_nNumVertexPolygon = nNumVertexPolygon;

	m_pFrame = pFrame;

	m_pAnimeNumber = pAnimeNumber;

	m_pTexture = pTexture;

	m_pShadowMap = pShadowMap;

	m_pCluster = pCluster;

	m_mesh = mesh;

	ConfigConstantBuffer(sizeof(SkinMeshModel::ModelConstant));

	ConfigSamplerState();

	ConfigBlendState();
}

CanvasRenderer::CanvasRenderer(ID3D11Buffer* pVertexBuffer,
	ID3D11Buffer* pIndexBuffer,
	ShaderManager* pShaderManager,
	ID3D11ShaderResourceView* pTexture,
	int	nNumVertexPolygon,
	D3D_PRIMITIVE_TOPOLOGY ePolygon,
	VertexShader::VERTEX_TYPE eVsType,
	PixelShader::PIXEL_TYPE ePsType)
{
	m_ePolygon = ePolygon;

	m_pVertexBuffer = pVertexBuffer;

	m_pIndexBuffer = pIndexBuffer;

	m_pVertexShader = pShaderManager->GetVertexShader(eVsType);

	m_pPixelShader = pShaderManager->GetPixelShader(ePsType);

	m_nNumVertexPolygon = nNumVertexPolygon;

	m_pTexture = pTexture;

	ConfigSamplerState();
}

///////////////////////////////////////////////////////////////////////////////
// デストラクタ
///////////////////////////////////////////////////////////////////////////////
Renderer::~Renderer()
{
	;
}

MeshRenderer::~MeshRenderer()
{
	;
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
	;
}

CanvasRenderer::~CanvasRenderer()
{
	;
}

///////////////////////////////////////////////////////////////////////////////
// 終了
///////////////////////////////////////////////////////////////////////////////
void Renderer::Release(void)
{
	SAFE_RELEASE(m_pBlendState);

	SAFE_RELEASE(m_pSampleLinear);

	SAFE_RELEASE(m_pTexture);

	SAFE_RELEASE(m_pVertexBuffer);

	SAFE_RELEASE(m_pConstantBuffer);

	SAFE_RELEASE(m_pIndexBuffer);
}

void MeshRenderer::Release()
{
	SAFE_RELEASE(m_pShadowMap);

	Renderer::Release();
}

void SkinnedMeshRenderer::Release()
{
	SAFE_RELEASE(m_pShadowMap);

	Renderer::Release();
}

void CanvasRenderer::Release()
{
	Renderer::Release();
}


///////////////////////////////////////////////////////////////////////////////
//定数バッファ設定
///////////////////////////////////////////////////////////////////////////////
void Renderer::ConfigConstantBuffer(UINT ByteWidth)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();

	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = ByteWidth;
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.StructureByteStride = sizeof(float);
	pDevice->CreateBuffer(&cb, NULL, &m_pConstantBuffer);
}

///////////////////////////////////////////////////////////////////////////////
//サンプラーステート設定
///////////////////////////////////////////////////////////////////////////////
void Renderer::ConfigSamplerState(void)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();

	//テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pDevice->CreateSamplerState(&SamDesc, &m_pSampleLinear);
}

///////////////////////////////////////////////////////////////////////////////
//ブレンドステート設定
///////////////////////////////////////////////////////////////////////////////
void Renderer::ConfigBlendState(void)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();

	D3D11_BLEND_DESC BlendStateDesc;
	BlendStateDesc.AlphaToCoverageEnable = FALSE;
	BlendStateDesc.IndependentBlendEnable = FALSE;
	for (int i = 0; i < 8; i++)
	{
		BlendStateDesc.RenderTarget[i].BlendEnable = FALSE;
		BlendStateDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		BlendStateDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		BlendStateDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		BlendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		BlendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		BlendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		BlendStateDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	pDevice->CreateBlendState(&BlendStateDesc, &m_pBlendState);
}

///////////////////////////////////////////////////////////////////////////////
// 描画
///////////////////////////////////////////////////////////////////////////////
void MeshRenderer::Draw(void)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pAppRenderer->GetDeviceContex();

	//バーテックスバッファーをセット
	UINT stride = sizeof(AppRenderer::Vertex3D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	
	//プリミティブ・トポロジーをセット
	pDeviceContext->IASetPrimitiveTopology(m_ePolygon);

	//頂点インプットレイアウトをセット
	pDeviceContext->IASetInputLayout(m_pVertexShader->GetVertexLayout());

	AppRenderer::Constant hConstant;

	XMMATRIX hWorld = XMMatrixIdentity();
	XMMATRIX hPosition = XMMatrixTranslation(m_pTransform->position.x, m_pTransform->position.y, m_pTransform->position.z);
	XMMATRIX hRotate = XMMatrixRotationRollPitchYaw(D3DToRadian(m_pTransform->rot.x), D3DToRadian(m_pTransform->rot.y), D3DToRadian(m_pTransform->rot.z));
	XMMATRIX hScaling = XMMatrixScaling(1, 1, 1);

	hWorld = XMMatrixMultiply(hWorld, hScaling);
	hWorld = XMMatrixMultiply(hWorld, hRotate);
	hWorld = XMMatrixMultiply(hWorld, hPosition);

	XMMATRIX hView = m_pConstant->view;
	XMMATRIX hProj = m_pConstant->projection;

	hConstant.world = XMMatrixTranspose(hWorld);

	hConstant.view = XMMatrixTranspose(hView);

	hConstant.projection = XMMatrixTranspose(hProj);

	if (m_pLightConstant != NULL)
	{
		XMMATRIX hLightView = m_pLightConstant->view;
		XMMATRIX hLightProj = m_pLightConstant->projection;

		hConstant.lightView = XMMatrixTranspose(hLightView);

		hConstant.light = m_pConstant->light;

		XMMATRIX mat = XMMatrixTranspose(hWorld * hLightView * hLightProj);
		hConstant.lightProjection = mat;
	}

	pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &hConstant, 0, 0);

	//コンテキストに設定
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	//使用するシェーダーの登録
	pDeviceContext->VSSetShader(m_pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader->GetPixelShader(), NULL, 0);
	//テクスチャーをシェーダーに渡す
	pDeviceContext->PSSetSamplers(0, 1, &m_pSampleLinear);
	pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture);

	if (m_pShadowMap != NULL)
	{
		pDeviceContext->PSSetShaderResources(1, 1, &m_pShadowMap);
	}

	if (m_pIndexBuffer != NULL)
	{
		//そのインデックスバッファをコンテキストに設定
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		pDeviceContext->DrawIndexed(m_nNumVertexPolygon, 0, 0);
	}
	else
	{
		pDeviceContext->Draw(m_nNumVertexPolygon, 0);
	}

}

void SkinnedMeshRenderer::Draw()
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pAppRenderer->GetDeviceContex();

	ID3D11RenderTargetView* pRenderTarget = pAppRenderer->GetRenderTargetView();
	ID3D11DepthStencilView* pDepthStencil = pAppRenderer->GetDepthStencilView();

	//pDeviceContext->OMSetRenderTargets(1, &pRenderTarget, NULL);

	//ブレンディングをコンテキストに設定
	float blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	pDeviceContext->OMSetBlendState(m_pBlendState, blendFactor, 0xffffffff);

	//バーテックスバッファーをセット
	UINT stride = sizeof(SkinMeshModel::ModelVertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//プリミティブ・トポロジーをセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点インプットレイアウトをセット
	pDeviceContext->IASetInputLayout(m_pVertexShader->GetVertexLayout());

	XMMATRIX hLcl = XMMatrixIdentity();
	XMMATRIX hLclPosition = XMMatrixTranslation(m_mesh.LclPos.x, m_mesh.LclPos.y, m_mesh.LclPos.z);
	XMMATRIX hLclRotate = XMMatrixRotationRollPitchYaw(D3DToRadian(m_mesh.LclRot.x), D3DToRadian(m_mesh.LclRot.y), D3DToRadian(m_mesh.LclRot.z));
	XMMATRIX hLclScaling = XMMatrixScaling(m_mesh.LclScl.x, m_mesh.LclScl.y, m_mesh.LclScl.z);

	hLcl = XMMatrixMultiply(hLcl, hLclScaling);
	hLcl = XMMatrixMultiply(hLcl, hLclRotate);
	hLcl = XMMatrixMultiply(hLcl, hLclPosition);

	SkinMeshModel::ModelConstant hConstant;

	XMMATRIX hWorld = XMMatrixIdentity();
	XMMATRIX hPosition = XMMatrixTranslation(m_pTransform->position.x, m_pTransform->position.y, m_pTransform->position.z);
	XMMATRIX hRotate = XMMatrixRotationRollPitchYaw(D3DToRadian(m_pTransform->rot.x), D3DToRadian(m_pTransform->rot.y), D3DToRadian(m_pTransform->rot.z));
	XMMATRIX hScaling = XMMatrixScaling(1, 1, 1);

	hWorld = XMMatrixMultiply(hWorld, hScaling);
	hWorld = XMMatrixMultiply(hWorld, hRotate);
	hWorld = XMMatrixMultiply(hWorld, hPosition);

	XMMATRIX hView = m_pConstant->view;
	XMMATRIX hProj = m_pConstant->projection;

	hConstant.world = XMMatrixTranspose(hWorld);

	hConstant.view = XMMatrixTranspose(hView);

	hConstant.projection = XMMatrixTranspose(hProj);

	hConstant.lclCluster = XMMatrixTranspose(hLcl);

	if (m_pLightConstant != NULL)
	{
		XMMATRIX hLightView = m_pLightConstant->view;
		XMMATRIX hLightProj = m_pLightConstant->projection;

		hConstant.lightView = XMMatrixTranspose(hLightView);

		hConstant.light = m_pConstant->light;

		XMMATRIX mat = XMMatrixTranspose(hWorld * hLightView * hLightProj);
		hConstant.lightProjection = mat;
	}

	for (int i = 0; i < m_mesh.nNumCluster; i++)
	{
		hConstant.cluster[i] = m_pCluster[i].pMatrix[m_pAnimeNumber[0]][m_pFrame[0]];
	}

	pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &hConstant, 0, 0);

	//コンテキストに設定
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//使用するシェーダーの登録
	pDeviceContext->VSSetShader(m_pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader->GetPixelShader(), NULL, 0);

	//テクスチャーをシェーダーに渡す
	pDeviceContext->PSSetSamplers(0, 1, &m_pSampleLinear);
	
	//pDeviceContext->PSSetShaderResources(1, 1, &m_ToonTexture);
	if (m_pTexture != NULL)
	{
		pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture);
	}

	if (m_pShadowMap != NULL)
	{
		//pDeviceContext->PSSetShaderResources(1, 1, &m_pShadowMap);
	}

	if (m_pIndexBuffer != NULL)
	{
		//そのインデックスバッファをコンテキストに設定
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		pDeviceContext->DrawIndexed(m_nNumVertexPolygon, 0, 0);
	}
	else
	{
		pDeviceContext->Draw(m_nNumVertexPolygon, 0);
	}

	pDeviceContext->OMSetRenderTargets(1, &pRenderTarget, pDepthStencil);
}

void CanvasRenderer::Draw(void)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();

	ID3D11Device* pDevice = pAppRenderer->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pAppRenderer->GetDeviceContex();

	//バーテックスバッファーをセット
	UINT stride = sizeof(AppRenderer::Vertex2D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//プリミティブ・トポロジーをセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//頂点インプットレイアウトをセット
	pDeviceContext->IASetInputLayout(m_pVertexShader->GetVertexLayout());

	//コンテキストに設定
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	//使用するシェーダーの登録
	pDeviceContext->VSSetShader(m_pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader->GetPixelShader(), NULL, 0);

	//テクスチャーをシェーダーに渡す
	pDeviceContext->PSSetSamplers(0, 1, &m_pSampleLinear);
	pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture);

	if (m_pIndexBuffer != NULL)
	{
		//そのインデックスバッファをコンテキストに設定
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		pDeviceContext->DrawIndexed(m_nNumVertexPolygon, 0, 0);
	}
	else
	{
		pDeviceContext->Draw(m_nNumVertexPolygon, 0);
	}
}
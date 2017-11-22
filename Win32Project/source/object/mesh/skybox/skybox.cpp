//=============================================================================
//
// [scene2D.cpp]
// Author : shota fukuoka
//
//=============================================================================
#include "skybox.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SKY_BOX_VERTEX (34)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数:
//*****************************************************************************

//=============================================================================
// コンストラクタ
//=============================================================================
SkyBox::SkyBox(RenderManager* pRenderManager,
	ShaderManager* pShaderManager,
	TextureManager* pTextureManager,
	AppRenderer::Constant* pConstant)
{
	m_pVertexBuffer = NULL;
	m_pos = VECTOR3(-10000.0f, 10000.0f, 10000.0f);
	m_vertex.normal = VECTOR3(0.0f, 0.0f, 0.0f);			// ポリゴンの位置
	m_size = VECTOR3(20000.0f, 20000.0f, 100.0f);			// ポリゴンの位置

	MakeVertex();

	Texture* pTexture = new Texture("resource/sprite/skybox_000.png", pTextureManager);

	pRenderManager->AddRenderer(new MeshRenderer(m_pVertexBuffer,
		NULL,
		pShaderManager,
		pTexture->GetTexture(),
		NULL,
		m_pTransform,
		pConstant,
		NULL,
		SKY_BOX_VERTEX,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		VertexShader::VS_3D,
		PixelShader::PS_3D));

	delete[] pTexture;
	pTexture = NULL;
}

//=============================================================================
// デストラクタ
//=============================================================================
SkyBox::~SkyBox()
{
}

void SkyBox::MakeVertex(void)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pAppRenderer->GetDeviceContex();

	//四角形
	AppRenderer::Vertex3D vertices[] =
	{
		VECTOR3(m_pos.x + m_size.x, m_pos.y,-m_pos.z), VECTOR3(0,0,0), VECTOR4(0,0,0,1), VECTOR2(0.75,0.333),
		VECTOR3(m_pos.x, m_pos.y,-m_pos.z),VECTOR3(0,0,0), VECTOR4(0,0,0,1), VECTOR2(1,0.333),
		VECTOR3(m_pos.x + m_size.x, m_pos.y - m_size.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1) , VECTOR2(0.75,0.666),
		VECTOR3(m_pos.x, m_pos.y - m_size.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1), VECTOR2(1,0.666),
		VECTOR3(m_pos.x, m_pos.y - m_size.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1), VECTOR2(1,0.666),

		VECTOR3(m_pos.x,m_pos.y,m_pos.z),VECTOR3(0,0,0), VECTOR4(0,0,0,1), VECTOR2(0.25,0.333),
		VECTOR3(m_pos.x,m_pos.y,m_pos.z),VECTOR3(0,0,0), VECTOR4(0,0,0,1), VECTOR2(0.25,0.333),
		VECTOR3(m_pos.x + m_size.x,m_pos.y,m_pos.z), VECTOR3(0,0,0), VECTOR4(0,0,0,1), VECTOR2(0.5,0.333333333),
		VECTOR3(m_pos.x,m_pos.y - m_size.y,m_pos.z), VECTOR3(0,0,0), VECTOR4(0,0,0,1), VECTOR2(0.25,0.666666666),
		VECTOR3(m_pos.x + m_size.x,m_pos.y - m_size.y,m_pos.z),  VECTOR3(0,0,0), VECTOR4(0,0,0,1), VECTOR2(0.5,0.666666666),
		VECTOR3(m_pos.x + m_size.x,m_pos.y - m_size.y,m_pos.z),  VECTOR3(0,0,0), VECTOR4(0,0,0,1), VECTOR2(0.5,0.666666666),
		
		VECTOR3(m_pos.x,m_pos.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0,0.333333333),
		VECTOR3(m_pos.x,m_pos.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0,0.333333333),
		VECTOR3(m_pos.x,m_pos.y,m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.25,0.333333333),
		VECTOR3(m_pos.x,m_pos.y - m_size.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0,0.666666666),
		VECTOR3(m_pos.x,m_pos.y - m_size.y,m_pos.z),  VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.25,0.666666666),
		VECTOR3(m_pos.x,m_pos.y - m_size.y,m_pos.z),  VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.25,0.666666666),
		
		VECTOR3(m_pos.x + m_size.x,m_pos.y,m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.5,0.333333333),
		VECTOR3(m_pos.x + m_size.x,m_pos.y,m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.5,0.333333333),
		VECTOR3(m_pos.x + m_size.x,m_pos.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.75,0.333333333),
		VECTOR3(m_pos.x + m_size.x,m_pos.y - m_size.y,m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.5,0.666666666),
		VECTOR3(m_pos.x + m_size.x,m_pos.y - m_size.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1), VECTOR2(0.75,0.666666666),
		VECTOR3(m_pos.x + m_size.x,m_pos.y - m_size.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1), VECTOR2(0.75,0.666666666),
		
		VECTOR3(m_pos.x,m_pos.y - m_size.y,m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.25,0.666),
		VECTOR3(m_pos.x,m_pos.y - m_size.y,m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.25,0.666),
		VECTOR3(m_pos.x + m_size.x,m_pos.y - m_size.y,m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.5,0.666),
		VECTOR3(m_pos.x,m_pos.y - m_size.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.25,0.999),
		VECTOR3(m_pos.x + m_size.x,m_pos.y - m_size.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1), VECTOR2(0.5,0.999),
		VECTOR3(m_pos.x + m_size.x,m_pos.y - m_size.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1), VECTOR2(0.5,0.999),
		
		VECTOR3(m_pos.x,m_pos.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.25,0.0),
		VECTOR3(m_pos.x,m_pos.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.25,0.0),
		VECTOR3(m_pos.x + m_size.x,m_pos.y,-m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.5,0.0),
		VECTOR3(m_pos.x,m_pos.y,m_pos.z), VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.25,0.333),
		VECTOR3(m_pos.x + m_size.x,m_pos.y,m_pos.z),  VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.5,0.333),
		VECTOR3(m_pos.x + m_size.x,m_pos.y,m_pos.z),  VECTOR3(0,0,0),VECTOR4(0,0,0,1),VECTOR2(0.5,0.333),
	};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(AppRenderer::Vertex3D) * SKY_BOX_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer)))
	{
		return;
	}
}


//=============================================================================
// ポリゴンの終了処理
//=============================================================================
void SkyBox::Release(void)
{
	Mesh::Release();
}

//=============================================================================
// ポリゴンの更新処理
//=============================================================================
void SkyBox::Update(void)
{
}
//=============================================================================
//
// [scene2D.cpp]
// Author : shota fukuoka
//
//=============================================================================
#include <stdio.h>
#include <tchar.h>
#include "mesh_field.h"
#include "../../../renderer/render_manager.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MESH_FILED_POSITION_X (-4000.0f)
#define MESH_FILED_POSITION_Z (4000.0f)
#define MESH_FILED_SIZE_X (80)
#define MESH_FILED_SIZE_Z (80)
#define MESH_FILED_POLYGON_X (100)
#define MESH_FILED_POLYGON_Z (100)
#define MESH_FILED_VERTEX	((2 + 2 * MESH_FILED_POLYGON_X) * MESH_FILED_POLYGON_Z + (MESH_FILED_POLYGON_X - 1) * 2)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数:
//*****************************************************************************

//=============================================================================
// コンストラクタ
//=============================================================================
MeshField::MeshField(RenderManager* pRenderManager, ShaderManager* pShaderManager, TextureManager* pTextureManager, AppRenderer::Constant* pConstant, AppRenderer::Constant* pLightCameraConstant)
{
	m_pVertexBuffer = NULL;

	m_pIndexBuffer = NULL;

	m_size = VECTOR3( 0.0f, 0.0f, 0.0f );

	m_vertex.position = VECTOR3(-4000.0f, 0.0f, 4000.0f);
	m_vertex.tex = VECTOR2(0.0f, 0.0f);
	m_size = VECTOR3(80, 0, 80);
	m_vertex.normal = VECTOR3(0, 1, 0);
	m_NumPolygon = VECTOR3(100.0f, 0.0f, 100.0f);

	m_NumVertexPolygon = (2 + 2 * m_NumPolygon.x) * m_NumPolygon.z + (m_NumPolygon.x - 1) * 2;
	m_nNumAllPolygon = m_NumVertexPolygon - 2;
	m_nNumVertex = (m_NumPolygon.x + 1) * (m_NumPolygon.z + 1);

	MakeVertex();

	Texture* pTexture = new Texture("resource/sprite/field000.jpg", pTextureManager);


	pRenderManager->AddRenderer(new MeshRenderer(m_pVertexBuffer,
												m_pIndexBuffer,
												pShaderManager,
												pTexture->GetTexture(),
												pRenderManager->GetShadowTexture(),
												m_pTransform,
												pConstant,
												pLightCameraConstant,
												MESH_FILED_VERTEX,
												D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
												VertexShader::VS_NORMAL,
												PixelShader::PS_NORMAL));

	pRenderManager->AddShadowRenderer(new MeshRenderer(m_pVertexBuffer,
												m_pIndexBuffer,
												pShaderManager,
												pTexture->GetTexture(),
												NULL,
												m_pTransform,
												pLightCameraConstant,
												NULL,
												MESH_FILED_VERTEX,
												D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
												VertexShader::VS_NORMAL,
												PixelShader::PS_SHADOW));

	delete[] pTexture;
	pTexture = NULL;
}

//=============================================================================
// デストラクタ
//=============================================================================
MeshField::~MeshField()
{
}

//=============================================================================
// ポリゴンの初期化処理
//=============================================================================
HRESULT MeshField::Init( void )
{
	m_vertex.position = VECTOR3(-4000.0f, 0.0f, 4000.0f);
	m_vertex.tex = VECTOR2(0.0f, 0.0f);
	m_size = VECTOR3(80, 0, 80);
	m_vertex.normal = VECTOR3(0, 1, 0);
	m_NumPolygon = VECTOR3(100.0f, 0.0f, 100.0f);

	m_NumVertexPolygon = (2 + 2 * m_NumPolygon.x) * m_NumPolygon.z + (m_NumPolygon.x - 1) * 2;
	m_nNumAllPolygon = m_NumVertexPolygon - 2;
	m_nNumVertex = (m_NumPolygon.x + 1) * (m_NumPolygon.z + 1);

	MakeVertex();


	return S_OK;
}

//=============================================================================
// ポリゴンの終了処理
//=============================================================================
void MeshField::Release(void)
{
	Mesh::Release();
}

//=============================================================================
// ポリゴンの更新処理
//=============================================================================
void MeshField::Update( void )
{

}

void MeshField::MakeVertex(void)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pAppRenderer->GetDeviceContex();

	int nCnt = 0;
	int PosCnt = 0;
	int NrmCnt = 0;
	int y = 0;

	VECTOR3	 Position[101][101];
	VECTOR3	 Normal[101][101];
	VECTOR3 directionX, directionZ;
	VECTOR3 normalX, normalZ;

	AppRenderer::Vertex3D* vertices = new AppRenderer::Vertex3D[m_nNumVertex];
	for (int z = 0; z < (m_NumPolygon.z + 1); z++)
	{
		for (int x = 0; x < (m_NumPolygon.x + 1); x++, PosCnt++)
		{
			if (PosCnt != ((m_NumPolygon.x + 1) * (m_NumPolygon.z + 1)))
			{
				y = 0;
				vertices[PosCnt].position = VECTOR3(m_vertex.position.x + (m_size.x * x), m_vertex.position.y + y, m_vertex.position.z - (m_size.z * z));
				m_Polygon[PosCnt] = VECTOR3(m_vertex.position.x + (m_size.x * x), m_vertex.position.y + y, m_vertex.position.z - (m_size.z * z));
				Position[z][x] = VECTOR3(m_vertex.position.x + (m_size.x * x), m_vertex.position.y + y, m_vertex.position.z - (m_size.z * z));
			}
		}
	}


	for (int z = 0; z < (m_NumPolygon.z + 1); z++)
	{
		for (int x = 0; x < (m_NumPolygon.x + 1); x++, PosCnt++)
		{
			directionX.x = Position[z][x + 1].x - Position[z][x - 1].x;
			directionX.y = Position[z][x + 1].y - Position[z][x - 1].y;
			directionX.z = Position[z][x + 1].z - Position[z][x - 1].z;
			normalX.x = -directionX.y;
			normalX.y = directionX.x;
			normalX.z = 0.0f;

			directionZ.x = Position[z + 1][x].x - Position[z - 1][x].x;
			directionZ.y = Position[z + 1][x].y - Position[z - 1][x].y;
			directionZ.z = Position[z + 1][x].z - Position[z - 1][x].z;
			normalZ.x = 0.0f;
			normalZ.y = directionX.z;
			normalZ.z = -directionX.y;

			VECTOR3 normal;
			normal.x = normalX.x + normalZ.x;
			normal.y = normalX.y + normalZ.y;
			normal.z = normalX.z + normalZ.z;

			XMVector3Normalize((XMVECTOR&)normal);

			Normal[z][x] = normal;
		}
	}

	for (int z = 0; z < (m_NumPolygon.z + 1); z++)
	{
		for (int x = 0; x < (m_NumPolygon.x + 1); x++, NrmCnt++)
		{
			if (PosCnt != ((m_NumPolygon.x + 1) * (m_NumPolygon.z + 1)))
			{
				vertices[NrmCnt].normal = VECTOR3(Normal[z][x].x, Normal[z][x].y, Normal[z][x].z);
			}
		}
	}

	for (int nLoop = 0; nLoop < ((m_NumPolygon.x + 1) * (m_NumPolygon.z + 1)); nLoop++)
	{
		vertices[nLoop].color = VECTOR4(255, 255, 255, 255);
	}

	for (int nLoop = 0; nLoop < (m_NumPolygon.z + 1); nLoop++)
	{
		for (int nLoop2 = 0; nLoop2 < (m_NumPolygon.x + 1); nLoop2++, nCnt++)
		{
			if (nCnt != ((m_NumPolygon.x + 1) * (m_NumPolygon.z + 1)))
			{
				vertices[nCnt].tex = VECTOR2(nLoop2, nLoop);
			}
		}
	}

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(AppRenderer::Vertex3D) * m_nNumVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	pDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);

	WORD* hIndexData = new WORD[m_NumVertexPolygon];

	int nCntIdx = 0;
	int nNo = 0;

	for (int nLoop = 0; nLoop < m_NumPolygon.z; nLoop++)
	{
		if (nCntIdx != 0)
		{
			hIndexData[nCntIdx] = (nLoop + 1) * (m_NumPolygon.z + 1) + 0;
			nCntIdx++;
		}
		hIndexData[nCntIdx] = (nLoop + 1) * (m_NumPolygon.z + 1) + 0;
		nCntIdx++;

		hIndexData[nCntIdx] = nLoop * (m_NumPolygon.z + 1) + 0;
		nCntIdx++;
		for (int nLoop2 = 0; nLoop2 < m_NumPolygon.x; nLoop2++)
		{
			hIndexData[nCntIdx] = (nLoop + 1) * (m_NumPolygon.z + 1) + nLoop2 + 1;
			nCntIdx++;
			hIndexData[nCntIdx] = nLoop * (m_NumPolygon.z + 1) + nLoop2 + 1;
			if (nCntIdx != m_NumVertexPolygon - 1)
			{
				nCntIdx++;
			}
		}
		if (nCntIdx != m_NumVertexPolygon - 1)

		{
			hIndexData[nCntIdx] = nLoop * (m_NumPolygon.z + 1) + m_NumPolygon.z;
			nCntIdx++;
		}
	}

	//インデックスバッファ作成
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(WORD) * m_NumVertexPolygon;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(WORD);
	
	D3D11_SUBRESOURCE_DATA hSubResourceData;
	hSubResourceData.pSysMem = hIndexData;
	hSubResourceData.SysMemPitch = 0;
	hSubResourceData.SysMemSlicePitch = 0;
	
	pDevice->CreateBuffer(&hBufferDesc, &hSubResourceData, &m_pIndexBuffer);

	delete[] vertices;
	delete[] hIndexData;
}

float MeshField::GetHeight(VECTOR3 Position)
{
	VECTOR3 pos;
	int x, z = 0;

	pos = Position - (m_vertex.position);

	x = pos.x / m_size.x;
	z = -(pos.z / m_size.z);

	VECTOR3 p0, p1, p2, p3,
		v01, v0p, vc0,
		v12, v1p, vc1,
		v20, v2p, vc2,
		v32, v3p, vc3,
		v21, v4p, vc4,
		v13, v5p, vc5;
	VECTOR3 vn = VECTOR3(0, 0, 0);

	int Lu, Ld, Ru, Rd = 0;

	Lu = (m_NumPolygon.x + 1) * z + x;
	Ld = (m_NumPolygon.x + 1) * (z + 1) + x;
	Ru = ((m_NumPolygon.x + 1) * z + x) + 1;
	Rd = ((m_NumPolygon.x + 1) * (z + 1) + x) + 1;

	p0 = m_Polygon[Ld];
	p1 = m_Polygon[Lu];
	p2 = m_Polygon[Rd];
	p3 = m_Polygon[Ru];

	v01 = p1 - p0;
	v0p = Position - p0;
	VECTOR3::Cross(&vc0, &v01, &v0p);

	v12 = p2 - p1;
	v1p = Position - p1;
	VECTOR3::Cross(&vc1, &v12, &v1p);

	v20 = p0 - p2;
	v2p = Position - p2;
	VECTOR3::Cross(&vc2, &v20, &v2p);

	v32 = p2 - p3;
	v3p = Position - p3;
	VECTOR3::Cross(&vc3, &v32, &v3p);

	v21 = p1 - p2;
	v4p = Position - p2;
	VECTOR3::Cross(&vc4, &v21, &v4p);

	v13 = p3 - p1;
	v5p = Position - p1;
	VECTOR3::Cross(&vc5, &v13, &v5p);

	if (vc1.y >= 0.0f)
	{
		VECTOR3 v02 = p2 - p0;
		VECTOR3::Cross(&vn, &v01, &v02);
		VECTOR3::Normalize(&vn, &vn);
		Position.y = p0.y - ((Position.x - p0.x) * vn.x + (Position.z - p0.z) * vn.z) / vn.y;
		return Position.y;
	}
	if (vc1.y <= 0.0f)
	{
		VECTOR3 v02 = p1 - p3;
		VECTOR3::Cross(&vn, &v32, &v02);
		VECTOR3::Normalize(&vn, &vn);
		Position.y = p0.y - ((Position.x - p3.x) * vn.x + (Position.z - p3.z) * vn.z) / vn.y;
		return Position.y;
	}
	return Position.y;
}

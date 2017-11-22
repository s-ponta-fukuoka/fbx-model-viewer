//=============================================================================
//
// player.cpp
// Author : SHOTA FUKUOKA
//
//=============================================================================

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "player.h"
#include "../../../model/model.h"
#include "../../../model/model_manager.h"
#include "../../../renderer/render_manager.h"
#include "../../../device/input.h"
#include "../../../gui/imgui.h"
#include "../../../gui/imgui_impl_dx11.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ANIME_CLIP_MAX (10)

Player* Player::m_pPlayer = NULL;

///////////////////////////////////////////////////////////////////////////////
//コンストラクタ
///////////////////////////////////////////////////////////////////////////////
Player::Player(RenderManager* pRenderManager,
	ShaderManager* pShaderManager,
	TextureManager* pTextureManager,
	ModelManager*	pModelManager,
	AppRenderer::Constant* pConstant,
	AppRenderer::Constant* pLightCameraConstant)
	: m_pModel(NULL)
	, m_pFrame(NULL)
	,m_nOldStartTime(0)
	,m_nOldEndTime(0)
	, m_nNumClip(1)
{
	m_pModel = new SkinMeshModel("resource/model/naka.fbx");
	m_pModel = pModelManager->SeekSkinMeshModel(m_pModel);

	SkinMeshModel::Mesh* pMesh = m_pModel->GetMesh();

	m_pFrame = new int();

	m_pAnimeNumber = new int();

	m_pAnimeClip = new ANIME_CLIP[ANIME_CLIP_MAX];

	PixelShader::PIXEL_TYPE ePsType;

	for (int i = 0; i < m_pModel->GetNumMesh(); i++)
	{

		MakeVertex(i, pMesh);

		ID3D11ShaderResourceView* pTextureResource = NULL;

		if (pMesh[i].pFileName != NULL)
		{
			ePsType = PixelShader::PS_TOON;
			Texture* pTexture = new Texture(pMesh[i].pFileName, pTextureManager);
			pTextureResource = pTexture->GetTexture();
		}
		else
		{
			ePsType = PixelShader::PS_MAT;
		}

		pRenderManager->AddRenderer(new SkinnedMeshRenderer(m_pVertexBuffer,
			m_pIndexBuffer,
			pShaderManager,
			pTextureResource,
			pRenderManager->GetShadowTexture(),
			m_pTransform,
			pConstant,
			pLightCameraConstant,
			pMesh[i].nNumPolygonVertex,
			m_pFrame,
			m_pAnimeNumber,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			VertexShader::VS_TOON,
			ePsType,
			pMesh[i].pCluster,
			pMesh[i]));

		pRenderManager->AddShadowRenderer(new SkinnedMeshRenderer(m_pVertexBuffer,
			m_pIndexBuffer,
			pShaderManager,
			pTextureResource,
			NULL,
			m_pTransform,
			pLightCameraConstant,
			NULL,
			pMesh[i].nNumPolygonVertex,
			m_pFrame,
			m_pAnimeNumber,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			VertexShader::VS_TOON,
			ePsType = PixelShader::PS_SHADOW,
			pMesh[i].pCluster,
			pMesh[i]));
	}

	m_pAnimeClip[0].nStartTime = m_pModel->GetAnime()->nStartTime;
	m_pAnimeClip[0].nEndTime = m_pModel->GetAnime()->nEndTime;

	for (int i = 0; i < ANIME_CLIP_MAX; i++)
	{
		m_pAnimeClip[i].bStop = false;
	}

	m_nOldStartTime = m_pModel->GetAnime()->nStartTime;
	m_nOldEndTime = m_pModel->GetAnime()->nEndTime;
}

///////////////////////////////////////////////////////////////////////////////
//デストラクタ
///////////////////////////////////////////////////////////////////////////////
Player::~Player()
{
	delete[] m_pAnimeClip;
	m_pAnimeClip = NULL;

	Release();
}

///////////////////////////////////////////////////////////////////////////////
//インスタンス生成
///////////////////////////////////////////////////////////////////////////////
void Player::CreateInstance(RenderManager* pRenderManager,
	ShaderManager* pShaderManager,
	TextureManager* pTextureManager,
	ModelManager* pModelManager,
	AppRenderer::Constant* pConstant,
	AppRenderer::Constant* pLightCameraConstant)
{
	if (m_pPlayer != NULL) { return; }
	m_pPlayer = new Player(pRenderManager, 
		pShaderManager, 
		pTextureManager, 
		pModelManager, 
		pConstant, 
		pLightCameraConstant);
}

///////////////////////////////////////////////////////////////////////////////
//インスタンス取得
///////////////////////////////////////////////////////////////////////////////
Player* Player::GetInstance(void)
{
	return m_pPlayer;
}

///////////////////////////////////////////////////////////////////////////////
//初期化
///////////////////////////////////////////////////////////////////////////////
HRESULT Player::Init(void)
{
	Character::Init();
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//終了
///////////////////////////////////////////////////////////////////////////////
void Player::Release(void)
{
	delete m_pModel;
	delete m_pFrame;
	delete m_pAnimeNumber;

	Character::Release();
}

///////////////////////////////////////////////////////////////////////////////
//更新
///////////////////////////////////////////////////////////////////////////////
void Player::Update(void)
{
	Object::Update(); 
	
	static char str[10][256];

	ImGui::Begin("anime_config");
	{
		if (ImGui::SliderInt("Animation", &m_pFrame[0], m_nOldStartTime, m_nOldEndTime - 1))
		{
			m_pModel->GetAnime()->nStartTime = m_pAnimeClip[0].nStartTime;
			m_pModel->GetAnime()->nEndTime = m_pAnimeClip[0].nEndTime;
		}

		if (ImGui::Button("BACK"))
		{
			m_pFrame[0]--;
		}

		if (ImGui::Button("NEXT"))
		{
			m_pFrame[0]++;
		}

		if (ImGui::Button("START/STOP"))
		{
			m_pAnimeClip[0].bStop = !m_pAnimeClip[0].bStop;
			m_pModel->GetAnime()->nStartTime = m_pAnimeClip[0].nStartTime;
			m_pModel->GetAnime()->nEndTime = m_pAnimeClip[0].nEndTime;
		}

		if (ImGui::SliderInt("StartTime", &m_pAnimeClip[0].nStartTime, 0, m_pModel->GetAnime()->nEndTime - 1))
		{
			m_pModel->GetAnime()->nStartTime = m_pAnimeClip[0].nStartTime;
			m_pModel->GetAnime()->nEndTime = m_pAnimeClip[0].nEndTime;
		}

		if(ImGui::SliderInt("EndTime", &m_pAnimeClip[0].nEndTime, m_pModel->GetAnime()->nStartTime + 1, m_nOldEndTime))
		{
			m_pModel->GetAnime()->nStartTime = m_pAnimeClip[0].nStartTime;
			m_pModel->GetAnime()->nEndTime = m_pAnimeClip[0].nEndTime;
		}

		ImGui::InputText("ClipName", str[m_nNumClip], IM_ARRAYSIZE(str));

		if (ImGui::Button("ClipAdd"))
		{
			m_pAnimeClip[m_nNumClip].nStartTime = m_pAnimeClip[0].nStartTime;
			m_pAnimeClip[m_nNumClip].nEndTime = m_pAnimeClip[0].nEndTime;
			m_nNumClip++;
		}
	}
	ImGui::End();


	for (int i = 1; i < m_nNumClip; i++)
	{
		ImGui::Begin(str[i]);
		{
			if (ImGui::SliderInt("Animation", &m_pFrame[0], m_pAnimeClip[i].nStartTime, m_pAnimeClip[i].nEndTime-1))
			{
				m_pModel->GetAnime()->nStartTime = m_pAnimeClip[i].nStartTime;
				m_pModel->GetAnime()->nEndTime = m_pAnimeClip[i].nEndTime;
			}

			if (ImGui::Button("BACK"))
			{
				m_pFrame[0]--;
			}

			if (ImGui::Button("NEXT"))
			{
				m_pFrame[0]++;
			}

			if (ImGui::Button("START/STOP"))
			{
				m_pAnimeClip[i].bStop = !m_pAnimeClip[i].bStop;
				m_pModel->GetAnime()->nStartTime = m_pAnimeClip[i].nStartTime;
				m_pModel->GetAnime()->nEndTime = m_pAnimeClip[i].nEndTime;
			}
		}
		ImGui::End();

		if (m_pAnimeClip[i].bStop && !m_pAnimeClip[0].bStop)
		{
			m_pFrame[0]++;
		}

		if (m_pAnimeClip[0].bStop && !m_pAnimeClip[i].bStop)
		{
			m_pFrame[0]++;
		}
	}

	if (m_pFrame[0] >= m_pModel->GetAnime()->nEndTime)
	{
		m_pFrame[0] = m_pModel->GetAnime()->nStartTime;
	}

	if (m_pFrame[0] < m_pModel->GetAnime()->nStartTime)
	{
		m_pFrame[0] = m_pModel->GetAnime()->nEndTime;
	}
}

///////////////////////////////////////////////////////////////////////////////
//バッファ作成
///////////////////////////////////////////////////////////////////////////////
void Player::MakeVertex(int nMeshCount, SkinMeshModel::Mesh* pMesh)
{
	AppRenderer* pAppRenderer = AppRenderer::GetInstance();
	ID3D11Device* pDevice = pAppRenderer->GetDevice();

	//四角形
	SkinMeshModel::ModelVertex* vertices = new SkinMeshModel::ModelVertex[pMesh[nMeshCount].nNumVertex];
	for (int j = 0; j < pMesh[nMeshCount].nNumVertex; j++)
	{
		vertices[j].position = VECTOR3(pMesh[nMeshCount].pPosition[j].x, pMesh[nMeshCount].pPosition[j].y, pMesh[nMeshCount].pPosition[j].z);
		vertices[j].color = VECTOR4(pMesh[nMeshCount].color.x, pMesh[nMeshCount].color.y, pMesh[nMeshCount].color.z, pMesh[nMeshCount].color.w);
		vertices[j].boneIndex = VECTOR4(pMesh[nMeshCount].boneIndex[0][j], pMesh[nMeshCount].boneIndex[1][j], pMesh[nMeshCount].boneIndex[2][j], pMesh[nMeshCount].boneIndex[3][j]);
		vertices[j].weight = VECTOR4(pMesh[nMeshCount].weight[0][j], pMesh[nMeshCount].weight[1][j], pMesh[nMeshCount].weight[2][j], pMesh[nMeshCount].weight[3][j]);
	}

	WORD* hIndexData = new WORD[pMesh[nMeshCount].nNumPolygonVertex];

	for (int j = 0; j < pMesh[nMeshCount].nNumPolygonVertex; j++)
	{
		vertices[pMesh[nMeshCount].pIndexNumber[j]].normal = VECTOR3(pMesh[nMeshCount].pNormal[j].x, pMesh[nMeshCount].pNormal[j].y, pMesh[nMeshCount].pNormal[j].z);
		vertices[pMesh[nMeshCount].pIndexNumber[j]].tex = VECTOR2(pMesh[nMeshCount].pTex[j].x, 1 - pMesh[nMeshCount].pTex[j].y);
		hIndexData[j] = pMesh[nMeshCount].pIndexNumber[j];
	}

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SkinMeshModel::ModelVertex) * pMesh[nMeshCount].nNumVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer)))
		return;


	//インデックスバッファ作成
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(WORD) *  pMesh[nMeshCount].nNumPolygonVertex;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(WORD);

	D3D11_SUBRESOURCE_DATA hSubResourceData;
	hSubResourceData.pSysMem = hIndexData;
	hSubResourceData.SysMemPitch = 0;
	hSubResourceData.SysMemSlicePitch = 0;

	if (FAILED(pDevice->CreateBuffer(&hBufferDesc, &hSubResourceData, &m_pIndexBuffer)))
		return;
	
	delete[] vertices;

	delete[] hIndexData;

	delete[] pMesh[nMeshCount].pPosition;
	delete[] pMesh[nMeshCount].pNormal;
	delete[] pMesh[nMeshCount].pTex;
	delete[] pMesh[nMeshCount].pWeight;
	delete[] pMesh[nMeshCount].pBoneIndex;
}

void Player::SaveFile(char* FileName)
{
	FILE* pFile;
	
	pFile = fopen(FileName, "wb");

	int nNumClip = m_nNumClip - 1;

	fwrite(&nNumClip, sizeof(int), 1, pFile);

	for (int i = 1; i < m_nNumClip; i++)
	{
		fwrite(&m_pAnimeClip[i].nStartTime, sizeof(m_pAnimeClip[i].nStartTime), 1, pFile);
		fwrite(&m_pAnimeClip[i].nEndTime, sizeof(m_pAnimeClip[i].nEndTime), 1, pFile);
	}

	int nNumMesh = m_pModel->GetNumMesh();
	fwrite(&nNumMesh, sizeof(nNumMesh), 1, pFile);

	SkinMeshModel::Mesh* pMesh = m_pModel->GetMesh();

	for (int i = 0; i < nNumMesh; i++)
	{
		fwrite(&pMesh[i].nNumVertex, sizeof(pMesh[i].nNumVertex), 1, pFile);

		for (int j = 0; j < pMesh[i].nNumVertex; j++)
		{
			fwrite(&pMesh[i].pPosition[j], sizeof(pMesh[i].pPosition[j]), 1, pFile);

			fwrite(&pMesh[i].color, sizeof(pMesh[i].color), 1, pFile);
		}

		fwrite(&pMesh[i].nNumPolygonVertex, sizeof(pMesh[i].nNumPolygonVertex), 1, pFile);

		for (int j = 0; j < pMesh[i].nNumPolygonVertex; j++)
		{
			fwrite(&pMesh[i].pNormal[j], sizeof(pMesh[i].pNormal[j]), 1, pFile);

			fwrite(&pMesh[i].pTex[j], sizeof(pMesh[i].pTex[j]), 1, pFile);

			fwrite(&pMesh[i].pIndexNumber[j], sizeof(pMesh[i].pIndexNumber[j]), 1, pFile);
		}

		fwrite(&pMesh[i].nNumCluster, sizeof(pMesh[i].nNumCluster), 1, pFile);

		SkinMeshModel::Cluster* pCluster;

		pCluster = new SkinMeshModel::Cluster[pMesh[i].nNumCluster];

		for (int j = 0; j < pMesh[i].nNumCluster; j++)
		{
			pCluster[j].pMatrix = new XMMATRIX*[m_nNumClip - 1];
			for (int k = 0; k < m_nNumClip-1; k++)
			{
				pCluster[j].pMatrix[k] = new XMMATRIX[m_pAnimeClip[k+1].nEndTime];

				for (int l = m_pAnimeClip[k+1].nStartTime; l < m_pAnimeClip[k+1].nEndTime; l++)
				{
					pCluster[j].pMatrix[k][l] = pMesh[i].pCluster[j].pMatrix[0][l];

					fwrite(&pCluster[j].pMatrix[k][l], sizeof(pCluster[j].pMatrix[k][l]), 1, pFile);
				}
			}
		}

		fwrite(&pMesh[i].LclPos, sizeof(pMesh[i].LclPos), 1, pFile);
		fwrite(&pMesh[i].LclRot, sizeof(pMesh[i].LclRot), 1, pFile);
		fwrite(&pMesh[i].LclScl, sizeof(pMesh[i].LclScl), 1, pFile);

		fwrite(&pMesh[i].pFileName, sizeof(pMesh[i].pFileName), 1, pFile);
	}

	fclose(pFile);
}

void Player::LoadFile(char* FileName)
{
	FILE* pFile;

	pFile = fopen(FileName, "rb");

	fread(&m_nNumClip - 1, sizeof(m_nNumClip - 1), 1, pFile);

	for (int i = 1; i < m_nNumClip; i++)
	{
		fread(&m_pAnimeClip[i].nStartTime, sizeof(m_pAnimeClip[i].nStartTime), 1, pFile);
		fread(&m_pAnimeClip[i].nEndTime, sizeof(m_pAnimeClip[i].nEndTime), 1, pFile);
	}

	int nNumMesh = m_pModel->GetNumMesh();
	fread(&nNumMesh, sizeof(m_pModel->GetNumMesh()), 1, pFile);

	SkinMeshModel::Mesh* pMesh = m_pModel->GetMesh();

	for (int i = 0; i < nNumMesh; i++)
	{
		fread(&pMesh[i].nNumVertex, sizeof(pMesh[i].nNumVertex), 1, pFile);

		for (int j = 0; j < pMesh[i].nNumVertex; j++)
		{
			fread(&pMesh[i].pPosition[j], sizeof(pMesh[i].pPosition[j]), 1, pFile);

			fread(&pMesh[i].color, sizeof(pMesh[i].color), 1, pFile);
		}

		fread(&pMesh[i].nNumPolygonVertex, sizeof(pMesh[i].nNumPolygonVertex), 1, pFile);

		for (int j = 0; j < pMesh[i].nNumPolygonVertex; j++)
		{
			fread(&pMesh[i].pNormal[j], sizeof(pMesh[i].pNormal[j]), 1, pFile);

			fread(&pMesh[i].pTex[j], sizeof(pMesh[i].pTex[j]), 1, pFile);

			fread(&pMesh[i].pIndexNumber[j], sizeof(pMesh[i].pIndexNumber[j]), 1, pFile);
		}

		fread(&pMesh[i].nNumCluster, sizeof(pMesh[i].nNumCluster), 1, pFile);

		SkinMeshModel::Cluster* pCluster;

		pCluster = new SkinMeshModel::Cluster[pMesh[i].nNumCluster];

		for (int j = 0; j < pMesh[i].nNumCluster; j++)
		{
			pCluster[j].pMatrix = new XMMATRIX*[m_nNumClip - 1];
			for (int k = 0; k < m_nNumClip - 1; k++)
			{
				pCluster[j].pMatrix[k] = new XMMATRIX[m_pAnimeClip[k + 1].nEndTime];

				for (int l = m_pAnimeClip[k + 1].nStartTime; l < m_pAnimeClip[k + 1].nEndTime; l++)
				{
					pCluster[j].pMatrix[k][l] = pMesh[i].pCluster[j].pMatrix[0][l];

					fread(&pCluster[j].pMatrix[k][l], sizeof(pCluster[j].pMatrix[k][l]), 1, pFile);
				}
			}
		}

		fread(&pMesh[i].LclPos, sizeof(pMesh[i].LclPos), 1, pFile);
		fread(&pMesh[i].LclRot, sizeof(pMesh[i].LclRot), 1, pFile);
		fread(&pMesh[i].LclScl, sizeof(pMesh[i].LclScl), 1, pFile);

		fread(&pMesh[i].pFileName, sizeof(pMesh[i].pFileName), 1, pFile);
	}

	fclose(pFile);
}
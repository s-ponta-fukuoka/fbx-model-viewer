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
	m_pModel = new SkinMeshModel("resource/model/enemy.fbx");
	m_pModel = pModelManager->SeekSkinMeshModel(m_pModel);

	m_pSaveMesh = new SaveMesh[m_pModel->GetNumMesh()];

	m_pMesh = m_pModel->GetMesh();

	m_pFrame = new int();

	m_pAnimeNumber = new int();

	m_pAnimeClip = new ANIME_CLIP[ANIME_CLIP_MAX];

	PixelShader::PIXEL_TYPE ePsType;

	for (int i = 0; i < m_pModel->GetNumMesh(); i++)
	{

		MakeVertex(i, m_pMesh);

		ID3D11ShaderResourceView* pTextureResource = NULL;

		if (m_pMesh[i].pFileName != NULL)
		{
			ePsType = PixelShader::PS_TOON;
			Texture* pTexture = new Texture(m_pMesh[i].pFileName, pTextureManager);
			//Texture* pTexture = new Texture("resource/sprite/NULL.jpg", pTextureManager);
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
			m_pMesh[i].nNumPolygonVertex,
			m_pFrame,
			m_pAnimeNumber,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			VertexShader::VS_TOON,
			ePsType,
			m_pMesh[i].pCluster,
			m_pMesh[i]));

		pRenderManager->AddShadowRenderer(new SkinnedMeshRenderer(m_pVertexBuffer,
			m_pIndexBuffer,
			pShaderManager,
			pTextureResource,
			NULL,
			m_pTransform,
			pLightCameraConstant,
			NULL,
			m_pMesh[i].nNumPolygonVertex,
			m_pFrame,
			m_pAnimeNumber,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			VertexShader::VS_TOON,
			ePsType = PixelShader::PS_SHADOW,
			m_pMesh[i].pCluster,
			m_pMesh[i]));
	}

	m_pAnimeClip[0].nStartTime = m_pModel->GetAnime()->nStartTime;
	m_pAnimeClip[0].nEndTime = m_pModel->GetAnime()->nEndTime;

	for (int i = 0; i < ANIME_CLIP_MAX; i++)
	{
		m_pAnimeClip[i].bStop = false;
	}

	m_pTransform->rot.x = 0;

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
//インスタンス削除
///////////////////////////////////////////////////////////////////////////////
void Player::DeleteInstance(void)
{
	delete m_pPlayer;
	m_pPlayer = NULL;
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
	static char strStartTime[256] = "0";
	static char strEndTime[256] = "0";

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
		ImGui::InputText("StartTimeEdit", strStartTime, IM_ARRAYSIZE(strStartTime));

		if(ImGui::SliderInt("EndTime", &m_pAnimeClip[0].nEndTime, m_pModel->GetAnime()->nStartTime + 1, m_nOldEndTime))
		{
			m_pModel->GetAnime()->nStartTime = m_pAnimeClip[0].nStartTime;
			m_pModel->GetAnime()->nEndTime = m_pAnimeClip[0].nEndTime;
		}
		ImGui::InputText("EndTimeEdit", strEndTime, IM_ARRAYSIZE(strEndTime));

		ImGui::InputText("ClipName", str[m_nNumClip], IM_ARRAYSIZE(str));

		if (ImGui::Button("ClipAdd"))
		{
			m_pAnimeClip[m_nNumClip].nStartTime = m_pAnimeClip[0].nStartTime;
			m_pAnimeClip[m_nNumClip].nEndTime = m_pAnimeClip[0].nEndTime;
			m_nNumClip++;
		}
	}
	ImGui::End();

	m_pAnimeClip[0].nStartTime = atoi(strStartTime);
	m_pAnimeClip[0].nEndTime = atoi(strEndTime);
	m_pModel->GetAnime()->nStartTime = m_pAnimeClip[0].nStartTime;
	m_pModel->GetAnime()->nEndTime = m_pAnimeClip[0].nEndTime;

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

	m_pSaveMesh[nMeshCount].nNumVertex = pMesh[nMeshCount].nNumVertex;
	m_pSaveMesh[nMeshCount].nNumCluster = pMesh[nMeshCount].nNumCluster;
	m_pSaveMesh[nMeshCount].nNumPolygonVertex = pMesh[nMeshCount].nNumPolygonVertex;

	//四角形
	SkinMeshModel::ModelVertex* vertices = new SkinMeshModel::ModelVertex[pMesh[nMeshCount].nNumVertex];
	for (int j = 0; j < pMesh[nMeshCount].nNumVertex; j++)
	{
		vertices[j].position = VECTOR3(pMesh[nMeshCount].pPosition[j].x, pMesh[nMeshCount].pPosition[j].y, pMesh[nMeshCount].pPosition[j].z);
		vertices[j].color = VECTOR4(pMesh[nMeshCount].color.x, pMesh[nMeshCount].color.y, pMesh[nMeshCount].color.z, pMesh[nMeshCount].color.w);
		vertices[j].boneIndex = pMesh[nMeshCount].pBoneIndex[j];
		vertices[j].weight = pMesh[nMeshCount].pWeight[j];

		m_pSaveMesh[nMeshCount].position[j] = VECTOR3(pMesh[nMeshCount].pPosition[j].x, pMesh[nMeshCount].pPosition[j].y, pMesh[nMeshCount].pPosition[j].z);
		m_pSaveMesh[nMeshCount].color = VECTOR4(pMesh[nMeshCount].color.x, pMesh[nMeshCount].color.y, pMesh[nMeshCount].color.z, pMesh[nMeshCount].color.w);
		m_pSaveMesh[nMeshCount].boneIndex[j] = pMesh[nMeshCount].pBoneIndex[j];
		m_pSaveMesh[nMeshCount].weight[j] = pMesh[nMeshCount].pWeight[j];
	}

	WORD* hIndexData = new WORD[pMesh[nMeshCount].nNumPolygonVertex];

	for (int j = 0; j < pMesh[nMeshCount].nNumPolygonVertex; j++)
	{
		vertices[pMesh[nMeshCount].pIndexNumber[j]].normal = VECTOR3(pMesh[nMeshCount].pNormal[j].x, pMesh[nMeshCount].pNormal[j].y, pMesh[nMeshCount].pNormal[j].z);
		vertices[pMesh[nMeshCount].pIndexNumber[j]].tex = VECTOR2(pMesh[nMeshCount].pTex[j].x, 1 - pMesh[nMeshCount].pTex[j].y);
		hIndexData[j] = pMesh[nMeshCount].pIndexNumber[j];

		m_pSaveMesh[nMeshCount].normal[j] = pMesh[nMeshCount].pNormal[j];
		m_pSaveMesh[nMeshCount].tex[j] = pMesh[nMeshCount].pTex[j];
		m_pSaveMesh[nMeshCount].IndexNumber[j] = pMesh[nMeshCount].pIndexNumber[j];
	}

	m_pSaveMesh[nMeshCount].LclPos = pMesh[nMeshCount].LclPos;
	m_pSaveMesh[nMeshCount].LclRot = pMesh[nMeshCount].LclRot;
	m_pSaveMesh[nMeshCount].LclScl = pMesh[nMeshCount].LclScl;


	if (pMesh[nMeshCount].pFileName != NULL)
	{
		for (int i = 0; i < strlen(pMesh[nMeshCount].pFileName); i++)
		{
			m_pSaveMesh[nMeshCount].FileName[i] = (char)pMesh[nMeshCount].pFileName[i];
		}
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
}

void Player::SaveFile(char* FileName)
{
	FILE* pFile;
	
	rewind(stdin);
	pFile = fopen(FileName, "w");

	int nNumClip = m_nNumClip - 1;

	fprintf(pFile, "%d\n", nNumClip);//アニメーション数
	for (int i = 0; i < nNumClip; i++)
	{
		//スタート
		fprintf(pFile, "%d:", m_pAnimeClip[i+1].nStartTime);

		//エンド
		fprintf(pFile, "%d\n", m_pAnimeClip[i+1].nEndTime);
	}

	//メッシュの数
	int nNumMesh = m_pModel->GetNumMesh();
	fprintf(pFile, "\n%d\n\n", nNumMesh);
	for (int i = 0; i < nNumMesh; i++)
	{
		fprintf(pFile, "%d\n", m_pMesh[i].nNumVertex);//ポリゴン数
		for (int j = 0; j < m_pMesh[i].nNumVertex; j++)
		{
			//座標
			fprintf(pFile, "%f:", m_pMesh[i].pPosition[j].x);
			fprintf(pFile, "%f:", m_pMesh[i].pPosition[j].y);
			fprintf(pFile, "%f\n", m_pMesh[i].pPosition[j].z);

			//色
			fprintf(pFile, "%f:", m_pMesh[i].color.x);
			fprintf(pFile, "%f:", m_pMesh[i].color.y);
			fprintf(pFile, "%f:", m_pMesh[i].color.z);
			fprintf(pFile, "%f\n", m_pMesh[i].color.w);

			//クラスターインデックス
			fprintf(pFile, "%d:", m_pMesh[i].boneIndex[0][j]);
			fprintf(pFile, "%d:", m_pMesh[i].boneIndex[1][j]);
			fprintf(pFile, "%d:", m_pMesh[i].boneIndex[2][j]);
			fprintf(pFile, "%d\n", m_pMesh[i].boneIndex[3][j]);

			//ウェイト
			fprintf(pFile, "%f:", m_pMesh[i].weight[0][j]);
			fprintf(pFile, "%f:", m_pMesh[i].weight[1][j]);
			fprintf(pFile, "%f:", m_pMesh[i].weight[2][j]);
			fprintf(pFile, "%f\n", m_pMesh[i].weight[3][j]);
		}

		fprintf(pFile, "%d\n", m_pMesh[i].nNumPolygonVertex);//頂点数
		for (int j = 0; j < m_pMesh[i].nNumPolygonVertex; j++)
		{
			//法線
			fprintf(pFile, "%f:", m_pMesh[i].pNormal[j].x);
			fprintf(pFile, "%f:", m_pMesh[i].pNormal[j].y);
			fprintf(pFile, "%f\n", m_pMesh[i].pNormal[j].z);

			//UV
			fprintf(pFile, "%f:", m_pMesh[i].pTex[j].x);
			fprintf(pFile, "%f\n", m_pMesh[i].pTex[j].y);

			//インデックスナンバー
			fprintf(pFile, "%d\n", m_pMesh[i].pIndexNumber[j]);
		}


		fprintf(pFile, "%d\n", m_pMesh[i].nNumCluster);//クラスタ数

		SkinMeshModel::Cluster* pCluster;
		
		pCluster = new SkinMeshModel::Cluster[m_pMesh[i].nNumCluster];
	

		for (int j = 0; j < m_pSaveMesh[i].nNumCluster; j++)
		{
			pCluster[j].pMatrix = new XMMATRIX*[nNumClip];
			for (int k = 0; k < nNumClip; k++)
			{
				pCluster[j].pMatrix[k] = new XMMATRIX[m_pAnimeClip[k+1].nEndTime];
				for (int l = m_pAnimeClip[k+1].nStartTime; l < m_pAnimeClip[k+1].nEndTime; l++)
				{
					pCluster[j].pMatrix[k][l] = m_pMesh[i].pCluster[j].pMatrix[0][l];

					//クラスター行列
					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._11);
					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._12);
					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._13);
					fprintf(pFile, "%f\n", pCluster[j].pMatrix[k][l]._14);

					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._21);
					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._22);
					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._23);
					fprintf(pFile, "%f\n", pCluster[j].pMatrix[k][l]._24);

					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._31);
					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._32);
					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._33);
					fprintf(pFile, "%f\n", pCluster[j].pMatrix[k][l]._34);

					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._41);
					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._42);
					fprintf(pFile, "%f:", pCluster[j].pMatrix[k][l]._43);
					fprintf(pFile, "%f\n", pCluster[j].pMatrix[k][l]._44);
				}
			}
		}

		//変換行列
		fprintf(pFile, "%f:", m_pMesh[i].LclPos.x);
		fprintf(pFile, "%f:", m_pMesh[i].LclPos.y);
		fprintf(pFile, "%f\n", m_pMesh[i].LclPos.z);

		fprintf(pFile, "%f:", m_pMesh[i].LclRot.x);
		fprintf(pFile, "%f:", m_pMesh[i].LclRot.y);
		fprintf(pFile, "%f\n", m_pMesh[i].LclRot.z);

		fprintf(pFile, "%f:", m_pMesh[i].LclScl.x);
		fprintf(pFile, "%f:", m_pMesh[i].LclScl.y);
		fprintf(pFile, "%f\n", m_pMesh[i].LclScl.z);

		fprintf(pFile, "\n");
	}

	for (int i = 0; i < nNumMesh; i++)
	{
		//ファイル名
		fprintf(pFile, "%s\n", m_pMesh[i].pFileName);
	}

	fclose(pFile);

	//pFile = fopen(FileName, "wb");
	//
	//m_pSaveMesh;
	//
	//int nNumClip = m_nNumClip - 1;
	//
	//fwrite(&nNumClip, sizeof(int), 1, pFile);
	//
	//for (int i = 1; i < m_nNumClip; i++)
	//{
	//	fwrite(&m_pAnimeClip[i].nStartTime, sizeof(int), 1, pFile);
	//	fwrite(&m_pAnimeClip[i].nEndTime, sizeof(int), 1, pFile);
	//}
	//
	//int nNumMesh = m_pModel->GetNumMesh();
	//fwrite(&nNumMesh, sizeof(int), 1, pFile);
	//
	//for (int i = nNumMesh-1; i < nNumMesh; i++)
	//{
	//	fwrite(&m_pSaveMesh[i].nNumVertex, sizeof(int), 1, pFile);
	//
	//	for (int j = 0; j < m_pSaveMesh[i].nNumVertex; j++)
	//	{
	//		fwrite(&m_pSaveMesh[i].position[j], sizeof(VECTOR3), 1, pFile);
	//
	//		fwrite(&m_pSaveMesh[i].color, sizeof(VECTOR4), 1, pFile);
	//
	//		fwrite(&m_pSaveMesh[i].boneIndex[j], sizeof(VECTOR4), 1, pFile);
	//
	//		fwrite(&m_pSaveMesh[i].weight, sizeof(VECTOR4), 1, pFile);
	//	}
	//
	//	fwrite(&m_pSaveMesh[i].nNumPolygonVertex, sizeof(int), 1, pFile);
	//
	//	for (int j = 0; j < m_pSaveMesh[i].nNumPolygonVertex; j++)
	//	{
	//		fwrite(&m_pSaveMesh[i].normal[j], sizeof(VECTOR3), 1, pFile);
	//
	//		fwrite(&m_pSaveMesh[i].tex[j], sizeof(VECTOR2), 1, pFile);
	//
	//		fwrite(&m_pSaveMesh[i].IndexNumber[j], sizeof(int), 1, pFile);
	//	}
	//
	//	fwrite(&m_pSaveMesh[i].nNumCluster, sizeof(int), 1, pFile);
	//
	//	//SaveCluster* pCluster;
	//
	//	//pCluster = new SaveCluster[m_pMesh[i].nNumCluster];
	//
	//	for (int j = 0; j < m_pSaveMesh[i].nNumCluster; j++)
	//	{
	//		for (int k = 0; k < m_nNumClip; k++)
	//		{
	//
	//			for (int l = m_pAnimeClip[k+1].nStartTime; l < m_pAnimeClip[k+1].nEndTime; l++)
	//			{
	//				//pCluster[j].pMatrix[k][l] = m_pSaveMesh[i].cluster[j].pMatrix[0][l];
	//
	//				fwrite(&m_pSaveMesh[i].cluster[j].pMatrix[0][l], sizeof(XMMATRIX), 1, pFile);
	//			}
	//		}
	//	}
	//
	//	fwrite(&m_pSaveMesh[i].LclPos, sizeof(VECTOR3), 1, pFile);
	//	fwrite(&m_pSaveMesh[i].LclRot, sizeof(VECTOR3), 1, pFile);
	//	fwrite(&m_pSaveMesh[i].LclScl, sizeof(VECTOR3), 1, pFile);
	//
	//	//if (m_pMesh[i].pFileName == NULL)
	//	//{
	//	//	char* null = "NULL";
	//	//	for (int j = 0; j < strlen(null); j++)
	//	//	{
	//	//		m_pSaveMesh[i].FileName[j] = (char)null[j];
	//	//	}
	//	//	m_pMesh[i].pFileName = "NULL";
	//	//}
	//	//
	//	//int size = strlen(m_pMesh[i].pFileName);
	//	//
	//	//fwrite(&size, sizeof(int), 1, pFile);
	//	//
	//	//fwrite(&m_pSaveMesh[i].FileName, sizeof(char), size, pFile);
	//}
	//
	//fclose(pFile);
}
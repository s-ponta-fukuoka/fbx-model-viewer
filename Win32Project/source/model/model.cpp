//=============================================================================
//
// model.h
// Author : shota fukuoka
//
//=============================================================================
#include "model.h"
#include <iostream>
#include <functional>
#//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数:
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
//コンストラクタ
///////////////////////////////////////////////////////////////////////////////
Model::Model()
{
	;
}

SkinMeshModel::SkinMeshModel(char* pFileName)
	:m_nNumAnime(0)
	,m_nNumMesh(0)
	,m_pAnime(NULL)
	,m_pMesh(NULL)
{
	m_pFileName = pFileName;
}

///////////////////////////////////////////////////////////////////////////////
//デストラクタ
///////////////////////////////////////////////////////////////////////////////
Model::~Model()
{
	;
}

SkinMeshModel::~SkinMeshModel()
{
	;
}

///////////////////////////////////////////////////////////////////////////////
//終了
///////////////////////////////////////////////////////////////////////////////
void Model::Release(void)
{
	;
}

void SkinMeshModel::Release(void)
{
	for (int i = 0; i < m_nNumMesh; i++)
	{
		for (int clusterCnt = 0; clusterCnt < m_pMesh[i].nNumCluster; clusterCnt++)
		{
			for (int k = 0; k < m_nNumAnime; k++)
			{
				delete[] m_pMesh[i].pCluster[clusterCnt].pMatrix[k];
			}
			delete[] m_pMesh[i].pCluster[clusterCnt].pMatrix;
		}
		delete[]  m_pMesh[i].pCluster;
	}
	delete[] m_pMesh;
}


///////////////////////////////////////////////////////////////////////////////
//ファイルネーム取得
///////////////////////////////////////////////////////////////////////////////
const char* Model::GetFileName()
{
	return m_pFileName;
}

///////////////////////////////////////////////////////////////////////////////
//読み込み
///////////////////////////////////////////////////////////////////////////////
void SkinMeshModel::LoadFile(const  char* FilenName)
{
	//------------------------------------
	//FBXSDK初期化
	//------------------------------------
	FbxManager *pSdkManager = FbxManager::Create();		//fbxsdkmanagerオブジェクト生成

	FbxImporter *pImporter = FbxImporter::Create(pSdkManager, "");		//インポータの作成
	if (pImporter == NULL) {
		printf("インポート作成失敗しました\n");
		getchar();
		pSdkManager->Destroy();
		return;
	}

	const char *pFileName = FilenName;
	int FileFormat = -1;
	if (!pImporter->Initialize(pFileName)) {
		printf("ファイル読み込み失敗しました\n");
		getchar();
		pSdkManager->Destroy();
		return;
	}

	FbxScene *pScene = FbxScene::Create(pSdkManager, "");
	if (pScene == NULL) {
		printf("シーン作成失敗しました\n");
		getchar();
		pSdkManager->Destroy();
		return;
	}

	if (!pImporter->Import(pScene)) {
		printf("ファイルからシーン読み込み失敗しました\n");
		getchar();
		pSdkManager->Destroy();
		return;
	}

	pImporter->Destroy();//インポーターの破棄

	FbxGeometryConverter converter(pSdkManager);

	// あらかじめポリゴンを全て三角形化しておく
	converter.Triangulate(pScene, true);

	// マテリアルごとにメッシュを分離
	converter.SplitMeshesPerMaterial(pScene, true);

	//------------------------------------
	//FBXSDK読み込み
	//------------------------------------

	FbxMesh *mesh;
	m_nNumMesh = pScene->GetMemberCount<FbxMesh>();
	int nNumMaterial = pScene->GetMaterialCount();

	m_pMesh = new Mesh[m_nNumMesh];
	m_pMaterial = new MATERIAL[nNumMaterial];

	FbxArray<FbxString*> animation_names;
	FbxTime start_time, end_time;
	pScene->FillAnimStackNameArray(animation_names);
	m_nNumAnime = animation_names.GetCount();
	m_pAnime = new Anime[m_nNumAnime];

	//------------------------------------
	//アニメーション
	//------------------------------------
	for (int i = 0; i < m_nNumAnime; i++)
	{
		//FbxtakeInfo取得
		auto take_info = pScene->GetTakeInfo(*(animation_names[i]));

		//開始時間
		m_pAnime[i].nStartTime = take_info->mLocalTimeSpan.GetStart().Get() / FbxTime::GetOneFrameValue(FbxTime::eFrames24);

		//終了時間
		m_pAnime[i].nEndTime = take_info->mLocalTimeSpan.GetStop().Get() / FbxTime::GetOneFrameValue(FbxTime::eFrames24);
	}

	//------------------------------------
	//マテリアル
	//------------------------------------
	for (int i = 0; i < nNumMaterial; i++)
	{
		FbxSurfaceLambert* lambert;
		FbxSurfacePhong* phong;

		FbxSurfaceMaterial* material = pScene->GetMaterial(i);
		// materialはKFbxSurfaceMaterialオブジェクト
		m_pMaterial[i].pMaterialName = (char*)material->GetName();

		// LambertかPhongか
		if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			// Lambertにダウンキャスト
			lambert = (FbxSurfaceLambert*)material;

			FbxDouble3 diffuse = lambert->Diffuse;
			m_pMaterial[i].color.x = diffuse[0];
			m_pMaterial[i].color.y = diffuse[1];
			m_pMaterial[i].color.z = diffuse[2];
			m_pMaterial[i].color.w = diffuse[3];
		}
		else if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			// Phongにダウンキャスト
			phong = (FbxSurfacePhong*)material;
		}
		

		// ディフューズプロパティを検索
		FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

		// プロパティが持っているレイヤードテクスチャの枚数をチェック
		int layerNum = property.GetSrcObjectCount(FbxLayeredTexture::ClassId);
		// レイヤードテクスチャが無ければ通常テクスチャ
		if (layerNum == 0)
		{
			// 通常テクスチャの枚数をチェック
			int numGeneralTexture = property.GetSrcObjectCount(FbxTexture::ClassId);
			m_pMesh[i].pFileName = NULL;
			// 各テクスチャについてテクスチャ情報をゲット
			for (int clusterCnt = 0; clusterCnt < numGeneralTexture; clusterCnt++)
			{
				// i番目のテクスチャオブジェクト取得
				FbxFileTexture* texture = FbxCast<FbxFileTexture>(property.GetSrcObject(FbxFileTexture::ClassId, clusterCnt));

				// materialはKFbxSurfaceMaterialオブジェクト
				const char* materialName = texture->GetName();

				// テクスチャファイルパスを取得（フルパス）
				std::string relFileName = texture->GetRelativeFileName();

				//int path_i = relFileName.find_last_of("/");//7
				int path_i = relFileName.find_last_of("\\");//7
				int ext_i = relFileName.find_last_of(".");//10
				std::string pathname = relFileName.substr(0, path_i);//0文字目から７文字切り出す "C:\\aaa\\"
				std::string extname = relFileName.substr(ext_i+1, relFileName.size() - ext_i); // 10文字目から４文字切り出す ".txt"
				std::string filename = relFileName.substr(path_i+1, ext_i - path_i);// ７文字目から３文字切り出す　"bbb"

				std::string strPathName = "resource/sprite/" + filename + extname;

				m_pMaterial[i].pFileName = strPathName;
			}

			if (numGeneralTexture == 0)
			{
				m_pMaterial[i].pFileName = "NULL";

				const FbxImplementation *pImplementation = GetImplementation(material, FBXSDK_IMPLEMENTATION_CGFX);

				if (pImplementation != NULL)
				{
					const FbxBindingTable*  pRootTable = pImplementation->GetRootTable();

					size_t entryCount = pRootTable->GetEntryCount();

					for (int clusterCnt = 0; clusterCnt < entryCount; clusterCnt++)
					{
						const char* entryName = pRootTable->GetEntry(clusterCnt).GetSource();

						FbxProperty property = material->RootProperty.FindHierarchical(entryName);

						int fileTextureCount = property.GetSrcObjectCount<FbxFileTexture>();

						for (int k = 0; k < fileTextureCount; k++)
						{
							FbxFileTexture* pFileTexture = property.GetSrcObject<FbxFileTexture>(k);

							if (pFileTexture != NULL)
							{
								// テクスチャファイルパスを取得（フルパス）
								std::string relFileName = pFileTexture->GetRelativeFileName();

								int path_i = relFileName.find_last_of("/");//7
								int ext_i = relFileName.find_last_of(".");//10
								std::string pathname = relFileName.substr(0, path_i + 1);//0文字目から７文字切り出す "C:\\aaa\\"
								std::string extname = relFileName.substr(ext_i, relFileName.size() - ext_i); // 10文字目から４文字切り出す ".txt"
								std::string filename = relFileName.substr(path_i, ext_i - path_i);// ７文字目から３文字切り出す　"bbb"

								std::string strPathName = "resource/sprite" + filename + extname;

								m_pMaterial[i].pFileName = strPathName;
							}
						}
					}
				}
			}
		}
	}

	//------------------------------------
	//メッシュ
	//------------------------------------
	for (int meshCnt = 0; meshCnt < m_nNumMesh; meshCnt++)
	{
		mesh = pScene->GetSrcObject<FbxMesh>(meshCnt);
		if (mesh != NULL)
		{
			//頂点//////////////////////////////////////////////////
			m_pMesh[meshCnt].nNumPolygon = mesh->GetPolygonCount();//総ポリゴン数
			m_pMesh[meshCnt].nNumPolygonVertex = mesh->GetPolygonVertexCount();//ポリゴン頂点インデックス
			m_pMesh[meshCnt].nNumVertex = mesh->GetControlPointsCount();//頂点数
			FbxVector4 *src = mesh->GetControlPoints();//頂点座標配列

			m_pMesh[meshCnt].pPosition = new VECTOR3[m_pMesh[meshCnt].nNumVertex];
			for (int i = 0; i < 4; i++)
			{
				m_pMesh[meshCnt].boneIndex[i] = new int[m_pMesh[meshCnt].nNumVertex];
				m_pMesh[meshCnt].weight[i] = new float[m_pMesh[meshCnt].nNumVertex];
			}
			for (int i = 0; i < m_pMesh[meshCnt].nNumVertex; i++)
			{
				m_pMesh[meshCnt].pPosition[i].x = (float)src[i][0];
				m_pMesh[meshCnt].pPosition[i].y = (float)src[i][1];
				m_pMesh[meshCnt].pPosition[i].z = (float)src[i][2];
			}

			m_pMesh[meshCnt].pIndexNumber = new int[m_pMesh[meshCnt].nNumPolygonVertex];
			int *IndexVertices = mesh->GetPolygonVertices();
			for (int i = 0; i < m_pMesh[meshCnt].nNumPolygonVertex; i++)
			{
				m_pMesh[meshCnt].pIndexNumber[i] = IndexVertices[i];
			}

			//法線//////////////////////////////////////////////////
			int LayerCount = mesh->GetLayerCount();
			for (int i = 0; i < LayerCount; i++)
			{
				FbxLayer *layer = mesh->GetLayer(i);
				FbxLayerElementNormal *normalLayer = layer->GetNormals();
				if (normalLayer == 0)
				{
					continue;//法線ない
				}

				//法線の数・インデックス
				int normalCount = normalLayer->GetDirectArray().GetCount();

				m_pMesh[meshCnt].pNormal = new VECTOR3[normalCount];

				//マッピングモード・リファレンスモード取得
				FbxLayerElement::EMappingMode mappingMode = normalLayer->GetMappingMode();
				FbxLayerElement::EReferenceMode referencegMode = normalLayer->GetReferenceMode();

				if (mappingMode == FbxLayerElement::eByPolygonVertex)
				{
					if (referencegMode == FbxLayerElement::eDirect)
					{
						for (int clusterCnt = 0; clusterCnt < normalCount; clusterCnt++)
						{
							m_pMesh[meshCnt].pNormal[clusterCnt].x = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[0];
							m_pMesh[meshCnt].pNormal[clusterCnt].y = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[1];
							m_pMesh[meshCnt].pNormal[clusterCnt].z = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[2];
						}
					}
				}
				else if (mappingMode == FbxLayerElement::eByControlPoint)
				{
					if (referencegMode == FbxLayerElement::eDirect)
					{
						for (int clusterCnt = 0; clusterCnt <normalCount; clusterCnt++)
						{
							m_pMesh[meshCnt].pNormal[clusterCnt].x = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[0];
							m_pMesh[meshCnt].pNormal[clusterCnt].y = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[1];
							m_pMesh[meshCnt].pNormal[clusterCnt].z = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[2];
						}
					}
				}

				//UV//////////////////////////////////////////////////
				FbxLayerElementUV *layerUV = layer->GetUVs();
				if (layerUV == 0)
				{
					continue;
				}

				//UV取得
				// UVの数・インデックス
				int UVNum = layerUV->GetDirectArray().GetCount();
				int indexNumUV = layerUV->GetIndexArray().GetCount();
				m_pMesh[meshCnt].pTex = new VECTOR2[indexNumUV];

				// 頂点に格納されている全UVセットを名前で取得
				FbxStringList uvsetName;
				mesh->GetUVSetNames(uvsetName);

				// マッピングモード・リファレンスモード別にUV取得
				FbxLayerElement::EMappingMode	mappingModeUV = layerUV->GetMappingMode();
				FbxLayerElement::EReferenceMode	refModeUV = layerUV->GetReferenceMode();

				if (mappingModeUV == FbxLayerElement::eByPolygonVertex)
				{
					if (refModeUV == FbxLayerElement::eDirect)
					{
						// 直接取得
						for (int i = 0; i < indexNumUV; ++i)
						{
							m_pMesh[meshCnt].pTex[i].x = 1 - (float)layerUV->GetDirectArray().GetAt(i)[0];
							m_pMesh[meshCnt].pTex[i].y = 1 - (float)layerUV->GetDirectArray().GetAt(i)[1];
						}
					}
					else
					{
						if (refModeUV == FbxLayerElement::eIndexToDirect)
						{
							// インデックスから取得
							for (int i = 0; i < indexNumUV; ++i) {
								int index = layerUV->GetIndexArray().GetAt(i);
								m_pMesh[meshCnt].pTex[i].x = (float)layerUV->GetDirectArray().GetAt(index)[0];
								m_pMesh[meshCnt].pTex[i].y = (float)layerUV->GetDirectArray().GetAt(index)[1];
							}
						}
					}
				}
			}

			src = NULL;
			IndexVertices = NULL;

			//マテリアル紐付け//////////////////////////////////////////////////
			FbxLayerElementArrayTemplate<int>* matIndexs;
			char* pUvName;
			if (mesh->GetMaterialIndices(&matIndexs) && matIndexs->GetCount() > 0)
			{
				auto material = mesh->GetNode()->GetMaterial(matIndexs->GetAt(0));
				pUvName = (char*)material->GetName();
			}


			for (int i = 0; i < nNumMaterial; i++)
			{
				if (m_pMaterial[i].pMaterialName == pUvName)
				{
					if (m_pMaterial[i].pFileName != "NULL")
					{
						m_pMesh[meshCnt].pFileName = (char*)m_pMaterial[i].pFileName.data();
						m_pMesh[meshCnt].color = m_pMaterial[i].color;
					}
				}
			}

			//メッシュスキン数の取得
			int nNumDeformer = mesh->GetDeformerCount(FbxDeformer::eSkin);

			for (int i = 0; i < nNumDeformer; i++)
			{
				//スキン情報を取得
				FbxSkin* pSkin = (FbxSkin*)mesh->GetDeformer(i, FbxDeformer::eSkin);

				//スキンのクラスター(ボーン)の数を取得する。
				int nNumCluster = pSkin->GetClusterCount();

				m_pMesh[meshCnt].pCluster = new Cluster[nNumCluster];

				m_pMesh[meshCnt].nNumCluster = nNumCluster;

				m_pMesh[meshCnt].pWeight = new VECTOR4[m_pMesh[meshCnt].nNumVertex];
				m_pMesh[meshCnt].pBoneIndex = new VECTOR4[m_pMesh[meshCnt].nNumVertex];

				for (int k = 0; k < m_pMesh[meshCnt].nNumVertex; k++)
				{
					for (int i = 0; i < 4; i++)
					{
						m_pMesh[meshCnt].weight[i][k] = 0;
						m_pMesh[meshCnt].boneIndex[i][k] = 0;

						m_pMesh[meshCnt].pWeight[i] = VECTOR4(0,0,0,0);
						m_pMesh[meshCnt].pBoneIndex[i] = VECTOR4(0, 0, 0, 0);
					}
				}

				bool bUse[4][5000] = { false };
				int BoneCnt[5000] = { 0 };

				for (int boneCnt = 0; boneCnt < nNumCluster; boneCnt++)
				{
					FbxCluster* cluster = pSkin->GetCluster(boneCnt);

					//クラスター頂点インデックスとウェイト数を取得
					int controlPointIndexCount = cluster->GetControlPointIndicesCount();
					int* pointAry = cluster->GetControlPointIndices();
					double* weightAry = cluster->GetControlPointWeights();

					m_pMesh[meshCnt].pCluster[boneCnt].Index = new int[controlPointIndexCount];
					m_pMesh[meshCnt].pCluster[boneCnt].weight = new float[controlPointIndexCount];

					for (int indexCnt = 0; indexCnt < controlPointIndexCount; indexCnt++)
					{
						//頂点インデックスとウェイトの配列取得
						m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt] = pointAry[indexCnt];
						m_pMesh[meshCnt].pCluster[boneCnt].weight[indexCnt] = (float)weightAry[indexCnt];

						if (bUse[BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]]][m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]])
						{
								BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]] ++;
						}

						if (BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]] < 4)
						{
							m_pMesh[meshCnt].weight[BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]]][m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]] = (float)weightAry[indexCnt];

							m_pMesh[meshCnt].boneIndex[BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]]][m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]] = boneCnt;

							bUse[BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]]][m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]] = true;
						}
					}

					m_pMesh[meshCnt].pCluster[boneCnt].pMatrix = new XMMATRIX*[m_nNumAnime];

					for (int animeCnt = 0; animeCnt < m_nNumAnime; animeCnt++)
					{
						FbxAnimStack* pStack = pScene->GetSrcObject<FbxAnimStack>(animeCnt);
						pScene->SetCurrentAnimationStack(pStack);
						m_pMesh[meshCnt].pCluster[boneCnt].pMatrix[animeCnt] = new XMMATRIX[m_pAnime[animeCnt].nEndTime];
						FbxAMatrix initMat;
						FbxAMatrix Mat;

						//初期姿勢行列を取得
						cluster->GetTransformLinkMatrix(initMat);
						cluster->GetTransformMatrix(Mat);

						XMVECTOR Determinant;
						XMMATRIX init = XMMatrixInverse(&Determinant, SetMatrix(initMat));
						XMMATRIX mat = SetMatrix(Mat);
						//XMMATRIX init = SetMatrix(initMat);

						//クラスターからノード取得
						FbxNode* node = cluster->GetLink();

						FbxTime period;
						period.SetTime(0, 0, 0, 1, 0, FbxTime::eFrames24);

						for (int frameCnt = 0; frameCnt <m_pAnime[animeCnt].nEndTime; frameCnt++)
						{
							m_pMesh[meshCnt].pCluster[boneCnt].pMatrix[animeCnt][frameCnt] = init * SetMatrix(node->EvaluateGlobalTransform(frameCnt * period.Get()));
						}
					}
				}

			}

			FbxNode* n = mesh->GetNode();
			//if (meshCnt != 6)
			{
				m_pMesh[meshCnt].LclPos.x = n->LclTranslation.Get().mData[0];
				m_pMesh[meshCnt].LclPos.y = n->LclTranslation.Get().mData[1];
				m_pMesh[meshCnt].LclPos.z = n->LclTranslation.Get().mData[2];

				m_pMesh[meshCnt].LclRot.x = n->LclRotation.Get().mData[0];
				m_pMesh[meshCnt].LclRot.y = n->LclRotation.Get().mData[1];
				m_pMesh[meshCnt].LclRot.z = n->LclRotation.Get().mData[2];

				m_pMesh[meshCnt].LclScl.x = n->LclScaling.Get().mData[0];
				m_pMesh[meshCnt].LclScl.y = n->LclScaling.Get().mData[1];
				m_pMesh[meshCnt].LclScl.z = n->LclScaling.Get().mData[2];
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//メッシュの取得
///////////////////////////////////////////////////////////////////////////////
SkinMeshModel::Mesh *SkinMeshModel::GetMesh(void)
{
	return m_pMesh;
}

///////////////////////////////////////////////////////////////////////////////
//アニメ―ションの取得
///////////////////////////////////////////////////////////////////////////////
SkinMeshModel::Anime *SkinMeshModel::GetAnime(void)
{
	return m_pAnime;
}

///////////////////////////////////////////////////////////////////////////////
//メッシュ数の取得
///////////////////////////////////////////////////////////////////////////////
int SkinMeshModel::GetNumMesh(void)
{
	return m_nNumMesh;
}

///////////////////////////////////////////////////////////////////////////////
//アニメーション数の取得
///////////////////////////////////////////////////////////////////////////////
int SkinMeshModel::GetNumAnime(void)
{
	return m_nNumAnime;
}

///////////////////////////////////////////////////////////////////////////////
//行列
///////////////////////////////////////////////////////////////////////////////
XMMATRIX SkinMeshModel::SetMatrix(FbxAMatrix mat)
{
	XMMATRIX out;

	out._11 = (float)mat.mData[0][0];
	out._12 = (float)mat.mData[0][1];
	out._13 = (float)mat.mData[0][2];
	out._14 = (float)mat.mData[0][3];

	out._21 = (float)mat.mData[1][0];
	out._22 = (float)mat.mData[1][1];
	out._23 = (float)mat.mData[1][2];
	out._24 = (float)mat.mData[1][3];

	out._31 = (float)mat.mData[2][0];
	out._32 = (float)mat.mData[2][1];
	out._33 = (float)mat.mData[2][2];
	out._34 = (float)mat.mData[2][3];

	out._41 = (float)mat.mData[3][0];
	out._42 = (float)mat.mData[3][1];
	out._43 = (float)mat.mData[3][2];
	out._44 = (float)mat.mData[3][3];

	return out;
}
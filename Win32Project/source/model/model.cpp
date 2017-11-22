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
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
//�R���X�g���N�^
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
//�f�X�g���N�^
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
//�I��
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
//�t�@�C���l�[���擾
///////////////////////////////////////////////////////////////////////////////
const char* Model::GetFileName()
{
	return m_pFileName;
}

///////////////////////////////////////////////////////////////////////////////
//�ǂݍ���
///////////////////////////////////////////////////////////////////////////////
void SkinMeshModel::LoadFile(const  char* FilenName)
{
	//------------------------------------
	//FBXSDK������
	//------------------------------------
	FbxManager *pSdkManager = FbxManager::Create();		//fbxsdkmanager�I�u�W�F�N�g����

	FbxImporter *pImporter = FbxImporter::Create(pSdkManager, "");		//�C���|�[�^�̍쐬
	if (pImporter == NULL) {
		printf("�C���|�[�g�쐬���s���܂���\n");
		getchar();
		pSdkManager->Destroy();
		return;
	}

	const char *pFileName = FilenName;
	int FileFormat = -1;
	if (!pImporter->Initialize(pFileName)) {
		printf("�t�@�C���ǂݍ��ݎ��s���܂���\n");
		getchar();
		pSdkManager->Destroy();
		return;
	}

	FbxScene *pScene = FbxScene::Create(pSdkManager, "");
	if (pScene == NULL) {
		printf("�V�[���쐬���s���܂���\n");
		getchar();
		pSdkManager->Destroy();
		return;
	}

	if (!pImporter->Import(pScene)) {
		printf("�t�@�C������V�[���ǂݍ��ݎ��s���܂���\n");
		getchar();
		pSdkManager->Destroy();
		return;
	}

	pImporter->Destroy();//�C���|�[�^�[�̔j��

	FbxGeometryConverter converter(pSdkManager);

	// ���炩���߃|���S����S�ĎO�p�`�����Ă���
	converter.Triangulate(pScene, true);

	// �}�e���A�����ƂɃ��b�V���𕪗�
	converter.SplitMeshesPerMaterial(pScene, true);

	//------------------------------------
	//FBXSDK�ǂݍ���
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
	//�A�j���[�V����
	//------------------------------------
	for (int i = 0; i < m_nNumAnime; i++)
	{
		//FbxtakeInfo�擾
		auto take_info = pScene->GetTakeInfo(*(animation_names[i]));

		//�J�n����
		m_pAnime[i].nStartTime = take_info->mLocalTimeSpan.GetStart().Get() / FbxTime::GetOneFrameValue(FbxTime::eFrames24);

		//�I������
		m_pAnime[i].nEndTime = take_info->mLocalTimeSpan.GetStop().Get() / FbxTime::GetOneFrameValue(FbxTime::eFrames24);
	}

	//------------------------------------
	//�}�e���A��
	//------------------------------------
	for (int i = 0; i < nNumMaterial; i++)
	{
		FbxSurfaceLambert* lambert;
		FbxSurfacePhong* phong;

		FbxSurfaceMaterial* material = pScene->GetMaterial(i);
		// material��KFbxSurfaceMaterial�I�u�W�F�N�g
		m_pMaterial[i].pMaterialName = (char*)material->GetName();

		// Lambert��Phong��
		if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			// Lambert�Ƀ_�E���L���X�g
			lambert = (FbxSurfaceLambert*)material;

			FbxDouble3 diffuse = lambert->Diffuse;
			m_pMaterial[i].color.x = diffuse[0];
			m_pMaterial[i].color.y = diffuse[1];
			m_pMaterial[i].color.z = diffuse[2];
			m_pMaterial[i].color.w = diffuse[3];
		}
		else if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			// Phong�Ƀ_�E���L���X�g
			phong = (FbxSurfacePhong*)material;
		}
		

		// �f�B�t���[�Y�v���p�e�B������
		FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

		// �v���p�e�B�������Ă��郌�C���[�h�e�N�X�`���̖������`�F�b�N
		int layerNum = property.GetSrcObjectCount(FbxLayeredTexture::ClassId);
		// ���C���[�h�e�N�X�`����������Βʏ�e�N�X�`��
		if (layerNum == 0)
		{
			// �ʏ�e�N�X�`���̖������`�F�b�N
			int numGeneralTexture = property.GetSrcObjectCount(FbxTexture::ClassId);
			m_pMesh[i].pFileName = NULL;
			// �e�e�N�X�`���ɂ��ăe�N�X�`�������Q�b�g
			for (int clusterCnt = 0; clusterCnt < numGeneralTexture; clusterCnt++)
			{
				// i�Ԗڂ̃e�N�X�`���I�u�W�F�N�g�擾
				FbxFileTexture* texture = FbxCast<FbxFileTexture>(property.GetSrcObject(FbxFileTexture::ClassId, clusterCnt));

				// material��KFbxSurfaceMaterial�I�u�W�F�N�g
				const char* materialName = texture->GetName();

				// �e�N�X�`���t�@�C���p�X���擾�i�t���p�X�j
				std::string relFileName = texture->GetRelativeFileName();

				//int path_i = relFileName.find_last_of("/");//7
				int path_i = relFileName.find_last_of("\\");//7
				int ext_i = relFileName.find_last_of(".");//10
				std::string pathname = relFileName.substr(0, path_i);//0�����ڂ���V�����؂�o�� "C:\\aaa\\"
				std::string extname = relFileName.substr(ext_i+1, relFileName.size() - ext_i); // 10�����ڂ���S�����؂�o�� ".txt"
				std::string filename = relFileName.substr(path_i+1, ext_i - path_i);// �V�����ڂ���R�����؂�o���@"bbb"

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
								// �e�N�X�`���t�@�C���p�X���擾�i�t���p�X�j
								std::string relFileName = pFileTexture->GetRelativeFileName();

								int path_i = relFileName.find_last_of("/");//7
								int ext_i = relFileName.find_last_of(".");//10
								std::string pathname = relFileName.substr(0, path_i + 1);//0�����ڂ���V�����؂�o�� "C:\\aaa\\"
								std::string extname = relFileName.substr(ext_i, relFileName.size() - ext_i); // 10�����ڂ���S�����؂�o�� ".txt"
								std::string filename = relFileName.substr(path_i, ext_i - path_i);// �V�����ڂ���R�����؂�o���@"bbb"

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
	//���b�V��
	//------------------------------------
	for (int meshCnt = 0; meshCnt < m_nNumMesh; meshCnt++)
	{
		mesh = pScene->GetSrcObject<FbxMesh>(meshCnt);
		if (mesh != NULL)
		{
			//���_//////////////////////////////////////////////////
			m_pMesh[meshCnt].nNumPolygon = mesh->GetPolygonCount();//���|���S����
			m_pMesh[meshCnt].nNumPolygonVertex = mesh->GetPolygonVertexCount();//�|���S�����_�C���f�b�N�X
			m_pMesh[meshCnt].nNumVertex = mesh->GetControlPointsCount();//���_��
			FbxVector4 *src = mesh->GetControlPoints();//���_���W�z��

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

			//�@��//////////////////////////////////////////////////
			int LayerCount = mesh->GetLayerCount();
			for (int i = 0; i < LayerCount; i++)
			{
				FbxLayer *layer = mesh->GetLayer(i);
				FbxLayerElementNormal *normalLayer = layer->GetNormals();
				if (normalLayer == 0)
				{
					continue;//�@���Ȃ�
				}

				//�@���̐��E�C���f�b�N�X
				int normalCount = normalLayer->GetDirectArray().GetCount();

				m_pMesh[meshCnt].pNormal = new VECTOR3[normalCount];

				//�}�b�s���O���[�h�E���t�@�����X���[�h�擾
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

				//UV�擾
				// UV�̐��E�C���f�b�N�X
				int UVNum = layerUV->GetDirectArray().GetCount();
				int indexNumUV = layerUV->GetIndexArray().GetCount();
				m_pMesh[meshCnt].pTex = new VECTOR2[indexNumUV];

				// ���_�Ɋi�[����Ă���SUV�Z�b�g�𖼑O�Ŏ擾
				FbxStringList uvsetName;
				mesh->GetUVSetNames(uvsetName);

				// �}�b�s���O���[�h�E���t�@�����X���[�h�ʂ�UV�擾
				FbxLayerElement::EMappingMode	mappingModeUV = layerUV->GetMappingMode();
				FbxLayerElement::EReferenceMode	refModeUV = layerUV->GetReferenceMode();

				if (mappingModeUV == FbxLayerElement::eByPolygonVertex)
				{
					if (refModeUV == FbxLayerElement::eDirect)
					{
						// ���ڎ擾
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
							// �C���f�b�N�X����擾
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

			//�}�e���A���R�t��//////////////////////////////////////////////////
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

			//���b�V���X�L�����̎擾
			int nNumDeformer = mesh->GetDeformerCount(FbxDeformer::eSkin);

			for (int i = 0; i < nNumDeformer; i++)
			{
				//�X�L�������擾
				FbxSkin* pSkin = (FbxSkin*)mesh->GetDeformer(i, FbxDeformer::eSkin);

				//�X�L���̃N���X�^�[(�{�[��)�̐����擾����B
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

					//�N���X�^�[���_�C���f�b�N�X�ƃE�F�C�g�����擾
					int controlPointIndexCount = cluster->GetControlPointIndicesCount();
					int* pointAry = cluster->GetControlPointIndices();
					double* weightAry = cluster->GetControlPointWeights();

					m_pMesh[meshCnt].pCluster[boneCnt].Index = new int[controlPointIndexCount];
					m_pMesh[meshCnt].pCluster[boneCnt].weight = new float[controlPointIndexCount];

					for (int indexCnt = 0; indexCnt < controlPointIndexCount; indexCnt++)
					{
						//���_�C���f�b�N�X�ƃE�F�C�g�̔z��擾
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

						//�����p���s����擾
						cluster->GetTransformLinkMatrix(initMat);
						cluster->GetTransformMatrix(Mat);

						XMVECTOR Determinant;
						XMMATRIX init = XMMatrixInverse(&Determinant, SetMatrix(initMat));
						XMMATRIX mat = SetMatrix(Mat);
						//XMMATRIX init = SetMatrix(initMat);

						//�N���X�^�[����m�[�h�擾
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
//���b�V���̎擾
///////////////////////////////////////////////////////////////////////////////
SkinMeshModel::Mesh *SkinMeshModel::GetMesh(void)
{
	return m_pMesh;
}

///////////////////////////////////////////////////////////////////////////////
//�A�j���\�V�����̎擾
///////////////////////////////////////////////////////////////////////////////
SkinMeshModel::Anime *SkinMeshModel::GetAnime(void)
{
	return m_pAnime;
}

///////////////////////////////////////////////////////////////////////////////
//���b�V�����̎擾
///////////////////////////////////////////////////////////////////////////////
int SkinMeshModel::GetNumMesh(void)
{
	return m_nNumMesh;
}

///////////////////////////////////////////////////////////////////////////////
//�A�j���[�V�������̎擾
///////////////////////////////////////////////////////////////////////////////
int SkinMeshModel::GetNumAnime(void)
{
	return m_nNumAnime;
}

///////////////////////////////////////////////////////////////////////////////
//�s��
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
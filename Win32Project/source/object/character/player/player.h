//=============================================================================
//
// player.h
// Author : SHOTA FUKUOKA
//
//=============================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include <Windows.h>
#include "../character.h"
#include "../../../model/model_manager.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
class ShaderManager;
class RenderManager;
class TextureManager;
class ModelManager;

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class Player : public Character
{
public:

	//�f�X�g���N�^
	virtual ~Player();

	//�C���X�^���X����
	static void CreateInstance(RenderManager* pRenderManager,
		ShaderManager* pShaderManager,
		TextureManager* pTextureManager,
		ModelManager* pModelManager,
		AppRenderer::Constant* pConstant,
		AppRenderer::Constant* pLightCameraConstant);

	//�C���X�^���X�擾
	static Player* GetInstance(void);

	//������
	HRESULT Init(void);

	//�I��
	void Release(void);

	//�X�V
	void Update(void);

	//�o�b�t�@�쐬
	void MakeVertex(int nMeshCount, SkinMeshModel::Mesh* pMesh);

	//�ۑ�
	void SaveFile(char* FileName);

	typedef struct
	{
		int					nStartTime;

		int					nEndTime;

		bool				bStop;
	}ANIME_CLIP;

	struct SaveCluster
	{
		XMMATRIX  pMatrix[32][1024];
	};

	struct SaveMesh
	{
		int					nNumCluster;
		int					nNumVertex;
		int					nNumPolygonVertex;
		char				FileName[256];
		int					IndexNumber[15000];
		VECTOR3				position[3000];
		VECTOR3				normal[15000];
		VECTOR4				color;
		VECTOR2				tex[15000];
		VECTOR4				boneIndex[3000];
		VECTOR4				weight[3000];
		SaveCluster*		cluster;
		VECTOR3				LclPos;
		VECTOR3				LclRot;
		VECTOR3				LclScl;

	};

private:
	//�R���X�g���N�^
	Player(RenderManager* pRenderManager,
		ShaderManager* pShaderManager,
		TextureManager* pTextureManager,
		ModelManager* pModelManager,
		AppRenderer::Constant* pConstant,
		AppRenderer::Constant* pLightCameraConstant);

	static Player*		m_pPlayer;

	SaveMesh*			m_pSaveMesh;

	ANIME_CLIP*			m_pAnimeClip;

	SkinMeshModel*		m_pModel;

	SkinMeshModel::Mesh*		m_pMesh;

	int*				m_pFrame;

	int*				m_pAnimeNumber;

	int					m_nOldStartTime;

	int					m_nOldEndTime;

	int					m_nNumClip;
};

#endif
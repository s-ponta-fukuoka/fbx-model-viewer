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

	void LoadFile(char* FileName);

	typedef struct
	{
		int					nStartTime;

		int					nEndTime;

		bool				bStop;
	}ANIME_CLIP;

private:
	//�R���X�g���N�^
	Player(RenderManager* pRenderManager,
		ShaderManager* pShaderManager,
		TextureManager* pTextureManager,
		ModelManager* pModelManager,
		AppRenderer::Constant* pConstant,
		AppRenderer::Constant* pLightCameraConstant);

	static Player*		m_pPlayer;

	ANIME_CLIP*			m_pAnimeClip;

	SkinMeshModel*		m_pModel;

	int*				m_pFrame;

	int*				m_pAnimeNumber;

	int					m_nOldStartTime;

	int					m_nOldEndTime;

	int					m_nNumClip;
};

#endif
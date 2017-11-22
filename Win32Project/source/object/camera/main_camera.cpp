//=============================================================================
//
// camera.cpp
// Author : shota fukuoka
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include "main_camera.h"
#include "../../../source/app/app.h"
#include "../../device/input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
///////////////////////////////////////////////////////////////////////////////
MainCamera::MainCamera(VECTOR3 position, VECTOR3 positionAt, VECTOR3 vectorUp)
{
	m_pTransform->position = position;

	m_positionAt = positionAt;

	m_vectorUp = vectorUp;

	m_fLength = -500.0f;
}

///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
///////////////////////////////////////////////////////////////////////////////
MainCamera::~MainCamera()
{
	;
}

///////////////////////////////////////////////////////////////////////////////
// ������
///////////////////////////////////////////////////////////////////////////////
HRESULT MainCamera::Init(void )
{
	Camera::Init();
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// �I��
///////////////////////////////////////////////////////////////////////////////
void MainCamera::Release(void)
{
	Camera::Release();
}

///////////////////////////////////////////////////////////////////////////////
// �X�V
///////////////////////////////////////////////////////////////////////////////
void MainCamera::Update(void)
{
	Camera::Update();

	InputMouse*	pInputMouse = InputMouse::GetInstance();

	InputKeyboard* pInputKeyboard = InputKeyboard::GetInstance();

	if (pInputMouse->GetLeftPress() && pInputKeyboard->GetKeyPress(DIK_LALT))
	{
		LONG axisX = pInputMouse->GetAxisX();
		LONG axisY = pInputMouse->GetAxisY();
	
		m_pTransform->rot.x += -D3D_PI * ((float)axisY / SCREEN_HEIGHT);
		m_pTransform->rot.y += -D3D_PI * ((float)axisX / SCREEN_WIDTH);
	}
	
	//Y����]�̍s������߂�
	XMMATRIX mtxRotY;
	mtxRotY = XMMatrixRotationY(m_pTransform->rot.y);
	//X����]�̍s������߂�
	XMMATRIX mtxRotX;
	mtxRotX = XMMatrixRotationX(m_pTransform->rot.x);
	//Y����]�AX����]���������킹��
	XMMATRIX mtxRot;
	mtxRot = mtxRotX * mtxRotY;
	
	// �o�͈ʒu �����ʒu ��]��
	XMVECTOR pos = XMVector3TransformCoord(XMVectorSet(0.0f, 20.0f, m_fLength, 0.0f), mtxRot);
	
	m_pTransform->position.x = XMVectorGetX(pos);
	m_pTransform->position.y = XMVectorGetY(pos);
	m_pTransform->position.z = XMVectorGetZ(pos);
	
	m_fLength += pInputMouse->GetAxisZ() * -0.01f;
}

///////////////////////////////////////////////////////////////////////////////
// �J�����Z�b�g
///////////////////////////////////////////////////////////////////////////////
void MainCamera::SetCamera(void)
{
	XMMATRIX hView = XMMatrixIdentity();
	XMVECTOR hEye = XMVectorSet(m_pTransform->position.x, m_pTransform->position.y, m_pTransform->position.z, 0.0f);	//�J�����̈ʒu
	XMVECTOR hAt = XMVectorSet(m_positionAt.x, m_positionAt.y, m_positionAt.z, 0.0f);		//�œ_�̈ʒu
	XMVECTOR hUp = XMVectorSet(m_vectorUp.x, m_vectorUp.y, m_vectorUp.z, 0.0f);
	hView = XMMatrixLookAtLH(hEye, hAt, hUp);

	XMMATRIX hProjection;	//�����ˉe�ϊ��s��
	hProjection = XMMatrixPerspectiveFovLH(D3D_PI / 3.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 30000.0f);

	XMVECTOR LightPos = XMVectorSet(0, 1.f, -1.0f, 1);
	m_pConstant->light = LightPos;

	m_pConstant->view = hView;
	m_pConstant->projection = hProjection;
}

///////////////////////////////////////////////////////////////////////////////
//�萔�擾
///////////////////////////////////////////////////////////////////////////////
AppRenderer::Constant* MainCamera::GetConstant(void)
{
	return m_pConstant;
}
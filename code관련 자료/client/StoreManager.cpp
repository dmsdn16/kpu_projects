#include "StoreManager.h"

StoreManager::StoreManager()
{

}
StoreManager::~StoreManager()
{

}



void StoreManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMMATRIX curView = XMLoadFloat4x4(&pCamera->GetViewMatrix());
	XMMATRIX curPro = XMLoadFloat4x4(&pCamera->GetProjectionMatrix());


	XMMATRIX UI = ::XMMatrixOrthographicLH(FRAME_BUFFER_WIDTH/2, FRAME_BUFFER_HEIGHT/2, 0.0f, 1.0f);
	XMMATRIX matView = XMMatrixIdentity();
	
	float fScale[3];
	fScale[0] = 150.f;	// �ؽ�ó�� X ũ�� ��
	fScale[1] = 150.f;	// �ؽ�ó�� Y ũ�� ��
	fScale[2] = 1.f;    // �ؽ�ó�� Z ũ�� ��(Z�� 1�� ��������)

	
	
	pCamera->SetViewMatrix(matView);
	pCamera->SetProMatrix(UI);

	__super::Render(pd3dCommandList, pCamera);

	pCamera->SetViewMatrix(curView);
	pCamera->SetProMatrix(curPro);
}



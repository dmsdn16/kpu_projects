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


	XMMATRIX UI = ::XMMatrixOrthographicLH(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.01f, 1.0f);
	XMMATRIX matView = XMMatrixIdentity();
	
	pCamera->SetViewMatrix(matView);
	pCamera->SetProMatrix(UI);

	__super::Render(pd3dCommandList, pCamera);

	pCamera->SetViewMatrix(curView);
	pCamera->SetProMatrix(curPro);

}



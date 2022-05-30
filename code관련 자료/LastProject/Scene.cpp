#include "stdafx.h"
#include "Scene.h"

std::random_device rd;
std::mt19937 gen(rd());


ID3D12DescriptorHeap *CScene::m_pd3dCbvSrvDescriptorHeap = NULL;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorStartHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorStartHandle;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorNextHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorNextHandle;

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 45); 

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature); 

	BuildDefaultLightsAndMaterials();

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	XMFLOAT3 xmf3Scale(10.0f, 2.0f, 10.0f);
	XMFLOAT4 xmf4Color(1.0f, 0.0f, 0.0f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Plain.raw"), 257, 257, xmf3Scale, xmf4Color);
	//m_pTerrain->Rotate(-10, 0, 0);

	m_nGameObjects = 6;
	m_ppGameObjects = new CGameObject*[m_nGameObjects];
	m_nPlateObjects = 1;
	m_ppPlateObjects = new CGameObject * [m_nPlateObjects];

	CLoadedModelInfo* pPlateModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/battle_plate.bin", NULL);
	m_ppPlateObjects[0] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlateModel, 1);
	m_ppPlateObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppPlateObjects[0]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.0f, 0.0f);
	m_ppPlateObjects[0]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
	m_ppPlateObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
	m_ppPlateObjects[0]->SetPosition(900.0f, 0.0f, 1400.0f);
	m_ppPlateObjects[0]->SetScale(130, 150, 0);
	m_ppPlateObjects[0]->Rotate(90, 0, 0);

	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/orcc.bin", NULL);
	m_ppGameObjects[0] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pAngrybotModel, 1);
	m_ppGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppGameObjects[0]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.0f, 3.0f);
	m_ppGameObjects[0]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
	m_ppGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
	m_ppGameObjects[0]->SetPosition(340.0f, 500.0f, 260.0f);
	m_ppGameObjects[0]->SetScale(200,200,200);
	//m_ppGameObjects[0]->Rotate(-30, 0, 0);
	//ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, m_ppGameObjects[0]);
	//ObjectManager::GetInstance()->PushObject(ObjectManager::OT_WARRIOR, m_ppGameObjects[0]);

	CLoadedModelInfo* pHumanThiefModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/HumanThiefWalk.bin", NULL);
	m_ppGameObjects[1] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pHumanThiefModel, 1);
	m_ppGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppGameObjects[1]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.0f, 3.0f);
	m_ppGameObjects[1]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
	m_ppGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.0f);
	m_ppGameObjects[1]->SetPosition(1460.0f,500.0f, 260.0f);
	m_ppGameObjects[1]->SetScale(200, 200, 200);
	//ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, m_ppGameObjects[1]);
	//ObjectManager::GetInstance()->PushObject(ObjectManager::OT_WARRIOR, m_ppGameObjects[1]);

	//CLoadedModelInfo* pHumanThiefModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/HumanThiefWalk.bin", NULL);
	m_ppGameObjects[2] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pHumanThiefModel, 1);
	m_ppGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppGameObjects[2]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.0f, 3.0f);
	m_ppGameObjects[2]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
	m_ppGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.0f);
	m_ppGameObjects[2]->SetPosition(1180.0f, 500.0f, 260.0f);
	m_ppGameObjects[2]->SetScale(200, 200, 200);
//	m_ppGameObjects[2]->Rotate(0, 0, 180);
	//ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, m_ppGameObjects[2]);
	//ObjectManager::GetInstance()->PushObject(ObjectManager::OT_WARRIOR, m_ppGameObjects[2]);

	CLoadedModelInfo* pOrcThiefWalkModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/OrcThiefWalk.bin", NULL);
	m_ppGameObjects[3] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pOrcThiefWalkModel, 1);
	m_ppGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppGameObjects[3]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.5f, 2.5f);
	m_ppGameObjects[3]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
	m_ppGameObjects[3]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
	m_ppGameObjects[3]->SetPosition(760.0f, 500.0f, 1540.0f);
	m_ppGameObjects[3]->SetScale(200, 200, 200);
	ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, m_ppGameObjects[3]);
    ObjectManager::GetInstance()->PushObject(ObjectManager::OT_WARRIOR, m_ppGameObjects[3]);

	CLoadedModelInfo* porcModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/orcc.bin", NULL);
	m_ppGameObjects[4] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, porcModel, 1);
	m_ppGameObjects[4]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppGameObjects[4]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.5f, 2.5f);
	m_ppGameObjects[4]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
	m_ppGameObjects[4]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
	m_ppGameObjects[4]->SetPosition(1180.0f, 650.0f, 1540.0f);
	m_ppGameObjects[4]->SetScale(200, 200, 200);
	ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, m_ppGameObjects[4]);
	ObjectManager::GetInstance()->PushObject(ObjectManager::OT_WARRIOR, m_ppGameObjects[4]);

	m_ppGameObjects[5] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pAngrybotModel, 1);
	m_ppGameObjects[5]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppGameObjects[5]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.5f, 2.5f);
	m_ppGameObjects[5]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
	m_ppGameObjects[5]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
	m_ppGameObjects[5]->SetPosition(340.0f, 650.0f, 1540.0f);
	m_ppGameObjects[5]->SetScale(200, 200, 200);
	ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, m_ppGameObjects[5]);
	ObjectManager::GetInstance()->PushObject(ObjectManager::OT_WARRIOR, m_ppGameObjects[5]);

	//제작 부분 수정 필수
	m_UI1 = new CGameObject * [4];
	m_UI2 = new CGameObject * [4];
	m_UI3 = new CGameObject * [4];
	m_UI4 = new CGameObject * [4];
	m_UI5 = new CGameObject * [4];

	//유닛
	Unit1 = new CGameObject * [4];
	Unit2 = new CGameObject * [4];
	Unit3 = new CGameObject * [4];
	Unit4 = new CGameObject * [4];
	Unit5 = new CGameObject * [4];

	//CLoadedModelInfo* pOrcThiefWalkModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/OrcThiefWalk.bin", NULL);

	for (int i = 0; i < 4; ++i)
	{
		Unit1[i] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pAngrybotModel, 1);
		Unit1[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		Unit1[i]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.5f, 2.5f);
		Unit1[i]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
		Unit1[i]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
		Unit1[i]->SetPosition(0.0f, 0.0f, 50.0f);
		Unit1[i]->SetScale(200, 200, 200);
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, Unit1[i]);
		UnitList1.push_back(Unit1[i]);
	}
	for (int i = 0; i < 4; ++i)
	{
		Unit2[i] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pOrcThiefWalkModel, 1);
		Unit2[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		Unit2[i]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.5f, 2.5f);
		Unit2[i]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
		Unit2[i]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
		Unit2[i]->SetPosition(0.0f, 0.0f, 50.0f);
		Unit2[i]->SetScale(200, 200, 200);
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, Unit2[i]);
		UnitList2.push_back(Unit2[i]);
	}
	for (int i = 0; i < 4; ++i)
	{
		Unit3[i] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pHumanThiefModel, 1);
		Unit3[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		Unit3[i]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.5f, 2.5f);
		Unit3[i]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
		Unit3[i]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
		Unit3[i]->SetPosition(0.0f, 0.0f, 0.0f);
		Unit3[i]->SetScale(200, 200, 200);
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, Unit3[i]);
		UnitList3.push_back(Unit3[i]);
	}
	for (int i = 0; i < 4; ++i)
	{
		Unit4[i] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pOrcThiefWalkModel, 1);
		Unit4[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		Unit4[i]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.5f, 2.5f);
		Unit4[i]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
		Unit4[i]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
		Unit4[i]->SetPosition(0.0f, 0.0f, 0.0f);
		Unit4[i]->SetScale(200, 200, 200);
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, Unit4[i]);
		UnitList4.push_back(Unit4[i]);
	}
	for (int i = 0; i < 4; ++i)
	{
		
		Unit5[i] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, porcModel, 1);
		Unit5[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		Unit5[i]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.5f, 2.5f);
		Unit5[i]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
		Unit5[i]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
		Unit5[i]->SetPosition(0.0f, 0.0f, 0.0f);
		Unit5[i]->SetScale(200, 200, 200);
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, Unit5[i]);
		UnitList5.push_back(Unit5[i]);
	}
	
	//UI상자
	CLoadedModelInfo* pUI1botModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/orcc.bin", NULL);
	CLoadedModelInfo* pUI2botModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/OrcThiefWalk.bin", NULL);
	CLoadedModelInfo* pUI3botModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/HumanThiefWalk.bin", NULL);
	//CLoadedModelInfo*pUI4botModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/orcc.bin", NULL);
	//CLoadedModelInfo* pUI5botModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/orcc.bin", NULL);
	for (int i = 0; i < m_U; ++i)
	{
		m_UI1[i] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pUI1botModel, 1);
		m_UI1[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_UI1[i]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.0f, 0.0f);
		m_UI1[i]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
		m_UI1[i]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.0f);
		m_UI1[i]->SetPosition(0.0f, 0.0f, 0.0f);
		m_UI1[i]->SetScale(100, 100, 100);
		m_UI1[i]->Rotate(45, 0, 180);
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UI, m_UI1[i]);

		m_UI2[i] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pUI2botModel, 1);
		m_UI2[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_UI2[i]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.0f, 0.0f);
		m_UI2[i]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
		m_UI2[i]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.0f);
		m_UI2[i]->SetPosition(0.0f, 0.0f, 0.0f);
		m_UI2[i]->SetScale(100, 100, 100);
		m_UI2[i]->Rotate(45, 0, 180);
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UI, m_UI2[i]);

		m_UI3[i] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pUI3botModel, 1);
		m_UI3[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_UI3[i]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.0f, 0.0f);
		m_UI3[i]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
		m_UI3[i]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.0f);
		m_UI3[i]->SetPosition(0.0f, 0.0f, 0.0f);
		m_UI3[i]->SetScale(100, 100, 100);
		m_UI3[i]->Rotate(45, 0, 180);
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UI, m_UI3[i]);

		m_UI4[i] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pUI2botModel, 1);
		m_UI4[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_UI4[i]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.0f, 0.0f);
		m_UI4[i]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
		m_UI4[i]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.0f);
		m_UI4[i]->SetPosition(0.0f, 0.0f, 0.0f);
		m_UI4[i]->SetScale(100, 100, 100);
		m_UI4[i]->Rotate(45, 0, 180);
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UI, m_UI4[i]);

		m_UI5[i] = new CAngrybotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pUI1botModel, 1);
		m_UI5[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_UI5[i]->m_pSkinnedAnimationController->SetTrackStartEndTime(0, 0.0f,0.0f);
		m_UI5[i]->m_pSkinnedAnimationController->SetTrackPosition(0, 0.55f);
		m_UI5[i]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.0f);
		m_UI5[i]->SetPosition(0.0f, 0.0f, 0.0f);
		m_UI5[i]->SetScale(100, 100, 100);
		m_UI5[i]->Rotate(45, 0, 180);
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UI, m_UI5[i]);
	}

	if (pAngrybotModel) delete pAngrybotModel;
	if (pPlateModel) delete pPlateModel;
	//if (pHumanArcherModel)delete pHumanArcherModel;
	if (pHumanThiefModel)delete pHumanThiefModel;
	if (porcModel)delete porcModel;
	if (pOrcThiefWalkModel) delete pOrcThiefWalkModel;
	//if (pOrcThiefWalkModel2) delete pOrcThiefWalkModel2;
	if (pUI1botModel) delete pUI1botModel;
	if (pUI2botModel) delete pUI2botModel;
	if (pUI3botModel) delete pUI3botModel;
	//if (pOrcModel) delete pOrcModel;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pTerrain) delete m_pTerrain;
	if (m_pSkyBox) delete m_pSkyBox;

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	if (m_ppPlateObjects)
	{
		for (int i = 0; i < m_nPlateObjects; i++) if (m_ppPlateObjects[i]) m_ppPlateObjects[i]->Release();
		delete[] m_ppPlateObjects;
	}

	if (Unit1)
	{
		for (int i = 0; i < 4; i++) if (Unit1[i]) Unit1[i]->Release();
		delete[] Unit1;
	}

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[10];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 6; //t6: gtxtAlbedoTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 7; //t7: gtxtSpecularTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 8; //t8: gtxtNormalTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 9; //t9: gtxtMetallicTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 10; //t10: gtxtEmissionTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 11; //t11: gtxtEmissionTexture
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 12; //t12: gtxtEmissionTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 13; //t13: gtxtSkyBoxTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[8].NumDescriptors = 1;
	pd3dDescriptorRanges[8].BaseShaderRegister = 1; //t1: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[9].NumDescriptors = 1;
	pd3dDescriptorRanges[9].BaseShaderRegister = 2; //t2: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[9].RegisterSpace = 0;
	pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[15];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 20;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[2]);
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[3]);
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[4]);
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[5]);
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[6]);
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[7]);
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[11].Descriptor.ShaderRegister = 7; //Skinned Bone Offsets
	pd3dRootParameters[11].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[12].Descriptor.ShaderRegister = 8; //Skinned Bone Transforms
	pd3dRootParameters[12].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[8]);
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[14].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[14].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[9]);
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nPlateObjects; i++) m_ppPlateObjects[i]->ReleaseUploadBuffers();
	for (int i = 0; i < 4; i++) Unit1[i]->ReleaseUploadBuffers();
}





// 리롤
void CScene::Reroll()
{
	ResetUi();

	u_count = 0; // ui위치
	x_count = 0; // ui 갯수

	for (int i = 0; i < 5; ++i)
	{
		UIBox[i] = uc[i];
	}
	//나중에 코스트별 분리 필요
	//std::cout << UIBox[0] << std::endl;
	for (int i = 0; i < 4; ++i)
	{
		int a = CalRoll();
		//std::cout << UIBox[0] << std::endl;
		switch (a)
		{
		case 1:

			m_UI1[x_count]->SetPosition(340 + 280 * u_count, 500, 50);
			x_count++;
			u_count++;
			--UIBox[0];

			break;
		case 2:

			m_UI2[x_count]->SetPosition(340 + 280 * u_count, 500, 50);
			x_count++;
			u_count++;
			--UIBox[1];
			break;
		case 3:

			m_UI3[x_count]->SetPosition(340 + 280 * u_count, 500, 50);
			x_count++;
			u_count++;
			--UIBox[2];
			break;
		case 4:

			m_UI4[x_count]->SetPosition(340 + 280 * u_count, 500, 50);
			x_count++;
			u_count++;
			--UIBox[3];
			break;
		case 5:

			m_UI5[x_count]->SetPosition(340 + 280 * u_count, 500, 50);
			x_count++;
			u_count++;
			--UIBox[4];
			break;
		default:
			break;
		}
	}
}

// 누적 확률을 이용한 확률계산
int CScene::CalRoll()
{
	int	allunit = 0;
	for (int i = 0; i < 5; ++i)
	{
		allunit += UIBox[i];
	}
	std::uniform_int_distribution<int> RandomDir(1, allunit);

	int ran = RandomDir(gen);

	int p[] = { UIBox[4],UIBox[3],UIBox[2],UIBox[1],UIBox[0], }; //5, 4, 3, 2, 1

	int cumulative = 0;

	//std::cout << allunit << std::endl;

	for (int i = 0; i < 5; i++)
	{
		cumulative += p[i];
		if (ran <= cumulative)
		{
			return 5 - i;
		}
	}

	// 확률은 언제나자신/전체
}

void CScene::AdmCount()
{
	//ui1c = 1;
	//ui2c = 1;
	//ui3c = 1;
	//ui4c = 1;
	//ui5c = 1;
}

void CScene::ResetUi()
{
	for (int i = 0; i < m_U; ++i)
	{
		m_UI1[i]->SetPosition((500 + 500), 3000, -100.0f);
		m_UI2[i]->SetPosition((500 + 500), 3000, -100.0f);
		m_UI3[i]->SetPosition((500 + 500), 3000, -100.0f);
		m_UI4[i]->SetPosition((500 + 500), 3000, -100.0f);
		m_UI5[i]->SetPosition((500 + 500), 3000, -100.0f);
	}
}

void CScene::UnitSell()
{
	for (int i = 0; i < m_U; ++i)
	{
		if ((m_pick->GetInstance()->IntersecUnit()) == Unit1[i])
		{
			for (int k = 0; k < 6; ++k)
			{
				if (m_pick->GetInstance()->IntersecUnit()->GetPosition().x == (1460 - (140 * k)))
				{
					Unit1[i]->SetPosition(0.0f, 3000.0f, 0.0f);
					std::cout << "유닛 1 판매" << std::endl;
					UnitList1.push_back(Unit1[i]);
					++uc[0];
					++Canbuy;
					array[k] = 0;
					ObjectManager::GetInstance()->RemoveObject(ObjectManager::OT_MY_UNIT, Unit1[i]);
					break;
				}
			}
		}
		if ((m_pick->GetInstance()->IntersecUnit()) == Unit2[i])
		{
			for (int k = 0; k < 6; ++k)
			{
				if (m_pick->GetInstance()->IntersecUnit()->GetPosition().x == (1460 - (140 * k)))
				{
					Unit2[i]->SetPosition(0.0f, 3000.0f, 0.0f);
					std::cout << "유닛 2 판매" << std::endl;
					UnitList2.push_back(Unit2[i]);
					++uc[1];
					++Canbuy;
					array[k] = 0;
					ObjectManager::GetInstance()->RemoveObject(ObjectManager::OT_MY_UNIT, Unit2[i]);
					break;
				}
			}
		}

		if ((m_pick->GetInstance()->IntersecUnit()) == Unit3[i])
		{
			for (int k = 0; k < 6; ++k)
			{
				if (m_pick->GetInstance()->IntersecUnit()->GetPosition().x == (1460 - (140 * k)))
				{
					Unit3[i]->SetPosition(0.0f, 3000.0f, 0.0f);
					std::cout << "유닛 3 판매" << std::endl;
					UnitList3.push_back(Unit3[i]);
					++uc[2];
					++Canbuy;
					array[k] = 0;
					ObjectManager::GetInstance()->RemoveObject(ObjectManager::OT_MY_UNIT, Unit3[i]);
					break;
				}
			}
		}


		if ((m_pick->GetInstance()->IntersecUnit()) == Unit4[i])
		{
			for (int k = 0; k < 6; ++k)
			{
				if (m_pick->GetInstance()->IntersecUnit()->GetPosition().x == (1460 - (140 * k)))
				{
					Unit4[i]->SetPosition(0.0f, 3000.0f, 0.0f);
					std::cout << "유닛 4 판매" << std::endl;
					UnitList4.push_back(Unit4[i]);
					++uc[3];
					++Canbuy;
					array[k] = 0;
					ObjectManager::GetInstance()->RemoveObject(ObjectManager::OT_MY_UNIT, Unit4[i]);
					break;
				}
			}
		}
		if ((m_pick->GetInstance()->IntersecUnit()) == Unit5[i])
		{
			for (int k = 0; k < 6; ++k)
			{
				if (m_pick->GetInstance()->IntersecUnit()->GetPosition().x == (1460 - (140 * k)))
				{
					//std::cout << UnitList5.back() << endl;
					Unit5[i]->SetPosition(0.0f, 3000.0f, 0.0f);
					std::cout << "유닛 5 판매" << std::endl;
					UnitList5.push_back(Unit5[i]);
					//std::cout << UnitList5.back() << endl;
					++uc[4];
					++Canbuy;
					array[k] = 0;
					ObjectManager::GetInstance()->RemoveObject(ObjectManager::OT_MY_UNIT, Unit5[i]);
					break;
				}
			}
		}

	}
}


// 여기 수정하면 끝!!!!!!!!!!!
void CScene::UnitBuy()
{
	if (Canbuy != 0)
	{
		for (int i = 0; i < m_U; ++i)
		{
			if ((m_pick->GetInstance()->IntersecTri()) == m_UI1[i])
			{
				--uc[0];
				if (0 <= uc[0])
				{
					--Canbuy;
					m_UI1[i]->SetPosition(0, 3000, 0);

					for (int j = 0; j < 4; ++j)
					{
						//std::cout << UnitList1.front() << std::endl;
						//std::cout << Unit1[j] << std::endl;
						if (UnitList1.front() == Unit1[j])
						{
							std::cout << "aaaa" << std::endl;
							for (int k = 0; k < 6; ++k)
							{
								if (array[k] == 0)
								{
									std::cout << k << std::endl;
									Unit1[j]->SetPosition(1460 - (140 * k),500, 420);
									std::cout << "유닛 1 구매" << std::endl;
									uic[0]++;
									UnitList1.pop_front();
									array[k] = 1;
									ObjectManager::GetInstance()->PushObject(ObjectManager::OT_MY_UNIT, Unit1[j]);
									break;
								}
							}break;
						}
					}
				}
				//std::cout << "asda" <<std::endl;
				//x = 200, z= 100 초기값
			}
			if ((m_pick->GetInstance()->IntersecTri()) == m_UI2[i])
			{
				--uc[1];
				if (0 <= uc[1])
				{
					--Canbuy;

					m_UI2[i]->SetPosition(0, 3000, 0);

					for (int j = 0; j < 4; ++j)
					{
						if (UnitList2.front() == Unit2[j])
						{

							for (int k = 0; k < 6; ++k)
							{
								if (array[k] == 0)
								{
									Unit2[j]->SetPosition(1460 - (140 * k), 500, 420);
									std::cout << "유닛 2 구매" << std::endl;
									uic[1]++;
									UnitList2.pop_front();
									array[k] = 1;
									ObjectManager::GetInstance()->PushObject(ObjectManager::OT_MY_UNIT, Unit2[j]);
									break;
								}
							}break;
						}
					}
				}
			}

			if ((m_pick->GetInstance()->IntersecTri()) == m_UI3[i])
			{
				--uc[2];
				if (0 <= uc[2])
				{
					--Canbuy;
					m_UI3[i]->SetPosition(0, 3000, 0);
					for (int j = 0; j < 4; ++j)
					{
						if (UnitList3.front() == Unit3[j])
						{
							for (int k = 0; k < 6; ++k)
							{
								if (array[k] == 0)
								{
									Unit3[j]->SetPosition(1460 - (140 * k), 500, 420);
									std::cout << "유닛 3 구매" << std::endl;
									UnitList3.pop_front();
									array[k] = 1;
									ObjectManager::GetInstance()->PushObject(ObjectManager::OT_MY_UNIT, Unit3[j]);
									break;
								}
							}break;
						}
					}
				}
			}
			if ((m_pick->GetInstance()->IntersecTri()) == m_UI4[i])
			{
				--uc[3];
				if (0 <= uc[3])
				{
					++count;
					--Canbuy;
					m_UI4[i]->SetPosition(0, 3000, 0);
					for (int j = 0; j < 4; ++j)
					{
						if (UnitList4.front() == Unit4[j])
						{
							for (int k = 0; k < 6; ++k)
							{
								if (array[k] == 0)
								{
									Unit4[j]->SetPosition(1460 - (140 * k), 500, 420);
									std::cout << "유닛 4 구매" << std::endl;
									UnitList4.pop_front();
									array[k] = 1;
									ObjectManager::GetInstance()->PushObject(ObjectManager::OT_MY_UNIT, Unit4[j]);
									break;
								}
							}break;
						}
					};
				}
			}

			if ((m_pick->GetInstance()->IntersecTri()) == m_UI5[i])
			{
				--uc[4];
				if (0 <= uc[4])
				{
					++count;
					--Canbuy;
					m_UI5[i]->SetPosition(0, 3000, 0);
					for (int j = 0; j < 4; ++j)
					{
						if (UnitList5.front() == Unit5[j])
						{
							for (int k = 0; k < 6; ++k)
							{
								if (array[k] == 0)
								{
									Unit5[j]->SetPosition(1460 - (140 * k), 500, 420);
									std::cout << "유닛 5 구매" << std::endl;
									UnitList5.pop_front();
									array[k] = 1;
									ObjectManager::GetInstance()->PushObject(ObjectManager::OT_MY_UNIT, Unit5[j]);
									//std::cout << UnitList5.front() << std::endl;
									//std::cout << Unit5[1] << std::endl;
									break;
								}
							}break;
						}
					}
				}
			}
		}
	}
	else
		std::cout << " 구매 불가 !!!" << std::endl;
}


void CScene::CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorNextHandle = m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorNextHandle = m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorNextHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorNextHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferViews(ID3D12Device *pd3dDevice, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_d3dCbvGPUDescriptorNextHandle;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		m_d3dCbvCPUDescriptorNextHandle.ptr = m_d3dCbvCPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);
		m_d3dCbvGPUDescriptorNextHandle.ptr = m_d3dCbvGPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
	}
	return(d3dCbvGPUDescriptorHandle);
}

D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
		case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
		case RESOURCE_TEXTURE2D_ARRAY:
			d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
			d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
			d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			break;
		case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
			d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
			d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
			d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
			d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
			d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
			d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
			break;
		case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
			d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
			d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
			d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			break;
		case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
			d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
			d3dShaderResourceViewDesc.Buffer.NumElements = 0;
			d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
			d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			break;
	}
	return(d3dShaderResourceViewDesc);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateShaderResourceViews(ID3D12Device *pd3dDevice, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorNextHandle;
	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		int nTextureType = pTexture->GetTextureType();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource *pShaderResource = pTexture->GetTexture(i);
			D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
			pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

			pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameter + i) : nRootParameter, m_d3dSrvGPUDescriptorNextHandle);
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
	return(d3dSrvGPUDescriptorHandle);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W': m_ppGameObjects[0]->MoveForward(+3.0f); break;
		case 'S': m_ppGameObjects[0]->MoveForward(-3.0f); break;
		case 'A': m_ppGameObjects[0]->MoveStrafe(-3.0f); break;
		case 'D': m_ppGameObjects[0]->MoveStrafe(+3.0f); break;
		case 'Q': m_ppGameObjects[0]->MoveUp(+3.0f); break;
		case 'R': m_ppGameObjects[0]->MoveUp(-3.0f); break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	
	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}

	if (start == 0)
	{
		Reroll();
		++start;
	}
	
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 3; i < m_nGameObjects; i++)
	{
		if (m_ppGameObjects[i])
		{
			m_ppGameObjects[i]->Animate(m_fElapsedTime);
			if (!m_ppGameObjects[i]->m_pSkinnedAnimationController) m_ppGameObjects[i]->UpdateTransform(NULL);
			m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
			//std::cout << m_ppGameObjects[i]->m_xmOOBB.Center.x << std::endl;
		}
	}

	for (int i = 0; i < m_nPlateObjects; i++)
	{
		if (m_ppPlateObjects[i])
		{
			m_ppPlateObjects[i]->Animate(m_fElapsedTime);
			if (!m_ppPlateObjects[i]->m_pSkinnedAnimationController) m_ppPlateObjects[i]->UpdateTransform(NULL);
			//m_ppPlateObjects[i]->Render(pd3dCommandList, pCamera);
			//std::cout << m_ppPlateObjects[i]->GetPosition().x << std::endl;
		}
	}
	for (int i = 0; i < m_U; ++i)
	{
		if (m_UI1[i])
		{
			m_UI1[i]->Animate(m_fElapsedTime);
			if (!m_UI1[i]->m_pSkinnedAnimationController)m_UI1[i]->UpdateTransform(NULL);
			m_UI1[i]->Render(pd3dCommandList, pCamera);
		}
		if (m_UI2[i])
		{
			m_UI2[i]->Animate(m_fElapsedTime);
			if (!m_UI2[i]->m_pSkinnedAnimationController)m_UI2[i]->UpdateTransform(NULL);
			m_UI2[i]->Render(pd3dCommandList, pCamera);
		}
		if (m_UI3[i])
		{
			m_UI3[i]->Animate(m_fElapsedTime);
			if (!m_UI3[i]->m_pSkinnedAnimationController)m_UI3[i]->UpdateTransform(NULL);
			m_UI3[i]->Render(pd3dCommandList, pCamera);
		}
		if (m_UI4[i])
		{
			m_UI4[i]->Animate(m_fElapsedTime);
			if (!m_UI4[i]->m_pSkinnedAnimationController)m_UI4[i]->UpdateTransform(NULL);
			m_UI4[i]->Render(pd3dCommandList, pCamera);
		}
		if (m_UI5[i]) 
		{
			m_UI5[i]->Animate(m_fElapsedTime);
			if (!m_UI5[i]->m_pSkinnedAnimationController)m_UI5[i]->UpdateTransform(NULL);
			m_UI5[i]->Render(pd3dCommandList, pCamera);
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		if (Unit1[i]) 
		{
			Unit1[i]->Animate(m_fElapsedTime);
			if (!Unit1[i]->m_pSkinnedAnimationController)Unit1[i]->UpdateTransform(NULL);
			Unit1[i]->Render(pd3dCommandList, pCamera);
		}
		if (Unit2[i])
		{
			Unit2[i]->Animate(m_fElapsedTime);
			if (!Unit2[i]->m_pSkinnedAnimationController)Unit2[i]->UpdateTransform(NULL);
			Unit2[i]->Render(pd3dCommandList, pCamera);
		}
		if (Unit3[i])
		{
			Unit3[i]->Animate(m_fElapsedTime);
			if (!Unit3[i]->m_pSkinnedAnimationController)Unit3[i]->UpdateTransform(NULL);
			Unit3[i]->Render(pd3dCommandList, pCamera);
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		if (Unit4[i])
		{
			Unit4[i]->Animate(m_fElapsedTime);
			if (!Unit4[i]->m_pSkinnedAnimationController)Unit4[i]->UpdateTransform(NULL);
			Unit4[i]->Render(pd3dCommandList, pCamera);
		}
		if (Unit5[i])
		{
			Unit5[i]->Animate(m_fElapsedTime);
			if (!Unit5[i]->m_pSkinnedAnimationController)Unit5[i]->UpdateTransform(NULL);
			Unit5[i]->Render(pd3dCommandList, pCamera);
		}
	}
	//std::cout << Unit1[0]->GetPosition().y << std::endl;

	//Unit1[0]->MoveForward(1.0f);

	if (FrameCount == 0)
	{
		//m_ppGameObjects[1]->MoveUp(1.4f);
		/*if (!s.empty())
		{
			int p = s.top().first;
			int q = s.top().second;
			zmap[s.top().first][s.top().second] = '*';
			std::cout << s.top().first << " y : " << s.top().second << std::endl;
			s.pop();
			if (!s.empty())
			{
				int k = p - s.top().first;
				int j = q - s.top().second;
				//std::cout << k << "y : "<< j << std::endl;
				TracePath(k, j,0);
			}
		}*/
		
	}

	list<CGameObject*> Mine = ObjectManager::GetInstance()->GetObjectList(ObjectManager::OT_MY_UNIT);

	//release 모드 너무 빨라서 필요
	if (FrameCount == 0)
	{
		if (GetStart())
		{
			count = 0;
			for (int i = 3; i < m_nGameObjects; i++)
			{
				//m_ppGameObjects[i]->MoveStrafe(14.0f);
				for (auto& k : Mine)
				{
					cmpdis = sqrt(pow((k->GetPosition().x - m_ppGameObjects[i]->GetPosition().x), 2) + pow((k->GetPosition().z - m_ppGameObjects[i]->GetPosition().z), 2));
					//std::cout << cmpdis << std::endl;
					if (cmpdis > 160)
					{
						count++;
					}

					if (count == (6 - Canbuy))
					{
						m_ppGameObjects[i]->Checking();
						count = 0;
					}
				}
			}
			ncount = 0;

			for (auto& k : Mine)
			{
				for (int i = 3; i < m_nGameObjects; i++)
				{
					cmpdis = sqrt(pow((k->GetPosition().x - m_ppGameObjects[i]->GetPosition().x), 2) + pow((k->GetPosition().z - m_ppGameObjects[i]->GetPosition().z), 2));
					if (cmpdis > 160)
					{
						ncount++;
					}

					if (ncount == 3)
					{
						k->CheckingOwn();
						ncount = 0;
					}
				}

			}
		}
	}
		//MakingMap();
		//count++;

		
	

	FrameCount++;

	if (FrameCount == 10) // 수치는 나중에 개선
		FrameCount = 0;
		
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera);
}


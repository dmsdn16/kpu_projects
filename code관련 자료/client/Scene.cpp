//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Player.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

//#define _WITH_TEXT_MODEL_FILE
#define _WITH_BINARY_MODEL_FILE

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	uniform_int_distribution<int>rd(0, 255);
	default_random_engine dre;

#ifdef _WITH_TEXT_MODEL_FILE
	CMesh* pUfoMesh = new CMesh(pd3dDevice, pd3dCommandList, "Models/UFO.txt", true);
	CMesh* pFlyerMesh = new CMesh(pd3dDevice, pd3dCommandList, "Models/FlyerPlayership.txt", true);
#endif
#ifdef _WITH_BINARY_MODEL_FILE
	CMesh* pUfoMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/UFO.bin", false);
	CMesh* pHouseMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/ams_house3.bin", false);
	CMesh* pFlyerMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/FlyerPlayership.bin", false);
	CMesh* pCityMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/ams_house4.bin", false);
	CMesh* pCubeMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/Cube.bin", false);
	CMesh* pCityMesh2 = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/ams_house5.bin", false);
	CMesh* pCityMesh3 = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/c4_bin.bin", false);
	CMesh* pCityMesh4 = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/city3_h13_LOD.bin", false);
	CMesh* pCityMesh5 = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/city3_h16_LOD.bin", false);
	CMesh* pCityMesh6 = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/city3_h18.bin", false);
	CMesh* pCityMesh7 = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/c4_pillar.bin", false);
	CMesh* pCityMesh8 = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/city3_h17.bin", false);
	CMesh* pCityMesh9 = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/city3_h14.bin", false);
	CMesh* pRoadMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/entrance_rail.bin", false);
	CMesh* pRoadMesh2 = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/road.bin", false);
	CMesh* pLampMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/c4_lamp.bin", false);
	CMesh* pBriMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/c4_bridge.bin", false);


#endif

	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다. 
	XMFLOAT3 xmf3Scale(8.0f, 2.0f, 8.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);
	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다. 
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메
	쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 17,
		17, xmf3Scale, xmf4Color);
#else
//지형을 하나의 격자 메쉬(257x257)로 생성한다. 
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("Assets/Image/Terrain/1HeightMap.raw"), 257, 257, 257,
		257, xmf3Scale, xmf4Color);
#endif

	m_nObjects = 64; // 건물 갯수
	m_ppObjects = new CGameObject * [m_nObjects];
	m_bObjects = new CGameObject * [1];
	m_mObjects = new CGameObject * [m_pObjects];
	m_doorObjects = new CGameObject * [1];
	CPseudoLightingShader* pShader = new CPseudoLightingShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_bObjects[0] = new CUfoObject(1);
	m_bObjects[0]->SetMesh(0, pUfoMesh);
	m_bObjects[0]->SetShader(pShader);
	m_bObjects[0]->SetScale(100.0f);
	m_bObjects[0]->SetPosition(1000.0f, 400.0f, 1000.0f);
	m_bObjects[0]->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));

	


	// 건물
	// area1
	for (int i = 0; i < 8; ++i)
	{
		float z = 100 + (100 * i);
		m_ppObjects[i] = new CBuildingObject();
		m_ppObjects[i]->SetMesh(0, pCityMesh);
		m_ppObjects[i]->SetShader(pShader);
		m_ppObjects[i]->SetScale(20.0f);
		m_ppObjects[i]->SetPosition(100.0f, m_pTerrain->GetHeight(100, z), z);
		m_ppObjects[i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
		m_ppObjects[i]->Rotate(0.0f, 90.0f, 0.0f);
	}
	for (int i = 0; i < 8; ++i)
	{
		float z = 100 + (100 * i);
		m_ppObjects[8 + i] = new CBuildingObject();
		m_ppObjects[8 + i]->SetMesh(0, pCityMesh);
		m_ppObjects[8 + i]->SetShader(pShader);
		m_ppObjects[8 + i]->SetScale(20.0f);
		m_ppObjects[8 + i]->SetPosition(300.0f, m_pTerrain->GetHeight(300, z), z);
		m_ppObjects[8 + i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
		m_ppObjects[8 + i]->Rotate(0.0f, -90.0f, 0.0f);
	}
	m_ppObjects[16] = new CBuildingObject();
	m_ppObjects[16]->SetMesh(0, pCityMesh2);
	m_ppObjects[16]->SetShader(pShader);
	m_ppObjects[16]->SetScale(20.0f);
	m_ppObjects[16]->SetPosition(150.0f, m_pTerrain->GetHeight(100, 700), 1000.0f);
	m_ppObjects[16]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_ppObjects[16]->Rotate(0.0f, 180.0f, 0.0f);

	m_ppObjects[17] = new CBuildingObject();
	m_ppObjects[17]->SetMesh(0, pCityMesh2);
	m_ppObjects[17]->SetShader(pShader);
	m_ppObjects[17]->SetScale(20.0f);
	m_ppObjects[17]->SetPosition(220.0f, m_pTerrain->GetHeight(100, 700), 1000.0f);
	m_ppObjects[17]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_ppObjects[17]->Rotate(0.0f, 180.0f, 0.0f);

	// area2
	for (int i = 0; i < 4; i++)
	{
		float z = 100 + 50 * i;
		m_ppObjects[18 + i] = new CBuildingObject();
		m_ppObjects[18 + i]->SetMesh(0, pCityMesh3);
		m_ppObjects[18 + i]->SetShader(pShader);
		m_ppObjects[18 + i]->SetScale(60.0f);
		m_ppObjects[18 + i]->SetPosition(1800.0f, m_pTerrain->GetHeight(1800, z), z);
		m_ppObjects[18 + i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
		//	m_ppObjects[18 + i]->Rotate(0.0f, 180.0f, 0.0f);
	}
	m_ppObjects[22] = new CBuildingObject();
	m_ppObjects[22]->SetMesh(0, pCityMesh4);
	m_ppObjects[22]->SetShader(pShader);
	m_ppObjects[22]->SetScale(20.0f);
	m_ppObjects[22]->SetPosition(1700.0f, m_pTerrain->GetHeight(1700, 200), 200.0f);
	m_ppObjects[22]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	//	m_ppObjects[22]->Rotate(0.0f, 180.0f, 0.0f);

	m_ppObjects[23] = new CBuildingObject();
	m_ppObjects[23]->SetMesh(0, pCityMesh5);
	m_ppObjects[23]->SetShader(pShader);
	m_ppObjects[23]->SetScale(20.0f);
	m_ppObjects[23]->SetPosition(1450.0f, m_pTerrain->GetHeight(1300, 200), 200.0f);
	m_ppObjects[23]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	//	m_ppObjects[23]->Rotate(0.0f, 180.0f, 0.0f);

	m_ppObjects[24] = new CBuildingObject();
	m_ppObjects[24]->SetMesh(0, pCityMesh5);
	m_ppObjects[24]->SetShader(pShader);
	m_ppObjects[24]->SetScale(20.0f);
	m_ppObjects[24]->SetPosition(800.0f, m_pTerrain->GetHeight(800, 20), 70.0f);
	m_ppObjects[24]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));

	for (int i = 0; i < 9; i++)
	{
		float z = 100 + 50 * i;
		m_ppObjects[25 + i] = new CBuildingObject();
		m_ppObjects[25 + i]->SetMesh(0, pCityMesh6);
		m_ppObjects[25 + i]->SetShader(pShader);
		m_ppObjects[25 + i]->SetScale(20.0f);
		m_ppObjects[25 + i]->SetPosition(1950.0f, m_pTerrain->GetHeight(1900, z), z);
		m_ppObjects[25 + i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
		m_ppObjects[25 + i]->Rotate(0.0f, -90.0f, 0.0f);
	}
	for (int i = 0; i < 4; ++i)
	{
		float x = 400 + 100 * i;
		m_ppObjects[34 + i] = new CBuildingObject();
		m_ppObjects[34 + i]->SetMesh(0, pLampMesh);
		m_ppObjects[34 + i]->SetShader(pShader);
		m_ppObjects[34 + i]->SetScale(20.0f);
		m_ppObjects[34 + i]->SetPosition(x, m_pTerrain->GetHeight(x, 50), 50.0f);
		m_ppObjects[34 + i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));

		float x2 = 900 + 100 * i;
		m_ppObjects[38 + i] = new CBuildingObject();
		m_ppObjects[38 + i]->SetMesh(0, pLampMesh);
		m_ppObjects[38 + i]->SetShader(pShader);
		m_ppObjects[38 + i]->SetScale(20.0f);
		m_ppObjects[38 + i]->SetPosition(x2, m_pTerrain->GetHeight(x2, 50), 50.0f);
		m_ppObjects[38 + i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	}
	for (int i = 0; i < 7; ++i)
	{
		float x2 = 1200 + 100 * i;
		m_ppObjects[42 + i] = new CBuildingObject();
		m_ppObjects[42 + i]->SetMesh(0, pLampMesh);
		m_ppObjects[42 + i]->SetShader(pShader);
		m_ppObjects[42 + i]->SetScale(20.0f);
		m_ppObjects[42 + i]->SetPosition(x2, m_pTerrain->GetHeight(x2, 50), 50.0f);
		m_ppObjects[42 + i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	}
	//area2 road

	m_ppObjects[49] = new CBuildingObject();
	m_ppObjects[49]->SetMesh(0, pCubeMesh);
	m_ppObjects[49]->SetShader(pShader);
	m_ppObjects[49]->SetScale(20.0f);
	m_ppObjects[49]->SetScaleX(110.0f);
	m_ppObjects[49]->SetScaleY(40.0f);
	m_ppObjects[49]->SetScaleZ(120.0f);
	m_ppObjects[49]->SetPosition(1040.0f, 250, 1620.0f);
	m_ppObjects[49]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));

	m_ppObjects[50] = new CBuildingObject();
	m_ppObjects[50]->SetMesh(0, pBriMesh);
	m_ppObjects[50]->SetShader(pShader);
	m_ppObjects[50]->SetScale(20.0f);
	m_ppObjects[50]->SetScaleX(50.0f);
	m_ppObjects[50]->SetPosition(1500, m_pTerrain->GetHeight(1950, 500), 1620.0f);
	m_ppObjects[50]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_ppObjects[50]->Rotate(0.0f, 0.0f, 0.0f);

	m_ppObjects[51] = new CBuildingObject();
	m_ppObjects[51]->SetMesh(0, pBriMesh);
	m_ppObjects[51]->SetShader(pShader);
	m_ppObjects[51]->SetScale(20.0f);
	m_ppObjects[51]->SetScaleZ(50.0f);
	m_ppObjects[51]->SetPosition(1950, m_pTerrain->GetHeight(1950, 500), 1050.0f);
	m_ppObjects[51]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_ppObjects[51]->Rotate(0.0f, 90.0f, 0.0f);

	//area3
	m_ppObjects[52] = new CBuildingObject();
	m_ppObjects[52]->SetMesh(0, pCityMesh8);
	m_ppObjects[52]->SetShader(pShader);
	m_ppObjects[52]->SetScale(20.0f);
	m_ppObjects[52]->SetScaleX(25.0f);
	m_ppObjects[52]->SetScaleZ(32.0f);
	m_ppObjects[52]->SetPosition(800, m_pTerrain->GetHeight(800, 1900), 1900.0f);
	m_ppObjects[52]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_ppObjects[52]->Rotate(0.0f, 180.0f, 0.0f);

	m_ppObjects[53] = new CBuildingObject();
	m_ppObjects[53]->SetMesh(0, pCityMesh9);
	m_ppObjects[53]->SetShader(pShader);
	m_ppObjects[53]->SetScale(20.0f);
	m_ppObjects[53]->SetPosition(650, m_pTerrain->GetHeight(700, 1900), 1900.0f);
	m_ppObjects[53]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_ppObjects[53]->Rotate(0.0f, 180.0f, 0.0f);

	m_ppObjects[54] = new CBuildingObject();
	m_ppObjects[54]->SetMesh(0, pCityMesh6);
	m_ppObjects[54]->SetShader(pShader);
	m_ppObjects[54]->SetScale(20.0f);
	m_ppObjects[54]->SetPosition(1000, m_pTerrain->GetHeight(1000, 1900), 1900.0f);
	m_ppObjects[54]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_ppObjects[54]->Rotate(0.0f, 180.0f, 0.0f);


	for (int i = 0; i < 3; ++i)
	{
		float z = 450 * (i + 1);

		m_ppObjects[55 + i] = new CBuildingObject();
		m_ppObjects[55 + i]->SetMesh(0, pCityMesh7);
		m_ppObjects[55 + i]->SetShader(pShader);
		m_ppObjects[55 + i]->SetScale(120.0f);
		m_ppObjects[55 + i]->SetPosition(500, m_pTerrain->GetHeight(500, z), z);
		m_ppObjects[55 + i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
		m_ppObjects[55 + i]->Rotate(0.0f, 180.0f, 0.0f);
	}

	for (int i = 0; i < 3; ++i)
	{
		float z = 450 * (i + 1);

		m_ppObjects[58 + i] = new CBuildingObject();
		m_ppObjects[58 + i]->SetMesh(0, pCityMesh7);
		m_ppObjects[58 + i]->SetShader(pShader);
		m_ppObjects[58 + i]->SetScale(120.0f);
		m_ppObjects[58 + i]->SetPosition(1000, m_pTerrain->GetHeight(1000, z), z);
		m_ppObjects[58 + i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
		m_ppObjects[58 + i]->Rotate(0.0f, 180.0f, 0.0f);
	}

	for (int i = 0; i < 3; ++i)
	{
		float z = 450 * (i + 1);

		m_ppObjects[61 + i] = new CBuildingObject();
		m_ppObjects[61 + i]->SetMesh(0, pCityMesh7);
		m_ppObjects[61 + i]->SetShader(pShader);
		m_ppObjects[61 + i]->SetScale(120.0f);
		m_ppObjects[61 + i]->SetPosition(1500, m_pTerrain->GetHeight(1500, z), z);
		m_ppObjects[61 + i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
		m_ppObjects[61 + i]->Rotate(0.0f, 180.0f, 0.0f);
	}

	// 문
	m_doorObjects[0] = new CBuildingObject();
	m_doorObjects[0]->SetMesh(0, pRoadMesh);
	m_doorObjects[0]->SetShader(pShader);
	m_doorObjects[0]->SetScale(100.0f);
	m_doorObjects[0]->SetPosition(190.0f, m_pTerrain->GetHeight(100, 80), 0.0f);
	m_doorObjects[0]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_doorObjects[0]->Rotate(0.0f, 90.0f, 0.0f);



	
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 4; //Time, ElapsedTime, xCursor, yCursor
	pd3dRootParameters[0].Constants.ShaderRegister = 0; //Time
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 19; //16 + 3
	pd3dRootParameters[1].Constants.ShaderRegister = 1; //World
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[2].Constants.Num32BitValues = 35; //16 + 16 + 3
	pd3dRootParameters[2].Constants.ShaderRegister = 2; //Camera
	pd3dRootParameters[2].Constants.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}

	
	if (m_bObjects)
	{
		if (m_bObjects[0])
		{
			delete m_bObjects[0];
		}
		delete[] m_bObjects;
	}
	if (m_doorObjects)
	{
		if (m_doorObjects[0])
		{
			delete m_doorObjects[0];
		}
		delete[] m_doorObjects;
	}



	if (m_pTerrain) delete m_pTerrain;
}

void CScene::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}
	
	if (m_bObjects)
	{
		if (m_bObjects[0]) m_bObjects[0]->ReleaseUploadBuffers();
	}
	if (m_doorObjects)
	{
		if (m_doorObjects[0]) m_doorObjects[0]->ReleaseUploadBuffers();
	}



	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}

void CScene::CheckPlayerByObjectCollisions()
{
	CTerrainPlayer* Player = (CTerrainPlayer*)m_pPlayer;

	for (int i = 0; i < m_nObjects; i++) {
		if (m_ppObjects[i]->life)
			if (i == 50 || i == 51)
			{
				BrigeCollision();
			}
			else
			{
				if (m_ppObjects[i]->m_xmOOBB.Intersects(Player->m_xmOOBB))
					Player->ResetPlayerPos();
			}
	}

	if (m_bObjects[0]->life)
		if (m_bObjects[0]->m_xmOOBB.Intersects(Player->m_xmOOBB))
			Player->ResetPlayerPos();

}

void CScene::BrigeCollision()
{
	// 다리 충돌체크 구현
	CTerrainPlayer* Player = (CTerrainPlayer*)m_pPlayer;

	if (m_ppObjects[50]->m_xmOOBB.Intersects(Player->m_xmOOBB))
	{
		if (1100 < Player->GetPosition().x && Player->GetPosition().x < 1900)
		{
			if (240 < Player->GetPosition().y && Player->GetPosition().y < 400)
			{
				//Beep(1000, 50);
			}
			else {
				Player->ResetPlayerPos();
			}


			// 50 =  x = 1500, z = 1620
		}	// 51 = x = 1950  z = 1050
		else {
			Player->ResetPlayerPos();
		}

	}

	if (m_ppObjects[51]->m_xmOOBB.Intersects(Player->m_xmOOBB))
	{
		if (500 < Player->GetPosition().z && Player->GetPosition().z < 1600)
		{
			if (240 < Player->GetPosition().y && Player->GetPosition().y < 400)
			{
				//Beep(1000, 50);
			}
			else {
				Player->ResetPlayerPos();
			}


			// 50 =  x = 1500, z = 1620
		}	// 51 = x = 1950  z = 1050
		else {
			Player->ResetPlayerPos();
		}
	}


}

void CScene::CheckMissileByObjectCollisions()
{
	CTerrainPlayer* Player = (CTerrainPlayer*)m_pPlayer;

	// 보스와 미사일이 충돌했을 때 타격횟수 추가 필요

	if (m_bObjects[0]->life)
	{
		for (int j = 0; j < Player->GetMissileNum(); ++j)
		{
			CMissileObject* Missile = Player->GetMissile(j);
			if (Missile->GetFire())
				if (m_bObjects[0]->m_xmOOBB.Intersects(Missile->m_xmOOBB))
				{
					Beep(1000, 50);
					Missile->SetFire(false);
					Missile->SetPosition(0.0f, -1000.0f, 0.0f);
					cnt++;
					if (cnt == 30)
					{
						m_bObjects[0]->life = false;
					}

				}
		}
	}
}

// 보스와의 충돌체크 만들기

void CScene::CollisonBossMissile()
{
	CTerrainPlayer* Player = (CTerrainPlayer*)m_pPlayer;


	

	
	

}

// 맵과의 충돌
void CScene::CheckMissileByTerrainCollisions()
{
	CTerrainPlayer* Player = (CTerrainPlayer*)m_pPlayer;
	for (int i = 0; i < Player->GetMissileNum(); ++i) {
		CMissileObject* Missile = Player->GetMissile(i);
		XMFLOAT3 posi = Missile->GetPosition();
		if (Missile->GetFire() && m_pTerrain->GetHeight(posi.x, posi.z) > posi.y) {
			Missile->SetFire(false);
			Missile->SetPosition(0.0f, -1000.0f, 0.0f);
			//Beep(1000, 50);
		}
	}
}


// 여기 수정하면 끝!!!!!!!!!!!
void CScene::EnemyAttack(float fTimeElapsed)
{
	std::uniform_int_distribution<> RandomDir(1.0f, 10.0f);
	// 수치 개선 필수
	// 랜덤으로 날아가도록 하기 필요

	
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
	
	m_bObjects[0]->Animate(fTimeElapsed);
	//m_doorObjects[0]->Animate(fTimeElapsed);

	CTerrainPlayer* Player = (CTerrainPlayer*)m_pPlayer;

	CheckPlayerByObjectCollisions();
	CheckMissileByTerrainCollisions();
	CheckMissileByObjectCollisions();
	EnemyAttack(fTimeElapsed);
	CollisonBossMissile();
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_pTerrain)
		m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList, pCamera);
	}
	

	if (m_bObjects[0]) m_bObjects[0]->Render(pd3dCommandList, pCamera);
	if (m_doorObjects[0]) m_doorObjects[0]->Render(pd3dCommandList, pCamera);

}


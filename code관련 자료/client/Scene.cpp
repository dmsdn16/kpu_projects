//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Player.h"
#include "Scene.h"

std::uniform_int_distribution<int> RandomDir(1,100);
std::default_random_engine dre;

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
	CMesh* pCube = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/Cube.bin", false);




#endif

	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다. 
	XMFLOAT3 xmf3Scale(11.0f, 2.0f, 8.0f);
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


	ObjectManager::GetInstance()->PushObject(ObjectManager::OT_TERRAIN, m_pTerrain);


	m_nObjects = 59; // 건물 갯수
	m_ppObjects = new CGameObject * [m_nObjects];
	m_bObjects = new CGameObject * [1];
	m_UI1 = new CGameObject * [4];
	m_UI2 = new CGameObject * [4];
	m_UI3 = new CGameObject * [4];
	m_UI4 = new CGameObject * [4];
	m_UI5 = new CGameObject * [4];
	Unit1 = new CGameObject * [2];
	Unit2 = new CGameObject * [2];
	Unit3 = new CGameObject * [2];
	Unit4 = new CGameObject * [2];
	Unit5 = new CGameObject * [2];
	CPseudoLightingShader* pShader = new CPseudoLightingShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	for (int i = 0; i < 2; ++i)
	{
		Unit1[i] = new CGameObject();
		Unit1[i]->SetMesh(0, pUfoMesh);
		Unit1[i]->SetShader(pShader);
		Unit1[i]->SetScale(20.0f);
		Unit1[i]->SetPosition(0, 3000.0f, 0);
		Unit1[i]->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_Building, Unit1[i]);
	}
	for (int i = 0; i < 2; ++i)
	{
		Unit2[i] = new CGameObject();
		Unit2[i]->SetMesh(0, pUfoMesh);
		Unit2[i]->SetShader(pShader);
		Unit2[i]->SetScale(20.0f);
		Unit2[i]->SetPosition(0, 3000.0f, 0);
		Unit2[i]->SetColor(XMFLOAT3(0.0f, 1.0f, 0.0f));
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_Building, Unit2[i]);
	}
	for (int i = 0; i < 2; ++i)
	{
		Unit3[i] = new CGameObject();
		Unit3[i]->SetMesh(0, pUfoMesh);
		Unit3[i]->SetShader(pShader);
		Unit3[i]->SetScale(20.0f);
		Unit3[i]->SetPosition(0, 3000.0f, 0);
		Unit3[i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_Building, Unit3[i]);
	}
	for (int i = 0; i < 2; ++i)
	{
		Unit4[i] = new CGameObject();
		Unit4[i]->SetMesh(0, pUfoMesh);
		Unit4[i]->SetShader(pShader);
		Unit4[i]->SetScale(20.0f);
		Unit4[i]->SetPosition(0, 3000.0f, 0);
		Unit4[i]->SetColor(XMFLOAT3(1.0f, 1.0f, 0.0f));
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_Building, Unit4[i]);
	}
	for (int i = 0; i < 2; ++i)
	{
		Unit5[i] = new CGameObject();
		Unit5[i]->SetMesh(0, pUfoMesh);
		Unit5[i]->SetShader(pShader);
		Unit5[i]->SetScale(20.0f);
		Unit5[i]->SetPosition(0, 3000.0f, 0);
		Unit5[i]->SetColor(XMFLOAT3(0.0f, 0.0f, 0.0f));
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_Building, Unit5[i]);
	}

	//UI상자
	for (int i = 0; i < m_U; ++i)
	{
		m_UI1[i] = new CGameObject();
		m_UI1[i]->SetMesh(0, pCube);
		m_UI1[i]->SetShader(pShader);
		m_UI1[i]->SetScale(200.0f);
		m_UI1[i]->SetPosition((500 + 500 * i), 3000, -100.0f);
		m_UI1[i]->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UI, m_UI1[i]);

		m_UI2[i] = new CGameObject();
		m_UI2[i]->SetMesh(0, pCube);
		m_UI2[i]->SetShader(pShader);
		m_UI2[i]->SetScale(200.0f);
		m_UI2[i]->SetPosition((500 + 500 * i), 3000, -100.0f);
		m_UI2[i]->SetColor(XMFLOAT3(0.0f, 1.0f, 0.0f));
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UI, m_UI2[i]);

		m_UI3[i] = new CGameObject();
		m_UI3[i]->SetMesh(0, pCube);
		m_UI3[i]->SetShader(pShader);
		m_UI3[i]->SetScale(200.0f);
		m_UI3[i]->SetPosition((500 + 500 * i), 3000, -100.0f);
		m_UI3[i]->SetColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UI, m_UI3[i]);

		m_UI4[i] = new CGameObject();
		m_UI4[i]->SetMesh(0, pCube);
		m_UI4[i]->SetShader(pShader);
		m_UI4[i]->SetScale(200.0f);
		m_UI4[i]->SetPosition((500 + 500 * i), 3000, -100.0f);
		m_UI4[i]->SetColor(XMFLOAT3(1.0f, 1.0f, 0.0f));
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UI, m_UI4[i]);

		m_UI5[i] = new CGameObject();
		m_UI5[i]->SetMesh(0, pCube);
		m_UI5[i]->SetShader(pShader);
		m_UI5[i]->SetScale(200.0f);
		m_UI5[i]->SetPosition((500 + 500 * i), 3000, -100.0f);
		m_UI5[i]->SetColor(XMFLOAT3(0.0f, 0.0f, 0.0f));
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UI, m_UI5[i]);
	}


	//m_UI[0]->Rotate(40.0f,20.0f,-10.0f);

	m_bObjects[0] = new CUfoObject(1);
	m_bObjects[0]->SetMesh(0, pUfoMesh);
	m_bObjects[0]->SetShader(pShader);
	m_bObjects[0]->SetScale(100.0f);
	m_bObjects[0]->SetPosition(1000.0f, -400.0f, 1000.0f);
	m_bObjects[0]->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));



	ObjectManager::GetInstance()->PushObject(ObjectManager::OT_UNIT, m_bObjects[0]);


	// 건물
	// area1
	for (int i = 0; i < 20; ++i)
	{
		float z = 100 + (100 * i);
		m_ppObjects[i] = new CBuildingObject();
		m_ppObjects[i]->SetMesh(0, pCityMesh);
		m_ppObjects[i]->SetShader(pShader);
		m_ppObjects[i]->SetScale(20.0f);
		m_ppObjects[i]->SetPosition(100.0f, m_pTerrain->GetHeight(100, z), z);
		m_ppObjects[i]->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
		m_ppObjects[i]->Rotate(0.0f, 90.0f, 0.0f);
	}


	for (int i = 0; i < 9; i++)
	{
		float z = 100 + 50 * i;
		m_ppObjects[20 + i] = new CBuildingObject();
		m_ppObjects[20 + i]->SetMesh(0, pCityMesh6);
		m_ppObjects[20 + i]->SetShader(pShader);
		m_ppObjects[20 + i]->SetScale(20.0f);
		m_ppObjects[20 + i]->SetPosition(2700.0f, m_pTerrain->GetHeight(1900, z), z);
		m_ppObjects[20 + i]->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
		m_ppObjects[20 + i]->Rotate(0.0f, -90.0f, 0.0f);
	}
	//램프
	for (int i = 0; i < 28; ++i)
	{
		float x = 200 + 100 * i;
		m_ppObjects[29 + i] = new CBuildingObject();
		m_ppObjects[29 + i]->SetMesh(0, pLampMesh);
		m_ppObjects[29 + i]->SetShader(pShader);
		m_ppObjects[29 + i]->SetScale(20.0f);
		m_ppObjects[29 + i]->SetPosition(x, m_pTerrain->GetHeight(x, 50), 200.0f);
		m_ppObjects[29 + i]->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
	}
	// 다리

	m_ppObjects[57] = new CBuildingObject();
	m_ppObjects[57]->SetMesh(0, pBriMesh);
	m_ppObjects[57]->SetShader(pShader);
	m_ppObjects[57]->SetScale(20.0f);
	m_ppObjects[57]->SetScaleX(120.0f);
	m_ppObjects[57]->SetPosition(1300, m_pTerrain->GetHeight(1950, 500) + 100, 1750.0f);
	m_ppObjects[57]->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_ppObjects[57]->Rotate(0.0f, 0.0f, 0.0f);

	m_ppObjects[58] = new CBuildingObject();
	m_ppObjects[58]->SetMesh(0, pBriMesh);
	m_ppObjects[58]->SetShader(pShader);
	m_ppObjects[58]->SetScale(20.0f);
	m_ppObjects[58]->SetScaleZ(80.0f);
	m_ppObjects[58]->SetPosition(2700, m_pTerrain->GetHeight(1950, 500) + 100, 1050.0f);
	m_ppObjects[58]->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_ppObjects[58]->Rotate(0.0f, 90.0f, 0.0f);


	for (int i = 0; i < m_nObjects; i++)
	{
		ObjectManager::GetInstance()->PushObject(ObjectManager::OT_Building, m_ppObjects[i]);
	}

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

	if (m_UI1)
	{
		for (int i = 0; i < 4; i++)if (m_UI1) delete m_UI1[i];
		delete[] m_UI1;
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
	if (m_UI1)
	{
		for (int i = 0; i < 4; i++)if (m_UI1) m_UI1[i]->ReleaseUploadBuffers();
	}




	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}



// 리롤
void CScene::Reroll()
{
	int u_count = 0; // ui위치
	int x_count = 0; // ui 갯수

	for (int i = 0; i < m_U; ++i)
	{

		m_UI1[i]->SetPosition((500 + 500), 3000, -100.0f);
		m_UI2[i]->SetPosition((500 + 500), 3000, -100.0f);
		m_UI3[i]->SetPosition((500 + 500), 3000, -100.0f);
		m_UI4[i]->SetPosition((500 + 500), 3000, -100.0f);
		m_UI5[i]->SetPosition((500 + 500), 3000, -100.0f);

	}

	for (int i = 0; i < 4; ++i)
	{
		int a = CalRoll();

		//std::cout << a << std::endl;
	
		switch (a)
		{
		case 1:
			
			if (UnitName1 != 0)
			{
				m_UI1[x_count]->SetPosition(500 + 500 * u_count, 0, -100);
				x_count++;
				u_count++;
			}
			else
				Reroll();
			
			break;
		case 2:
			if (UnitName2 != 0)
			{
				m_UI2[x_count]->SetPosition(500 + 500 * u_count, 0, -100);
				x_count++;
				u_count++;
			}
			else
				Reroll();
			break;
		case 3:
			if (UnitName3 != 0)
			{
				m_UI3[x_count]->SetPosition(500 + 500 * u_count, 0, -100);
				x_count++;
				u_count++;
			}
			else
				Reroll();
			break;
		case 4:
			if (UnitName4 != 0)
			{
				m_UI4[x_count]->SetPosition(500 + 500 * u_count, 0, -100);
				x_count++;
				u_count++;
			}
			else
				Reroll();
			break;
		case 5:
			if (UnitName5 != 0)
			{
				m_UI5[x_count]->SetPosition(500 + 500 * u_count, 0, -100);
				x_count++;
				u_count++;
			}
			else
				Reroll();
			break;
		default:
			break;
		}
	}
	//std::cout << "무야호" << std::endl;
}

// 누적 확률을 이용한 확률계산
int CScene::CalRoll()
{
	int ran = RandomDir(dre);
	
	double p[] = { 5.0f, 15.0f, 15.0f, 30.0f, 35.0f }; //4, 3, 2, 1, 0

	double cumulative = 0.0f;

	for (int i = 0; i < 5; i++)
	{
		cumulative += p[i];
		if (ran <= cumulative)
		{
			return 5 - i;
		}
	}


}


// 여기 수정하면 끝!!!!!!!!!!!
void CScene::UnitBuy()
{
	for (int i = 0; i < m_U; ++i)
	{
		if ((m_pick->GetInstance()->IntersecTri()) == m_UI1[i])
		{
			++u1c;
			if (u1c < 3)
			{
				++count;
				m_UI1[i]->SetPosition(0, 3000, 0);
				if (Unit1[0]->GetPosition().z == 0)
				{
					Unit1[0]->SetPosition(200 + (300 * count), m_pTerrain->GetHeight(100, 100), 100);
					std::cout << "유닛 1 구매" << std::endl;
				}
				else
					Unit1[1]->SetPosition(200 + (300 * count), m_pTerrain->GetHeight(100, 100), 100);

				--UnitName1;
			}
			//std::cout << "asda" <<std::endl;
			//x = 200, z= 100 초기값
		}
		if ((m_pick->GetInstance()->IntersecTri()) == m_UI2[i])
		{

			++u2c;
			if (u2c < 3)
			{
				++count;
				m_UI2[i]->SetPosition(0, 3000, 0);
				if (Unit2[0]->GetPosition().z == 0)
				{
					Unit2[0]->SetPosition(200 + (300 * count), m_pTerrain->GetHeight(100, 100), 100);
					std::cout << "유닛 2 구매" << std::endl;
				}
				else
					Unit2[1]->SetPosition(200 + (300 * count), m_pTerrain->GetHeight(100, 100), 100);
				--UnitName2;
			}
		}

		if ((m_pick->GetInstance()->IntersecTri()) == m_UI3[i])
		{
			++u3c;
			if (u3c < 3)
			{
				++count;
				m_UI3[i]->SetPosition(0, 3000, 0);
				if (Unit3[0]->GetPosition().z == 0)
				{
					Unit3[0]->SetPosition(200 + (300 * count), m_pTerrain->GetHeight(100, 100), 100);
					std::cout << "유닛 3 구매" << std::endl;
				}
				else
					Unit3[1]->SetPosition(200 + (300 * count), m_pTerrain->GetHeight(100, 100), 100);
				--UnitName3;
			}
		}
		if ((m_pick->GetInstance()->IntersecTri()) == m_UI4[i])
		{
			++u4c;
			if (u4c < 3)
			{
				++count;
				m_UI4[i]->SetPosition(0, 3000, 0);
				if (Unit4[0]->GetPosition().z == 0)
				{
					Unit4[0]->SetPosition(200 + (300 * count), m_pTerrain->GetHeight(100, 100), 100);
					std::cout << "유닛 4 구매" << std::endl;
				}
				else
					Unit4[1]->SetPosition(200 + (300 * count), m_pTerrain->GetHeight(100, 100), 100);
				--UnitName4;
			}
		}

		if ((m_pick->GetInstance()->IntersecTri()) == m_UI5[i])
		{
			++u5c;
			if (u5c < 3)
			{
				++count;
				m_UI5[i]->SetPosition(0, 3000, 0);
				if (Unit5[0]->GetPosition().z == 0)
				{
					Unit5[0]->SetPosition(200 + (300 * count), m_pTerrain->GetHeight(100, 100), 100);
					std::cout << "유닛 4 구매" << std::endl;
				}
				else
					Unit5[1]->SetPosition(200 + (300 * count), m_pTerrain->GetHeight(100, 100), 100);
				--UnitName5;
			}
		}
	}
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
	for (int i = 0; i < m_U; ++i)
	{
		m_UI1[i]->Animate(fTimeElapsed);
		m_UI2[i]->Animate(fTimeElapsed);
		m_UI3[i]->Animate(fTimeElapsed);
		m_UI4[i]->Animate(fTimeElapsed);
		m_UI5[i]->Animate(fTimeElapsed);
	}

	for (int i = 0; i < 2; ++i)
	{
		Unit1[i]->Animate(fTimeElapsed);
	}
	for (int i = 0; i < 2; ++i)
	{
		Unit2[i]->Animate(fTimeElapsed);
	}
	for (int i = 0; i < 2; ++i)
	{
		Unit3[i]->Animate(fTimeElapsed);
	}
	for (int i = 0; i < 2; ++i)
	{
		Unit4[i]->Animate(fTimeElapsed);
	}
	for (int i = 0; i < 2; ++i)
	{
		Unit5[i]->Animate(fTimeElapsed);
	}
	if (start == 0)
	{
		Reroll();
		++start;
	}

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
	for (int i = 0; i < m_U; ++i)
	{
		if (m_UI1[i]) m_UI1[i]->Render(pd3dCommandList, pCamera);
		if (m_UI2[i]) m_UI2[i]->Render(pd3dCommandList, pCamera);
		if (m_UI3[i]) m_UI3[i]->Render(pd3dCommandList, pCamera);
		if (m_UI4[i]) m_UI4[i]->Render(pd3dCommandList, pCamera);
		if (m_UI5[i]) m_UI5[i]->Render(pd3dCommandList, pCamera);
	}
	for (int i = 0; i < 2; ++i)
	{
		if (Unit1[i]) Unit1[i]->Render(pd3dCommandList, pCamera);
		if (Unit2[i]) Unit2[i]->Render(pd3dCommandList, pCamera);
		if (Unit3[i]) Unit3[i]->Render(pd3dCommandList, pCamera);
		if (Unit4[i]) Unit4[i]->Render(pd3dCommandList, pCamera);
		if (Unit5[i]) Unit5[i]->Render(pd3dCommandList, pCamera);
	}

}


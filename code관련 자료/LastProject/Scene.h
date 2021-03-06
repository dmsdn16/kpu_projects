
#pragma once

#include "Shader.h"
#include "Player.h"
#include "pickingManager.h"
#define MAX_LIGHTS						16 

#define POINT_LIGHT						1
#define SPOT_LIGHT						2
#define DIRECTIONAL_LIGHT				3




struct LIGHT
{
	XMFLOAT4							m_xmf4Ambient;
	XMFLOAT4							m_xmf4Diffuse;
	XMFLOAT4							m_xmf4Specular;
	XMFLOAT3							m_xmf3Position;
	float 								m_fFalloff;
	XMFLOAT3							m_xmf3Direction;
	float 								m_fTheta; //cos(m_fTheta)
	XMFLOAT3							m_xmf3Attenuation;
	float								m_fPhi; //cos(m_fPhi)
	bool								m_bEnable;
	int									m_nType;
	float								m_fRange;
	float								padding;
};										
										
struct LIGHTS							
{										
	LIGHT								m_pLights[MAX_LIGHTS];
	XMFLOAT4							m_xmf4GlobalAmbient;
	int									m_nLights;
};

class CScene
{
public:
    CScene();
    ~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildDefaultLightsAndMaterials();
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput(UCHAR *pKeysBuffer);
    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	void ReleaseUploadBuffers();


	CPlayer								*m_pPlayer = NULL;

protected:
	ID3D12RootSignature					*m_pd3dGraphicsRootSignature = NULL;

	static ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorNextHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;

public:
	static void CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);

	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferViews(ID3D12Device *pd3dDevice, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViews(ID3D12Device *pd3dDevice, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorNextHandle() { return(m_d3dCbvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorNextHandle() { return(m_d3dCbvGPUDescriptorNextHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorNextHandle() { return(m_d3dSrvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorNextHandle() { return(m_d3dSrvGPUDescriptorNextHandle); }

	int									m_nGameObjects = 0;
	CGameObject							**m_ppGameObjects = NULL;

	int									m_nPlateObjects = 0;
	CGameObject							** m_ppPlateObjects = NULL;

	float								m_fElapsedTime = 0.0f;

	int									m_nShaders = 0;
	CShader								**m_ppShaders = NULL;

	CSkyBox								*m_pSkyBox = NULL;
	CHeightMapTerrain					*m_pTerrain = NULL;

	LIGHT								*m_pLights = NULL;
	int									m_nLights = 0;

	XMFLOAT4							m_xmf4GlobalAmbient;

	ID3D12Resource						*m_pd3dcbLights = NULL;
	LIGHTS								*m_pcbMappedLights = NULL;

public:

	int									MAX = 101;
	double								INF = 1e9 + 7;

	// 직선
	const int dx1[4] = { 0, 0, 1, -1 };
	const int dy1[4] = { -1, 1, 0, 0 };

	// 대각선
	const int dx2[4] = { 1, -1, -1, 1 };
	const int dy2[4] = { -1, 1, -1, 1 };

	using Pair = std::pair<int, int>;
	using pPair = std::pair<double, Pair>;
	std::stack<Pair> s;
	char zmap[101][101];
	int Bmap[9][9] = {0,};
	int ROW = 0;
	int COL = 0;
	bool begin = false;

	bool isDestination(int row, int col, Pair dst); // 현재좌표와 도착좌표 비교
	bool isRanger(int row, int col); // 현재 자신의 위치 확인
	bool isUnBlock(std::vector<std::vector<int>>& map, int row, int col); // 현재좌표가 벽인지 확인
	float GetValue(int row, int col, Pair dst); // 도착지점까지의 거리 계산
	void Path(Cell cellDetails[101][101], Pair dst); //최단 경로 검색 함수
	bool Astar(std::vector<std::vector<int>>& map, Pair src, Pair dst); //astar실행함수
	void PrintMap(); // astar확인용 출력
	std::vector<std::vector<int>> fileload(std::string filepath);

	
	void Reroll();
	int CalRoll();
	void AdmCount();
	void ResetUi();
	void UnitSell();
	void UnitBuy();
	void SetStart(bool start) { begin = start; }
	bool GetStart() { return begin; }

	int count = 0;
	int ncount = 0;
	int FrameCount = 0;
	
	int distance = sqrt(pow(160, 2) + pow(140, 2));
	int cmpdis = 0;

	PickMgr* m_pick = NULL;

	CGameObject** m_UI5 = NULL;
	CGameObject** m_UI1 = NULL;
	CGameObject** m_UI2 = NULL;
	CGameObject** m_UI3 = NULL;
	CGameObject** m_UI4 = NULL;
	CGameObject** Unit1 = NULL;
	CGameObject** Unit2 = NULL;
	CGameObject** Unit3 = NULL;
	CGameObject** Unit4 = NULL;
	CGameObject** Unit5 = NULL;

	int							m_U = 4;
	int							countt = 0;
	int							uc[5] = { 4,4,4,2,2 }; // 유닛 갯수

	int							start = 0;

	int							UIBox[5] = { 0, };
	int							u_count = 0; // ui위치
	int							x_count = 0; // ui 갯수

	int							uic[5] = { 0, };
	int							Canbuy = 6;
	int                         array[6] = { 0, };

public:
	list<CGameObject*> oj;
	list<CGameObject*> UnitList1;
	list<CGameObject*> UnitList2;
	list<CGameObject*> UnitList3;
	list<CGameObject*> UnitList4;
	list<CGameObject*> UnitList5;
};

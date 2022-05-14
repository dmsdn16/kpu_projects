//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "pickingManager.h"
#include "StoreManager.h"

class CScene
{
public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	void ReleaseUploadBuffers();

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }

	void Reroll();
	void UnitBuy();
	int CalRoll();
	void AdmCount();
	void ResetUi();
	void UnitSell();
	


	CPlayer* m_pPlayer = NULL;


protected:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	CGameObject** m_ppObjects = 0; // °Ç¹°
	CGameObject** m_bObjects = 0; // º¸½º
	CGameObject** m_UI5 = 0;
	CGameObject** m_UI1 = 0;
	CGameObject** m_UI2 = 0;
	CGameObject** m_UI3 = 0;
	CGameObject** m_UI4 = 0;
	CGameObject** Unit1 = 0;
	CGameObject** Unit2 = 0;
	CGameObject** Unit3 = 0;
	CGameObject** Unit4 = 0;
	CGameObject** Unit5 = 0;

	int							m_nObjects = 0;
	int                         m_pObjects = 0;
	int                         m_Brige = 0;
	int							m_U = 4;
	int							count = 0;
	int							uc[5] = { 4,4,4,2,2 }; // À¯´Ö °¹¼ö
	
	int							start = 0;

	int							UIBox[5] = { 0, };
	int							u_count = 0; // uiÀ§Ä¡
	int							x_count = 0; // ui °¹¼ö

	int							uic[5] = {0,};
	int							Canbuy = 6;
	int                         array[6] = { 0, };
	

	CHeightMapTerrain* m_pTerrain = NULL;
	PickMgr* m_pick = NULL;

public:
	list<CGameObject*> oj;
	list<CGameObject*> UnitList1;
	list<CGameObject*> UnitList2;
	list<CGameObject*> UnitList3;
	list<CGameObject*> UnitList4;
	list<CGameObject*> UnitList5;
};

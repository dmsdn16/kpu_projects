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
	void PlayerSpin();
	void BrigeCollision();
	void CollisonBossMissile();


	CPlayer* m_pPlayer = NULL;


protected:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	CGameObject** m_ppObjects = 0; // 건물
	CGameObject** m_bObjects = 0; // 보스
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
	int							u1c = 0;
	int							u2c = 0;
	int							u3c = 0;
	int							u4c = 0;
	int							u5c = 0;
	int							start = 0;

	CHeightMapTerrain* m_pTerrain = NULL;
	PickMgr* m_pick = NULL;

private:
	int m_missileNum = 10; // 미사일 갯수
	CMissileObject** m_pmissile = 0;
public:
	int cnt = 0; // 보스 타격 횟수

};

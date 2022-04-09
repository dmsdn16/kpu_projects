//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
static std::default_random_engine dre;
class CScene
{
public:
    CScene();
    ~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput();
    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	void ReleaseUploadBuffers();

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }

	void CheckPlayerByObjectCollisions();
	void CheckMissileByObjectCollisions();
	void CheckMissileByTerrainCollisions();
	void EnemyAttack(float fTimeElapsed);
	void PlayerSpin();
	void BrigeCollision();
	void CollisonBossMissile();

	CPlayer* m_pPlayer = NULL;

protected:
	ID3D12RootSignature			*m_pd3dGraphicsRootSignature = NULL;

	CGameObject					**m_ppObjects = 0; // �ǹ�
	CGameObject                 **m_bObjects = 0; // ����
	CGameObject                 **m_mObjects = 0; // �� ����ü
	CGameObject                 **m_doorObjects = 0; // ��
	CGameObject                 **m_BriObjects = 0; // �ٸ�
	CGameObject					** cob = 0;
	int							m_nObjects = 0;
	int                         m_pObjects = 0;
	int                         m_Brige = 0;

	CHeightMapTerrain* m_pTerrain = NULL;
private:
	int m_missileNum = 10; // �̻��� ����
	CMissileObject** m_pmissile = 0;
public:
	int cnt = 0; // ���� Ÿ�� Ƚ��
};

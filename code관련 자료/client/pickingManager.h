#pragma once
#include "stdafx.h"
#include "Object.h"
class PickMgr
{
public:
	static PickMgr* GetInstance(void) {
		if (nullptr == pInstance)
			pInstance = new PickMgr;

		return pInstance;
	}

	static void DestroyInstance(void)
	{
		if (nullptr != pInstance)
			delete pInstance;

	}

private:
	static PickMgr* pInstance;

private:
	XMFLOAT3 m_RayPos;
	//XMFLOAT4 m_RayVec;
	XMFLOAT3 m_RayVec;

	POINT m_ptMouse;
	POINT m_ptResolution;

	HWND m_hWnd;
	ID3D12Device* m_pGraphic_Device;

	POINT m_WindowResolution;

public:
	void Create(HWND _hWnd, int	_nWndClientWidth, int _nWndClientHeight, ID3D12Device* _pd3dDevice);
	void Tick(void);
	CGameObject* IntersecTri(void);
	void SetRayPos(XMFLOAT3 Raypos) { m_RayPos = Raypos; }
	void SetRayVec(XMFLOAT3 Rayvec) { m_RayVec = Rayvec; }
	XMFLOAT3 GetRayPos() { return m_RayPos; }
	XMFLOAT3 GetRayVec() { return m_RayVec; }
};
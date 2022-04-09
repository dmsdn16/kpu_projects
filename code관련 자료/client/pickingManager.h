#pragma once
#include "stdafx.h"
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
	XMFLOAT3 m_Raypos;
	XMFLOAT3 m_Rayvec;

public:
	void Create(HWND m_hWnd, int )
	void Tick(void);
};
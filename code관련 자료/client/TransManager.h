#pragma once
#include "Camera.h"
#include "stdafx.h"
class TransManager
{

public:
	static TransManager* GetInstance(void) {
		if (nullptr == pInstance)
			pInstance = new TransManager;

		return pInstance;
	}

	static void DestroyInstance(void)
	{
		if (nullptr != pInstance)
			delete pInstance;

	}

private:
	static TransManager* pInstance;



public:
	void SetCamera(CCamera* Camera)
	{
		pCamera = Camera;
	}

	XMFLOAT4X4 GetViewMaterix()
	{
		if(pCamera != nullptr)
			return pCamera->GetViewMatrix();
	}

	XMFLOAT4X4 GetProjectionMatrix()
	{
		if (pCamera != nullptr)
			return pCamera->GetProjectionMatrix();
	}

private:
	CCamera* pCamera = nullptr;



};


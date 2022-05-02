#pragma once
#include "stdafx.h"
#include "Object.h"

class StoreManager : public CGameObject
{
public:
	StoreManager();
	~StoreManager();
	void Orth();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL) override;

	
};

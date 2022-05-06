#pragma once
#include "stdafx.h"
#include "Object.h"
class ObjectManager
{

public:
	static ObjectManager* GetInstance(void) {
		if (nullptr == pInstance)
			pInstance = new ObjectManager;

		return pInstance;
	}

	static void DestroyInstance(void)
	{
		if (nullptr != pInstance)
			delete pInstance;

	}

private:
	static ObjectManager* pInstance;


public:
	enum OBJECT_TYPE
	{
		OT_PLAYER,
		OT_UNIT,
		OT_Building,
		OT_UI,
		OT_TERRAIN,
		OT_END
	};

public:
	void PushObject(OBJECT_TYPE eType, CGameObject* pObject);
	const list<CGameObject*>& GetObjectList(OBJECT_TYPE eType);


private:
	list<CGameObject*>		m_objcetlist[OT_END];


};


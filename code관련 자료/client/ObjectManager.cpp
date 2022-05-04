#include "ObjectManager.h"

ObjectManager* ObjectManager::pInstance = nullptr;

void ObjectManager::PushObject(OBJECT_TYPE eType, CGameObject* pObject)
{
	if(nullptr!=pObject)
		m_objcetlist[eType].push_back(pObject);
}

const list<CGameObject*>& ObjectManager::GetObjectList(OBJECT_TYPE eType)
{
	return m_objcetlist[eType];
}

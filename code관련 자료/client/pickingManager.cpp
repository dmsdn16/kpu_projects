#include "pickingManager.h"
#include "TransManager.h"
#include "Scene.h"
#include "ObjectManager.h"


PickMgr* PickMgr::pInstance = nullptr;


void PickMgr::Create(
    HWND _hWnd, int	_nWndClientWidth, int _nWndClientHeight, ID3D12Device* _pd3dDevice)

{
    m_WindowResolution.x = _nWndClientWidth;
    m_WindowResolution.y = _nWndClientHeight;

    m_hWnd = _hWnd;

    m_pGraphic_Device = _pd3dDevice;
}
void PickMgr::Tick(void)
{
    // �����쿡�� ���콺 ��ǥ, ������ ���� / ���� ����, ��������� �����, ����, ��������, ����� ������ 3�� ��ǥ
        // g_iWinCX, g_iWinCY
        GetCursorPos(&m_ptMouse);
    ScreenToClient(m_hWnd, &m_ptMouse);


    // ���� �����̽��� ���콺 ��ǥ
    XMFLOAT3 vMouseProjection{};
    vMouseProjection.x = 1.f * m_ptMouse.x / (m_ptResolution.x / 2) - 1;
    vMouseProjection.y = -1.f * m_ptMouse.y / (m_ptResolution.y / 2) + 1;
    vMouseProjection.z = 0.f;

    XMFLOAT4X4 MatrixProjectionInverse{};
    MatrixProjectionInverse = TransManager::GetInstance()->GetProjectionMatrix();
    MatrixProjectionInverse = Matrix4x4::Inverse(MatrixProjectionInverse);
  

    // ���� vMouseProjection�� ��ġ�� �佺���̽��� �ִ�.
    vMouseProjection = Vector3::TransformCoord(vMouseProjection, MatrixProjectionInverse);

  
    // ���� ���� : ī�޶��� ��ġ�� ������ 0.f, 0.f, 0.f�� ��´�.
    XMFLOAT3 vCameraPos{ 0.f, 0.f, 0.f };

    XMFLOAT4X4 MatrixView{};
    MatrixView = TransManager::GetInstance()->GetViewMaterix();
    MatrixView = Matrix4x4::Inverse(MatrixView);
    

    // ���� ī�޶��� ��ġ�� ���忡 �ִ� ������Ʈ�� ī�޶� �������� �̵��ϱ� ������ �����Ѵ�.
    vCameraPos = Vector3::TransformCoord(vCameraPos, MatrixView);
    // ���� ���� ���� �����ش�. �̸��� �򰥷��� �ٲ�.
    XMFLOAT3 vRay{ vMouseProjection };

    // ���� ���͸� ī�޶� �������� ���� �̵��Ѵ�. ��, ���� ���̴� ���⺤���̹Ƿ� D3DXVec3TransformNormal�� �����Ѵ�.
    vRay = Vector3::TransformNormal(vRay, XMLoadFloat4x4(&MatrixView));
    // ���� ���� �����̽��� ���� ���Ϳ� ���� ������ �����Ѵ�.
    m_RayPos = vCameraPos;

    XMFLOAT3 vec3;
    vec3 = Vector3::Normalize(vRay);
    //m_RayVec = XMFLOAT4(vec3.x,vec3.y,vec3.z, 1.0f);
    m_RayVec = vec3;

    SetRayPos(m_RayPos);
    SetRayVec(m_RayVec);

}

CGameObject* PickMgr::IntersecTri()
{
    float x;
    XMVECTOR ray1,ray2;

    map<float, CGameObject*> map;
    list<CGameObject*> oj = ObjectManager::GetInstance()->GetObjectList(ObjectManager::OT_UNIT);
    for (auto& object : oj)
    {
        if(object->m_xmOOBB.Intersects(ray1,ray2,x))
        { 
            map.emplace(x, object);
        }
      
    }
    if (!map.empty())
    {
        return map.begin()->second;
    }
    return nullptr;
    
    
}
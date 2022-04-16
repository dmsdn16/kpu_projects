#include "pickingManager.h"
#include "TransManager.h"
#include "Scene.h"


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

}

bool PickMgr::IntersecTri()
{
    PickMgr ray = (*this);
    
    float u, v, t;

    float vv = Vector3::DotProduct(ray.m_RayVec,ray.m_RayVec);
    float qv = Vector3::DotProduct(ray.m_RayPos, ray.m_RayVec);
    float qq = Vector3::DotProduct(ray.m_RayPos, ray.m_RayPos);

    CGameObject* m_bObjects[1];

    m_bObjects[0]->m_xmOOBB.Center;

    return true;
    
}
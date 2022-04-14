#include "pickingManager.h"
#include "TransManager.h"


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
    // 윈도우에서 마우스 좌표, 윈도우 가로 / 세로 길이, 투영행렬의 역행렬, 레이, 레이포스, 대상의 폴리곤 3개 좌표
        // g_iWinCX, g_iWinCY
        GetCursorPos(&m_ptMouse);
    ScreenToClient(m_hWnd, &m_ptMouse);


    // 투영 스페이스의 마우스 좌표
    XMFLOAT3 vMouseProjection{};
    vMouseProjection.x = 1.f * m_ptMouse.x / (m_ptResolution.x / 2) - 1;
    vMouseProjection.y = -1.f * m_ptMouse.y / (m_ptResolution.y / 2) + 1;
    vMouseProjection.z = 0.f;

    XMFLOAT4X4 MatrixProjectionInverse{};
    MatrixProjectionInverse = TransManager::GetInstance()->GetProjectionMatrix();
    MatrixProjectionInverse = Matrix4x4::Inverse(MatrixProjectionInverse);
  

    // 현재 vMouseProjection의 위치는 뷰스페이스에 있다.
    vMouseProjection = Vector3::TransformCoord(vMouseProjection, MatrixProjectionInverse);

  
    // 레이 포스 : 카메라의 위치를 월드의 0.f, 0.f, 0.f로 잡는다.
    XMFLOAT3 vCameraPos{ 0.f, 0.f, 0.f };

    XMFLOAT4X4 MatrixView{};
    MatrixView = TransManager::GetInstance()->GetViewMaterix();
    MatrixView = Matrix4x4::Inverse(MatrixView);
    

    // 현재 카메라의 위치는 월드에 있던 오브젝트를 카메라를 원점으로 이동하기 이전에 존재한다.
    vCameraPos = Vector3::TransformCoord(vCameraPos, MatrixView);
    // 레이 벡터 또한 구해준다. 이름이 헷갈려서 바꿈.
    XMFLOAT3 vRay{ vMouseProjection };

    // 레이 벡터를 카메라 시점으로 같이 이동한다. 단, 현재 레이는 방향벡터이므로 D3DXVec3TransformNormal로 연산한다.
    vRay = Vector3::TransformNormal(vRay, XMLoadFloat4x4(&MatrixView));
    // 이제 월드 스페이스에 레이 벡터와 레이 포스가 존재한다.
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

    return true;
    
}
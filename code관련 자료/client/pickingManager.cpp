#include "pickingManager.h"


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

    D3DXMATRIX MatrixProjectionInverse{};
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &MatrixProjectionInverse);
    D3DXMatrixInverse(&MatrixProjectionInverse, nullptr, &MatrixProjectionInverse);

    // ���� vMouseProjection�� ��ġ�� �佺���̽��� �ִ�.
    D3DXVec3TransformCoord(&vMouseProjection, &vMouseProjection, &MatrixProjectionInverse);


    // ���� ���� : ī�޶��� ��ġ�� ������ 0.f, 0.f, 0.f�� ��´�.
    XMFLOAT3 vCameraPos{ 0.f, 0.f, 0.f };

    D3DXMATRIX MatrixWorld{};

    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &MatrixWorld);
    D3DXMatrixInverse(&MatrixWorld, nullptr, &MatrixWorld);

    // ���� ī�޶��� ��ġ�� ���忡 �ִ� ������Ʈ�� ī�޶� �������� �̵��ϱ� ������ �����Ѵ�.
    D3DXVec3TransformCoord(&vCameraPos, &vCameraPos, &MatrixWorld);

    // ���� ���� ���� �����ش�. �̸��� �򰥷��� �ٲ�.
    XMFLOAT3 vRay{ vMouseProjection };

    // ���� ���͸� ī�޶� �������� ���� �̵��Ѵ�. ��, ���� ���̴� ���⺤���̹Ƿ� D3DXVec3TransformNormal�� �����Ѵ�.
    D3DXVec3TransformNormal(&vRay, &vRay, &MatrixWorld);

    // ���� ���� �����̽��� ���� ���Ϳ� ���� ������ �����Ѵ�.
    m_RayDecs.vRayPos = vCameraPos;

    D3DXVec3Normalize(&vRay, &vRay);
    m_RayDecs.vRayDirection = vRay;
}
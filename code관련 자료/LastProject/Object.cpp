#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Scene.h"

CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers)
{
	m_nTextureType = nTextureType;
	m_nTextures = nTextures;

	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppd3dTextureUploadBuffers = new ID3D12Resource*[m_nTextures];
		m_ppd3dTextures = new ID3D12Resource*[m_nTextures];
	}

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}

	if (m_pRootArgumentInfos)
	{
		delete[] m_pRootArgumentInfos;
	}

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_nTextureType == RESOURCE_TEXTURE2D_ARRAY)
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	}
	else
	{
		for (int i = 0; i < m_nTextures; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle);
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex, bool bIsDDSFile)
{
	if (bIsDDSFile)
		m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &(m_ppd3dTextureUploadBuffers[nIndex]), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	else
		m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromWICFile(pd3dDevice, pd3dCommandList, pszFileName, &(m_ppd3dTextureUploadBuffers[nIndex]), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

CMaterial::CMaterial(int nTextures)
{
	m_nTextures = nTextures;

	m_ppTextures = new CTexture*[m_nTextures];
	m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
	for (int i = 0; i < m_nTextures; i++) m_ppTextures[i] = NULL;
	for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';
}

CMaterial::~CMaterial()
{
	if (m_pShader) m_pShader->Release();

	if (m_nTextures > 0)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppTextures[i]) m_ppTextures[i]->Release();
		delete[] m_ppTextures;

		if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;
	}
}

void CMaterial::SetShader(CShader *pShader){
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::SetTexture(CTexture *pTexture, UINT nTexture) 
{ 
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->Release();
	m_ppTextures[nTexture] = pTexture; 
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->AddRef();  
}

void CMaterial::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->ReleaseUploadBuffers();
	}
}

CShader *CMaterial::m_pWireFrameShader = NULL;
CShader *CMaterial::m_pSkinnedAnimationWireFrameShader = NULL;

void CMaterial::PrepareShaders(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_pWireFrameShader = new CWireFrameShader();
	m_pWireFrameShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_pWireFrameShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pSkinnedAnimationWireFrameShader = new CSkinnedAnimationWireFrameShader();
	m_pSkinnedAnimationWireFrameShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_pSkinnedAnimationWireFrameShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CMaterial::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->UpdateShaderVariable(pd3dCommandList, 0);
	}
}

CAnimationCurve::CAnimationCurve(int nKeys)
{
	m_nKeys = nKeys;
	m_pfKeyTimes = new float[nKeys];
	m_pfKeyValues = new float[nKeys];
}

CAnimationCurve::~CAnimationCurve()
{
	if (m_pfKeyTimes) delete[] m_pfKeyTimes;
	if (m_pfKeyValues) delete[] m_pfKeyValues;
}

float CAnimationCurve::GetValueByLinearInterpolation(float fPosition)
{
	for (int k = 0; k < (m_nKeys - 1); k++)
	{
		if ((m_pfKeyTimes[k] <= fPosition) && (fPosition < m_pfKeyTimes[k+1]))
		{
			float t = (fPosition - m_pfKeyTimes[k]) / (m_pfKeyTimes[k+1] - m_pfKeyTimes[k]);
			return(m_pfKeyValues[k] * (1.0f - t) + m_pfKeyValues[k+1] * t);
		}
	}
	return(m_pfKeyValues[m_nKeys-1]);
}

CAnimationLayer::CAnimationLayer() 
{ 
}

CAnimationLayer::~CAnimationLayer()
{
	for (int i = 0; i < m_nAnimatedBoneFrames; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (m_ppAnimationCurves[i][j]) delete m_ppAnimationCurves[i][j];
		}
	}
	if (m_ppAnimationCurves) delete[] m_ppAnimationCurves;

	if (m_ppAnimatedBoneFrameCaches) delete[] m_ppAnimatedBoneFrameCaches;
}

void CAnimationLayer::LoadAnimationKeyValues(int nBoneFrame, int nCurve, FILE *pInFile)
{
	int nAnimationKeys = ::ReadIntegerFromFile(pInFile);

	m_ppAnimationCurves[nBoneFrame][nCurve] = new CAnimationCurve(nAnimationKeys);

	::fread(m_ppAnimationCurves[nBoneFrame][nCurve]->m_pfKeyTimes, sizeof(float), nAnimationKeys, pInFile);
	::fread(m_ppAnimationCurves[nBoneFrame][nCurve]->m_pfKeyValues, sizeof(float), nAnimationKeys, pInFile);
}

XMFLOAT4X4 CAnimationLayer::GetSRT(int nBoneFrame, float fPosition, float fTrackWeight) 
{ 
	CGameObject *pBoneFrame = m_ppAnimatedBoneFrameCaches[nBoneFrame];
	XMFLOAT3 xmf3S = pBoneFrame->m_xmf3Scale;
	XMFLOAT3 xmf3R = pBoneFrame->m_xmf3Rotation;
	XMFLOAT3 xmf3T = pBoneFrame->m_xmf3Translation;

	if (m_ppAnimationCurves[nBoneFrame][0]) xmf3T.x = m_ppAnimationCurves[nBoneFrame][0]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][1]) xmf3T.y = m_ppAnimationCurves[nBoneFrame][1]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][2]) xmf3T.z = m_ppAnimationCurves[nBoneFrame][2]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][3]) xmf3R.x = m_ppAnimationCurves[nBoneFrame][3]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][4]) xmf3R.y = m_ppAnimationCurves[nBoneFrame][4]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][5]) xmf3R.z = m_ppAnimationCurves[nBoneFrame][5]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][6]) xmf3S.x = m_ppAnimationCurves[nBoneFrame][6]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][7]) xmf3S.y = m_ppAnimationCurves[nBoneFrame][7]->GetValueByLinearInterpolation(fPosition);
	if (m_ppAnimationCurves[nBoneFrame][8]) xmf3S.z = m_ppAnimationCurves[nBoneFrame][8]->GetValueByLinearInterpolation(fPosition);

	float fWeight = m_fWeight * fTrackWeight;
	XMMATRIX S = XMMatrixScaling(xmf3S.x * fWeight, xmf3S.y * fWeight, xmf3S.z * fWeight);
	XMMATRIX R = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationX(xmf3R.x * fWeight), XMMatrixRotationY(xmf3R.y * fWeight)), XMMatrixRotationZ(xmf3R.z * fWeight));
	XMMATRIX T = XMMatrixTranslation(xmf3T.x * fWeight, xmf3T.y * fWeight, xmf3T.z * fWeight);

	XMFLOAT4X4 xmf4x4Transform;
	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixMultiply(XMMatrixMultiply(S, R), T));

	return(xmf4x4Transform);
}

CAnimationSet::CAnimationSet(float fStartTime, float fEndTime, char *pstrName)
{
	m_fStartTime = fStartTime;
	m_fEndTime = fEndTime;
	m_fLength = fEndTime - fStartTime;

	strcpy_s(m_pstrAnimationSetName, 64, pstrName);
}

CAnimationSet::~CAnimationSet()
{
	if (m_pAnimationLayers) delete[] m_pAnimationLayers;

	if (m_pCallbackKeys) delete[] m_pCallbackKeys;
	if (m_pAnimationCallbackHandler) delete m_pAnimationCallbackHandler;
}

void CAnimationSet::HandleCallback()
{
	if (m_pAnimationCallbackHandler)
	{
		for (int i = 0; i < m_nCallbackKeys; i++)
		{
			if (::IsEqual(m_pCallbackKeys[i].m_fTime, m_fPosition, ANIMATION_CALLBACK_EPSILON))
			{
				if (m_pCallbackKeys[i].m_pCallbackData) m_pAnimationCallbackHandler->HandleCallback(m_pCallbackKeys[i].m_pCallbackData, m_fPosition);
				break;
			}
		}
	}
}

float CAnimationSet::SetPosition(float fElapsedTime, float fTrackStartTime, float fTrackEndTime)
{
	switch (m_nType)
	{
		case ANIMATION_TYPE_LOOP:
		{
			m_fPosition += fElapsedTime;
			if (m_fPosition < fTrackStartTime) m_fPosition = fTrackStartTime;
			if (m_fPosition >= fTrackEndTime) m_fPosition = fTrackStartTime;
			break;
		}
		case ANIMATION_TYPE_ONCE:
			break;
		case ANIMATION_TYPE_PINGPONG:
			break;
	}

	return(m_fPosition);
}

void CAnimationSet::Animate(float fElapsedTime, float fTrackWeight, float fTrackStartTime, float fTrackEndTime)
{
	float fPosition = SetPosition(fElapsedTime, fTrackStartTime, fTrackEndTime);

	for (int i = 0; i < m_nAnimationLayers; i++)
	{
		for (int j = 0; j < m_pAnimationLayers[i].m_nAnimatedBoneFrames; j++) 
		{
			m_pAnimationLayers[i].m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = m_pAnimationLayers[i].GetSRT(j, fPosition, fTrackWeight);
		}
	}
}

void CAnimationSet::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSet::SetCallbackKey(int nKeyIndex, float fKeyTime, void *pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationSet::SetAnimationCallbackHandler(CAnimationCallbackHandler *pCallbackHandler)
{
	m_pAnimationCallbackHandler = pCallbackHandler;
}

CAnimationSets::CAnimationSets(int nAnimationSets)
{
	m_nAnimationSets = nAnimationSets;
	m_ppAnimationSets = new CAnimationSet*[nAnimationSets];
	for (int i = 0; i < m_nAnimationSets; i++) m_ppAnimationSets[i] = NULL;
}

CAnimationSets::~CAnimationSets()
{
	for (int i = 0; i < m_nAnimationSets; i++) if (m_ppAnimationSets[i]) delete m_ppAnimationSets[i];
	if (m_ppAnimationSets) delete[] m_ppAnimationSets;
}

void CAnimationSets::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	m_ppAnimationSets[nAnimationSet]->m_nCallbackKeys = nCallbackKeys;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSets::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void *pData)
{
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationSets::SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler)
{
	m_ppAnimationSets[nAnimationSet]->SetAnimationCallbackHandler(pCallbackHandler);
}

CAnimationController::CAnimationController(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nAnimationTracks, CLoadedModelInfo *pModel, bool bApplyRootMotion)
{
	m_nAnimationTracks = nAnimationTracks;
    m_pAnimationTracks = new CAnimationTrack[nAnimationTracks];

	m_bApplyRootMotion = bApplyRootMotion;

	SetAnimationSets(pModel->m_pAnimationSets);

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;
	m_ppSkinnedMeshes = new CSkinnedMesh*[m_nSkinnedMeshes];
	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i] = pModel->m_ppSkinnedMeshes[i];

	m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource*[m_nSkinnedMeshes];
	m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4*[m_nSkinnedMeshes];

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void **)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}
}

CAnimationController::~CAnimationController()
{
	if (m_pAnimationTracks) delete[] m_pAnimationTracks;

	if (m_pAnimationSets) m_pAnimationSets->Release();

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i]->Unmap(0, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Release();
	}
	if (m_ppd3dcbSkinningBoneTransforms) delete[] m_ppd3dcbSkinningBoneTransforms;
	if (m_ppcbxmf4x4MappedSkinningBoneTransforms) delete[] m_ppcbxmf4x4MappedSkinningBoneTransforms;

	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
}

void CAnimationController::SetAnimationSets(CAnimationSets *pAnimationSets)
{
	if (m_pAnimationSets) m_pAnimationSets->Release();

	m_pAnimationSets = pAnimationSets;
	if (m_pAnimationSets) m_pAnimationSets->AddRef();
}

void CAnimationController::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackKeys(nAnimationSet, nCallbackKeys);
}

void CAnimationController::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void *pData)
{
	if (m_pAnimationSets) m_pAnimationSets->SetCallbackKey(nAnimationSet, nKeyIndex, fKeyTime, pData);
}

void CAnimationController::SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler *pCallbackHandler)
{
	if (m_pAnimationSets) m_pAnimationSets->SetAnimationCallbackHandler(nAnimationSet, pCallbackHandler);
}

void CAnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationTracks)
	{
		m_pAnimationTracks[nAnimationTrack].SetAnimationSet(nAnimationSet);
		m_pAnimationTracks[nAnimationTrack].SetStartEndTime(m_pAnimationSets->m_ppAnimationSets[nAnimationSet]->m_fStartTime, m_pAnimationSets->m_ppAnimationSets[nAnimationSet]->m_fEndTime);
	}
}

void CAnimationController::SetTrackEnable(int nAnimationTrack, bool bEnable)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetEnable(bEnable);
}

void CAnimationController::SetTrackPosition(int nAnimationTrack, float fPosition)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetPosition(fPosition);
}

void CAnimationController::SetTrackSpeed(int nAnimationTrack, float fSpeed)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetSpeed(fSpeed);
}

void CAnimationController::SetTrackWeight(int nAnimationTrack, float fWeight)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetWeight(fWeight);
}

void CAnimationController::SetTrackStartEndTime(int nAnimationTrack, float fStartTime, float fEndTime)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetStartEndTime(fStartTime, fEndTime);
}

void CAnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppSkinnedMeshes[i]->m_pd3dcbSkinningBoneTransforms = m_ppd3dcbSkinningBoneTransforms[i];
		m_ppSkinnedMeshes[i]->m_pcbxmf4x4MappedSkinningBoneTransforms = m_ppcbxmf4x4MappedSkinningBoneTransforms[i];
	}
}

void CAnimationController::AdvanceTime(float fTimeElapsed, CGameObject *pRootGameObject) 
{
	m_fTime += fTimeElapsed; 
	if (m_pAnimationTracks)
	{
		for (int i = 0; i < m_nAnimationTracks; i++)
		{
			if (m_pAnimationTracks[i].m_bEnable)
			{				
				CAnimationSet *pAnimationSet = m_pAnimationSets->m_ppAnimationSets[m_pAnimationTracks[i].m_nAnimationSet];
				pAnimationSet->Animate(fTimeElapsed * m_pAnimationTracks[i].m_fSpeed, m_pAnimationTracks[i].m_fWeight, m_pAnimationTracks[i].m_fStartTime, m_pAnimationTracks[i].m_fEndTime);
			}
		}

		pRootGameObject->UpdateTransform(NULL);

		for (int k = 0; k < m_nAnimationTracks; k++)
		{
			if (m_pAnimationTracks[k].m_bEnable) m_pAnimationSets->m_ppAnimationSets[m_pAnimationTracks[k].m_nAnimationSet]->HandleCallback();
		}
	}
} 

CLoadedModelInfo::~CLoadedModelInfo()
{
	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
}

void CLoadedModelInfo::PrepareSkinning()
{
	int nSkinnedMesh = 0;
	m_ppSkinnedMeshes = new CSkinnedMesh*[m_nSkinnedMeshes];
	m_pModelRootObject->FindAndSetSkinnedMesh(m_ppSkinnedMeshes, &nSkinnedMesh);

	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i]->PrepareSkinning(m_pModelRootObject);
}

CGameObject::CGameObject()
{
	m_xmf4x4ToParent = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::CGameObject(int nMaterials) : CGameObject()
{
	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new CMaterial*[m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
		}
	}
	if (m_ppMaterials) delete[] m_ppMaterials;

	if (m_pSkinnedAnimationController) delete m_pSkinnedAnimationController;
}

void CGameObject::AddRef() 
{ 
	m_nReferences++; 

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void CGameObject::Release() 
{ 
	if (m_pChild) m_pChild->Release();
	if (m_pSibling) m_pSibling->Release();

	if (--m_nReferences <= 0) delete this; 
}

void CGameObject::SetChild(CGameObject *pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetBoxMesh(CMesh* pMesh)
{
	if (m_pMesh2) m_pMesh2->Release();
	m_pMesh2 = pMesh;
	if (m_pMesh2) m_pMesh2->AddRef();
}

void CGameObject::SetShader(CShader *pShader)
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = new CMaterial(0);
	m_ppMaterials[0]->SetShader(pShader);
}

void CGameObject::SetShader(int nMaterial, CShader *pShader)
{
	if (m_ppMaterials[nMaterial]) 
		m_ppMaterials[nMaterial]->SetShader(pShader);
}

void CGameObject::SetWireFrameShader()
{
 	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = NULL;
	CMaterial *pMaterial = new CMaterial(0);
	pMaterial->SetWireFrameShader();
	SetMaterial(0, pMaterial);
}

void CGameObject::SetSkinnedAnimationWireFrameShader()
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = NULL;
	CMaterial *pMaterial = new CMaterial(0);
	pMaterial->SetSkinnedAnimationWireFrameShader();
	SetMaterial(0, pMaterial);
}

void CGameObject::SetMaterial(int nMaterial, CMaterial *pMaterial)
{
	if (m_ppMaterials[nMaterial]) 
		m_ppMaterials[nMaterial]->Release();

	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

void CGameObject::UpdateBox()
{

		if (m_nReferences == 0)
		{
			if (GetPosition().x != 134)
			{
				//std::cout << GetPosition().x << std::endl;
				//std::cout << GetPosition().y << std::endl;
				//std::cout << GetPosition().z << std::endl;
			}
		}
}

CSkinnedMesh *CGameObject::FindSkinnedMesh(char *pstrSkinnedMeshName)
{
	CSkinnedMesh *pSkinnedMesh = NULL;
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT)) 
	{
		pSkinnedMesh = (CSkinnedMesh *)m_pMesh;
		if(!strcmp(pSkinnedMesh->m_pstrMeshName, pstrSkinnedMeshName)) return(pSkinnedMesh);
	}

	if (m_pSibling) if (pSkinnedMesh = m_pSibling->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);
	if (m_pChild) if (pSkinnedMesh = m_pChild->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);

	return(NULL);
}

void CGameObject::FindAndSetSkinnedMesh(CSkinnedMesh **ppSkinnedMeshes, int *pnSkinnedMesh)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT)) ppSkinnedMeshes[(*pnSkinnedMesh)++] = (CSkinnedMesh *)m_pMesh;

	if (m_pSibling) m_pSibling->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
	if (m_pChild) m_pChild->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
}

CGameObject *CGameObject::FindFrame(char *pstrFrameName)
{
	CGameObject *pFrameObject = NULL;

	if (!strcmp(m_pstrFrameName, pstrFrameName)) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::SetActive(char *pstrFrameName, bool bActive)
{
	CGameObject *pFrameObject = FindFrame(pstrFrameName);
	if (pFrameObject) pFrameObject->m_bActive = bActive;
}

void CGameObject::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackAnimationSet(nAnimationTrack, nAnimationSet);
}

void CGameObject::SetTrackAnimationPosition(int nAnimationTrack, float fPosition)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackPosition(nAnimationTrack, fPosition);
}

void CGameObject::Animate(float fTimeElapsed)
{
	OnPrepareRender();

	UpdateBox();

	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild)
	{
		//XMFLOAT4X4	m_xmf4x4InverseWorld = ::Matrix4x4::Inverse(m_xmf4x4World);
		//m_pChild->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4InverseWorld));
		//XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
		m_pChild->Animate(fTimeElapsed);
	}
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (m_bActive)
	{
		if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

		if (m_pMesh)
		{
			UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

			if (m_nMaterials > 0)
			{
				for (int i = 0; i < m_nMaterials; i++)
				{
					if (m_ppMaterials[i])
					{
						if (m_ppMaterials[i]->m_pShader)
							m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
						m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
					}
					m_pMesh->Render(pd3dCommandList, i);
				}
			}
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}

void CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &m_xmf3Color, 16);
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, CMaterial *pMaterial)
{
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParent._41 = x;
	m_xmf4x4ToParent._42 = y;
	m_xmf4x4ToParent._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4 *pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

UINT ReadUnsignedIntegerFromFile(FILE *pInFile)
{
	UINT nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(UINT), 1, pInFile); 
	return(nValue);
}

int ReadIntegerFromFile(FILE *pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile); 
	return(nValue);
}

float ReadFloatFromFile(FILE *pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile); 
	return(fValue);
}

int ReadStringFromFile(FILE *pInFile, char *pstrToken)
{
	int nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(int), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile); 
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

CGameObject *CGameObject::LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject *pParent, FILE *pInFile, CShader *pShader, int *pnSkinnedMeshes)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = ::ReadIntegerFromFile(pInFile);

	CGameObject *pGameObject = new CGameObject();
	::ReadStringFromFile(pInFile, pGameObject->m_pstrFrameName);

	for ( ; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Transform>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4ToParent, sizeof(XMFLOAT4X4), 1, pInFile);

			nReads = (UINT)::fread(&pGameObject->m_xmf3Scale, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&pGameObject->m_xmf3Rotation, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&pGameObject->m_xmf3Translation, sizeof(XMFLOAT3), 1, pInFile);
			 
	}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CMesh *pMesh = new CMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(pMesh);
			//pGameObject->SetBoxMesh(pMesh);

			/**/pGameObject->SetWireFrameShader();
		}
		else if (!strcmp(pstrToken, "<SkinDeformations>:"))
		{
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;


			CSkinnedMesh *pSkinnedMesh = new CSkinnedMesh(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->LoadSkinDeformationsFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pSkinnedMesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

			::ReadStringFromFile(pInFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			pGameObject->SetMesh(pSkinnedMesh);

			/**/pGameObject->SetSkinnedAnimationWireFrameShader();
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<Frame>:"))
					{
						CGameObject *pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader, pnSkinnedMeshes);
						if(pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
						TCHAR pstrDebug[256] = { 0 };
						_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
						OutputDebugString(pstrDebug);
#endif
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

void CGameObject::PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

void CGameObject::LoadAnimationFromFile(FILE *pInFile, CLoadedModelInfo *pLoadedModel)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nAnimationSets = 0;

	for ( ; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nAnimationSets = ::ReadIntegerFromFile(pInFile);
			pLoadedModel->m_pAnimationSets = new CAnimationSets(nAnimationSets);
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			int nAnimationSet = ::ReadIntegerFromFile(pInFile);

			::ReadStringFromFile(pInFile, pstrToken); //Animation Set Name

			float fStartTime = ::ReadFloatFromFile(pInFile);
			float fEndTime = ::ReadFloatFromFile(pInFile);

			pLoadedModel->m_pAnimationSets->m_ppAnimationSets[nAnimationSet] = new CAnimationSet(fStartTime, fEndTime, pstrToken);
			CAnimationSet *pAnimationSet = pLoadedModel->m_pAnimationSets->m_ppAnimationSets[nAnimationSet];

			::ReadStringFromFile(pInFile, pstrToken);
			if (!strcmp(pstrToken, "<AnimationLayers>:"))
			{
				pAnimationSet->m_nAnimationLayers = ::ReadIntegerFromFile(pInFile);
				pAnimationSet->m_pAnimationLayers = new CAnimationLayer[pAnimationSet->m_nAnimationLayers];

				for (int i = 0; i < pAnimationSet->m_nAnimationLayers; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<AnimationLayer>:"))
					{
						int nAnimationLayer = ::ReadIntegerFromFile(pInFile);
						CAnimationLayer *pAnimationLayer = &pAnimationSet->m_pAnimationLayers[nAnimationLayer];

						pAnimationLayer->m_nAnimatedBoneFrames = ::ReadIntegerFromFile(pInFile);

						pAnimationLayer->m_ppAnimatedBoneFrameCaches = new CGameObject *[pAnimationLayer->m_nAnimatedBoneFrames];
						pAnimationLayer->m_ppAnimationCurves = new CAnimationCurve *[pAnimationLayer->m_nAnimatedBoneFrames][9];

						pAnimationLayer->m_fWeight = ::ReadFloatFromFile(pInFile);

						for (int j = 0; j < pAnimationLayer->m_nAnimatedBoneFrames; j++)
						{
							::ReadStringFromFile(pInFile, pstrToken);
							if (!strcmp(pstrToken, "<AnimationCurve>:"))
							{
								int nCurveNode = ::ReadIntegerFromFile(pInFile); //j

								for (int k = 0; k < 9; k++) pAnimationLayer->m_ppAnimationCurves[j][k] = NULL;

								::ReadStringFromFile(pInFile, pstrToken);
								pAnimationLayer->m_ppAnimatedBoneFrameCaches[j] = pLoadedModel->m_pModelRootObject->FindFrame(pstrToken);

								for ( ; ; )
								{
									::ReadStringFromFile(pInFile, pstrToken);
									if (!strcmp(pstrToken, "<TX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 0, pInFile);
									else if(!strcmp(pstrToken, "<TY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 1, pInFile);
									else if(!strcmp(pstrToken, "<TZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 2, pInFile);
									else if(!strcmp(pstrToken, "<RX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 3, pInFile);
									else if(!strcmp(pstrToken, "<RY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 4, pInFile);
									else if(!strcmp(pstrToken, "<RZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 5, pInFile);
									else if(!strcmp(pstrToken, "<SX>:")) pAnimationLayer->LoadAnimationKeyValues(j, 6, pInFile);
									else if(!strcmp(pstrToken, "<SY>:")) pAnimationLayer->LoadAnimationKeyValues(j, 7, pInFile);
									else if(!strcmp(pstrToken, "<SZ>:")) pAnimationLayer->LoadAnimationKeyValues(j, 8, pInFile);
									else if(!strcmp(pstrToken, "</AnimationCurve>"))
									{
										break;
									}
								}
							}
						}
						::ReadStringFromFile(pInFile, pstrToken); //</AnimationLayer>
					}
				}
				::ReadStringFromFile(pInFile, pstrToken); //</AnimationLayers>
			}
			::ReadStringFromFile(pInFile, pstrToken); //</AnimationSet>
		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			break;
		}
	}
}

CLoadedModelInfo *CGameObject::LoadGeometryAndAnimationFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName, CShader *pShader)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CLoadedModelInfo *pLoadedModel = new CLoadedModelInfo();
	pLoadedModel->m_pModelRootObject = new CGameObject();
	strcpy_s(pLoadedModel->m_pModelRootObject->m_pstrFrameName, "RootNode");

	char pstrToken[64] = { '\0' };

	for ( ; ; )
	{
		if (::ReadStringFromFile(pInFile, pstrToken))
		{
			if (!strcmp(pstrToken, "<Hierarchy>"))
			{
				for ( ; ; )
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<Frame>:"))
					{
						CGameObject *pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader, &pLoadedModel->m_nSkinnedMeshes);
						if (pChild) pLoadedModel->m_pModelRootObject->SetChild(pChild);
					}
					else if (!strcmp(pstrToken, "</Hierarchy>"))
					{
						break;
					}
				}
			}
			else if (!strcmp(pstrToken, "<Animation>"))
			{
				CGameObject::LoadAnimationFromFile(pInFile, pLoadedModel);
				pLoadedModel->PrepareSkinning();
			}
			else if (!strcmp(pstrToken, "</Animation>"))
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pLoadedModel->m_pModelRootObject, NULL);
#endif

	::fclose(pInFile);

	return(pLoadedModel);
}

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(1)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	CHeightMapGridMesh *pMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nWidth, nLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
	SetMesh(pMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainBaseTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainBaseTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/Base_Texture.dds", 0);

	CTexture *pTerrainDetailTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainDetailTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/Detail_Texture_1.dds", 0);
	
	CTerrainShader *pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainBaseTexture, 13, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainDetailTexture, 14, false);

	CMaterial *pTerrainMaterial = new CMaterial(2);
	pTerrainMaterial->SetTexture(pTerrainBaseTexture, 0);
	pTerrainMaterial->SetTexture(pTerrainDetailTexture, 1);
	pTerrainMaterial->SetShader(pTerrainShader);

	SetMaterial(0, pTerrainMaterial);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

CSkyBox::CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature) : CGameObject(1)
{
	CSkyBoxMesh *pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 2.0f);
	SetMesh(pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"SkyBox/SkyBox_0.dds", 0);

	CSkyBoxShader *pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 10, false);

	CMaterial *pSkyBoxMaterial = new CMaterial(1);
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(0, pSkyBoxMaterial);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	CGameObject::Render(pd3dCommandList, pCamera);
}

CAngrybotObject::CAngrybotObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, int nAnimationTracks)
{
	CLoadedModelInfo *pAngrybotModel = pModel;
	if (!pAngrybotModel) pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Angrybot.bin", NULL);

	SetChild(pAngrybotModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pAngrybotModel);

	strcpy_s(m_pstrFrameName, "Angrybot");

	Rotate(-90.0f, 0.0f, 0.0f);
	SetScale(0.2f, 0.2f, 0.2f);
}

CAngrybotObject::~CAngrybotObject()
{
}


Champion::Champion()
{

}

Champion::~Champion()
{

}

bool CGameObject::isDestination(int row, int col, Pair dst)
{
	if (row == dst.first && col == dst.second) return true;
	return false;
}

bool CGameObject::isRanger(int row, int col)
{
	return (row >= 0 && row < ROW&& col >= 0 && col < COL);
}

bool CGameObject::isUnBlock(int row, int col)
{
	return (Bmap[row][col] == 0);
}

float CGameObject::GetValue(int row, int col, Pair dst)
{
	return (double)std::sqrt(std::pow(row - dst.first, 2) + std::pow(col - dst.second, 2));
}


void CGameObject::Path(Cell cellDetails[101][101], Pair dst)
{
	// 여기서 이동구현
	//std::stack<Pair> s;
	int y = dst.first;
	int x = dst.second;

	s.push({ y, x });
	// cellDetails의 x, y의 부모좌표가 모두 현재좌표와 동일할때까지 반복
	while (!(cellDetails[y][x].parent_x == x && cellDetails[y][x].parent_y == y))
	{
		int tempy = cellDetails[y][x].parent_y;
		int tempx = cellDetails[y][x].parent_x;
		y = tempy;
		x = tempx;
		s.push({ y, x });
	}

	if(!s.empty())
	{
		int p = s.top().first;
		int q = s.top().second;
		zmap[s.top().first][s.top().second] = '*';
		//std::cout << s.top().first << " y : " << s.top().second << std::endl;
		s.pop();
		if (!s.empty())
		{
			int k = p - s.top().first;
			int j = q - s.top().second;
			//std::cout << k << "y : "<< j << std::endl;
			TracePath(k, j, 0);
		}
		
	}


}

void CGameObject::Astar(Pair src, Pair dst)
{
	

	/*if (!isRanger(src.first, src.second) || !isRanger(dst.first, dst.second)) return false;
	if (!isUnBlock(src.first, src.second) || !isUnBlock(dst.first, dst.second)) return false;
	if (isDestination(src.first, src.second, dst)) {
		return false;*/
	bool closedList[101][101];
	std::memset(closedList, false, sizeof(closedList));

	Cell cellDetails[101][101];

	// 내용초기화
		// 계산해야할 값부분은 INF로하고, 계산할 경로는 -1로 초기화
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			cellDetails[i][j].f = cellDetails[i][j].g = cellDetails[i][j].h = INF;
			cellDetails[i][j].parent_x = cellDetails[i][j].parent_y = -1;
		}
	}


	// src의 좌표가 첫좌표가 된다.
	int sy = src.first;
	int sx = src.second;
	cellDetails[sy][sx].f = cellDetails[sy][sx].g = cellDetails[sy][sx].h = 0.0;
	cellDetails[sy][sx].parent_x = sx;
	cellDetails[sy][sx].parent_y = sy;

	std::set<pPair> openList;
	openList.insert({ 0.0, { sy, sx } });

	while (!openList.empty()) {
		pPair p = *openList.begin();
		openList.erase(openList.begin());

		int y = p.second.first;
		int x = p.second.second;
		closedList[y][x] = true;

		double ng, nf, nh;

		// 직선
		for (int i = 0; i < 4; ++i) {
			int ny = y + dy1[i];
			int nx = x + dx1[i];

			if (isRanger(ny, nx)) {
				if (isDestination(ny, nx, dst)) {
					cellDetails[ny][nx].parent_y = y;
					cellDetails[ny][nx].parent_x = x;
					Path(cellDetails, dst);
					//return true;
				}


				else if (!closedList[ny][nx] && isUnBlock(ny, nx)) {
					// 이부분 y x, ny nx 헷갈리는거 조심
					ng = cellDetails[y][x].g + 1.0;
					nh = GetValue(ny, nx, dst);
					nf = ng + nh;

					// 만약 한번도 갱신이 안된f거나, 새로갱신될 f가 기존f보다 작을시 참
					if (cellDetails[ny][nx].f == INF || cellDetails[ny][nx].f > nf) {
						cellDetails[ny][nx].f = nf;
						cellDetails[ny][nx].g = ng;
						cellDetails[ny][nx].h = nh;
						cellDetails[ny][nx].parent_x = x;
						cellDetails[ny][nx].parent_y = y;
						openList.insert({ nf, { ny, nx } });
					}
				}
			}
		}

		// 대각선
		/*for (int i = 0; i < 4; ++i) {
			int ny = y + dy2[i];
			int nx = x + dx2[i];

			if (isRanger(ny, nx)) {
				if (isDestination(ny, nx, dst)) {
					cellDetails[ny][nx].parent_y = y;
					cellDetails[ny][nx].parent_x = x;
					Path(cellDetails, dst);
					return true;
				}
				else if (!closedList[ny][nx] && isUnBlock(ny, nx)) {
					ng = cellDetails[y][x].g + 1.414;
					nh = GetValue(ny, nx, dst);
					nf = ng + nh;

					if (cellDetails[ny][nx].f == INF || cellDetails[ny][nx].f > nf) {
						cellDetails[ny][nx].f = nf;
						cellDetails[ny][nx].g = ng;
						cellDetails[ny][nx].h = nh;
						cellDetails[ny][nx].parent_x = x;
						cellDetails[ny][nx].parent_y = y;
						openList.insert({ nf, { ny, nx } });
					}
				}
			}
		}*/
	}

	//return false;
}

void CGameObject::PrintMap()
{
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			std::cout << zmap[i][j];
		}
		std::cout << '\n';
	}
	std::cout << '\n';
	std::cout << '\n';
	std::cout << '\n';
	
		if (GetPosition().x != 134)
		{
			//std::cout << GetPosition().x << std::endl;
			//std::cout << GetPosition().y << std::endl;
			//std::cout << GetPosition().z << std::endl;
		}
	
}



std::vector<std::vector<int>> CGameObject::fileload(std::string filepath)
{
	std::ifstream ifs(filepath);

	int col, row, cur = 0;
	ROW = 9;
	COL = 9;
	
	std::vector<std::vector<int>> result(ROW, std::vector<int>(COL));

	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			//result[i][j] = MakingMap();
		}
	}

	return result;
	

	return std::vector<std::vector<int>>();
}

void CGameObject::Checking()
{
	Pair src, dst;
	int row, col;

	/// 방법1 - 맵정보 직접 입력하기 
	/*
	std::cin >> row >> col;
	ROW = row;
	COL = col;

	std::vector<std::vector<int>> grid(row, std::vector<int>(col));

	for(int i=0;i<row;++i){
		for(int j=0;j<col;++j){
			std::cin >> grid[i][j];
		}
	}*/

	/// 방법2 - 파일로 부터 맵정보 불러오기 
	//std::vector<std::vector<int>> grid = fileload("MAP.txt");
	//if (grid.empty()) std::cout << "파일이 안열림" << std::endl;

	if (tt == 0)
	{
		//MakingMap();
		tt++;
	}
	if (tt == 600)
		tt = 0;
	MakingMap();
	//MoveUp(-100.0f);
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			if (Bmap[i][j] == 2) {
				src = { i, j };
				Bmap[i][j] = 0;
			}
			if (Bmap[i][j] == 3) {
				dst = { i, j };
				Bmap[i][j] = 0;
			}
			
		}
	}
	//std::cout <<"목적지"<< dst.first << " ," << dst.second << std::endl;
	//std::cout << "출발지" << src.first << " ," << src.second << std::endl;
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			zmap[i][j] = Bmap[i][j] +'0';
		}
	}

	Astar(src, dst);
	PrintMap();

}

void CGameObject::TracePath(int x, int y, int z)
{
	a++;
	//std::cout <<  a  << std::endl;
	if (x == -1) // 아래로 이동
	{
		//std::cout << " a " << std::endl;
		MoveUp(1);
	}
	else if (x == 1) // 위로 이동
	{
		//std::cout << " b " << std::endl;
		MoveUp(-1);
	}
	else if (y == -1) // 오른쪽이동
	{
		//std::cout << " c " << std::endl;
		MoveStrafe(-1);
	}
	else if (y == 1) // 왼쪽이동
	{
		//std::cout << " d " << std::endl;
		MoveStrafe(1);
	}
}

void CGameObject::MakingMap()
{
	list<CGameObject*> War = ObjectManager::GetInstance()->GetObjectList(ObjectManager::OT_WARRIOR);
	list<CGameObject*> Mine = ObjectManager::GetInstance()->GetObjectList(ObjectManager::OT_MY_UNIT);
	int Cx, Cz;
	int p, q;
	int a = 0;
	for (int i = 0; i < 9; ++i)
	{
		for (int j = 0; j < 9; j++)
		{
			Bmap[i][j] = 0;
		}
	}
	for (auto& ex : War)
	{
		Cx = ex->GetPosition().x;
		Cz = ex->GetPosition().z;
		//std::cout << Cx << Cz << std::endl;
		p = (1460 - Cx) / 140;
		q = (1540 - Cz) / 160;
		//std:: cout << "Cx : " << Cx << " , Cz : " << Cz << std::endl;
		int i = abs(8 - p);
		int j = abs(8 - q);
		Bmap[q][p] = 1;
		//a++;
		//tt++;
		//std::cout << a << std::endl;
	}

	for (auto& ex : Mine)
	{
		Cx = ex->GetPosition().x;
		Cz = ex->GetPosition().z;
		//std::cout << Cx << Cz << std::endl;
		p = (1460 - Cx) / 140;
		q = (1540 - Cz) / 160;
		//std:: cout << "Cx2 : " << Cx << " , Cz2 : " << Cz << " , j : " << p << " , k : " << q << std::endl;
		int i = abs(8 - p);
		int j = abs(8 - q);
		Bmap[q][p] = 5;
		//a++;
		//tt++;
		//std::cout << a << std::endl;
	}

	for (auto& ex : War)
	{
		Cx = ex->GetPosition().x;
		Cz = ex->GetPosition().z;

		p = (1460 - Cx) / 140;
		q = (1540 - Cz) / 160;
		//std:: cout << "Cx2 : " << Cx2 << " , Cz2 : " << Cz2 << " , j : " << p << " , k : " << q << std::endl;
		if (Cx == GetPosition().x && Cz == GetPosition().z)
		{
			Bmap[q][p] = 2;
			CaldisEnemy(q, p);
			//std::cout << p<< " , " << q << std::endl;
		}

		
	}
	

}

void CGameObject::Caldis(int x, int y)
{
	int Mindis = 10000;
	int temp = 0;
	
	for (int i = 0; i < 9; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			if (Bmap[i][j] == 4)
			{
				//std::cout << i << j << std::endl;
				temp = sqrt(pow(x - i, 2)) + (pow(y - j, 2));
				if (temp <= Mindis)
				{
					Mindis = temp;
					Mindisx = i;
					Mindisy = j;
				}
			}
		}
	}

	//std::cout << Mindisx << Mindisy << std::endl;
	Bmap[Mindisx][Mindisy] = 3;
}

void CGameObject::CaldisEnemy(int x, int y)
{
	int Mindis = 10000;
	int temp = 0;

	for (int i = 0; i < 9; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			if (Bmap[i][j] == 5)
			{
				//std::cout << i << j << std::endl;
				temp = sqrt(pow(x - i, 2)) + (pow(y - j, 2));
				if (temp <= Mindis)
				{
					Mindis = temp;
					Mindisx = i;
					Mindisy = j;
				}
			}
		}
	}

	//std::cout << Mindisx << Mindisy << std::endl;
	Bmap[Mindisx][Mindisy] = 3;
}


void CGameObject::CheckingOwn()
{
	Pair src, dst;
	int row, col;

	/// 방법1 - 맵정보 직접 입력하기 
	/*
	std::cin >> row >> col;
	ROW = row;
	COL = col;

	std::vector<std::vector<int>> grid(row, std::vector<int>(col));

	for(int i=0;i<row;++i){
		for(int j=0;j<col;++j){
			std::cin >> grid[i][j];
		}
	}*/

	/// 방법2 - 파일로 부터 맵정보 불러오기 
	//std::vector<std::vector<int>> grid = fileload("MAP.txt");
	//if (grid.empty()) std::cout << "파일이 안열림" << std::endl;

	if (tt == 0)
	{
		//MakingMap();
		tt++;
	}
	if (tt == 600)
		tt = 0;
	MakingOwnMap();
	//MoveUp(-100.0f);
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			if (Bmap[i][j] == 2) {
				src = { i, j };
				Bmap[i][j] = 0;
			}
			if (Bmap[i][j] == 3) {
				dst = { i, j };
				Bmap[i][j] = 0;
			}

		}
	}
	//std::cout <<"목적지"<< dst.first << " ," << dst.second << std::endl;
	//std::cout << "출발지" << src.first << " ," << src.second << std::endl;
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			zmap[i][j] = Bmap[i][j] + '0';
		}
	}

	Astar(src, dst);
	PrintMap();
}

void CGameObject::MakingOwnMap()
{
	list<CGameObject*> War = ObjectManager::GetInstance()->GetObjectList(ObjectManager::OT_WARRIOR);
	list<CGameObject*> Mine = ObjectManager::GetInstance()->GetObjectList(ObjectManager::OT_MY_UNIT);
	int Cx, Cz;
	int p, q;
	int a = 0;
	for (int i = 0; i < 9; ++i)
	{
		for (int j = 0; j < 9; j++)
		{
			Bmap[i][j] = 0;
		}
	}
	for (auto& ex : Mine)
	{
		Cx = ex->GetPosition().x;
		Cz = ex->GetPosition().z;
		//std::cout << Cx << Cz << std::endl;
		p = (1460 - Cx) / 140;
		q = (1540 - Cz) / 160;
		int i = abs(8 - p);
		int j = abs(8 - q);
		Bmap[q][p] = 1;
		std::cout << "Cx2 : " << Cx << " , Cz2 : " << Cz << " , j : " << i << " , k : " << j << std::endl;
		//a++;
		//tt++;
		//std::cout << a << std::endl;
	}

	for (auto& ex : War)
	{
		Cx = ex->GetPosition().x;
		Cz = ex->GetPosition().z;
		//std::cout << Cx << Cz << std::endl;
		p = (1460 - Cx) / 140;
		q = (1540 - Cz) / 160;
		//std:: cout << "Cx2 : " << Cx2 << " , Cz2 : " << Cz2 << " , j : " << p << " , k : " << q << std::endl;
		int i = abs(8 - p);
		int j = abs(8 - q);
		Bmap[q][p] = 4;
		//std::cout << i<< " , " << j << std::endl;
		//std::cout << Cx << " , " << Cz << std::endl;
	}

	for (auto& ex : Mine)
	{
		Cx = ex->GetPosition().x;
		Cz = ex->GetPosition().z;

		p = (1460 - Cx) / 140;
		q = (1540 - Cz) / 160;
		//std:: cout << "Cx2 : " << Cx2 << " , Cz2 : " << Cz2 << " , j : " << p << " , k : " << q << std::endl;
		if (Cx == GetPosition().x && Cz == GetPosition().z)
		{
			int i = abs(8-p);
			int j = abs(8 - q);
			Bmap[q][p] = 2;
			Caldis(i, j);
			//std::cout << i<< " , " << j << std::endl;
			//std::cout << Cx << " , " << Cz << std::endl;
		}

	}
}


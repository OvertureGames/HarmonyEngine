/**************************************************************************
*	IHyLeafDraw2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Leafs/IHyLeafDraw2d.h"
#include "HyEngine.h"

/*static*/ HyAssets *IHyLeafDraw2d::sm_pHyAssets = nullptr;

IHyLeafDraw2d::IHyLeafDraw2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent) :	IHyNodeDraw2d(eNodeType, pParent),
																												m_eLoadState(HYLOADSTATE_Inactive),
																												m_pData(nullptr),
																												m_sNAME(szName ? szName : ""),
																												m_sPREFIX(szPrefix ? szPrefix : ""),
																												m_BoundingVolume(*this)
{
}

IHyLeafDraw2d::~IHyLeafDraw2d()
{
	if(m_eLoadState != HYLOADSTATE_Inactive)
		Unload();
}

void IHyLeafDraw2d::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
	m_uiExplicitFlags |= EXPLICIT_Enabled;
}

void IHyLeafDraw2d::SetPauseUpdate(bool bUpdateWhenPaused)
{
	if(bUpdateWhenPaused)
	{
		if(m_bPauseOverride == false)
			HyScene::AddNode_PauseUpdate(this);
	}
	else
	{
		if(m_bPauseOverride == true)
			HyScene::RemoveNode_PauseUpdate(this);
	}

	m_bPauseOverride = bUpdateWhenPaused;
	m_uiExplicitFlags |= EXPLICIT_PauseUpdate;
}

void IHyLeafDraw2d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight)
{
	m_LocalScissorRect.x = uiLocalX;
	m_LocalScissorRect.y = uiLocalY;
	m_LocalScissorRect.width = uiWidth;
	m_LocalScissorRect.height = uiHeight;

	m_LocalScissorRect.iTag = 1;
	m_uiExplicitFlags |= EXPLICIT_Scissor;
}

void IHyLeafDraw2d::ClearScissor(bool bUseParentScissor)
{
	m_LocalScissorRect.iTag = 0;
	m_RenderState.ClearScissorRect();

	if(bUseParentScissor == false)
		m_uiExplicitFlags |= EXPLICIT_Scissor;
	else
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;
}

void IHyLeafDraw2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	m_uiExplicitFlags |= EXPLICIT_DisplayOrder;

	HyScene::SetInstOrderingDirty();
}

const std::string &IHyLeafDraw2d::GetName()
{
	return m_sNAME;
}

const std::string &IHyLeafDraw2d::GetPrefix()
{
	return m_sPREFIX;
}

IHyNodeData *IHyLeafDraw2d::AcquireData()
{
	if(m_pData == nullptr)
	{
		HyAssert(sm_pHyAssets != nullptr, "AcquireData was called before the engine has initialized HyAssets");

		sm_pHyAssets->GetNodeData(this, m_pData);
		if(m_pData)
			OnDataAcquired();
		else
			HyAssert(m_eTYPE == HYTYPE_Primitive2d, "Could not find data for: " << GetPrefix() << "/" << GetName());
	}

	return m_pData;
}

const HyShape2d &IHyLeafDraw2d::GetBoundingVolume()
{
	if(IsDirty(DIRTY_BoundingVolume) || m_BoundingVolume.IsValid() == false)
	{
		CalcBoundingVolume();
		ClearDirty(DIRTY_BoundingVolume);
	}

	return m_BoundingVolume;
}

const b2AABB &IHyLeafDraw2d::GetWorldAABB()
{
	if(IsDirty(DIRTY_WorldAABB))
	{
		glm::mat4 mtxWorld;
		GetWorldTransform(mtxWorld);
		float fWorldRotationRadians = glm::atan(mtxWorld[0][1], mtxWorld[0][0]);

		GetBoundingVolume(); // This will update BV if it's dirty
		m_BoundingVolume.GetB2Shape()->ComputeAABB(&m_aabbCached, b2Transform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(fWorldRotationRadians)), 0);

		ClearDirty(DIRTY_WorldAABB);
	}

	return m_aabbCached;
}

HyShape2d *IHyLeafDraw2d::GetUserBoundingVolume(uint32 uiIndex)
{
	return nullptr;
	//return m_BoundingVolumeList[uiIndex].Get;
}

int32 IHyLeafDraw2d::GetCoordinateSystem()
{
	return m_RenderState.GetAssignedWindow();
}

void IHyLeafDraw2d::UseCameraCoordinates()
{
	m_RenderState.SetCoordinateSystem(-1);
}

void IHyLeafDraw2d::UseWindowCoordinates(int32 iWindowIndex /*= 0*/)
{
	m_RenderState.SetCoordinateSystem(iWindowIndex);
}

int32 IHyLeafDraw2d::GetShaderId()
{
	return m_RenderState.GetShaderId();
}

void IHyLeafDraw2d::SetCustomShader(IHyShader *pShader)
{
	HyAssert(m_eLoadState == HYLOADSTATE_Inactive, "IHyDraw2d::SetCustomShader was used on an already loaded instance - I can make this work I just haven't yet");
	HyAssert(pShader->IsFinalized(), "IHyDraw2d::SetCustomShader tried to set a non-finalized shader");
	HyAssert(pShader->GetId() >= HYSHADERPROG_CustomStartIndex, "HyGfxData::SetRequiredCustomShaderId was passed an invalid custom shader Id");

	m_RequiredCustomShaders.clear();
	m_RequiredCustomShaders.insert(pShader->GetId());
	m_RenderState.SetShaderId(pShader->GetId());
}

/*virtual*/ bool IHyLeafDraw2d::IsLoaded() const /*override*/
{
	return m_eLoadState == HYLOADSTATE_Loaded;
}

/*virtual*/ void IHyLeafDraw2d::Load() /*override*/
{
	HyAssert(sm_pHyAssets, "IHyDraw2d::Load was invoked before engine has been initialized");

	// Don't load if the name is blank, and it's required by this node type
	if(m_sNAME.empty() && m_eTYPE != HYTYPE_Entity2d && m_eTYPE != HYTYPE_Primitive2d && m_eTYPE != HYTYPE_TexturedQuad2d)
		return;

	if(m_eTYPE != HYTYPE_Entity2d)
		sm_pHyAssets->LoadNodeData(this);
}

/*virtual*/ void IHyLeafDraw2d::Unload() /*override*/
{
	HyAssert(sm_pHyAssets, "IHyDraw2d::Unload was invoked before engine has been initialized");	
	sm_pHyAssets->RemoveNodeData(this);
}

/*virtual*/ void IHyLeafDraw2d::NodeUpdate() /*override final*/
{
	if((m_uiExplicitFlags & EXPLICIT_Scissor) != 0)
	{
		if(m_LocalScissorRect.iTag == 1)
		{
			glm::mat4 mtx;
			GetWorldTransform(mtx);

			m_RenderState.SetScissorRect(static_cast<int32>(mtx[3].x + m_LocalScissorRect.x),
										 static_cast<int32>(mtx[3].y + m_LocalScissorRect.y),
										 static_cast<uint32>(mtx[0].x * m_LocalScissorRect.width),
										 static_cast<uint32>(mtx[1].y * m_LocalScissorRect.height));
		}
		else
		{
			m_RenderState.ClearScissorRect();
		}
	}

	DrawUpdate();

	if(m_eLoadState == HYLOADSTATE_Loaded)
	{
		OnUpdateUniforms();
		m_RenderState.SetUniformCrc32(m_ShaderUniforms.GetCrc32());
	}
}

/*virtual*/ void IHyLeafDraw2d::_SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;

	if(0 == (m_uiExplicitFlags & EXPLICIT_Scissor))
	{
		if(worldScissorRectRef.iTag == 1)
			m_RenderState.SetScissorRect(worldScissorRectRef);
		else
			m_RenderState.ClearScissorRect();
	}
}

/*virtual*/ int32 IHyLeafDraw2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_DisplayOrder;

	if(0 == (m_uiExplicitFlags & EXPLICIT_DisplayOrder))
	{
		m_iDisplayOrder = iOrderValue;
		iOrderValue += 1;

		HyScene::SetInstOrderingDirty();
	}

	return iOrderValue;
}

/*virtual*/ void IHyLeafDraw2d::_UseCameraCoordinates(bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_CoordinateSystem;

	if(0 == (m_uiExplicitFlags & EXPLICIT_CoordinateSystem))
		UseCameraCoordinates();
}

/*virtual*/ void IHyLeafDraw2d::_UseWindowCoordinates(int32 iWindowIndex, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_CoordinateSystem;

	if(0 == (m_uiExplicitFlags & EXPLICIT_CoordinateSystem))
		UseWindowCoordinates(iWindowIndex);
}

IHyNodeData *IHyLeafDraw2d::UncheckedGetData()
{
	return m_pData;
}

const HyRenderState &IHyLeafDraw2d::GetRenderState() const
{
	return m_RenderState;
}

void IHyLeafDraw2d::WriteShaderUniformBuffer(char *&pRefDataWritePos)
{
	m_ShaderUniforms.WriteUniformsBufferData(pRefDataWritePos);
}


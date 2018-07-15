/**************************************************************************
*	IHyDrawable3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/Drawables/IHyDrawable3d.h"
#include "HyEngine.h"

HyScene *IHyDrawable3d::sm_pScene = nullptr;

IHyDrawable3d::IHyDrawable3d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity3d *pParent) :	IHyLoadable3d(eNodeType, szPrefix, szName, pParent)
{
}

IHyDrawable3d::IHyDrawable3d(const IHyDrawable3d &copyRef) :	IHyLoadable3d(copyRef),
																IHyDrawable(copyRef)
{
}

IHyDrawable3d::~IHyDrawable3d()
{
	if(m_eLoadState != HYLOADSTATE_Inactive)
		Unload();
}

const IHyDrawable3d &IHyDrawable3d::operator=(const IHyDrawable3d &rhs)
{
	IHyLoadable3d::operator=(rhs);
	IHyDrawable::operator=(rhs);

	return *this;
}

/*virtual*/ bool IHyDrawable3d::IsValid() /*override final*/
{
	return m_bEnabled && OnIsValid();
}

/*virtual*/ void IHyDrawable3d::NodeUpdate() /*override final*/
{
	if(IsLoaded())
	{
		// This update will set the appearance of the instance to its current state
		DrawLoadedUpdate();
		OnUpdateUniforms();
	}
}

/*virtual*/ void IHyDrawable3d::OnLoaded() /*override*/
{
	if(m_hShader == HY_UNUSED_HANDLE)
		m_hShader = Hy_DefaultShaderHandle(m_eTYPE);

	sm_pScene->AddNode_Loaded(this);
}

/*virtual*/ void IHyDrawable3d::OnUnloaded() /*override*/
{
	sm_pScene->RemoveNode_Loaded(this);
}

/*virtual*/ HyType IHyDrawable3d::_DrawableGetType() /*override*/
{
	return m_eTYPE;
}

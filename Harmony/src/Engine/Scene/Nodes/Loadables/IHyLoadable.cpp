/**************************************************************************
*	IHyLoadable.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Assets/HyAssets.h"

/*static*/ HyAssets *IHyLoadable::sm_pHyAssets = nullptr;

IHyLoadable::IHyLoadable(const char *szPrefix, const char *szName) :	m_eLoadState(HYLOADSTATE_Inactive),
																		m_pData(nullptr),
																		m_sName(szName ? szName : ""),
																		m_sPrefix(szPrefix ? szPrefix : "")
{
}

IHyLoadable::IHyLoadable(const IHyLoadable &copyRef) :	m_eLoadState(HYLOADSTATE_Inactive),
														m_pData(nullptr),
														m_sName(copyRef.m_sName),
														m_sPrefix(copyRef.m_sPrefix)
{
}

IHyLoadable::~IHyLoadable()
{
}

const IHyLoadable &IHyLoadable::operator=(const IHyLoadable &rhs)
{
	if(m_sPrefix != rhs.m_sPrefix || m_sName != rhs.m_sName)
	{
		if(m_eLoadState != HYLOADSTATE_Inactive)
			Unload();

		m_sPrefix = rhs.m_sPrefix;
		m_sName = rhs.m_sName;
		m_pData = nullptr;			// Ensures virtual OnDataAcquired() is invoked when the below Load() is invoked
	}

	if(rhs.IsLoaded())
		Load();

	return *this;
}

const std::string &IHyLoadable::GetName() const
{
	return m_sName;
}

const std::string &IHyLoadable::GetPrefix() const
{
	return m_sPrefix;
}

const IHyNodeData *IHyLoadable::AcquireData()
{
	if(m_pData == nullptr)
	{
		HyAssert(sm_pHyAssets != nullptr, "AcquireData was called before the engine has initialized HyAssets");

		sm_pHyAssets->AcquireNodeData(this, m_pData);
		if(m_pData)
			OnDataAcquired();
		else
			HyLogInfo("Could not find data for: " << GetPrefix() << "/" << GetName());
	}

	return m_pData;
}

/*virtual*/ bool IHyLoadable::IsLoaded() const /*override*/
{
	return m_eLoadState == HYLOADSTATE_Loaded;
}

/*virtual*/ void IHyLoadable::Load() /*override*/
{
	HyAssert(sm_pHyAssets, "IHyLoadable2d::Load was invoked before engine has been initialized");

	// Don't load if the name is blank, and it's required by this node type
	if(m_sName.empty() && _LoadableGetType() != HYTYPE_Entity && _LoadableGetType() != HYTYPE_Primitive && _LoadableGetType() != HYTYPE_TexturedQuad)
		return;

	sm_pHyAssets->LoadNodeData(this);
}

/*virtual*/ void IHyLoadable::Unload() /*override*/
{
	HyAssert(sm_pHyAssets, "IHyLoadable2d::Unload was invoked before engine has been initialized");
	sm_pHyAssets->RemoveNodeData(this);
}

const IHyNodeData *IHyLoadable::UncheckedGetData()
{
	return m_pData;
}
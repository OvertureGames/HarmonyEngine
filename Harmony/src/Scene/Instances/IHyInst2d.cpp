/*******************************************************************************
 *	IHyInst2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *********************************************************************************/
#include "Scene/Instances/IHyInst2d.h"

#include "FileIO/Data/IHyData.h"
#include "FileIO/IHyFileIO.h"

/*static*/ IHyFileIO *IHyInst2d::sm_pFileIO = NULL;

IHyInst2d::IHyInst2d(HyInstanceType eInstType, const char *szPrefix, const char *szName) :	m_eTYPE(eInstType),
																							m_sPREFIX(szPrefix ? szPrefix : ""),
																							m_sNAME(szName ? szName : ""),
																							m_pDataPtr(NULL),
																							m_eLoadState(HYLOADSTATE_Inactive),
																							m_pParent(NULL),
																							m_bDirty(true),
																							m_bEnabled(true),
																							m_uiDisplayOrder(0),
																							m_iTag(0)
{
	m_vColor.Set(1.0f);
	SetOnDirtyCallback(OnDirty, this);
}

/*virtual*/ IHyInst2d::~IHyInst2d(void)
{
	Unload();
}

void IHyInst2d::Load()
{
	// TODO: fix this code. Handle default more eloquently
	if(GetCoordinateType() == HYCOORD_Default && HyScene::DefaultCoordinateType() != HYCOORD_Default)
		SetCoordinateType(HyScene::DefaultCoordinateType(), true);

	sm_pFileIO->LoadInst2d(this);
}

void IHyInst2d::Unload()
{
	sm_pFileIO->RemoveInst(this);
}

void IHyInst2d::GetWorldTransform(mat4 &outMtx)
{
	if(m_bDirty)
	{
		if(m_pParent)
			m_pParent->GetWorldTransform(m_mtxCached);

		mat4 mtxLocal;
		GetLocalTransform(mtxLocal);

		m_mtxCached *= mtxLocal;
		m_bDirty = false;
	}

	outMtx = m_mtxCached;
}

void IHyInst2d::AddChild(IHyInst2d &childInst)
{
	childInst.Detach();

	childInst.m_pParent = this;
	m_vChildList.push_back(&childInst);
}

void IHyInst2d::Detach()
{
	if(m_pParent == NULL)
		return;

	for(vector<IHyInst2d *>::iterator iter = m_pParent->m_vChildList.begin(); iter != m_pParent->m_vChildList.end(); ++iter)
	{
		if(*iter == this)
		{
			m_pParent->m_vChildList.erase(iter);
			return;
		}
	}

	HyError("IObjInst2d::Detach() could not find itself in parent's child list");
}

void IHyInst2d::SetDisplayOrder(uint32 uiOrderValue)
{
	m_uiDisplayOrder = uiOrderValue;
	HyScene::SetInstOrderingDirty();
}

void IHyInst2d::SetData(IHyData *pData)
{
	m_pDataPtr = pData;
	
	if(m_pDataPtr == NULL)
		m_eLoadState = HYLOADSTATE_Loaded;
	else
		m_eLoadState = (m_pDataPtr->GetLoadState() == HYLOADSTATE_Loaded) ? HYLOADSTATE_Loaded : HYLOADSTATE_Queued;
}

void IHyInst2d::SetLoaded()
{
	m_eLoadState = HYLOADSTATE_Loaded;
	OnDataLoaded();
}

void IHyInst2d::SetDirty()
{
	m_bDirty = true;

	for(uint32 i = 0; i < m_vChildList.size(); ++i)
		m_vChildList[i]->SetDirty();
}

/*static*/ void IHyInst2d::OnDirty(void *pParam)
{
	IHyInst2d *pThis = reinterpret_cast<IHyInst2d *>(pParam);
	pThis->SetDirty();
}

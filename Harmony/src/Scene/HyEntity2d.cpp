/**************************************************************************
 *	HyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/HyEntity2d.h"

#include "Scene/HyScene.h"
#include "Scene/Instances/HySprite2d.h"
#include "Scene/Instances/HySpine2d.h"
#include "Scene/Instances/HyText2d.h"
#include "Scene/Instances/HyPrimitive2d.h"

/*static*/ HyScene *HyEntity2d::sm_pCtor = NULL;

// Hidden ctor used within AddChild()
HyEntity2d::HyEntity2d(HyEntity2d *pParent) : m_kpParent(pParent)
{
	CtorInit();
}

// Public ctor
HyEntity2d::HyEntity2d(void) : m_kpParent(NULL)
{
	CtorInit();
}

HyEntity2d::~HyEntity2d(void)
{
	Clear(false);

	//for(uint32 i = 0; i < m_vChildNodes.size(); ++i)
	//	m_vChildNodes[i]->m_kpParent = NULL;
}

void HyEntity2d::CtorInit()
{
	m_bDirty = true;
	SetOnDirtyCallback(OnDirty, this);

}

void HyEntity2d::OnUpdate()
{
	
}

IHyInst2d *HyEntity2d::Set(HyInstanceType eType, const char *szPrefix, const char *szName)
{
	HyAssert(sm_pCtor, "HyEntity2d::Set() cannot be used before IGame::Initialize() is invoked.");

	IHyInst2d *pNewInst = NULL;
	switch(eType)
	{
	case HYINST_Sprite2d:	pNewInst = HY_NEW HySprite2d(szPrefix, szName);	break;
	case HYINST_Spine2d:	pNewInst = HY_NEW HySpine2d(szPrefix, szName);		break;
	case HYINST_Text2d:		pNewInst = HY_NEW HyText2d(szPrefix, szName);		break;
	default:
		break;
	}

	if(pNewInst != NULL)
	{
		LinkInst(pNewInst);
	}

	return pNewInst;
}

HySprite2d *HyEntity2d::SetSprite(const char *szPrefix, const char *szName)
{
	HySprite2d *pNewInst = HY_NEW HySprite2d(szPrefix, szName);
	LinkInst(pNewInst);

	return pNewInst;
}

HySpine2d *HyEntity2d::SetSpine(const char *szPrefix, const char *szName)
{
	HySpine2d *pNewInst = HY_NEW HySpine2d(szPrefix, szName);
	LinkInst(pNewInst);

	return pNewInst;
}

HyText2d *HyEntity2d::SetText(const char *szPrefix, const char *szName)
{
	HyText2d *pNewInst = HY_NEW HyText2d(szPrefix, szName);
	LinkInst(pNewInst);

	return pNewInst;
}

HyPrimitive2d *HyEntity2d::SetPrimitive()
{
	HyPrimitive2d *pNewInst = HY_NEW HyPrimitive2d();
	LinkInst(pNewInst);

	return pNewInst;
}

HySprite2d *HyEntity2d::GetSprite()
{
	for(uint32 i = 0; i < m_vInstList.size(); ++i)
	{
		if(m_vInstList[i]->GetInstType() == HYINST_Sprite2d)
			return reinterpret_cast<HySprite2d *>(m_vInstList[i]);
	}
	return NULL;
}
HySpine2d *HyEntity2d::GetSpine()
{
	for(uint32 i = 0; i < m_vInstList.size(); ++i)
	{
		if(m_vInstList[i]->GetInstType() == HYINST_Spine2d)
			return reinterpret_cast<HySpine2d *>(m_vInstList[i]);
	}
	return NULL;
}
HyText2d *HyEntity2d::GetText()
{
	for(uint32 i = 0; i < m_vInstList.size(); ++i)
	{
		if(m_vInstList[i]->GetInstType() == HYINST_Text2d)
			return reinterpret_cast<HyText2d *>(m_vInstList[i]);
	}
	return NULL;
}
HyPrimitive2d *HyEntity2d::GetPrimitive()
{
	for(uint32 i = 0; i < m_vInstList.size(); ++i)
	{
		if(m_vInstList[i]->GetInstType() == HYINST_Primitive2d)
			return reinterpret_cast<HyPrimitive2d *>(m_vInstList[i]);
	}
	return NULL;
}

void HyEntity2d::LinkInst(IHyInst2d *pInst)
{
	//// TODO: fix this code. Handle default more eloquently
	//if(GetCoordinateType() == HYCOORD_Default && HyScene::DefaultCoordinateType() != HYCOORD_Default)
	//	SetCoordinateType(HyScene::DefaultCoordinateType(), true);

	//// Call LoadInst2d() before SetParent() to prevent accidentially deleting the instance's IData if
	//// it's the only associated instance of the data.
	//sm_pCtor->LoadInst2d(pInst);
	//pInst->SetParent(this);

	//m_vInstList.push_back(pInst);
}

bool HyEntity2d::Remove(IHyInst2d *pInst)
{
	HyAssert(sm_pCtor, "HyEntity2d::Remove() cannot be used before IGame::Initialize() is invoked.");
	
	for (vector<IHyInst2d *>::iterator iter = m_vInstList.begin(); iter != m_vInstList.end(); ++iter)
	{
		if((*iter) == pInst)
		{
			Erase(iter);
			return true;
		}
	}

	return false;
}

void HyEntity2d::Clear(bool bClearChildren)
{
	while(m_vInstList.empty() == false)
		Erase(--m_vInstList.end());

	if(bClearChildren)
	{
		for(uint32 i = 0; i < m_vChildNodes.size(); ++i)
			m_vChildNodes[i]->Clear(true);
	}
}

HyEntity2d *HyEntity2d::AddChild()
{
	HyEntity2d *pEnt2d = HY_NEW HyEntity2d(this);
	m_vChildNodes.push_back(pEnt2d);

	pEnt2d->SetDirty();

	return pEnt2d;
}

void HyEntity2d::RemoveChild(HyEntity2d *pEnt2d)
{
	for(vector<HyEntity2d *>::iterator iter = m_vChildNodes.begin(); iter != m_vChildNodes.end(); ++iter)
	{
		if((*iter) == pEnt2d)
		{
			m_vChildNodes.erase(iter);
			break;
		}
	}
}

void HyEntity2d::Erase(vector<IHyInst2d *>::iterator &iterRef)
{
	//sm_pCtor->RemoveInst((*iterRef));
	//(*iterRef)->RemoveParent();

	//m_vInstList.erase(iterRef);
}

void HyEntity2d::SetDirty()
{
	m_bDirty = true;

	for(uint32 i = 0; i < m_vChildNodes.size(); ++i)
		m_vChildNodes[i]->SetDirty();
}

void HyEntity2d::SetInstOrderingDirty()
{
	HyAssert(sm_pCtor, "HyEntity2d::SetInstOrderingDirty() cannot be used before IGame::Initialize() is invoked.");
	sm_pCtor->SetInstOrderingDirty();
}


/*static*/ void HyEntity2d::OnDirty(void *pParam)
{
	HyEntity2d *pThis = reinterpret_cast<HyEntity2d *>(pParam);
	pThis->SetDirty();
}

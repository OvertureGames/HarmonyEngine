/**************************************************************************
*	IHyBody2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h"
#include "Scene/HyScene.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"

IHyBody2d::IHyBody2d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity2d *pParent) :
	IHyLoadable2d(eNodeType, sPrefix, sName, pParent),
	IHyBody(),
	m_fAlpha(1.0f),
	m_fCachedAlpha(1.0f),
	m_iDisplayOrder(0),
	topColor(*this, DIRTY_Color),
	botColor(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsDrawable;

	topColor.Set(1.0f);
	botColor.Set(1.0f);

	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();

	if(m_pParent)
	{
		m_iCoordinateSystem = m_pParent->GetCoordinateSystem();

		if(m_pParent->IsScissorSet())
		{
			if(m_pScissor == nullptr)
				m_pScissor = HY_NEW ScissorRect();

			m_pScissor->m_WorldScissorRect = m_pParent->m_pScissor->m_WorldScissorRect;
		}

		if(m_pParent->IsStencilSet())
			m_hStencil = m_pParent->m_hStencil;

		//int32 iOrderValue = m_pParent->GetDisplayOrder() + 1;

		//if(m_pParent->IsReverseDisplayOrder() == false)
		//{
		//	for(uint32 i = 0; i < m_pParent->ChildCount(); ++i)
		//	{
		//		if(0 != (m_pParent->ChildGet(i)->m_uiFlags & NODETYPE_IsDrawable))
		//			iOrderValue = static_cast<IHyBody2d *>(m_ChildList[i])->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
		//	}
		//}
		//else
		//{
		//	for(int32 i = static_cast<int32>(m_ChildList.size()) - 1; i >= 0; --i)
		//	{
		//		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsDrawable))
		//			iOrderValue = static_cast<IHyBody2d *>(m_ChildList[i])->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
		//	}
		//}

		//m_iDisplayOrder = iOrderValue;
		m_pParent->SetChildrenDisplayOrder(false);
	}
}

IHyBody2d::IHyBody2d(const IHyBody2d &copyRef) :
	IHyLoadable2d(copyRef),
	IHyBody(copyRef),
	m_iDisplayOrder(copyRef.m_iDisplayOrder),
	topColor(*this, DIRTY_Color),
	botColor(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsDrawable;

	topColor = copyRef.topColor;
	botColor = copyRef.botColor;
	alpha = copyRef.alpha;

	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();
}

IHyBody2d::IHyBody2d(IHyBody2d &&donor) noexcept :
	IHyLoadable2d(std::move(donor)),
	IHyBody(std::move(donor)),
	m_iDisplayOrder(std::move(donor.m_iDisplayOrder)),
	topColor(*this, DIRTY_Color),
	botColor(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiFlags |= NODETYPE_IsDrawable;

	topColor = std::move(donor.topColor);
	botColor = std::move(donor.botColor);
	alpha = std::move(donor.alpha);

	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();
}

IHyBody2d::~IHyBody2d()
{
}

IHyBody2d &IHyBody2d::operator=(const IHyBody2d &rhs)
{
	IHyLoadable2d::operator=(rhs);
	IHyBody::operator=(rhs);

	m_iDisplayOrder = rhs.m_iDisplayOrder;

	topColor = rhs.topColor;
	botColor = rhs.botColor;
	alpha = rhs.alpha;

	CalculateColor();

	return *this;
}

IHyBody2d &IHyBody2d::operator=(IHyBody2d &&donor) noexcept
{
	IHyLoadable2d::operator=(std::move(donor));
	IHyBody::operator=(std::move(donor));

	m_iDisplayOrder = std::move(donor.m_iDisplayOrder);

	topColor = std::move(donor.topColor);
	botColor = std::move(donor.botColor);
	alpha = std::move(donor.alpha);

	CalculateColor();

	return *this;
}

void IHyBody2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void IHyBody2d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float IHyBody2d::CalculateAlpha()
{
	CalculateColor();
	return m_fCachedAlpha;
}

const glm::vec3 &IHyBody2d::CalculateTopTint()
{
	CalculateColor();
	return m_CachedTopColor;
}

const glm::vec3 &IHyBody2d::CalculateBotTint()
{
	CalculateColor();
	return m_CachedBotColor;
}

int32 IHyBody2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

/*virtual*/ void IHyBody2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	m_uiFlags |= EXPLICIT_DisplayOrder;

	HyScene::SetInstOrderingDirty();
}

/*virtual*/ void IHyBody2d::ResetDisplayOrder()
{
	m_uiFlags &= ~EXPLICIT_DisplayOrder;

	HyEntity2d *pRootParent = m_pParent;
	while(pRootParent)
	{
		if(pRootParent->m_pParent)
			pRootParent = pRootParent->m_pParent;
		else
			break;
	}

	if(pRootParent)
	{
		pRootParent->SetChildrenDisplayOrder(false);
		HyScene::SetInstOrderingDirty();
	}
}

/*virtual*/ void IHyBody2d::Update() /*override*/
{
	IHyLoadable2d::Update();
}

/*virtual*/ int32 IHyBody2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding)
{
	if(bIsOverriding)
		m_uiFlags &= ~EXPLICIT_DisplayOrder;

	if(0 == (m_uiFlags & EXPLICIT_DisplayOrder))
	{
		m_iDisplayOrder = iOrderValue;
		iOrderValue += 1;

		HyScene::SetInstOrderingDirty();
	}

	return iOrderValue;
}

void IHyBody2d::CalculateColor()
{
	if(IsDirty(DIRTY_Color))
	{
		m_fCachedAlpha = alpha.Get();
		m_CachedTopColor = topColor.Get();
		m_CachedBotColor = botColor.Get();

		if(m_pParent)
		{
			m_fCachedAlpha *= m_pParent->CalculateAlpha();
			m_CachedTopColor *= m_pParent->CalculateTopTint();
			m_CachedBotColor *= m_pParent->CalculateTopTint();
		}

		ClearDirty(DIRTY_Color);
	}
}

/*virtual*/ IHyNode &IHyBody2d::_VisableGetNodeRef() /*override final*/
{
	return *this;
}

/*virtual*/ HyEntity2d *IHyBody2d::_VisableGetParent2dPtr() /*override final*/
{
	return m_pParent;
}

/*virtual*/ HyEntity3d *IHyBody2d::_VisableGetParent3dPtr() /*override final*/
{
	return nullptr;
}

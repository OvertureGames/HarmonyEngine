/**************************************************************************
*	IHyDrawable2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Drawables/IHyDrawable2d.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity3d.h"
#include "Scene/HyScene.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"

IHyDrawable2d::IHyDrawable2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent) :
	IHyLoadable2d(eNodeType, szPrefix, szName, pParent),
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
		SetupNewChild(*m_pParent, *this);
}

IHyDrawable2d::IHyDrawable2d(const IHyDrawable2d &copyRef) :
	IHyLoadable2d(copyRef),
	IHyDrawable(copyRef),
	m_fAlpha(copyRef.m_fAlpha),
	m_iDisplayOrder(copyRef.m_iDisplayOrder),
	topColor(*this, DIRTY_Color),
	botColor(*this, DIRTY_Color),
	alpha(m_fAlpha, *this, DIRTY_Color)
{
	topColor.Set(copyRef.topColor.Get());
	botColor.Set(copyRef.botColor.Get());
	alpha.Set(copyRef.alpha.Get());

	CalculateColor();
}

IHyDrawable2d::~IHyDrawable2d()
{
}

const IHyDrawable2d &IHyDrawable2d::operator=(const IHyDrawable2d &rhs)
{
	IHyLoadable2d::operator=(rhs);
	IHyDrawable::operator=(rhs);

	m_fAlpha = rhs.m_fAlpha;
	m_iDisplayOrder = rhs.m_iDisplayOrder;

	topColor.Set(rhs.topColor.Get());
	botColor.Set(rhs.botColor.Get());
	alpha.Set(rhs.alpha.Get());

	CalculateColor();

	return *this;
}

void IHyDrawable2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void IHyDrawable2d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float IHyDrawable2d::CalculateAlpha()
{
	CalculateColor();
	return m_fCachedAlpha;
}

const glm::vec3 &IHyDrawable2d::CalculateTopTint()
{
	CalculateColor();
	return m_CachedTopColor;
}

const glm::vec3 &IHyDrawable2d::CalculateBotTint()
{
	CalculateColor();
	return m_CachedBotColor;
}

int32 IHyDrawable2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

/*virtual*/ void IHyDrawable2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	m_uiFlags |= EXPLICIT_DisplayOrder;

	HyScene::SetInstOrderingDirty();
}

/*virtual*/ void IHyDrawable2d::ResetDisplayOrder()
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

/*virtual*/ void IHyDrawable2d::Update() /*override*/
{
	IHyLoadable2d::Update();
}

/*virtual*/ int32 IHyDrawable2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding)
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

void IHyDrawable2d::CalculateColor()
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

/*virtual*/ IHyNode &IHyDrawable2d::_VisableGetNodeRef() /*override final*/
{
	return *this;
}

/*virtual*/ HyEntity2d *IHyDrawable2d::_VisableGetParent2dPtr() /*override final*/
{
	return m_pParent;
}

/*virtual*/ HyEntity3d *IHyDrawable2d::_VisableGetParent3dPtr() /*override final*/
{
	return nullptr;
}

/*friend*/ void SetupNewChild(HyEntity2d &parentRef, IHyDrawable2d &childRef)
{
	childRef._SetCoordinateSystem(parentRef.GetCoordinateSystem(), false);

	if(parentRef.IsScissorSet())
		static_cast<IHyDrawable2d &>(childRef)._SetScissor(parentRef.m_pScissor, false);

	if(parentRef.IsStencilSet())
		static_cast<IHyDrawable2d &>(childRef)._SetStencil(parentRef.m_hStencil, false);

	parentRef.SetChildrenDisplayOrder(false);
}

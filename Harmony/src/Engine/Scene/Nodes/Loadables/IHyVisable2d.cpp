/**************************************************************************
*	IHyVisable2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/IHyVisable2d.h"
#include "Scene/Nodes/Loadables/Entities/HyEntity2d.h"
#include "Scene/HyScene.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"

IHyVisable2d::IHyVisable2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent) :	IHyLoadable2d(eNodeType, szPrefix, szName, pParent),
																												m_fAlpha(1.0f),
																												m_fCachedAlpha(1.0f),
																												m_iDisplayOrder(0),
																												topColor(*this, DIRTY_Color),
																												botColor(*this, DIRTY_Color),
																												alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiExplicitAndTypeFlags |= NODETYPE_IsVisable;

	topColor.Set(1.0f);
	botColor.Set(1.0f);
	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();
}

IHyVisable2d::IHyVisable2d(const IHyVisable2d &copyRef) :	IHyLoadable2d(copyRef),
															IHyVisable(copyRef),
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

IHyVisable2d::~IHyVisable2d()
{
}

const IHyVisable2d &IHyVisable2d::operator=(const IHyVisable2d &rhs)
{
	IHyLoadable2d::operator=(rhs);
	IHyVisable::operator=(rhs);

	m_fAlpha = rhs.m_fAlpha;
	m_iDisplayOrder = rhs.m_iDisplayOrder;

	topColor.Set(rhs.topColor.Get());
	botColor.Set(rhs.botColor.Get());
	alpha.Set(rhs.alpha.Get());

	CalculateColor();

	return *this;
}

void IHyVisable2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void IHyVisable2d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float IHyVisable2d::CalculateAlpha()
{
	CalculateColor();
	return m_fCachedAlpha;
}

const glm::vec3 &IHyVisable2d::CalculateTopTint()
{
	CalculateColor();
	return m_CachedTopColor;
}

const glm::vec3 &IHyVisable2d::CalculateBotTint()
{
	CalculateColor();
	return m_CachedBotColor;
}

int32 IHyVisable2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

/*virtual*/ void IHyVisable2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	m_uiExplicitAndTypeFlags |= EXPLICIT_DisplayOrder;

	HyScene::SetInstOrderingDirty();
}

/*virtual*/ int32 IHyVisable2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitAndTypeFlags &= ~EXPLICIT_DisplayOrder;

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_DisplayOrder))
	{
		m_iDisplayOrder = iOrderValue;
		iOrderValue += 1;

		HyScene::SetInstOrderingDirty();
	}

	return iOrderValue;
}

void IHyVisable2d::CalculateColor()
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

/*virtual*/ IHyNode &IHyVisable2d::_VisableGetNodeRef() /*override final*/
{
	return *this;
}

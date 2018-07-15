/**************************************************************************
*	IHyLoadable2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"
#include "Scene/Nodes/Loadables/Entities/HyEntity2d.h"
#include "Scene/HyScene.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"

IHyLoadable2d::IHyLoadable2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent) :	IHyNode2d(eNodeType, pParent),
																												IHyLoadable(szPrefix, szName),
																												m_fAlpha(1.0f),
																												m_fCachedAlpha(1.0f),
																												m_pScissor(nullptr),
																												m_hStencil(HY_UNUSED_HANDLE),
																												m_iCoordinateSystem(-1),
																												m_iDisplayOrder(0),
																												topColor(*this, DIRTY_Color),
																												botColor(*this, DIRTY_Color),
																												alpha(m_fAlpha, *this, DIRTY_Color)
{
	m_uiExplicitAndTypeFlags |= NODETYPE_IsLoadable;

	topColor.Set(1.0f);
	botColor.Set(1.0f);
	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();
}

IHyLoadable2d::IHyLoadable2d(const IHyLoadable2d &copyRef) :	IHyNode2d(copyRef),
																IHyLoadable(copyRef),
																m_fAlpha(copyRef.m_fAlpha),
																m_pScissor(nullptr),
																m_hStencil(copyRef.m_hStencil),
																m_iCoordinateSystem(copyRef.m_iCoordinateSystem),
																m_iDisplayOrder(copyRef.m_iDisplayOrder),
																topColor(*this, DIRTY_Color),
																botColor(*this, DIRTY_Color),
																alpha(m_fAlpha, *this, DIRTY_Color)
{
	if(copyRef.m_pScissor)
	{
		m_pScissor = HY_NEW ScissorRect();
		m_pScissor->m_LocalScissorRect = copyRef.m_pScissor->m_LocalScissorRect;
		GetWorldScissor(m_pScissor->m_WorldScissorRect);
	}

	topColor.Set(copyRef.topColor.Get());
	botColor.Set(copyRef.botColor.Get());
	alpha.Set(copyRef.alpha.Get());

	CalculateColor();
}

IHyLoadable2d::~IHyLoadable2d()
{
	delete m_pScissor;
}

const IHyLoadable2d &IHyLoadable2d::operator=(const IHyLoadable2d &rhs)
{
	IHyNode2d::operator=(rhs);
	IHyLoadable::operator=(rhs);

	m_fAlpha = rhs.m_fAlpha;

	delete m_pScissor;
	m_pScissor = nullptr;
	if(rhs.m_pScissor)
	{
		m_pScissor = HY_NEW ScissorRect();
		m_pScissor->m_LocalScissorRect = rhs.m_pScissor->m_LocalScissorRect;
		GetWorldScissor(m_pScissor->m_WorldScissorRect);
	}

	m_hStencil = rhs.m_hStencil;
	m_iCoordinateSystem = rhs.m_iCoordinateSystem;
	m_iDisplayOrder = rhs.m_iDisplayOrder;

	topColor.Set(rhs.topColor.Get());
	botColor.Set(rhs.botColor.Get());
	alpha.Set(rhs.alpha.Get());

	CalculateColor();

	return *this;
}

void IHyLoadable2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void IHyLoadable2d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float IHyLoadable2d::CalculateAlpha()
{
	CalculateColor();
	return m_fCachedAlpha;
}

const glm::vec3 &IHyLoadable2d::CalculateTopTint()
{
	CalculateColor();
	return m_CachedTopColor;
}

const glm::vec3 &IHyLoadable2d::CalculateBotTint()
{
	CalculateColor();
	return m_CachedBotColor;
}

bool IHyLoadable2d::IsScissorSet() const
{
	return m_pScissor != nullptr;
}

void IHyLoadable2d::GetLocalScissor(HyScreenRect<int32> &scissorOut) const
{
	if(m_pScissor == nullptr)
		return;

	scissorOut = m_pScissor->m_LocalScissorRect;
}

void IHyLoadable2d::GetWorldScissor(HyScreenRect<int32> &scissorOut)
{
	if(m_pScissor == nullptr)
		return;

	if(IsDirty(DIRTY_Scissor))
	{
		if((m_uiExplicitAndTypeFlags & EXPLICIT_Scissor) == 0 && m_pParent)
			m_pParent->GetWorldScissor(m_pScissor->m_WorldScissorRect);
		else
		{
			if(m_pScissor->m_LocalScissorRect.iTag == SCISSORTAG_Enabled)
			{
				glm::mat4 mtx;
				GetWorldTransform(mtx);

				m_pScissor->m_WorldScissorRect.x = static_cast<int32>(mtx[3].x + m_pScissor->m_LocalScissorRect.x);
				m_pScissor->m_WorldScissorRect.y = static_cast<int32>(mtx[3].y + m_pScissor->m_LocalScissorRect.y);
				m_pScissor->m_WorldScissorRect.width = static_cast<uint32>(mtx[0].x * m_pScissor->m_LocalScissorRect.width);
				m_pScissor->m_WorldScissorRect.height = static_cast<uint32>(mtx[1].y * m_pScissor->m_LocalScissorRect.height);
				m_pScissor->m_WorldScissorRect.iTag = SCISSORTAG_Enabled;
			}
			else
			{
				m_pScissor->m_WorldScissorRect.iTag = SCISSORTAG_Disabled;
			}
		}

		ClearDirty(DIRTY_Scissor);
	}

	scissorOut = m_pScissor->m_WorldScissorRect;
}

/*virtual*/ void IHyLoadable2d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight)
{
	if(m_pScissor == nullptr)
		m_pScissor = HY_NEW ScissorRect();

	m_pScissor->m_LocalScissorRect.x = uiLocalX;
	m_pScissor->m_LocalScissorRect.y = uiLocalY;
	m_pScissor->m_LocalScissorRect.width = uiWidth;
	m_pScissor->m_LocalScissorRect.height = uiHeight;
	m_pScissor->m_LocalScissorRect.iTag = SCISSORTAG_Enabled;

	m_uiExplicitAndTypeFlags |= EXPLICIT_Scissor;

	GetWorldScissor(m_pScissor->m_WorldScissorRect);
}

/*virtual*/ void IHyLoadable2d::ClearScissor(bool bUseParentScissor)
{
	if(m_pScissor == nullptr)
		return;

	m_pScissor->m_LocalScissorRect.iTag = SCISSORTAG_Disabled;
	m_pScissor->m_WorldScissorRect.iTag = SCISSORTAG_Disabled;

	if(bUseParentScissor == false)
		m_uiExplicitAndTypeFlags |= EXPLICIT_Scissor;
	else
	{
		m_uiExplicitAndTypeFlags &= ~EXPLICIT_Scissor;
		if(m_pParent)
			m_pParent->GetWorldScissor(m_pScissor->m_WorldScissorRect);
	}
}

bool IHyLoadable2d::IsStencilSet() const
{
	return m_hStencil != HY_UNUSED_HANDLE;
}

HyStencil *IHyLoadable2d::GetStencil() const
{
	return IHyRenderer::FindStencil(m_hStencil);
}

/*virtual*/ void IHyLoadable2d::SetStencil(HyStencil *pStencil)
{
	if(pStencil == nullptr)
		m_hStencil = HY_UNUSED_HANDLE;
	else
		m_hStencil = pStencil->GetHandle();

	m_uiExplicitAndTypeFlags |= EXPLICIT_Stencil;
}

/*virtual*/ void IHyLoadable2d::ClearStencil(bool bUseParentStencil)
{
	m_hStencil = HY_UNUSED_HANDLE;

	if(bUseParentStencil == false)
		m_uiExplicitAndTypeFlags |= EXPLICIT_Stencil;
	else
	{
		m_uiExplicitAndTypeFlags &= ~EXPLICIT_Stencil;
		if(m_pParent)
		{
			HyStencil *pStencil = m_pParent->GetStencil();
			m_hStencil = pStencil ? pStencil->GetHandle() : HY_UNUSED_HANDLE;
		}
	}
}

int32 IHyLoadable2d::GetCoordinateSystem() const
{
	return m_iCoordinateSystem;
}

/*virtual*/ void IHyLoadable2d::UseCameraCoordinates()
{
	m_iCoordinateSystem = -1;
	m_uiExplicitAndTypeFlags |= EXPLICIT_CoordinateSystem;
}

/*virtual*/ void IHyLoadable2d::UseWindowCoordinates(int32 iWindowIndex /*= 0*/)
{
	m_iCoordinateSystem = iWindowIndex;
	m_uiExplicitAndTypeFlags |= EXPLICIT_CoordinateSystem;
}

int32 IHyLoadable2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

/*virtual*/ void IHyLoadable2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	m_uiExplicitAndTypeFlags |= EXPLICIT_DisplayOrder;

	HyScene::SetInstOrderingDirty();
}

/*virtual*/ void IHyLoadable2d::_SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitAndTypeFlags &= ~EXPLICIT_Scissor;

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_Scissor))
	{
		if(m_pScissor == nullptr)
			m_pScissor = HY_NEW ScissorRect();

		m_pScissor->m_WorldScissorRect = worldScissorRectRef;
	}
}

/*virtual*/ void IHyLoadable2d::_SetStencil(HyStencilHandle hHandle, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitAndTypeFlags &= ~EXPLICIT_Stencil;

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_Stencil))
		m_hStencil = hHandle;
}

/*virtual*/ void IHyLoadable2d::_SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitAndTypeFlags &= ~EXPLICIT_CoordinateSystem;

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_CoordinateSystem))
		m_iCoordinateSystem = iWindowIndex;
}

/*virtual*/ int32 IHyLoadable2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) /*override*/
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

void IHyLoadable2d::CalculateColor()
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

/*virtual*/ HyType IHyLoadable2d::_LoadableGetType() /*override*/
{
	return m_eTYPE;
}

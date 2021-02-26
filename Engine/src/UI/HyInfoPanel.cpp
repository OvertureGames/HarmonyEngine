/**************************************************************************
*	HyInfoPanel.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyInfoPanel.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Diagnostics/Console/IHyConsole.h"

HyInfoPanel::ProcPanel::ProcPanel(float fWidth, float fHeight, float fStroke, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_Fill(this),
	m_Stroke(this)
{
	m_Fill.SetAsBox(fWidth, fHeight);
	m_Fill.SetTint(0.5f, 0.5f, 0.5f);
	
	m_Stroke.SetAsBox(fWidth, fHeight);
	m_Stroke.SetTint(0.3f, 0.3f, 0.3f);
	m_Stroke.SetWireframe(true);
	m_Stroke.SetLineThickness(fStroke);

	Load();
}

HyInfoPanel::HyInfoPanel(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_bIsDisabled(false),
	m_pProcPanel(nullptr),
	m_Panel("", "", this),
	m_Text("", "", this)
{
}

HyInfoPanel::HyInfoPanel(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_bIsDisabled(false),
	m_pProcPanel(nullptr),
	m_Panel("", "", this),
	m_Text("", "", this)
{
	Setup(fWidth, fHeight, fStroke, sTextPrefix, sTextName);
}

HyInfoPanel::HyInfoPanel(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_bIsDisabled(false),
	m_pProcPanel(nullptr),
	m_Panel("", "", this),
	m_Text("", "", this)
{
	Setup(fWidth, fHeight, fStroke, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

HyInfoPanel::HyInfoPanel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_bIsDisabled(false),
	m_pProcPanel(nullptr),
	m_Panel("", "", this),
	m_Text("", "", this)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName);
}

HyInfoPanel::HyInfoPanel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_bIsDisabled(false),
	m_pProcPanel(nullptr),
	m_Panel("", "", this),
	m_Text("", "", this)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

/*virtual*/ HyInfoPanel::~HyInfoPanel()
{
	delete m_pProcPanel;
}

void HyInfoPanel::Setup(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName)
{
	m_Panel.Uninit();
	delete m_pProcPanel;
	m_pProcPanel = HY_NEW ProcPanel(fWidth, fHeight, fStroke, this);
	m_Text.Init(sTextPrefix, sTextName, this);

	DoSetup(0, 0, 0, 0);
}

void HyInfoPanel::Setup(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	m_Panel.Uninit();
	delete m_pProcPanel;
	m_pProcPanel = HY_NEW ProcPanel(fWidth, fHeight, fStroke, this);
	m_Text.Init(sTextPrefix, sTextName, this);

	DoSetup(iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

void HyInfoPanel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName)
{
	delete m_pProcPanel;
	m_pProcPanel = nullptr;
	m_Panel.Init(sPanelPrefix, sPanelName, this);
	m_Text.Init(sTextPrefix, sTextName, this);

	DoSetup(0, 0, 0, 0);
}

void HyInfoPanel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	delete m_pProcPanel;
	m_pProcPanel = nullptr;
	m_Panel.Init(sPanelPrefix, sPanelName, this);
	m_Text.Init(sTextPrefix, sTextName, this);

	DoSetup(iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

float HyInfoPanel::GetPanelWidth()
{
	if(m_pProcPanel)
	{
		b2AABB tmpAABB;
		b2Transform tmpTransform;
		tmpTransform.SetIdentity();
		m_pProcPanel->m_Fill.GetLocalBoundingVolume().GetB2Shape()->ComputeAABB(&tmpAABB, tmpTransform, 0);
		return tmpAABB.upperBound.x - tmpAABB.lowerBound.x;
	}
	else
		return m_Panel.GetCurFrameWidth(true);
}

float HyInfoPanel::GetPanelHeight()
{
	if(m_pProcPanel)
	{
		b2AABB tmpAABB;
		b2Transform tmpTransform;
		tmpTransform.SetIdentity();
		m_pProcPanel->m_Fill.GetLocalBoundingVolume().GetB2Shape()->ComputeAABB(&tmpAABB, tmpTransform, 0);
		return tmpAABB.upperBound.y - tmpAABB.lowerBound.y;
	}
	else
		return m_Panel.GetCurFrameHeight(true);
}

/*virtual*/ void HyInfoPanel::SetSpriteState(uint32 uiStateIndex)
{
	if(m_Panel.IsLoadDataValid() == false)
	{
		HyLogWarning("HyInfoPanel::SetSpriteState was invoked with an invalid panel sprite");
		return;
	}

	// Calculate the original text offset specified from an earlier call to Setup()
	glm::ivec2 vOrigTextOffset = m_Text.pos.Get();
	glm::ivec2 vPanelOffset(0);
	const HySprite2dData *pPanelData = static_cast<const HySprite2dData *>(m_Panel.AcquireData());
	if(pPanelData)
	{
		const HySprite2dFrame &frameRef = pPanelData->GetFrame(m_Panel.GetState(), m_Panel.GetFrame());
		vPanelOffset = frameRef.vOFFSET;
	}
	vOrigTextOffset.x -= vPanelOffset.x;
	vOrigTextOffset.y -= vPanelOffset.y;

	// Now set new sprite state, so below SetTextLocation() can offset properly onto it
	m_Panel.SetState(uiStateIndex);

	// Realign text on new panel sprite state
	SetTextLocation(static_cast<int32>(m_Text.GetTextBox().x), static_cast<int32>(m_Text.GetTextBox().y), vOrigTextOffset.x, vOrigTextOffset.y);
}

/*virtual*/ std::string HyInfoPanel::GetText() const
{
	return m_Text.GetText();
}

/*virtual*/ void HyInfoPanel::SetText(std::string sText)
{
	m_Text.SetText(sText);
}

/*virtual*/ void HyInfoPanel::SetTextState(uint32 uiStateIndex)
{
	m_Text.SetState(uiStateIndex);
}

/*virtual*/ void HyInfoPanel::SetTextLocation(int32 iWidth, int32 iHeight, int32 iOffsetX, int32 iOffsetY)
{
	glm::ivec2 vPanelOffset(0);
	if(m_pProcPanel == nullptr)
	{
		const HySprite2dData *pPanelData = static_cast<const HySprite2dData *>(m_Panel.AcquireData());
		if(pPanelData)
		{
			const HySprite2dFrame &frameRef = pPanelData->GetFrame(m_Panel.GetState(), m_Panel.GetFrame());
			vPanelOffset = frameRef.vOFFSET;
		}
	}
	
	m_Text.pos.Set(iOffsetX + vPanelOffset.x, iOffsetY + vPanelOffset.y);

	if(iWidth > 0 && iHeight > 0)
		m_Text.SetAsScaleBox(static_cast<float>(iWidth), static_cast<float>(iHeight), true);
	else
		m_Text.SetAsScaleBox(GetPanelWidth(), GetPanelHeight(), true);
}

/*virtual*/ void HyInfoPanel::SetTextAlignment(HyTextAlign eAlignment)
{
	m_Text.SetTextAlignment(eAlignment);
}

/*virtual*/ void HyInfoPanel::SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB)
{
	m_Text.SetLayerColor(uiLayerIndex, fR, fG, fB);
}

bool HyInfoPanel::IsDisabled() const
{
	return m_bIsDisabled;
}

/*virtual*/ void HyInfoPanel::SetAsDisabled(bool bIsDisabled)
{
	m_bIsDisabled = bIsDisabled;
	if(m_bIsDisabled)
		SetTint(0.3f, 0.3f, 0.3f);
	else
		SetTint(1.0f, 1.0f, 1.0f);
}

HySprite2d &HyInfoPanel::GetSpriteNode()
{
	return m_Panel;
}

HyText2d &HyInfoPanel::GetTextNode()
{
	return m_Text;
}

/*virtual*/ void HyInfoPanel::DoSetup(int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY)
{
	m_Text.SetTextAlignment(HYALIGN_Center);
	SetTextLocation(iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
	SetAsDisabled(m_bIsDisabled);
}

#include "SpriteDraw.h"
#include "SpriteWidget.h"

SpriteDraw::SpriteDraw(ProjectItem *pProjItem, IHyApplication &hyApp) : IDraw(pProjItem, hyApp),
                                                                        m_Sprite("", "GuiMadeSprite", this),
                                                                        m_primOriginHorz(this),
                                                                        m_primOriginVert(this)
{
    std::vector<glm::vec2> lineList(2, glm::vec2());

    lineList[0].x = -2048.0f;
    lineList[0].y = 0.0f;
    lineList[1].x = 2048.0f;
    lineList[1].y = 0.0f;
    m_primOriginHorz.SetAsLineChain(lineList);

    lineList[0].x = 0.0f;
    lineList[0].y = -2048.0f;
    lineList[1].x = 0.0f;
    lineList[1].y = 2048.0f;
    m_primOriginVert.SetAsLineChain(lineList);

    m_primOriginHorz.SetTint(1.0f, 0.0f, 0.0f);
    m_primOriginVert.SetTint(1.0f, 0.0f, 0.0f);

//    for(int i = 0; i < m_pProjItem->GetModel()->GetNumStates(); ++i)
//    {
//        SpriteStateData *pStateData = static_cast<SpriteStateData *>(m_pProjItem->GetModel()->GetStateData(i));
//        for(int j = 0; j < pStateData->GetFramesModel()->rowCount(); ++j)
//        {
//            AtlasFrame *pFrame = pStateData->GetFramesModel()->GetFrameAt(j)->m_pFrame;

//            HyTexturedQuad2d *pNewTexturedQuad = new HyTexturedQuad2d(pFrame->GetAtlasGrpId(), pFrame->GetTextureIndex(), this);
//            pNewTexturedQuad->SetTextureSource(pFrame->GetX(), pFrame->GetY(), pFrame->GetCrop().width(), pFrame->GetCrop().height());

//            m_TexturedQuadIdMap.insert(pFrame->GetId(), pNewTexturedQuad);
//        }
//    }
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
//    for(auto iter = m_TexturedQuadIdMap.begin(); iter != m_TexturedQuadIdMap.end(); ++iter)
//        delete iter.value();
}

/*virtual*/ void SpriteDraw::OnApplyJsonData(jsonxx::Value &valueRef, bool bReloadInAssetManager) /*override*/
{
    m_Sprite.GuiOverrideData<HySprite2dData>(valueRef, bReloadInAssetManager);
    
    SpriteWidget *pWidget = static_cast<SpriteWidget *>(m_pProjItem->GetWidget());
    m_Sprite.AnimSetPause(pWidget->IsPlayingAnim() == false);
}

void SpriteDraw::SetFrame(quint32 uiStateIndex, quint32 uiFrameIndex)
{
    m_Sprite.AnimSetState(uiStateIndex);
    m_Sprite.AnimSetFrame(uiFrameIndex);
}

/*virtual*/ void SpriteDraw::OnShow(IHyApplication &hyApp) /*override*/
{
    m_primOriginHorz.SetEnabled(true);
    m_primOriginVert.SetEnabled(true);

    m_Sprite.SetEnabled(true);
    
//    if(m_pCurFrame)
//        m_pCurFrame->SetEnabled(true);
}

/*virtual*/ void SpriteDraw::OnHide(IHyApplication &hyApp) /*override*/
{
    SetEnabled(false);
}

/*virtual*/ void SpriteDraw::OnUpdate() /*override*/
{
    SpriteWidget *pWidget = static_cast<SpriteWidget *>(m_pProjItem->GetWidget());
    
    m_Sprite.AnimSetPause(pWidget->IsPlayingAnim() == false);
    
    if(m_Sprite.AnimIsPaused())
    {
        int iStateIndex, iFrameIndex;
        pWidget->GetSpriteInfo(iStateIndex, iFrameIndex);
        
        m_Sprite.AnimSetState(static_cast<uint32>(iStateIndex));
        m_Sprite.AnimSetFrame(static_cast<uint32>(iFrameIndex));
    }
    else
        pWidget->SetSelectedFrame(m_Sprite.AnimGetFrame());
}

#include "ItemProject.h"
#include "WidgetAtlas.h"

ItemProject::ItemProject() : m_eState(DRAWSTATE_AtlasManager)
{
    m_pAtlases = new WidgetAtlas(this);
}

ItemProject::~ItemProject()
{
    delete m_pAtlases;
}

/*virtual*/ void ItemProject::Hide()
{
    
}

/*virtual*/ void ItemProject::Show()
{
//    switch(m_eState)
//    {
//    case DRAWSTATE_AtlasManager:
//        m_pAtlases->m_Textures
//        m_CurAtlas.SetAsTexturedQuad(
//        break;
//    }
}

/*virtual*/ void ItemProject::Draw(HyApp *pHyApp)
{
    //pHyApp->GetViewport().GetResolution().iWidth
    //pHyApp->GetViewport().GetResolution().iHeight
}
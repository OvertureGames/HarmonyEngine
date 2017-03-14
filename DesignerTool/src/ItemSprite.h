/**************************************************************************
 *	ItemSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMSPRITE_H
#define ITEMSPRITE_H

#include "ItemWidget.h"
#include "WidgetSprite.h"

class WidgetAtlasManager;

class SpriteFrame
{
public:
    HyGuiFrame *            m_pFrame;
    int                     m_iRowIndex;
    
    QPoint                  m_vOffset;
    float                   m_fDuration;
    
    SpriteFrame(HyGuiFrame *pFrame, int iRowIndex) :    m_pFrame(pFrame),
                                                        m_iRowIndex(iRowIndex),
                                                        m_vOffset(0, 0),
                                                        m_fDuration(0.016f)
    { }

    QPoint GetRenderOffset()
    {
        QPoint ptRenderOffset;
        
        ptRenderOffset.setX(m_vOffset.x() + m_pFrame->GetCrop().left());
        ptRenderOffset.setY(m_vOffset.y() + (m_pFrame->GetSize().height() - m_pFrame->GetCrop().bottom()));

        return ptRenderOffset;
    }
};

class ItemSprite : public ItemWidget
{
    Q_OBJECT
    
    HyPrimitive2d               m_primOriginHorz;
    HyPrimitive2d               m_primOriginVert;
    
public:
    ItemSprite(ItemProject *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal);
    virtual ~ItemSprite();
    
protected:
    virtual void OnGiveMenuActions(QMenu *pMenu);

    virtual void OnGuiLoad(IHyApplication &hyApp);
    virtual void OnGuiUnload(IHyApplication &hyApp);
    virtual void OnGuiShow(IHyApplication &hyApp);
    virtual void OnGuiHide(IHyApplication &hyApp);
    virtual void OnGuiUpdate(IHyApplication &hyApp);

    virtual void OnLink(HyGuiFrame *pFrame);
    virtual void OnReLink(HyGuiFrame *pFrame);
    virtual void OnUnlink(HyGuiFrame *pFrame);
    
    virtual QJsonValue OnSave();
};

#endif // ITEMSPRITE_H

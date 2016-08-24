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

struct SpriteFrame
{
    HyGuiFrame *            m_pFrame;
    int                     m_iRowIndex;
    
    QPoint                  m_vOffset;
    float                   m_fDuration;
    
    SpriteFrame(HyGuiFrame *pFrame, int iRowIndex) :    m_pFrame(pFrame),
                                                        m_iRowIndex(iRowIndex),
                                                        m_vOffset(0, 0),
                                                        m_fDuration(0.016f)
    { }
};
//Q_DECLARE_METATYPE(SpriteFrame *)

class ItemSprite : public ItemWidget
{
    Q_OBJECT
    
    HyPrimitive2d               m_primOriginHorz;
    HyPrimitive2d               m_primOriginVert;
    
    enum eCameraStates
    {
    };
    QPointF                     m_ptCameraDestination;
    
public:
    ItemSprite(const QString sPath, WidgetAtlasManager &atlasManRef);
    virtual ~ItemSprite();
    
    virtual QList<QAction *> GetActionsForToolBar();
    
protected:
    virtual void OnDraw_Load(IHyApplication &hyApp);
    virtual void OnDraw_Unload(IHyApplication &hyApp);

    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);

    virtual void OnLink(HyGuiFrame *pFrame);
    virtual void OnUnlink(HyGuiFrame *pFrame);
    
    virtual void OnSave();
};

#endif // ITEMSPRITE_H

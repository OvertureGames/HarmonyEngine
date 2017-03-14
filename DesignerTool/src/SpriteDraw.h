#ifndef SPRITEDRAW_H
#define SPRITEDRAW_H

#include "IDraw.h"
#include "SpriteItem.h"

class SpriteDraw : public IDraw
{
    SpriteItem *                m_pItem;

    HyPrimitive2d               m_primOriginHorz;
    HyPrimitive2d               m_primOriginVert;

public:
    SpriteDraw(SpriteItem *pItem);
    virtual ~SpriteDraw();

protected:
    virtual void OnGuiLoad(IHyApplication &hyApp) override;
    virtual void OnGuiUnload(IHyApplication &hyApp) override;
    virtual void OnGuiShow(IHyApplication &hyApp) override;
    virtual void OnGuiHide(IHyApplication &hyApp) override;
    virtual void OnGuiUpdate(IHyApplication &hyApp) override;
};

#endif // SPRITEDRAW_H

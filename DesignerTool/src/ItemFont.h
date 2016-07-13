/**************************************************************************
 *	ItemFont.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMFONT_H
#define ITEMFONT_H

#include "Item.h"

class ItemFont : public Item
{
    Q_OBJECT
    
    struct tGlyphSet
    {
        QString     sFontName;
        int         iSize;
        QString     sAvailChars;
        
        // Fill colors
        QColor      clrTopLeft;
        QColor      clrTopRight;
        QColor      clrBotLeft;
        QColor      clrBotRight;
        
        struct tDrawInfo
        {
            int     iType;  // 0 - Fill, 1 - Line, 2 - Inner, 3 - Outer
            float   fThickness;
        };
        
        QList<tDrawInfo>    drawOrder;
    };
    
    struct tFontMap
    {
        int                 iAtlasWidth;
        int                 iAtlasHeight;
        
        QList<tGlyphSet>    glyphSets;
    };
    
    
public:
    ItemFont(const QString sPath);
    virtual ~ItemFont();
    virtual void Draw(WidgetRenderer &renderer);
};

#endif // ITEMFONT_H

/**************************************************************************
 *	HyGuiRenderer.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HYGUIRENDERER_H
#define HYGUIRENDERER_H

#include "GL/glew.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "Harmony/HyEngine.h"

class ItemProject;

class HyGuiRenderer : public QOpenGLWidget//, protected QOpenGLFunctions
{
    Q_OBJECT

    ItemProject *       m_pProjOwner;
    HyEngine *          m_pHyEngine;
    
    bool                m_bIsUpdating;

public:
    HyGuiRenderer(QWidget *parent = 0);
    HyGuiRenderer(ItemProject *pProj, QWidget *parent = 0);
    ~HyGuiRenderer();
    
    HyRendererInterop *GetHarmonyRenderer();

    // QOpenGLWidget overrides
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
};

#endif // HYGUIRENDERER_H

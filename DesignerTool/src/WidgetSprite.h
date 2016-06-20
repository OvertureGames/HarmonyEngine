/**************************************************************************
 *	WidgetSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITE_H
#define WIDGETSPRITE_H

#include "WidgetSpriteState.h"

#include <QWidget>
#include <QMenu>
#include <QUndoGroup>
#include <QUndoStack>
#include <QComboBox>

namespace Ui {
class WidgetSprite;
}

class ItemSprite;

class WidgetSprite : public QWidget
{
    Q_OBJECT
    
    ItemSprite *            m_pItemSprite;
    QUndoStack *            m_pUndoStack;

    QList<QAction *>        m_StateActionsList;
    WidgetSpriteState *     m_pCurSpriteState;

public:
    explicit WidgetSprite(QAction *pImportFramesAction, QAction *pRelinquishFramesAction, ItemSprite *pItemSprite, QWidget *parent = 0);
    ~WidgetSprite();

    void UpdateActions();
    
private slots:
    void on_actionAddState_triggered();
    
    void on_actionRemoveState_triggered();
    
    void on_actionRenameState_triggered();
    
    void on_actionOrderStateBackwards_triggered();
    
    void on_actionOrderStateForwards_triggered();
    
    void on_actionAddFrames_triggered();
    
    void on_actionRemoveFrame_triggered();
    
    void on_actionOrderFrameUpwards_triggered();
    
    void on_actionOrderFrameDownwards_triggered();
    
    void on_cmbStates_currentIndexChanged(int index);

private:
    Ui::WidgetSprite *ui;
};

#endif // WIDGETSPRITE_H

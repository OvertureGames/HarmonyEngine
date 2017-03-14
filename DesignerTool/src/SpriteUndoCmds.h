/**************************************************************************
 *	WidgetSpriteUndoCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEUNDOCMDS_H
#define SPRITEUNDOCMDS_H

#include <QUndoCommand>
#include <QTableWidget>

class SpriteTableView;
class IProjItem;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_OrderFrame : public QUndoCommand
{
    SpriteTableView *     m_pSpriteTableView;
    int                         m_iFrameIndex;
    int                         m_iFrameIndexDest;
    
public:
    SpriteUndoCmd_OrderFrame(SpriteTableView *pSpriteTableView, int iFrameIndex, int iFrameIndexDestination, QUndoCommand *pParent = 0);
    virtual ~SpriteUndoCmd_OrderFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_OffsetFrame : public QUndoCommand
{
    SpriteTableView *     m_pSpriteTableView;
    int                         m_iFrameIndex;
    QList<QPoint>               m_OriginalOffsetList;
    QPoint                      m_vNewOffset;
    
public:
    SpriteUndoCmd_OffsetFrame(SpriteTableView *pSpriteTableView, int iIndex, QPoint vOffset, QUndoCommand *pParent = 0);
    virtual ~SpriteUndoCmd_OffsetFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_OffsetXFrame : public QUndoCommand
{
    SpriteTableView *     m_pSpriteTableView;
    int                         m_iFrameIndex;
    QList<QPoint>               m_OriginalOffsetList;
    QList<int>                  m_NewOffsetList;

public:
    SpriteUndoCmd_OffsetXFrame(SpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent = 0);
    virtual ~SpriteUndoCmd_OffsetXFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_OffsetYFrame : public QUndoCommand
{
    SpriteTableView *     m_pSpriteTableView;
    int                         m_iFrameIndex;
    QList<QPoint>               m_OriginalOffsetList;
    QList<int>                  m_NewOffsetList;

public:
    SpriteUndoCmd_OffsetYFrame(SpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent = 0);
    virtual ~SpriteUndoCmd_OffsetYFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_DurationFrame : public QUndoCommand
{
    SpriteTableView *    m_pSpriteTableView;
    int                        m_iFrameIndex;
    QList<float>               m_OriginalDurationList;
    float                      m_fNewDuration;
    
public:
    SpriteUndoCmd_DurationFrame(SpriteTableView *pSpriteTableView, int iIndex, float fDuration, QUndoCommand *pParent = 0);
    virtual ~SpriteUndoCmd_DurationFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

#endif // SPRITEUNDOCMDS_H
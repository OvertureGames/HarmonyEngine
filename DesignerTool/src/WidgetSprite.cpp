/**************************************************************************
 *	WidgetSprite.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetSprite.h"
#include "ui_WidgetSprite.h"

#include "ItemSprite.h"
#include "WidgetUndoCmds.h"
#include "WidgetSpriteUndoCmds.h"
#include "DlgInputName.h"
#include "WidgetAtlasManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAction>

WidgetSprite::WidgetSprite(ItemSprite *pItemSprite, QWidget *parent) :   QWidget(parent),
                                                                         m_pItemSprite(pItemSprite),
                                                                         ui(new Ui::WidgetSprite),
                                                                         m_pCurSpriteState(NULL)
{
    ui->setupUi(this);

    ui->txtPrefixAndName->setText(m_pItemSprite->GetName(true));
    
    QMenu *pEditMenu = m_pItemSprite->GetEditMenu();
    pEditMenu->addAction(ui->actionAddState);
    pEditMenu->addAction(ui->actionRemoveState);
    pEditMenu->addAction(ui->actionRenameState);
    pEditMenu->addAction(ui->actionOrderStateBackwards);
    pEditMenu->addAction(ui->actionOrderStateForwards);
    pEditMenu->addSeparator();
    pEditMenu->addAction(ui->actionImportFrames);
    pEditMenu->addAction(ui->actionRemoveFrames);
    pEditMenu->addAction(ui->actionOrderFrameUpwards);
    pEditMenu->addAction(ui->actionOrderFrameDownwards);

    ui->btnAddState->setDefaultAction(ui->actionAddState);
    ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
    ui->btnRenameState->setDefaultAction(ui->actionRenameState);
    ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
    ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

    m_StateActionsList.push_back(ui->actionImportFrames);
    m_StateActionsList.push_back(ui->actionRemoveFrames);
    m_StateActionsList.push_back(ui->actionOrderFrameUpwards);
    m_StateActionsList.push_back(ui->actionOrderFrameDownwards);
    
    ui->cmbStates->clear();
}

WidgetSprite::~WidgetSprite()
{
    delete ui;
}

// This Load() code can't be put into the ctor because of GetAtlasManager().RequestFrames()
void WidgetSprite::Load()
{
    // If a .hyspr file exists, parse and initalize with it, otherwise make default empty sprite
    QFile spriteFile(m_pItemSprite->GetAbsPath());
    if(spriteFile.exists())
    {
        if(!spriteFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetSprite::WidgetSprite() could not open ") % m_pItemSprite->GetAbsPath(), LOGTYPE_Error);

        QJsonDocument spriteJsonDoc = QJsonDocument::fromJson(spriteFile.readAll());
        spriteFile.close();

        QJsonArray stateArray = spriteJsonDoc.array();
        for(int i = 0; i < stateArray.size(); ++i)
        {
            QJsonObject stateObj = stateArray[i].toObject();

            m_pItemSprite->GetUndoStack()->push(new WidgetUndoCmd_AddState<WidgetSprite, WidgetSpriteState>("Add Sprite State", this, m_StateActionsList, ui->cmbStates));
            m_pItemSprite->GetUndoStack()->push(new WidgetUndoCmd_RenameState<WidgetSpriteState>("Rename Sprite State", ui->cmbStates, stateObj["name"].toString()));
            
            WidgetSpriteState *pSpriteState = GetCurSpriteState();
            
            pSpriteState->GetChkBox_Reverse()->setChecked(stateObj["reverse"].toBool());
            pSpriteState->GetChkBox_Looping()->setChecked(stateObj["loop"].toBool());
            pSpriteState->GetChkBox_Bounce()->setChecked(stateObj["bounce"].toBool());
            
            QJsonArray spriteFrameArray = stateObj["frames"].toArray();
            for(int j = 0; j < spriteFrameArray.size(); ++j)
            {
                QJsonObject spriteFrameObj = spriteFrameArray[j].toObject();
                
                QList<quint32> requestList;
                requestList.append(JSONOBJ_TOINT(spriteFrameObj, "checksum"));
                QList<HyGuiFrame *> pRequestedList = m_pItemSprite->GetAtlasManager().RequestFrames(m_pItemSprite, requestList);

                QPoint vOffset(spriteFrameObj["offsetX"].toInt() - pRequestedList[0]->GetCrop().left(),
                               spriteFrameObj["offsetY"].toInt() - (pRequestedList[0]->GetSize().height() - pRequestedList[0]->GetCrop().bottom()));
                m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetFrame(pSpriteState->GetFrameView(), j, vOffset));
                m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_DurationFrame(pSpriteState->GetFrameView(), j, spriteFrameObj["duration"].toDouble()));
            }
        }
    }
    else
    {
        on_actionAddState_triggered();
        
        //m_pItemSprite->Save();
    }

    // Clear the UndoStack because we don't want any of the above initialization to be able to be undone.
    // I don't believe any 'ItemSpriteCmd_AddState' will leak their dynamically allocated 'm_pSpriteState', since they should become children of 'ui->grpStateLayout'
    m_pItemSprite->GetUndoStack()->clear();

    UpdateActions();
}

ItemSprite *WidgetSprite::GetItemOwner()
{
    return m_pItemSprite;
}

void WidgetSprite::AppendActionsForToolBar(QList<QAction *> &actionList)
{
    actionList.append(ui->actionAlignUp);
    actionList.append(ui->actionAlignLeft);
    actionList.append(ui->actionAlignDown);
    actionList.append(ui->actionAlignRight);
    actionList.append(ui->actionAlignCenterHorizontal);
    actionList.append(ui->actionAlignCenterVertical);
    actionList.append(ui->actionApplyToAll);
}

void WidgetSprite::GetSaveInfo(QJsonArray &spriteStateArrayRef)
{
    for(int i = 0; i < ui->cmbStates->count(); ++i)
    {
        QJsonObject spriteState;
        ui->cmbStates->itemData(i).value<WidgetSpriteState *>()->GetStateFrameInfo(spriteState);
        
        spriteStateArrayRef.append(spriteState);
    }
}

WidgetSpriteState *WidgetSprite::GetCurSpriteState()
{
    return ui->cmbStates->currentData().value<WidgetSpriteState *>();
}

void WidgetSprite::RefreshFrame(HyGuiFrame *pFrame)
{
    for(int i = 0; i < ui->cmbStates->count(); ++i)
    {
        WidgetSpriteState *pSpriteState = ui->cmbStates->itemData(i).value<WidgetSpriteState *>();
        pSpriteState->RefreshFrame(pFrame);
    }
}

QList<HyGuiFrame *> WidgetSprite::GetAllDrawInsts()
{
    QList<HyGuiFrame *> returnList;
    
    for(int i = 0; i < ui->cmbStates->count(); ++i)
        ui->cmbStates->itemData(i).value<WidgetSpriteState *>()->AppendFramesToListRef(returnList);
    
    return returnList;
}

void WidgetSprite::UpdateActions()
{
    ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
    ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
    ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));
    
    WidgetSpriteState *pCurState = GetCurSpriteState();
    bool bFrameIsSelected = pCurState && pCurState->GetNumFrames() > 0 && pCurState->GetSelectedIndex() >= 0;
    
    ui->actionOrderFrameUpwards->setEnabled(pCurState && pCurState->GetSelectedIndex() != 0 && pCurState->GetNumFrames() > 1);
    ui->actionOrderFrameDownwards->setEnabled(pCurState && pCurState->GetSelectedIndex() != pCurState->GetNumFrames() - 1 && pCurState->GetNumFrames() > 1);
    ui->actionRemoveFrames->setEnabled(bFrameIsSelected);
    ui->actionAlignCenterHorizontal->setEnabled(bFrameIsSelected);
    ui->actionAlignCenterVertical->setEnabled(bFrameIsSelected);
    ui->actionAlignUp->setEnabled(bFrameIsSelected);
    ui->actionAlignDown->setEnabled(bFrameIsSelected);
    ui->actionAlignLeft->setEnabled(bFrameIsSelected);
    ui->actionAlignRight->setEnabled(bFrameIsSelected);
}

void WidgetSprite::on_actionAddState_triggered()
{
    QUndoCommand *pCmd = new WidgetUndoCmd_AddState<WidgetSprite, WidgetSpriteState>("Add Sprite State", this, m_StateActionsList, ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionRemoveState_triggered()
{
    QUndoCommand *pCmd = new WidgetUndoCmd_RemoveState<WidgetSprite, WidgetSpriteState>("Remove Sprite State", this, ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionRenameState_triggered()
{
    DlgInputName *pDlg = new DlgInputName("Rename Sprite State", ui->cmbStates->currentData().value<WidgetSpriteState *>()->GetName());
    if(pDlg->exec() == QDialog::Accepted)
    {
        QUndoCommand *pCmd = new WidgetUndoCmd_RenameState<WidgetSpriteState>("Rename Sprite State", ui->cmbStates, pDlg->GetName());
        m_pItemSprite->GetUndoStack()->push(pCmd);
    }
}

void WidgetSprite::on_actionOrderStateBackwards_triggered()
{
    QUndoCommand *pCmd = new WidgetUndoCmd_MoveStateBack<WidgetSprite, WidgetSpriteState>("Shift Sprite State Index <-", this, ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSprite::on_actionOrderStateForwards_triggered()
{
    QUndoCommand *pCmd = new WidgetUndoCmd_MoveStateForward<WidgetSprite, WidgetSpriteState>("Shift Sprite State Index ->", this, ui->cmbStates);
    m_pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSprite::on_actionImportFrames_triggered()
{
    QUndoCommand *pCmd = new WidgetUndoCmd_AddFrames<WidgetSprite>("Add Frames", this);
    m_pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSprite::on_actionRemoveFrames_triggered()
{
    WidgetSpriteState *pSpriteState = ui->cmbStates->itemData(ui->cmbStates->currentIndex()).value<WidgetSpriteState *>();
    SpriteFrame *pSpriteFrame = pSpriteState->GetSelectedFrame();

    QUndoCommand *pCmd = new WidgetUndoCmd_DeleteFrame<WidgetSprite>("Remove Frame", this, pSpriteFrame->m_pFrame);
    m_pItemSprite->GetUndoStack()->push(pCmd);
}

void WidgetSprite::on_cmbStates_currentIndexChanged(int index)
{
    WidgetSpriteState *pSpriteState = ui->cmbStates->itemData(index).value<WidgetSpriteState *>();
    if(m_pCurSpriteState == pSpriteState)
        return;

    if(m_pCurSpriteState)
        m_pCurSpriteState->hide();

    ui->grpStateLayout->addWidget(pSpriteState);

    m_pCurSpriteState = pSpriteState;
    m_pCurSpriteState->show();

    UpdateActions();
}

void WidgetSprite::on_actionAlignLeft_triggered()
{
    WidgetSpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(0.0f);

        m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetXFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(0.0f);
    m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetXFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void WidgetSprite::on_actionAlignRight_triggered()
{
    WidgetSpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().width() * -1);

        m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetXFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(pSpriteTableView->currentIndex().row())->m_pFrame->GetSize().width() * -1);
    m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetXFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void WidgetSprite::on_actionAlignUp_triggered()
{
    WidgetSpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().height() * -1);

        m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetYFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(pSpriteTableView->currentIndex().row())->m_pFrame->GetSize().height() * -1);
    m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetYFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void WidgetSprite::on_actionAlignDown_triggered()
{
    WidgetSpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(0.0f);

        m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetYFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(0.0f);
    m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetYFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void WidgetSprite::on_actionAlignCenterVertical_triggered()
{
    WidgetSpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().height() * -0.5f);

        m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetYFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(pSpriteTableView->currentIndex().row())->m_pFrame->GetSize().height() * -0.5f);
    m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetYFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void WidgetSprite::on_actionAlignCenterHorizontal_triggered()
{
    WidgetSpriteTableView *pSpriteTableView = m_pCurSpriteState->GetFrameView();
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(pSpriteTableView->model());

    if(pSpriteFramesModel->rowCount() == 0)
        return;

    if(ui->actionApplyToAll->isChecked())
    {
        QList<int> newOffsetList;
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            newOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().width() * -0.5f);

        m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetXFrame(pSpriteTableView, -1, newOffsetList));
        return;
    }

    QList<int> newOffsetList;
    newOffsetList.append(pSpriteFramesModel->GetFrameAt(pSpriteTableView->currentIndex().row())->m_pFrame->GetSize().width() * -0.5f);
    m_pItemSprite->GetUndoStack()->push(new WidgetSpriteUndoCmd_OffsetXFrame(pSpriteTableView, pSpriteTableView->currentIndex().row(), newOffsetList));
}

void WidgetSprite::on_actionOrderFrameUpwards_triggered()
{
    int iSelectedIndex = GetCurSpriteState()->GetSelectedIndex();

    QUndoCommand *pCmd = new WidgetSpriteUndoCmd_OrderFrame(GetCurSpriteState()->GetFrameView(), iSelectedIndex, iSelectedIndex - 1);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

void WidgetSprite::on_actionOrderFrameDownwards_triggered()
{
    int iSelectedIndex = GetCurSpriteState()->GetSelectedIndex();

    QUndoCommand *pCmd = new WidgetSpriteUndoCmd_OrderFrame(GetCurSpriteState()->GetFrameView(), iSelectedIndex, iSelectedIndex + 1);
    m_pItemSprite->GetUndoStack()->push(pCmd);

    UpdateActions();
}

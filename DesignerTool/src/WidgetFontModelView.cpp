/**************************************************************************
 *	WidgetFontModelView.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetFontModelView.h"
#include "ItemFontCmds.h"
#include "WidgetFont.h"

#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontTableView::WidgetFontTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void WidgetFontTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();

    iWidth -= 64;
    setColumnWidth(WidgetFontModel::COLUMN_Type, iWidth / 2);
    setColumnWidth(WidgetFontModel::COLUMN_Thickness, 64);
    setColumnWidth(WidgetFontModel::COLUMN_DefaultColor, iWidth / 2);

    QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetFontDelegate::WidgetFontDelegate(ItemFont *pItemFont, QComboBox *pCmbStates, QObject *pParent /*= 0*/) :  QStyledItemDelegate(pParent),
                                                                                                                m_pItemFont(pItemFont),
                                                                                                                m_pCmbStates(pCmbStates)
{
}

/*virtual*/ QWidget* WidgetFontDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pReturnWidget = NULL;

    const WidgetFontModel *pFontModel = static_cast<const WidgetFontModel *>(index.model());

    switch(index.column())
    {
    case WidgetFontModel::COLUMN_Type:
        pReturnWidget = new QComboBox(pParent);
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_NORMAL));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_OUTLINE_EDGE));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_OUTLINE_POSITIVE));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_OUTLINE_NEGATIVE));
        static_cast<QComboBox *>(pReturnWidget)->addItem(pFontModel->GetRenderModeString(RENDER_SIGNED_DISTANCE_FIELD));
        break;

    case WidgetFontModel::COLUMN_Thickness:
        pReturnWidget = new QDoubleSpinBox(pParent);
        static_cast<QDoubleSpinBox *>(pReturnWidget)->setRange(0.0, 4096.0);
        break;

    case WidgetFontModel::COLUMN_DefaultColor:
        pReturnWidget = new QPushButton(pParent);
        //static_cast<QPushButton *>(pReturnWidget)->
        break;
    }

    return pReturnWidget;
}

/*virtual*/ void WidgetFontDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
    const WidgetFontModel *pFontModel = static_cast<const WidgetFontModel *>(index.model());

    switch(index.column())
    {
    case WidgetFontModel::COLUMN_Type:
        static_cast<QComboBox *>(pEditor)->setCurrentIndex(pFontModel->GetLayerRenderMode(index.row()));
        break;

    case WidgetFontModel::COLUMN_Thickness:
        static_cast<QDoubleSpinBox *>(pEditor)->setValue(pFontModel->GetLayerOutlineThickness(index.row()));
        break;

    case WidgetFontModel::COLUMN_DefaultColor:
        break;
    }
}

/*virtual*/ void WidgetFontDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
    WidgetFontModel *pFontModel = static_cast<WidgetFontModel *>(pModel);

    switch(index.column())
    {
    case WidgetFontModel::COLUMN_Type:
        m_pItemFont->GetUndoStack()->push(new ItemFontCmd_LayerRenderMode(*static_cast<WidgetFont *>(m_pItemFont->GetWidget()),
                                                                          m_pCmbStates,
                                                                          pFontModel->GetLayerId(index.row()),
                                                                          pFontModel->GetLayerRenderMode(index.row()),
                                                                          static_cast<rendermode_t>(static_cast<QComboBox *>(pEditor)->currentIndex())));
        break;

    case WidgetFontModel::COLUMN_Thickness:
        m_pItemFont->GetUndoStack()->push(new ItemFontCmd_LayerOutlineThickness(*static_cast<WidgetFont *>(m_pItemFont->GetWidget()),
                                                                                m_pCmbStates,
                                                                                pFontModel->GetLayerId(index.row()),
                                                                                pFontModel->GetLayerOutlineThickness(index.row()),
                                                                                static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;

    case WidgetFontModel::COLUMN_DefaultColor:
        //m_pItemFont->GetUndoStack()->push(new ItemFontCmd_StageColor(m_pTableView, index.row(), static_cast<QDoubleSpinBox *>(pEditor)->value()));
        break;
    }
}

/*virtual*/ void WidgetFontDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    pEditor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int WidgetFontModel::sm_iUniqueIdCounter = 0;

WidgetFontModel::WidgetFontModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_sRenderModeStringList.append("Normal");
    m_sRenderModeStringList.append("Outline Edge");
    m_sRenderModeStringList.append("Outline Added");
    m_sRenderModeStringList.append("Outline Removed");
    m_sRenderModeStringList.append("Distance Field");
}

/*virtual*/ WidgetFontModel::~WidgetFontModel()
{
}


QString WidgetFontModel::GetRenderModeString(rendermode_t eMode) const
{
    return m_sRenderModeStringList[eMode];
}

int WidgetFontModel::AddNewLayer(rendermode_t eRenderMode, float fSize, float fOutlineThickness)
{
    sm_iUniqueIdCounter++;

    int iRowIndex = m_LayerList.count();
    FontLayer *pLayer = new FontLayer(sm_iUniqueIdCounter, eRenderMode, fSize, fOutlineThickness);

    beginInsertRows(QModelIndex(), iRowIndex, iRowIndex);
    m_LayerList.append(pLayer);
    endInsertRows();

    return pLayer->iUNIQUE_ID;
}

void WidgetFontModel::RemoveLayer(int iId)
{
    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        if(m_LayerList[i]->iUNIQUE_ID == iId)
        {
            beginRemoveRows(QModelIndex(), i, i);
            m_RemovedLayerList.append(QPair<int, FontLayer *>(i, m_LayerList[i]));
            m_LayerList.removeAt(i);
            endRemoveRows();
        }
    }
}

void WidgetFontModel::ReAddLayer(int iId)
{
    for(int i = 0; i < m_RemovedLayerList.count(); ++i)
    {
        if(m_RemovedLayerList[i].second->iUNIQUE_ID == iId)
        {
            beginInsertRows(QModelIndex(), m_RemovedLayerList[i].first, m_RemovedLayerList[i].first);
            m_LayerList.insert(m_RemovedLayerList[i].first, m_RemovedLayerList[i].second);
            m_RemovedLayerList.removeAt(i);
            endInsertRows();
        }
    }
}

int WidgetFontModel::GetLayerId(int iRowIndex) const
{
    return m_LayerList[iRowIndex]->iUNIQUE_ID;
}

rendermode_t WidgetFontModel::GetLayerRenderMode(int iRowIndex) const
{
    return m_LayerList[iRowIndex]->eMode;
}

void WidgetFontModel::SetLayerRenderMode(int iId, rendermode_t eMode)
{
    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        if(m_LayerList[i]->iUNIQUE_ID == iId)
        {
            m_LayerList[i]->eMode = eMode;
            dataChanged(createIndex(i, COLUMN_Type), createIndex(i, COLUMN_Type));
        }
    }
}

float WidgetFontModel::GetLayerOutlineThickness(int iRowIndex) const
{
    return m_LayerList[iRowIndex]->fOutlineThickness;
}

void WidgetFontModel::SetLayerOutlineThickness(int iId, float fThickness)
{
    for(int i = 0; i < m_LayerList.count(); ++i)
    {
        if(m_LayerList[i]->iUNIQUE_ID == iId)
        {
            m_LayerList[i]->fOutlineThickness = fThickness;
            dataChanged(createIndex(i, COLUMN_Thickness), createIndex(i, COLUMN_Thickness));
        }
    }
}

void WidgetFontModel::MoveRowUp(int iIndex)
{
    if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
        return;

    m_LayerList.swap(iIndex, iIndex - 1);
    endMoveRows();
}

void WidgetFontModel::MoveRowDown(int iIndex)
{
    if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)    // + 2 is here because Qt is retarded
        return;

    m_LayerList.swap(iIndex, iIndex + 1);
    endMoveRows();
}

void WidgetFontModel::SetFontSize(float fSize)
{
    for(int i = 0; i < m_LayerList.count(); ++i)
        m_LayerList[i]->fSize = fSize;
}

/*virtual*/ int WidgetFontModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return m_LayerList.count();
}

/*virtual*/ int WidgetFontModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return NUMCOLUMNS;
}

/*virtual*/ QVariant WidgetFontModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    FontLayer *pLayer = m_LayerList[index.row()];

    if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Type)
    {
        return Qt::AlignCenter;
    }
    
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_Type:
            return GetRenderModeString(pLayer->eMode);
        case COLUMN_Thickness:
            return QString::number(GetLayerOutlineThickness(index.row()), 'g', 2);
        case COLUMN_DefaultColor:
            return QVariant();//pStage->m_fOutlineThickness;
        }
    }

    return QVariant();
}

/*virtual*/ QVariant WidgetFontModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch(iIndex)
            {
            case COLUMN_Type:
                return QString("Type");
            case COLUMN_Thickness:
                return QString("Thickness");
            case COLUMN_DefaultColor:
                return QString("Default Color");
            }
        }
        else
            return QString::number(iIndex);
    }

    return QVariant();
}

/*virtual*/ bool WidgetFontModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/)
{
    HyGuiLog("WidgetFontModel::setData was invoked", LOGTYPE_Error);

//    SpriteFrame *pFrame = m_FramesList[index.row()];

//    if(role == Qt::EditRole)
//    {
//        switch(index.column())
//        {
//        case COLUMN_OffsetX:
//            pFrame->m_vOffset.setX(value.toInt());
//            break;
//        case COLUMN_OffsetY:
//            pFrame->m_vOffset.setY(value.toInt());
//            break;
//        case COLUMN_Duration:
//            pFrame->m_fDuration = value.toFloat();
//            break;
//        }
//    }

//    QVector<int> vRolesChanged;
//    vRolesChanged.append(role);
//    dataChanged(index, index, vRolesChanged);

    return true;
}

/*virtual*/ Qt::ItemFlags WidgetFontModel::flags(const QModelIndex & index) const
{
    // TODO: Make a read only version of all entries
//    if(index.column() == COLUMN_Type)
//        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
//    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

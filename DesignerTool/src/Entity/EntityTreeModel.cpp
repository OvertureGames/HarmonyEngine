/**************************************************************************
*	EntityTreeModel.cpp
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "EntityTreeModel.h"
#include "EntityModel.h"

EntityTreeItem::EntityTreeItem(EntityTreeModel *pTreeModel, ProjectItem *pItem) :   m_pTreeModel(pTreeModel),
																					m_pItem(pItem)
{
}

EntityTreeItem::~EntityTreeItem()
{
}

ProjectItem *EntityTreeItem::GetItem()
{
	return m_pItem;
}

/*virtual*/ QString EntityTreeItem::GetToolTip() const
{
	return QString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityTreeModel::EntityTreeModel(EntityModel *pEntityModel, ProjectItem &entityItemRef, QObject *parent) :  QAbstractItemModel(parent),
																											m_pEntityModel(pEntityModel)
{
	m_pRootItem = new EntityTreeItem(this, nullptr);
	m_pEntityItem = new EntityTreeItem(this, &entityItemRef);

	InsertItem(0, m_pEntityItem, m_pRootItem);
}

/*virtual*/ EntityTreeModel::~EntityTreeModel()
{
	delete m_pEntityItem;
	delete m_pRootItem;
}

int EntityTreeModel::GetNumStates()
{
	return m_pEntityModel->GetNumStates();
}

QModelIndex EntityTreeModel::index(int iRow, int iColumn, const QModelIndex &parent) const
{
	if(hasIndex(iRow, iColumn, parent) == false)
		return QModelIndex();

	EntityTreeItem *pParentItem;

	if(parent.isValid() == false)
		pParentItem = m_pRootItem;
	else
		pParentItem = static_cast<EntityTreeItem *>(parent.internalPointer());

	EntityTreeItem *pChildItem = static_cast<EntityTreeItem *>(pParentItem->GetChild(iRow));
	if(pChildItem)
		return createIndex(iRow, iColumn, pChildItem);
	else
		return QModelIndex();
}

QModelIndex EntityTreeModel::parent(const QModelIndex &index) const
{
	if(index.isValid() == false)
		return QModelIndex();

	EntityTreeItem *pChildItem = static_cast<EntityTreeItem *>(index.internalPointer());
	EntityTreeItem *pParentItem = static_cast<EntityTreeItem *>(pChildItem->GetParent());

	if(pParentItem == m_pRootItem)
		return QModelIndex();

	return createIndex(pParentItem->GetRow(), 0, pParentItem);
}

int EntityTreeModel::rowCount(const QModelIndex &parentIndex) const
{
	// Only data in column '0' has rows
	if(parentIndex.column() > 0)
		return 0;

	EntityTreeItem *pParentItem;
	if(parentIndex.isValid() == false)
		pParentItem = m_pRootItem;
	else
		pParentItem = static_cast<EntityTreeItem *>(parentIndex.internalPointer());

	return pParentItem->GetNumChildren();
}

int EntityTreeModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

QVariant EntityTreeModel::data(const QModelIndex &index, int iRole /*= Qt::DisplayRole*/) const
{
	if(index.isValid() == false)
		return QVariant();

	EntityTreeItem *pTreeItem = static_cast<EntityTreeItem *>(index.internalPointer());

	switch(iRole)
	{
	case Qt::DisplayRole:
		return pTreeItem->GetItem()->GetName(false);
	case Qt::DecorationRole:
		return pTreeItem->GetItem()->GetIcon(SUBICON_None);
	case Qt::ToolTipRole:
		return pTreeItem->GetToolTip();
	}

	return QVariant();
}

void EntityTreeModel::InsertItem(int iRow, EntityTreeItem *pItem, EntityTreeItem *pParentItem)
{
	QList<EntityTreeItem *> itemList;
	itemList << pItem;
	InsertItems(iRow, itemList, pParentItem);
}

void EntityTreeModel::InsertItems(int iRow, QList<EntityTreeItem *> itemList, EntityTreeItem *pParentItem)
{
	QModelIndex parentIndex = pParentItem ? createIndex(pParentItem->GetRow(), 0, pParentItem) : QModelIndex();

	EntityTreeItem *pParent;
	if(parentIndex.isValid() == false)
		pParent = m_pRootItem;
	else
		pParent = static_cast<EntityTreeItem *>(parentIndex.internalPointer());

	iRow = HyClamp(iRow, 0, pParent->GetNumChildren());

	beginInsertRows(parentIndex, iRow, iRow + itemList.size() - 1);

	for(int i = 0; i < itemList.size(); ++i)
		pParent->InsertChild(iRow + i, itemList[i]);

	endInsertRows();
}

void EntityTreeModel::RemoveItems(int iRow, int iCount, EntityTreeItem *pParentItem)
{
	removeRows(iRow, iCount, pParentItem ? createIndex(pParentItem->GetRow(), 0, pParentItem) : QModelIndex());
}

bool EntityTreeModel::removeRows(int iRow, int iCount, const QModelIndex &parentIndex)
{
	EntityTreeItem *pParent;
	if(parentIndex.isValid() == false)
		pParent = m_pRootItem;
	else
		pParent = static_cast<EntityTreeItem *>(parentIndex.internalPointer());

	if(pParent->GetNumChildren() < iRow + iCount)
		return false;

	beginRemoveRows(parentIndex, iRow, iRow + iCount - 1);

	for(int i = 0; i < iCount; ++i)
		pParent->RemoveChild(iRow);

	endRemoveRows();

	return true;
}

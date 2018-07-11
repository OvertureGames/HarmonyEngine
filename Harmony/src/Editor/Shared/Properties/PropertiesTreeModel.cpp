/**************************************************************************
*	PropertiesTreeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "PropertiesTreeModel.h"
#include "PropertiesUndoCmd.h"

PropertiesTreeModel::PropertiesTreeModel(ProjectItem &itemRef, int iStateIndex, QVariant subState, QObject *parent) :	QAbstractItemModel(parent),
																														m_ItemRef(itemRef),
																														m_iSTATE_INDEX(iStateIndex),
																														m_iSUBSTATE(subState)
{
	m_pRootItem = new PropertiesTreeItem("Root", this, PropertiesDef(), QColor(), "", true);
}

/*virtual*/ PropertiesTreeModel::~PropertiesTreeModel()
{
	delete m_pRootItem;
}

ProjectItem &PropertiesTreeModel::GetItem()
{
	return m_ItemRef;
}

QVariant PropertiesTreeModel::GetValue(QString sUniquePropertyName) const
{
	for(int i = 0; i < m_CategoryList.size(); ++i)
	{
		for(int j = 0; j < m_CategoryList[i]->GetNumChildren(); ++j)
		{
			if(0 == static_cast<PropertiesTreeItem *>(m_CategoryList[i]->GetChild(j))->GetName().compare(sUniquePropertyName, Qt::CaseSensitive))
				return static_cast<PropertiesTreeItem *>(m_CategoryList[i]->GetChild(j))->GetData();
		}
	}

	return QVariant();
}

bool PropertiesTreeModel::AppendCategory(QString sName, QColor color, QVariant commonDelegateBuilder /*= QVariant()*/, bool bCheckable /*= false*/, bool bStartChecked /*= false*/, QString sToolTip /*= ""*/)
{
	for(int i = 0; i < m_CategoryList.size(); ++i)
	{
		if(0 == m_CategoryList[i]->GetName().compare(sName, Qt::CaseSensitive))
			return false;
	}

	PropertiesDef def;
	def.eType = bCheckable ? PROPERTIESTYPE_CategoryChecked : PROPERTIESTYPE_Category;
	def.delegateBuilder = commonDelegateBuilder;

	PropertiesTreeItem *pNewTreeItem = new PropertiesTreeItem(sName, this, def, color, sToolTip, !bCheckable);
	pNewTreeItem->SetData(bStartChecked ? Qt::Checked : Qt::Unchecked);

	InsertItem(m_CategoryList.size(), pNewTreeItem, m_pRootItem);
	m_CategoryList.push_back(pNewTreeItem);

	return true;
}

bool PropertiesTreeModel::AppendProperty(QString sCategoryName, QString sName, PropertiesDef defintion, QString sToolTip, bool bReadOnly)
{
	PropertiesTreeItem *pCategoryTreeItem = ValidateCategory(sCategoryName, sName);
	if(pCategoryTreeItem == nullptr)
		return false;

	PropertiesTreeItem *pNewTreeItem = new PropertiesTreeItem(sName, this, defintion, pCategoryTreeItem->GetColor(), sToolTip, bReadOnly);
	pNewTreeItem->SetData(defintion.defaultData);

	InsertItem(pCategoryTreeItem->GetNumChildren(), pNewTreeItem, pCategoryTreeItem);
	return true;
}

void PropertiesTreeModel::RefreshProperties()
{
	for(int i = 0; i < m_CategoryList.size(); ++i)
	{
		dataChanged(createIndex(0, 0, m_CategoryList[i]->GetChild(0)),
					createIndex(m_CategoryList[i]->GetNumChildren() - 1, 1, m_CategoryList[i]->GetChild(m_CategoryList[i]->GetNumChildren() - 1)));
	}
}

QVariant PropertiesTreeModel::headerData(int iSection, Qt::Orientation orientation, int role) const
{
	if(role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;

	if(role == Qt::DisplayRole)
	{
		if(iSection == 0)
			return "Property";
		else
			return "Value";
	}

	return QVariant();
}

//bool PropertiesModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
//{
//    if (value != headerData(section, orientation, role)) {
//        // FIXME: Implement me!
//        Q_EMIT headerDataChanged(orientation, section, section);
//        return true;
//    }
//    return false;
//}

QModelIndex PropertiesTreeModel::index(int iRow, int iColumn, const QModelIndex &parent) const
{
	if(hasIndex(iRow, iColumn, parent) == false)
		return QModelIndex();

	PropertiesTreeItem *pParentItem;

	if(parent.isValid() == false)
		pParentItem = m_pRootItem;
	else
		pParentItem = static_cast<PropertiesTreeItem *>(parent.internalPointer());

	PropertiesTreeItem *pChildItem = static_cast<PropertiesTreeItem *>(pParentItem->GetChild(iRow));
	if(pChildItem)
		return createIndex(iRow, iColumn, pChildItem);
	else
		return QModelIndex();
}

QModelIndex PropertiesTreeModel::parent(const QModelIndex &index) const
{
	if(index.isValid() == false)
		return QModelIndex();

	PropertiesTreeItem *pChildItem = static_cast<PropertiesTreeItem *>(index.internalPointer());
	PropertiesTreeItem *pParentItem = static_cast<PropertiesTreeItem *>(pChildItem->GetParent());

	if(pParentItem == m_pRootItem)
		return QModelIndex();

	return createIndex(pParentItem->GetRow(), 0, pParentItem);
}

int PropertiesTreeModel::rowCount(const QModelIndex &parentIndex) const
{
	PropertiesTreeItem *pParentItem;
	if(parentIndex.isValid() == false)
		pParentItem = m_pRootItem;
	else
		pParentItem = static_cast<PropertiesTreeItem *>(parentIndex.internalPointer());

	return pParentItem->GetNumChildren();
}

int PropertiesTreeModel::columnCount(const QModelIndex &parent) const
{
	return 2;
}

QVariant PropertiesTreeModel::data(const QModelIndex &index, int iRole) const
{
	if(index.isValid() == false)
		return QVariant();

	PropertiesTreeItem *pTreeItem = static_cast<PropertiesTreeItem *>(index.internalPointer());

	switch(iRole)
	{
	case Qt::DisplayRole:
		if(index.column() == 0)
			return pTreeItem->GetName();
		else if(index.column() == 1)
			return pTreeItem->GetValue();

	case Qt::TextAlignmentRole:
		if(pTreeItem->IsCategory())
			return Qt::AlignHCenter;

	case Qt::ToolTipRole:
		return pTreeItem->GetToolTip();

	case Qt::BackgroundRole:
		if(pTreeItem->IsCategory())
			return QBrush(QColor::fromRgb(160, 160, 160));
		else
			return QBrush((0 == (pTreeItem->GetRow() & 1)) ? pTreeItem->GetColor() : pTreeItem->GetColor().lighter());

	case Qt::ForegroundRole:
		if(pTreeItem->IsCategory())
			return QBrush(QColor::fromRgb(255, 255, 255));

	case Qt::FontRole:
		if(pTreeItem->IsCategory())
		{
			QFont font;
			font.setBold(true);
			return font;
		}

	case Qt::CheckStateRole:
		if((index.column() == 0 && pTreeItem->GetType() == PROPERTIESTYPE_CategoryChecked) ||
		   (index.column() == 1 && pTreeItem->GetType() == PROPERTIESTYPE_bool))
		{
			return pTreeItem->GetData().toInt();
		}
	}

	return QVariant();
}

bool PropertiesTreeModel::setData(const QModelIndex &index, const QVariant &value, int iRole)
{
	if(index.isValid() == false)
		return false;

	if(data(index, iRole) != value)
	{
		PropertiesTreeItem *pTreeItem = static_cast<PropertiesTreeItem *>(index.internalPointer());

		QUndoCommand *pCmd = new PropertiesUndoCmd(*this, m_iSTATE_INDEX, m_iSUBSTATE, *pTreeItem, index, value, iRole);
		m_ItemRef.GetUndoStack()->push(pCmd);
		//Q_EMIT dataChanged(index, index, QVector<int>() << iRole); <- Called within PropertiesUndoCmd's redo/undo

		return true;
	}

	return false;
}

Qt::ItemFlags PropertiesTreeModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags returnFlags = Qt::NoItemFlags;

	if(index.isValid() == false)
		return returnFlags;

	PropertiesTreeItem *pTreeItem = static_cast<PropertiesTreeItem *>(index.internalPointer());

	if(pTreeItem->IsCategory())
	{
		if(pTreeItem->IsReadOnly() == false)
			returnFlags |= Qt::ItemIsEnabled;

		if(pTreeItem->GetType() == PROPERTIESTYPE_CategoryChecked && index.column() == 0)
			returnFlags |= Qt::ItemIsUserCheckable;
	}
	else
	{
		PropertiesTreeItem *pCategoryItem = static_cast<PropertiesTreeItem *>(pTreeItem->GetParent());
		if(pCategoryItem->IsCategory() == false)
			HyGuiLog("PropertiesTreeModel::flags() passed in index is not a category and its parent is not one either", LOGTYPE_Error);

		if(pCategoryItem->GetType() == PROPERTIESTYPE_Category ||
		   (pCategoryItem->GetType() == PROPERTIESTYPE_CategoryChecked && pCategoryItem->GetData().toInt() == Qt::Checked))
		{
			if(pTreeItem->IsReadOnly() == false)
				returnFlags |= Qt::ItemIsEnabled;
		}

		if(index.column() == 1)
		{
			if(pTreeItem->IsReadOnly() == false)
				returnFlags |= (Qt::ItemIsSelectable | Qt::ItemIsEditable);

			if(pTreeItem->GetType() == PROPERTIESTYPE_bool)
				returnFlags |= Qt::ItemIsUserCheckable;
		}
	}

	return returnFlags;
}

//bool PropertiesModel::insertRows(int row, int count, const QModelIndex &parent)
//{
//    beginInsertRows(parent, row, row + count - 1);
//    // FIXME: Implement me!
//    endInsertRows();

//    return true;
//}

//bool PropertiesModel::insertColumns(int column, int count, const QModelIndex &parent)
//{
//    beginInsertColumns(parent, column, column + count - 1);
//    // FIXME: Implement me!
//    endInsertColumns();

//    return true;
//}

bool PropertiesTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
	beginRemoveRows(parent, row, row + count - 1);
	// FIXME: Implement me!
	endRemoveRows();

	return true;
}

//bool PropertiesModel::removeColumns(int column, int count, const QModelIndex &parent)
//{
//    beginRemoveColumns(parent, column, column + count - 1);
//    // FIXME: Implement me!
//    endRemoveColumns();

//    return true;
//}

PropertiesTreeItem *PropertiesTreeModel::ValidateCategory(QString sCategoryName, QString sUniquePropertyName)
{
	PropertiesTreeItem *pCategoryTreeItem = nullptr;
	for(int i = 0; i < m_CategoryList.size(); ++i)
	{
		if(0 == m_CategoryList[i]->GetName().compare(sCategoryName, Qt::CaseSensitive))
		{
			pCategoryTreeItem = m_CategoryList[i];
			break;
		}
	}

	// Now ensure that no property with this name already exists
	for(int i = 0; i < pCategoryTreeItem->GetNumChildren(); ++i)
	{
		if(0 == static_cast<PropertiesTreeItem *>(pCategoryTreeItem->GetChild(i))->GetName().compare(sUniquePropertyName, Qt::CaseSensitive))
			return nullptr; // nullptr indicates failure
	}

	return pCategoryTreeItem;
}

void PropertiesTreeModel::InsertItem(int iRow, PropertiesTreeItem *pItem, PropertiesTreeItem *pParentItem)
{
	QList<PropertiesTreeItem *> itemList;
	itemList << pItem;
	InsertItems(iRow, itemList, pParentItem);
}

void PropertiesTreeModel::InsertItems(int iRow, QList<PropertiesTreeItem *> itemList, PropertiesTreeItem *pParentItem)
{
	QModelIndex parentIndex = pParentItem ? createIndex(pParentItem->GetRow(), 0, pParentItem) : QModelIndex();

	PropertiesTreeItem *pParent;
	if(parentIndex.isValid() == false)
		pParent = m_pRootItem;
	else
		pParent = static_cast<PropertiesTreeItem *>(parentIndex.internalPointer());

	iRow = HyClamp(iRow, 0, pParent->GetNumChildren());

	beginInsertRows(parentIndex, iRow, iRow + itemList.size() - 1);

	for(int i = 0; i < itemList.size(); ++i)
		pParent->InsertChild(iRow + i, itemList[i]);

	endInsertRows();
}

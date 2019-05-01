/**************************************************************************
*	PropertiesTreeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "PropertiesTreeModel.h"
#include "PropertiesUndoCmd.h"

PropertiesTreeModel::PropertiesTreeModel(ProjectItem &ownerRef, int iStateIndex, QVariant subState, QObject *pParent /*= nullptr*/) :
	ITreeModel({ "Property", "Value" }, pParent),
	m_OwnerRef(ownerRef),
	m_iSTATE_INDEX(iStateIndex),
	m_iSUBSTATE(subState)
{
}

/*virtual*/ PropertiesTreeModel::~PropertiesTreeModel()
{
}

ProjectItem &PropertiesTreeModel::GetOwner()
{
	return m_OwnerRef;
}

//QVariant PropertiesTreeModel::GetValue(QString sUniquePropertyName) const
//{
//	for(int i = 0; i < m_CategoryList.size(); ++i)
//	{
//		for(int j = 0; j < m_CategoryList[i]->GetNumChildren(); ++j)
//		{
//			if(0 == static_cast<PropertiesTreeItem *>(m_CategoryList[i]->GetChild(j))->GetName().compare(sUniquePropertyName, Qt::CaseSensitive))
//				return static_cast<PropertiesTreeItem *>(m_CategoryList[i]->GetChild(j))->GetData();
//		}
//	}
//
//	return QVariant();
//}

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

	PropertiesTreeItem *pNewTreeItem = new PropertiesTreeItem(sName, def, color, sToolTip, !bCheckable);
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

	PropertiesTreeItem *pNewTreeItem = new PropertiesTreeItem(sName, defintion, pCategoryTreeItem->GetColor(), sToolTip, bReadOnly);
	pNewTreeItem->SetData(defintion.defaultData);

	InsertItem(pCategoryTreeItem->GetNumChildren(), pNewTreeItem, pCategoryTreeItem);
	return true;
}

//void PropertiesTreeModel::RefreshProperties()
//{
//	for(int i = 0; i < m_CategoryList.size(); ++i)
//	{
//		dataChanged(createIndex(0, 0, m_CategoryList[i]->GetChild(0)),
//					createIndex(m_CategoryList[i]->GetNumChildren() - 1, 1, m_CategoryList[i]->GetChild(m_CategoryList[i]->GetNumChildren() - 1)));
//	}
//}

/*virtual*/ QVariant PropertiesTreeModel::data(const QModelIndex &index, int iRole) const /*override*/
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

/*virtual*/ Qt::ItemFlags PropertiesTreeModel::flags(const QModelIndex &index) const /*override*/
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

/*virtual*/ bool PropertiesTreeModel::setData(const QModelIndex &index, const QVariant &value, int iRole) /*override*/
{
	if(ITreeModel::setData(index, value, iRole))
	{
		TreeModelItem *pTreeItem = static_cast<TreeModelItem*>(index.internalPointer());

		QUndoCommand *pCmd = new PropertiesUndoCmd(*this, m_iSTATE_INDEX, m_iSUBSTATE, *pTreeItem, index, value, iRole);
		m_OwnerRef.GetUndoStack()->push(pCmd);

		return true;
	}

	return false;
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

//bool PropertiesTreeModel::removeRows(int row, int count, const QModelIndex &parent)
//{
//	beginRemoveRows(parent, row, row + count - 1);
//	// FIXME: Implement me!
//	endRemoveRows();
//
//	return true;
//}

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


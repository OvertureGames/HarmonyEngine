/**************************************************************************
*	TextLayersModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TEXTLAYERSMODEL_H
#define TEXTLAYERSMODEL_H

#include "IModel.h"
#include "ProjectItemData.h"
#include "GlobalWidgetMappers.h"
#include "ProjectItemMimeData.h"
#include "TextFontManager.h"

#include <QObject>
#include <QJsonArray>

class TextLayersModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum Column
	{
		COLUMN_Mode = 0,
		COLUMN_Thickness,
		COLUMN_Color,

		NUMCOLUMNS
	};
private:

	TextFontManager &					m_FontManagerRef;

	QList<TextLayerHandle>				m_LayerList;
	QList<QPair<int, TextLayerHandle> >	m_RemovedLayerList;

public:
	TextLayersModel(TextFontManager &fontManagerRef, const QList<TextLayerHandle> &layerList);
	virtual ~TextLayersModel();

	bool IsEmpty() const;
	bool HasHandle(TextLayerHandle hHandle) const;

	TextFontManager &GetFontManager();
	const TextFontManager &GetFontManager() const;
	QJsonArray GetLayersArray() const;
	
	void GetMiscInfo(float &fLeftSideNudgeAmtOut, float &fLineAscenderOut, float &fLineDescenderOut, float &fLineHeightOut) const;

	TextLayerHandle AddNewLayer(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize);
	void RemoveLayer(TextLayerHandle hHandle);
	void ReAddLayer(TextLayerHandle hHandle);
	void MoveRowUp(int iIndex);
	void MoveRowDown(int iIndex);

	QString GetFont() const;
	QString GetFontPath() const;
	void SetFont(QString sFontName);

	float GetFontSize() const;
	void SetFontSize(float fSize);

	QModelIndex GetIndex(TextLayerHandle hLayer, Column eCol) const;
	TextLayerHandle GetHandle(const QModelIndex &indexRef) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &indexRef, int iRole = Qt::DisplayRole) const override;
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole = Qt::EditRole) override;
	virtual Qt::ItemFlags flags(const QModelIndex &indexRef) const override;
};

#endif // TEXTLAYERSMODEL_H

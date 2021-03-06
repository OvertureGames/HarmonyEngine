/**************************************************************************
 *	DlgInputName.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGINPUTNAME_H
#define DLGINPUTNAME_H

#include <QDialog>
#include "ExplorerItemData.h"

namespace Ui {
class DlgInputName;
}

class DlgInputName : public QDialog
{
	Q_OBJECT
	
	void CtorInit(QString sDlgTitle, QString sCurName);

public:
	explicit DlgInputName(const QString sDlgTitle, QString sCurName, QWidget *pParent = 0);
	explicit DlgInputName(const QString sDlgTitle, ExplorerItemData *pItem, QWidget *parent = 0);
	~DlgInputName();
	
	QString GetName();
	
private Q_SLOTS:
	void on_txtName_textChanged(const QString &arg1);
	
private:
	Ui::DlgInputName *ui;

	
	void ErrorCheck();
};

#endif // DLGINPUTNAME_H

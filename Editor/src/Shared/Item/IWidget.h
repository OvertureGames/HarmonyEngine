/**************************************************************************
*	IWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IWIDGET_H
#define IWIDGET_H

#include <QWidget>
#include <QMenu>

namespace Ui {
class IWidget;
}

class IStateData;
class ProjectItemData;

class IWidget : public QWidget
{
	Q_OBJECT

protected:
	ProjectItemData &			m_ItemRef;

public:
	explicit IWidget(ProjectItemData &itemRef, QWidget *pParent = nullptr);
	~IWidget();

	ProjectItemData &GetItem();

	int GetCurStateIndex();
	IStateData *GetCurStateData();
	void UpdateActions();
	void FocusState(int iStateIndex, QVariant subState);

	virtual void OnGiveMenuActions(QMenu *pMenu) = 0;
	virtual void OnUpdateActions() = 0;
	virtual void OnFocusState(int iStateIndex, QVariant subState) = 0;

private Q_SLOTS:
	void OnCurrentIndexChanged(int index);

	void OnAddStateTriggered();

	void OnRemoveStateTriggered();

	void OnRenameStateTriggered();

	void OnOrderStateBackwardsTriggered();

	void OnOrderStateForwardsTriggered();

private:
	Ui::IWidget *ui;
};

#endif // IWIDGET_H

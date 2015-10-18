#ifndef WIDGETATLASMANAGER_H
#define WIDGETATLASMANAGER_H

#include <QWidget>
#include <QDir>

#include "ItemProject.h"
#include "WidgetAtlasGroup.h"

namespace Ui {
class WidgetAtlasManager;
}

class WidgetAtlasManager : public QWidget
{
    Q_OBJECT

    ItemProject *                   m_pProjOwner;

    QDir                            m_MetaDir;
    QDir                            m_DataDir;

    QMap<int, WidgetAtlasGroup *>   m_AtlasGroups;

public:
    explicit WidgetAtlasManager(QWidget *parent = 0);
    explicit WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAtlasManager();

private:
    Ui::WidgetAtlasManager *ui;

    void MakeNewAtlasGroup(int iId = -1);

    void LoadData();
    void SaveData();
};

#endif // WIDGETATLASMANAGER_H

/**************************************************************************
 *	WidgetAtlasGroup.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETATLASGROUP_H
#define WIDGETATLASGROUP_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QImage>
#include <QVariant>

#include "HyGlobal.h"
#include "DlgAtlasGroupSettings.h"
#include "IHyGuiDrawItem.h"
#include "HyGuiFrame.h"
#include "ItemProject.h"

#include "scriptum/imagepacker.h"

namespace Ui {
class WidgetAtlasGroup;
}

class WidgetAtlasGroup : public QWidget
{
    Q_OBJECT

    // NOTE: Order of these member variables matter here for the member initializer list
    QDir                        m_MetaDir;
    QDir                        m_DataDir;

    DlgAtlasGroupSettings       m_dlgSettings;
    
    QList<HyGuiFrame *>         m_FrameList;
    ImagePacker                 m_Packer;

public:
    explicit WidgetAtlasGroup(QWidget *parent = 0);
    explicit WidgetAtlasGroup(QDir metaDir, QDir dataDir, QWidget *parent = 0);
    ~WidgetAtlasGroup();
    
    bool IsMatching(QDir metaDir, QDir dataDir);
    
    QList<HyGuiFrame *> &GetFrameList();
    QTreeWidget *GetTreeWidget();

    void GetAtlasInfo(QJsonObject &atlasObj);
    
    int GetId();

    void ResizeAtlasListColumns();

private slots:
    void on_btnAddImages_clicked();
    void on_btnAddDir_clicked();

    void on_btnSettings_clicked();

    void on_atlasList_itemSelectionChanged();
    
    void on_actionDeleteImage_triggered();

protected:
    virtual void enterEvent(QEvent *pEvent);
    virtual void leaveEvent(QEvent *pEvent);

private:
    Ui::WidgetAtlasGroup *ui;

    WidgetAtlasManager *GetManager();
    
    void ImportImages(QStringList sImportImgList);
    void Refresh();

    void CreateTreeItem(QTreeWidgetItem *pParent, QString sName, int iTextureIndex, eAtlasNodeType eType, HyGuiFrame *pFrame);
};

#endif // WIDGETATLASGROUP_H

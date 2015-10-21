#ifndef WIDGETATLASGROUP_H
#define WIDGETATLASGROUP_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QImage>
#include <QVariant>

#include "HyGlobal.h"
#include "DlgAtlasGroupSettings.h"
#include "scriptum/imagepacker.h"

namespace Ui {
class WidgetAtlasGroup;
}

class HyGuiFrame
{
    quint32             m_uiHash;
    QString             m_sName;
    int                 m_iWidth;
    int                 m_iHeight;
    bool                m_bRotation;
    int                 m_iPosX;
    int                 m_iPosY;
    QRect               m_rAlphaCrop;
    
    QStringList         m_sLinks;
    QTreeWidgetItem *   m_pTreeItem;

public:
    HyGuiFrame(quint32 uiCRC, QString sN, int iW, int iH, bool bRot, int iX, int iY) :  m_uiHash(uiCRC),
                                                                                        m_sName(sN),
                                                                                        m_iWidth(iW),
                                                                                        m_iHeight(iH),
                                                                                        m_bRotation(bRot),
                                                                                        m_iPosX(iX),
                                                                                        m_iPosY(iY),
                                                                                        m_pTreeItem(NULL)
    { }
    
    HyGuiFrame(quint32 uiCRC, QString sN) : m_uiHash(uiCRC),
                                            m_sName(sN),
                                            m_iWidth(-1),
                                            m_iHeight(-1),
                                            m_bRotation(false),
                                            m_iPosX(-1),
                                            m_iPosY(-1),
                                            m_pTreeItem(NULL)
    { }
    
    quint32 GetHash()   { return m_uiHash; }
    
    void SetLink(QString sFullPath)
    {
        m_sLinks.append(sFullPath);
    }
    void SetLink(eItemType eType, QString sPrefix, QString sName)
    {
        QString sLink(HyGlobal::ItemName(eType) % "/");
        sLink += sPrefix;
        sLink += sName;
        
        m_sLinks.append(sLink);
    }
    
    void SetTreeWidgetItem(QTreeWidgetItem *pItem)
    {
        m_pTreeItem = pItem;
        
        QVariant v; v.setValue(this);
        m_pTreeItem->setData(0, QTreeWidgetItem::UserType, v);
    }
    
    QString ConstructImageFileName()
    {
        QString sMetaImgName;
        sMetaImgName = sMetaImgName.sprintf("%010u-%s", m_uiHash, m_sName.toStdString().c_str());
        sMetaImgName += ".png";
        
        return sMetaImgName;
    }
};
Q_DECLARE_METATYPE(HyGuiFrame *)

class WidgetAtlasGroup : public QWidget
{
    Q_OBJECT

    QDir                        m_MetaDir;
    QDir                        m_DataDir;

    DlgAtlasGroupSettings       m_dlgSettings;
    
    QList<QTreeWidgetItem *>    m_TextureList;
    QList<HyGuiFrame *>         m_FrameList;
    ImagePacker                 m_Packer;

public:
    explicit WidgetAtlasGroup(QWidget *parent = 0);
    explicit WidgetAtlasGroup(QDir metaDir, QDir dataDir, QWidget *parent = 0);
    ~WidgetAtlasGroup();

    QTreeWidgetItem *CreateTreeItem(QTreeWidgetItem *pParent, QString sName, eAtlasNodeType eType);

private slots:
    void on_btnAddImages_clicked();
    void on_btnAddDir_clicked();

private:
    Ui::WidgetAtlasGroup *ui;
    
    void ImportImages(QStringList sImportImgList);
    void Refresh();
};

#endif // WIDGETATLASGROUP_H

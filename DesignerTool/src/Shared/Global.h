/**************************************************************************
 *	Global.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QIcon>
#include <QValidator>
#include <QAction>
#include <QStringBuilder>
#include <QDir>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QStack>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "WidgetOutputLog.h"
#include "Harmony/HyEngine.h"

class Project;

#define HyDesignerToolName "Harmony Designer Tool"
//#define HYGUI_UseBinaryMetaFiles

enum HyGuiItemType
{
    TYPE_Unknown = -1,

    DIR_Audio = 0,
    DIR_Particles,
    DIR_Fonts,
    DIR_Spine,
    DIR_Sprites,
    DIR_Shaders,
    DIR_Entities,
    DIR_Atlases,
    DIR_AudioBanks,

    ITEM_Project,       // Sorted this way, so debugger uses "ITEM_Project" instead of "NUMDIR"
    NUMDIR = ITEM_Project,

    ITEM_Prefix,        // Essentially a sub-directory
    ITEM_Primitive,
    ITEM_AtlasImage,
    ITEM_Audio,
    ITEM_Particles,
    ITEM_Font,
    ITEM_Spine,
    ITEM_Sprite,
    ITEM_Shader,
    ITEM_Entity,
    ITEM_BoundingVolume,
    ITEM_Physics,

    NUMTYPES,
    NUMITEMS = NUMTYPES - NUMDIR
};

// NOTE: Do not rearrange the order of this enum
enum AtlasItemType
{
    ATLASITEM_Unknown = -1,

    ATLASITEM_Filter = 0,
    ATLASITEM_Image,
    ATLASITEM_Font,
    ATLASITEM_Spine,

    NUMATLASITEM
};

enum AtlasFrameError
{
    ATLASFRAMEERROR_CannotFindMetaImg = 0,
    ATLASFRAMEERROR_CouldNotPack,

    NUMATLASFRAMEERROR
};

enum MdiArea
{
    MDI_MainWindow      = 1 << 0,
    MDI_Explorer        = 1 << 1,
    MDI_AtlasManager    = 1 << 2,
    MDI_AudioManager    = 1 << 3,
    MDI_ItemProperties  = 1 << 4,
    MDI_Output          = 1 << 5,

    NUM_MDI             = 6,
    MDI_All             = (MDI_MainWindow | MDI_Explorer | MDI_AtlasManager | MDI_AudioManager | MDI_ItemProperties | MDI_Output)
};

enum SubIcon
{
    SUBICON_None = 0,
    SUBICON_New,
    SUBICON_Open,
    SUBICON_Dirty,
    SUBICON_Close,
    SUBICON_Delete,
    SUBICON_Settings,
    SUBICON_Warning,
    SUBICON_Pending,

    NUM_SUBICONS
};

#define HYTREEWIDGETITEM_IsFilter               "HyTreeFilter"

#define HYDEFAULT_PrefixChar                    '+' // Names prefixed with a '+' are engine defaults that are automatically added to project

#define HYGUIPATH_DataFile                      "data.json"
#define HYGUIPATH_TempDir                       "temp/"
#define HYGUIPATH_DataAtlases                   "atlas.json"
#define HYGUIPATH_MetaSettings                  "atlas.hygui"
#define HYGUIPATH_DataAudioCategories           "categories.json"

QAction *FindAction(QList<QAction *> list, QString sName);
#define FINDACTION(str) FindAction(this->actions(), str)

#define HyGuiLog(msg, type) { QString sHyLogTmpStr = msg; WidgetOutputLog::Log(sHyLogTmpStr, type); }

#define JSONOBJ_TOINT(obj, key) obj.value(key).toVariant().toLongLong()

#define ATLASFRAMEID_NotSet 0xFFFFFFFF

char *QStringToCharPtr(QString sString);

QString PointToQString(QPointF ptPoint);
QPointF StringToPoint(QString sPoint);
QByteArray JsonValueToSrc(QJsonValue value);

class HyGlobal
{
    static QString                  sm_sItemNames[NUMTYPES];
    static QString                  sm_sSubIconNames[NUM_SUBICONS];

    static QIcon                    sm_ItemIcons[NUMTYPES][NUM_SUBICONS];

    static QRegExpValidator *       sm_pCodeNameValidator;
    static QRegExpValidator *       sm_pFileNameValidator;
    static QRegExpValidator *       sm_pFilePathValidator;
    static QRegExpValidator *       sm_pVector2dValidator;

    static QString                  sm_ErrorStrings[NUMATLASFRAMEERROR];

public:
    static void Initialize();

    static HyGuiItemType GetDirFromItem(HyGuiItemType eItem);
    static HyGuiItemType GetItemFromDir(HyGuiItemType eDir);
    static HyGuiItemType GetItemFromAtlasItem(AtlasItemType eFrameType);
    static AtlasItemType GetAtlasItemFromItem(HyGuiItemType eItem);
    static QList<HyGuiItemType> SubDirList();
    static QStringList SubDirNameList();
    static QString AtlasTextureTypeString(HyTextureFormat eType);

    static const QString ItemName(HyGuiItemType eItm)                  { return sm_sItemNames[eItm]; }
    static const QString ItemExt(HyGuiItemType eItem);
    static const QIcon ItemIcon(HyGuiItemType eItm, SubIcon eSubIcon)  { return sm_ItemIcons[eItm][eSubIcon]; }

    static const QRegExpValidator *CodeNameValidator()                  { return sm_pCodeNameValidator; }
    static const QRegExpValidator *FileNameValidator()                  { return sm_pFileNameValidator; }
    static const QRegExpValidator *FilePathValidator()                  { return sm_pFilePathValidator; }
    static const QRegExpValidator *Vector2dValidator()                  { return sm_pVector2dValidator; }

    static const QString GetGuiFrameErrors(uint uiErrorFlags);

    static bool IsEngineDirValid(const QDir &engineDir);

    static quint32 CRCData(quint32 crc, const uchar *buf, size_t len);

    static QString MakeFileNameFromCounter(int iCount);

    static void RecursiveFindFileOfExt(QString sExt, QStringList &appendList, QDir dirEntry);

    static QString GetTreeWidgetItemPath(const QTreeWidgetItem *pItem);

    // Includes 'pParentItem' in returned list
    static QList<QTreeWidgetItem *> RecursiveTreeChildren(QTreeWidgetItem *pParentItem);

    static QDir PrepTempDir(Project *pProject);
};

struct SortTreeWidgetsPredicate
{
    bool operator()(const QTreeWidgetItem *pA, const QTreeWidgetItem *pB) const
    {
        return QString::compare(pA->text(0), pB->text(0)) < 0;
    }
};

#endif // GLOBAL_H
/**************************************************************************
 *	Global.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "Project.h"

/*static*/ QString HyGlobal::sm_sItemNames[NUMTYPES];
/*static*/ QString HyGlobal::sm_sItemNamesPlural[NUMTYPES];
/*static*/ QString HyGlobal::sm_AssetNames[NUMASSETTYPES];
/*static*/ QString HyGlobal::sm_sSubIconNames[NUM_SUBICONS];

/*static*/ QIcon HyGlobal::sm_ItemIcons[NUMTYPES][NUM_SUBICONS];
/*static*/ QColor HyGlobal::sm_ItemColors[NUMTYPES];

/*static*/ QString HyGlobal::sm_Themes[NUMTHEMES];

/*static*/ QRegExpValidator *HyGlobal::sm_pCodeNameValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pFreeFormValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pFileNameValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pFilePathValidator = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pNumbersValidator  = nullptr;
/*static*/ QRegExpValidator *HyGlobal::sm_pVector2dValidator = nullptr;

/*static*/ QString HyGlobal::sm_ErrorStrings[NUM_ASSETERRORS];

/*static*/ void HyGlobal::Initialize()
{
	sm_sItemNames[ITEM_Project] = "Project";
	sm_sItemNames[ITEM_Prefix] = "Prefix";
	sm_sItemNames[ITEM_Filter] = "Filter";
	sm_sItemNames[ITEM_AtlasImage] = "Atlas";
	sm_sItemNames[ITEM_Primitive] = "Primitive";
	sm_sItemNames[ITEM_Audio] = "Audio";
	sm_sItemNames[ITEM_Particles] = "Particles";
	sm_sItemNames[ITEM_Text] = "Text";
	sm_sItemNames[ITEM_Spine] = "Spine";
	sm_sItemNames[ITEM_Sprite] = "Sprite";
	sm_sItemNames[ITEM_Source] = "Source";
	sm_sItemNames[ITEM_Header] = "Header";
	sm_sItemNames[ITEM_Entity] = "Entity2d";
    sm_sItemNames[ITEM_Prefab] = "Prefab";
	sm_sItemNames[ITEM_Entity3d] = "Entity3d";
	sm_sItemNames[ITEM_BoundingVolume] = "BoundingVolume";
	sm_sItemNames[ITEM_Physics] = "Physics";

	sm_sItemNamesPlural[ITEM_Project] = "Projects";
	sm_sItemNamesPlural[ITEM_Prefix] = "Prefixes";
	sm_sItemNamesPlural[ITEM_Filter] = "Filters";
	sm_sItemNamesPlural[ITEM_AtlasImage] = "Atlases";
	sm_sItemNamesPlural[ITEM_Primitive] = "Primitives";
	sm_sItemNamesPlural[ITEM_Audio] = "Audio";
	sm_sItemNamesPlural[ITEM_Particles] = "Particles";
	sm_sItemNamesPlural[ITEM_Text] = "Texts";
	sm_sItemNamesPlural[ITEM_Spine] = "Spine";
	sm_sItemNamesPlural[ITEM_Sprite] = "Sprites";
	sm_sItemNamesPlural[ITEM_Source] = "Source";
	sm_sItemNamesPlural[ITEM_Header] = "Headers";
	sm_sItemNamesPlural[ITEM_Entity] = "Entities2d";
    sm_sItemNamesPlural[ITEM_Prefab] = "Prefabs";
	sm_sItemNamesPlural[ITEM_Entity3d] = "Entities3d";
	sm_sItemNamesPlural[ITEM_BoundingVolume] = "BoundingVolumes";
	sm_sItemNamesPlural[ITEM_Physics] = "Physics";

	sm_AssetNames[ASSET_Source] = "Source";
	sm_AssetNames[ASSET_Atlas] = "Atlases";
	sm_AssetNames[ASSET_Prefabs] = "Prefabs";
	sm_AssetNames[ASSET_Audio] = "Audio";

	sm_sSubIconNames[SUBICON_None] = "";
	sm_sSubIconNames[SUBICON_New] = "-New";
	sm_sSubIconNames[SUBICON_Open] = "-Open";
	sm_sSubIconNames[SUBICON_Dirty] = "-Dirty";
	sm_sSubIconNames[SUBICON_Close] = "-Close";
	sm_sSubIconNames[SUBICON_Delete] = "-Delete";
	sm_sSubIconNames[SUBICON_Settings] = "-Settings";
	sm_sSubIconNames[SUBICON_Warning] = "-Warning";
	sm_sSubIconNames[SUBICON_Activated] = "-Pending";

	for(int i = 0; i < NUMTYPES; ++i)
	{
		for(int j = 0; j < NUM_SUBICONS; ++j)
		{
			QString sUrl = ":/icons16x16/items/" % sm_sItemNames[i] % sm_sSubIconNames[j] % ".png";
			sm_ItemIcons[i][j].addFile(sUrl);
		}
	}

	sm_ItemColors[ITEM_Project] = QColor(9, 215, 33);
	sm_ItemColors[ITEM_Prefix] = QColor(228, 212, 128);
	sm_ItemColors[ITEM_Filter] = QColor(228, 212, 128);
	sm_ItemColors[ITEM_AtlasImage] = QColor(45, 131, 176);
	sm_ItemColors[ITEM_Primitive] = QColor(101, 233, 235);
	sm_ItemColors[ITEM_Audio] = QColor(203, 233, 131);
	sm_ItemColors[ITEM_Particles] = QColor(218, 0, 0);
	sm_ItemColors[ITEM_Text] = QColor(179, 179, 179);
	sm_ItemColors[ITEM_Spine] = QColor(209, 159, 223);
	sm_ItemColors[ITEM_Sprite] = QColor(129, 166, 225);
	sm_ItemColors[ITEM_Source] = QColor(225, 151, 97);
	sm_ItemColors[ITEM_Header] = QColor(225, 151, 97);
	sm_ItemColors[ITEM_Entity] = QColor(128, 128, 128);
    sm_ItemColors[ITEM_Prefab] = QColor(203, 233, 131);
	sm_ItemColors[ITEM_Entity3d] = QColor(128, 128, 128);
	sm_ItemColors[ITEM_BoundingVolume] = QColor(234, 232, 58);
	sm_ItemColors[ITEM_Physics] = QColor(201, 58, 203);

	sm_Themes[THEME_Lappy486] = "Lappy486";
	sm_Themes[THEME_Compe] = "Compe";

	sm_pCodeNameValidator = new QRegExpValidator(QRegExp("[A-Za-z_]+[A-Za-z0-9_]*"));
	sm_pFreeFormValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|_\\-\\s]*"));
	sm_pFileNameValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|_\\-]*"));
	sm_pFilePathValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9\\(\\)|/_\\-]*"));
	sm_pNumbersValidator  = new QRegExpValidator(QRegExp("[0-9]*"));
	sm_pVector2dValidator = new QRegExpValidator(QRegExp("\\([0-9]*\\.?[0-9]*,[0-9]*\\.?[0-9]*\\)"));

	sm_ErrorStrings[ASSETERROR_CannotFindMetaFile] = "Cannot find source meta-image";
	sm_ErrorStrings[ASSETERROR_CouldNotPack] = "Could not pack this frame in atlas";
}

/*static*/ QList<HyGuiItemType> HyGlobal::GetProjItemTypeList()
{
	QList<HyGuiItemType> list;
	list.append(ITEM_Audio);
	list.append(ITEM_Particles);
	list.append(ITEM_Text);
	list.append(ITEM_Spine);
	list.append(ITEM_Sprite);
	list.append(ITEM_Prefab);
	list.append(ITEM_Entity);
	list.append(ITEM_Entity3d);

	return list;
}

/*static*/ QList<HyGuiItemType> HyGlobal::GetTypeList()
{
	QList<HyGuiItemType> list;

	list.append(ITEM_Project);
	list.append(ITEM_Prefix);
	list.append(ITEM_Filter);
	list.append(ITEM_AtlasImage);
	list.append(ITEM_Primitive);
	list.append(ITEM_Audio);
	list.append(ITEM_Particles);
	list.append(ITEM_Text);
	list.append(ITEM_Spine);
	list.append(ITEM_Sprite);
	list.append(ITEM_Source);
	list.append(ITEM_Header);
	list.append(ITEM_Entity);
	list.append(ITEM_Prefab);
	list.append(ITEM_Entity3d);
	list.append(ITEM_BoundingVolume);
	list.append(ITEM_Physics);

	if(list.size() != NUMTYPES)
		HyGuiLog("HyGlobal::GetTypeList missing a type!", LOGTYPE_Error);

	return list;
}

/*static*/ QStringList HyGlobal::GetTypeNameList()
{
	QList<HyGuiItemType> dirList = GetTypeList();

	QStringList list;
	for(int i = 0; i < dirList.size(); ++i)
		list.append(sm_sItemNames[dirList[i]]);

	return list;
}

/*static*/ HyGuiItemType HyGlobal::GetTypeFromString(QString sType)
{
	QStringList sTypeList = GetTypeNameList();
	for(int i = 0; i < sTypeList.size(); ++i)
	{
		if(sType.compare(sTypeList[i], Qt::CaseInsensitive) == 0)
			return GetTypeList()[i];
	}

	return ITEM_Unknown;
}

/*static*/ const QString HyGlobal::ItemExt(HyGuiItemType eItem)
{
	switch(eItem)
	{
	case ITEM_Project:
		return ".hyproj";
	}

	return "";
}

/*static*/ const QString HyGlobal::GetGuiFrameErrors(uint uiErrorFlags)
{
	QString sErrorString;
	sErrorString.clear();

	for(int i = 0; i < NUM_ASSETERRORS; ++i)
	{
		if((uiErrorFlags & (1 << i)) != 0)
		{
			if(sErrorString.isEmpty())
				sErrorString += sm_ErrorStrings[i];
			else
				sErrorString += "; \n" % sm_ErrorStrings[i];
		}
	}

	return sErrorString;
}

/*static*/ bool HyGlobal::IsEngineDirValid(const QDir &engineDir)
{
	QDir dir(engineDir);
	if(!dir.exists())
		return false;

	if(!dir.exists("Editor/") ||
	   !dir.exists("Engine/") ||
	   !dir.exists("CMakeLists.txt"))
	{
		return false;
	}

	return true;
}

/*static*/ quint32 HyGlobal::CRCData(quint32 crc, const uchar *buf, size_t len)
{
	static quint32 table[256];
	static int have_table = 0;
	quint32 rem, octet;
	const uchar *p, *q;

	/* This check is not thread safe; there is no mutex. */
	if(have_table == 0)
	{
		/* Calculate CRC table. */
		for(int i = 0; i < 256; i++)
		{
			rem = i;  /* remainder from polynomial division */
			for(int j = 0; j < 8; j++)
			{
				if(rem & 1)
				{
					rem >>= 1;
					rem ^= 0xedb88320;
				}
				else
				{
					rem >>= 1;
				}
			}
			table[i] = rem;
		}
		have_table = 1;
	}

	crc = ~crc;
	q = buf + len;
	for(p = buf; p < q; p++)
	{
		octet = *p;  /* Cast to unsigned octet. */
		crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
	}
	return ~crc;
}

/*static*/ QString HyGlobal::MakeFileNameFromCounter(int iCount)
{
	QString sNewString = QString("%1").arg(iCount, 5, 10, QChar('0'));

	return sNewString;
}

/*static*/ void HyGlobal::RecursiveFindFileOfExt(QString sExt, QStringList &appendList, QDir dirEntry)
{
	if(sExt.startsWith('.'))
		sExt.remove(0, 1);
	sExt = sExt.toLower();

	QFileInfoList list = dirEntry.entryInfoList();
	QStack<QFileInfoList> dirStack;
	dirStack.push(list);

	while(dirStack.isEmpty() == false)
	{
		list = dirStack.pop();
		for(int i = 0; i < list.count(); i++)
		{
			QFileInfo info = list[i];
			QString sFileName = info.fileName();
			QString sFilePath = info.filePath();
			QString sSuffix = info.suffix().toLower();

			if(info.isDir() && info.fileName() != ".." && info.fileName() != ".")
			{
				QDir subDir(info.filePath());
				QFileInfoList subList = subDir.entryInfoList();

				dirStack.push(subList);
			}
			else if(info.suffix().toLower() == sExt)
			{
				appendList.push_back(info.filePath());
			}
		}
	}
}

/*static*/ QString HyGlobal::GetTreeWidgetItemPath(const QTreeWidgetItem *pItem)
{
	QString sPath = "";
	while(pItem)
	{
		sPath = "/" % pItem->text(0) % sPath;
		pItem = pItem->parent();
	}

	return sPath;
}

// Includes 'pParentItem' in returned list
/*static*/ QList<QTreeWidgetItem *> HyGlobal::RecursiveTreeChildren(QTreeWidgetItem *pParentItem)
{
	QStack<QTreeWidgetItem *> itemStack;
	itemStack.push(pParentItem);

	QList<QTreeWidgetItem *> returnList;
	QStringList testList;
	while(!itemStack.isEmpty())
	{
		QTreeWidgetItem *pItem = itemStack.pop();
		returnList.append(pItem);
		testList.append(pItem->text(0));

		for(int i = 0; i < pItem->childCount(); ++i)
			itemStack.push(pItem->child(i));
	}

	return returnList;
}

/*static*/ QDir HyGlobal::PrepTempDir(Project *pProject)
{
	QDir metaDir(pProject->GetMetaDataAbsPath());
	QDir metaTempDir(metaDir.absoluteFilePath(HYGUIPATH_TempDir));
	if(metaTempDir.exists())
	{
		QFileInfoList tempFileInfoList = metaTempDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
		for(int i = 0; i < tempFileInfoList.size(); ++i)
		{
			if(false == QFile::remove(tempFileInfoList[i].absoluteFilePath()))
				HyGuiLog("Could not remove temp file: " % tempFileInfoList[i].fileName(), LOGTYPE_Error);
		}
	}
	else if(false == metaTempDir.mkpath("."))
		HyGuiLog("Could not make meta temp directory", LOGTYPE_Error);

	return metaTempDir;
}

bool HyGlobal::IsItemFileDataValid(const FileDataPair &itemfileDataRef)
{
	std::function<bool(const QJsonArray &)> fpMetaStateArrayCheck = [](const QJsonArray &metaStateArrayRef)->bool
	{
		for(int i = 0; i < metaStateArrayRef.size(); ++i)
		{
			if(metaStateArrayRef[i].isObject() == false ||
				metaStateArrayRef[i].toObject().contains("name") == false ||
				metaStateArrayRef[i].toObject()["name"].isString() == false)
			{
				return false;
			}
		}

		return true;
	};

	return (itemfileDataRef.m_Meta.contains("stateArray") && itemfileDataRef.m_Meta["stateArray"].isArray() &&
		itemfileDataRef.m_Data.contains("stateArray") && itemfileDataRef.m_Meta["stateArray"].isArray() &&
		itemfileDataRef.m_Meta["stateArray"].toArray().count() == itemfileDataRef.m_Data["stateArray"].toArray().count() &&
		fpMetaStateArrayCheck(itemfileDataRef.m_Meta["stateArray"].toArray()) &&
		itemfileDataRef.m_Meta.contains("CameraPos") && itemfileDataRef.m_Meta["CameraPos"].isArray() &&
		itemfileDataRef.m_Meta.contains("CameraZoom") && itemfileDataRef.m_Meta["CameraZoom"].isDouble() &&
		itemfileDataRef.m_Meta.contains("UUID") && itemfileDataRef.m_Meta["UUID"].isString());
}

FileDataPair HyGlobal::GenerateNewItemFileData(QString sImportPath /*= ""*/)
{
	FileDataPair newItemFileData;
	newItemFileData.m_Meta["stateArray"] = QJsonArray();
	newItemFileData.m_Data["stateArray"] = QJsonArray();

	QJsonArray cameraPosArray;
	cameraPosArray.append(0);
	cameraPosArray.append(0);
	newItemFileData.m_Meta["CameraPos"] = cameraPosArray;
	newItemFileData.m_Meta["CameraZoom"] = 1;

	newItemFileData.m_Meta["UUID"] = QUuid::createUuid().toString(QUuid::WithoutBraces);

	if(IsItemFileDataValid(newItemFileData) == false)
		HyGuiLog("ExplorerModel::GenerateNewItemFileData - IsItemFileDataValid return false", LOGTYPE_Error);

	return newItemFileData;
}

void HyGlobal::ModifyJsonObject(QJsonObject &objRef, const QString &path, const QJsonValue &newValue)
{
	const int indexOfDot = path.indexOf('.');
	const QString propertyName = path.left(indexOfDot);
	const QString subPath = indexOfDot>0 ? path.mid(indexOfDot+1) : QString();

	QJsonValue subValue = objRef[propertyName];

	if(subPath.isEmpty())
		subValue = newValue;
	else
	{
		QJsonObject obj = subValue.toObject();
		ModifyJsonObject(obj, subPath, newValue);
		subValue = obj;
	}

	objRef[propertyName] = subValue;
}

void HyGlobal::ModifyJsonObject(QJsonDocument &docRef, const QString &sPath, const QJsonValue &newValue)
{
	QJsonObject obj = docRef.object();
	ModifyJsonObject(obj, sPath, newValue);

	docRef = QJsonDocument(obj);
}


QAction *FindAction(QList<QAction *> list, QString sName)
{
	for(int i = 0; i < list.size(); ++i)
	{
		if(list[i]->objectName() == sName)
			return list[i];
	}
	
	return nullptr;
}

char *QStringToCharPtr(QString sString)
{
	QByteArray ba;
	ba.append(sString);
	return ba.data();
}

QString PointToQString(QPointF ptPoint)
{
	return "(" % QString::number(ptPoint.x(), 'g', 4) % "," % QString::number(ptPoint.y(), 'g', 4) % ")";
}

QPointF StringToPoint(QString sPoint)
{
	sPoint.replace(QString("("), QString(""));
	sPoint.replace(QString(")"), QString(""));
	QStringList sComponentList = sPoint.split(',');
	
	if(sComponentList.size() < 2)
		return QPointF(0.0f, 0.0f);
	else
		return QPointF(sComponentList[0].toFloat(), sComponentList[1].toFloat());
}

QByteArray JsonValueToSrc(QJsonValue value)
{
	QByteArray src;
	if(value.isArray())
	{
		QJsonDocument tmpDoc(value.toArray());
		src = tmpDoc.toJson();
	}
	else
	{
		QJsonDocument tmpDoc(value.toObject());
		src = tmpDoc.toJson();
	}

	return src;
}

/**************************************************************************
 *	IAssetItemData.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IAssetItemData.h"
#include "IManagerModel.h"

AssetItemData::AssetItemData(IManagerModel &modelRef, QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, QString sMetaFileExt, uint uiErrors) :
	m_ModelRef(modelRef),
	m_UNIQUE_ID(uuid),
	m_uiChecksum(uiChecksum),
	m_uiBankId(uiBankId),
	m_sName(sName),
	m_sMetaFileExt(sMetaFileExt),
	m_uiErrors(uiErrors) // '0' when there is no error
{
}

const QUuid &AssetItemData::GetUuid()
{
	return m_UNIQUE_ID;
}

quint32 AssetItemData::GetChecksum()
{
	return m_uiChecksum;
}

quint32 AssetItemData::GetBankId()
{
	return m_uiBankId;
}

void AssetItemData::SetBankId(quint32 uiNewBankId)
{
	m_uiBankId = uiNewBankId;
}

QString AssetItemData::GetFilter() const
{
	return m_ModelRef.AssembleFilter(this);
}

QString AssetItemData::GetName() const
{
	return m_sName;
}
void AssetItemData::SetName(QString sNewName)
{
	m_sName = sNewName;
}

QSet<ProjectItemData *> AssetItemData::GetLinks()
{
	return m_DependencySet;
}

void AssetItemData::InsertLink(ProjectItemData *pProjItem)
{
	m_DependencySet.insert(pProjItem);
}

void AssetItemData::RemoveLink(ProjectItemData *pProjItem)
{
	m_DependencySet.remove(pProjItem);
}

QString AssetItemData::ConstructMetaFileName()
{
	QString sMetaName;
	sMetaName = sMetaName.sprintf("%010u", m_uiChecksum);
	sMetaName += m_sMetaFileExt;

	return sMetaName;
}

bool AssetItemData::DeleteMetaFile()
{
	QFile imageFile(m_ModelRef.GetMetaDir().absoluteFilePath(ConstructMetaFileName()));
	if(imageFile.remove() == false)
		return false;

	return true;
}

void AssetItemData::SetError(AtlasFrameError eError)
{
	if(eError == ATLASFRAMEERROR_CannotFindMetaImg)
		HyGuiLog(m_sName % " - GUIFRAMEERROR_CannotFindMetaImg", LOGTYPE_Error);

	m_uiErrors |= (1 << eError);

	//UpdateTreeItemIconAndToolTip();
}

void AssetItemData::ClearError(AtlasFrameError eError)
{
	m_uiErrors &= ~(1 << eError);

	//UpdateTreeItemIconAndToolTip();
}

uint AssetItemData::GetErrors()
{
	return m_uiErrors;
}
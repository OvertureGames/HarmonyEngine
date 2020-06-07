/**************************************************************************
 *	AtlasModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASMODEL_H
#define ATLASMODEL_H

#include "IManagerModel.h"

#include "AtlasFrame.h"
#include "_Dependencies/scriptum/imagepacker.h"

class AtlasModel : public IManagerModel
{
	Q_OBJECT

public:
	AtlasModel(Project &projRef);
	virtual ~AtlasModel();

	//void StashTreeWidgets(QList<AtlasTreeItem *> treeItemList);
	//QList<AtlasTreeItem *> GetTopLevelTreeItemList();

	QFileInfoList GetExistingTextureInfoList(uint uiBankIndex);

	int GetNumTextures(uint uiBankIndex);
	QSize GetAtlasDimensions(uint uiBankIndex);
	HyTextureFormat GetAtlasTextureType(uint uiBankIndex);

	bool IsImageValid(QImage &image, quint32 uiAtlasGrpId);
	bool IsImageValid(int iWidth, int iHeight, quint32 uiAtlasGrpId);
	bool IsImageValid(int iWidth, int iHeight, const QJsonObject &atlasSettings);

	AtlasFrame *GenerateFrame(ProjectItemData *pItem, QString sName, QImage &newImage, quint32 uiAtlasGrpIndex, HyGuiItemType eType);
	bool ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack);

	void RepackAll(uint uiAtlasGrpIndex);
	void Repack(uint uiAtlasGrpIndex, QSet<int> repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet);

protected:
	virtual void OnCreateBank(BankData &newBankRef) override;
	virtual void OnDeleteBank(BankData &bankToBeDeleted) override;
	virtual AssetItemData *OnAllocateAssetData(QJsonObject metaObj) override;
	virtual AssetItemData *OnAllocateAssetData(QString sFilePath, quint32 uiBankId, HyGuiItemType eType) override;

	virtual QJsonObject GetSaveJson() override;

private Q_SLOTS:
	void OnLoadUpdate(QString sMsg, int iPercComplete);
	void OnRepackFinished();
};

//struct AtlasGrp
//{
//	QDir										m_DataDir;
//
//	QJsonObject									m_PackerSettings;
//	ImagePacker									m_Packer;
//	QList<AtlasFrame *>							m_FrameList;
//
//	AtlasGrp(QString sAbsDataDirPath) : m_DataDir(sAbsDataDirPath)
//	{ }
//
//	quint32 GetId()
//	{
//		return JSONOBJ_TOINT(m_PackerSettings, "atlasGrpId");
//	}
//
//	QFileInfoList GetExistingTextureInfoList()
//	{
//		return m_DataDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
//	}
//
//	void SetPackerSettings()
//	{
//		m_Packer.sortOrder = m_PackerSettings["cmbSortOrder"].toInt();// m_iSortOrderIndex;//ui->cmbSortOrder->currentIndex();
//		m_Packer.border.t = m_PackerSettings["sbFrameMarginTop"].toInt();// m_iFrameMarginTop;//ui->sbFrameMarginTop->value();
//		m_Packer.border.l = m_PackerSettings["sbFrameMarginLeft"].toInt();// m_iFrameMarginLeft;//ui->sbFrameMarginLeft->value();
//		m_Packer.border.r = m_PackerSettings["sbFrameMarginRight"].toInt();// m_iFrameMarginRight;//ui->sbFrameMarginRight->value();
//		m_Packer.border.b = m_PackerSettings["sbFrameMarginBottom"].toInt();// m_iFrameMarginBottom;//ui->sbFrameMarginBottom->value();
//		m_Packer.extrude = m_PackerSettings["extrude"].toInt();// m_iExtrude;//ui->extrude->value();
//		m_Packer.merge = m_PackerSettings["chkMerge"].toBool();// m_bMerge;//ui->chkMerge->isChecked();
//		m_Packer.square = m_PackerSettings["chkSquare"].toBool();// m_bSquare;//ui->chkSquare->isChecked();
//		m_Packer.autosize = m_PackerSettings["chkAutosize"].toBool();// m_bAutoSize;//ui->chkAutosize->isChecked();
//		m_Packer.minFillRate = m_PackerSettings["minFillRate"].toInt();// m_iFillRate;//ui->minFillRate->value();
//		m_Packer.mergeBF = false;
//		m_Packer.rotate = ImagePacker::NEVER;
//	}
//};

#endif // ATLASMODEL_H

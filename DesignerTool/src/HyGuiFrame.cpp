/**************************************************************************
 *	HyGuiFrame.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyGuiFrame.h"
#include "scriptum/imagepacker.h"

HyGuiFrame::HyGuiFrame(quint32 uiChecksum, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, eAtlasNodeType eType, int iW, int iH, int iTexIndex, bool bRot, int iX, int iY, QString sFilter, uint uiErrors) : m_uiATLAS_GROUP_ID(uiAtlasGroupId),
                                                                                                                                                                                                                m_eType(eType),
                                                                                                                                                                                                                m_pTreeWidgetItem(NULL),
                                                                                                                                                                                                                m_uiChecksum(uiChecksum),
                                                                                                                                                                                                                m_sName(sN),
                                                                                                                                                                                                                m_iWidth(iW),
                                                                                                                                                                                                                m_iHeight(iH),
                                                                                                                                                                                                                m_rAlphaCrop(rAlphaCrop),
                                                                                                                                                                                                                m_iTextureIndex(iTexIndex),
                                                                                                                                                                                                                m_bRotation(bRot),
                                                                                                                                                                                                                m_iPosX(iX),
                                                                                                                                                                                                                m_iPosY(iY),
                                                                                                                                                                                                                m_sFilter(sFilter),
                                                                                                                                                                                                                m_uiErrors(uiErrors)
{
}

HyGuiFrame::~HyGuiFrame()
{
    QMapIterator<void *, HyTexturedQuad2d *> iter(m_DrawInstMap);
    while(iter.hasNext())
    {
        iter.next();
        delete iter.value();
    }
}

void HyGuiFrame::ReplaceImage(QString sImgPath, QDir metaDir)
{
    QFileInfo fileInfo(sImgPath);
    QImage newImage(fileInfo.absoluteFilePath());

    ReplaceImage(fileInfo.baseName(), newImage, metaDir);
}

void HyGuiFrame::ReplaceImage(QString sName, QImage &newImage, QDir metaDir)
{
    QFile oldImageFile(metaDir.path() % "/" % ConstructImageFileName());
    if(oldImageFile.remove() == false)
        HyGuiLog("Could not remove old meta image file when replacing " % sName, LOGTYPE_Error);

    m_uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());
    m_sName = sName;
    m_iWidth = newImage.width();
    m_iHeight = newImage.height();
    
    if(m_eType != ATLAS_Font && m_eType != ATLAS_Spine) // Cannot crop 'sub-atlases' because they rely on their own UV coordinates
        m_rAlphaCrop = ImagePacker::crop(newImage);
    else
        m_rAlphaCrop = QRect(0, 0, newImage.width(), newImage.height());

    m_iTextureIndex = -1;
    m_bRotation = false;
    m_iPosX = -1;
    m_iPosY = -1;

    if(newImage.save(metaDir.path() % "/" % ConstructImageFileName()) == false)
        HyGuiLog("Could not save frame image to meta directory: " % m_sName, LOGTYPE_Error);
}

HyTexturedQuad2d *HyGuiFrame::DrawInst(void *pKey)
{
    QMap<void *, HyTexturedQuad2d *>::iterator iter = m_DrawInstMap.find(pKey);
    if(iter != m_DrawInstMap.end())
        return iter.value();
    
    // Not found, create a new HyTexturedQuad2d based on key
    HyTexturedQuad2d *pDrawInst = new HyTexturedQuad2d(m_uiATLAS_GROUP_ID);
    if(m_bRotation == false)
        pDrawInst->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rAlphaCrop.width(), m_rAlphaCrop.height());
    else
    {
        pDrawInst->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rAlphaCrop.height(), m_rAlphaCrop.width());
        pDrawInst->rot_pivot.Set(m_rAlphaCrop.height() * 0.5f, m_rAlphaCrop.width() * 0.5f);
        pDrawInst->rot.Set(90);
    }

    pDrawInst->SetEnabled(false);
    pDrawInst->SetTag(reinterpret_cast<int64>(this));

    m_DrawInstMap.insert(pKey, pDrawInst);

    return pDrawInst;
}

void HyGuiFrame::DeleteDrawInst(void *pKey)
{
    QMap<void *, HyTexturedQuad2d *>::iterator iter = m_DrawInstMap.find(pKey);
    if(iter != m_DrawInstMap.end())
    {
        iter.value()->Unload();
        m_DrawInstMap.remove(pKey);
    }
}

void HyGuiFrame::DeleteAllDrawInst()
{
    QMap<void *, HyTexturedQuad2d *>::iterator iter = m_DrawInstMap.begin();
    for(; iter != m_DrawInstMap.end(); ++iter)
    {
        delete iter.value();
    }
    
    m_DrawInstMap.clear();
}

void HyGuiFrame::SetTreeWidgetItem(QTreeWidgetItem *pTreeItem)
{
    m_pTreeWidgetItem = pTreeItem;
}

QString HyGuiFrame::GetFilter()
{
    return m_sFilter;
}

void HyGuiFrame::SetFilter(QString sFilter)
{
    m_sFilter = sFilter;
}

void HyGuiFrame::UpdateInfoFromPacker(int iTextureIndex, bool bRotation, int iX, int iY)
{
    m_iTextureIndex = iTextureIndex;
    m_bRotation = bRotation;
    m_iPosX = iX;
    m_iPosY = iY;

    if(m_iTextureIndex != -1)
    {
        QMapIterator<void *, HyTexturedQuad2d *> iter(m_DrawInstMap);
        while(iter.hasNext())
        {
            iter.next();
            
            if(m_bRotation == false)
                iter.value()->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rAlphaCrop.width(), m_rAlphaCrop.height());
            else
            {
                iter.value()->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rAlphaCrop.height(), m_rAlphaCrop.width());
                iter.value()->rot_pivot.Set(m_rAlphaCrop.height() * 0.5f, m_rAlphaCrop.width() * 0.5f);
                iter.value()->rot.Set(90);
            }
        }
    }
    else
        SetError(GUIFRAMEERROR_CouldNotPack);
}

QString HyGuiFrame::ConstructImageFileName()
{
    QString sMetaImgName;
    sMetaImgName = sMetaImgName.sprintf("%010u-%s", m_uiChecksum, m_sName.toStdString().c_str());
    sMetaImgName += ".png";

    return sMetaImgName;
}

void HyGuiFrame::GetJsonObj(QJsonObject &frameObj)
{
    frameObj.insert("checksum", QJsonValue(static_cast<qint64>(GetChecksum())));
    frameObj.insert("name", QJsonValue(GetName()));
    frameObj.insert("width", QJsonValue(GetSize().width()));
    frameObj.insert("height", QJsonValue(GetSize().height()));
    frameObj.insert("textureIndex", QJsonValue(GetTextureIndex()));
    frameObj.insert("type", QJsonValue(GetType()));
    frameObj.insert("rotate", QJsonValue(IsRotated()));
    frameObj.insert("x", QJsonValue(GetX()));
    frameObj.insert("y", QJsonValue(GetY()));
    frameObj.insert("cropLeft", QJsonValue(GetCrop().left()));
    frameObj.insert("cropTop", QJsonValue(GetCrop().top()));
    frameObj.insert("cropRight", QJsonValue(GetCrop().right()));
    frameObj.insert("cropBottom", QJsonValue(GetCrop().bottom()));
    frameObj.insert("errors", QJsonValue(static_cast<int>(GetErrors())));
}

void HyGuiFrame::SetError(eGuiFrameError eError)
{
    m_uiErrors |= (1 << eError);

    if(m_pTreeWidgetItem)
    {
        m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(ATLAS_Frame_Warning));
        m_pTreeWidgetItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(m_uiErrors));
    }
}

void HyGuiFrame::ClearError(eGuiFrameError eError)
{
    m_uiErrors &= ~(1 << eError);

    if(m_pTreeWidgetItem)
    {
        if(m_uiErrors == 0)
            m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(m_eType));
        else
        {
            m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(ATLAS_Frame_Warning));
            m_pTreeWidgetItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(m_uiErrors));
        }
    }
}

uint HyGuiFrame::GetErrors()
{
    return m_uiErrors;
}

/**************************************************************************
 *	HyFileAtlas.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyFileAtlas.h"
#include "Assets/HyAssets.h"
#include "Renderer/IHyRenderer.h"
#include "soil2/SOIL2.h"
#include "HyEngine.h"

HyFileAtlas::HyFileAtlas(std::string sFileName,
				 uint32 uiBankId,
				 uint32 uiIndexInGroup,
				 uint32 uiManifestIndex,
				 HyJsonObj textureObj) :
	IHyFile(sFileName, HYFILE_Atlas, uiManifestIndex),
	m_uiBANK_ID(uiBankId),
	m_uiINDEX_IN_GROUP(uiIndexInGroup),
	m_uiWIDTH(textureObj["width"].GetUint()),
	m_uiHEIGHT(textureObj["height"].GetUint()),
	m_eTEXTURE_FORMAT(HyAssets::GetTextureFormatFromString(textureObj["format"].GetString())),
	m_eTEXTURE_FILTERING(HyAssets::GetTextureFilteringFromString(textureObj["filtering"].GetString())),
	m_hTextureHandle(0),
	m_uiNUM_FRAMES(textureObj["assets"].GetArray().Size()),
	m_pPixelData(nullptr),
	m_uiPixelDataSize(0)
{
	m_pFrames = HY_NEW HyRectangle<int32>[m_uiNUM_FRAMES];

	HyJsonArray framesArrayRef = textureObj["assets"].GetArray();
	for(uint32 k = 0; k < m_uiNUM_FRAMES; ++k)
	{
		HyJsonObj srcFrameObj = framesArrayRef[k].GetObject();

		m_pFrames[k].bottom = srcFrameObj["bottom"].GetUint();
		m_pFrames[k].right = srcFrameObj["right"].GetUint();
		m_pFrames[k].left = srcFrameObj["left"].GetUint();
		m_pFrames[k].top = srcFrameObj["top"].GetUint();

		m_ChecksumMap[srcFrameObj["checksum"].GetUint()] = &m_pFrames[k];
	}
}

HyFileAtlas::~HyFileAtlas()
{
	delete [] m_pFrames;
	DeletePixelData();
}

uint32 HyFileAtlas::GetBankId() const
{
	return m_uiBANK_ID;
} 

uint32 HyFileAtlas::GetIndexInGroup() const
{
	return m_uiINDEX_IN_GROUP;
}

uint32 HyFileAtlas::GetWidth() const
{
	return m_uiWIDTH;
}

uint32 HyFileAtlas::GetHeight() const
{
	return m_uiHEIGHT;
}

HyTextureHandle HyFileAtlas::GetTextureHandle() const
{
	return m_hTextureHandle;
}

bool HyFileAtlas::GetUvRect(uint32 uiChecksum, HyRectangle<float> &UVRectOut) const
{
	float fTexWidth = static_cast<float>(m_uiWIDTH);
	float fTexHeight = static_cast<float>(m_uiHEIGHT);

	const HyRectangle<int32> *pSrcRect = nullptr;
	std::map<uint32, HyRectangle<int32> *>::const_iterator iter = m_ChecksumMap.find(uiChecksum);
	if(iter != m_ChecksumMap.end())
		pSrcRect = iter->second;

	if(pSrcRect)
	{
		UVRectOut.left = static_cast<float>(pSrcRect->left) / fTexWidth;
		UVRectOut.top = static_cast<float>(pSrcRect->top) / fTexHeight;
		UVRectOut.right = static_cast<float>(pSrcRect->right) / fTexWidth;
		UVRectOut.bottom = static_cast<float>(pSrcRect->bottom) / fTexHeight;

		return true;
	}

	return false;
}

void HyFileAtlas::DeletePixelData()
{
	SOIL_free_image_data(m_pPixelData);// stbi_image_free(m_pPixelData);
	m_pPixelData = nullptr;
	m_uiPixelDataSize = 0;
}

void HyFileAtlas::OnLoadThread()
{
	m_Mutex_PixelData.lock();

	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		if(m_pPixelData)
		{
			m_Mutex_PixelData.unlock();
			return;
		}

		char szTmpBuffer[16];
		std::string sAtlasFilePath = HyEngine::DataDir() + HYASSETS_AtlasDir;
		sprintf(szTmpBuffer, "%05d", m_uiBANK_ID);
		sAtlasFilePath += szTmpBuffer;
		sAtlasFilePath += "/";
		sAtlasFilePath += m_sFILE_NAME;

		if(sAtlasFilePath[sAtlasFilePath.size() - 1] == 'g')
		{
			int iWidth, iHeight, iNum8bitClrChannels; // out variables
			m_pPixelData = SOIL_load_image(sAtlasFilePath.c_str(), &iWidth, &iHeight, &iNum8bitClrChannels, 4);
			m_uiPixelDataSize = iWidth * iHeight * 4;
		}
		else
			m_pPixelData = SOIL_load_DDS(sAtlasFilePath.c_str(), &m_uiPixelDataSize, 0);

		// Use PBO/DMA transfer if available
		if(m_pGfxApiPixelBuffer)
		{
			memcpy(m_pGfxApiPixelBuffer, m_pPixelData, m_uiPixelDataSize);
		}

		HyAssert(m_pPixelData != nullptr, "HyFileAtlas failed to load image data");
	}

	m_Mutex_PixelData.unlock();
}

void HyFileAtlas::OnRenderThread(IHyRenderer &rendererRef)
{
	m_Mutex_PixelData.lock();
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		m_hTextureHandle = rendererRef.AddTexture(m_eTEXTURE_FORMAT, m_eTEXTURE_FILTERING, 0, m_uiWIDTH, m_uiHEIGHT, m_hGfxApiPbo, m_hGfxApiPbo != 0 ? nullptr : m_pPixelData, m_uiPixelDataSize, m_eTEXTURE_FORMAT);
		DeletePixelData();
	}
	else // GetLoadableState() == HYLOADSTATE_Discarded
	{
		rendererRef.DeleteTexture(m_hTextureHandle);
	}
	m_Mutex_PixelData.unlock();
}

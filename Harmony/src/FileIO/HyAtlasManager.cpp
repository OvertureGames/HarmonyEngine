/**************************************************************************
 *	HyAtlasManager.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FileIO/HyAtlasManager.h"

#include "FileIO/IHyFileIO.h"
#include "Renderer/IHyRenderer.h"

#include "Utilities/stb_image.h"

HyAtlasManager::HyAtlasManager(std::string sAtlasDataDir)
{
	m_sAtlasDirPath = sAtlasDataDir;

	jsonxx::Array atlasGroupArray;

	std::string sAtlasInfoFilePath(m_sAtlasDirPath);
	sAtlasInfoFilePath += "atlasInfo.json";
	atlasGroupArray.parse(IHyFileIO::ReadTextFile(sAtlasInfoFilePath.c_str()));

	m_uiNumAtlasGroups = static_cast<uint32>(atlasGroupArray.size());
	m_pAtlasGroups = reinterpret_cast<HyAtlasGroup *>(new unsigned char[sizeof(HyAtlasGroup) * m_uiNumAtlasGroups]);
	HyAtlasGroup *pAtlasGroupWriteLocation = m_pAtlasGroups;

	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i, ++pAtlasGroupWriteLocation)
	{
		jsonxx::Object atlasGroupObj = atlasGroupArray.get<jsonxx::Object>(i);

		new (pAtlasGroupWriteLocation)HyAtlasGroup(*this,
													static_cast<uint32>(atlasGroupObj.get<jsonxx::Number>("id")),
													static_cast<int32>(atlasGroupObj.get<jsonxx::Number>("width")),
													static_cast<int32>(atlasGroupObj.get<jsonxx::Number>("height")),
													static_cast<int32>(atlasGroupObj.get<jsonxx::Number>("num8BitClrChannels")),
													atlasGroupObj.get<jsonxx::Array>("textures"));
	}
}

HyAtlasManager::~HyAtlasManager()
{
	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i)
		m_pAtlasGroups->~HyAtlasGroup();

	delete m_pAtlasGroups;
}

HyAtlasGroup *HyAtlasManager::RequestTexture(uint32 uiTextureId)
{
	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i)
	{
		if(m_pAtlasGroups[i].ContainsTexture(uiTextureId))
		{
			m_pAtlasGroups[i].Load();
			return &m_pAtlasGroups[i];
		}
	}

	HyError("HyAtlasManager::RequestTexture() could not find the atlas group containing texture ID: " << uiTextureId);
	return &m_pAtlasGroups[0];
}

std::string HyAtlasManager::GetTexturePath(uint32 uiTextureId)
{
	std::string sTexturePath(m_sAtlasDirPath);

	char szTmpBuffer[16];
	sprintf(szTmpBuffer, "%05d", uiTextureId);
	
	sTexturePath += szTmpBuffer;
	sTexturePath += ".png";

	return sTexturePath;
}

//////////////////////////////////////////////////////////////////////////
HyAtlasGroup::HyAtlasGroup(HyAtlasManager &managerRef, uint32 uiLoadGroupId, uint32 uiWidth, uint32 uiHeight, uint32 uiNumClrChannels, jsonxx::Array &texturesArrayRef) :	m_ManagerRef(managerRef),
																																											m_uiLOADGROUPID(uiLoadGroupId),
																																											m_uiWIDTH(uiWidth),
																																											m_uiHEIGHT(uiHeight),
																																											m_uiNUM_8BIT_CHANNELS(uiNumClrChannels),
																																											m_uiNUM_ATLASES(static_cast<uint32>(texturesArrayRef.size())),
																																											m_uiGfxApiHandle(0)
{
	m_pAtlases = reinterpret_cast<HyAtlas *>(new unsigned char[sizeof(HyAtlas) * m_uiNUM_ATLASES]);
	HyAtlas *pAtlasWriteLocation = m_pAtlases;

	for(uint32 j = 0; j < m_uiNUM_ATLASES; ++j)
	{
		jsonxx::Object texObj = texturesArrayRef.get<jsonxx::Object>(j);

		uint32 uiTextureId = static_cast<uint32>(texObj.get<jsonxx::Number>("id"));
		jsonxx::Array srcFramesArray = texObj.get<jsonxx::Array>("srcFrames");

		new (pAtlasWriteLocation)HyAtlas(uiTextureId, srcFramesArray);
	}
}

HyAtlasGroup::~HyAtlasGroup()
{
	for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
		m_pAtlases->~HyAtlas();

	delete m_pAtlases;
}

uint32 HyAtlasGroup::GetGfxApiHandle()
{
	return m_uiGfxApiHandle;
}

uint32 HyAtlasGroup::GetNumColorChannels()
{
	return m_uiNUM_8BIT_CHANNELS;
}

uint32 HyAtlasGroup::GetWidth()
{
	return m_uiWIDTH;
}

uint32 HyAtlasGroup::GetHeight()
{
	return m_uiHEIGHT;
}

bool HyAtlasGroup::ContainsTexture(uint32 uiTextureId)
{
	for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
	{
		if(m_pAtlases[i].GetId() == uiTextureId)
			return true;
	}

	return false;
}

void HyAtlasGroup::Load()
{
	m_csTextures.Lock();

	if(m_uiGfxApiHandle == 0)
	{
		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			m_pAtlases[i].Load(m_ManagerRef.GetTexturePath(m_pAtlases[i].GetId()).c_str());
	}

	m_csTextures.Unlock();
}

// Returns 'true' if texture was just loaded
void HyAtlasGroup::Assign(IHyData *pData)
{
	m_csDataRefs.Lock();
	m_AssociatedDataSet.insert(pData);
	m_csDataRefs.Unlock();
}

void HyAtlasGroup::Relinquish(IHyData *pData)
{
	m_csDataRefs.Lock();

	for(set<IHyData *>::iterator iter = m_AssociatedDataSet.begin(); iter != m_AssociatedDataSet.end(); ++iter)
	{
		if((*iter) == pData)
		{
			m_AssociatedDataSet.erase(iter);
			break;
		}
	}

	m_csDataRefs.Unlock();
}

void HyAtlasGroup::OnRenderThread(IHyRenderer &rendererRef)
{
	bool bUpload;
	
	m_csDataRefs.Lock();
	bUpload = m_AssociatedDataSet.empty() == false;
	m_csDataRefs.Unlock();
	

	m_csTextures.Lock();
	if(bUpload)
	{
		vector<unsigned char *> vTextureArrayData;
		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			vTextureArrayData.push_back(m_pAtlases[i].GetPixelData());

		m_uiGfxApiHandle = rendererRef.AddTextureArray(m_uiNUM_8BIT_CHANNELS, m_uiWIDTH, m_uiHEIGHT, vTextureArrayData);

		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			m_pAtlases[i].DeletePixelData();
	}
	else
	{
		rendererRef.DeleteTextureArray(m_uiGfxApiHandle);
		m_uiGfxApiHandle = 0;
	}
	m_csTextures.Unlock();
}

//////////////////////////////////////////////////////////////////////////
HyAtlas::HyAtlas(uint32 uiTextureId, jsonxx::Array &srcFramesArrayRef) :	m_uiTEXTUREID(uiTextureId),
																			m_pPixelData(NULL)
{
	m_uiNumFrames = static_cast<uint32>(srcFramesArrayRef.size());
	m_pFrames = new HyRectangle<int32>[m_uiNumFrames];

	for(uint32 k = 0; k < m_uiNumFrames; ++k)
	{
		jsonxx::Object srcFrameObj = srcFramesArrayRef.get<jsonxx::Object>(k);

		m_pFrames[k].height = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("height"));
		m_pFrames[k].width = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("width"));
		m_pFrames[k].x = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("x"));
		m_pFrames[k].y = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("y"));
		m_pFrames[k].iTag = srcFrameObj.get<jsonxx::Boolean>("rotated") ? 1 : 0;
	}
}

HyAtlas::~HyAtlas()
{
	delete [] m_pFrames;
	DeletePixelData();
}

uint32 HyAtlas::GetId()
{
	return m_uiTEXTUREID;
}

void HyAtlas::Load(const char *szFilePath)
{
	if(m_pPixelData)
		return;

	int iWidth, iHeight, iNum8bitClrChannels;
	m_pPixelData = stbi_load(szFilePath, &iWidth, &iHeight, &iNum8bitClrChannels, 0);

	HyAssert(m_pPixelData != NULL, "HyAtlas failed to load image data");
}

unsigned char *HyAtlas::GetPixelData()
{
	return m_pPixelData;
}

void HyAtlas::DeletePixelData()
{
	stbi_image_free(m_pPixelData);
	m_pPixelData = NULL;
}
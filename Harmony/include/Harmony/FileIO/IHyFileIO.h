/**************************************************************************
 *	IHyFileIO.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyFileIO_h__
#define __IHyFileIO_h__

#include "Afx/HyStdAfx.h"

#include "FileIO/HyFactory.h"
#include "FileIO/HyAtlasManager.h"

#include "Renderer/HyGfxComms.h"
#include "Scene/HyScene.h"

#include "Threading/Threading.h"

#include <queue>
using std::queue;

class IHyInst2d;

class IHyData;
class IHyData2d;
class HySfxData;
class HySprite2dData;
class HySpine2dData;
class HyText2dData;
class HyTexturedQuad2dData;
class HyMesh3dData;

class IHyFileIO
{
	const std::string									m_sDATADIR;

	HyGfxComms &										m_GfxCommsRef;
	HyScene &											m_SceneRef;

	// Thread control structure to help sync loading of factory data
	struct LoadThreadCtrl
	{
		queue<IHyData *> &	m_LoadQueueRef_Shared;
		queue<IHyData *> &	m_LoadQueueRef_Retrieval;

		WaitEvent		m_WaitEvent_HasNewData;
		BasicSection	m_csSharedQueue;
		BasicSection	m_csRetrievalQueue;

		LoadThreadCtrl(queue<IHyData *> &LoadQueueRef_Shared, queue<IHyData *> &LoadQueueRef_Retrieval) :	m_LoadQueueRef_Shared(LoadQueueRef_Shared),
																											m_LoadQueueRef_Retrieval(LoadQueueRef_Retrieval),
																											m_WaitEvent_HasNewData(L"Thread Idler", true)
		{}
	};
	LoadThreadCtrl										m_LoadingCtrl;

	vector<IHyInst2d *>									m_vQueuedInst2d;

	HyFactory<HySfxData>								m_Sfx;
	HyFactory<HySprite2dData>							m_Sprite2d;
	HyFactory<HySpine2dData>							m_Spine2d;
	HyFactory<HyMesh3dData>								m_Mesh3d;
	HyFactory<HyText2dData>								m_Txt2d;
	HyFactory<HyTexturedQuad2dData>						m_Quad2d;

	HyAtlasManager										m_AtlasManager;

	// Queues responsible for passing and retrieving factory data between the loading thread
	queue<IHyData *>									m_LoadQueue_Prepare;
	queue<IHyData *>									m_LoadQueue_Shared;
	queue<IHyData *>									m_LoadQueue_Retrieval;

	queue<IHyData2d *> *								m_pGfxQueue_Retrieval;

	// Loading thread info pointer
	ThreadInfoPtr										m_pLoadingThread;


public:
	IHyFileIO(const char *szDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef);
	virtual ~IHyFileIO();

	void Update();

	void LoadInst2d(IHyInst2d *pInst);
	void RemoveInst(IHyInst2d *pInst);

	static char *ReadTextFile(const char *szFilePath, int *iLength);
	static std::string ReadTextFile(const char *szFilePath);
	static bool FileExists(const std::string &sFilePath);

private:
	void FinalizeData(IHyData *pData);
	void DiscardData(IHyData *pData);

	static void LoadingThread(void *pParam);
};

#endif /* __IHyFileIO_h__ */
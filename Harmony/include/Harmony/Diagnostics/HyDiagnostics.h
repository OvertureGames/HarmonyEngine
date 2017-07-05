/**************************************************************************
*	HyDiagnostics.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyDiagnostics_h__
#define HyDiagnostics_h__

#include "Afx/HyStdAfx.h"
#include "Time/IHyTime.h"

struct HarmonyInit;
class HyAssets;
class HyScene;
class HyText2d;

class HyDiagnostics
{
	friend class IHyRenderer;
	friend class IHyTime;

	HarmonyInit &		m_InitStructRef;
	HyAssets &			m_AssetsRef;
	HyScene &			m_SceneRef;

	std::string			m_sPlatform;
	uint32				m_uiNumCpuCores;
	uint64				m_uiTotalMemBytes;
	uint64				m_uiVirtualMemBytes;

	std::string			m_sGfxApi;
	std::string			m_sVersion;
	std::string			m_sVendor;
	std::string			m_sRenderer;
	std::string			m_sShader;
	std::string			m_sCompressedTextures;

	bool				m_bInitialMemCheckpointSet;
#if defined(HY_DEBUG) && defined(HY_COMPILER_MSVC) && defined(HY_PLATFORM_WINDOWS)
	_CrtMemState		m_MemCheckpoint1;
	_CrtMemState		m_MemCheckpoint2;
#endif

	uint32				m_uiFps_Update;
	uint32				m_uiFps_Render;
	HyText2d *			m_pOnScreenText;

public:
	HyDiagnostics(HarmonyInit &initStruct, HyAssets &assetsRef, HyScene &sceneRef);
	~HyDiagnostics();

	void InitOnScreenText(const char *szTextPrefix, const char *szTextName);

	void BootMessage();
	
	void ShowFps(bool bShowOnScreen, bool bShowConsole);
	bool IsShowFps();

	void DumpAtlasUsage();
	void DumpNodeUsage();
	void DumpMemoryUsage();

	void StartMemoryCheckpoint();
	void EndMemoryCheckpoint();

private:
	void SetRendererInfo(const std::string &sApi, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, const std::string &sCompressedTextures);

	void SetCurrentFps(uint32 uiFps_Update, uint32 uiFps_Render);
};

#endif /* HyDiagnostics_h__ */

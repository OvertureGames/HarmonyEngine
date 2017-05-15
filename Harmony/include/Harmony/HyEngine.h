/**************************************************************************
 *	HyEngine.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEngine_h__
#define __HyEngine_h__

#include "Afx/HyStdAfx.h"
#include "Afx/HyInteropAfx.h"
#include "IHyApplication.h"
#include "Memory/HyMemoryHeap.h"
#include "Audio/IHyAudio.h"
#include "Diagnostics/HyDiagnostics.h"
#include "Diagnostics/HyGuiComms.h"
#include "Renderer/Components/HyWindow.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Leafs/Misc/HyAudio2d.h"
#include "Scene/Nodes/Leafs/Draws/HySprite2d.h"
#include "Scene/Nodes/Leafs/Draws/HySpine2d.h"
#include "Scene/Nodes/Leafs/Draws/HyPrimitive2d.h"
#include "Scene/Nodes/Leafs/Draws/HyText2d.h"
#include "Scene/Nodes/Leafs/Draws/HyTexturedQuad2d.h"
#include "Scene/Nodes/Entities/HyEntityLeaf2d.h"
#include "Profiler/HyProfileManager.h"
#include "Assets/HyAssets.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyFileIO.h"
#include "Utilities/HyStrManip.h"

class HyEngine
{
	friend class HyGuiRenderer;

	static HyEngine *		sm_pInstance;

	// The order of these member declarations matter
	IHyApplication &		m_AppRef;
	
	HyDiagnostics			m_Diagnostics;

	HyGfxComms				m_GfxBuffer;
	HyScene					m_Scene;
	HyAssets 				m_Assets;
	HyGuiComms				m_GuiComms;

	HyTimeInterop			m_Time;
	HyInputInterop			m_Input;
	HyRendererInterop		m_Renderer;
	HyAudioInterop			m_Audio;

// If HY_PLATFORM_GUI, make this ctor public as gui tool requires special usage.
#ifdef HY_PLATFORM_GUI
public:
#endif
	// Otherwise, private ctor invoked from RunGame(), once.
	HyEngine(IHyApplication &gameRef);

public:
	~HyEngine();

	static void RunGame(IHyApplication &gameRef);

#ifndef HY_PLATFORM_GUI
private:
#endif
	bool BootUpdate();
	bool Update();

	void Shutdown();

	HyRendererInterop &GetRenderer();

	friend void HyPauseGame(bool bPause);
	friend float HyPixelsPerMeter();
	friend HyCoordinateType HyDefaultCoordinateType();
	friend HyCoordinateUnit HyDefaultCoordinateUnit();
};

#endif /* __HyEngine_h__ */

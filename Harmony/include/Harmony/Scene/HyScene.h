/**************************************************************************
 *	HyScene.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyScene_h__
#define __HyScene_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Physics/HyBox2dRuntime.h"
#include "Scene/Physics/HyDebugPhys2d.h"

#include <vector>

// Forward declarations
class IHyNode;
class IHyNode2d;
class IHyLeafDraw2d;
class IHyNode3d;

class HySound;
class HySpine2d;
class HyMesh3d;
class HyText2d;
class HyPrimitive2d;

class HyGfxComms;
class HyPhysics2d;
class HyTweenVec2;
class HyTweenVec3;

class HyWindow;
class HyEntity2d;
class HyTweenFloat;

//////////////////////////////////////////////////////////////////////////
class HyScene
{
	friend class HyEngine;

	static bool											sm_bInst2dOrderingDirty;

	b2World												m_b2World;
	int32												m_iPhysVelocityIterations;
	int32												m_iPhysPositionIterations;
	HyDebugPhys2d										m_DrawPhys2d;
	HyBox2dRuntime										m_Phys2dContactListener;

	HyGfxComms &										m_GfxCommsRef;
	std::vector<HyWindow *> &							m_WindowListRef;

	static std::vector<IHyNode *>						sm_NodeList;				// Master list of all nodes

	static std::vector<IHyNode *>						sm_NodeList_PauseUpdate;	// List of nodes who will update when the game is paused
	bool												m_bPauseGame;

	std::vector<IHyLeafDraw2d *>						m_NodeList_Loaded;			// List of nodes who can be drawn, and their graphics assets are fully loaded
	std::vector<IHyNode3d *>							m_LoadedInst3dList;	// TODO: rename this

	static uint32										sm_uiRenderedBufferCount;	// Increments every time a buffered is marked 'GFXFLAG_HasRendered' to be used performance diagnostics

	// Used when writing the graphics draw buffer
	char *												m_pCurWritePos;

public:
	HyScene(HyGfxComms &gfxCommsRef, std::vector<HyWindow *> &WindowListRef);
	~HyScene(void);

	static void SetInstOrderingDirty()					{ sm_bInst2dOrderingDirty = true; }
	
	static void AddNode(IHyNode *pNode);
	static void RemoveNode(IHyNode *pNode);

	static void AddNode_PauseUpdate(IHyNode *pNode);
	static void RemoveNode_PauseUpdate(IHyNode *pNode);

	void AddNode_Loaded(IHyLeafDraw2d *pInst);
	void RemoveNode_Loaded(IHyLeafDraw2d *pInst);

	void CopyAllLoadedNodes(std::vector<IHyLeafDraw2d *> &vInstsToCopy);

	void DebugDrawPhysics2d(bool bDraw)					{ m_DrawPhys2d.SetDrawEnabled(bDraw); }

	void SetPause(bool bPause);

	static uint32 GetAndClearRenderedBufferCount();

private:
	void PreUpdate();
	void PostUpdate();
	
	void WriteDrawBuffer();
	
	static bool Node2dSortPredicate(const IHyLeafDraw2d *pInst1, const IHyLeafDraw2d *pInst2);
};

#endif /* __HyScene_h__ */


/**************************************************************************
 *	IHyRenderer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyRenderer_h__
#define __IHyRenderer_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/HyGfxComms.h"
#include "Renderer/HyRenderState.h"

class HyWindow;

class IHyRenderer
{
protected:
	HyGfxComms &				m_GfxCommsRef;
	vector<HyWindow> &		m_ViewportsRef;

	queue<IHyData2d *> *		m_pMsgQueuePtr;		// The pointer to the currently active render message queue
	queue<IHyData2d *> *		m_pSendMsgQueuePtr;	// The pointer to the currently active render message queue
	char *						m_pDrawBufferPtr;	// The pointer to the currently active draw buffer

	HyGfxComms::tDrawHeader *	m_DrawpBufferHeader;
	HyRenderState *				m_pCurRenderState;
	HyRenderState				m_PrevRenderState;

public:
	IHyRenderer(HyGfxComms &gfxCommsRef, vector<HyWindow> &viewportsRef);
	virtual ~IHyRenderer(void);

	virtual void StartRender() = 0;

	virtual bool Begin_3d() = 0;
	virtual void SetRenderState_3d(uint32 uiNewRenderState) = 0;
	virtual void End_3d() = 0;

	virtual bool Begin_2d() = 0;
	virtual void DrawRenderState_2d(HyRenderState &renderState) = 0;
	virtual void End_2d() = 0;

	virtual void FinishRender() = 0;

	// Returns the texture ID used for API specific drawing.
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, vector<unsigned char *> &vPixelData) = 0;
	virtual void DeleteTextureArray(uint32 uiTextureHandle) = 0;

	int32 GetNumCameras2d()									{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToCameras2d)); }
	HyRectangle<float> *GetCameraViewportRect2d(int iIndex)	{ return reinterpret_cast<HyRectangle<float> *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToCameras2d + sizeof(int32) + (iIndex * (sizeof(HyRectangle<float>) + sizeof(mat4)))); }
	mat4 *GetCameraView2d(int iIndex)						{ return reinterpret_cast<mat4 *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToCameras2d + sizeof(int32) + (iIndex * (sizeof(HyRectangle<float>) + sizeof(mat4))) + sizeof(HyRectangle<float>)); }

	int32 GetNumInsts3d()									{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToCameras3d)); }
	int32 GetNumCameras3d()									{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToCameras3d)); }

	int32 GetNumRenderStates2d()							{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToInst2d)); }
	HyRenderState *GetRenderStatesPtr2d()					{ return reinterpret_cast<HyRenderState *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToInst2d + sizeof(int32)); } // Last sizeof(int32) is skipping number of 2dInsts
	char *GetVertexData2d()									{ return reinterpret_cast<char *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToVertexData2d); }

	void Update();
	void Draw2d();
};

#endif /* __IHyRenderer_h__ */

/**************************************************************************
 *	IHyRenderer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/IHyRenderer.h"

IHyRenderer::IHyRenderer(HyGfxComms &gfxCommsRef, vector<HyViewport> &viewportsRef) : m_GfxCommsRef(gfxCommsRef),
																					m_ViewportsRef(viewportsRef)
{
}

IHyRenderer::~IHyRenderer(void)
{
}

void IHyRenderer::Update()
{
	// Swap to newest draw buffers (is only threadsafe on Render thread)
	if(!m_GfxCommsRef.Render_GetSharedPtrs(m_pMsgQueuePtr, m_pSendMsgQueuePtr, m_pDrawBufferPtr))
	{
		InteropSleep(10);
		return;
	}
	m_DrawpBufferHeader = reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBufferPtr);

	ProcessGameMsgs();

	StartRender();

	while(Begin_3d())
	{
		//Draw3d();
		End_3d();
	}

	while(Begin_2d())
	{
		Draw2d();
		End_2d();
	}

	FinishRender();

	reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBufferPtr)->uiReturnFlags |= GFXFLAG_HasRendered;
}

void IHyRenderer::ProcessGameMsgs()
{
	// Handle each command message first. Which loads/unloads gfx resources.
	while(!m_pMsgQueuePtr->empty())
	{
		IData *pData = m_pMsgQueuePtr->front();
		m_pMsgQueuePtr->pop();

		if(pData->GetLoadState() == HYLOADSTATE_Queued)
			pData->OnGfxLoad(*this);
		else
			pData->OnGfxRemove(*this);

		m_pSendMsgQueuePtr->push(pData);
	}
}

void IHyRenderer::Draw2d()
{
	// Each render state will require its own draw. The order of these render states should be 
	// depth sorted with render states batched together to reduce state changes.
	m_pCurRenderState = GetRenderStatesPtr2d();
	memset(&m_PrevRenderState, 0, sizeof(HyRenderState));

	int32 iNumRenderStates = GetNumRenderStates2d();
	for(int32 i = 0; i < iNumRenderStates; ++i, ++m_pCurRenderState)
	{
		DrawRenderState_2d(*m_pCurRenderState);
		m_PrevRenderState = *m_pCurRenderState;
	}
}
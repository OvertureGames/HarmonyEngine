/**************************************************************************
 *	HyScene.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/HyScene.h"
#include "HyEngine.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Components/HyWindow.h"
#include "Scene/Nodes/Leafs/Draws/HySprite2d.h"
#include "Scene/Nodes/Leafs/Draws/HySpine2d.h"
#include "Scene/Nodes/Leafs/Draws/HyPrimitive2d.h"
#include "Scene/Nodes/Leafs/Draws/HyText2d.h"
#include "Scene/Nodes/Leafs/Draws/HyTexturedQuad2d.h"
#include "Scene/Physics/HyPhysEntity2d.h"

bool HyScene::sm_bInst2dOrderingDirty = false;
std::vector<IHyNode *> HyScene::sm_MasterNodeList;
std::vector<IHyNode *> HyScene::sm_NodeList_PauseUpdate;

HyScene::HyScene(std::vector<HyWindow *> &WindowListRef) :	m_b2World(b2Vec2(0.0f, -10.0f)),
															m_iPhysVelocityIterations(8),
															m_iPhysPositionIterations(3),
															m_WindowListRef(WindowListRef),
															m_bPauseGame(false)
{
	m_b2World.SetDebugDraw(&m_DrawPhys2d);
	m_b2World.SetContactListener(&m_Phys2dContactListener);
	
	m_DrawPhys2d.SetFlags(b2Draw::e_shapeBit);// | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);

	// Link HyScene to all classes that access it
	HyPhysEntity2d::sm_b2WorldRef = &m_b2World;
}

HyScene::~HyScene(void)
{
	IHyLeafDraw2d::sm_pHyAssets = nullptr;
}

/*static*/ void HyScene::AddNode(IHyNode *pNode)
{
	sm_MasterNodeList.push_back(pNode);
}

/*static*/ void HyScene::RemoveNode(IHyNode *pNode)
{
	for(auto it = sm_MasterNodeList.begin(); it != sm_MasterNodeList.end(); ++it)
	{
		if((*it) == pNode)
		{
			// TODO: Log about erasing Node
			sm_MasterNodeList.erase(it);
			break;
		}
	}
}

/*static*/ void HyScene::AddNode_PauseUpdate(IHyNode *pNode)
{
	sm_NodeList_PauseUpdate.push_back(pNode);
}

/*static*/ void HyScene::RemoveNode_PauseUpdate(IHyNode *pNode)
{
	for(auto it = sm_NodeList_PauseUpdate.begin(); it != sm_NodeList_PauseUpdate.end(); ++it)
	{
		if((*it) == pNode)
		{
			// TODO: Log about erasing Node
			sm_NodeList_PauseUpdate.erase(it);
			break;
		}
	}
}

void HyScene::AddNode_Loaded(IHyLeafDraw2d *pInst)
{
	m_NodeList_Loaded.push_back(pInst);
	sm_bInst2dOrderingDirty = true;
}

void HyScene::RemoveNode_Loaded(IHyLeafDraw2d *pInst)
{
	for(auto it = m_NodeList_Loaded.begin(); it != m_NodeList_Loaded.end(); ++it)
	{
		if((*it) == pInst)
		{
			// TODO: Log about erasing instance
			m_NodeList_Loaded.erase(it);
			break;
		}
	}
}

void HyScene::CopyAllLoadedNodes(std::vector<IHyLeafDraw2d *> &nodeListOut)
{
	nodeListOut = m_NodeList_Loaded;
}

b2World &HyScene::GetPhysics2d()
{
	return m_b2World;
}

void HyScene::SetPause(bool bPause)
{
	m_bPauseGame = bPause;
}

//PRIVATE//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HyScene::UpdatePhysics()
{
	HY_PROFILE_BEGIN("Physics")
		m_b2World.Step(HyUpdateDelta(), m_iPhysVelocityIterations, m_iPhysPositionIterations);
	HY_PROFILE_END
}

void HyScene::UpdateNodes()
{
	HY_PROFILE_BEGIN("Nodes")
	if(m_bPauseGame == false)
	{
		for(uint32 i = 0; i < sm_MasterNodeList.size(); ++i)
			sm_MasterNodeList[i]->Update();
	}
	else
	{
		for(uint32 i = 0; i < sm_NodeList_PauseUpdate.size(); ++i)
			sm_NodeList_PauseUpdate[i]->Update();
	}
	HY_PROFILE_END
}

void HyScene::PrepareRender(IHyRenderer &rendererRef)
{
	HY_PROFILE_BEGIN("PrepareRender")
	if(sm_bInst2dOrderingDirty)
	{
		std::sort(m_NodeList_Loaded.begin(), m_NodeList_Loaded.end(), &Node2dSortPredicate);
		sm_bInst2dOrderingDirty = false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// BUFFER HEADER (contains offsets from here)-| Num 3d Cams (4bytes)-|-Cam3d-|-Cam3d-|...|-Num 2d Cams (4bytes)-|-Cam2d-|-Cam2d-|...|-Num 3d Insts (4bytes)-|-Inst3d-|-Inst3d-|-Inst3d...-|-Num 2d Insts (4bytes)-|-Inst2d-|-Inst2d-|-Inst2d...-|-<possible blank/empty data since we skip non-enabled instances>-|-RenderState info (Texture Binds, Uniform Data)-|-Vertex Data-|-RenderState info (Texture Binds, Uniform Data)-|-Vertex Data...
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// TODO: should I ensure that I start all writes on a 4byte boundary? ARM systems may be an issue

	// GET BUFFER HEADER (and write to its members as data offsets become known below)
	m_pCurWritePos = rendererRef.GetDrawBuffer();

	IHyRenderer::DrawBufferHeader *pDrawHeader = new (m_pCurWritePos)IHyRenderer::DrawBufferHeader;

	pDrawHeader->uiReturnFlags = 0;
	m_pCurWritePos += sizeof(IHyRenderer::DrawBufferHeader);

	glm::mat4 mtxView;
	uint32 uiNumWindows = static_cast<uint32>(m_WindowListRef.size());
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d CAMERA(S) BUFFER
	pDrawHeader->uiOffsetToCameras3d = m_pCurWritePos - rendererRef.GetDrawBuffer();
	char *pWriteNum3dCamsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	int32 iCount = 0;
	for(uint32 i = 0; i < uiNumWindows; ++i)
	{
		uint32 uiNumCameras3d = static_cast<uint32>(m_WindowListRef[i]->m_Cams3dList.size());
		for(uint32 j = 0; j < uiNumCameras3d; ++j)
		{
			if(m_WindowListRef[i]->m_Cams3dList[j]->IsEnabled())
			{
				*(reinterpret_cast<uint32 *>(m_pCurWritePos)) = i;
				m_pCurWritePos += sizeof(uint32);

				*(reinterpret_cast<HyRectangle<float> *>(m_pCurWritePos)) = m_WindowListRef[i]->m_Cams3dList[j]->GetViewport();
				m_pCurWritePos += sizeof(HyRectangle<float>);

				HyError("GetLocalTransform should be 3d");
				m_WindowListRef[i]->m_Cams3dList[j]->GetLocalTransform(mtxView);
				*(reinterpret_cast<glm::mat4 *>(m_pCurWritePos)) = mtxView;
				m_pCurWritePos += sizeof(glm::mat4);

				iCount++;
			}
		}
	}
	*(reinterpret_cast<uint32 *>(pWriteNum3dCamsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d CAMERA(S) BUFFER
	pDrawHeader->uiOffsetToCameras2d = m_pCurWritePos - rendererRef.GetDrawBuffer();
	char *pWriteNum2dCamsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	for(uint32 i = 0; i < uiNumWindows; ++i)
	{
		uint32 uiNumCameras2d = static_cast<uint32>(m_WindowListRef[i]->m_Cams2dList.size());
		for(uint32 j = 0; j < uiNumCameras2d; ++j)
		{
			if(m_WindowListRef[i]->m_Cams2dList[j]->IsEnabled())
			{
				*(reinterpret_cast<uint32 *>(m_pCurWritePos)) = i;
				m_pCurWritePos += sizeof(uint32);

				*(reinterpret_cast<HyRectangle<float> *>(m_pCurWritePos)) = m_WindowListRef[i]->m_Cams2dList[j]->GetViewport();
				m_pCurWritePos += sizeof(HyRectangle<float>);

				m_WindowListRef[i]->m_Cams2dList[j]->GetLocalTransform(mtxView);

				// Reversing X and Y because it's more intuitive (or I'm not multiplying the matrices correctly above or in the shader)
				mtxView[3].x *= -1;
				mtxView[3].y *= -1;

				*(reinterpret_cast<glm::mat4 *>(m_pCurWritePos)) = mtxView;
				m_pCurWritePos += sizeof(glm::mat4);

				iCount++;
			}
		}
	}
	*(reinterpret_cast<uint32 *>(pWriteNum2dCamsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d DRAW BUFFER
	pDrawHeader->uiOffsetToInst3d = m_pCurWritePos - rendererRef.GetDrawBuffer();
	char *pWriteNum3dInstsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	uint32 uiTotalNumInsts = static_cast<uint32>(m_LoadedInst3dList.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_LoadedInst3dList[i]->IsEnabled())
		{
			// TODO: 
			//new (m_pCurWritePos) HyDrawText2d(reinterpret_cast<HyText2d *>(m_NodeList_Loaded[i]), uiVertexDataOffset, pCurVertexWritePos);
			//m_pCurWritePos += sizeof(HyDrawText2d);
			iCount++;
		}
	}
	*(reinterpret_cast<uint32 *>(pWriteNum3dInstsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d DRAW BUFFER
	pDrawHeader->uiOffsetToInst2d = m_pCurWritePos - rendererRef.GetDrawBuffer();
	char *pWriteNum2dInstsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	uiTotalNumInsts = static_cast<uint32>(m_NodeList_Loaded.size());

	char *pStartVertexWritePos = m_pCurWritePos + (uiTotalNumInsts * sizeof(HyRenderState));
	pDrawHeader->uiOffsetToVertexData2d = pStartVertexWritePos - rendererRef.GetDrawBuffer();
	char *pCurVertexWritePos = pStartVertexWritePos;

	size_t	uiVertexDataOffset = 0;
	HyRenderState *pCurRenderState2d = nullptr;

	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_NodeList_Loaded[i]->IsEnabled() == false)// || m_NodeList_Loaded[i]->GetType() == HYTYPE_Primitive2d)
			continue;

		// If previously written instance has equal render state by "operator ==" then it's to be assumed the instance data can be batched and doesn't need to write another render state
		if(pCurRenderState2d == nullptr ||
			m_NodeList_Loaded[i]->GetRenderState() != *pCurRenderState2d ||
			m_NodeList_Loaded[i]->GetRenderState().IsEnabled(HyRenderState::DRAWINSTANCED) == false)
		{
			// Start a new draw. Write render state to buffer to be sent to render thread
			memcpy(m_pCurWritePos, &m_NodeList_Loaded[i]->GetRenderState(), sizeof(HyRenderState));
			pCurRenderState2d = reinterpret_cast<HyRenderState *>(m_pCurWritePos);
			pCurRenderState2d->SetDataOffset(uiVertexDataOffset);

			// This function is responsible for incrementing the draw pointer to after what's written
			m_NodeList_Loaded[i]->WriteShaderUniformBuffer(pCurVertexWritePos);

			m_pCurWritePos += sizeof(HyRenderState);
			iCount++;
		}
		else
		{
			// This instance will be batched with the current render state
			pCurRenderState2d->AppendInstances(m_NodeList_Loaded[i]->GetRenderState().GetNumInstances());
		}

		// OnWriteDrawBufferData() is responsible for incrementing the draw pointer to after what's written
		m_NodeList_Loaded[i]->OnWriteDrawBufferData(pCurVertexWritePos);
		uiVertexDataOffset = pCurVertexWritePos - pStartVertexWritePos;
	}

	*(reinterpret_cast<uint32 *>(pWriteNum2dInstsHere)) = iCount;
	pDrawHeader->uiVertexBufferSize2d = pCurVertexWritePos - pStartVertexWritePos;

	// Do final check to see if we wrote passed our bounds
	HyAssert(pDrawHeader->uiVertexBufferSize2d < HY_GFX_BUFFER_SIZE, "HyScene::WriteUpdateBuffer() has written passed its bounds! Embiggen 'HY_GFX_BUFFER_SIZE'");

	HY_PROFILE_END
}

/*static*/ bool HyScene::Node2dSortPredicate(const IHyLeafDraw2d *pInst1, const IHyLeafDraw2d *pInst2)
{
	if(pInst1->GetDisplayOrder() == pInst2->GetDisplayOrder())
	{
		if(pInst1->GetRenderState() == pInst2->GetRenderState())
			return reinterpret_cast<const char *>(pInst1) < reinterpret_cast<const char *>(pInst2);

		return pInst1->GetRenderState() < pInst2->GetRenderState();
	}

	return pInst1->GetDisplayOrder() < pInst2->GetDisplayOrder();
}


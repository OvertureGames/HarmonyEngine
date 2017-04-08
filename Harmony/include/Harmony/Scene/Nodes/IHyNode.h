/**************************************************************************
*	IHyNode.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyTransformNode_h__
#define __IHyTransformNode_h__

#include "Afx/HyStdAfx.h"
#include "Scene/HyScene.h"

#include <functional>

class IHyNode
{
	friend class HyScene;
	friend class HyTweenFloat;

protected:
	const HyType					m_eTYPE;

	bool							m_bDirty;
	bool							m_bIsDraw2d;
	bool							m_bEnabled;
	bool							m_bPauseOverride;	// Will continue to Update when game is paused

	IHyNode *						m_pParent;
	std::vector<IHyNode *>			m_ChildList;


	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

	std::vector<HyTweenFloat *>		m_ActiveAnimFloatsList;

public:
	IHyNode(HyType eInstType, IHyNode *pParent = nullptr);
	virtual ~IHyNode();
	
	HyType GetType();
	bool IsDraw2d();

	bool IsEnabled();
	virtual void SetEnabled(bool bEnabled);

	// Sets whether to Update when game is paused
	void SetPauseOverride(bool bPauseOverride);

	int64 GetTag();
	void SetTag(int64 iTag);

	void ChildAppend(IHyNode &childInst);
	bool ChildInsert(IHyNode &insertBefore, IHyNode &childInst);
	bool ChildFind(IHyNode &childInst);
	void ChildrenTransfer(IHyNode &newParent);
	uint32 ChildCount();
	IHyNode *ChildGet(uint32 uiIndex);

	void ParentDetach();
	bool ParentExists();

	void ForEachChild(std::function<void(IHyNode *)> func);

protected:
	void Update();
	virtual void InstUpdate() = 0;

	void SetDirty();
	void InsertActiveAnimFloat(HyTweenFloat *pAnimFloat);
};

#endif /* __IHyTransformNode_h__ */

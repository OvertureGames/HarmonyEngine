/**************************************************************************
*	IHyNode.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyNode_h__
#define IHyNode_h__

#include "Afx/HyStdAfx.h"

class HyTweenFloat;

class IHyNode
{
	friend class HyScene;
	friend class HyEntity2d;
	friend class HyTweenFloat;
	friend class IHyVisable;

protected:
	const HyType					m_eTYPE;

	enum NodeDirtyFlag
	{
		DIRTY_BoundingVolume		= 1 << 0,
		DIRTY_WorldAABB				= 1 << 1,
		DIRTY_Transform				= 1 << 2,
		DIRTY_Color					= 1 << 3,
		DIRTY_Scissor				= 1 << 4,

		DIRTY_ALL =					DIRTY_BoundingVolume | DIRTY_WorldAABB | DIRTY_Transform | DIRTY_Color | DIRTY_Scissor
	};
	uint32							m_uiDirtyFlags;

	// When directly manipulating a node, store a flag to indicate that this attribute has been explicitly set. If later 
	// changes occur to a parent of this node, it may optionally ignore the change when it propagates down the child hierarchy.
	enum ExplicitFlags
	{
		EXPLICIT_Enabled			= 1 << 0,
		EXPLICIT_PauseUpdate		= 1 << 1,
		EXPLICIT_Scissor			= 1 << 2,
		EXPLICIT_Stencil			= 1 << 3,
		EXPLICIT_DisplayOrder		= 1 << 4,
		EXPLICIT_CoordinateSystem	= 1 << 5,
	};
	enum NodeTypeFlags // NOTE: continue the bits in ExplicitFlags (stored in same 32bit member)
	{
		NODETYPE_IsLoadable			= 1 << 6,
		NODETYPE_IsDrawable			= 1 << 7,
		NODETYPE_IsVisable			= 1 << 8,
	};
	uint32							m_uiExplicitAndTypeFlags;

	std::vector<HyTweenFloat *>		m_ActiveTweenFloatsList;

	bool							m_bEnabled;
	bool							m_bPauseOverride;	// Will continue to Update when game is paused

	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

	// Don't allow move semantics since pointers to these nodes are stored in things like HyScene, and those pointers would become invalid
	IHyNode(IHyNode &&moveRef) HY_NOEXCEPT = delete;
	IHyNode &operator=(IHyNode &&moveRef) HY_NOEXCEPT = delete;

public:
	IHyNode(HyType eNodeType);
	IHyNode(const IHyNode &copyRef);
	virtual ~IHyNode();

	const IHyNode &operator=(const IHyNode &rhs);

	HyType GetType() const;

	bool IsEnabled() const;
	virtual void SetEnabled(bool bEnabled);

	virtual void SetPauseUpdate(bool bUpdateWhenPaused);

	int64 GetTag() const;
	void SetTag(int64 iTag);

protected:
	virtual void PhysicsUpdate() = 0;
	virtual void NodeUpdate() = 0;
	
	virtual void _SetEnabled(bool bEnabled, bool bIsOverriding);						// Only Entity2d/3d will invoke this
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding);			// Only Entity2d/3d will invoke this

	virtual void SetDirty(uint32 uiDirtyFlags);
	bool IsDirty(NodeDirtyFlag eDirtyType);
	void ClearDirty(NodeDirtyFlag eDirtyType);

private:
	void InsertActiveTweenFloat(HyTweenFloat *pTweenFloat);								// Only HyTweenFloat will invoke this
	void Update();																		// Only Scene will invoke this
};

#endif /* IHyNode_h__ */

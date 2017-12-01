/**************************************************************************
*	HyStencil.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyStencil_h__
#define HyStencil_h__

#include "Afx/HyStdAfx.h"

class HyRenderState;
class IHyDrawInst2d;

enum HyStencilBehavior
{
	HYSTENCILBEHAVIOR_Mask = 0,
	HYSTENCILBEHAVIOR_InvertedMask
};

class HyStencil
{
	friend class IHyRenderer;

	static HyStencilHandle			sm_hHandleCount;
	const HyStencilHandle			m_hHANDLE;

	std::vector<IHyDrawInst2d *>	m_InstanceList;
	HyRenderState *					m_pRenderStatePtr;

	HyStencilBehavior				m_eBehavior;

public:
	HyStencil();
private: ~HyStencil();
public:
	void Destroy();

	HyStencilHandle GetHandle() const;

	// It's the user's responsibility to ensure added instances continue to be valid
	void AddInstance(IHyDrawInst2d *pInstance);
	bool RemoveInstance(IHyDrawInst2d *pInstance);

	HyStencilBehavior GetBehavior() const;

	void SetAsMask();
	void SetAsInvertedMask();

	const std::vector<IHyDrawInst2d *> &GetInstanceList() const;
	HyRenderState *GetRenderStatePtr() const;

private:
	void SetRenderStatePtr(HyRenderState *pPtr);
};

#endif /* HyStencil_h__ */
/**************************************************************************
*	IHyDrawInst2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLeafDraw2d_h__
#define IHyLeafDraw2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Draws/IHyDraw2d.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Renderer/Effects/HyShader.h"
#include "Renderer/Components/HyShaderUniforms.h"

class HyStencil;
class HyPortal2d;

// NOTE: This class should contain a copy of all the functions/members of IHyLeaf2d. Multiple inheritance is not an option
class IHyDrawInst2d : public IHyDraw2d
{
	friend class IHyRenderer;
	friend class HyAssets;
	friend class HyShape2d;

protected:
	static HyAssets *				sm_pHyAssets;

	HyLoadState						m_eLoadState;

	IHyNodeData *					m_pData;
	const std::string				m_sNAME;
	const std::string				m_sPREFIX;

	HyShaderHandle					m_hShader;
	HyRenderMode					m_eRenderMode;
	HyTextureHandle					m_hTextureHandle;
	HyShaderUniforms 				m_ShaderUniforms;
	HyPortal2dHandle				m_hPortals[HY_MAX_PORTAL_HANDLES];

	HyShape2d						m_BoundingVolume;
	b2AABB							m_aabbCached;

public:
	IHyDrawInst2d(HyType eInstType, const char *szPrefix, const char *szName, HyEntity2d *pParent);
	virtual ~IHyDrawInst2d();

	virtual IHyDrawInst2d *Clone() const = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// NOTE: Below mutators manipulate data from derived class "IHyDraw2d". Handled in regard to being a "leaf"
	void SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight);
	void ClearScissor(bool bUseParentScissor);

	void SetStencil(HyStencil *pStencil);
	void ClearStencil(bool bUseParentStencil);

	void UseCameraCoordinates();
	void UseWindowCoordinates(int32 iWindowIndex = 0);

	void SetDisplayOrder(int32 iOrderValue);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const std::string &GetName() const;
	const std::string &GetPrefix() const;

	HyRenderMode GetRenderMode() const;
	HyTextureHandle GetTextureHandle() const;

	IHyNodeData *AcquireData();

	const HyShape2d &GetBoundingVolume();
	const b2AABB &GetWorldAABB();
	HyShape2d *GetUserBoundingVolume(uint32 uiIndex);

	// Passing nullptr will use built-in default shader
	void SetCustomShader(HyShader *pShader);
	HyShaderHandle GetShaderHandle();

	bool SetPortal(HyPortal2d *pPortal);
	bool ClearPortal(HyPortal2d *pPortal);

	virtual bool IsLoaded() const override;
	virtual void Load() override;
	virtual void Unload() override;

protected:
	virtual void CalcBoundingVolume() = 0;
	virtual void AcquireBoundingVolumeIndex(uint32 &uiStateOut, uint32 &uiSubStateOut) = 0;

	virtual void OnShapeSet(HyShape2d *pShape) { }

	virtual void NodeUpdate() override final;

	virtual void _SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding) override;
	virtual void _SetStencil(HyStencilHandle hHandle, bool bIsOverriding) override;
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) override;
	virtual void _SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) override;

	IHyNodeData *UncheckedGetData();

	void WriteShaderUniformBuffer(char *&pRefDataWritePos);

	virtual bool IsLoadDataValid() { return true; }
	virtual void DrawUpdate() { }
	virtual void OnDataAcquired() { }									// Invoked once on the first time this node's data is queried
	virtual void OnLoaded() { }											// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnUpdateUniforms() { }									// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) { }		// This function is responsible for incrementing the passed in reference pointer the size of the data written

#ifdef HY_PLATFORM_GUI
public:
	template<typename HYDATATYPE>
	void GuiOverrideData(jsonxx::Value &dataValueRef)
	{
		delete m_pData;
		m_pData = HY_NEW HYDATATYPE("+HyGuiOverride", dataValueRef, *sm_pHyAssets);
		OnDataAcquired();
	}
#endif
};

#endif /* IHyLeafDraw2d_h__ */

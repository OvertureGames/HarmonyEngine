/**************************************************************************
 *	HyCamera.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyCamera_h__
#define HyCamera_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/HyMath.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/IHyNode3d.h"

class HyWindow;

class IHyCamera
{
protected:
	HyWindow *			m_pWindowPtr;
	HyRectangle<float>	m_ViewportRect;	// Values are [0.0-1.0] representing percentages

	uint32				m_uiCullMaskBit;

protected:
	IHyCamera(HyWindow *pWindow);
	IHyCamera(const IHyCamera &copyRef) = delete;
	virtual ~IHyCamera();

	const IHyNode2d &operator=(const IHyNode2d &rhs) = delete;

public:
	HyWindow &GetWindow()						{ return *m_pWindowPtr; }
	const HyRectangle<float> &GetViewport()		{ return m_ViewportRect; }

	virtual void GetCameraTransform(glm::mat4 &outMtx) = 0;

	// All values are [0.0 - 1.0] representing percentages of the entire game window
	void SetViewport(float fPosX, float fPosY, float fWidth, float fHeight);

	virtual void SetZoom(const float fZoom) = 0;
	virtual float GetZoom() const = 0;

	void SetCullMaskBit(uint32 uiBit);
	uint32 GetCameraBitFlag();
};

class HyCamera2d final : public IHyNode2d, public IHyCamera
{
	friend class HyWindow;

	b2AABB		m_aabbViewBounds;

private:
	HyCamera2d(HyWindow *pWindow);
	virtual ~HyCamera2d();
	
public:
	virtual void SetZoom(const float fZoom) override;
	virtual float GetZoom() const override;

	virtual void GetCameraTransform(glm::mat4 &outMtx) override;

	const b2AABB &GetWorldViewBounds();

protected:
	virtual void NodeUpdate() override;
};

class HyCamera3d final : public IHyNode3d, public IHyCamera
{
	friend class HyWindow;

protected:
	HyCamera3d(HyWindow *pWindow);
	virtual ~HyCamera3d();

public:
	virtual void SetZoom(const float fZoom) override;
	virtual float GetZoom() const override;

	virtual void GetCameraTransform(glm::mat4 &outMtx) override;

protected:
	virtual void NodeUpdate() override;
};

#endif /* HyCamera_h__ */
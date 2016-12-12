/**************************************************************************
 *	HyCamera.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyCamera_h__
#define __HyCamera_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/HyMath.h"
#include "Scene/Transforms/IHyTransform2d.h"
#include "Scene/Transforms/IHyTransform3d.h"

class HyWindow;

class IHyCamera
{
protected:
	HyWindow *			m_pWindowPtr;
	HyRectangle<float>	m_ViewportRect;	// Values are [0.0-1.0] representing percentages

	IHyCamera(HyWindow *pWindow);
public:
	virtual ~IHyCamera();

	HyWindow &GetWindow()						{ return *m_pWindowPtr; }
	const HyRectangle<float> &GetViewport()		{ return m_ViewportRect; }

	// All values are [0.0 - 1.0] representing percentages of the entire game window
	void SetViewport(float fPosX, float fPosY, float fWidth, float fHeight);

	virtual void SetZoom(const float fZoom) = 0;
	virtual float GetZoom() const = 0;
};

class HyCamera2d : public IHyTransform2d, public IHyCamera
{
	friend class HyWindow;

	HyCamera2d(HyWindow *pWindow);
public:
	virtual ~HyCamera2d();


	virtual void SetZoom(const float fZoom)		{ scale.Set(fZoom, fZoom); }
	virtual float GetZoom() const				{ return scale.Get().x; }

	HyRectangle<float> GetWorldViewBounds();
	
private:
	virtual void OnUpdate();
};

class HyCamera3d : public IHyTransform3d, public IHyCamera
{
	friend class HyWindow;

	HyCamera3d(HyWindow *pWindow);
public:
	virtual ~HyCamera3d();


	virtual void SetZoom(const float fZoom)		{ scale.Set(1.0f, 1.0f, fZoom); }
	virtual float GetZoom() const				{ return scale.Get().z; }
	
private:
	virtual void OnUpdate();
};

#endif /* __HyCamera_h__ */

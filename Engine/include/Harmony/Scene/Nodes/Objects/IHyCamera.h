/**************************************************************************
*	IHyCamera.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyCamera_h__
#define IHyCamera_h__

#include "Afx/HyStdAfx.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyRand.h"

class HyWindow;
class HyScene;
class IHyRenderer;

template<typename NODETYPE>
class IHyCamera : public NODETYPE
{
	friend class HyScene;
	friend class IHyRenderer;

protected:
	HyWindow *			m_pWindowPtr;
	HyRectangle<float>	m_ViewportRect;	// Values are [0.0-1.0] representing percentages

	uint32				m_uiCullMaskBit;

	float				m_fCameraShakeRadius;
	float				m_fCameraShakeAngle;
	glm::vec3			m_ptCameraShakeCenter;

protected:
	IHyCamera(HyWindow *pWindow);

	IHyCamera(const IHyCamera &copyRef) = delete;
	virtual ~IHyCamera();

public:
	HyWindow &GetWindow();
	const HyRectangle<float> &GetViewport();

	// All values are [0.0 - 1.0] representing percentages of the entire game window
	void SetViewport(float fPosX, float fPosY, float fWidth, float fHeight);

	bool IsCameraShake();
	void CameraShake(float fRadius);

	virtual void SetZoom(const float fZoom) = 0;
	virtual float GetZoom() const = 0;

protected:
	virtual void Update() override;

private:
	void SetCullMaskBit(uint32 uiBit);
	uint32 GetCameraBitFlag();
};

#endif /* IHyCamera_h__ */

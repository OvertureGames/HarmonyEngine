/**************************************************************************
 *	HyPrimitive2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPrimitive2d_h__
#define HyPrimitive2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"

class HyAtlasGroupData;

class HyPrimitive2d : public IHyDrawInst2d
{
protected:
	glm::vec2 *		m_pVertBuffer;
	uint32			m_uiNumVerts;

	bool			m_bWireframe;
	float			m_fLineThickness;
	bool			m_bDirty;

public:
	HyPrimitive2d(HyEntity2d *pParent = nullptr);
	virtual ~HyPrimitive2d(void);

	const HyPrimitive2d &HyPrimitive2d::operator=(const HyPrimitive2d& p);

	virtual bool IsEnabled() const override;

	HyShape2d &GetShape();

	uint32 GetNumVerts() const;

	bool IsWireframe();
	void SetWireframe(bool bIsWireframe);

	float GetLineThickness();
	void SetLineThickness(float fThickness);

protected:
	virtual void CalcBoundingVolume() override;
	virtual void AcquireBoundingVolumeIndex(uint32 &uiStateOut, uint32 &uiSubStateOut) override;

	virtual void OnShapeSet(HyShape2d *pShape) override;

	virtual void DrawUpdate();

private:
	void ClearData();

	void SetAsLineChain(b2Vec2 *pVertexList, uint32 uiNumVertices);
	void SetAsCircle(glm::vec2 &ptCenter, float fRadius);
	void SetAsPolygon(b2Vec2 *pVertexList, uint32 uiNumVertices);

	virtual void OnUpdateUniforms() override;
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) override;
};

#endif /* HyPrimitive2d_h__ */
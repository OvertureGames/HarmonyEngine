/**************************************************************************
 *	HyOpenGL.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyOpenGL_h__
#define __HyOpenGL_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/GfxApi/IGfxApi.h"
#include "Renderer/GfxApi/OpenGL/HyOpenGLShader.h"

//#define HY_INDEX_BUFFER_SIZE (128 * 1024)
//#define HY_RESTART_INDEX 65535

class HyOpenGL : public IGfxApi
{
protected:
	mat4					m_mtxView;
	mat4					m_mtxProj;
	const mat4				m_kmtxIdentity;

	int32					m_iNumCams2d;
	int32					m_iNumPasses3d;

	///////// 2D MEMBERS ////////////////////
	enum eVAOTypes
	{
		QUADBATCH,
		PRIMITIVE,
		CUSTOM,

		NUM_VAOTYPES
	};
	GLuint					m_pVAO2d[NUM_VAOTYPES];
	HyOpenGLShader			m_pShader2d[NUM_VAOTYPES];
	
	GLuint					m_hVBO2d;
	GLenum					m_eDrawMode;

	//uint16 *				m_pGenericIndexBuffer2d;

	//void (HyOpenGL::*m_fpDraw2d)(char *pData);

public:
	HyOpenGL();
	virtual ~HyOpenGL(void);

	virtual bool CreateWindows();

	virtual bool Initialize();
	virtual bool PollApi();
	virtual bool CheckDevice();

	virtual void StartRender();
	
	virtual bool Begin_3d();
	virtual void SetRenderState_3d(uint32 uiNewRenderState);
	virtual void End_3d();

	virtual bool Begin_2d();
	virtual void DrawRenderState_2d(HyRenderState &renderState);
	virtual void End_2d();

	virtual void FinishRender();

	virtual bool Shutdown();

	// Returns the texture ID used for API specific drawing.
	virtual uint32 AddTexture(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, void *pPixelData);
	virtual void DeleteTexture(HyTexture &texture);

//private:
//	void DrawBatchedQuads2d(char *pData);
//	static void DrawPrim2dInst(IDraw2d *pBaseInst, void *pApi);
//	static void DrawTxt2dInst(IDraw2d *pBaseInst, void *pApi);
};

#endif /* __HyOpenGL_h__ */
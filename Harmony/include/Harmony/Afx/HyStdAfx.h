/**************************************************************************
 *	HyStdAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyStdAfx_h__
#define __HyStdAfx_h__

// Order of these #includes matter!
#include "Compilers/HyCompiler.h"
#include "Platforms/HyPlatform.h"

#define HY_TEMP_TEXTBUFFER_SIZE 1024

enum HyInstanceType
{
	HYINST_Unknown = 0,
	HYINST_Sound2d,
	HYINST_Particles2d,
	HYINST_Sprite2d,
	HYINST_Spine2d,
	HYINST_TexturedQuad2d,
	HYINST_Primitive2d,
	HYINST_Text2d,
	HYINST_Mesh3d,

	HYINST_Shader
};

enum HyDataType
{
	HYDATA_Regular = 0,
	HYDATA_2d,
	HYDATA_3d
};

enum HyLoadState
{
	HYLOADSTATE_Inactive = 0,
	HYLOADSTATE_Queued,
	HYLOADSTATE_Loaded,
	HYLOADSTATE_Discarded,
	HYLOADSTATE_ReloadGfx
};

enum HyShaderType
{
	HYSHADER_Vertex = 0,
	HYSHADER_Fragment,
	HYSHADER_Geometry,
	HYSHADER_TessControl,
	HYSHADER_TessEvaluation,

	HYNUMSHADERTYPES
};

enum HyShaderVariable
{
	HYSHADERVAR_bool = 0,
	HYSHADERVAR_int,
	HYSHADERVAR_uint,
	HYSHADERVAR_float,
	HYSHADERVAR_double,
	HYSHADERVAR_bvec2,
	HYSHADERVAR_bvec3,
	HYSHADERVAR_bvec4,
	HYSHADERVAR_ivec2,
	HYSHADERVAR_ivec3,
	HYSHADERVAR_ivec4,
	HYSHADERVAR_vec2,
	HYSHADERVAR_vec3,
	HYSHADERVAR_vec4,
	HYSHADERVAR_dvec2,
	HYSHADERVAR_dvec3,
	HYSHADERVAR_dvec4,
	HYSHADERVAR_mat3,
	HYSHADERVAR_mat4
};

enum HyCoordinateType
{
	HYCOORDTYPE_Default = 0,

	HYCOORDTYPE_Camera,
	HYCOORDTYPE_Screen
};

enum HyCoordinateUnit
{
	HYCOORDUNIT_Default = 0,

	HYCOORDUNIT_Pixels,
	HYCOORDUNIT_Meters
};

enum HyWindowType
{
	HYWINDOW_WindowedFixed,
	HYWINDOW_WindowedSizeable,
	HYWINDOW_FullScreen,
	HYWINDOW_BorderlessWindow
};

struct HyWindowInfo
{
	std::string		sName;
	glm::ivec2		vResolution;
	glm::ivec2		vLocation;
	HyWindowType	eType;

	enum eDirtyFlags
	{
		FLAG_Title = 1 << 0,
		FLAG_Resolution = 1 << 1,
		FLAG_Location = 1 << 2,
		FLAG_Type = 1 << 3
	};
	uint32			uiDirtyFlags;
};

#define HY_MAXWINDOWS 6

// Client supplies these initialization parameters to the engine
struct HarmonyInit
{
	std::string				sGameName;
	std::string				sDataDir;
	uint32					uiNumWindows;
	HyWindowInfo			windowInfo[HY_MAXWINDOWS];
	HyCoordinateType		eDefaultCoordinateType;
	HyCoordinateUnit		eDefaultCoordinateUnit;
	float					fPixelsPerMeter;
	uint32					uiNumInputMappings;
	uint16					uiDebugPort;

	HarmonyInit(std::string sGameNameStr, std::string sDataDirStr) :	sGameName(sGameNameStr),
																		sDataDir(sDataDirStr)
	{
		CtorInit();
	}

	HarmonyInit() :	sGameName("Untitled Game"),
					sDataDir("/data")
	{
		CtorInit();
	}

private:
	void CtorInit()
	{
		uiNumWindows = 1;
		eDefaultCoordinateType = HYCOORDTYPE_Camera;
		eDefaultCoordinateUnit = HYCOORDUNIT_Pixels;
		fPixelsPerMeter = 80.0f;
		uiNumInputMappings = 1;
		uiDebugPort = 1313;

		for(int i = 0; i < HY_MAXWINDOWS; ++i)
		{
			windowInfo[i].sName = "Window: " + std::to_string(i);
			windowInfo[i].eType = HYWINDOW_WindowedFixed;
			windowInfo[i].vResolution.x = 512;
			windowInfo[i].vResolution.x = 256;
			windowInfo[i].vLocation.x = i * windowInfo[i].vResolution.x;
			windowInfo[i].vLocation.y = 0;
			
			windowInfo[i].uiDirtyFlags = 0;
		}
	}
};

#endif // __HyStdAfx_h__

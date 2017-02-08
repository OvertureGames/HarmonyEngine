/**************************************************************************
 *	HyOpenGLShaderSrc.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyOpenGLShaderSrc_h__
#define __HyOpenGLShaderSrc_h__

#include "Afx/HyStdAfx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// QUADBATCH
const char * const szHYQUADBATCH_VERTEXSHADER = R"src(
#version 130

/*layout(location = 0)*/ in vec2 size;
/*layout(location = 1)*/ in vec2 offset;
/*layout(location = 2)*/ in vec4 topTint;
/*layout(location = 3)*/ in vec4 botTint;
/*layout(location = 4)*/ in float textureIndex;
/*layout(location = 5)*/ in vec2 UVcoord0;
/*layout(location = 6)*/ in vec2 UVcoord1;
/*layout(location = 7)*/ in vec2 UVcoord2;
/*layout(location = 8)*/ in vec2 UVcoord3;
/*layout(location = 9)*/ in mat4 mtxLocalToWorld;

/*smooth*/ out vec2 interpUV;
/*smooth*/ out vec4 interpColor;
/*flat*/ out float texIndex;

uniform mat4 u_mtxWorldToCamera;
uniform mat4 u_mtxCameraToClip;

/*const*/ vec2 position[] = vec2[4](vec2(1.0f, 1.0f),
									vec2(0.0f, 1.0f),
									vec2(1.0f, 0.0f),
									vec2(0.0f, 0.0f));

void main()
{
	switch(gl_VertexID)
	{
	case 0:
		interpUV.x = UVcoord0.x;
		interpUV.y = UVcoord0.y;

		interpColor = topTint;
		break;
	case 1:
		interpUV.x = UVcoord1.x;
		interpUV.y = UVcoord1.y;

		interpColor = topTint;
		break;
	case 2:
		interpUV.x = UVcoord2.x;
		interpUV.y = UVcoord2.y;

		interpColor = botTint;
		break;
	case 3:
		interpUV.x = UVcoord3.x;
		interpUV.y = UVcoord3.y;

		interpColor = botTint;
		break;
	}
	texIndex = textureIndex;


	vec4 pos = vec4((position[gl_VertexID].x * size.x) + offset.x,
					(position[gl_VertexID].y * size.y) + offset.y,
					0.0, 1.0);

	pos = mtxLocalToWorld * pos;
	pos = u_mtxWorldToCamera * pos;
	gl_Position = u_mtxCameraToClip * pos;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYQUADBATCH_FRAGMENTSHADER = R"src(
#version 130
#extension GL_EXT_texture_array : enable

/*smooth*/ in vec2 interpUV;
/*smooth*/ in vec4 interpColor;
/*flat*/ in float texIndex;

uniform sampler2DArray Tex;

out vec4 outputColor;

void main()
{
	// Blend interpColor with whatever texel I get from interpUV
	vec4 texelClr = texture(Tex, vec3(interpUV.x, interpUV.y, texIndex));

	outputColor = interpColor * texelClr;
}
)src";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIMITIVE
const char * const szHYPRIMATIVE_VERTEXSHADER = R"src(
#version 130

/*layout(location = 0)*/ in vec2 a_vPosition;

uniform mat4 u_mtxTransform;
uniform mat4 u_mtxWorldToCamera;
uniform mat4 u_mtxCameraToClip;
uniform vec4 u_vColor;

void main()
{
	vec4 temp = u_mtxTransform * vec4(a_vPosition, 0, 1);
	temp = u_mtxWorldToCamera * temp;
	gl_Position = u_mtxCameraToClip * temp;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYPRIMATIVE_FRAGMENTSHADER = R"src(
#version 130

out vec4 vFragColorOut;

uniform vec4 u_vColor;

void main()
{
	vFragColorOut = u_vColor;
}
)src";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LINES2D
const char * const szHYLINES2D_VERTEXSHADER = R"src(
#version 130

in vec2 a_vPosition;
in vec2 a_vNormal;

out vec2 vNormalOut;

uniform float u_fHalfWidth;
uniform float u_fFeatherAmt;
uniform vec4 u_vColor;
uniform mat4 u_mtxTransform;
uniform mat4 u_mtxWorldToCamera;
uniform mat4 u_mtxCameraToClip;

void main()
{
	vNormalOut = a_vNormal;

	vec4 vPos = u_mtxTransform * vec4(a_vPosition, 0, 1);
	vPos = u_mtxWorldToCamera * vPos;
	gl_Position = u_mtxCameraToClip * (vPos + vec4(a_vNormal * (u_fHalfWidth + (u_fFeatherAmt * 0.5f)), 0, 0));
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYLINES2D_FRAGMENTSHADER = R"src(
#version 130

in vec2 vNormalOut;

out vec4 vFragColorOut;

uniform float u_fHalfWidth;
uniform float u_fFeatherAmt;
uniform vec4 u_vColor;

void main()
{
	vFragColorOut = u_vColor;
	vFragColorOut.w = smoothstep(u_fHalfWidth, u_fHalfWidth - u_fFeatherAmt, length(vNormalOut) * u_fHalfWidth);
}
)src";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif __HyOpenGLShaderSrc_h__

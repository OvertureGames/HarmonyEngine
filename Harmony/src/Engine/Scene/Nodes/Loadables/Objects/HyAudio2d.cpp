/**************************************************************************
 *	HyAudio2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"

HyAudio2d::HyAudio2d(const char *szPrefix, const char *szName, HyEntity2d *pParent) :
	IHyLoadable2d(HYTYPE_Audio, szPrefix, szName, pParent)
{
}

/*virtual*/ HyAudio2d::~HyAudio2d(void)
{
}

void HyAudio2d::Play()
{
}

void HyAudio2d::Stop()
{
}

float HyAudio2d::GetPitch()
{
	return 0.0f;
}

void HyAudio2d::SetPitch(float fPitch)
{
}

void HyAudio2d::SetReverb()
{
}

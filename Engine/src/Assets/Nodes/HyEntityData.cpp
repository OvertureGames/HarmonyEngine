/**************************************************************************
 *	HyEntityData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyEntityData.h"

HyEntityData::HyEntityData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef) :
	IHyNodeData(sPath)
{
}

HyEntityData::~HyEntityData()
{
}

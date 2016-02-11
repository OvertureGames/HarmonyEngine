/**************************************************************************
 *	HySfxData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Data/HySfxData.h"


HySfxData::HySfxData(const std::string &sPath) : IHyData(HYDATA_Regular, HYINST_Sound2d, sPath)
{
}

HySfxData::~HySfxData(void)
{
}

/*virtual*/ void HySfxData::DoFileLoad(HyTextures &atlasManagerRef)
{
}
/**************************************************************************
 *	PrimitiveModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "PrimitiveModel.h"

PrimitiveModel::PrimitiveModel(ProjectItem &itemRef, FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef)
{
}

PrimitiveModel::~PrimitiveModel()
{
}

/*virtual*/ bool PrimitiveModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
	return true;
}

/*virtual*/ FileDataPair PrimitiveModel::GetStateFileData(uint32 uiIndex) const /*override*/
{
	return FileDataPair();
}

/*virtual*/ QList<AtlasFrame *> PrimitiveModel::GetAtlasFrames() const /*override*/
{
	return QList<AtlasFrame *>();
}

/*virtual*/ QStringList PrimitiveModel::GetFontUrls() const /*override*/
{
	return QStringList();
}

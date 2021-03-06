/**************************************************************************
 *	AtlasRepackThread.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASREPACKTHREAD_H
#define ATLASREPACKTHREAD_H

#include "IRepackThread.h"
#include "AtlasFrame.h"
#include "_Dependencies/scriptum/imagepacker.h"

class AtlasRepackThread : public IRepackThread
{
	Q_OBJECT

	BankData &			m_BankRef;
	
	struct PackerBucket
	{
		ImagePacker			m_Packer;

		QSet<int>			m_TextureIndexSet;
		QList<AtlasFrame *> m_FramesList;
	};
	QMap<QPair<HyTextureFormat, HyTextureFiltering>, PackerBucket *> m_BucketMap;

public:
	AtlasRepackThread(BankData &bankRef, QList<AtlasFrame *>newFramesList, QDir metaDir);
	virtual ~AtlasRepackThread();

	virtual void OnRun() override;

private:
	void ConstructAtlasTexture(ImagePacker &imagePackerRef, HyTextureFormat eFormat, int iPackerBinIndex, int iActualTextureIndex);
	void SetPackerSettings(ImagePacker &imagePackerRef);
};

#endif // ATLASREPACKTHREAD_H

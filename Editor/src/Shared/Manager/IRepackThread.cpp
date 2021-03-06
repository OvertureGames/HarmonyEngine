/**************************************************************************
 *	IRepackThread.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IRepackThread.h"

IRepackThread::IRepackThread(QDir metaDir) :
	m_MetaDir(metaDir)
{
}

/*virtual*/ IRepackThread::~IRepackThread()
{
}

/*virtual*/ void IRepackThread::run() /*override*/
{
	OnRun();
	Q_EMIT RepackIsFinished();
}

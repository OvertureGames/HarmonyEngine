/**************************************************************************
 *	main.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "MainWindow.h"
#include <QApplication>

#include "Global.h"
#include "AtlasFrame.h"
#include "DlgProjectSettings.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	qRegisterMetaTypeStreamOperators<AtlasFrame *>("AtlasFrame *");
	qRegisterMetaTypeStreamOperators<ExplorerItem *>("ExplorerItem *");
	//qRegisterMetaTypeStreamOperators<ProjectItem *>("ProjectItem *");

	HyGlobal::Initialize();
	DlgProjectSettings::InitDefaultValues();

	MainWindow w;
	w.show();

	return a.exec();
}
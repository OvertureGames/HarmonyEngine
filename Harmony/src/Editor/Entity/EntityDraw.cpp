/**************************************************************************
*	EntityDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "EntityDraw.h"

EntityDraw::EntityDraw(ProjectItem *pProjItem, IHyApplication &hyApp) : IDraw(pProjItem, hyApp)
{

}

/*virtual*/ EntityDraw::~EntityDraw()
{
}

/*virtual*/ void EntityDraw::OnApplyJsonData(jsonxx::Value &valueRef) /*override*/
{
}

/*virtual*/ void EntityDraw::OnShow(IHyApplication &hyApp) /*override*/
{
}

/*virtual*/ void EntityDraw::OnHide(IHyApplication &hyApp) /*override*/
{
}

/*virtual*/ void EntityDraw::OnResizeRenderer() /*override*/
{
}
/**************************************************************************
 *	TestBed.cpp
 *	
 *	Copyright (c) 2013 Jason Knobler
 *	Harmony Engine
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "TestBed.h"

TestBed::TestBed(HarmonyInit &initStruct) : IApplication(initStruct)
							//						m_physBox2("Box", b2_dynamicBody),
								//					m_physGround("Ground", b2_staticBody),
									//				m_TxtInst("Vera", 48, 48)
{
}

TestBed::~TestBed(void)
{
}

/*virtual*/ bool TestBed::Initialize()
{
	//m_pInputArray->GetGamePadIds(m_vGamePadIds);

	//if(m_vGamePadIds.empty() == false)
	//{
	//	m_pInputArray->BindBtnMap(GP360_ButtonA, HyInputKey(m_vGamePadIds[0], GP360_ButtonA));
	//	m_pInputArray->BindBtnMap(GP360_ButtonB, HyInputKey(m_vGamePadIds[0], GP360_ButtonB));

	//	m_pInputArray->BindAxisMap(GP_RStickX, HyInputKey(m_vGamePadIds[0], GP_RStickX));
	//	m_pInputArray->BindAxisMap(GP_RStickY, HyInputKey(m_vGamePadIds[0], GP_RStickY));
	//	m_pInputArray->BindAxisMap(GP_Triggers, HyInputKey(m_vGamePadIds[0], GP_Triggers));

	//	m_pInputArray->BindAxisMap(GP_LStickX, HyInputKey(m_vGamePadIds[0], GP_LStickX));
	//}
	//m_pInputArray->BindAxisMapPos(GP_LStickX, HyInputKey('D'));
	//m_pInputArray->BindAxisMapNeg(GP_LStickX, HyInputKey('A'));

	//m_pInputArray->BindBtnMap(GP360_ButtonA, HyInputKey(' '));

	m_pCam = m_Viewport.CreateCamera2d();
	
	m_Player.Initialize();

	//HyGfxWindow::tResolution tRes;
	//m_Window.GetResolution(tRes);
	//m_Camera.SetOrthographic(static_cast<float>(tRes.iWidth), static_cast<float>(tRes.iHeight));

	m_pCam->Pos().Set(0.0f, 0.0f);
	m_pCam->SetZoom(0.8f);

	return true;
}

/*virtual*/ bool TestBed::Update()
{
	//if(m_pInputArray->SetReadKeyCallback GpBtnDown(GP360_ButtonB))
	//{
	//	if(m_pInputMapArray->GpBtnDown(GP360_ButtonB))
	//		m_EntSpineBoy.ProcTranslate(Procedrual_QuadraticIn, vec2(-100.0f, 100.0f), 1.0f);
	//}

	//m_Player.Move(m_pInputArray->GpAxis(GP_LStickX));

	//if(m_pInputArray->GpBtnDown(GP360_ButtonA) && m_Player.IsFeetOnGround())
	//	m_Player.Jump(2.75f);

	//m_pCam->Pos().Set(m_Player.GetPos());
	//m_pCam->Pos().Offset(m_pInputArray->GpAxis(GP_RStickX) * 2.0f, -m_pInputArray->GpAxis(GP_RStickY) * 2.0f);
	//m_pCam->SetZoom(m_pCam->GetZoom() + m_pInputArray->GpAxis(GP_Triggers) * 0.05f);


	//m_TxtInst.SetString(HyStr("CamX: %f\tCamY:%f"), m_pCam->Pos().X(), m_pCam->Pos().Y());

	return true;
}

/*virtual*/ bool TestBed::Shutdown()
{
	return true;
}

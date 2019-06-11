/**************************************************************************
*	HyAudio.h
*
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudio_h__
#define HyAudio_h__

#include "Afx/HyStdAfx.h"
#include "Audio/Harness/IHyAudio.h"
#include "Audio/Harness/IHyAudioBank.h"
#include "Audio/Harness/IHyAudioInst.h"

class HyAudioBank;

class HyAudio
{
	using fpAllocateHyAudio					= IHyAudio *(*)();
	using fpAllocateHyAudioBank				= IHyAudioBank *(*)(IHyAudio *);
	using fpAllocateHyAudioInst				= IHyAudioInst *(*)(IHyAudio *, const char *);

	fpAllocateHyAudio						m_fpAllocateHyAudio;
	fpAllocateHyAudioBank					m_fpAllocateHyAudioBank;
	fpAllocateHyAudioInst					m_fpAllocateHyAudioInst;
	IHyAudio *								m_pInternal;

	std::map<std::string, HyAudioBank *>	m_AudioBankMap;

public:
	HyAudio(std::string sDataDir);
	~HyAudio();

	IHyAudioInst *AllocateAudioInst(const char *szPath);
	HyAudioBank *GetAudioBank(const std::string &sBankName);

	void Update();

private:
	IHyAudioBank *AllocateAudioBank();
};

#endif /* HyAudio_h__ */

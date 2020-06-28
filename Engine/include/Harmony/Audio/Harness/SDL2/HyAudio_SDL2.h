/**************************************************************************
 *	HyAudio_SDL2.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudio_SDL2_h__
#define HyAudio_SDL2_h__

#include "Afx/HyStdAfx.h"
#include "Audio/Harness/IHyAudioCore.h"

class IHyNode;
class IHyFileAudioGuts;

#if defined(HY_USE_SDL2)
class HyRawSoundBuffer;
class HyFileAudioGuts_SDL2;

class HyAudio_SDL2 : public IHyAudioCore
{
	std::vector<std::string>			m_sDeviceList;

	SDL_AudioDeviceID					m_hDevice;
	SDL_AudioSpec						m_DesiredSpec;

	std::vector<HyFileAudioGuts_SDL2 *>	m_AudioFileList;

	// Used in callback thread ///////////////////////////////////////////////
	struct Play
	{
		const IHyNode *					m_pNode;
		float							m_fVolume;
		float							m_fPitch;
		HyRawSoundBuffer *				m_pBuffer;
		uint32							m_uiRemainingBytes;
	};
	std::vector<Play>					m_PlayList;
	std::vector<Play>					m_PauseList;
	///////////////////////////////////////////////////////////////////////////

public:
	HyAudio_SDL2();
	virtual ~HyAudio_SDL2(void);

	const char *GetAudioDriver();

	virtual void OnUpdate() override;

	static IHyFileAudioGuts *AllocateBank(IHyAudioCore *pAudio, const jsonxx::Object &bankObjRef);

private:
	static void OnCallback(void *pUserData, uint8_t *pStream, int32 iLen);
};
#endif // defined(HY_USE_SDL2)

#endif /* HyAudio_SDL2_h__ */

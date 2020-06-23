/**************************************************************************
 *	HyAudio_SDL2.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Audio/Harness/SDL2/HyAudio_SDL2.h"
#include "Audio/Harness/SDL2/HyFileAudioGuts_SDL2.h"
#include "Audio/Harness/SDL2/HyAudioInst_SDL2.h"
#include "Diagnostics/Console/HyConsole.h"

#if defined(HY_USE_SDL2)

HyAudio_SDL2::HyAudio_SDL2()
{
	HyLogTitle("SDL2 Audio");

	int32 iNumDevices = SDL_GetNumAudioDevices(0);
	for(uint32 i = 0; i < iNumDevices; ++i)
		m_sDeviceList.push_back(SDL_GetAudioDeviceName(i, 0));

	m_DesiredSpec.freq = 44100;							// 44100 or 48000
#if defined(HY_ENDIAN_LITTLE)
	m_DesiredSpec.format = AUDIO_F32LSB;
#else
	m_DesiredSpec.format = AUDIO_F32MSB;
#endif
	m_DesiredSpec.channels = 2;							// 1 mono, 2 stereo, 4 quad, 6 (5.1)
	m_DesiredSpec.samples = 4096;						// Specifies a unit of audio data to be used at a time. Must be a power of 2
	m_DesiredSpec.callback = HyAudio_SDL2::OnCallback;
	m_DesiredSpec.userdata = this;

	/*
	* Note: If you're having issues with Emscripten / EMCC play around with these flags
	*
	* 0                                    Allow no changes
	* SDL_AUDIO_ALLOW_FREQUENCY_CHANGE     Allow frequency changes (e.g. AUDIO_FREQUENCY is 48k, but allow files to play at 44.1k
	* SDL_AUDIO_ALLOW_FORMAT_CHANGE        Allow Format change (e.g. AUDIO_FORMAT may be S32LSB, but allow wave files of S16LSB to play)
	* SDL_AUDIO_ALLOW_CHANNELS_CHANGE      Allow any number of channels (e.g. AUDIO_CHANNELS being 2, allow actual 1)
	* SDL_AUDIO_ALLOW_ANY_CHANGE           Allow all changes above
	*/
	m_hDevice = SDL_OpenAudioDevice(nullptr, 0, &m_DesiredSpec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if(m_hDevice == 0)
	{
		HyLogError("SDL_OpenAudioDevice failed: " << SDL_GetError());
		return;
	}

	//HyLog("Default Device:   " << atlasGrpArray.size());
	HyLog("Audio Driver:     " << SDL_GetCurrentAudioDriver());

	// Start audio device
	SDL_PauseAudioDevice(m_hDevice, 0);
}

/*virtual*/ HyAudio_SDL2::~HyAudio_SDL2(void)
{
	if(m_hDevice > 0)
		SDL_CloseAudioDevice(m_hDevice);
}

const char *HyAudio_SDL2::GetAudioDriver()
{
	return SDL_GetCurrentAudioDriver();
}

/*virtual*/ void HyAudio_SDL2::OnUpdate() /*override*/
{
	SDL_LockAudioDevice(m_hDevice);
	SDL_UnlockAudioDevice(m_hDevice);
}

void HyAudio_SDL2::QueueInst(HyAudioInst_SDL2 *pInst, uint32 uiSoundChecksum)
{
	Play newPlay = {};
	bool bFound = false;
	for(auto file : m_AudioFileList)
	{
		if(file->GetBufferInfo(uiSoundChecksum, newPlay.m_pBufferPos, newPlay.m_uiRemainingBytes, newPlay.m_AudioSpec))
		{
			bFound = true;
			break;
		}
	}
	if(bFound == false)
	{
		HyLogWarning("Could not find audio: " << uiSoundChecksum);
		return;
	}

	newPlay.m_pInst = pInst;

	SDL_LockAudioDevice(m_hDevice);
	m_PlayList.push_back(newPlay);
	SDL_UnlockAudioDevice(m_hDevice);
}

/*static*/ void HyAudio_SDL2::OnCallback(void *pUserData, uint8_t *pStream, int32 iLen)
{
	HyAudio_SDL2 *pThis = reinterpret_cast<HyAudio_SDL2 *>(pUserData);
	SDL_memset(pStream, 0, iLen); // If there is nothing to play, this callback should fill the buffer with silence

	for(uint32 i = 0; i < static_cast<uint32>(pThis->m_PlayList.size()); ++i)
	{
		Play &playRef = pThis->m_PlayList[i];

		uint32 uiLength = (static_cast<uint32_t>(iLen) > playRef.m_uiRemainingBytes) ? playRef.m_uiRemainingBytes : static_cast<uint32_t>(iLen);
		SDL_MixAudioFormat(pStream, playRef.m_pBufferPos, playRef.m_AudioSpec.format, uiLength, SDL_MIX_MAXVOLUME);

		playRef.m_pBufferPos += uiLength;
		playRef.m_uiRemainingBytes -= uiLength;
	}

	for(auto iter = pThis->m_PlayList.begin(); iter != pThis->m_PlayList.end();)
	{
		if(iter->m_uiRemainingBytes == 0)
			iter = pThis->m_PlayList.erase(iter);
		else
			++iter;
	}
}

/*static*/ IHyFileAudioGuts *HyAudio_SDL2::AllocateBank(IHyAudio *pAudio, const jsonxx::Object &bankObjRef)
{
	HyFileAudioGuts_SDL2 *pNewFileGuts = HY_NEW HyFileAudioGuts_SDL2(bankObjRef);
	static_cast<HyAudio_SDL2 *>(pAudio)->m_AudioFileList.push_back(pNewFileGuts);

	return pNewFileGuts;
}

/*static*/ IHyAudioInst *HyAudio_SDL2::AllocateInst(IHyAudio *pAudio, const jsonxx::Object &instObjRef)
{
	return HY_NEW HyAudioInst_SDL2(*static_cast<HyAudio_SDL2 *>(pAudio), instObjRef);
}

#endif // defined(HY_USE_SDL2)

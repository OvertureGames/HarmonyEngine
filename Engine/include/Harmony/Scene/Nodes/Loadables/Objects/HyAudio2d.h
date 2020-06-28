/**************************************************************************
 *	HyAudio2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudio2d_h__
#define HyAudio2d_h__

#include "Afx/HyInteropAfx.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"
#include "Audio/Harness/IHyAudioInst.h"

class HyAudio2d : public IHyLoadable2d
{
	friend class IHyAudioCore;
	static IHyAudioCore * 		sm_pAudioCore;
	
	uint32						m_uiCueFlags;

	float						m_fVolume;
	float						m_fPitch;

	//uint8_t					loop;
	//uint8_t					fade;

public:
	HyAnimFloat					volume;
	HyAnimFloat					pitch;

public:
	HyAudio2d(std::string sPrefix = "", std::string sName = "", HyEntity2d *pParent = nullptr);
	// TODO: copy ctor and move ctor
	virtual ~HyAudio2d(void);

	// TODO: assignment operator and move operator

	void PlayOneShot(bool bUseCurrentSettings = true);
	void Start();
	void Stop();
	void SetPause(bool bPause);

protected:
	virtual void OnLoadedUpdate() override;

private:
	// Hide any transform functionality inherited from IHyNode2d
	void GetLocalTransform(glm::mat4 &outMtx) const = delete;
	const glm::mat4 &GetWorldTransform() = delete;
	using IHyNode2d::GetWorldAABB;
	using IHyNode2d::pos;
	using IHyNode2d::rot;
	using IHyNode2d::rot_pivot;
	using IHyNode2d::scale;
	using IHyNode2d::scale_pivot;

	using IHyNode::IsVisible;
	using IHyNode::SetVisible;
	using IHyNode::IsPauseUpdate;
	using IHyNode::SetPauseUpdate;
};

#endif /* HyAudio2d_h__ */

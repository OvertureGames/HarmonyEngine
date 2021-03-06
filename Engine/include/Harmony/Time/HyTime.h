/**************************************************************************
 *	HyTime.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTime_h__
#define HyTime_h__

#include "Afx/HyStdAfx.h"
#include "Watches/HyTimer.h"
#include "Watches/HyStopwatch.h"

// Forward declarations
class IHyTimeInst;
class HyStopwatch;
class HyDiagnostics;

class HyTime
{
	double								m_dUpdateTick_Seconds;		// Non-zero value when a 'ThrottledUpdate' is desired

	static std::vector<IHyTimeInst *>	sm_TimeInstList;

	double								m_dTotalElapsedTime;
	double								m_dThrottledTime;

	double								m_dSpiralOfDeathCounter;	// In 'ThrottledUpdate' environments a potential to have updates take longer than the alloted time step will cause an infinite loop
																	// This counter keeps track of these scenarios
	double								m_dCurDeltaTime;

	uint64_t							m_uiPrev;
	uint64_t							m_uiCur;

public:
	HyTime(uint32 uiUpdateTickMs);
	~HyTime();

	void SetUpdateTickMs(uint32 uiUpdateTickMs);

	float GetUpdateStepSeconds() const;
	double GetUpdateStepSecondsDbl() const;

	double GetTotalElapsedTime() const;

	// Sets member variable 'm_dCurDeltaTime' to the delta seconds from its previous call (or from its initialization)
	// Delta time is in seconds.
	void SetCurDeltaTime();

	void CalcTimeDelta();

	bool ThrottleUpdate();

	// Will eat any delta that exist, and the next invoke to ThrottleUpdate() will 
	// produce a delta from this point.
	void ResetDelta();

	std::string GetDateTime();

	static void AddTimeInst(IHyTimeInst *pTimeInst);
	static void RemoveTimeInst(IHyTimeInst *pTimeInst);
};

#endif /* HyTime_h__ */

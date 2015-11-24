/**************************************************************************
 *	HyLiveVarManager.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyLiveVarManager_h__
#define __HyLiveVarManager_h__

#include "Afx/HyStdAfx.h"

#include <vector>
using std::vector;

class HyLiveVar
{
	static uint32	sm_uiGuidCounter;

	uint32			m_uiGuid;
	std::string		m_sName;

	struct tGroup
	{
		// Keeps track of 4 byte values
		struct tVar
		{
			void *pValuePtr;
			uint32 uiCachedRawValue;

			tVar(void *pVar) :	pValuePtr(pVar),
								uiCachedRawValue(*reinterpret_cast<uint32 *>(pValuePtr))
			{
			}

			bool IsDirty()
			{
				if(*reinterpret_cast<uint32 *>(pValuePtr) != uiCachedRawValue)
				{
					uiCachedRawValue = *reinterpret_cast<uint32 *>(pValuePtr);
					return true;
				}
				return false;
			}
		};
		vector<tVar>	m_vVariables;
	};
	vector<tGroup>	m_vGroups;

public:
	HyLiveVar();

	void AddWatch(const char *szGroupName, int32 *pVariable);
};

class HyLiveVarManager
{
	static HyLiveVarManager *		sm_pInstance;

	vector<HyLiveVar>				m_vPages;

	HyLiveVarManager(void);

public:
	~HyLiveVarManager(void);

	HyLiveVarManager *GetInstance();

	static HyLiveVar *CreateLiveWatchPage(std::string sName);
	static HyLiveVar *GetLiveWatchPage(std::string sName);
};

#endif /* __HyLiveVarManager_h__ */
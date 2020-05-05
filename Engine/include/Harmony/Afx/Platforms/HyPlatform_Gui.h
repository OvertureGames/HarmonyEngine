/**************************************************************************
*	HyPlatform_Gui.h
*	
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPlatform_Gui_h__
#define HyPlatform_Gui_h__

#include <glad/glad.h>

#define HY_GUI_DATAOVERRIDE "+HyGuiOverride"
#define HY_MAXWINDOWS 1
#define HY_ENDIAN_LITTLE	// All x86 and x86-64 machines are little-endian.
#define HY_NEW new

// Diagnostics assertion
#if defined(HY_DEBUG)
	#define HyAssert(condition, message) \
		do { \
			if(!(condition)) { \
			std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
			<< " line " << __LINE__ << ": " << message << std::endl; \
			std::terminate(); \
			} \
		} while (false)
	#define HyError(message) abort();
#else
	#define HyAssert(condition, message) do { } while (false)
	#define HyError(message) do { } while (false)
#endif

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	#define HY_PLATFORM_GUI_WIN
#endif

#endif /* HyPlatform_Gui_h__ */

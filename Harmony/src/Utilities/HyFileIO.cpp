/**************************************************************************
*	HyFileIO.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Utilities/HyFileIO.h"

char *HyReadTextFile(const char *szFilePath, int *iLength)
{
	char *pData;
	FILE *pFile = fopen(szFilePath, "rb");
	if(!pFile)
		return 0;

	fseek(pFile, 0, SEEK_END);
	*iLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	pData = new char[*iLength];
	fread(pData, 1, *iLength, pFile);
	fclose(pFile);

	return pData;
}

std::string HyReadTextFile(const char *szFilePath)
{
	if(szFilePath == NULL)
	{
		//sm_sLogStr = "ReadTextFile - filename is NULL\n";
		return std::string();
	}

	std::ifstream infile(szFilePath, std::ios::binary);
	if(!infile)
	{
		//sm_sLogStr = "ReadTextFile() - invalid filename\n";
		return std::string();
	}

	// TODO: Make this a lot more safer!
	std::istreambuf_iterator<char> begin(infile), end;

	std::string sReadOutput;
	sReadOutput.append(begin, end);

	return sReadOutput;
}

bool HyFileExists(const std::string &sFilePath)
{
	// TODO: implement
	return true;
}
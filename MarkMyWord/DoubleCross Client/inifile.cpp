#include "os.h"
#include "inifile.h"

IniFile::IniFile(char szNewIniFilename[], char szNewIniSectionname[])
	: VIniFile(szNewIniFilename, szNewIniSectionname)
	{
	}

IniFile::~IniFile()
	{
	}

void IniFile::ReadString(char szEntry[], char szDefault[], char szResult[], int iResultLength)
	{
	GetPrivateProfileString(szIniSectionname, szEntry, szDefault, szResult, iResultLength, szIniFilename);
	}

void IniFile::WriteString(char szEntry[], char szValue[])
	{
#ifdef DEMO
	return;
#endif
	WritePrivateProfileString(szIniSectionname, szEntry, szValue, szIniFilename);
	}

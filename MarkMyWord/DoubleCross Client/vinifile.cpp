#include "os.h"
#include "vinifile.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

VIniFile::VIniFile(char szNewIniFilename[], char szNewIniSectionname[])
	{
	lstrcpy(szIniFilename, szNewIniFilename);
	lstrcpy(szIniSectionname, szNewIniSectionname);
	}

VIniFile::~VIniFile()
	{
	}

int VIniFile::ReadValue(char szEntry[], int defaultValue)
	{
	char szTemp[255];
	char szDefault[32];

	itoa(defaultValue, szDefault, 10);
	ReadString(szEntry, szDefault, szTemp, 31);
	return atoi(szTemp);
	}

void VIniFile::WriteValue(char szEntry[], int value)
	{
	char szValue[12];

	itoa(value, szValue, 10);
	WriteString(szEntry, szValue);
	}

float VIniFile::ReadValue(char szEntry[], float defaultValue)
	{
	char szTemp[255];
	char szDefault[64];

	sprintf(szDefault, "%f", defaultValue);
	ReadString(szEntry, szDefault, szTemp, 64);
	return (float)atof(szTemp);
	}

void VIniFile::WriteValue(char szEntry[], float value)
	{
	char szValue[64];

	sprintf(szValue, "%f", value);
	WriteString(szEntry, szValue);
	}

DWORD VIniFile::ReadValue(char szEntry[], DWORD dwDefault)
	{
	char szTemp[255];
	char szDefault[12];

	ltoa(dwDefault, szDefault, 10);
	ReadString(szEntry, szDefault, szTemp, 11);
	return atol(szTemp);
	}

void VIniFile::WriteValue(char szEntry[], DWORD dwValue)
	{
	char szValue[12];

	ltoa(dwValue, szValue, 10);
	WriteString(szEntry, szValue);
	}

WORD VIniFile::ReadValue(char szEntry[], WORD wDefault)
	{
	char szTemp[255];
	char szDefault[7];

	itoa(wDefault, szDefault, 10);
	ReadString(szEntry, szDefault, szTemp, 6);
	return (WORD)atoi(szTemp);
	}

void VIniFile::WriteValue(char szEntry[], WORD wValue)
	{
	char szValue[7];

	itoa(wValue, szValue, 10);
	WriteString(szEntry, szValue);
	}

BYTE VIniFile::ReadValue(char szEntry[], BYTE btDefault)
	{
	char szTemp[255];
	char szDefault[7];

	itoa(btDefault, szDefault, 10);
	ReadString(szEntry, szDefault, szTemp, 6);
	return (BYTE)atoi(szTemp);
	}

void VIniFile::WriteValue(char szEntry[], BYTE btValue)
	{
	char szValue[7];

	itoa((int)(signed char)btValue, szValue, 10);
	WriteString(szEntry, szValue);
	}

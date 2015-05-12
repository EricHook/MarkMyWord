#ifndef _inifile
#define _inifile

#include "vinifile.h"

class IniFile : public VIniFile
	{
	public:
		IniFile(char szNewIniFilename[], char szNewIniSectionname[]);
		~IniFile();
		void ReadString(char szEntry[], char szDefault[], char szResult[], int iResultLength);
		void WriteString(char szEntry[], char szValue[]);
	};

#endif // _inifile
#ifndef _vinifile
#define _vinifile

#include "object.h"

class VIniFile : public Object
	{
	public:
		VIniFile(char szNewIniFilename[], char szNewIniSectionname[]);
		~VIniFile();
		int ReadValue(char szEntry[], int defaultValue);
		void WriteValue(char szEntry[], int value);
		float ReadValue(char szEntry[], float defaultValue);
		void WriteValue(char szEntry[], float value);
		DWORD ReadValue(char szEntry[], DWORD dwDefault);
		void WriteValue(char szEntry[], DWORD dwValue);
		WORD ReadValue(char szEntry[], WORD wDefault);
		void WriteValue(char szEntry[], WORD wValue);
		BYTE ReadValue(char szEntry[], BYTE btDefault);
		void WriteValue(char szEntry[], BYTE btValue);

		// Pure Virtuals:
		virtual void ReadString(char szEntry[], char szDefault[], char szResult[], int iResultLength) = 0;
		virtual void WriteString(char szEntry[], char szValue[]) = 0;

	protected:
		char szIniFilename[255];
		char szIniSectionname[255];
	};

#endif // _vinifile

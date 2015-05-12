#ifndef _sendmail
#define _sendmail

#ifdef _WIN32
	#include "winos.h"
	#include <winsock.h>
#endif

#include "cmBase.h"
#include "cmMsg.h"
#include "cmConn.h"
#include "cmConMgr.h"

#ifdef _WIN32
	#include "mapi.h"
#endif

//
class TSendMail
{
protected:
	#ifdef _WIN32
		LPMAPISENDMAIL	lpfnMapiSendMail;
		HINSTANCE		hMapiDll;
	#endif

public:
	TSendMail();
	~TSendMail();
	int SendMail(char *recipient, char *subject, char *text);
	bool Create();

};

//
int SendMail(const char *sender, char *recipient, char *subject, char *text);

#endif

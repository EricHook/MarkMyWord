#include "sendmail.h"

//
TSendMail::TSendMail()
{
#ifdef _WIN32
	hMapiDll			= NULL;
	lpfnMapiSendMail	= NULL;
#endif
}

//
TSendMail::~TSendMail()
{
#ifdef _WIN32
	if (hMapiDll)
		FreeLibrary(hMapiDll);
#endif
}

//
bool TSendMail::Create()
{
#ifdef _WIN32
	ASSERT(_WARN_, !hMapiDll);

	if (hMapiDll)
		return true;

	hMapiDll = LoadLibrary("MAPI32");

	if (!hMapiDll)
		return false;

	lpfnMapiSendMail = (LPMAPISENDMAIL)GetProcAddress(hMapiDll, "MAPISendMail");
	if (!lpfnMapiSendMail)
		return false;
#endif

	return true;
}

//
int TSendMail::SendMail(char *recipient, char *subject, char *text)
{
	return 0;
}


#ifdef _WIN32

//
int SendMail(const char * /* sender */, char *recipient, char *subject, char *text)
{
	LPMAPISENDMAIL	lpfnMapiSendMail;
	HINSTANCE		hMapiDll;
	MapiMessage m;
	MapiRecipDesc recip;
	char recipientAddress[256];

	memset(&m,		0, sizeof(MapiMessage));
	memset(&recip,	0, sizeof(MapiRecipDesc));

	hMapiDll = LoadLibrary("MAPI32");

	if (!hMapiDll)
		return 0;

	lpfnMapiSendMail = (LPMAPISENDMAIL)GetProcAddress(hMapiDll, "MAPISendMail");
	if (!lpfnMapiSendMail)
		return 0;

	recip.ulRecipClass = MAPI_TO;
	recip.lpszName = recipient;

	sprintf(recipientAddress, "SMTP:%s", recipient);
	recip.lpszAddress = recipientAddress;
	m.lpszSubject = subject;
	m.lpszNoteText = text;
	m.nRecipCount = 1;
	m.lpRecips = &recip;

	ULONG rv = lpfnMapiSendMail(0, 0, &m, 0, 0);

	TRACE("SendMail() call returned %d.\n", rv);
	
	FreeLibrary(hMapiDll);
	return 0;
}

#else

#include <pthread.h>

//
void *SendMailThreadProc(void *arg)
{
	char *block = (char *)arg;
	FILE *f;
	
	f = popen("/usr/lib/sendmail -oi -t", "w");
	
	if (f != NULL) {
		fprintf(f, block);
		pclose(f);
	}
	
	delete[] block;
	
	return NULL;
}

int SendMail(const char *sender, char *recipient, char *subject, char *text)
{
	char *block;
	// int error;
	// pthread_t thread;
	
	if (fork() != 0)
		return 0; // I am the parent
	
	// else, I am the child
	
	if (strlen(text) > 10000)
		text[10000] = 0;
	
	block = new char[15000];
	if (!block)
		return -1;
	
	sprintf(block, "From: %s\n"
				   "To: %s\n"
				   // "Reply-To: %s\n"
				   "Subject: %s\n"
				   "%s\n",
				   sender, recipient, subject, text);
	
	/*
	error = pthread_create(&thread, NULL, SendMailThreadProc, block);
	
	if (error != 0)
		delete[] block;
	
	pthread_detach(thread);
	*/
	
	SendMailThreadProc(block);
	
	exit(0); // the child dies
	
	ASSERT(_WARN_, FALSE);
	
	return 0;
}

#endif

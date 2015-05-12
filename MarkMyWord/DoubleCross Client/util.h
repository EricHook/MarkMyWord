#ifndef _util
#define _util

unsigned int random(unsigned int limit);
int random(int limit);
int random(int lowerBound, int upperBound);
void DebugString(char *lpszFormat, ...);
void ReportError(const char *fmt, ...);
void SetMainMenu(WORD menu);
Boolean URectInRect(RECT *r1, RECT *r2);
int IntersectArea(RECT *r1, RECT *r2);

// #define MB_LARGE 0x10000000L
// int SCIMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

Boolean ControlKeyDown();
Boolean ShiftKeyDown();
Boolean EscapeKeyDown();

char *MoneyToString(int n, char *s);

void GetStringDialog(char *prompt, char *result, int maxLength);
char *strnzcpy(char *s, char *t, int n);
bool strizzmember(char *s, char *member);

Boolean IHaveAHoyleCDProductInstalled(char *s1, char *s2, char *s3);

void CenterHWND(HWND w, int dX, int dY);

char *GetString(int id, char *s, int maxLength = 256);

char *GetGameString(int id, char *s, int maxLength = 256);

#endif	// _util

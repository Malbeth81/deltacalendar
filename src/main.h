#ifndef MAINH
#define	MAINH

#include "themes.h"

extern HINSTANCE hInstance;
extern HWND hWindow;
extern HMENU hMenu;

extern bool Autorun;
extern bool AutoUpdate;
extern bool StayOnTop;
extern bool SnapToScreen;
extern bool ShowTrayIcon;
extern bool ShowWeekDays;
extern bool ShowWeekNums;
extern bool ShowMemo;
extern unsigned short StartOfWeek;
extern unsigned short DateFormat;
extern char* LanguageFile;

extern TTheme Theme;
extern TThemeList ThemeList;
extern SYSTEMTIME CurrentTime;
extern SYSTEMTIME DisplayTime;

void TopMost(bool Enabled);
void UpdateRegion();

#endif

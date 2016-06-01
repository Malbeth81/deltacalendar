#ifndef TOOLSH
#define	TOOLSH

#define fsBold      1
#define fsItalic    2
#define fsUnderline 4

typedef enum {aLeft, aCenter, aRight} TAlignment;
typedef enum {eNone, eOutline, eShadow} TEffect;
typedef unsigned short int TFontStyle;

int Ceil(const float X);
unsigned short DayOfWeek(SYSTEMTIME* Date);
unsigned short DayOfYear(SYSTEMTIME Date);
unsigned short DaysInAMonth(unsigned short Year, unsigned short Month);
HFONT EasyCreateFont(HDC DC, LPCTSTR szFontName, UINT nPointSize, TFontStyle Style);
unsigned short FirstWeekdayOfMonth(SYSTEMTIME Date);
unsigned short FirstWeekdayOfYear(SYSTEMTIME Date);
char* FormatDate(unsigned short Format, SYSTEMTIME Date);
char* GetWindowText(HWND hWnd);
char* IntToStr(int Number);
int Max(const int a, const int b);
RECT Rect(const int Left, const int Top, const int Right, const int Bottom);
int SubStrPos(const char* Str, unsigned int Index);
int SubStrPos(const char* Str, unsigned int Index, const char Separator);
int SubStrLen(const char *Str, const unsigned int Pos);
int SubStrLen(const char *Str, const unsigned int Pos, const char Separator);
char* SubStr(const char* Source, unsigned int Index);
char* SubStr(const char* Source, unsigned int Index, const char Separator);
unsigned int WeekOfYear(SYSTEMTIME Date);

#endif

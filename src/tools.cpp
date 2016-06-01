#include "themes.h"

int Ceil(const float X)
{
  int Value = (int)X;
  if (X-Value > 0)
    Value = Value+1;
  return Value;
}

unsigned short DayOfWeek(SYSTEMTIME* Date)
{
  FILETIME fDate;
  SystemTimeToFileTime(Date, &fDate);
  FileTimeToSystemTime(&fDate, Date);
  return Date->wDayOfWeek;
}

unsigned short DayOfYear(SYSTEMTIME Date)
{
  unsigned short Day = Date.wDay;
  for (unsigned short i = Date.wMonth-1; i > 0; i--)
    Day += DaysInAMonth(Date.wYear, i);
  return Day;
}

unsigned short DaysInAMonth(unsigned short Year, unsigned short Month)
{
  const short MonthDays[2][12] = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                                  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
  bool LeapYear = (Year%4 == 0) && ((Year%100 != 0) || (Year%400 == 0));
  return MonthDays[(Month == 2 && LeapYear)][Month-1];
}

HFONT EasyCreateFont(HDC DC, LPCTSTR szFontName, UINT nPointSize, TFontStyle Style)
{
  return CreateFont(-MulDiv(nPointSize, GetDeviceCaps(DC, LOGPIXELSY), 72),
         0, 0, 0, (Style & fsBold) == fsBold ? FW_BOLD : FW_NORMAL,
         (Style & fsItalic) == fsItalic, (Style & fsUnderline) == fsUnderline, false,
         DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
         DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,szFontName);
}

unsigned short FirstWeekdayOfMonth(SYSTEMTIME Date)
{
  FILETIME fDate;
  Date.wDay = 1;
  SystemTimeToFileTime(&Date, &fDate);
  FileTimeToSystemTime(&fDate, &Date);
  return Date.wDayOfWeek;
}

unsigned short FirstWeekdayOfYear(SYSTEMTIME Date)
{
  FILETIME fDate;
  Date.wDay = 1;
  Date.wMonth = 1;
  SystemTimeToFileTime(&Date, &fDate);
  FileTimeToSystemTime(&fDate, &Date);
  return Date.wDayOfWeek;
}

char* FormatDate(unsigned short Format, SYSTEMTIME Date)
{
  char* Months[12] = {"January", "Febuary", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
  char* ShortMonths[12] = {"Jan.", "Feb.", "Mar.", "Apr.", "May", "June", "July", "Aug.", "Sept.", "Oct.", "Nov.", "Dec."};
  char* Str = new char[20];
  switch (Format)
  {
  case 0:
    strcpy(Str, IntToStr(Date.wDay));
    strcat(Str, "/");
    strcat(Str, IntToStr(Date.wMonth));
    strcat(Str, "/");
    strcat(Str, IntToStr(Date.wYear));
     break;
  case 1:
    strcpy(Str, IntToStr(Date.wYear));
    strcat(Str, "/");
    strcat(Str, IntToStr(Date.wMonth));
    strcat(Str, "/");
    strcat(Str, IntToStr(Date.wDay));
    break;
  case 2:
    strcpy(Str, IntToStr(Date.wDay));
    strcat(Str, "-");
    strcat(Str, IntToStr(Date.wMonth));
    strcat(Str, "-");
    strcat(Str, IntToStr(Date.wYear));
    break;
  case 3:
    strcpy(Str, IntToStr(Date.wYear));
    strcat(Str, "-");
    strcat(Str, IntToStr(Date.wMonth));
    strcat(Str, "-");
    strcat(Str, IntToStr(Date.wDay));
    break;
  case 4:
    strcpy(Str, Months[Date.wMonth-1]);
    strcat(Str, " ");
    strcat(Str, IntToStr(Date.wDay));
    strcat(Str, ", ");
    strcat(Str, IntToStr(Date.wYear));
    break;
  case 5:
    strcpy(Str, IntToStr(Date.wDay));
    strcat(Str, " ");
    strcat(Str, Months[Date.wMonth-1]);
    strcat(Str, " ");
    strcat(Str, IntToStr(Date.wYear));
    break;
  case 6:
    strcpy(Str, ShortMonths[Date.wMonth-1]);
    strcat(Str, " ");
    strcat(Str, IntToStr(Date.wDay));
    strcat(Str, ", ");
    strcat(Str, IntToStr(Date.wYear));
    break;
  case 7:
    strcpy(Str, IntToStr(Date.wDay));
    strcat(Str, " ");
    strcat(Str, ShortMonths[Date.wMonth-1]);
    strcat(Str, " ");
    strcat(Str, IntToStr(Date.wYear));
    break;
  }
  return Str;
}

char* GetWindowText(HWND hWnd)
{
  int Len = GetWindowTextLength(hWnd)+1;
  char* Str = new char[Len];
  GetWindowText(hWnd, Str, Len);
  return Str;
}

char* IntToStr(int Number)
{
  char* Str = new char[sizeof(int)*8+1];
  itoa(Number, Str, 10);
  return Str;
}

int Max(const int a, const int b)
{
  if (a > b)
    return a;
  return b;
}

RECT Rect(const int Left, const int Top, const int Right, const int Bottom)
{
  RECT R;
  R.left = Left;
  R.top = Top;
  R.bottom = Bottom;
  R.right = Right;
  return R;
}

int SubStrPos(const char* Str, unsigned int Index)
{
  int i = 0;
  while (i < strlen(Str) && Index > 0)
  {
    if (i < strlen(Str)-1 && Str[i] == '\n' && Str[i+1] == '\r')
    {
      Index--;
      i++;
    }
    else if (Str[i] == '\n' || Str[i] == '\r')
      Index--;
    i++;
  }
  if (Index > 0)
    return -1;
  else
    return i;
}

int SubStrPos(const char* Str, unsigned int Index, const char Separator)
{
  int i = 0;
  while (i < strlen(Str) && Index > 0)
  {
    if (Str[i] == Separator)
      Index--;
    i++;
  }
  if (Index > 0)
    return -1;
  else
    return i;
}

int SubStrLen(const char *Str, const unsigned int Pos)
{
  int i = 0;
  while (i+Pos <= strlen(Str))
    if (Str[i+Pos] == '\n' || Str[i+Pos] == '\r')
      return i;
    else
      i++;
  return i;
}

int SubStrLen(const char *Str, const unsigned int Pos, const char Separator)
{
  int i = 0;
  while (i+Pos < strlen(Str))
    if (Str[i+Pos] == Separator)
      return i;
    else
      i++;
  return i;
}

char* SubStr(const char* Source, unsigned int Index)
{
  int Pos = SubStrPos(Source, Index);
  if (Pos >= 0)
  {
    int Len = SubStrLen(Source, Pos);
    char* Str = new char[Len+1];
    strncpy(Str, &Source[Pos], Len);
    Str[Len] = '\0';
    return Str;
  }
  return NULL;
}

char* SubStr(const char* Source, unsigned int Index, const char Separator)
{
  int Pos = SubStrPos(Source, Index, Separator);
  if (Pos >= 0)
  {
    int Len = SubStrLen(Source, Pos, Separator);
    char* Str = new char[Len+1];
    strncpy(Str, &Source[Pos], Len);
    Str[Len] = '\0';
    return Str;
  }
  return NULL;
}

unsigned int WeekOfYear(SYSTEMTIME Date)
{
  SYSTEMTIME First;
  unsigned short Day = DayOfYear(Date);
  unsigned short Week = 0;
  
  if (Date.wDayOfWeek > 0)
    Day += 7-Date.wDayOfWeek;
    
  while (Day > 7)
  {
    Week++;
    Day -= 7;
  }
  First.wDay = 1;
  First.wMonth = 1;
  First.wYear = Date.wYear;
  switch (DayOfWeek(&First))
  {
    case 5:                      
    case 6:
    case 0:
      break;
    default:          
      Week++;
  }
  if (Week == 0)
  {   
    First.wDay = 31;
    First.wMonth = 12;
    First.wYear = Date.wYear-1;
    return WeekOfYear(First);
  }
  if (Date.wDayOfWeek >= 1 && Date.wMonth == 12 && Date.wDay >= 31-Date.wDayOfWeek)
    Week = 1;
  return Week;
}
                   /*
function TextHeight(DC: HDC; Text: PChar): Integer;
var
  Dim: SIZE;
begin
  GetTextExtentPoint32(DC, Text, Length(Text), Dim);
  Result := Dim.cx;
end;

function TextWidth(DC: HDC; Text: PChar): Integer;
var
  Dim: SIZE;
begin
  GetTextExtentPoint32(DC, Text, Length(Text), Dim);
  Result := Dim.cy;
end;
*/

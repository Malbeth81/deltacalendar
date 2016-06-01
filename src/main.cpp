#include <windows.h>
#include "resource.h"
#include "themes.h"
#include "systray.h"
#include "menu.h"
#include "tools.h"
#include "config.h"
#include "about.h"

const char ClassName[] = "DeltaCalendar";

HINSTANCE hInstance;
HWND hWindow;

bool Autorun;
bool AutoUpdate;
bool StayOnTop;
bool SnapToScreen;
bool ShowTrayIcon;
bool ShowWeekDays;
bool ShowWeekNums;
bool ShowMemo;
unsigned short StartOfWeek;
unsigned short DateFormat;
char* LanguageFile;

TTheme Theme;
TThemeList ThemeList;
SYSTEMTIME CurrentTime;
SYSTEMTIME DisplayTime;

char Weekdays[] = "SMTWTFS";
unsigned short FirstWeekday;
unsigned short TotalDays;
unsigned short TotalRows;
RECT RCalendar, RDate, RWeekday, RWeeknum, RDay, RMemo;

// PRIVATE FUNCTIONS -----------------------------------------------------------

void CalculateData(HDC DC, PTheme Theme)
{
  HFONT OldFont;
  SIZE Dim;
  int YearOffset, MonthOffset, DayOffset,
  DateHeight, WeekdayHeight, WeeknumWidth,
  DayHeight, DayWidth/*, MemoHeight, MemoWidth*/;
//  i: Integer;
  /* Calculate calendar data */
  YearOffset = DisplayTime.wYear-CurrentTime.wYear;
  MonthOffset = DisplayTime.wMonth-CurrentTime.wMonth;
  DayOffset = DisplayTime.wDay-CurrentTime.wDay;
  GetLocalTime(&CurrentTime);
  DisplayTime.wYear = CurrentTime.wYear+YearOffset;
  DisplayTime.wMonth = CurrentTime.wMonth+MonthOffset;
  DisplayTime.wDay = CurrentTime.wDay+DayOffset;
  FirstWeekday = FirstWeekdayOfMonth(DisplayTime);
  TotalDays = DaysInAMonth(DisplayTime.wYear, DisplayTime.wMonth);
  TotalRows = Ceil(((7+FirstWeekday-StartOfWeek)%7+TotalDays)/7.0);

  /* Size of date region */
  OldFont = (HFONT)SelectObject(DC, EasyCreateFont(DC, Theme->Date.FontName, Theme->Date.FontSize, Theme->Date.FontStyle));
  GetTextExtentPoint32(DC, "July, 1st", 9, &Dim);
  DeleteObject(SelectObject(DC, OldFont));
  DateHeight = Dim.cy+4+Theme->Date.Margin*2;

  /* Size of weekday region */
  OldFont = (HFONT)SelectObject(DC, EasyCreateFont(DC, Theme->Weekday.FontName, Theme->Weekday.FontSize, Theme->Weekday.FontStyle));
  GetTextExtentPoint32(DC, "88", 2, &Dim);
  DeleteObject(SelectObject(DC, OldFont));
  WeekdayHeight = Dim.cy+2+Theme->Weekday.Margin*2;
  WeeknumWidth = Dim.cx+4+Theme->Weekday.Margin*2;

  /* Size of day region */
  OldFont = (HFONT)SelectObject(DC, EasyCreateFont(DC, Theme->Day.FontName, Theme->Day.FontSize, Theme->Day.FontStyle));
  GetTextExtentPoint32(DC, "88", 2, &Dim);
  DeleteObject(SelectObject(DC, OldFont));
  DayHeight = Dim.cy;
  DayWidth = Dim.cx;
  OldFont = (HFONT)SelectObject(DC, EasyCreateFont(DC, Theme->Today.FontName, Theme->Today.FontSize, Theme->Today.FontStyle));
  GetTextExtentPoint32(DC, "88", 2, &Dim);
  DeleteObject(SelectObject(DC, OldFont));
  DayHeight = Max(Dim.cy, DayHeight);
  DayWidth = Max(Dim.cx, DayHeight);
  OldFont = (HFONT)SelectObject(DC, EasyCreateFont(DC, Theme->Weekend.FontName, Theme->Weekend.FontSize, Theme->Weekend.FontStyle));
  GetTextExtentPoint32(DC, "88", 2, &Dim);
  DeleteObject(SelectObject(DC, OldFont));
  DayHeight = Max(Dim.cy, DayHeight);
  DayWidth = Max(Dim.cx, DayHeight);
  OldFont = (HFONT)SelectObject(DC, EasyCreateFont(DC, Theme->MemoDay.FontName, Theme->MemoDay.FontSize, Theme->MemoDay.FontStyle));
  GetTextExtentPoint32(DC, "88", 2, &Dim);
  DeleteObject(SelectObject(DC, OldFont));
  DayHeight = Max(Dim.cy, DayHeight);
  DayWidth = Max(Dim.cx, DayHeight);
  DayHeight = DayHeight+2+Theme->Day.Margin*2;
  DayWidth = DayWidth+4+Theme->Day.Margin*2;

  /* Size of memo region
  OldFont = (HFONT)SelectObject(DC, CreateEasyFont(DC, PChar(Memo.FontName), Memo.FontSize, 0, Memo.FontStyle));
  GetTextExtentPoint32(DC, 'July', 2, Dim);
  DeleteObject(SelectObject(DC, OldFont));
  i := Memos.Find(Today);
  if frmConfig.ShowTodaysMemo and (Index >= 0) then
  begin
    Strings.Text := frmConfig.Memos.Get(Index).Text;
    MemoHeight := (Canvas.TextHeight('July')*Strings.Count)+Strings.Count+4+Scheme.Memo.Margin*2;
    MemoWidth := DaysWidth;
    if frmConfig.ShowWeekNumbers then
      MemoWidth := MemoWidth+Scheme.Weekday.Spacing+WeekNumWidth;
  end
  else
    MemoHeight := 0;    */

  /* Calculate regions */
  RDate.left = Theme->Calendar.Margin+1;
  RDate.top = Theme->Calendar.Margin+1;
  RDate.right = RDate.left+DayWidth*7+Theme->Day.Spacing*6;
  if (ShowWeekNums)
    RDate.right = RDate.right+WeeknumWidth+Theme->Weekday.Spacing;
  RDate.bottom = RDate.top+DateHeight;

  RWeekday.left = RDate.left;
  if (ShowWeekNums)
    RWeekday.left = RWeekday.left+Theme->Weekday.Spacing+WeeknumWidth;
  RWeekday.top = RDate.bottom+Theme->Date.Spacing;
  RWeekday.right = RWeekday.left+DayWidth;
  RWeekday.bottom = RWeekday.top;
  if (ShowWeekDays)
    RWeekday.bottom = RWeekday.bottom+WeekdayHeight;

  RWeeknum.left = RDate.left;
  RWeeknum.top = RDate.bottom+Theme->Date.Spacing;
  if (ShowWeekDays)
    RWeeknum.top = RWeeknum.top+Theme->Weekday.Spacing+WeekdayHeight;
  RWeeknum.right = RWeeknum.left;
  if (ShowWeekNums)
    RWeeknum.right = RWeeknum.right+WeeknumWidth;
  RWeeknum.bottom = RWeeknum.top+DayHeight;

  RDay.left = RWeeknum.right+Theme->Weekday.Spacing;
  RDay.top = RWeekday.bottom+Theme->Weekday.Spacing;
  RDay.right = RDay.left+DayWidth;
  RDay.bottom = RDay.top+DayHeight; 

  RCalendar.left = 0;
  RCalendar.top = 0;
  RCalendar.right = RDay.left+DayWidth*7+Theme->Day.Spacing*6+Theme->Calendar.Margin+1;
  RCalendar.bottom = RDay.top+DayHeight*TotalRows+Theme->Day.Spacing*(TotalRows-1)+Theme->Calendar.Margin+1;
}

void DrawFrame(HDC DC, RECT R, PSettings Settings)
{
  HBRUSH OldBrush;
  HPEN OldPen;
  if (Settings->Background)
    OldBrush = (HBRUSH)SelectObject(DC, CreateSolidBrush(Settings->BackgroundColor));
  else
    OldBrush = (HBRUSH)SelectObject(DC, (HBRUSH)GetStockObject(HOLLOW_BRUSH));
  if (Settings->Frame)
    OldPen = (HPEN)SelectObject(DC, CreatePen(PS_SOLID, 0, Settings->FrameColor));
  else
    OldPen = (HPEN)SelectObject(DC, CreatePen(PS_SOLID, 0, Settings->BackgroundColor));
  if (Settings->Background || Settings->Frame)
    RoundRect(DC, R.left, R.top, R.right, R.bottom, Settings->FrameRoundness, Settings->FrameRoundness);
  DeleteObject(SelectObject(DC, OldBrush));
  DeleteObject(SelectObject(DC, OldPen));
}

void DrawText(HDC DC, char* Text, RECT R, PSettings Settings)
{
  HFONT OldFont;
  SIZE Dim;
  int X, Y;
  SetBkMode(DC, TRANSPARENT);
  OldFont = (HFONT)SelectObject(DC, EasyCreateFont(DC, Settings->FontName, Settings->FontSize, Settings->FontStyle));
  GetTextExtentPoint32(DC, Text, strlen(Text), &Dim);
  switch (Settings->Alignment)
  {
  case aRight:
    X = R.right-Dim.cx-(Settings->Effect != eNone ? 1 : 0);
    break;
  case aCenter:
    X = R.left+(R.right-R.left-Dim.cx)/2;
    break;
  default:
    X = R.left+1+(Settings->Effect == eOutline ? 1 : 0);
  }
  Y = R.top;
  switch (Settings->Effect)
  {
  case eOutline:
    SetTextColor(DC, Settings->EffectColor);
    ExtTextOut(DC, X+1, Y, ETO_CLIPPED, &R, Text, strlen(Text), NULL);
    ExtTextOut(DC, X-1, Y, ETO_CLIPPED, &R, Text, strlen(Text), NULL);
    ExtTextOut(DC, X, Y+1, ETO_CLIPPED, &R, Text, strlen(Text), NULL);
    ExtTextOut(DC, X, Y-1, ETO_CLIPPED, &R, Text, strlen(Text), NULL);
    ExtTextOut(DC, X+1, Y-1, ETO_CLIPPED, &R, Text, strlen(Text), NULL);
    ExtTextOut(DC, X+1, Y+1, ETO_CLIPPED, &R, Text, strlen(Text), NULL);
    ExtTextOut(DC, X-1, Y+1, ETO_CLIPPED, &R, Text, strlen(Text), NULL);
    ExtTextOut(DC, X-1, Y-1, ETO_CLIPPED, &R, Text, strlen(Text), NULL);
    break;
  case eShadow:
    SetTextColor(DC, Settings->EffectColor);
    ExtTextOut(DC, X+1, Y+1, ETO_CLIPPED, &R, Text, strlen(Text), NULL);
    break;
  }
  SetTextColor(DC, Settings->FontColor);
  TextOut(DC, X, Y, Text, lstrlen(Text));
  DeleteObject(SelectObject(DC, OldFont));
}

void DrawCalendar(HDC DC, PTheme Theme)
{
  HBRUSH OldBrush;
  HPEN OldPen;
  RECT R;
  char* Str;
  /* Clear the window */
  OldBrush = (HBRUSH)SelectObject(DC, CreateSolidBrush(0x00000000));
  OldPen = (HPEN)SelectObject(DC, CreatePen(PS_SOLID, 0, 0x00000000));
  Rectangle(DC, 0, 0, RCalendar.right, RCalendar.bottom);
  DeleteObject(SelectObject(DC, OldBrush));
  DeleteObject(SelectObject(DC, OldPen));
  /* Display frame */
  DrawFrame(DC, RCalendar, &Theme->Calendar);
  /* Display date */
  DrawFrame(DC, RDate, &Theme->Date);
  Str = FormatDate(DateFormat, DisplayTime);
  DrawText(DC, Str, Rect(RDate.left+1,RDate.top+1+Theme->Date.Margin,RDate.right-1,RDate.bottom-1-Theme->Date.Margin), &Theme->Date);
  /* Display weekdays */
  R = RWeekday;
  if (ShowWeekDays)
    for (int i = 0; i < 7; i++)
    {
      DrawFrame(DC, R, &Theme->Weekday);
      Str = new char[2];
      Str[0] = Weekdays[(i+StartOfWeek)%7];
      Str[1] = '\0';
      DrawText(DC, Str, Rect(R.left+1, R.top+1+Theme->Weekday.Margin, R.right-1, R.bottom-1-Theme->Weekday.Margin), &Theme->Weekday);
      R.left = R.right+Theme->Weekday.Spacing;
      R.right = R.left+RWeekday.right-RWeekday.left;
    }
  /* Display week numbers */
  R = RWeeknum;
  int Week = WeekOfYear(DisplayTime);
  if (ShowWeekNums)
    for (int i = 0; i < TotalRows; i++)
    {
      DrawFrame(DC, R, &Theme->Weekday);
      DrawText(DC, IntToStr(Week+i), Rect(R.left+1, R.top+1+Theme->Weekday.Margin, R.right-1, R.bottom-1-Theme->Weekday.Margin), &Theme->Weekday);
      R.top = R.bottom+Theme->Weekday.Spacing;
      R.bottom = R.top+RWeeknum.bottom-RWeeknum.top;
    }
  /* Display days */
  R = RDay;
  R.left += (RDay.right-RDay.left+Theme->Day.Spacing)*((7+FirstWeekday-StartOfWeek)%7);
  R.right = R.left+RDay.right-RDay.left;
  for (int i = 1; i <= TotalDays; i++)
  {
    Str = IntToStr(i);
    DrawFrame(DC, R, &Theme->Day);
    DrawText(DC, Str, Rect(R.left+1, R.top+1, R.right-1, R.bottom-1),&Theme->Day);
    if ((i+FirstWeekday-StartOfWeek)%7 == 0)
    {
      R.left = RDay.left;
      R.right = RDay.right;
      R.top = R.bottom+Theme->Day.Spacing;
      R.bottom = R.top+RDay.bottom-RDay.top;
    }
    else
    {
      R.left = R.right+Theme->Day.Spacing;
      R.right = R.left+RDay.right-RDay.left;
    }
  }
}

// PUBLIC FUNCTIONS ------------------------------------------------------------

void TopMost(bool Enabled)
{
  StayOnTop = Enabled;
  SetWindowPos(hWindow, StayOnTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE or SWP_NOSIZE);
  CheckMenuItem(hMenu, ID_TOPMOSTMENU, StayOnTop ? MF_CHECKED : MF_UNCHECKED);
}

void UpdateRegion()
{
  RECT R;
  TTheme MaskTheme = Theme;                       /* Creates mask theme */
  CreateMaskTheme(&MaskTheme);
  HDC MaskDC = CreateCompatibleDC(NULL);          /* Creates mask DC */
  CalculateData(MaskDC, &MaskTheme);
  HBITMAP Mask = CreateCompatibleBitmap(MaskDC, RCalendar.right, RCalendar.bottom);
  SelectObject(MaskDC, Mask);
  DrawCalendar(MaskDC, &MaskTheme);               /* Paints mask */
  HRGN Region1 = CreateRectRgn(0,0,0,0);
  HRGN Region2;
  for (int i = 0; i < RCalendar.right; i++)
    for (int j = 0; j < RCalendar.bottom; j++)
      if (GetPixel(MaskDC, i, j) > 0x00000000)    /* Calculate region */
      {
        Region2 = CreateRectRgn(i, j, i+1, j+1);
        CombineRgn(Region1,Region1,Region2,RGN_OR);
        DeleteObject(Region2);
      }
  DeleteObject(Mask);
  DeleteDC(MaskDC);
  SetWindowRgn(hWindow, Region1, TRUE);           /* Change region and size */
  GetWindowRect(hWindow, &R);
  SetWindowPos(hWindow, 0, R.left, R.top, RCalendar.right+1, RCalendar.bottom+1, SWP_NOACTIVATE | SWP_NOZORDER);
  if (IsWindowVisible(hWindow))
    InvalidateRect(hWindow, NULL, FALSE);         /* Repaints the window */
}

// EVENT FUNCTIONS -------------------------------------------------------------

void OnPaint()
{
  PAINTSTRUCT PS;
  HDC DC;
  DC = BeginPaint(hWindow, &PS);
  DrawCalendar(DC, &Theme);
  EndPaint(hWindow, &PS);
}

void OnPosChanging(LPWINDOWPOS WndPos)
{
  if (SnapToScreen)
  {
    RECT DeskArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &DeskArea, 0);
    if (WndPos->x <= DeskArea.left+8)
      WndPos->x = DeskArea.left;
    if (WndPos->x+WndPos->cx >= DeskArea.right-8)
      WndPos->x = Max(0, DeskArea.right-WndPos->cx);
    if (WndPos->y <= DeskArea.top+8)
      WndPos->y = DeskArea.top;
    if (WndPos->y+WndPos->cy >= DeskArea.bottom-8)
      WndPos->y = Max(0, DeskArea.bottom-WndPos->cy);
  }
}

// WINAPI FUNCTIONS ------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  static UINT uTaskbarCreated;
  switch (Message)
  {
  case WM_COMMAND:        /* Process control messages */
    if (HIWORD(wParam) == 0)    
      MenuProc (LOWORD(wParam));
    return 0;
  case WM_CREATE:
    uTaskbarCreated = RegisterWindowMessage("TaskbarCreated");
    return 0;
  case WM_DESTROY:
    SysTrayIconVisible(false);
    PostQuitMessage(0);   /* Stops the GetMessage loop of WinMain() */
    return 0;
  case WM_NCHITTEST:
    return HTCAPTION;
  case WM_NCRBUTTONUP: 
    OpenMenu(LOWORD(lParam), HIWORD(lParam));
    return 0;
  case WM_PAINT:
    OnPaint();
    return 0;
  case WM_SHELLTRAYICON:
    SysTrayIconProc(LOWORD(lParam));
    return 0;
  case WM_WINDOWPOSCHANGING:
    OnPosChanging((WINDOWPOS*)lParam);
    break;
  }
  if (Message == uTaskbarCreated)
    SysTrayIconVisible(ShowTrayIcon);
  else
    return DefWindowProc(hWnd, Message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nShow)
{
  WNDCLASSEX WndClass;
  int ExStyle;
  MSG Messages;
  hInstance = hInst;
  /* Load options */
  StayOnTop = false;
  SnapToScreen = true;
  ShowTrayIcon = true;
  ShowWeekDays = true;
  ShowWeekNums = true;
  DateFormat = 0;
  StartOfWeek = 0;
  /* Checks if another instance is already running */
  HWND Inst = FindWindow(ClassName, NULL);
  if (Inst > 0)
  {
    SetForegroundWindow(Inst);
    PostQuitMessage(0);
    return 0;
  }
  /* Specify the window class information */
  WndClass.cbSize = sizeof(WNDCLASSEX);
  WndClass.lpszClassName = ClassName;
  WndClass.hInstance = hInstance;
  WndClass.lpfnWndProc = WindowProc;
  WndClass.style = CS_DBLCLKS;
  WndClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
  WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_MAINICON));
  WndClass.hIconSm = NULL;
  WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  WndClass.lpszMenuName = NULL;
  WndClass.cbClsExtra = 0;
  WndClass.cbWndExtra = 0;
  /* Register and create the window */
  if (!RegisterClassEx(&WndClass))
    return 0;
  ExStyle = WS_EX_TOOLWINDOW;
  if (StayOnTop)
    ExStyle = ExStyle || WS_EX_TOPMOST;
  hWindow = CreateWindowEx(ExStyle, ClassName, "Windows App", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
                           0, 0, NULL, NULL, hInstance, NULL);
  CreatePopMenu();
  if (ShowTrayIcon)
    SysTrayIconVisible(ShowTrayIcon);
  /* Loads or creates default theme */
  if (ThemeList.LoadFromFile("Themes.dat") && ThemeList.GetCount() > 0)
    CopyTheme(&Theme, ThemeList.GetFirst());
  else
  {
    CreateDefaultTheme(&Theme);
    ThemeList.Add(&Theme);
  }
  /* Updates and displays the window */
  UpdateRegion();
  ShowWindow(hWindow, nShow);
  while(GetMessage(&Messages, NULL, 0, 0))
  {
    TranslateMessage(&Messages);
    DispatchMessage(&Messages);
  }
  /* Saves the themes */
  CopyTheme(ThemeList.GetFirst(), &Theme);
  ThemeList.SaveToFile("Themes.dat");
  return Messages.wParam;
}


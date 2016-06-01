#include <windows.h>
#include <prsht.h>
#include "resource.h"
#include "colorbutton.h"
#include "themes.h"
#include "systray.h"
#include "menu.h"
#include "main.h"

unsigned short Index = 0;
bool RegionChanged1 = false;
bool RegionChanged2 = false;

// PRIVATE FUNCTIONS -----------------------------------------------------------

PSettings Settings(unsigned short Index = Index)
{
  switch (Index)
  {
    case 0: return &Theme.Calendar;
    case 1: return &Theme.Date;
    case 2: return &Theme.Weekday;
    case 3: return &Theme.Weeknum;
    case 4: return &Theme.Day;
    case 5: return &Theme.Weekend;
    case 6: return &Theme.Today;
    case 7: return &Theme.MemoDay;
    case 8: return &Theme.Memo;
    default: return NULL;
  }
}

void FillList(HWND hDlg, int ID, char* Text)
{
  int i = 0;
  char* Str = "";
  while (Str != NULL)
  {
    Str = SubStr(Text, i, ';');
    if (Str != NULL)
      SendDlgItemMessage(hDlg, ID, CB_ADDSTRING, 0, (LPARAM)Str);
    i++;
  }
}

int CALLBACK FontComboProc(const LOGFONTA *LogFont, const TEXTMETRICA *Metric, DWORD FontType, LPARAM lParam)
{
  if (SendDlgItemMessage((HWND)lParam, ID_FONTCOMBO, CB_FINDSTRINGEXACT, 0, (LPARAM)LogFont->lfFaceName) == CB_ERR)
    SendDlgItemMessage((HWND)lParam, ID_FONTCOMBO, CB_ADDSTRING, 0, (LPARAM)LogFont->lfFaceName);
  return 1;
}

void ListFonts(HWND hDlg)
{
  LOGFONT LFont;
  LFont.lfCharSet = DEFAULT_CHARSET;
  LFont.lfPitchAndFamily = FF_DONTCARE;
  *LFont.lfFaceName = 0;
  HDC DC = GetDC(0);
  EnumFontFamiliesEx(DC, &LFont, FontComboProc, (LPARAM)hDlg, 0);
  ReleaseDC(0, DC);
}

int CALLBACK FontSizeComboProc(const LOGFONTA *LogFont, const TEXTMETRICA *Metric, DWORD FontType, LPARAM lParam)
{
  static const unsigned short int TrueTypeSizes[13] = {8,9,10,11,12,14,16,18,20,22,24,26,28};
  if ((FontType & TRUETYPE_FONTTYPE) == 0)
  {
    HDC DC = GetDC(0);
    int Size = MulDiv(Metric->tmHeight-Metric->tmInternalLeading, 72, GetDeviceCaps(DC, LOGPIXELSY));
    SendDlgItemMessage((HWND)lParam, ID_FONTSIZECOMBO, CB_ADDSTRING, 0, (LPARAM)IntToStr(Size));
    ReleaseDC(0, DC);
    return 1;
  }
  else
  {
    for (int i = 0; i < 13; i++)
    {
    	char* Str = IntToStr(TrueTypeSizes[i]);
      SendDlgItemMessage((HWND)lParam, ID_FONTSIZECOMBO, CB_ADDSTRING, 0, (LPARAM)Str);
      delete[] Str;
    }
    return 0;
  }
}

void ListFontSizes(HWND hDlg, HWND hWnd)
{
  /* Gets the current value of the font combo */
  LOGFONT LFont;
  LFont.lfHeight = 0;
  LFont.lfCharSet = DEFAULT_CHARSET;
  LFont.lfPitchAndFamily = FF_DONTCARE;
  if (GetWindowText(hWnd, LFont.lfFaceName, LF_FACESIZE))
  {
    /* Gets the current value of the size combo */
    BOOL Success;
    UINT Size = GetDlgItemInt(hDlg, ID_FONTSIZECOMBO, &Success, FALSE);
    /* Clears the list */
    SendDlgItemMessage(hDlg, ID_FONTSIZECOMBO, CB_RESETCONTENT, 0, 0);
    /* Populates the list */
    HDC DC = GetDC(0);
    EnumFontFamiliesEx(DC, &LFont, FontSizeComboProc, (LPARAM)hDlg, 0);
    ReleaseDC(0, DC);
    /* Puts back the selected value */
    if (Success)
      SetDlgItemInt(hDlg, ID_FONTSIZECOMBO, Size, FALSE);
  }
}

void ListThemes(HWND hDlg)
{
  /* Clears the list */
  SendDlgItemMessage(hDlg, ID_THEMECOMBO, CB_RESETCONTENT, 0, 0);
  //SetDlgItemInt(hDlg, ID_THEMECOMBO, ThemeList.Count, FALSE);
  PTheme Ptr = ThemeList.GetFirst();
  if (Ptr != NULL)
    Ptr = Ptr->Next;
  while (Ptr != NULL)
  {
    SendDlgItemMessage(hDlg, ID_THEMECOMBO, CB_ADDSTRING, 0, (LPARAM)Ptr->Name);
    Ptr = Ptr->Next;
  }
}

void DisplaySettings(HWND hDlg)
{
  SendDlgItemMessage(hDlg, ID_ITEMCOMBO, CB_SETCURSEL, (WPARAM)Index, 0);
  SendDlgItemMessage(hDlg, ID_FONTCOMBO, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)Settings()->FontName);
  ListFontSizes(hDlg, GetDlgItem(hDlg, ID_FONTCOMBO));
  SetDlgItemInt(hDlg, ID_FONTSIZECOMBO, Settings()->FontSize, FALSE);
  SendDlgItemMessage(hDlg, ID_ALIGNCOMBO, CB_SETCURSEL, (WPARAM)Settings()->Alignment, 0);
  CheckDlgButton(hDlg, ID_BOLDBTN, (WPARAM)Settings()->FontStyle & fsBold ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hDlg, ID_ITALICBTN, (WPARAM)Settings()->FontStyle & fsItalic ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hDlg, ID_UNDERLINEBTN, (WPARAM)Settings()->FontStyle & fsUnderline ? BST_CHECKED : BST_UNCHECKED);
  SetButtonColor(GetDlgItem(hDlg, ID_FONTCOLORBTN), Settings()->FontColor);
  SendDlgItemMessage(hDlg, ID_FRAMECOMBO, CB_SETCURSEL, (WPARAM)Settings()->Frame, 0);
  SetDlgItemInt(hDlg, ID_CORNERSEDT, Settings()->FrameRoundness, FALSE);
  SetButtonColor(GetDlgItem(hDlg, ID_FRAMECOLORBTN), Settings()->FrameColor);
  SendDlgItemMessage(hDlg, ID_EFFECTCOMBO, CB_SETCURSEL, (WPARAM)Settings()->Effect, 0);
  SetButtonColor(GetDlgItem(hDlg, ID_EFFECTCOLORBTN), Settings()->EffectColor);
  SendDlgItemMessage(hDlg, ID_BACKGROUNDCOMBO, CB_SETCURSEL, (WPARAM)Settings()->Background, 0);
  SetButtonColor(GetDlgItem(hDlg, ID_BACKCOLORBTN), Settings()->BackgroundColor);
  SetDlgItemInt(hDlg, ID_MARGINEDT, Settings()->Margin, FALSE);
  SetDlgItemInt(hDlg, ID_SPACINGEDT, Settings()->Spacing, FALSE);
}

void SaveSettings(HWND hDlg)
{
  LONG Val;
  Settings()->FontName = GetWindowText(GetDlgItem(hDlg, ID_FONTCOMBO));
  Settings()->FontSize = GetDlgItemInt(hDlg, ID_FONTSIZECOMBO, NULL, FALSE);
  Settings()->Alignment = (TAlignment)SendDlgItemMessage(hDlg, ID_ALIGNCOMBO, CB_GETCURSEL, 0, 0);
  Settings()->FontStyle = (IsDlgButtonChecked(hDlg, ID_BOLDBTN) == BST_CHECKED ? fsBold : 0);
  Settings()->FontStyle |= (IsDlgButtonChecked(hDlg, ID_ITALICBTN) == BST_CHECKED ? fsItalic : 0);
  Settings()->FontStyle |= (IsDlgButtonChecked(hDlg, ID_UNDERLINEBTN) == BST_CHECKED ? fsUnderline : 0);
  Settings()->FontColor = GetButtonColor(GetDlgItem(hDlg, ID_FONTCOLORBTN));
  Val = SendDlgItemMessage(hDlg, ID_FRAMECOMBO, CB_GETCURSEL, 0, 0);
  Settings()->Frame = (Val == CB_ERR ? 0 : Val);
  Settings()->FrameRoundness = GetDlgItemInt(hDlg, ID_CORNERSEDT, NULL, FALSE);
  Settings()->FrameColor = GetButtonColor(GetDlgItem(hDlg, ID_FRAMECOLORBTN));
  Val = SendDlgItemMessage(hDlg, ID_EFFECTCOMBO, CB_GETCURSEL, 0, 0);
  Settings()->Effect = (TEffect)(Val == CB_ERR ? 0 : Val);
  Settings()->EffectColor = GetButtonColor(GetDlgItem(hDlg, ID_EFFECTCOLORBTN));
  Val = SendDlgItemMessage(hDlg, ID_BACKGROUNDCOMBO, CB_GETCURSEL, 0, 0);
  Settings()->Background = (Val == CB_ERR ? 0 : Val);
  Settings()->BackgroundColor = GetButtonColor(GetDlgItem(hDlg, ID_BACKCOLORBTN));
  Settings()->Margin = GetDlgItemInt(hDlg, ID_MARGINEDT, NULL, FALSE);
  Settings()->Spacing = GetDlgItemInt(hDlg, ID_SPACINGEDT, NULL, FALSE);
}

// WINAPI FUNCTIONS ------------------------------------------------------------

BOOL CALLBACK ConfigDialog1Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_COMMAND:             /* Process control messages */
    switch (LOWORD(wParam))
    {
    case ID_AUTORUNCHECK:
    case ID_TRAYICONCHECK:
    case ID_UPDATECHECK:
    case ID_STAYONTOPCHECK:
    case ID_SCREENSNAPCHECK:
      if (HIWORD(wParam) == BN_CLICKED)
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
      break;
    case ID_WEEKDAYSCHECK:
    case ID_WEEKNUMSCHECK:
    case ID_TODAYMEMOCHECK:
      if (HIWORD(wParam) == BN_CLICKED)
      {
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
        RegionChanged1 = true;
      }
      break;
    case ID_WEEKSTARTCOMBO:
    case ID_DATEFORMATCOMBO:
    case ID_LANGUAGECOMBO:
      if (HIWORD(wParam) == CBN_SELCHANGE)
      {
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
        RegionChanged1 = true;
      }
      break;
    }
    return TRUE;
  case WM_INITDIALOG:
    FillList(hDlg, ID_WEEKSTARTCOMBO, "Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday");
    FillList(hDlg, ID_DATEFORMATCOMBO, "Day/Month/Year;Year/Month/Day;Day-Month-Year;Year-Month-Day;Month Day, Year;Day Month Year;Mon. Day, Year;Day Mon. Year");
    FillList(hDlg, ID_LANGUAGECOMBO, "English");
    CheckDlgButton(hDlg, ID_AUTORUNCHECK, Autorun ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, ID_UPDATECHECK, AutoUpdate ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, ID_STAYONTOPCHECK, StayOnTop ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, ID_SCREENSNAPCHECK, SnapToScreen ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, ID_TRAYICONCHECK, ShowTrayIcon ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, ID_WEEKDAYSCHECK, ShowWeekDays ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, ID_WEEKNUMSCHECK, ShowWeekNums ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, ID_TODAYMEMOCHECK, ShowMemo ? BST_CHECKED : BST_UNCHECKED);
    SendDlgItemMessage(hDlg, ID_WEEKSTARTCOMBO, CB_SETCURSEL, (WPARAM)StartOfWeek, 0);
    SendDlgItemMessage(hDlg, ID_DATEFORMATCOMBO, CB_SETCURSEL, (WPARAM)DateFormat, 0);
    SendDlgItemMessage(hDlg, ID_LANGUAGECOMBO, CB_SETCURSEL, (WPARAM)0, 0);
    RegionChanged1 = false;
    return TRUE;
  case WM_NOTIFY:
    switch (((NMHDR *)lParam)->code)
    {
    case PSN_APPLY:
      Autorun = IsDlgButtonChecked(hDlg, ID_AUTORUNCHECK) == BST_CHECKED;
      AutoUpdate = IsDlgButtonChecked(hDlg, ID_UPDATECHECK) == BST_CHECKED;
      TopMost(IsDlgButtonChecked(hDlg, ID_STAYONTOPCHECK) == BST_CHECKED);
      SnapToScreen = IsDlgButtonChecked(hDlg, ID_SCREENSNAPCHECK) == BST_CHECKED;
      SysTrayIconVisible(IsDlgButtonChecked(hDlg, ID_TRAYICONCHECK) == BST_CHECKED);
      ShowWeekDays = IsDlgButtonChecked(hDlg, ID_WEEKDAYSCHECK) == BST_CHECKED;
      ShowWeekNums = IsDlgButtonChecked(hDlg, ID_WEEKNUMSCHECK) == BST_CHECKED;
      ShowMemo = IsDlgButtonChecked(hDlg, ID_TODAYMEMOCHECK) == BST_CHECKED;
      StartOfWeek = SendDlgItemMessage(hDlg, ID_WEEKSTARTCOMBO, CB_GETCURSEL, 0, 0);
      DateFormat = SendDlgItemMessage(hDlg, ID_DATEFORMATCOMBO, CB_GETCURSEL, 0, 0);
      //SendDlgItemMessage(hDlg, ID_LANGUAGECOMBO, CB_GETCURSEL, 0, 0);
      if (RegionChanged1)
      {
        UpdateRegion();
        InvalidateRect(hWindow, NULL, FALSE);
        RegionChanged1 = false;
      }
      return TRUE;
    default:
      return FALSE;
    }
  default:
    return FALSE;
  }
}

BOOL CALLBACK ConfigDialog2Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  HDC DC;
  switch (uMsg)
  {
  case WM_COMMAND:             /* Process control messages */
    switch (LOWORD(wParam))
    {
    case ID_THEMECOMBO:
      if (HIWORD(wParam) == CBN_SELCHANGE)
      {
        int Index = SendDlgItemMessage(hDlg, ID_THEMECOMBO, CB_GETCURSEL, 0, 0);
        if (Index != CB_ERR)
        {
          PTheme T = ThemeList.Get(Index+1);
          if (T != NULL)
          {
            CopyTheme(&Theme, T);
            DisplaySettings(hDlg);
          }
        }
      }
      break;
    case ID_SAVEBTN:
      if (HIWORD(wParam) == BN_CLICKED)
      {
        SaveSettings(hDlg);
        LPTSTR Str = GetWindowText(GetDlgItem(hDlg, ID_THEMECOMBO));
        Theme.Name = new char[strlen(Str)+1];
        strcpy(Theme.Name, Str);
        ThemeList.Add(&Theme);
        ListThemes(hDlg);
      }
      break;
    case ID_DELETEBTN:
      if (HIWORD(wParam) == BN_CLICKED)
      {
        int Index = SendDlgItemMessage(hDlg, ID_THEMECOMBO, CB_GETCURSEL, 0, 0);
        if (Index != CB_ERR)
        {
          ThemeList.Delete(Index+1);
          ListThemes(hDlg);
        }
      }
      break;
    case ID_ITEMCOMBO:
      if (HIWORD(wParam) == CBN_SELCHANGE)
      {
        SaveSettings(hDlg);
        Index = SendDlgItemMessage(hDlg, ID_ITEMCOMBO, CB_GETCURSEL, 0, 0);
        DisplaySettings(hDlg);
      }
      break;
    case ID_FONTCOMBO:
      if (HIWORD(wParam) == CBN_SELCHANGE)
      {
        ListFontSizes(hDlg, (HWND)lParam);
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
        RegionChanged2 = true;
      }
      break;
    case ID_FONTSIZECOMBO:
      if (HIWORD(wParam) == CBN_EDITCHANGE || HIWORD(wParam) == CBN_SELCHANGE)
      {
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
        RegionChanged2 = true;
      }
      break;
    case ID_BOLDBTN:
    case ID_ITALICBTN:
    case ID_UNDERLINEBTN:
      if (HIWORD(wParam) == BN_CLICKED)
      {
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
        if (!Settings()->Background && !Settings(0)->Background)
          RegionChanged2 = true;
      }
      break;
    case ID_ALIGNCOMBO:
    case ID_FRAMECOMBO:
    case ID_EFFECTCOMBO:
      if (HIWORD(wParam) == CBN_SELCHANGE)
      {
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
        if (!Settings()->Background && !Settings(0)->Background)
          RegionChanged2 = true;
      }
    case ID_BACKGROUNDCOMBO:
      if (HIWORD(wParam) == CBN_SELCHANGE)
      {
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
        if (Index == 0 || !Settings(0)->Background)
          RegionChanged2 = true;
      }
      break;
    case ID_CORNERSEDT:
      if (HIWORD(wParam) == EN_CHANGE)
      {
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
        if (Index == 0 || !Settings(0)->Background)
          RegionChanged2 = true;
      }
      break;
    case ID_MARGINEDT:
    case ID_SPACINGEDT:
      if (HIWORD(wParam) == EN_CHANGE)
      {
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
        RegionChanged2 = true;
      }
      break;
    case ID_FRAMECOLORBTN:
    case ID_FONTCOLORBTN:
    case ID_BACKCOLORBTN:
    case ID_EFFECTCOLORBTN:
      if (HIWORD(wParam) == BN_CLICKED)
      {
        ClickButton((HWND)lParam);
        SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
      }
      break;
    }
    return TRUE;
	case WM_DRAWITEM:
    DrawButton(GetDlgItem(hDlg, (UINT)wParam), (LPDRAWITEMSTRUCT)lParam);
		return TRUE;
  case WM_INITDIALOG:
    DC = GetDC(hDlg);
    SendDlgItemMessage(hDlg, ID_BOLDBTN, WM_SETFONT, (WPARAM)EasyCreateFont(DC, "MS Shell Dlg", 8, fsBold), 0);
    SendDlgItemMessage(hDlg, ID_ITALICBTN, WM_SETFONT, (WPARAM)EasyCreateFont(DC, "MS Shell Dlg", 8, fsItalic), 0);
    SendDlgItemMessage(hDlg, ID_UNDERLINEBTN, WM_SETFONT, (WPARAM)EasyCreateFont(DC, "MS Shell Dlg", 8, fsUnderline), 0);
    ReleaseDC(hDlg, DC);
    ApplyWindowsTheme(hDlg);
    ListThemes(hDlg);
    ListFonts(hDlg);
    FillList(hDlg, ID_ITEMCOMBO, "Calendar;Date;Weekdays;Week numbers;Day;Weekend;Today;Day with memo;Memo of the day");
    FillList(hDlg, ID_ALIGNCOMBO, "Left;Center;Right");
    FillList(hDlg, ID_FRAMECOMBO, "None;Single");
    FillList(hDlg, ID_BACKGROUNDCOMBO, "None;Plain");
    FillList(hDlg, ID_EFFECTCOMBO, "None;Outline;Shadow");
    SendDlgItemMessage(hDlg, ID_THEMECOMBO, CB_SETCURSEL, (WPARAM)atoi(ThemeList.GetFirst()->Name), 0);
    SendDlgItemMessage(hDlg, ID_ITEMCOMBO, CB_SETCURSEL, (WPARAM)Index, 0);
    DisplaySettings(hDlg);
    RegionChanged2 = false;
    return TRUE;
  case WM_NOTIFY:
    if (((NMHDR *)lParam)->code == PSN_APPLY)
    {
      SaveSettings(hDlg);
      int Index = SendDlgItemMessage(hDlg, ID_THEMECOMBO, CB_GETCURSEL, 0, 0);
      if (Index != CB_ERR)
      {
        CopyTheme(ThemeList.Get(Index+1), &Theme);
        ThemeList.GetFirst()->Name = IntToStr(Index);
      }
      if (RegionChanged2)
        UpdateRegion();
      InvalidateRect(hWindow, NULL, FALSE);
      RegionChanged2 = false;
      return TRUE;
    }
    else
      return FALSE;
  case WM_THEMECHANGED:
    ApplyWindowsTheme(hDlg);
    return TRUE;
  default:
    return FALSE;
  }
}

// PUBLIC FUNCTIONS ------------------------------------------------------------

void ShowConfig()
{
  PROPSHEETPAGE PropSheet[2];
  PROPSHEETHEADER PropHeader;

  PropSheet[0].dwSize = sizeof(PROPSHEETPAGE);
  PropSheet[0].dwFlags = PSP_USETITLE;
  PropSheet[0].hInstance = hInstance;
  PropSheet[0].pszTemplate = MAKEINTRESOURCE(ID_CONFIGDIALOG1);
  PropSheet[0].pfnDlgProc = ConfigDialog1Proc;
  PropSheet[0].lParam = 0;
  PropSheet[0].pszTitle = "Options";
  PropSheet[0].pfnCallback = NULL;
  
  PropSheet[1].dwSize = sizeof(PROPSHEETPAGE);
  PropSheet[1].dwFlags = PSP_USETITLE;
  PropSheet[1].hInstance = hInstance;
  PropSheet[1].pszTemplate = MAKEINTRESOURCE(ID_CONFIGDIALOG2);
  PropSheet[1].pfnDlgProc = ConfigDialog2Proc;
  PropSheet[1].lParam = 0;
  PropSheet[1].pszTitle = "Appearance and themes";
  PropSheet[1].pfnCallback = NULL;

  PropHeader.dwSize = sizeof(PROPSHEETHEADER);
  PropHeader.dwFlags = PSH_PROPSHEETPAGE;
  PropHeader.hwndParent = hWindow;
  PropHeader.hInstance = hInstance;
  PropHeader.pszCaption = "DeltaCalendar properties";
  PropHeader.nPages = sizeof(PropSheet)/sizeof(PROPSHEETPAGE);
  PropHeader.ppsp = (LPCPROPSHEETPAGE)&PropSheet;
  PropHeader.pfnCallback = NULL;
  DialogMenuItemEnabled(false);
  PropertySheet(&PropHeader);
  DialogMenuItemEnabled(true);
}

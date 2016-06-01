#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <uxtheme.h>
#include <tmschema.h>
#include "tools.h"

BOOL Themed = FALSE;
BOOL ThemeLibLoaded = FALSE;
HANDLE hTheme;
HMODULE hModThemes = NULL;
COLORREF Colors[16];

typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME );
typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc,
							int iPartId, int iStateId, const RECT *pRect,  const RECT *pClipRect);
typedef HRESULT(__stdcall *PFNDRAWTHEMEPARENTBACKGROUND)(HWND hWnd, HDC DC, RECT* R);
typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme,  HDC hdc,
							int iPartId, int iStateId,  const RECT *pBoundingRect,
							RECT *pContentRect);

PFNOPENTHEMEDATA zOpenThemeData = NULL;
PFNDRAWTHEMEBACKGROUND zDrawThemeBackground = NULL;
PFNDRAWTHEMEPARENTBACKGROUND zDrawThemeParentBackground = NULL;
PFNCLOSETHEMEDATA zCloseThemeData = NULL;
PFNGETTHEMEBACKGROUNDCONTENTRECT zGetThemeBackgroundContentRect = NULL;

// PRIVATE FUNCTIONS -----------------------------------------------------------

void InitWindowsThemes()
{
	hModThemes = LoadLibrary("UXTHEME.DLL");
	if(hModThemes)
	{
		zOpenThemeData = (PFNOPENTHEMEDATA)GetProcAddress(hModThemes, "OpenThemeData");
		zDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)GetProcAddress(hModThemes, "DrawThemeBackground");
		zDrawThemeParentBackground = (PFNDRAWTHEMEPARENTBACKGROUND)GetProcAddress(hModThemes, "DrawThemeParentBackground");
		zCloseThemeData = (PFNCLOSETHEMEDATA)GetProcAddress(hModThemes, "CloseThemeData");
		zGetThemeBackgroundContentRect = (PFNGETTHEMEBACKGROUNDCONTENTRECT)GetProcAddress(hModThemes, "GetThemeBackgroundContentRect");
		if(zOpenThemeData && zDrawThemeBackground && zCloseThemeData && zGetThemeBackgroundContentRect)
			ThemeLibLoaded = TRUE;
		else
		{
			FreeLibrary(hModThemes);
			hModThemes = NULL;
		}
	}
}

// PUBLIC FUNCTIONS ------------------------------------------------------------

void ApplyWindowsTheme(HWND hWnd)
{
	if(ThemeLibLoaded)
		{
		if(hTheme)
			zCloseThemeData(hTheme);
    FreeLibrary(hModThemes);
    hModThemes = NULL;
    ThemeLibLoaded = FALSE;
    Themed = FALSE;
		}
	InitWindowsThemes();
	if(ThemeLibLoaded)
		{
		hTheme = zOpenThemeData(hWnd, L"Button");
		if(hTheme)
			Themed = TRUE;
		}
}

void SetButtonColor(HWND hWnd, COLORREF Color)
{
  char* Str = IntToStr(Color);
  SetWindowText(hWnd, Str);
  InvalidateRect(hWnd, NULL, FALSE);
}

COLORREF GetButtonColor(HWND hWnd)
{
	int Length = GetWindowTextLength(hWnd)+1;
	char* Text = new char[Length];
	GetWindowText(hWnd, Text, Length);
	return (COLORREF)atoi(Text);
}

void DrawButton(HWND hWnd, LPDRAWITEMSTRUCT DrawStruct)
{
  HBRUSH Brush;
  HPEN Pen;
	HDC DC = DrawStruct->hDC;
	bool IsPressed = (DrawStruct->itemState & ODS_SELECTED);
	bool IsFocused = (DrawStruct->itemState & ODS_FOCUS);
	bool IsDisabled = (DrawStruct->itemState & ODS_DISABLED);
	RECT R = DrawStruct->rcItem;
	SetBkMode(DC, TRANSPARENT);
	if(Themed)
	{
    zDrawThemeParentBackground(GetParent(hWnd), DC, &R);
		zDrawThemeBackground(hTheme, DC, BP_PUSHBUTTON, IsPressed ? PBS_PRESSED : (IsFocused ? PBS_DEFAULTED : PBS_NORMAL), &R, NULL);
  }
	else
	{
		if (IsFocused)
		{
			Brush = CreateSolidBrush(RGB(0,0,0));
			FrameRect(DC, &R, Brush);
			InflateRect(&R, -1, -1);
			DeleteObject(Brush);
		} 
		Brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		FillRect(DC, &R, Brush);
		DeleteObject(Brush);
		if (IsPressed)
		{
			Brush = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
			FrameRect(DC, &R, Brush);
			DeleteObject(Brush);
		}
		else 
			DrawFrameControl(DC, &R, DFC_BUTTON, DFCS_BUTTONPUSH | (IsPressed ? DFCS_PUSHED : 0));
	}
	RECT R2 = DrawStruct->rcItem;
  InflateRect(&R2, -6, -6);
	if (IsDisabled)
	{
    Pen = (HPEN)SelectObject(DC, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW)));
    Rectangle(DC, R2.left, R2.top, R2.right, R2.bottom);
		DeleteObject(SelectObject(DC, Pen));
  }
	else
	{
		Brush = (HBRUSH)SelectObject(DC, CreateSolidBrush(GetButtonColor(hWnd)));
    Rectangle(DC, R2.left, R2.top, R2.right, R2.bottom);
		DeleteObject(SelectObject(DC, Brush));
	} 
	if (IsFocused)
	{
		InflateRect(&R, -3, -3);
		DrawFocusRect(DC, &R);
	}
}

void ClickButton(HWND hWnd)
{
  CHOOSECOLOR Dlg;
  Dlg.lStructSize = sizeof(CHOOSECOLOR);
  Dlg.hwndOwner = GetParent(hWnd);
  Dlg.rgbResult = GetButtonColor(hWnd);
  Dlg.lpCustColors = &Colors[0];
  Dlg.Flags = CC_FULLOPEN | CC_RGBINIT;
  Dlg.lCustData = 0;
  if (ChooseColor(&Dlg) != 0)
    SetButtonColor(hWnd, Dlg.rgbResult);
}

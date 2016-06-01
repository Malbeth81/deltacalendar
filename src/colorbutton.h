#ifndef WINTHEMEH
#define WINTHEMEH

void ApplyWindowsTheme(HWND hWnd);
void SetButtonColor(HWND hWnd, COLORREF Color);
COLORREF GetButtonColor(HWND hWnd);
void DrawButton(HWND hWnd, LPDRAWITEMSTRUCT lpDIS);
void ClickButton(HWND hWnd);

#endif

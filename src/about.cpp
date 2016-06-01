#include <windows.h>
#include "resource.h"
#include "main.h"
#include "menu.h"

TRACKMOUSEEVENT MouseEvent;

BOOL CALLBACK AboutDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  HDC DC;
  switch (uMsg)
  {
  case WM_COMMAND:        /* Process control messages */
    switch (LOWORD(wParam))
    {
    case ID_ABOUTLINK:
      if (HIWORD(wParam) == STN_CLICKED || HIWORD(wParam) == STN_DBLCLK)
        ShellExecute(NULL, "open", "http://www.marcandre.info", NULL, NULL, SW_SHOW);
      break;
    case ID_OKBTN:
      EndDialog(hDlg, 1);
      break;
    }
    return TRUE;
  case WM_CLOSE:
    EndDialog(hDlg, 1);
    return TRUE;
  case WM_CTLCOLORSTATIC:
    if ((HWND)lParam == GetDlgItem(hDlg, ID_ABOUTLINK))
    {
      SetTextColor((HDC)wParam, 0x00FF0000);
      SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNFACE));
      return (BOOL)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    }
    return DefWindowProc(hDlg, uMsg, wParam, lParam);
  case WM_INITDIALOG:
    DC = GetDC(hDlg);
    SendDlgItemMessage(hDlg, ID_ABOUTLINK, WM_SETFONT, (WPARAM)EasyCreateFont(DC, "MS Shell Dlg", 8, fsUnderline), 0);
    ReleaseDC(hDlg, DC);
    return TRUE;
  default:
    return FALSE;
  }
}

void ShowAbout()
{
  DialogMenuItemEnabled(false);
  DialogBox(hInstance, MAKEINTRESOURCE(ID_ABOUTDIALOG), hWindow, AboutDialogProc);
  DialogMenuItemEnabled(true);
}

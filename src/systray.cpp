#include <windows.h>
#include "resource.h"
#include "systray.h"
#include "menu.h"
#include "main.h"

NOTIFYICONDATA TrayIconData;

void SysTrayIconVisible(bool Visible)
{
  if (Visible)
  {
    ShowTrayIcon = true;
    if (TrayIconData.hWnd != hWindow)
    {
      TrayIconData.cbSize = sizeof(NOTIFYICONDATA);
      TrayIconData.hWnd = hWindow;
      TrayIconData.uID = 0;
      TrayIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
      TrayIconData.uCallbackMessage = WM_SHELLTRAYICON;
      TrayIconData.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(ID_MAINICON), IMAGE_ICON, 16, 16, LR_SHARED);
      //LoadIcon(hInstance, MAKEINTRESOURCE(ID_TRAYICON));
      GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, NULL, NULL, TrayIconData.szTip, 64);
    }
    Shell_NotifyIcon(NIM_ADD, &TrayIconData);
  }
  else
  {
    ShowTrayIcon = false;
    Shell_NotifyIcon(NIM_DELETE, &TrayIconData);
  }
  CheckMenuItem(hMenu, ID_TRAYICONMENU, Visible ? MF_CHECKED : MF_UNCHECKED);
}

void SysTrayIconProc(WORD Msg)
{
  switch (Msg)
  {
  case WM_LBUTTONUP:
    if (IsWindowVisible(hWindow))
      SetForegroundWindow(hWindow);
    break;
  case WM_LBUTTONDBLCLK:
    if (IsWindowVisible(hWindow))
      ShowWindow(hWindow, SW_HIDE);
    else
      ShowWindow(hWindow, SW_NORMAL);
    break;
  case WM_RBUTTONUP:
    POINT Pt;
    if (GetCursorPos(&Pt))
    {
      SetForegroundWindow(hWindow);
      OpenMenu(Pt.x, Pt.y);
    }
    break;
  }
}

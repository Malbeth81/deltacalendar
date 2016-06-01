#include <windows.h>
#include "systray.h"
#include "menu.h"
#include "main.h"
#include "config.h"
#include "about.h"

HMENU hMenu;
HMENU hDateMenu;

void CreatePopMenu()
{
  hMenu = CreatePopupMenu();
  hDateMenu = CreatePopupMenu();
  AppendMenu(hMenu, MF_STRING, ID_CONFIGMENU, "&Configuration...");
  AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hDateMenu, "&Display month");
  AppendMenu(hDateMenu, MF_STRING, ID_CURRMONTHMENU, "&Current");
  AppendMenu(hDateMenu, MF_SEPARATOR, 0, "");
  AppendMenu(hDateMenu, MF_STRING, ID_PREVMONTHMENU, "&Previous");
  AppendMenu(hDateMenu, MF_STRING, ID_NEXTMONTHMENU, "&Next");
  AppendMenu(hDateMenu, MF_SEPARATOR, 0, "");
  AppendMenu(hDateMenu, MF_STRING, ID_CUSTMONTHMENU, "&Custom...");
  AppendMenu(hMenu, MF_STRING, ID_ABOUTMENU, "&About...");
  AppendMenu(hMenu, MF_SEPARATOR, 0, "");
  AppendMenu(hMenu, MF_STRING, ID_TOPMOSTMENU, "&Stay on top");
  AppendMenu(hMenu, MF_STRING, ID_SCREENSNAPMENU, "&Snap to the screen's edge");
  AppendMenu(hMenu, MF_STRING, ID_TRAYICONMENU, "&Show system tray icon");
  AppendMenu(hMenu, MF_SEPARATOR, 0, "");
  AppendMenu(hMenu, MF_STRING, ID_QUITMENU, "&Close");
  CheckMenuItem(hMenu, ID_TOPMOSTMENU, StayOnTop ? MF_CHECKED : MF_UNCHECKED);
  CheckMenuItem(hMenu, ID_SCREENSNAPMENU, SnapToScreen ? MF_CHECKED : MF_UNCHECKED);
  CheckMenuItem(hMenu, ID_TRAYICONMENU, ShowTrayIcon ? MF_CHECKED : MF_UNCHECKED);
}

void DialogMenuItemEnabled(bool Enabled)
{
  EnableMenuItem(hMenu, ID_ABOUTMENU, Enabled ? MF_ENABLED : MF_GRAYED);
  EnableMenuItem(hMenu, ID_CUSTMONTHMENU, Enabled ? MF_ENABLED : MF_GRAYED);
  EnableMenuItem(hMenu, ID_CONFIGMENU, Enabled ? MF_ENABLED : MF_GRAYED);
}

void OpenMenu(int x, int y)
{
  TrackPopupMenu(hMenu, TPM_LEFTALIGN, x, y, 0, hWindow, NULL);
}

void MenuProc(WORD ID)
{
  switch (ID)
  {
  case ID_CONFIGMENU:
    ShowConfig();
    break;
  case ID_CURRMONTHMENU:
    DisplayTime.wYear = CurrentTime.wYear;
    DisplayTime.wMonth = CurrentTime.wMonth;
    DisplayTime.wDay = CurrentTime.wDay;
    UpdateRegion();
    break;
  case ID_PREVMONTHMENU:
    if (DisplayTime.wMonth > 1)
      DisplayTime.wMonth--;
    else
    {
      DisplayTime.wMonth = 12;
      DisplayTime.wYear--;
    }
    UpdateRegion();
    break;
  case ID_NEXTMONTHMENU:
    if (DisplayTime.wMonth < 12)
      DisplayTime.wMonth++;
    else
    {
      DisplayTime.wMonth = 1;
      DisplayTime.wYear++;
    }
    UpdateRegion();
    break;
  case ID_CUSTMONTHMENU:
    break;
  case ID_TOPMOSTMENU:
    TopMost(!StayOnTop);                  /* Menu item is checked in the function */
    break;
  case ID_SCREENSNAPMENU:
    SnapToScreen = !SnapToScreen;
    CheckMenuItem(hMenu, ID_SCREENSNAPMENU, SnapToScreen ? MF_CHECKED : MF_UNCHECKED);
    break;
  case ID_TRAYICONMENU:
    SysTrayIconVisible(!ShowTrayIcon);    /* Menu item is checked in the function */
    break;
  case ID_ABOUTMENU:
    ShowAbout();
    break;
  case ID_QUITMENU:
    PostMessage(hWindow, WM_CLOSE, 0, 0);
    break;
  }
}

#ifndef MENUH
#define	MENUH

#define ID_CONFIGMENU 101
#define ID_CURRMONTHMENU 102
#define ID_PREVMONTHMENU 103
#define ID_NEXTMONTHMENU 104
#define ID_CUSTMONTHMENU 105
#define ID_ABOUTMENU 106
#define ID_TOPMOSTMENU 107
#define ID_SCREENSNAPMENU 108
#define ID_TRAYICONMENU 109
#define ID_QUITMENU 110

extern HMENU hMenu;
extern HMENU hDateMenu;

void CreatePopMenu();
void DialogMenuItemEnabled(bool Enabled);
void OpenMenu(int x, int y);
void MenuProc(WORD ID);

#endif

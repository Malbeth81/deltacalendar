#ifndef SYSTRAYH
#define	SYSTRAYH

static const int WM_SHELLTRAYICON = WM_APP+1;

void SysTrayIconVisible(bool Visible);
void SysTrayIconProc(WORD Msg);

#endif

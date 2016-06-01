#ifndef THEMESH
#define	THEMESH

#include <windows.h>
#include "tools.h"

typedef struct _Settings {
  TAlignment  Alignment;
  bool        Background;
  COLORREF    BackgroundColor;
  TEffect     Effect;
  COLORREF    EffectColor;
  char*       FontName;
  USHORT      FontSize;
  TFontStyle  FontStyle;
  COLORREF    FontColor;
  bool        Frame;
  USHORT      FrameRoundness;
  COLORREF    FrameColor;
  USHORT      Margin;
  USHORT      Spacing;
} TSettings, *PSettings;

typedef struct _Theme {
  char*     Name;
  TSettings Calendar;
  TSettings Date;
  TSettings Weekday;
  TSettings Weeknum;
  TSettings Day;
  TSettings Weekend;
  TSettings Today;
  TSettings MemoDay;
  TSettings Memo;
  _Theme*   Next;
} TTheme, *PTheme;

class TThemeList
{
  public:
    TThemeList();
    PTheme Get(unsigned int Index);
    PTheme GetFirst();
    PTheme GetLast();
    unsigned int GetCount();
    void Add(PTheme Theme);
    void Delete(unsigned int Index);
    bool LoadFromFile(char* Filename);
    bool SaveToFile(char* Filename);
  private:
    PTheme First;
    unsigned int Count;
};

void CreateDefaultTheme(PTheme Theme);
void CreateMaskTheme(PTheme Theme);
void CopyTheme(PTheme Dest, PTheme Src);

#endif

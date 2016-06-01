using namespace std;
#include <fstream>
#include "themes.h"

#define FileType "CalendarThemes"
#define FileVersion 110
#define FileSupport 110

void WriteInt(ofstream& File, int Val)
{
  File.write((char*)&Val, sizeof(int));
}

int ReadInt(ifstream& File)
{
  int Val;
  File.read((char*)&Val, sizeof(int));
  return Val;
}

void WriteStr(ofstream& File, char* Str)
{
  int Len = strlen(Str)+1;
  WriteInt(File, Len);
  File.write(Str, Len);
}

char* ReadStr(ifstream& File)
{
  int Len = ReadInt(File);
  char *Str = new char[Len];
  File.read(Str, Len);
  return Str;
}

TThemeList::TThemeList()
{
  First = NULL;
  Count = 0;
}

PTheme TThemeList::Get(unsigned int Index)
{
  PTheme Ptr = NULL;
  if (Index >= 0 && Index < Count)
  {
    Ptr = First;
    while (Ptr != NULL && Index > 0)
    {
      Ptr = Ptr->Next;
      Index --;
    }
  }
  return Ptr;
}

PTheme TThemeList::GetFirst()
{
  return First;
}

PTheme TThemeList::GetLast()
{
  PTheme Ptr = First;
  if (Ptr != NULL)
    while (Ptr->Next != NULL)
      Ptr = Ptr->Next;
  return Ptr;
}

unsigned int TThemeList::GetCount()
{
  return Count;
}

void TThemeList::Add(PTheme Theme)
{
  PTheme Ptr = new TTheme;
  CopyTheme(Ptr, Theme);
  Ptr->Next = NULL;
  PTheme Last = GetLast();
  if (Last == NULL)
    First = Ptr;
  else
    Last->Next = Ptr;
  Count++;
}

void TThemeList::Delete(unsigned int Index)
{
  if (Index >= 0 && Index < Count)
  {
    PTheme Prev = Get(Index-1);
    PTheme Ptr = (Prev == NULL ? First : Prev->Next);
    if (Ptr != NULL)
    {
      if (Prev == NULL)
        First = Ptr->Next;
      else
        Prev->Next = Ptr->Next;
      delete Ptr;
      Count--;
    }
  }
}

void LoadSettings(ifstream &File, PSettings Settings)
{
  Settings->Alignment = (TAlignment)ReadInt(File);
  Settings->Background = ReadInt(File);
  Settings->BackgroundColor = ReadInt(File);
  Settings->Effect = (TEffect)ReadInt(File);
  Settings->EffectColor = ReadInt(File);
  Settings->FontName = ReadStr(File);
  Settings->FontSize = ReadInt(File);
  Settings->FontStyle = ReadInt(File);
  Settings->FontColor = ReadInt(File);
  Settings->Frame = ReadInt(File);
  Settings->FrameRoundness = ReadInt(File);
  Settings->FrameColor = ReadInt(File);
  Settings->Margin = ReadInt(File);
  Settings->Spacing = ReadInt(File);
}

bool TThemeList::LoadFromFile(char* Filename)
{
  TTheme Ptr;
  ifstream File(Filename, ios::out | ios::binary);
  if (File.is_open())
  {
    if (strcmp(ReadStr(File), FileType) != 0 || ReadInt(File) < FileSupport)
    {
      File.close();
      return false;
    }
    Count = 0;
    while (File.peek() != EOF)
    {
      Ptr.Name = ReadStr(File);
      LoadSettings(File, &Ptr.Calendar);
      LoadSettings(File, &Ptr.Date);
      LoadSettings(File, &Ptr.Weekday);
      LoadSettings(File, &Ptr.Weeknum);
      LoadSettings(File, &Ptr.Day);
      LoadSettings(File, &Ptr.Weekend);
      LoadSettings(File, &Ptr.Today);
      LoadSettings(File, &Ptr.MemoDay);
      LoadSettings(File, &Ptr.Memo);
      Add(&Ptr);
    }
    File.close();
    return true;
  }
  return false;
}

void SaveSettings(ofstream &File, PSettings Settings)
{
  WriteInt(File, Settings->Alignment);
  WriteInt(File, Settings->Background);
  WriteInt(File, Settings->BackgroundColor);
  WriteInt(File, Settings->Effect);
  WriteInt(File, Settings->EffectColor);
  WriteStr(File, Settings->FontName);
  WriteInt(File, Settings->FontSize);
  WriteInt(File, Settings->FontStyle);
  WriteInt(File, Settings->FontColor);
  WriteInt(File, Settings->Frame);
  WriteInt(File, Settings->FrameRoundness);
  WriteInt(File, Settings->FrameColor);
  WriteInt(File, Settings->Margin);
  WriteInt(File, Settings->Spacing);
}

bool TThemeList::SaveToFile(char* Filename)
{
  ofstream File(Filename, ios::out | ios::binary);
  if (File.is_open())
  {
    WriteStr(File, FileType);
    WriteInt(File, FileVersion);
    PTheme Ptr = First;
    while (Ptr != NULL)
    {
      WriteStr(File, Ptr->Name);
      SaveSettings(File, &Ptr->Calendar);
      SaveSettings(File, &Ptr->Date);
      SaveSettings(File, &Ptr->Weekday);
      SaveSettings(File, &Ptr->Weeknum);
      SaveSettings(File, &Ptr->Day);
      SaveSettings(File, &Ptr->Weekend);
      SaveSettings(File, &Ptr->Today);
      SaveSettings(File, &Ptr->MemoDay);
      SaveSettings(File, &Ptr->Memo);
      Ptr = Ptr->Next;
    }
    File.close();
    return true;
  }
  return false;
}
    
void CreateDefaultTheme(PTheme Theme)
{
  TSettings Settings;
  Settings.Alignment = aCenter;
  Settings.Background = false;
  Settings.BackgroundColor = 0x00ffffff;
  Settings.Effect = eShadow;
  Settings.EffectColor = 0x00aaaaaa;
  Settings.FontColor = 0x00000000;
  Settings.FontName = "Arial";
  Settings.FontSize = 10;
  Settings.FontStyle = fsBold;
  Settings.Frame = false;
  Settings.FrameRoundness = 2;
  Settings.FrameColor = 0x00444444;
  Settings.Margin = 0;
  Settings.Spacing = 0;
  Theme->Name = "Default";
  Theme->Calendar = Settings;
  Theme->Date = Settings;
  Theme->Weekday = Settings;
  Theme->Weeknum = Settings;
  Theme->Day = Settings;
  Theme->Weekend = Settings;
  Theme->Today = Settings;
  Theme->MemoDay = Settings;
  Theme->Memo = Settings;
}

void CreateMaskTheme(PTheme Theme)
{
  Theme->Calendar.BackgroundColor = 0x00ffffff;
  Theme->Calendar.EffectColor = 0x00ffffff;
  Theme->Calendar.FontColor = 0x00ffffff;
  Theme->Calendar.FrameColor = 0x00ffffff;
  Theme->Date.BackgroundColor = 0x00ffffff;
  Theme->Date.EffectColor = 0x00ffffff;
  Theme->Date.FontColor = 0x00ffffff;
  Theme->Date.FrameColor = 0x00ffffff;
  Theme->Weekday.BackgroundColor = 0x00ffffff;
  Theme->Weekday.EffectColor = 0x00ffffff;
  Theme->Weekday.FontColor = 0x00ffffff;
  Theme->Weekday.FrameColor = 0x00ffffff;
  Theme->Day.BackgroundColor = 0x00ffffff;
  Theme->Day.EffectColor = 0x00ffffff;
  Theme->Day.FontColor = 0x00ffffff;
  Theme->Day.FrameColor = 0x00ffffff;
  Theme->Weekend.BackgroundColor = 0x00ffffff;
  Theme->Weekend.EffectColor = 0x00ffffff;
  Theme->Weekend.FontColor = 0x00ffffff;
  Theme->Weekend.FrameColor = 0x00ffffff;
  Theme->Today.BackgroundColor = 0x00ffffff;
  Theme->Today.EffectColor = 0x00ffffff;
  Theme->Today.FontColor = 0x00ffffff;
  Theme->Today.FrameColor = 0x00ffffff;
  Theme->MemoDay.BackgroundColor = 0x00ffffff;
  Theme->MemoDay.EffectColor = 0x00ffffff;
  Theme->MemoDay.FontColor = 0x00ffffff;
  Theme->MemoDay.FrameColor = 0x00ffffff;
  Theme->Memo.BackgroundColor = 0x00ffffff;
  Theme->Memo.EffectColor = 0x00ffffff;
  Theme->Memo.FontColor = 0x00ffffff;
  Theme->Memo.FrameColor = 0x00ffffff;
}

void CopySettings(PSettings Dest, PSettings Src)
{
  Dest->Alignment = Src->Alignment;
  Dest->Background = Src->Background;
  Dest->BackgroundColor = Src->BackgroundColor;
  Dest->Effect = Src->Effect;
  Dest->EffectColor = Src->EffectColor;
  Dest->FontName = new char[strlen(Src->FontName)];
  strcpy(Dest->FontName, Src->FontName);
  Dest->FontSize = Src->FontSize;
  Dest->FontStyle = Src->FontStyle;
  Dest->FontColor = Src->FontColor;
  Dest->Frame = Src->Frame;
  Dest->FrameRoundness = Src->FrameRoundness;
  Dest->FrameColor = Src->FrameColor;
  Dest->Margin = Src->Margin;
  Dest->Spacing = Src->Spacing;
}
  
void CopyTheme(PTheme Dest, PTheme Src)
{
  Dest->Name = new char[strlen(Src->Name)];
  strcpy(Dest->Name, Src->Name);
  CopySettings(&Dest->Calendar, &Src->Calendar);
  CopySettings(&Dest->Date, &Src->Date);
  CopySettings(&Dest->Weekday, &Src->Weekday);
  CopySettings(&Dest->Weeknum, &Src->Weeknum);
  CopySettings(&Dest->Day, &Src->Day);
  CopySettings(&Dest->Weekend, &Src->Weekend);
  CopySettings(&Dest->Today, &Src->Today);
  CopySettings(&Dest->MemoDay, &Src->MemoDay);
  CopySettings(&Dest->Memo, &Src->Memo);
}

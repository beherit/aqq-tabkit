//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SideSlideExceptionsFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sLabel"
#pragma link "sListBox"
#pragma link "sTabControl"
#pragma resource "*.dfm"
TSideSlideExceptionsForm *SideSlideExceptionsForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)void RefreshSideSlideExceptions();
//---------------------------------------------------------------------------
__fastcall TSideSlideExceptionsForm::TSideSlideExceptionsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSideSlideExceptionsForm::aExitExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TSideSlideExceptionsForm::FormShow(TObject *Sender)
{
  //Odczyt ustawien
  aLoadSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSideSlideExceptionsForm::SaveButtonClick(TObject *Sender)
{
  //Zapis ustawien
  aSaveSettings->Execute();
  //Odswiezenie wyjatkow w rdzeniu wtyczki
  RefreshSideSlideExceptions();
  //Zamkniecie formy
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TSideSlideExceptionsForm::AddButtonClick(TObject *Sender)
{
  UnicodeString Process;
  if(InputQuery("Nowy wyj¹tek","Nazwa pliku wykonywalnego:",Process))
  {
	if(!Process.IsEmpty())
	{
	  ProcessListBox->Items->Add(Process);
	  SaveButton->Enabled = true;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TSideSlideExceptionsForm::ProcessListBoxClick(TObject *Sender)
{
  if(ProcessListBox->ItemIndex!=-1)
   DeleteButton->Enabled = true;
  else
   DeleteButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSideSlideExceptionsForm::DeleteButtonClick(TObject *Sender)
{
  //Usuniecie zaznaczonego elementu
  ProcessListBox->DeleteSelected();
  //Wylaczenie przycisku
  DeleteButton->Enabled = false;
  //Wlaczenie mozlisci zapisu ustawien
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSideSlideExceptionsForm::aLoadSettingsExecute(TObject *Sender)
{
  ProcessListBox->Clear();
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Settings.ini");
  TStringList *ProcessList = new TStringList;
  Ini->ReadSection("SideSlideExceptions",ProcessList);
  int ProcessListCount = ProcessList->Count;
  delete ProcessList;
  if(ProcessListCount>0)
  {
	for(int Count=0;Count<ProcessListCount;Count++)
	{
	  UnicodeString Process = Ini->ReadString("SideSlideExceptions",("Process"+IntToStr(Count+1)), "");
	  if(!Process.IsEmpty()) ProcessListBox->Items->Add(Process);
	}
  }
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSideSlideExceptionsForm::aSaveSettingsExecute(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Settings.ini");
  Ini->EraseSection("SideSlideExceptions");
  if(ProcessListBox->Count)
   for(int Count=0;Count<ProcessListBox->Count;Count++)
	Ini->WriteString("SideSlideExceptions",("Process"+IntToStr(Count+1)),ProcessListBox->Items->Strings[Count]);
  delete Ini;
}
//---------------------------------------------------------------------------

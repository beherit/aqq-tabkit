//---------------------------------------------------------------------------
#define STRICT
#include <vcl.h>
#pragma hdrstop
#include "SettingsFrm.h"
#include <gdiplus.h>
//#include <windows.h>
//#include <algorithm>
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma link "IdBaseComponent"
#pragma link "IdCoder"
#pragma link "IdCoder3to4"
#pragma link "IdCoderMIME"
#pragma link "cspin"
#pragma link "IdBaseComponent"
#pragma link "IdCoder"
#pragma link "IdCoder3to4"
#pragma link "IdCoderMIME"
#pragma link "cspin"
#pragma link "IdBaseComponent"
#pragma link "IdCoder"
#pragma link "IdCoder3to4"
#pragma link "IdCoderMIME"
//#pragma link "GdiPlus.lib"
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//using std::min;
//using std::max;
//---------------------------------------------------------------------------
__declspec(dllimport)void LoadSettings();
__declspec(dllimport)void CheckAntiSpim();
__declspec(dllimport)void CheckHideStatusBar();
__declspec(dllimport)void ShowToolBar();
__declspec(dllimport)void DestroyStayOnTop();
__declspec(dllimport)void BuildStayOnTop();
__declspec(dllimport)void DestroyFrmUnsentMsg();
__declspec(dllimport)void BuildFrmUnsentMsg();
__declspec(dllimport)void EraseUnsentMsg();
__declspec(dllimport)void ShowUnsentMsg();
__declspec(dllimport)void DestroyFrmClosedTabs();
__declspec(dllimport)void BuildFrmClosedTabs();
__declspec(dllimport)void EraseClosedTabs();
__declspec(dllimport)void ChangeFrmSendTitlebar();
__declspec(dllimport)void ChangeFrmMainTitlebar();
__declspec(dllimport)void EraseClipTabs();
__declspec(dllimport)void EraseClipTabsIcons();
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetPluginUserDirW();
//---------------------------------------------------------------------------
__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UnsentMsgTrayIconClick(TObject *Sender)
{
  ShowUnsentMsg();
  UnsentMsgTrayIcon->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aExitExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::CancelButtonClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aLoadSettingsExecute(TObject *Sender)
{
  CategoryPanelGroup->CollapseAll();

  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Settings.ini");
  //UnsentMsg
  RememberUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Enable",true);
  InfoUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Info",true);
  CloudUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Cloud",true);
  DetailedCloudUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","DetailedCloud",false);
  TrayUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Tray",true);
  FastAccessUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FastAccess",true);
  FrmMainUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FrmMain",true);
  FrmSendUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FrmSend",false);
  FastClearUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FastClear",false);
  //TabsSwitching
  SwitchToNewMsgCheckBox->Checked = Ini->ReadBool("TabsSwitching","SwitchToNewMsg",true);
  int pSwitchToNewMsgMode = Ini->ReadInteger("TabsSwitching","SwitchToNewMsgMode",1);
  if(pSwitchToNewMsgMode==1)
   SwitchToNewMsgMode1RadioButton->Checked = true;
  else
   SwitchToNewMsgMode2RadioButton->Checked = true;
  TabsHotKeysCheckBox->Checked = Ini->ReadBool("TabsSwitching","TabsHotKeys",true);
  int pTabsHotKeysMode = Ini->ReadInteger("TabsSwitching","TabsHotKeysMode",2);
  if(pTabsHotKeysMode==1)
   TabsHotKeysMode1RadioButton->Checked = true;
  else
   TabsHotKeysMode2RadioButton->Checked = true;
  //ClosedTabs
  RememberClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","Enable",true);
  FastAccessClosedTabsCheckBox->Checked =  Ini->ReadBool("ClosedTabs","FastAccess",true);
  FrmMainClosedTabsCheckBox->Checked =  Ini->ReadBool("ClosedTabs","FrmMain",true);
  FrmSendClosedTabsCheckBox->Checked =  Ini->ReadBool("ClosedTabs","FrmSend",false);
  ItemsCountClosedTabsCSpinEdit->Value = Ini->ReadInteger("ClosedTabs","ItemsCount",5);
  ShowTimeClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","ClosedTime",false);
  FastClearClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","FastClear",false);
  UnCloseTabHotKeyCheckBox->Checked =  Ini->ReadBool("ClosedTabs","HotKey",false);
  int pUnCloseTabHotKeyMode = Ini->ReadInteger("ClosedTabs","HotKeyMode",1);
  if(pUnCloseTabHotKeyMode==1)
   UnCloseTabHotKeyMode1RadioButton->Checked = true;
  else
   UnCloseTabHotKeyMode2RadioButton->Checked = true;
  UnCloseTabHotKeyInput->HotKey = Ini->ReadInteger("ClosedTabs","HotKeyDef",0);
  CountClosedTabsCSpinEdit->Value = Ini->ReadInteger("ClosedTabs","Count",5);
  RestoreLastMsgClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","RestoreLastMsg",false);
  OnlyConversationTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","OnlyConversationTabs",false);
  //SessionRemember
  RestoreTabsSessionCheckBox->Checked = Ini->ReadBool("SessionRemember","RestoreTabs",true);
  ManualRestoreTabsSessionCheckBox->Checked = Ini->ReadBool("SessionRemember","ManualRestoreTabs",false);
  RestoreMsgSessionCheckBox->Checked = Ini->ReadBool("SessionRemember","RestoreMsg",false);
  //Titlebar
  TweakFrmSendTitlebarCheckBox->Checked = Ini->ReadBool("Titlebar","TweakSend",false);
  int pTweakFrmSendTitlebarMode = Ini->ReadInteger("Titlebar","SendMode",1);
  if(pTweakFrmSendTitlebarMode==1)
   TweakFrmSendTitlebarMode1RadioButton->Checked = true;
  else if(pTweakFrmSendTitlebarMode==2)
   TweakFrmSendTitlebarMode2RadioButton->Checked = true;
  else
   TweakFrmSendTitlebarMode3RadioButton->Checked = true;
  TweakFrmMainTitlebarCheckBox->Checked = Ini->ReadBool("Titlebar","TweakMain",false);
  int pTweakFrmMainTitlebarMode = Ini->ReadInteger("Titlebar","MainMode",1);
  if(pTweakFrmMainTitlebarMode==1)
   TweakFrmMainTitlebarMode1RadioButton->Checked = true;
  else
   TweakFrmMainTitlebarMode2RadioButton->Checked = true;
  TweakFrmMainTitlebarMode2Edit->Text = Ini->ReadString("Titlebar","MainText","");
  //NewMsg
  InactiveFrmNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","InactiveFrm",true);
  InactiveTabsNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","InactiveTabs",true);
  InactiveNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","InactiveNotifer",false);
  ChatStateNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","ChatStateNotifer",true);
  //ClipTabs
  OpenClipTabsCheckBox->Checked = Ini->ReadBool("ClipTabs","OpenClipTabs",true);
  InactiveClipTabsCheckBox->Checked = Ini->ReadBool("ClipTabs","InactiveClipTabs",false);
  CounterClipTabsCheckBox->Checked = Ini->ReadBool("ClipTabs","Counter",false);
  //Other
  StayOnTopCheckBox->Checked = Ini->ReadBool("Other","StayOnTop",false);
  EmuTabsWCheckBox->Checked = Ini->ReadBool("Other","EmuTabsW",false);
  QuickQuoteCheckBox->Checked = Ini->ReadBool("Other","QuickQuote",false);
  AntiSpimCheckBox->Checked = !Ini->ReadBool("Other","AntiSpim",true);
  HideStatusBarCheckBox->Checked = Ini->ReadBool("Other","HideStatusBar",false);
  HideToolBarCheckBox->Checked = Ini->ReadBool("Other","HideToolBar",false);
  CollapseImagesCheckBox->Checked = Ini->ReadBool("Other","CollapseImages",false);
  delete Ini;

  aUnsentMsgChk->Execute();
  aTabsSwitchingChk->Execute();
  aClosedTabsChk->Execute();
  aSessionRememberChk->Execute();
  aTitlebarTweakChk->Execute();
  aNewMsgChk->Execute();
  SaveButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aUnsentMsgChkExecute(TObject *Sender)
{
  DetailedCloudUnsentMsgCheckBox->Enabled = CloudUnsentMsgCheckBox->Checked;
  CloudUnsentMsgCheckBox->Enabled = InfoUnsentMsgCheckBox->Checked;
  TrayUnsentMsgCheckBox->Enabled = InfoUnsentMsgCheckBox->Checked;
  if(!InfoUnsentMsgCheckBox->Checked)
   DetailedCloudUnsentMsgCheckBox->Enabled = false;
  FrmMainUnsentMsgCheckBox->Enabled = FastAccessUnsentMsgCheckBox->Checked;
  FrmSendUnsentMsgCheckBox->Enabled = FastAccessUnsentMsgCheckBox->Checked;
  FastClearUnsentMsgCheckBox->Enabled = FastAccessUnsentMsgCheckBox->Checked;
  InfoUnsentMsgCheckBox->Enabled =  RememberUnsentMsgCheckBox->Checked;
  FastAccessUnsentMsgCheckBox->Enabled = RememberUnsentMsgCheckBox->Checked;
  if(!RememberUnsentMsgCheckBox->Checked)
  {
	CloudUnsentMsgCheckBox->Enabled = false;
	DetailedCloudUnsentMsgCheckBox->Enabled = false;
	TrayUnsentMsgCheckBox->Enabled = false;
	FrmMainUnsentMsgCheckBox->Enabled = false;
	FrmSendUnsentMsgCheckBox->Enabled = false;
	FastClearUnsentMsgCheckBox->Enabled = false;
  }

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsExecute(TObject *Sender)
{
  //Sprawdzanie zaleznosci opcji
  //ClosedTabs
  if((!FrmMainClosedTabsCheckBox->Checked)&&(!FrmSendClosedTabsCheckBox->Checked))
  {
	FastAccessClosedTabsCheckBox->Checked = false;
	aClosedTabsChk->Execute();
  }
  if((!FastAccessClosedTabsCheckBox->Checked)&&(!UnCloseTabHotKeyCheckBox->Checked))
  {
	RememberClosedTabsCheckBox->Checked = false;
	aClosedTabsChk->Execute();
  }
  //UnsentMsg
  if((!CloudUnsentMsgCheckBox->Checked)&&(!TrayUnsentMsgCheckBox->Checked))
  {
	InfoUnsentMsgCheckBox->Checked = false;
	aUnsentMsgChk->Execute();
  }
  if((!FrmMainUnsentMsgCheckBox->Checked)&&(!FrmSendUnsentMsgCheckBox->Checked))
  {
	FastAccessUnsentMsgCheckBox->Checked = false;
	aUnsentMsgChk->Execute();
  }
  if((!InfoUnsentMsgCheckBox->Checked)&&(!FastAccessUnsentMsgCheckBox->Checked))
  {
	RememberUnsentMsgCheckBox->Checked = false;
    aUnsentMsgChk->Execute();
  }
  //Zapisywanie poszczegolnych ustawien
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Settings.ini");
  //UnsentMsg
  Ini->WriteBool("UnsentMsg","Enable",RememberUnsentMsgCheckBox->Checked);
  Ini->WriteBool("UnsentMsg","Info",InfoUnsentMsgCheckBox->Checked);
  Ini->WriteBool("UnsentMsg","Cloud",CloudUnsentMsgCheckBox->Checked);
  Ini->WriteBool("UnsentMsg","DetailedCloud",DetailedCloudUnsentMsgCheckBox->Checked);
  Ini->WriteBool("UnsentMsg","Tray",TrayUnsentMsgCheckBox->Checked);
  if((!FrmMainUnsentMsgCheckBox->Checked)&&(!FrmSendUnsentMsgCheckBox->Checked))
   Ini->WriteBool("UnsentMsg","FastAccess",false);
  else
   Ini->WriteBool("UnsentMsg","FastAccess",FastAccessUnsentMsgCheckBox->Checked);
  Ini->WriteBool("UnsentMsg","FrmMain",FrmMainUnsentMsgCheckBox->Checked);
  Ini->WriteBool("UnsentMsg","FrmSend",FrmSendUnsentMsgCheckBox->Checked);
  Ini->WriteBool("UnsentMsg","FastClear",FastClearUnsentMsgCheckBox->Checked);
  //TabsSwitching
  Ini->WriteBool("TabsSwitching","SwitchToNewMsg",SwitchToNewMsgCheckBox->Checked);
  if(SwitchToNewMsgMode1RadioButton->Checked)
   Ini->WriteInteger("TabsSwitching","SwitchToNewMsgMode", 1);
  else
   Ini->WriteInteger("TabsSwitching","SwitchToNewMsgMode", 2);
  Ini->WriteBool("TabsSwitching","TabsHotKeys",TabsHotKeysCheckBox->Checked);
  if(TabsHotKeysMode1RadioButton->Checked)
   Ini->WriteInteger("TabsSwitching","TabsHotKeysMode",1);
  else
   Ini->WriteInteger("TabsSwitching","TabsHotKeysMode",2);
  //ClosedTabs
  Ini->WriteBool("ClosedTabs","Enable",RememberClosedTabsCheckBox->Checked);
  if((!FrmMainClosedTabsCheckBox->Checked)&&(!FrmSendClosedTabsCheckBox->Checked))
   Ini->WriteBool("ClosedTabs","FastAccess",false);
  else
   Ini->WriteBool("ClosedTabs","FastAccess",FastAccessClosedTabsCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","FrmMain",FrmMainClosedTabsCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","FrmSend",FrmSendClosedTabsCheckBox->Checked);
  Ini->WriteInteger("ClosedTabs","ItemsCount",ItemsCountClosedTabsCSpinEdit->Value);
  Ini->WriteBool("ClosedTabs","ClosedTime",ShowTimeClosedTabsCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","FastClear",FastClearClosedTabsCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","HotKey",UnCloseTabHotKeyCheckBox->Checked);
  if(UnCloseTabHotKeyMode1RadioButton->Checked)
   Ini->WriteInteger("ClosedTabs","HotKeyMode",1);
  else
   Ini->WriteInteger("ClosedTabs","HotKeyMode",2);
  Ini->WriteInteger("ClosedTabs","HotKeyDef",UnCloseTabHotKeyInput->HotKey);
  Ini->WriteInteger("ClosedTabs","Count",CountClosedTabsCSpinEdit->Value);
  Ini->WriteBool("ClosedTabs","RestoreLastMsg",RestoreLastMsgClosedTabsCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","OnlyConversationTabs",OnlyConversationTabsCheckBox->Checked);
  //SessionRemember
  Ini->WriteBool("SessionRemember","RestoreTabs",RestoreTabsSessionCheckBox->Checked);
  if(RestoreTabsSessionCheckBox->Checked)
  {
	Ini->WriteBool("SessionRemember","ManualRestoreTabs",ManualRestoreTabsSessionCheckBox->Checked);
	Ini->WriteBool("SessionRemember","RestoreMsg",RestoreMsgSessionCheckBox->Checked);
  }
  else
  {
	Ini->WriteBool("SessionRemember","ManualRestoreTabs",false);
	Ini->WriteBool("SessionRemember","RestoreMsg",false);
  }
  //Titlebar
  Ini->WriteBool("Titlebar","TweakSend",TweakFrmSendTitlebarCheckBox->Checked);
  if(TweakFrmSendTitlebarMode1RadioButton->Checked)
   Ini->WriteInteger("Titlebar","SendMode",1);
  else if(TweakFrmSendTitlebarMode2RadioButton->Checked)
   Ini->WriteInteger("Titlebar","SendMode",2);
  else
   Ini->WriteInteger("Titlebar","SendMode",3);
  Ini->WriteBool("Titlebar","TweakMain",TweakFrmMainTitlebarCheckBox->Checked);
  if(TweakFrmMainTitlebarMode1RadioButton->Checked)
   Ini->WriteInteger("Titlebar","MainMode",1);
  else
   Ini->WriteInteger("Titlebar","MainMode",2);
  if(TweakFrmMainTitlebarMode2Edit->Text!="Wpisz tutaj swój tekst")
   Ini->WriteString("Titlebar","MainText",TweakFrmMainTitlebarMode2Edit->Text);
  else
   Ini->WriteString("Titlebar","MainText","");
  //NewMsg
  Ini->WriteBool("NewMsg","InactiveFrm",InactiveFrmNewMsgCheckBox->Checked);
  Ini->WriteBool("NewMsg","InactiveTabs",InactiveTabsNewMsgCheckBox->Checked);
  Ini->WriteBool("NewMsg","InactiveNotifer",InactiveNotiferNewMsgCheckBox->Checked);
  Ini->WriteBool("NewMsg","ChatStateNotifer",ChatStateNotiferNewMsgCheckBox->Checked);
  //ClipTabs
  Ini->WriteBool("ClipTabs","OpenClipTabs",OpenClipTabsCheckBox->Checked);
  Ini->WriteBool("ClipTabs","InactiveClipTabs",InactiveClipTabsCheckBox->Checked);
  Ini->WriteBool("ClipTabs","Counter",CounterClipTabsCheckBox->Checked);
  //Other
  Ini->WriteBool("Other","StayOnTop",StayOnTopCheckBox->Checked);
  Ini->WriteBool("Other","EmuTabsW",EmuTabsWCheckBox->Checked);
  Ini->WriteBool("Other","QuickQuote",QuickQuoteCheckBox->Checked);
  Ini->WriteBool("Other","AntiSpim",!AntiSpimCheckBox->Checked);
  Ini->WriteBool("Other","HideStatusBar",HideStatusBarCheckBox->Checked);
  Ini->WriteBool("Other","HideToolBar",HideToolBarCheckBox->Checked);
  Ini->WriteBool("Other","CollapseImages",CollapseImagesCheckBox->Checked);

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SaveButtonClick(TObject *Sender)
{
  aSaveSettingsW->Execute();
  SaveButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aTabsSwitchingChkExecute(TObject *Sender)
{
  SwitchToNewMsgMode1RadioButton->Enabled = SwitchToNewMsgCheckBox->Checked;
  SwitchToNewMsgMode2RadioButton->Enabled = SwitchToNewMsgCheckBox->Checked;
  TabsHotKeysMode1RadioButton->Enabled = TabsHotKeysCheckBox->Checked;
  TabsHotKeysMode2RadioButton->Enabled = TabsHotKeysCheckBox->Checked;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsWExecute(TObject *Sender)
{
  DestroyFrmUnsentMsg();
  DestroyFrmClosedTabs();
  DestroyStayOnTop();
  aSaveSettings->Execute();
  LoadSettings();
  CheckAntiSpim();
  CheckHideStatusBar();
  ShowToolBar();
  BuildStayOnTop();
  BuildFrmUnsentMsg();
  BuildFrmClosedTabs();
  ChangeFrmSendTitlebar();
  ChangeFrmMainTitlebar();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OkButtonClick(TObject *Sender)
{
  aSaveSettingsW->Execute();
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aClosedTabsChkExecute(TObject *Sender)
{
  FrmMainClosedTabsCheckBox->Enabled = FastAccessClosedTabsCheckBox->Checked;
  FrmSendClosedTabsCheckBox->Enabled = FastAccessClosedTabsCheckBox->Checked;
  ShowTimeClosedTabsCheckBox->Enabled = FastAccessClosedTabsCheckBox->Checked;
  FastClearClosedTabsCheckBox->Enabled = FastAccessClosedTabsCheckBox->Checked;
  ItemsCountClosedTabsLabel->Enabled = FastAccessClosedTabsCheckBox->Checked;
  ItemsCountClosedTabsCSpinEdit->Enabled = FastAccessClosedTabsCheckBox->Checked;
  UnCloseTabHotKeyMode1RadioButton->Enabled = UnCloseTabHotKeyCheckBox->Checked;
  UnCloseTabHotKeyMode2RadioButton->Enabled = UnCloseTabHotKeyCheckBox->Checked;
  UnCloseTabHotKeyInput->Enabled = UnCloseTabHotKeyMode2RadioButton->Checked;
  FastAccessClosedTabsCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  UnCloseTabHotKeyCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  CountClosedTabsLabel->Enabled = RememberClosedTabsCheckBox->Checked;
  CountClosedTabsCSpinEdit->Enabled = RememberClosedTabsCheckBox->Checked;
  RestoreLastMsgClosedTabsCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  OnlyConversationTabsCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  if(!RememberClosedTabsCheckBox->Checked)
  {
	FrmMainClosedTabsCheckBox->Enabled = false;
	FrmSendClosedTabsCheckBox->Enabled = false;
	ShowTimeClosedTabsCheckBox->Enabled = false;
	FastClearClosedTabsCheckBox->Enabled = false;
	ItemsCountClosedTabsLabel->Enabled = false;
	ItemsCountClosedTabsCSpinEdit->Enabled = false;
	UnCloseTabHotKeyMode1RadioButton->Enabled = false;
	UnCloseTabHotKeyMode2RadioButton->Enabled = false;
	UnCloseTabHotKeyInput->Enabled = false;
  }

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSessionRememberChkExecute(TObject *Sender)
{
  ManualRestoreTabsSessionCheckBox->Enabled = RestoreTabsSessionCheckBox->Checked;
  RestoreMsgSessionCheckBox->Enabled = RestoreTabsSessionCheckBox->Checked;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aTitlebarTweakChkExecute(TObject *Sender)
{
  TweakFrmSendTitlebarMode1RadioButton->Enabled = TweakFrmSendTitlebarCheckBox->Checked;
  TweakFrmSendTitlebarMode2RadioButton->Enabled = TweakFrmSendTitlebarCheckBox->Checked;
  TweakFrmSendTitlebarMode3RadioButton->Enabled = TweakFrmSendTitlebarCheckBox->Checked;
  TweakFrmMainTitlebarMode2Edit->Enabled = TweakFrmMainTitlebarMode2RadioButton->Checked;
  TweakFrmMainTitlebarMode1RadioButton->Enabled = TweakFrmMainTitlebarCheckBox->Checked;
  TweakFrmMainTitlebarMode2RadioButton->Enabled = TweakFrmMainTitlebarCheckBox->Checked;
  if(!TweakFrmMainTitlebarCheckBox->Checked)
   TweakFrmMainTitlebarMode2Edit->Enabled = false;
  if(!TweakFrmMainTitlebarMode2Edit->Enabled)
  {
	if(TweakFrmMainTitlebarMode2Edit->Text.IsEmpty())
	 TweakFrmMainTitlebarMode2Edit->Text = "Wpisz tutaj swój tekst";
  }
  else
  {
	if(TweakFrmMainTitlebarMode2Edit->Text=="Wpisz tutaj swój tekst")
	 TweakFrmMainTitlebarMode2Edit->Text = "";
  }

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UnsentMsgEraseButtonClick(TObject *Sender)
{
  EraseUnsentMsg();
  UnsentMsgEraseButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::ClosedTabsEraseButtonClick(TObject *Sender)
{
  EraseClosedTabs();
  ClosedTabsEraseButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SessionRememberEraseButtonClick(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Session.ini");
  Ini->EraseSection("Session");
  Ini->EraseSection("SessionMsg");
  delete Ini;
  SessionRememberEraseButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aNewMsgChkExecute(TObject *Sender)
{
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSettingsForm::aOtherChkExecute(TObject *Sender)
{
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OtherCategoryPanelExpand(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Session.ini");
  UnsentMsgEraseButton->Enabled = Ini->SectionExists("Messages");
  ClosedTabsEraseButton->Enabled = Ini->SectionExists("ClosedTabs");
  if((Ini->SectionExists("Session"))||(Ini->SectionExists("SessionMsg")))
   SessionRememberEraseButton->Enabled = true;
  else
   SessionRememberEraseButton->Enabled = false;
  ClipTabsEraseButton->Enabled = Ini->SectionExists("ClipTabs");
  FileListBox->Directory = GetPluginUserDirW() + "\\TabKit\\Avatars";
  FileListBox->Update();
  if(FileListBox->Items->Count)
   MiniAvatarsEraseButton->Enabled = true;
  else
   MiniAvatarsEraseButton->Enabled = false;
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormMouseWheelDown(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled)
{
  CategoryPanelGroup->VertScrollBar->Position = CategoryPanelGroup->VertScrollBar->Position + 10;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormMouseWheelUp(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled)
{
  CategoryPanelGroup->VertScrollBar->Position = CategoryPanelGroup->VertScrollBar->Position - 10;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::ClipTabsEraseButtonClick(TObject *Sender)
{
  EraseClipTabs();
  ClipTabsEraseButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aClipTabsChkExecute(TObject *Sender)
{
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
  unsigned int num = 0;
  unsigned int size = 0;

  Gdiplus::GetImageEncodersSize(&num, &size);
  if(size == 0)return -1;

  Gdiplus::ImageCodecInfo* imageCodecInfo = new Gdiplus::ImageCodecInfo[size];
  Gdiplus::GetImageEncoders(num, size, imageCodecInfo);

  for(unsigned int i = 0; i < num; ++i)
  {
	if(wcscmp(imageCodecInfo[i].MimeType, format) == 0)
	{
	  *pClsid = imageCodecInfo[i].Clsid;
	  delete[] imageCodecInfo;
	  return i;
	}
  }
  delete imageCodecInfo;
  return -1;
}
//-----------------------------------------------------------------

void __fastcall TSettingsForm::ConvertImage(UnicodeString Old, UnicodeString New)
{
  //Inicjalizacja GDIPlus
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
  //Konwertowanie grafiki
  Gdiplus::Graphics Grphx(Handle);
  Gdiplus::Image OrgImage(Old.w_str());
  Gdiplus::Image* Thumbnail = OrgImage.GetThumbnailImage(16, 16, NULL, NULL);
  Grphx.DrawImage(Thumbnail, 0, 0,Thumbnail->GetWidth(), Thumbnail->GetHeight());
  CLSID gifClsid;
  GetEncoderClsid(L"image/png", &gifClsid);
  Thumbnail->Save(New.w_str(), &gifClsid, NULL);
  delete Thumbnail;
  //Zakoñczenie sesji z GDIPlus
  Gdiplus::GdiplusShutdown(gdiplusToken);
}
//-----------------------------------------------------------------

void __fastcall TSettingsForm::MiniAvatarsEraseButtonClick(TObject *Sender)
{
  for(int Count=0;Count<FileListBox->Items->Count;Count++)
  {
	DeleteFile(FileListBox->Items->Strings[Count]);
  }
  EraseClipTabsIcons();
  MiniAvatarsEraseButton->Enabled = false;
}
//---------------------------------------------------------------------------

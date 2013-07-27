//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SettingsFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
__declspec(dllimport)void LoadSettings();
__declspec(dllimport)void DestroyFrmUnsentMsg();
__declspec(dllimport)void BuildFrmUnsentMsg();
__declspec(dllimport)void DestroyFrmClosedTabs();
__declspec(dllimport)void BuildFrmClosedTabs();
__declspec(dllimport)void ShowUnsentMsg();
__declspec(dllimport)UnicodeString GetPluginUserDir();
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
  PageControl->ActivePage = UnsentMsgTabSheet;

  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Settings.ini");
  //UnsentMsg
  RememberUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Enable",true);
  InfoUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Info",true);
  CloudUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Cloud",true);
  DetailedCloudUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","DetailedCloud",false);
  TrayUnsentMsgCheckBox->Checked =  Ini->ReadBool("UnsentMsg","Tray",true);
  FastAccessUnsentMsgCheckBox->Checked =  Ini->ReadBool("UnsentMsg","FastAccess",true);
  FrmMainUnsentMsgCheckBox->Checked =  Ini->ReadBool("UnsentMsg","FrmMain",true);
  FrmSendUnsentMsgCheckBox->Checked =  Ini->ReadBool("UnsentMsg","FrmSend",false);
  //TabsSwitching
  SwitchToNewMsgCheckBox->Checked = Ini->ReadBool("TabsSwitching","SwitchToNewMsg",true);
  int pSwitchToNewMsgMode = Ini->ReadInteger("TabsSwitching","SwitchToNewMsgMode",1);
  if(pSwitchToNewMsgMode==1)
   SwitchToNewMsgMode1RadioButton->Checked = true;
  else
   SwitchToNewMsgMode2RadioButton->Checked = true;
  TabsHotKeysCheckBox->Checked = Ini->ReadBool("TabsSwitching","TabsHotKeys",true);
  int pTabsHotKeysMode = Ini->ReadInteger("TabsSwitching","TabsHotKeysMode",1);
  if(pTabsHotKeysMode==1)
   TabsHotKeysMode1RadioButton->Checked = true;
  else
   TabsHotKeysMode2RadioButton->Checked = true;
  //ClosedTabs
  RememberClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","Enable",true);
  FastAccessClosedTabsCheckBox->Checked =  Ini->ReadBool("ClosedTabs","FastAccess",true);
  FrmMainClosedTabsCheckBox->Checked =  Ini->ReadBool("ClosedTabs","FrmMain",true);
  FrmSendClosedTabsCheckBox->Checked =  Ini->ReadBool("ClosedTabs","FrmSend",false);
  UnCloseTabHotKeyCheckBox->Checked =  Ini->ReadBool("ClosedTabs","UnCloseTabHotKey",false);
  int pUnCloseTabHotKeyMode = Ini->ReadInteger("ClosedTabs","UnCloseTabHotKeyMode",1);
  if(pUnCloseTabHotKeyMode==1)
   UnCloseTabHotKeyMode1RadioButton->Checked = true;
  else
   UnCloseTabHotKeyMode2RadioButton->Checked = true;
  UnCloseTabHotKeyInput->HotKey = Ini->ReadInteger("ClosedTabs","UnCloseTabHotKeyDef",0);
  //Mode potem
  delete Ini;

  aUnsentMsgChk->Execute();
  aTabsSwitchingChk->Execute();
  aClosedTabsChk->Execute();
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
  InfoUnsentMsgCheckBox->Enabled =  RememberUnsentMsgCheckBox->Checked;
  FastAccessUnsentMsgCheckBox->Enabled = RememberUnsentMsgCheckBox->Checked;
  if(!RememberUnsentMsgCheckBox->Checked)
  {
	CloudUnsentMsgCheckBox->Enabled = false;
	DetailedCloudUnsentMsgCheckBox->Enabled = false;
	TrayUnsentMsgCheckBox->Enabled = false;
	FrmMainUnsentMsgCheckBox->Enabled = false;
	FrmSendUnsentMsgCheckBox->Enabled = false;
  }

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::RememberUnsentMsgCheckBoxClick(TObject *Sender)
{
  aUnsentMsgChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::InfoUnsentMsgCheckBoxClick(TObject *Sender)
{
  aUnsentMsgChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::CloudUnsentMsgCheckBoxClick(TObject *Sender)
{
  aUnsentMsgChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::DetailedCloudUnsentMsgCheckBoxClick(TObject *Sender)
{
  aUnsentMsgChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TrayUnsentMsgCheckBoxClick(TObject *Sender)
{
  aUnsentMsgChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FastAccessUnsentMsgCheckBoxClick(TObject *Sender)
{
  aUnsentMsgChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FrmSendUnsentMsgCheckBoxClick(TObject *Sender)
{
  aUnsentMsgChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FrmMainUnsentMsgCheckBoxClick(TObject *Sender)
{
  aUnsentMsgChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsExecute(TObject *Sender)
{
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
  Ini->WriteBool("ClosedTabs","UnCloseTabHotKey",UnCloseTabHotKeyCheckBox->Checked);
  if(UnCloseTabHotKeyMode1RadioButton->Checked)
   Ini->WriteInteger("ClosedTabs","UnCloseTabHotKeyMode",1);
  else
   Ini->WriteInteger("ClosedTabs","UnCloseTabHotKeyMode",2);
  Ini->WriteInteger("ClosedTabs","UnCloseTabHotKeyDef",UnCloseTabHotKeyInput->HotKey);
  //Mode potem

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

void __fastcall TSettingsForm::SwitchToNewMsgCheckBoxClick(TObject *Sender)
{
  aTabsSwitchingChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SwitchToNewMsgMode1RadioButtonClick(TObject *Sender)
{
  aTabsSwitchingChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SwitchToNewMsgMode2RadioButtonClick(TObject *Sender)
{
  aTabsSwitchingChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TabsHotKeysCheckBoxClick(TObject *Sender)
{
  aTabsSwitchingChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TabsHotKeysMode1RadioButtonClick(TObject *Sender)
{
  aTabsSwitchingChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TabsHotKeysMode2RadioButtonClick(TObject *Sender)
{
  aTabsSwitchingChk->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TSettingsForm::aSaveSettingsWExecute(TObject *Sender)
{
  aSaveSettings->Execute();
  LoadSettings();
  DestroyFrmUnsentMsg();
  BuildFrmUnsentMsg();
  DestroyFrmClosedTabs();
  BuildFrmClosedTabs();
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
  UnCloseTabHotKeyMode1RadioButton->Enabled = UnCloseTabHotKeyCheckBox->Checked;
  UnCloseTabHotKeyMode2RadioButton->Enabled = UnCloseTabHotKeyCheckBox->Checked;
  UnCloseTabHotKeyInput->Enabled = UnCloseTabHotKeyMode2RadioButton->Checked;
  FastAccessClosedTabsCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  UnCloseTabHotKeyCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  if(!RememberClosedTabsCheckBox->Checked)
  {
	FrmMainClosedTabsCheckBox->Enabled = false;
	FrmSendClosedTabsCheckBox->Enabled = false;
	UnCloseTabHotKeyMode1RadioButton->Enabled = false;
	UnCloseTabHotKeyMode2RadioButton->Enabled = false;
	UnCloseTabHotKeyInput->Enabled = false;
  }

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::RememberClosedTabsCheckBoxClick(TObject *Sender)
{
  aClosedTabsChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FastAccessClosedTabsCheckBoxClick(TObject *Sender)
{
  aClosedTabsChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FrmMainClosedTabsCheckBoxClick(TObject *Sender)
{
  aClosedTabsChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FrmSendClosedTabsCheckBoxClick(TObject *Sender)
{
  aClosedTabsChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UnCloseTabHotKeyCheckBoxClick(TObject *Sender)
{
  aClosedTabsChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UnCloseTabHotKeyMode1RadioButtonClick(TObject *Sender)
{
  aClosedTabsChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UnCloseTabHotKeyMode2RadioButtonClick(TObject *Sender)
{
  aClosedTabsChk->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UnCloseTabHotKeyInputChange(TObject *Sender)
{
  aClosedTabsChk->Execute();
}
//---------------------------------------------------------------------------


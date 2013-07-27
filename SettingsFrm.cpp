//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SettingsFrm.h"
#include "SideSlideExceptionsFrm.h"
#include <gdiplus.h>
#include <inifiles.hpp>
#include <XMLDoc.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "acPNG"
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sCheckBox"
#pragma link "sComboBox"
#pragma link "sEdit"
#pragma link "sGroupBox"
#pragma link "sLabel"
#pragma link "sPageControl"
#pragma link "sRadioButton"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma link "sSpeedButton"
#pragma link "sSpinEdit"
#pragma link "acAlphaImageList"
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetPluginUserDirW();
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)UnicodeString NormalizeChannel(UnicodeString Channel);
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkThemeAnimateWindows();
__declspec(dllimport)bool ChkThemeGlowing();
__declspec(dllimport)int GetHUE();
__declspec(dllimport)int GetSaturation();
__declspec(dllimport)void LoadSettings();
__declspec(dllimport)void RefreshTabs();
__declspec(dllimport)void DestroyFrmClosedTabs();
__declspec(dllimport)void BuildFrmClosedTabs();
__declspec(dllimport)void EraseClosedTabs();
__declspec(dllimport)void DestroyFrmUnsentMsg();
__declspec(dllimport)void BuildFrmUnsentMsg();
__declspec(dllimport)void EraseUnsentMsg();
__declspec(dllimport)bool ShowUnsentMsg();
__declspec(dllimport)void ChangeFrmSendTitlebar();
__declspec(dllimport)void ChangeFrmMainTitlebar();
__declspec(dllimport)void EraseClipTabs();
__declspec(dllimport)void EraseClipTabsIcons();
__declspec(dllimport)void HookGlobalKeyboard();
__declspec(dllimport)int GetMinimizeRestoreFrmSendKey();
__declspec(dllimport)void MinimizeRestoreFrmSendExecute();
__declspec(dllimport)void MinimizeRestoreFrmMainExecute();
__declspec(dllimport)void DestroyStayOnTop();
__declspec(dllimport)void BuildStayOnTop();
__declspec(dllimport)void ShowToolBar();
__declspec(dllimport)void CheckHideScrollTabButtons();
//---------------------------------------------------------------------------
bool pHideTabCloseButtonChk;
bool pMiniAvatarsClipTabsChk;
//---------------------------------------------------------------------------
__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::WMTransparency(TMessage &Message)
{
  Application->ProcessMessages();
  if(sSkinManager->Active) sSkinProvider->BorderForm->UpdateExBordersPos(true,(int)Message.LParam);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::WMHotKey(TMessage &Message)
{
  //Minimalizacja / przywracanie okna rozmowy + FrmSendSideSlide
  if(Message.WParam==0x0100)
  {
	if(!MinimizeRestoreHotKey->Focused())
	 MinimizeRestoreFrmSendExecute();
	else
	 MinimizeRestoreHotKey->HotKey = GetMinimizeRestoreFrmSendKey();
  }
  //SideSlide dla FrmMain + otwieranie nowych wiadomosci
  if(Message.WParam==0x0200)
  {
	if(!MinimizeRestoreHotKey->Focused())
	 MinimizeRestoreFrmMainExecute();
	else
	 MinimizeRestoreHotKey->HotKey = GetMinimizeRestoreFrmSendKey();
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormCreate(TObject *Sender)
{
  //Wlaczona zaawansowana stylizacja okien
  if(ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	//Plik zaawansowanej stylizacji okien istnieje
	if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  if(ChkThemeAnimateWindows()) sSkinManager->AnimEffects->FormShow->Time = 200;
	  else sSkinManager->AnimEffects->FormShow->Time = 0;
	  sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
	  sSkinManager->Active = true;
	}
	//Brak pliku zaawansowanej stylizacji okien
	else sSkinManager->Active = false;
  }
  //Zaawansowana stylizacja okien wylaczona
  else sSkinManager->Active = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormShow(TObject *Sender)
{
  if(sSkinManager->Active)
  {
	//Kolor WebLabel'ow
	EmailWebLabel->Font->Color = sSkinManager->GetGlobalFontColor();
	EmailWebLabel->HoverFont->Color = sSkinManager->GetGlobalFontColor();
	JabberWebLabel->Font->Color = sSkinManager->GetGlobalFontColor();
	JabberWebLabel->HoverFont->Color = sSkinManager->GetGlobalFontColor();
	URLWebLabel->Font->Color = sSkinManager->GetGlobalFontColor();
	URLWebLabel->HoverFont->Color = sSkinManager->GetGlobalFontColor();
	ForumWebLabel->Font->Color = sSkinManager->GetGlobalFontColor();
	ForumWebLabel->HoverFont->Color = sSkinManager->GetGlobalFontColor();
	BugWebLabel->Font->Color = sSkinManager->GetGlobalFontColor();
	BugWebLabel->HoverFont->Color = sSkinManager->GetGlobalFontColor();
	StarWebLabel->Font->Color = sSkinManager->GetGlobalFontColor();
	StarWebLabel->HoverFont->Color = sSkinManager->GetGlobalFontColor();
	OtherPaymentsWebLabel->Font->Color = sSkinManager->GetGlobalFontColor();
	OtherPaymentsWebLabel->HoverFont->Color = sSkinManager->GetGlobalFontColor();
	//Zmiana kolorystyki AlphaControls
	sSkinManager->HueOffset = GetHUE();
	sSkinManager->Saturation = GetSaturation();
  }
  else
  {
	//Kolor WebLabel'ow
	EmailWebLabel->Font->Color = clWindowText;
	EmailWebLabel->HoverFont->Color = clWindowText;
	JabberWebLabel->Font->Color = clWindowText;
	JabberWebLabel->HoverFont->Color = clWindowText;
	URLWebLabel->Font->Color = clWindowText;
	URLWebLabel->HoverFont->Color = clWindowText;
	ForumWebLabel->Font->Color = clWindowText;
	ForumWebLabel->HoverFont->Color = clWindowText;
	BugWebLabel->Font->Color = clWindowText;
	BugWebLabel->HoverFont->Color = clWindowText;
	StarWebLabel->Font->Color = clWindowText;
	StarWebLabel->HoverFont->Color = clWindowText;
	OtherPaymentsWebLabel->Font->Color = clWindowText;
	OtherPaymentsWebLabel->HoverFont->Color = clWindowText;
  }
  //Odczyt ustawien
  aLoadSettings->Execute();
  //Ustawienie domyslnej zakladki
  sPageControl->ActivePage = DefaultTabSheet;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aLoadSettingsExecute(TObject *Sender)
{
  //Odczyt ustawien
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Settings.ini");
  //ClosedTabs
  RememberClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","Enable",true);
  FastAccessClosedTabsCheckBox->Checked =  Ini->ReadBool("ClosedTabs","FastAccess",true);
  FrmMainClosedTabsCheckBox->Checked =  Ini->ReadBool("ClosedTabs","FrmMain",true);
  FrmSendClosedTabsCheckBox->Checked =  Ini->ReadBool("ClosedTabs","FrmSend",true);
  ItemsCountClosedTabsSpinEdit->Value = Ini->ReadInteger("ClosedTabs","ItemsCount",5);
  ShowTimeClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","ClosedTime",false);
  FastClearClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","FastClear",true);
  UnCloseTabHotKeyCheckBox->Checked =  Ini->ReadBool("ClosedTabs","HotKey",false);
  if(Ini->ReadInteger("ClosedTabs","HotKeyMode",1)==1)
   UnCloseTabHotKeyMode1RadioButton->Checked = true;
  else
   UnCloseTabHotKeyMode2RadioButton->Checked = true;
  UnCloseTabHotKeyInput->HotKey = Ini->ReadInteger("ClosedTabs","HotKeyDef",0);
  UnCloseTabSPMouseCheckBox->Checked = Ini->ReadBool("ClosedTabs","SPMouse",true);
  UnCloseTabLPMouseCheckBox->Checked = Ini->ReadBool("ClosedTabs","LPMouse",false);
  CountClosedTabsSpinEdit->Value = Ini->ReadInteger("ClosedTabs","Count",5);
  RestoreLastMsgClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","RestoreLastMsg",false);
  OnlyConversationTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","OnlyConversationTabs",false);
  //UnsentMsg
  RememberUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Enable",true);
  InfoUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Info",true);
  CloudUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Cloud",true);
  DetailedCloudUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","DetailedCloud",false);
  TrayUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Tray",true);
  FastAccessUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FastAccess",true);
  FrmMainUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FrmMain",true);
  FrmSendUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FrmSend",true);
  FastClearUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FastClear",true);
  //TabsSwitching
  SwitchToNewMsgCheckBox->Checked = Ini->ReadBool("TabsSwitching","SwitchToNewMsg",true);
  if(Ini->ReadInteger("TabsSwitching","SwitchToNewMsgMode",1)==1)
   SwitchToNewMsgMode1RadioButton->Checked = true;
  else
   SwitchToNewMsgMode2RadioButton->Checked = true;
  TabsHotKeysCheckBox->Checked = Ini->ReadBool("TabsSwitching","TabsHotKeys",true);
  if(Ini->ReadInteger("TabsSwitching","TabsHotKeysMode",2)==1)
   TabsHotKeysMode1RadioButton->Checked = true;
  else
   TabsHotKeysMode2RadioButton->Checked = true;
  NewMgsHoyKeyCheckBox->Checked = Ini->ReadBool("TabsSwitching","NewMgsHoyKey",false);
  //SessionRemember
  RestoreTabsSessionCheckBox->Checked = Ini->ReadBool("SessionRemember","RestoreTabs",true);
  ManualRestoreTabsSessionCheckBox->Checked = Ini->ReadBool("SessionRemember","ManualRestoreTabs",false);
  RestoreMsgSessionCheckBox->Checked = Ini->ReadBool("SessionRemember","RestoreMsg",false);
  //NewMsg
  InactiveFrmNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","InactiveFrm",true);
  OffCoreInactiveTabsNewMsgCheckBox->Checked = !Ini->ReadBool("NewMsg","CoreInactiveTabs",true);
  InactiveTabsNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","InactiveTabs",false);
  InactiveNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","InactiveNotifer",false);
  ChatStateNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","ChatStateNotifer",true);
  ChatGoneNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","ChatGoneNotifer",false);
  TaskbarPenCheckBox->Checked = !Ini->ReadBool("NewMsg","TaskbarPen",true);
  //Titlebar
  TweakFrmSendTitlebarCheckBox->Checked = Ini->ReadBool("Titlebar","TweakSend",false);
  switch(Ini->ReadInteger("Titlebar","SendMode",1))
  {
	case 1:
	 TweakFrmSendTitlebarMode1RadioButton->Checked = true;
	 break;
	case 2:
	 TweakFrmSendTitlebarMode2RadioButton->Checked = true;
	 break;
	case 3:
	 TweakFrmSendTitlebarMode3RadioButton->Checked = true;
	 break;
	case 4:
	 TweakFrmSendTitlebarMode4RadioButton->Checked = true;
	 break;
  }
  TweakFrmMainTitlebarCheckBox->Checked = Ini->ReadBool("Titlebar","TweakMain",false);
  if(Ini->ReadInteger("Titlebar","MainMode",1)==1)
   TweakFrmMainTitlebarMode1RadioButton->Checked = true;
  else
   TweakFrmMainTitlebarMode2RadioButton->Checked = true;
  TweakFrmMainTitlebarModeExComboBox->ItemIndex = Ini->ReadInteger("Titlebar","MainModeEx",0);
  TweakFrmMainTitlebarMode2Edit->Text = Ini->ReadString("Titlebar","MainText","");
  //ClipTabs
  OpenClipTabsCheckBox->Checked = Ini->ReadBool("ClipTabs","OpenClipTabs",true);
  InactiveClipTabsCheckBox->Checked = Ini->ReadBool("ClipTabs","InactiveClipTabs",false);
  CounterClipTabsCheckBox->Checked = Ini->ReadBool("ClipTabs","Counter",false);
  ExcludeClipTabsFromTabSwitchingCheckBox->Checked = Ini->ReadBool("ClipTabs","ExcludeFromTabSwitching",false);
  ExcludeClipTabsFromSwitchToNewMsgCheckBox->Checked = !Ini->ReadBool("ClipTabs","ExcludeFromSwitchToNewMsg",true);
  ExcludeClipTabsFromTabsHotKeysCheckBox->Checked = Ini->ReadBool("ClipTabs","ExcludeFromTabsHotKeys",false);
  NoMiniAvatarsClipTabsCheckBox->Checked = !Ini->ReadBool("ClipTabs","MiniAvatars",true);
  pMiniAvatarsClipTabsChk = NoMiniAvatarsClipTabsCheckBox->Checked;
  //SideSlide
  SlideFrmMainCheckBox->Checked = Ini->ReadBool("SideSlide","SlideFrmMain",false);
  switch(Ini->ReadInteger("SideSlide","FrmMainEdge",2))
  {
	case 1:
	 FrmMainEdgeLeftRadioButton->Checked = true;
	 break;
	case 2:
	 FrmMainEdgeRightRadioButton->Checked = true;
	 break;
	case 3:
	 FrmMainEdgeBottomRadioButton->Checked = true;
	 break;
	case 4:
	 FrmMainEdgeTopRadioButton->Checked = true;
	 break;
  }
  switch(Ini->ReadInteger("SideSlide","FrmMainHideMode",3))
  {
	case 1:
	 FrmMainHideFocusRadioButton->Checked = true;
	 break;
	case 2:
	 FrmMainHideAppFocusRadioButton->Checked = true;
	 break;
	case 3:
	 FrmMainHideCursorRadioButton->Checked = true;
	 break;
  }
  FrmMainSlideInDelaySpinEdit->Value = Ini->ReadInteger("SideSlide","FrmMainSlideInDelay",1000);
  FrmMainSlideOutDelaySpinEdit->Value = Ini->ReadInteger("SideSlide","FrmMainSlideOutDelay",1);
  FrmMainSlideInTimeSpinEdit->Value = Ini->ReadInteger("SideSlide","FrmMainSlideInTime",300);
  FrmMainSlideOutTimeSpinEdit->Value = Ini->ReadInteger("SideSlide","FrmMainSlideOutTime",500);
  ChangeTabAfterSlideInCheckBox->Checked = Ini->ReadBool("SideSlide","ChangeTabAfterSlideIn",true);
  SlideFrmSendCheckBox->Checked = Ini->ReadBool("SideSlide","SlideFrmSend",false);
  switch(Ini->ReadInteger("SideSlide","FrmSendEdge",1))
  {
	case 1:
	 FrmSendEdgeLeftRadioButton->Checked = true;
	 break;
	case 2:
	 FrmSendEdgeRightRadioButton->Checked = true;
	 break;
	case 3:
	 FrmSendEdgeBottomRadioButton->Checked = true;
	 break;
	case 4:
	 FrmSendEdgeTopRadioButton->Checked = true;
	 break;
  }
  switch(Ini->ReadInteger("SideSlide","FrmSendHideMode",1))
  {
	case 1:
	 FrmSendHideFocusRadioButton->Checked = true;
	 break;
	case 2:
	 FrmSendHideAppFocusRadioButton->Checked = true;
	 break;
	case 3:
	 FrmSendHideCursorRadioButton->Checked = true;
	 break;
  }
  FrmSendSlideInDelaySpinEdit->Value = Ini->ReadInteger("SideSlide","FrmSendSlideInDelay",1000);
  FrmSendSlideOutDelaySpinEdit->Value = Ini->ReadInteger("SideSlide","FrmSendSlideOutDelay",1);
  FrmSendSlideInTimeSpinEdit->Value = Ini->ReadInteger("SideSlide","FrmSendSlideInTime",300);
  FrmSendSlideOutTimeSpinEdit->Value = Ini->ReadInteger("SideSlide","FrmSendSlideOutTime",500);
  SlideInAtNewMsgCheckBox->Checked = Ini->ReadBool("SideSlide","SlideInAtNewMsg",false);
  SideSlideFullScreenModeCheckBox->Checked = Ini->ReadBool("SideSlide","FullScreenMode",true);
  SideSlideCtrlAndMousBlockCheckBox->Checked = Ini->ReadBool("SideSlide","CtrlAndMouseBlock",true);
  //Other
  QuickQuoteCheckBox->Checked = Ini->ReadBool("Other","QuickQuote",true);
  CollapseImagesCheckBox->Checked = Ini->ReadBool("Other","CollapseImages",true);
  CollapseImagesModeComboBox->ItemIndex = Ini->ReadInteger("Other","CollapseImagesMode",1)-1;
  AntiSpimCheckBox->Checked = !Ini->ReadBool("Other","AntiSpim",false);
  MinimizeRestoreCheckBox->Checked = Ini->ReadBool("Other","MinimizeRestore",false);
  MinimizeRestoreHotKey->HotKey = Ini->ReadInteger("Other","MinimizeRestoreHotKey",24689);
  StayOnTopCheckBox->Checked = Ini->ReadBool("Other","StayOnTop",false);
  HideToolBarCheckBox->Checked = Ini->ReadBool("Other","HideToolBar",false);
  HideTabCloseButtonCheckBox->Checked = Ini->ReadBool("Other","HideTabCloseButton",false);
  pHideTabCloseButtonChk = HideTabCloseButtonCheckBox->Checked;
  HideScrollTabButtonsCheckBox->Checked = Ini->ReadBool("Other","HideScrollTabButtons",false);
  CloseBy2xLPMCheckBox->Checked = Ini->ReadBool("Other","CloseBy2xLPM",false);
  EmuTabsWCheckBox->Checked = Ini->ReadBool("Other","EmuTabsW",false);
  CloudTimeOutSpinEdit->Value = Ini->ReadInteger("Other","CloudTimeOut",6);
  CloudTickModeComboBox->ItemIndex = Ini->ReadBool("Other","CloudTickMode",true);
  SearchOnListCheckBox->Checked = !Ini->ReadBool("Other","SearchOnList",true);
  ShortenLinksCheckBox->Checked = Ini->ReadBool("Other","ShortenLinks",true);
  //Buttons state
  Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Session.ini");
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
  //Ustawienie stanow kontrolek
  aClosedTabsChk->Execute();
  aUnsentMsgChk->Execute();
  aTabsSwitchingChk->Execute();
  aSessionRememberChk->Execute();
  aNewMsgChk->Execute();
  aTitlebarTweakChk->Execute();
  aClipTabsChk->Execute();
  aSideSlideChk->Execute();
  aOtherChk->Execute();
  SaveButton->Enabled = false;
  CancelButton->SetFocus();
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
  //ClosedTabs
  Ini->WriteBool("ClosedTabs","Enable",RememberClosedTabsCheckBox->Checked);
  if((!FrmMainClosedTabsCheckBox->Checked)&&(!FrmSendClosedTabsCheckBox->Checked))
   Ini->WriteBool("ClosedTabs","FastAccess",false);
  else
   Ini->WriteBool("ClosedTabs","FastAccess",FastAccessClosedTabsCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","FrmMain",FrmMainClosedTabsCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","FrmSend",FrmSendClosedTabsCheckBox->Checked);
  Ini->WriteInteger("ClosedTabs","ItemsCount",ItemsCountClosedTabsSpinEdit->Value);
  Ini->WriteBool("ClosedTabs","ClosedTime",ShowTimeClosedTabsCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","FastClear",FastClearClosedTabsCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","HotKey",UnCloseTabHotKeyCheckBox->Checked);
  if(UnCloseTabHotKeyMode1RadioButton->Checked)
   Ini->WriteInteger("ClosedTabs","HotKeyMode",1);
  else
   Ini->WriteInteger("ClosedTabs","HotKeyMode",2);
  Ini->WriteInteger("ClosedTabs","HotKeyDef",UnCloseTabHotKeyInput->HotKey);
  Ini->WriteBool("ClosedTabs","SPMouse",UnCloseTabSPMouseCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","LPMouse",UnCloseTabLPMouseCheckBox->Checked);
  Ini->WriteInteger("ClosedTabs","Count",CountClosedTabsSpinEdit->Value);
  Ini->WriteBool("ClosedTabs","RestoreLastMsg",RestoreLastMsgClosedTabsCheckBox->Checked);
  Ini->WriteBool("ClosedTabs","OnlyConversationTabs",OnlyConversationTabsCheckBox->Checked);
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
  Ini->WriteBool("TabsSwitching","NewMgsHoyKey",NewMgsHoyKeyCheckBox->Checked);
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
  //NewMsg
  Ini->WriteBool("NewMsg","InactiveFrm",InactiveFrmNewMsgCheckBox->Checked);
  Ini->WriteBool("NewMsg","CoreInactiveTabs",!OffCoreInactiveTabsNewMsgCheckBox->Checked);
  Ini->WriteBool("NewMsg","InactiveTabs",InactiveTabsNewMsgCheckBox->Checked);
  Ini->WriteBool("NewMsg","InactiveNotifer",InactiveNotiferNewMsgCheckBox->Checked);
  Ini->WriteBool("NewMsg","ChatStateNotifer",ChatStateNotiferNewMsgCheckBox->Checked);
  Ini->WriteBool("NewMsg","ChatGoneNotifer",ChatGoneNotiferNewMsgCheckBox->Checked);
  Ini->WriteBool("NewMsg","TaskbarPen",!TaskbarPenCheckBox->Checked);
  //Titlebar
  Ini->WriteBool("Titlebar","TweakSend",TweakFrmSendTitlebarCheckBox->Checked);
  if(TweakFrmSendTitlebarMode1RadioButton->Checked)
   Ini->WriteInteger("Titlebar","SendMode",1);
  else if(TweakFrmSendTitlebarMode2RadioButton->Checked)
   Ini->WriteInteger("Titlebar","SendMode",2);
  else if(TweakFrmSendTitlebarMode3RadioButton->Checked)
   Ini->WriteInteger("Titlebar","SendMode",3);
  else
   Ini->WriteInteger("Titlebar","SendMode",4);
  Ini->WriteBool("Titlebar","TweakMain",TweakFrmMainTitlebarCheckBox->Checked);
  if(TweakFrmMainTitlebarMode1RadioButton->Checked)
   Ini->WriteInteger("Titlebar","MainMode",1);
  else
   Ini->WriteInteger("Titlebar","MainMode",2);
  if(TweakFrmMainTitlebarModeExComboBox->ItemIndex!=-1)
   Ini->WriteInteger("Titlebar","MainModeEx",TweakFrmMainTitlebarModeExComboBox->ItemIndex);
  if(TweakFrmMainTitlebarMode2Edit->Text!="Wpisz tutaj swój tekst")
   Ini->WriteString("Titlebar","MainText",TweakFrmMainTitlebarMode2Edit->Text);
  else
   Ini->WriteString("Titlebar","MainText","");
  //ClipTabs
  Ini->WriteBool("ClipTabs","OpenClipTabs",OpenClipTabsCheckBox->Checked);
  Ini->WriteBool("ClipTabs","InactiveClipTabs",InactiveClipTabsCheckBox->Checked);
  Ini->WriteBool("ClipTabs","Counter",CounterClipTabsCheckBox->Checked);
  Ini->WriteBool("ClipTabs","ExcludeFromTabSwitching",ExcludeClipTabsFromTabSwitchingCheckBox->Checked);
  Ini->WriteBool("ClipTabs","ExcludeFromSwitchToNewMsg",!ExcludeClipTabsFromSwitchToNewMsgCheckBox->Checked);
  Ini->WriteBool("ClipTabs","ExcludeFromTabsHotKeys",ExcludeClipTabsFromTabsHotKeysCheckBox->Checked);
  Ini->WriteBool("ClipTabs","MiniAvatars",!NoMiniAvatarsClipTabsCheckBox->Checked);
  //SideSlide
  Ini->WriteBool("SideSlide","SlideFrmMain",SlideFrmMainCheckBox->Checked);
  if(FrmMainEdgeLeftRadioButton->Checked)
   Ini->WriteInteger("SideSlide","FrmMainEdge",1);
  else if(FrmMainEdgeRightRadioButton->Checked)
   Ini->WriteInteger("SideSlide","FrmMainEdge",2);
  else if(FrmMainEdgeBottomRadioButton->Checked)
   Ini->WriteInteger("SideSlide","FrmMainEdge",3);
  else
   Ini->WriteInteger("SideSlide","FrmMainEdge",4);
  if(FrmMainHideFocusRadioButton->Checked)
   Ini->WriteInteger("SideSlide","FrmMainHideMode",1);
  else if(FrmMainHideAppFocusRadioButton->Checked)
   Ini->WriteInteger("SideSlide","FrmMainHideMode",2);
  else
   Ini->WriteInteger("SideSlide","FrmMainHideMode",3);
  Ini->WriteInteger("SideSlide","FrmMainSlideInDelay",FrmMainSlideInDelaySpinEdit->Value);
  Ini->WriteInteger("SideSlide","FrmMainSlideOutDelay",FrmMainSlideOutDelaySpinEdit->Value);
  Ini->WriteInteger("SideSlide","FrmMainSlideInTime",FrmMainSlideInTimeSpinEdit->Value);
  Ini->WriteInteger("SideSlide","FrmMainSlideOutTime",FrmMainSlideOutTimeSpinEdit->Value);
  Ini->WriteBool("SideSlide","ChangeTabAfterSlideIn",ChangeTabAfterSlideInCheckBox->Checked);
  Ini->WriteBool("SideSlide","SlideFrmSend",SlideFrmSendCheckBox->Checked);
  if(FrmSendEdgeLeftRadioButton->Checked)
   Ini->WriteInteger("SideSlide","FrmSendEdge",1);
  else if(FrmSendEdgeRightRadioButton->Checked)
   Ini->WriteInteger("SideSlide","FrmSendEdge",2);
  else if(FrmSendEdgeBottomRadioButton->Checked)
   Ini->WriteInteger("SideSlide","FrmSendEdge",3);
  else
   Ini->WriteInteger("SideSlide","FrmSendEdge",4);
  if(FrmSendHideFocusRadioButton->Checked)
   Ini->WriteInteger("SideSlide","FrmSendHideMode",1);
  else if(FrmSendHideAppFocusRadioButton->Checked)
   Ini->WriteInteger("SideSlide","FrmSendHideMode",2);
  else
   Ini->WriteInteger("SideSlide","FrmSendHideMode",3);
  Ini->WriteInteger("SideSlide","FrmSendSlideInDelay",FrmSendSlideInDelaySpinEdit->Value);
  Ini->WriteInteger("SideSlide","FrmSendSlideOutDelay",FrmSendSlideOutDelaySpinEdit->Value);
  Ini->WriteInteger("SideSlide","FrmSendSlideInTime",FrmSendSlideInTimeSpinEdit->Value);
  Ini->WriteInteger("SideSlide","FrmSendSlideOutTime",FrmSendSlideOutTimeSpinEdit->Value);
  Ini->WriteBool("SideSlide","SlideInAtNewMsg",SlideInAtNewMsgCheckBox->Checked);
  Ini->WriteBool("SideSlide","FullScreenMode",SideSlideFullScreenModeCheckBox->Checked);
  Ini->WriteBool("SideSlide","CtrlAndMouseBlock",SideSlideCtrlAndMousBlockCheckBox->Checked);
  //Other
  Ini->WriteBool("Other","QuickQuote",QuickQuoteCheckBox->Checked);
  Ini->WriteBool("Other","CollapseImages",CollapseImagesCheckBox->Checked);
  Ini->WriteInteger("Other","CollapseImagesMode",CollapseImagesModeComboBox->ItemIndex+1);
  Ini->WriteBool("Other","AntiSpim",!AntiSpimCheckBox->Checked);
  Ini->WriteBool("Other","MinimizeRestore",MinimizeRestoreCheckBox->Checked);
  Ini->WriteInteger("Other","MinimizeRestoreHotKey",MinimizeRestoreHotKey->HotKey);
  Ini->WriteBool("Other","StayOnTop",StayOnTopCheckBox->Checked);
  Ini->WriteBool("Other","HideToolBar",HideToolBarCheckBox->Checked);
  Ini->WriteBool("Other","HideTabCloseButton",HideTabCloseButtonCheckBox->Checked);
  Ini->WriteBool("Other","HideScrollTabButtons",HideScrollTabButtonsCheckBox->Checked);
  Ini->WriteBool("Other","CloseBy2xLPM",CloseBy2xLPMCheckBox->Checked);
  Ini->WriteBool("Other","EmuTabsW",EmuTabsWCheckBox->Checked);
  Ini->WriteInteger("Other","CloudTimeOut",CloudTimeOutSpinEdit->Value);
  Ini->WriteBool("Other","CloudTickMode",CloudTickModeComboBox->ItemIndex);
  Ini->WriteBool("Other","SearchOnList",!SearchOnListCheckBox->Checked);
  Ini->WriteBool("Other","ShortenLinks",ShortenLinksCheckBox->Checked);

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsWExecute(TObject *Sender)
{
  //Usuwanie elementow z interfejsu AQQ
  DestroyFrmUnsentMsg();
  DestroyFrmClosedTabs();
  DestroyStayOnTop();
  //Zapisywanie ustawien
  aSaveSettings->Execute();
  //Odczytywanie ustawien w rdzeniu wtyczki
  LoadSettings();
  //Przypisywanie globalnego hooka na klawiature
  HookGlobalKeyboard();
  //Tworzenie elementow w interfejsie AQQ
  BuildFrmClosedTabs();
  BuildFrmUnsentMsg();
  BuildStayOnTop();
  //Zmiana tekstu na pasku tytulowym okna rozmowy
  ChangeFrmSendTitlebar();
  //Zmiana tekstu na pasku tytulowym okna kontaktow
  ChangeFrmMainTitlebar();
  //Ukrywanie/pokazywanie elementow okna rozmowy
  ShowToolBar();
  CheckHideScrollTabButtons();
  //Odswiezenie wszystkich zakladek
  if((pMiniAvatarsClipTabsChk!=NoMiniAvatarsClipTabsCheckBox->Checked)
  ||(pHideTabCloseButtonChk!=HideTabCloseButtonCheckBox->Checked))
   RefreshTabs();
  pMiniAvatarsClipTabsChk = NoMiniAvatarsClipTabsCheckBox->Checked;
  pHideTabCloseButtonChk = HideTabCloseButtonCheckBox->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aClosedTabsChkExecute(TObject *Sender)
{
  FrmMainClosedTabsCheckBox->Enabled = FastAccessClosedTabsCheckBox->Checked;
  FrmSendClosedTabsCheckBox->Enabled = FastAccessClosedTabsCheckBox->Checked;
  ShowTimeClosedTabsCheckBox->Enabled = FastAccessClosedTabsCheckBox->Checked;
  FastClearClosedTabsCheckBox->Enabled = FastAccessClosedTabsCheckBox->Checked;
  ItemsCountClosedTabsLabel->Enabled = FastAccessClosedTabsCheckBox->Checked;
  ItemsCountClosedTabsSpinEdit->Enabled = FastAccessClosedTabsCheckBox->Checked;
  UnCloseTabHotKeyMode1RadioButton->Enabled = UnCloseTabHotKeyCheckBox->Checked;
  UnCloseTabHotKeyMode2RadioButton->Enabled = UnCloseTabHotKeyCheckBox->Checked;
  UnCloseTabHotKeyInput->Enabled = UnCloseTabHotKeyMode2RadioButton->Checked;
  FastAccessClosedTabsCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  UnCloseTabHotKeyCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  CountClosedTabsLabel->Enabled = RememberClosedTabsCheckBox->Checked;
  CountClosedTabsSpinEdit->Enabled = RememberClosedTabsCheckBox->Checked;
  RestoreLastMsgClosedTabsCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  OnlyConversationTabsCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  UnCloseMouseLabel->Enabled = RememberClosedTabsCheckBox->Checked;
  UnCloseTabSPMouseCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  UnCloseTabLPMouseCheckBox->Enabled = RememberClosedTabsCheckBox->Checked;
  if(!RememberClosedTabsCheckBox->Checked)
  {
	FrmMainClosedTabsCheckBox->Enabled = false;
	FrmSendClosedTabsCheckBox->Enabled = false;
	ShowTimeClosedTabsCheckBox->Enabled = false;
	FastClearClosedTabsCheckBox->Enabled = false;
	ItemsCountClosedTabsLabel->Enabled = false;
	ItemsCountClosedTabsSpinEdit->Enabled = false;
	UnCloseTabHotKeyMode1RadioButton->Enabled = false;
	UnCloseTabHotKeyMode2RadioButton->Enabled = false;
	UnCloseTabHotKeyInput->Enabled = false;
  }

  SaveButton->Enabled = true;
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

void __fastcall TSettingsForm::aTabsSwitchingChkExecute(TObject *Sender)
{
  SwitchToNewMsgMode1RadioButton->Enabled = SwitchToNewMsgCheckBox->Checked;
  SwitchToNewMsgMode2RadioButton->Enabled = SwitchToNewMsgCheckBox->Checked;
  TabsHotKeysMode1RadioButton->Enabled = TabsHotKeysCheckBox->Checked;
  TabsHotKeysMode2RadioButton->Enabled = TabsHotKeysCheckBox->Checked;
  NewMgsHoyKeyCheckBox->Enabled = TabsHotKeysCheckBox->Checked;

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

void __fastcall TSettingsForm::aNewMsgChkExecute(TObject *Sender)
{
  OffCoreInactiveTabsNewMsgCheckBox->Enabled = !InactiveTabsNewMsgCheckBox->Checked;
  if((InactiveFrmNewMsgCheckBox->Checked)||(TweakFrmSendTitlebarCheckBox->Checked))
   TaskbarPenCheckBox->Enabled = false;
  else
   TaskbarPenCheckBox->Enabled = true;

  ChatGoneNotiferNewMsgCheckBox->Enabled = ChatStateNotiferNewMsgCheckBox->Checked;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aTitlebarTweakChkExecute(TObject *Sender)
{
  TweakFrmSendTitlebarMode1RadioButton->Enabled = TweakFrmSendTitlebarCheckBox->Checked;
  TweakFrmSendTitlebarMode2RadioButton->Enabled = TweakFrmSendTitlebarCheckBox->Checked;
  TweakFrmSendTitlebarMode3RadioButton->Enabled = TweakFrmSendTitlebarCheckBox->Checked;
  TweakFrmSendTitlebarMode4RadioButton->Enabled = TweakFrmSendTitlebarCheckBox->Checked;
  TweakFrmMainTitlebarMode1RadioButton->Enabled = TweakFrmMainTitlebarCheckBox->Checked;
  TweakFrmMainTitlebarModeExComboBox->Enabled = TweakFrmMainTitlebarCheckBox->Checked;
  if(TweakFrmMainTitlebarCheckBox->Checked) TweakFrmMainTitlebarModeExComboBox->Enabled = TweakFrmMainTitlebarMode1RadioButton->Checked;
  TweakFrmMainTitlebarModeExLabel->Enabled = TweakFrmMainTitlebarCheckBox->Checked;
  TweakFrmMainTitlebarMode2RadioButton->Enabled = TweakFrmMainTitlebarCheckBox->Checked;
  TweakFrmMainTitlebarMode2Edit->Enabled = TweakFrmMainTitlebarCheckBox->Checked;
  if(TweakFrmMainTitlebarCheckBox->Checked) TweakFrmMainTitlebarMode2Edit->Enabled = TweakFrmMainTitlebarMode2RadioButton->Checked;
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

void __fastcall TSettingsForm::aClipTabsChkExecute(TObject *Sender)
{
  ExcludeClipTabsFromSwitchToNewMsgCheckBox->Enabled = ExcludeClipTabsFromTabSwitchingCheckBox->Checked;
  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSideSlideChkExecute(TObject *Sender)
{
  FrmSendEdgeGroupBox->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendEdgeLeftRadioButton->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendEdgeRightRadioButton->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendEdgeBottomRadioButton->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendEdgeTopRadioButton->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendHideGroupBox->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendHideFocusRadioButton->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendHideAppFocusRadioButton->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendHideCursorRadioButton->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendTimeGroupBox->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendSlideInDelaySpinEdit->Enabled = SlideFrmSendCheckBox->Checked;
  if(SlideFrmSendCheckBox->Checked)
   FrmSendSlideOutDelaySpinEdit->Enabled = FrmSendHideCursorRadioButton->Checked;
  else
   FrmSendSlideOutDelaySpinEdit->Enabled = false;
  FrmSendSlideInTimeSpinEdit->Enabled = SlideFrmSendCheckBox->Checked;
  FrmSendSlideOutTimeSpinEdit->Enabled = SlideFrmSendCheckBox->Checked;
  SlideInAtNewMsgCheckBox->Enabled = SlideFrmSendCheckBox->Checked;
  FrmMainEdgeGroupBox->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainEdgeLeftRadioButton->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainEdgeRightRadioButton->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainEdgeBottomRadioButton->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainEdgeTopRadioButton->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainHideGroupBox->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainHideFocusRadioButton->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainHideAppFocusRadioButton->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainHideCursorRadioButton->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainTimeGroupBox->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainSlideInDelaySpinEdit->Enabled = SlideFrmMainCheckBox->Checked;
  if(SlideFrmMainCheckBox->Checked)
   FrmMainSlideOutDelaySpinEdit->Enabled = FrmMainHideCursorRadioButton->Checked;
  else
   FrmMainSlideOutDelaySpinEdit->Enabled = false;
  FrmMainSlideInTimeSpinEdit->Enabled = SlideFrmMainCheckBox->Checked;
  FrmMainSlideOutTimeSpinEdit->Enabled = SlideFrmMainCheckBox->Checked;
  ChangeTabAfterSlideInCheckBox->Enabled = SlideFrmMainCheckBox->Checked;
  SideSlideFullScreenModeExceptionsButton->Enabled = SideSlideFullScreenModeCheckBox->Checked;

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aOtherChkExecute(TObject *Sender)
{
  CollapseImagesModeComboBox->Enabled = CollapseImagesCheckBox->Checked;
  MinimizeRestoreHotKey->Enabled = MinimizeRestoreCheckBox->Checked;
  SaveButton->Enabled = true;
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

void __fastcall TSettingsForm::SaveButtonClick(TObject *Sender)
{
  SaveButton->Enabled = false;
  CancelButton->Enabled = false;
  OkButton->Enabled = false;
  aSaveSettingsW->Execute();
  CancelButton->Enabled = true;
  OkButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OkButtonClick(TObject *Sender)
{
  SaveButton->Enabled = false;
  CancelButton->Enabled = false;
  OkButton->Enabled = false;
  aSaveSettingsW->Execute();
  CancelButton->Enabled = true;
  OkButton->Enabled = true;
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UnsentMsgTrayIconClick(TObject *Sender)
{
  if(ShowUnsentMsg()) UnsentMsgTrayIcon->Visible = false;
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

void __fastcall TSettingsForm::ClipTabsEraseButtonClick(TObject *Sender)
{
  EraseClipTabs();
  ClipTabsEraseButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::MiniAvatarsEraseButtonClick(TObject *Sender)
{
  FileListBox->Directory = GetPluginUserDirW() + "\\TabKit\\Avatars";
  FileListBox->Update();
  for(int Count=0;Count<FileListBox->Items->Count;Count++)
  {
	DeleteFile(FileListBox->Items->Strings[Count]);
  }
  EraseClipTabsIcons();
  MiniAvatarsEraseButton->Enabled = false;
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
  Grphx.DrawImage(Thumbnail, 0, 432,Thumbnail->GetWidth(), Thumbnail->GetHeight());
  CLSID gifClsid;
  GetEncoderClsid(L"image/png", &gifClsid);
  Thumbnail->Save(New.w_str(), &gifClsid, NULL);
  delete Thumbnail;
  //Zakoñczenie sesji z GDIPlus
  Gdiplus::GdiplusShutdown(gdiplusToken);
}
//-----------------------------------------------------------------

void __fastcall TSettingsForm::OtherTabSheetShow(TObject *Sender)
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

void __fastcall TSettingsForm::PayPalImageClick(TObject *Sender)
{
  ShellExecute(NULL, L"open", L"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=9QAXL9BG9XKRE", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::NewMsgTabSheetShow(TObject *Sender)
{
  if((InactiveFrmNewMsgCheckBox->Checked)||(TweakFrmSendTitlebarCheckBox->Checked))
   TaskbarPenCheckBox->Enabled = false;
  else
   TaskbarPenCheckBox->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::IdThreadComponentRun(TIdThreadComponent *Sender)
{
  //Odczyt pakietu XML
  XML = UTF8ToUnicodeString(XML.w_str());
  _di_IXMLDocument XMLDoc = LoadXMLData(XML);
  _di_IXMLNode Nodes = XMLDoc->DocumentElement;
  Nodes = Nodes->ChildNodes->GetNode(0);
  int ItemsCount = Nodes->ChildNodes->GetCount();
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Session.ini");
  for(int Count=0;Count<ItemsCount;Count++)
  {
	//Parsowanie XML
	_di_IXMLNode ChildNodes = Nodes->ChildNodes->GetNode(Count);
	UnicodeString JID = ChildNodes->Attributes["jid"];
	UnicodeString Channel = ChildNodes->Attributes["name"];
	Channel = Channel.Delete(Channel.LastDelimiter("("),Channel.Length());
	Channel = Channel.Trim();
	//Kodowanie HTML
	Channel = StringReplace(Channel, "&quot;", '"', TReplaceFlags() << rfReplaceAll);
	Channel = StringReplace(Channel, "&apos;", "'", TReplaceFlags() << rfReplaceAll);
	Channel = StringReplace(Channel, "&amp;", "&", TReplaceFlags() << rfReplaceAll);
	Channel = StringReplace(Channel, "&lt;", "<", TReplaceFlags() << rfReplaceAll);
	Channel = StringReplace(Channel, "&gt;", ">", TReplaceFlags() << rfReplaceAll);
	//Normalizacja nazw kanalow
	Channel = NormalizeChannel(Channel);
	//Zapisywanie nazwy kanalu
	Ini->WriteString("Channels",JID,Channel);
  }
  delete Ini;
  //Wylaczenie watku
  IdThreadComponent->Stop();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SideSlideFullScreenModeExceptionsButtonClick(TObject *Sender)
{
  SideSlideExceptionsForm = new TSideSlideExceptionsForm(Application);
  SideSlideExceptionsForm->SkinManagerEnabled = sSkinManager->Active;
  SideSlideExceptionsForm->ShowModal();
  delete SideSlideExceptionsForm;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define STRICT
#include <vcl.h>
#pragma hdrstop
#include "SettingsFrm.h"
#include <gdiplus.h>
#include <inifiles.hpp>
//#include <windows.h>
//#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//#pragma link "GdiPlus.lib"
#pragma link "IdBaseComponent"
#pragma link "IdCoder"
#pragma link "IdCoder3to4"
#pragma link "IdCoderMIME"
#pragma link "LMDPNGImage"
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
#pragma link "sSpinEdit"
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//using std::min;
//using std::max;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetPluginUserDirW();
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkNativeEnabled();
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
__declspec(dllimport)void CheckHideStatusBar();
__declspec(dllimport)void ShowToolBar();
__declspec(dllimport)void CheckHideTabListButton();
__declspec(dllimport)void CheckHideScrollTabButtons();
//---------------------------------------------------------------------------
bool pHideTabCloseButtonChk;
bool pRefreshTabs = false;
//---------------------------------------------------------------------------
__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TSettingsForm::WMHotKey(TMessage& Message)
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
  if(ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = true;
	  sSkinManager->Active = true;
	}
	else
	 sSkinManager->Active = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormShow(TObject *Sender)
{
  //Skorkowanie okna
  if(!ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = false;
	  sSkinManager->Active = true;
	}
	else
	 sSkinManager->Active = false;
  }
  if(sSkinManager->Active)
  {
	//Skorkowanie glownego komponentu
	CategoryPanelGroup->ChevronColor = sSkinManager->GetActiveEditFontColor();
	CategoryPanelGroup->ChevronHotColor = sSkinManager->GetHighLightFontColor();
	CategoryPanelGroup->Color = sSkinManager->GetActiveEditColor();
	CategoryPanelGroup->GradientBaseColor = sSkinManager->GetActiveEditColor();
	CategoryPanelGroup->GradientColor = sSkinManager->GetHighLightColor();
	CategoryPanelGroup->HeaderFont->Color = sSkinManager->GetActiveEditFontColor();
	ClipTabsCategoryPanel->Color = sSkinManager->GetGlobalColor();
	ClipTabsCategoryPanel->Font->Color = sSkinManager->GetGlobalFontColor();
	ClosedTabsCategoryPanel->Color = sSkinManager->GetGlobalColor();
	ClosedTabsCategoryPanel->Font->Color = sSkinManager->GetGlobalFontColor();
	NewMsgCategoryPanel->Color = sSkinManager->GetGlobalColor();
	NewMsgCategoryPanel->Font->Color = sSkinManager->GetGlobalFontColor();
	OtherCategoryPanel->Color = sSkinManager->GetGlobalColor();
	OtherCategoryPanel->Font->Color = sSkinManager->GetGlobalFontColor();
	SessionRememberCategoryPanel->Color = sSkinManager->GetGlobalColor();
	SessionRememberCategoryPanel->Font->Color = sSkinManager->GetGlobalFontColor();
	SideSlideCategoryPanel->Color = sSkinManager->GetGlobalColor();
	SideSlideCategoryPanel->Color = sSkinManager->GetGlobalColor();
	TabsSwitchingCategoryPanel->Color = sSkinManager->GetGlobalColor();
	TabsSwitchingCategoryPanel->Font->Color = sSkinManager->GetGlobalFontColor();
	TitlebarCategoryPanel->Color = sSkinManager->GetGlobalColor();
	TitlebarCategoryPanel->Font->Color = sSkinManager->GetGlobalFontColor();
	UnsentMsgCategoryPanel->Color = sSkinManager->GetGlobalColor();
	UnsentMsgCategoryPanel->Font->Color = sSkinManager->GetGlobalFontColor();
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
	OtherPaymentsWebLabel->Font->Color = sSkinManager->GetGlobalFontColor();
	OtherPaymentsWebLabel->HoverFont->Color = sSkinManager->GetGlobalFontColor();
  }
  else
  {
	//Skorkowanie glownego komponentu
	CategoryPanelGroup->ChevronColor = clBlack;
	CategoryPanelGroup->ChevronHotColor = clGray;
	CategoryPanelGroup->Color = clWindow;
	CategoryPanelGroup->GradientBaseColor = (TColor)0xF0F0F0;
	CategoryPanelGroup->GradientColor = clSilver;
	CategoryPanelGroup->HeaderFont->Color = clWindowText;
	ClipTabsCategoryPanel->Color = clWindow;
	ClipTabsCategoryPanel->Font->Color = clWindowText;
	ClosedTabsCategoryPanel->Color = clWindow;
	ClosedTabsCategoryPanel->Font->Color = clWindowText;
	NewMsgCategoryPanel->Color = clWindow;
	NewMsgCategoryPanel->Font->Color = clWindowText;
	OtherCategoryPanel->Color = clWindow;
	OtherCategoryPanel->Font->Color = clWindowText;
	SessionRememberCategoryPanel->Color = clWindow;
	SessionRememberCategoryPanel->Font->Color = clWindowText;
	SideSlideCategoryPanel->Color = clWindow;
	SideSlideCategoryPanel->Color = clWindow;
	TabsSwitchingCategoryPanel->Color = clWindow;
	TabsSwitchingCategoryPanel->Font->Color = clWindowText;
	TitlebarCategoryPanel->Color = clWindow;
	TitlebarCategoryPanel->Font->Color = clWindowText;
	UnsentMsgCategoryPanel->Color = clWindow;
	UnsentMsgCategoryPanel->Font->Color = clWindowText;
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
	OtherPaymentsWebLabel->Font->Color = clWindowText;
	OtherPaymentsWebLabel->HoverFont->Color = clWindowText;
  }
  //All CategoryPanel
  ClosedTabsCategoryPanel->TabOrder = 0;
  UnsentMsgCategoryPanel->TabOrder = 1;
  TabsSwitchingCategoryPanel->TabOrder = 2;
  SessionRememberCategoryPanel->TabOrder = 3;
  NewMsgCategoryPanel->TabOrder = 4;
  TitlebarCategoryPanel->TabOrder = 5;
  ClipTabsCategoryPanel->TabOrder = 6;
  SideSlideCategoryPanel->TabOrder = 7;
  OtherCategoryPanel->TabOrder = 8;
  //Odczyt ustawien
  aLoadSettings->Execute();
  //Status odswiezania  
  pRefreshTabs = true;
  //Odwiezenie panelu z przyciskami
  RefreshTimer->Enabled = true;
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
  FrmSendClosedTabsCheckBox->Checked =  Ini->ReadBool("ClosedTabs","FrmSend",false);
  ItemsCountClosedTabsSpinEdit->Value = Ini->ReadInteger("ClosedTabs","ItemsCount",5);
  ShowTimeClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","ClosedTime",false);
  FastClearClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","FastClear",false);
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
  FrmSendUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FrmSend",false);
  FastClearUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FastClear",false);
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
  switch(Ini->ReadInteger("SideSlide","FrmMainHideMode",1))
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
  QuickQuoteCheckBox->Checked = Ini->ReadBool("Other","QuickQuote",false);
  CollapseImagesCheckBox->Checked = Ini->ReadBool("Other","CollapseImages",false);
  CollapseImagesModeComboBox->ItemIndex = Ini->ReadInteger("Other","CollapseImagesMode",1)-1;
  AntiSpimCheckBox->Checked = !Ini->ReadBool("Other","AntiSpim",true);
  MinimizeRestoreCheckBox->Checked = Ini->ReadBool("Other","MinimizeRestore",false);
  MinimizeRestoreHotKey->HotKey = Ini->ReadInteger("Other","MinimizeRestoreHotKey",24689);
  StayOnTopCheckBox->Checked = Ini->ReadBool("Other","StayOnTop",false);
  HideStatusBarCheckBox->Checked = Ini->ReadBool("Other","HideStatusBar",false);
  HideToolBarCheckBox->Checked = Ini->ReadBool("Other","HideToolBar",false);
  HideTabCloseButtonCheckBox->Checked = Ini->ReadBool("Other","HideTabCloseButton",false);
  pHideTabCloseButtonChk = HideTabCloseButtonCheckBox->Checked;
  HideTabListButtonCheckBox->Checked = Ini->ReadBool("Other"," HideTabListButton",false);
  HideScrollTabButtonsCheckBox->Checked = Ini->ReadBool("Other","HideScrollTabButtons",false);
  CloseBy2xLPMCheckBox->Checked = Ini->ReadBool("Other","CloseBy2xLPM",false);
  EmuTabsWCheckBox->Checked = Ini->ReadBool("Other","EmuTabsW",false);
  CloudTimeOutSpinEdit->Value = Ini->ReadInteger("Other","CloudTimeOut",6);
  CloudTickModeComboBox->ItemIndex = Ini->ReadBool("Other","CloudTickMode",true);
  SearchOnListCheckBox->Checked = !Ini->ReadBool("Other","SearchOnList",true);
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
  //aClipTabsChk->Execute();
  aSideSlideChk->Execute();
  //aOtherChk->Execute();
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
  Ini->WriteBool("NewMsg","TaskbarPen",!TaskbarPenCheckBox->Checked);
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
  Ini->WriteBool("Other","HideStatusBar",HideStatusBarCheckBox->Checked);
  Ini->WriteBool("Other","HideToolBar",HideToolBarCheckBox->Checked);
  Ini->WriteBool("Other","HideTabCloseButton",HideTabCloseButtonCheckBox->Checked);
  Ini->WriteBool("Other","HideTabListButton",HideTabListButtonCheckBox->Checked);
  Ini->WriteBool("Other","HideScrollTabButtons",HideScrollTabButtonsCheckBox->Checked);
  Ini->WriteBool("Other","CloseBy2xLPM",CloseBy2xLPMCheckBox->Checked);
  Ini->WriteBool("Other","EmuTabsW",EmuTabsWCheckBox->Checked);
  Ini->WriteInteger("Other","CloudTimeOut",CloudTimeOutSpinEdit->Value);
  Ini->WriteBool("Other","CloudTickMode",CloudTickModeComboBox->ItemIndex);
  Ini->WriteBool("Other","SearchOnList",!SearchOnListCheckBox->Checked);

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
  CheckHideStatusBar();
  ShowToolBar();
  CheckHideTabListButton();
  CheckHideScrollTabButtons();
  if(pHideTabCloseButtonChk!=HideTabCloseButtonCheckBox->Checked) RefreshTabs();
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

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aTitlebarTweakChkExecute(TObject *Sender)
{
  TweakFrmSendTitlebarMode1RadioButton->Enabled = TweakFrmSendTitlebarCheckBox->Checked;
  TweakFrmSendTitlebarMode2RadioButton->Enabled = TweakFrmSendTitlebarCheckBox->Checked;
  TweakFrmSendTitlebarMode3RadioButton->Enabled = TweakFrmSendTitlebarCheckBox->Checked;
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

  SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aOtherChkExecute(TObject *Sender)
{
  CollapseImagesModeComboBox->Enabled = CollapseImagesCheckBox->Checked;
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
  Grphx.DrawImage(Thumbnail, 0, 0,Thumbnail->GetWidth(), Thumbnail->GetHeight());
  CLSID gifClsid;
  GetEncoderClsid(L"image/png", &gifClsid);
  Thumbnail->Save(New.w_str(), &gifClsid, NULL);
  delete Thumbnail;
  //Zakoñczenie sesji z GDIPlus
  Gdiplus::GdiplusShutdown(gdiplusToken);
}
//-----------------------------------------------------------------

void __fastcall TSettingsForm::ClipTabsCategoryPanelCollapse(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Status odswiezania
	pRefreshTabs = true;
	//Zwiniecie paneli
	CategoryPanelGroup->ExpandAll();
	//Rozwiniecie danego panelu
	ClipTabsCategoryPanel->Collapsed = true;
	//Zmiana aktywnej zakladki
	sPageControl->ActivePage = ClipTabsTabSheet;
	//Ustawienie fokusu na zakladce
	ClipTabsTabSheet->SetFocus();
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Status odswiezania
	pRefreshTabs = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::ClipTabsCategoryPanelExpand(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Ustawienie domyslnej zakladki
	sPageControl->ActivePage = DefaultTabSheet;
	//Ustawienie fokusu na kontrolce
	if(SaveButton->Enabled)	SaveButton->SetFocus();
	else CancelButton->SetFocus();
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::ClosedTabsCategoryPanelCollapse(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Status odswiezania
	pRefreshTabs = true;
	//Zwiniecie paneli
	CategoryPanelGroup->ExpandAll();
	//Rozwiniecie danego panelu
	ClosedTabsCategoryPanel->Collapsed = true;
	//Zmiana aktywnej zakladki
	sPageControl->ActivePage = ClosedTabsTabSheet;
	//Ustawienie fokusu na zakladce
	ClosedTabsTabSheet->SetFocus();
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Status odswiezania
	pRefreshTabs = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::ClosedTabsCategoryPanelExpand(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Ustawienie domyslnej zakladki
	sPageControl->ActivePage = DefaultTabSheet;
	//Ustawienie fokusu na kontrolce
	if(SaveButton->Enabled)	SaveButton->SetFocus();
	else CancelButton->SetFocus();
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::NewMsgCategoryPanelCollapse(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Status odswiezania
	pRefreshTabs = true;
	//Zwiniecie paneli
	CategoryPanelGroup->ExpandAll();
	//Rozwiniecie danego panelu
	NewMsgCategoryPanel->Collapsed = true;
	//Zmiana aktywnej zakladki
	sPageControl->ActivePage = NewMsgTabSheet;
	//Ustawienie fokusu na zakladce
	NewMsgTabSheet->SetFocus();
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Status odswiezania
	pRefreshTabs = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::NewMsgCategoryPanelExpand(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Ustawienie domyslnej zakladki
	sPageControl->ActivePage = DefaultTabSheet;
	//Ustawienie fokusu na kontrolce
	if(SaveButton->Enabled)	SaveButton->SetFocus();
	else CancelButton->SetFocus();
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OtherCategoryPanelCollapse(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Status odswiezania
	pRefreshTabs = true;
	//Zwiniecie paneli
	CategoryPanelGroup->ExpandAll();
	//Rozwiniecie danego panelu
	OtherCategoryPanel->Collapsed = true;
	//Zmiana aktywnej zakladki
	sPageControl->ActivePage = OtherTabSheet;
	//Ustawienie fokusu na zakladce
	OtherTabSheet->SetFocus();
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Status odswiezania
	pRefreshTabs = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OtherCategoryPanelExpand(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Ustawienie domyslnej zakladki
	sPageControl->ActivePage = DefaultTabSheet;
	//Ustawienie fokusu na kontrolce
	if(SaveButton->Enabled)	SaveButton->SetFocus();
	else CancelButton->SetFocus();
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SessionRememberCategoryPanelCollapse(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Status odswiezania
	pRefreshTabs = true;
	//Zwiniecie paneli
	CategoryPanelGroup->ExpandAll();
	//Rozwiniecie danego panelu
	SessionRememberCategoryPanel->Collapsed = true;
	//Zmiana aktywnej zakladki
	sPageControl->ActivePage = SessionRememberTabSheet;
	//Ustawienie fokusu na zakladce
	SessionRememberTabSheet->SetFocus();
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Status odswiezania
	pRefreshTabs = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SessionRememberCategoryPanelExpand(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Ustawienie domyslnej zakladki
	sPageControl->ActivePage = DefaultTabSheet;
	//Ustawienie fokusu na kontrolce
	if(SaveButton->Enabled)	SaveButton->SetFocus();
	else CancelButton->SetFocus();
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SideSlideCategoryPanelCollapse(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Status odswiezania
	pRefreshTabs = true;
	//Zwiniecie paneli
	CategoryPanelGroup->ExpandAll();
	//Rozwiniecie danego panelu
	SideSlideCategoryPanel->Collapsed = true;
	//Zmiana aktywnej zakladki
	sPageControl->ActivePage = SideSlideTabSheet;
	//Ustawienie fokusu na zakladce
	SideSlideTabSheet->SetFocus();
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Status odswiezania
	pRefreshTabs = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SideSlideCategoryPanelExpand(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Ustawienie domyslnej zakladki
	sPageControl->ActivePage = DefaultTabSheet;
	//Ustawienie fokusu na kontrolce
	if(SaveButton->Enabled)	SaveButton->SetFocus();
	else CancelButton->SetFocus();
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TabsSwitchingCategoryPanelCollapse(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Status odswiezania
	pRefreshTabs = true;
	//Zwiniecie paneli
	CategoryPanelGroup->ExpandAll();
	//Rozwiniecie danego panelu
	TabsSwitchingCategoryPanel->Collapsed = true;
	//Zmiana aktywnej zakladki
	sPageControl->ActivePage = TabsSwitchingTabSheet;
	//Ustawienie fokusu na zakladce
	TabsSwitchingTabSheet->SetFocus();
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Status odswiezania
	pRefreshTabs = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TabsSwitchingCategoryPanelExpand(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Ustawienie domyslnej zakladki
	sPageControl->ActivePage = DefaultTabSheet;
	//Ustawienie fokusu na kontrolce
	if(SaveButton->Enabled)	SaveButton->SetFocus();
	else CancelButton->SetFocus();
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TitlebarCategoryPanelCollapse(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Status odswiezania
	pRefreshTabs = true;
	//Zwiniecie paneli
	CategoryPanelGroup->ExpandAll();
	//Rozwiniecie danego panelu
	TitlebarCategoryPanel->Collapsed = true;
	//Zmiana aktywnej zakladki
	sPageControl->ActivePage = TitlebarTabSheet;
	//Ustawienie fokusu na zakladce
	TitlebarTabSheet->SetFocus();
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Status odswiezania
	pRefreshTabs = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TitlebarCategoryPanelExpand(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Ustawienie domyslnej zakladki
	sPageControl->ActivePage = DefaultTabSheet;
	//Ustawienie fokusu na kontrolce
	if(SaveButton->Enabled)	SaveButton->SetFocus();
	else CancelButton->SetFocus();
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UnsentMsgCategoryPanelCollapse(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Status odswiezania
	pRefreshTabs = true;
	//Zwiniecie paneli
	CategoryPanelGroup->ExpandAll();
	//Rozwiniecie danego panelu
	UnsentMsgCategoryPanel->Collapsed = true;
	//Zmiana aktywnej zakladki
	sPageControl->ActivePage = UnsentMsgTabSheet;
	//Ustawienie fokusu na zakladce
	UnsentMsgTabSheet->SetFocus();
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Status odswiezania
	pRefreshTabs = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::UnsentMsgCategoryPanelExpand(TObject *Sender)
{
  if(!pRefreshTabs)
  {
	//Odwiezenie panelu z przyciskami
	aRefreshPanels->Execute();
	//Ustawienie domyslnej zakladki
	sPageControl->ActivePage = DefaultTabSheet;
	//Ustawienie fokusu na kontrolce
	if(SaveButton->Enabled)	SaveButton->SetFocus();
	else CancelButton->SetFocus();
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aRefreshPanelsExecute(TObject *Sender)
{
  CategoryPanelGroup->Visible = false;
  CategoryPanelGroup->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::RefreshTimerTimer(TObject *Sender)
{
  //Wylaczenie timera
  RefreshTimer->Enabled = false;
  //Zwiniecie paneli
  CategoryPanelGroup->ExpandAll();
  //Odwiezenie panelu z przyciskami
  aRefreshPanels->Execute();
  //Status odswiezania
  pRefreshTabs = false;
}
//---------------------------------------------------------------------------

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
  ShellExecute(NULL, "open", "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=9QAXL9BG9XKRE", NULL, NULL, SW_SHOWNORMAL);
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

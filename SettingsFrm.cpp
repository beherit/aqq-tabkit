//---------------------------------------------------------------------------
// Copyright (C) 2010-2015 Krzysztof Grochocki
//
// This file is part of TabKit
//
// TabKit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// TabKit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio. If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <vcl.h>
#include <inifiles.hpp>
#include <XMLDoc.hpp>
#include <gdiplus.h>
#include <LangAPI.hpp>
#pragma hdrstop
#include "SettingsFrm.h"
#include "SideSlideExceptionsFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "acAlphaImageList"
#pragma link "acPNG"
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sCheckBox"
#pragma link "sComboBox"
#pragma link "sEdit"
#pragma link "sGroupBox"
#pragma link "sLabel"
#pragma link "sListView"
#pragma link "sMemo"
#pragma link "sPageControl"
#pragma link "sRadioButton"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma link "sSpeedButton"
#pragma link "sSpinEdit"
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetPluginUserDirW();
__declspec(dllimport)UnicodeString GetUserDir();
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)UnicodeString GetIconPath(int Icon);
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkThemeAnimateWindows();
__declspec(dllimport)bool ChkThemeGlowing();
__declspec(dllimport)int GetHUE();
__declspec(dllimport)int GetSaturation();
__declspec(dllimport)int GetBrightness();
__declspec(dllimport)void ChangeFrmSendTitlebar();
__declspec(dllimport)void ChangeFrmMainTitlebar();
__declspec(dllimport)void MinimizeRestoreFrmSendExecute();
__declspec(dllimport)void MinimizeRestoreFrmMainExecute();
__declspec(dllimport)void ShowToolBar();
__declspec(dllimport)void CheckHideScrollTabButtons();
__declspec(dllimport)UnicodeString ConvertToInt(UnicodeString Text);
__declspec(dllimport)UnicodeString EncodeBase64(UnicodeString Str);
//__declspec(dllimport)UnicodeString DecodeBase64(UnicodeString Str);
__declspec(dllimport)UnicodeString GetContactNick(UnicodeString JID);
__declspec(dllimport)UnicodeString FriendlyFormatJID(UnicodeString JID);
__declspec(dllimport)bool ChkYouTubeListItem();
__declspec(dllimport)UnicodeString GetYouTubeTitleListItem();
__declspec(dllimport)void AddToYouTubeExcludeList(UnicodeString ID);
__declspec(dllimport)bool RefreshListAllowed();
__declspec(dllimport)void RefreshList();
__declspec(dllimport)void GetChannelNameW(UnicodeString JID);
__declspec(dllimport)void DestroyFrmClosedTabs();
__declspec(dllimport)void BuildFrmClosedTabs(bool FixPosition);
__declspec(dllimport)void EraseClosedTabs();
__declspec(dllimport)void DestroyFrmUnsentMsg();
__declspec(dllimport)void BuildFrmUnsentMsg(bool FixPosition);
__declspec(dllimport)void EraseUnsentMsg();
__declspec(dllimport)bool ShowUnsentMsg();
__declspec(dllimport)void DestroyClipTab();
__declspec(dllimport)void BuildClipTab();
__declspec(dllimport)void EraseClipTabs();
__declspec(dllimport)void EraseClipTabsIcons();
__declspec(dllimport)void ShowFavouritesTabsInfo(UnicodeString Text);
__declspec(dllimport)void LoadFavouritesTabs();
__declspec(dllimport)void DestroyFrmSendFavouriteTab();
__declspec(dllimport)void DestroyFrmMainFavouriteTab();
__declspec(dllimport)void DestroyFavouritesTabs();
__declspec(dllimport)void BuildFrmSendFavouriteTab();
__declspec(dllimport)void BuildFrmMainFavouriteTab();
__declspec(dllimport)void BuildFavouritesTabs(bool FixPosition);
__declspec(dllimport)void DestroyStayOnTop();
__declspec(dllimport)void BuildStayOnTop();
__declspec(dllimport)void HookGlobalKeyboard();
__declspec(dllimport)void RefreshTabs();
__declspec(dllimport)int GetMinimizeRestoreFrmSendKey();
__declspec(dllimport)void LoadSettings();
//---------------------------------------------------------------------------

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
	//FrmMainSideSlide + otwieranie nowych wiadomosci
	if(Message.WParam==0x0200)
	{
		if(!MinimizeRestoreHotKey->Focused())
			MinimizeRestoreFrmMainExecute();
		else
			MinimizeRestoreHotKey->HotKey = GetMinimizeRestoreFrmSendKey();
	}
}
//---------------------------------------------------------------------------

//Pobieranie danych z danego URL
UnicodeString __fastcall TSettingsForm::IdHTTPGet(UnicodeString URL)
{
	//Zmienna z danymi
	UnicodeString ResponseText;
	//Proba pobrania danych
	try
	{
		//Wywolanie polaczenia
		ResponseText = IdHTTP->Get(URL);
	}
	//Blad
	catch(const Exception& e)
	{
		//Hack na wywalanie sie IdHTTP
		if(e.Message=="Connection Closed Gracefully.")
		{
			//Hack
			IdHTTP->CheckForGracefulDisconnect(false);
			//Rozlaczenie polaczenia
			IdHTTP->Disconnect();
		}
		//Inne bledy
		else
			//Rozlaczenie polaczenia
			IdHTTP->Disconnect();
		//Zwrot pustych danych
		return "";
	}
	//Pobranie kodu odpowiedzi
	int Response = IdHTTP->ResponseCode;
	//Wszystko ok
	if(Response==200)
		return ResponseText;
	//Inne bledy
	else
		return "";
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormCreate(TObject *Sender)
{
	//Lokalizowanie formy
	LangForm(this);
	//Poprawka pozycji komponentow
	ItemsCountClosedTabsSpinEdit->Left = Canvas->TextWidth(ItemsCountClosedTabsSpinEdit->BoundLabel->Caption) + 20;
	UnCloseTabHotKeyInput->Left = UnCloseTabHotKeyMode2RadioButton->Left + Canvas->TextWidth(UnCloseTabHotKeyMode2RadioButton->Caption) + 26;
	CountClosedTabsSpinEdit->Left = Canvas->TextWidth(CountClosedTabsSpinEdit->BoundLabel->Caption) + 20;
	ChatGoneSaveInArchiveCheckBox->Left = ChatGoneFrmSendNotiferNewMsgCheckBox->Left + Canvas->TextWidth(ChatGoneFrmSendNotiferNewMsgCheckBox->Caption) + 26;
	FrmMainEdgeComboBox->Left = FrmMainEdgeLabel->Left + Canvas->TextWidth(FrmMainEdgeLabel->Caption) + 6;
	FrmSendEdgeComboBox->Left = FrmSendEdgeLabel->Left + Canvas->TextWidth(FrmSendEdgeLabel->Caption) + 6;
	SideSlideFullScreenModeExceptionsButton->Left = SideSlideFullScreenModeCheckBox->Left + Canvas->TextWidth(SideSlideFullScreenModeCheckBox->Caption) + 26;
	CollapseImagesModeComboBox->Left = CollapseImagesCheckBox->Left + Canvas->TextWidth(CollapseImagesCheckBox->Caption) + 26;
	ShortenLinksModeComboBox->Left = ShortenLinksCheckBox->Left + Canvas->TextWidth(ShortenLinksCheckBox->Caption) + 26;
	CloudTimeOutSpinEdit->Left = ((CloudSettingsGroupBox->Width - CloudTimeOutSpinEdit->Width - Canvas->TextWidth(CloudTimeOutSpinEdit->BoundLabel->Caption) - 6)/2) + Canvas->TextWidth(CloudTimeOutSpinEdit->BoundLabel->Caption) + 6;
	CloudTickModeLabel->Left = ((CloudSettingsGroupBox->Width - CloudTickModeComboBox->Width - Canvas->TextWidth(CloudTickModeLabel->Caption) - 6)/2);
	CloudTickModeComboBox->Left = CloudTickModeLabel->Left + Canvas->TextWidth(CloudTickModeLabel->Caption) + 6;
	//Wlaczona zaawansowana stylizacja okien
	if(ChkSkinEnabled())
	{
		UnicodeString ThemeSkinDir = GetThemeSkinDir();
		//Plik zaawansowanej stylizacji okien istnieje
		if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
		{
			//Dane pliku zaawansowanej stylizacji okien
			ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
			sSkinManager->SkinDirectory = ThemeSkinDir;
			sSkinManager->SkinName = "Skin.asz";
			//Ustawianie animacji AlphaControls
			if(ChkThemeAnimateWindows()) sSkinManager->AnimEffects->FormShow->Time = 200;
			else sSkinManager->AnimEffects->FormShow->Time = 0;
			sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
			//Zmiana kolorystyki AlphaControls
			sSkinManager->HueOffset = GetHUE();
			sSkinManager->Saturation = GetSaturation();
			sSkinManager->Brightness = GetBrightness();
			//Aktywacja skorkowania AlphaControls
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
	//Odczyt ikonek
	FavouritesTabsAlphaImageList->Items->Clear();
	FavouritesTabsAlphaImageList->LoadFromFile(GetIconPath(98));
	FavouritesTabsAlphaImageList->LoadFromFile(GetIconPath(99));
	FavouritesTabsAlphaImageList->LoadFromFile(GetIconPath(15));
	FavouritesTabsAlphaImageList->LoadFromFile(GetIconPath(79));
	//Odczyt ustawien
	aLoadSettings->Execute();
	//Ustawienie hintu w kontrolce formatu daty
	TDateTime DateTime = TDateTime::CurrentDateTime();
	DateFormatEdit->Hint = DateTime.FormatString(DateFormatEdit->Text);
	//Ustawienie domyslnej zakladki
	sPageControl->ActivePage = ClosedTabsTabSheet;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aLoadSettingsExecute(TObject *Sender)
{
	//Odczyt ustawien
	TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Settings.ini");
	//ClosedTabs
	RememberClosedTabsGroupBox->Checked = Ini->ReadBool("ClosedTabs","Enable",true);
	FastAccessClosedTabsGroupBox->Checked =	Ini->ReadBool("ClosedTabs","FastAccess",true);
	FrmMainClosedTabsCheckBox->Checked =	Ini->ReadBool("ClosedTabs","FrmMain",true);
	FrmSendClosedTabsCheckBox->Checked =	Ini->ReadBool("ClosedTabs","FrmSend",true);
	ItemsCountClosedTabsSpinEdit->Value = Ini->ReadInteger("ClosedTabs","ItemsCount",5);
	ShowTimeClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","ClosedTime",false);
	DateFormatEdit->Text = Ini->ReadString("ClosedTabs", "DateFormat", "dddd, h:nn");
	FastClearClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","FastClear",true);
	UnCloseTabHotKeyGroupBox->Checked =	Ini->ReadBool("ClosedTabs","HotKey",false);
	if(Ini->ReadInteger("ClosedTabs","HotKeyMode",1)==1)
		UnCloseTabHotKeyMode1RadioButton->Checked = true;
	else
		UnCloseTabHotKeyMode2RadioButton->Checked = true;
	UnCloseTabHotKeyInput->HotKey = Ini->ReadInteger("ClosedTabs","HotKeyDef",0);
	UnCloseTabSPMouseCheckBox->Checked = Ini->ReadBool("ClosedTabs","SPMouse",true);
	UnCloseTabLPMouseCheckBox->Checked = Ini->ReadBool("ClosedTabs","LPMouse",false);
	UnCloseTab2xLPMouseCheckBox->Checked = Ini->ReadBool("ClosedTabs","2xLPMouse",false);
	CountClosedTabsSpinEdit->Value = Ini->ReadInteger("ClosedTabs","Count",5);
	RestoreLastMsgClosedTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","RestoreLastMsg",true);
	OnlyConversationTabsCheckBox->Checked = Ini->ReadBool("ClosedTabs","OnlyConversationTabs",false);
	SaveClosedInfoInArchiveCheckBox->Checked = Ini->ReadBool("ClosedTabs","SaveInfoInArchive",false);
	//UnsentMsg
	RememberUnsentMsgGroupBox->Checked = Ini->ReadBool("UnsentMsg","Enable",true);
	InfoUnsentMsgGroupBox->Checked = Ini->ReadBool("UnsentMsg","Info",true);
	CloudUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Cloud",true);
	DetailedCloudUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","DetailedCloud",false);
	TrayUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","Tray",true);
	FastAccessUnsentMsgGroupBox->Checked = Ini->ReadBool("UnsentMsg","FastAccess",true);
	FrmMainUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FrmMain",true);
	FrmSendUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FrmSend",true);
	FastClearUnsentMsgCheckBox->Checked = Ini->ReadBool("UnsentMsg","FastClear",true);
	//TabsSwitching
	SwitchToNewMsgGroupBox->Checked = Ini->ReadBool("TabsSwitching","SwitchToNewMsg",true);
	if(Ini->ReadInteger("TabsSwitching","SwitchToNewMsgMode",1)==1)
		SwitchToNewMsgMode1RadioButton->Checked = true;
	else
		SwitchToNewMsgMode2RadioButton->Checked = true;
	TabsHotKeysGroupBox->Checked = Ini->ReadBool("TabsSwitching","TabsHotKeys",true);
	if(Ini->ReadInteger("TabsSwitching","TabsHotKeysMode",2)==1)
		TabsHotKeysMode1RadioButton->Checked = true;
	else
		TabsHotKeysMode2RadioButton->Checked = true;
	NewMgsHoyKeyCheckBox->Checked = Ini->ReadBool("TabsSwitching","NewMgsHoyKey",true);
	//SessionRemember
	RestoreTabsSessionGroupBox->Checked = Ini->ReadBool("SessionRemember","RestoreTabs",true);
	ManualRestoreTabsSessionCheckBox->Checked = Ini->ReadBool("SessionRemember","ManualRestoreTabs",true);
	RestoreMsgSessionCheckBox->Checked = Ini->ReadBool("SessionRemember","RestoreMsg",false);
	//NewMsg
	InactiveFrmNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","InactiveFrm",true);
	KeyboardFlasherCheckBox->Checked = Ini->ReadBool("NewMsg","KeyboardFlasher",false);
	KeyboardFlasherModeComboBox->ItemIndex = Ini->ReadInteger("NewMsg","KeyboardFlasherMode",0);
	OffCoreInactiveTabsNewMsgCheckBox->Checked = !Ini->ReadBool("NewMsg","CoreInactiveTabs",true);
	InactiveTabsNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","InactiveTabs",false);
	InactiveNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","InactiveNotifer",false);
	ChatStateNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","ChatStateNotifer",true);
	ChatGoneNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","ChatGoneNotifer",true);
	ChatGoneCloudNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","ChatGoneCloudNotifer",false);
	ChatGoneSoundNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","ChatGoneSoundNotifer",false);
	ChatGoneFrmSendNotiferNewMsgCheckBox->Checked = Ini->ReadBool("NewMsg","ChatGoneFrmSendNotifer",true);
	ChatGoneSaveInArchiveCheckBox->Checked = Ini->ReadBool("NewMsg","ChatGoneSaveInArchive",false);
	TaskbarPenCheckBox->Checked = !Ini->ReadBool("NewMsg","TaskbarPen",true);
	//Titlebar
	TweakFrmSendTitlebarGroupBox->Checked = Ini->ReadBool("Titlebar","TweakSend",false);
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
	TweakFrmMainTitlebarGroupBox->Checked = Ini->ReadBool("Titlebar","TweakMain",false);
	if(Ini->ReadInteger("Titlebar","MainMode",1)==1)
		TweakFrmMainTitlebarMode1RadioButton->Checked = true;
	else
		TweakFrmMainTitlebarMode2RadioButton->Checked = true;
	TweakFrmMainTitlebarModeExComboBox->ItemIndex = Ini->ReadInteger("Titlebar","MainModeEx",0);
	TweakFrmMainTitlebarMode2Edit->Text = Ini->ReadString("Titlebar","MainText","");
	//ClipTabs
	ClipTabsGroupBox->Checked = Ini->ReadBool("ClipTabs","Enabled",true);
	OpenClipTabsCheckBox->Checked = Ini->ReadBool("ClipTabs","OpenClipTabs",true);
	InactiveClipTabsCheckBox->Checked = Ini->ReadBool("ClipTabs","InactiveClipTabs",false);
	CounterClipTabsCheckBox->Checked = Ini->ReadBool("ClipTabs","Counter",false);
	ExcludeClipTabsFromTabSwitchingCheckBox->Checked = Ini->ReadBool("ClipTabs","ExcludeFromTabSwitching",true);
	ExcludeClipTabsFromSwitchToNewMsgCheckBox->Checked = !Ini->ReadBool("ClipTabs","ExcludeFromSwitchToNewMsg",false);
	ExcludeClipTabsFromTabsHotKeysCheckBox->Checked = Ini->ReadBool("ClipTabs","ExcludeFromTabsHotKeys",true);
	NoMiniAvatarsClipTabsCheckBox->Checked = !Ini->ReadBool("ClipTabs","MiniAvatars",true);
	//FavouritesTabs
	FavouritesTabsGroupBox->Checked = Ini->ReadBool("FavouritesTabs","Enabled",true);
	FastAccessFavouritesTabsGroupBox->Checked = Ini->ReadBool("FavouritesTabs","FastAccess",true);
	FrmMainFastAccessFavouritesTabsCheckBox->Checked = Ini->ReadBool("FavouritesTabs","FrmMainFastAccess",false);
	FrmSendFastAccessFavouritesTabsCheckBox->Checked = Ini->ReadBool("FavouritesTabs","FrmSendFastAccess",true);
	FavouritesTabsHotKeysCheckBox->Checked = Ini->ReadBool("FavouritesTabs","HotKeys",false);
	FavouritesTabsListView->Clear();
	TIniFile *SessionIni = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Session.ini");
	TStringList *FavouritesTabs = new TStringList;
	SessionIni->ReadSection("FavouritesTabs",FavouritesTabs);
	int TabsCount = FavouritesTabs->Count;
	delete FavouritesTabs;
	if(TabsCount>0)
	{
		for(int Count=0;Count<TabsCount;Count++)
		{
			UnicodeString FavouriteTab = SessionIni->ReadString("FavouritesTabs", "Tab"+IntToStr(Count+1),"");
			if(!FavouriteTab.IsEmpty())
			{
				FavouritesTabsListView->Items->Add();
				FavouritesTabsListView->Items->Item[FavouritesTabsListView->Items->Count-1]->Caption = GetContactNick(FavouriteTab) + " (" + FriendlyFormatJID(FavouriteTab) + ")";
				FavouritesTabsListView->Items->Item[FavouritesTabsListView->Items->Count-1]->SubItems->Add(FavouriteTab);
			}
		}
	}
	delete SessionIni;
	//SideSlide
	SlideFrmMainGroupBox->Checked = Ini->ReadBool("SideSlide","SlideFrmMain",false);
	FrmMainEdgeComboBox->ItemIndex = Ini->ReadInteger("SideSlide","FrmMainEdge",2) - 1;
	FrmMainSlideInDelaySpinEdit->Value = Ini->ReadInteger("SideSlide","FrmMainSlideInDelay",1000);
	FrmMainSlideOutDelaySpinEdit->Value = Ini->ReadInteger("SideSlide","FrmMainSlideOutDelay",1);
	FrmMainSlideInTimeSpinEdit->Value = Ini->ReadInteger("SideSlide","FrmMainSlideInTime",300);
	FrmMainSlideOutTimeSpinEdit->Value = Ini->ReadInteger("SideSlide","FrmMainSlideOutTime",500);
	ChangeTabAfterSlideInCheckBox->Checked = Ini->ReadBool("SideSlide","ChangeTabAfterSlideIn",true);
	SlideFrmSendGroupBox->Checked = Ini->ReadBool("SideSlide","SlideFrmSend",false);
	FrmSendEdgeComboBox->ItemIndex = Ini->ReadInteger("SideSlide","FrmSendEdge",1) - 1;
	FrmSendSlideInDelaySpinEdit->Value = Ini->ReadInteger("SideSlide","FrmSendSlideInDelay",1000);
	FrmSendSlideOutDelaySpinEdit->Value = Ini->ReadInteger("SideSlide","FrmSendSlideOutDelay",1);
	FrmSendSlideInTimeSpinEdit->Value = Ini->ReadInteger("SideSlide","FrmSendSlideInTime",300);
	FrmSendSlideOutTimeSpinEdit->Value = Ini->ReadInteger("SideSlide","FrmSendSlideOutTime",500);
	SideSlideFullScreenModeCheckBox->Checked = Ini->ReadBool("SideSlide","FullScreenMode",true);
	SideSlideCtrlAndMousBlockCheckBox->Checked = Ini->ReadBool("SideSlide","CtrlAndMouseBlock",true);
	//Other
	QuickQuoteCheckBox->Checked = Ini->ReadBool("Other","QuickQuote",true);
	CollapseImagesCheckBox->Checked = Ini->ReadBool("Other","CollapseImages",true);
	CollapseImagesModeComboBox->ItemIndex = Ini->ReadInteger("Other","CollapseImagesMode",1)-1;
	AntiSpimCheckBox->Checked = !Ini->ReadBool("Other","AntiSpim",false);
	MinimizeRestoreCheckBox->Checked = Ini->ReadBool("Other","MinimizeRestore",false);
	MinimizeRestoreHotKey->HotKey = Ini->ReadInteger("Other","MinimizeRestoreHotKey",24689);
	StayOnTopCheckBox->Checked = Ini->ReadBool("Other","StayOnTop",true);
	HideToolBarCheckBox->Checked = Ini->ReadBool("Other","HideToolBar",false);
	HideTabCloseButtonCheckBox->Checked = Ini->ReadBool("Other","HideTabCloseButton",true);
	HideScrollTabButtonsCheckBox->Checked = Ini->ReadBool("Other","HideScrollTabButtons",false);
	CloseBy2xLPMCheckBox->Checked = Ini->ReadBool("Other","CloseBy2xLPM",false);
	CloudTimeOutSpinEdit->Value = Ini->ReadInteger("Other","CloudTimeOut",6);
	CloudTickModeComboBox->ItemIndex = Ini->ReadBool("Other","CloudTickMode",true);
	SearchOnListCheckBox->Checked = !Ini->ReadBool("Other","SearchOnList",true);
	ShortenLinksCheckBox->Checked = Ini->ReadBool("Other","ShortenLinks",true);
	ShortenLinksModeComboBox->ItemIndex = Ini->ReadInteger("Other","ShortenLinksMode",1)-1;
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
	aFavouritesTabsChk->Execute();
	aSideSlideChk->Execute();
	aOtherChk->Execute();
	SaveButton->Enabled = false;
	CancelButton->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsExecute(TObject *Sender)
{
	//Zapisywanie poszczegolnych ustawien
	TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Settings.ini");
	//ClosedTabs
	Ini->WriteBool("ClosedTabs","Enable",RememberClosedTabsGroupBox->Checked);
	if((!FrmMainClosedTabsCheckBox->Checked)&&(!FrmSendClosedTabsCheckBox->Checked))
		Ini->WriteBool("ClosedTabs","FastAccess",false);
	else
		Ini->WriteBool("ClosedTabs","FastAccess",FastAccessClosedTabsGroupBox->Checked);
	Ini->WriteBool("ClosedTabs","FrmMain",FrmMainClosedTabsCheckBox->Checked);
	Ini->WriteBool("ClosedTabs","FrmSend",FrmSendClosedTabsCheckBox->Checked);
	Ini->WriteInteger("ClosedTabs","ItemsCount",ItemsCountClosedTabsSpinEdit->Value);
	Ini->WriteBool("ClosedTabs","ClosedTime",ShowTimeClosedTabsCheckBox->Checked);
	Ini->WriteString("ClosedTabs", "DateFormat", DateFormatEdit->Text);
	Ini->WriteBool("ClosedTabs","FastClear",FastClearClosedTabsCheckBox->Checked);
	Ini->WriteBool("ClosedTabs","HotKey",UnCloseTabHotKeyGroupBox->Checked);
	if(UnCloseTabHotKeyMode1RadioButton->Checked)
		Ini->WriteInteger("ClosedTabs","HotKeyMode",1);
	else
		Ini->WriteInteger("ClosedTabs","HotKeyMode",2);
	Ini->WriteInteger("ClosedTabs","HotKeyDef",UnCloseTabHotKeyInput->HotKey);
	Ini->WriteBool("ClosedTabs","SPMouse",UnCloseTabSPMouseCheckBox->Checked);
	Ini->WriteBool("ClosedTabs","LPMouse",UnCloseTabLPMouseCheckBox->Checked);
	Ini->WriteBool("ClosedTabs","2xLPMouse",UnCloseTab2xLPMouseCheckBox->Checked);
	Ini->WriteInteger("ClosedTabs","Count",CountClosedTabsSpinEdit->Value);
	Ini->WriteBool("ClosedTabs","RestoreLastMsg",RestoreLastMsgClosedTabsCheckBox->Checked);
	Ini->WriteBool("ClosedTabs","OnlyConversationTabs",OnlyConversationTabsCheckBox->Checked);
	Ini->WriteBool("ClosedTabs","SaveInfoInArchive",SaveClosedInfoInArchiveCheckBox->Checked);
	//UnsentMsg
	Ini->WriteBool("UnsentMsg","Enable",RememberUnsentMsgGroupBox->Checked);
	Ini->WriteBool("UnsentMsg","Info",InfoUnsentMsgGroupBox->Checked);
	Ini->WriteBool("UnsentMsg","Cloud",CloudUnsentMsgCheckBox->Checked);
	Ini->WriteBool("UnsentMsg","DetailedCloud",DetailedCloudUnsentMsgCheckBox->Checked);
	Ini->WriteBool("UnsentMsg","Tray",TrayUnsentMsgCheckBox->Checked);
	if((!FrmMainUnsentMsgCheckBox->Checked)&&(!FrmSendUnsentMsgCheckBox->Checked))
		Ini->WriteBool("UnsentMsg","FastAccess",false);
	else
		Ini->WriteBool("UnsentMsg","FastAccess",FastAccessUnsentMsgGroupBox->Checked);
	Ini->WriteBool("UnsentMsg","FrmMain",FrmMainUnsentMsgCheckBox->Checked);
	Ini->WriteBool("UnsentMsg","FrmSend",FrmSendUnsentMsgCheckBox->Checked);
	Ini->WriteBool("UnsentMsg","FastClear",FastClearUnsentMsgCheckBox->Checked);
	//TabsSwitching
	Ini->WriteBool("TabsSwitching","SwitchToNewMsg",SwitchToNewMsgGroupBox->Checked);
	if(SwitchToNewMsgMode1RadioButton->Checked)
		Ini->WriteInteger("TabsSwitching","SwitchToNewMsgMode", 1);
	else
		Ini->WriteInteger("TabsSwitching","SwitchToNewMsgMode", 2);
	Ini->WriteBool("TabsSwitching","TabsHotKeys",TabsHotKeysGroupBox->Checked);
	if(TabsHotKeysMode1RadioButton->Checked)
		Ini->WriteInteger("TabsSwitching","TabsHotKeysMode",1);
	else
		Ini->WriteInteger("TabsSwitching","TabsHotKeysMode",2);
	Ini->WriteBool("TabsSwitching","NewMgsHoyKey",NewMgsHoyKeyCheckBox->Checked);
	//SessionRemember
	Ini->WriteBool("SessionRemember","RestoreTabs",RestoreTabsSessionGroupBox->Checked);
	if(RestoreTabsSessionGroupBox->Checked)
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
	Ini->WriteBool("NewMsg","KeyboardFlasher",KeyboardFlasherCheckBox->Checked);
	Ini->WriteInteger("NewMsg","KeyboardFlasherMode",KeyboardFlasherModeComboBox->ItemIndex);
	Ini->WriteBool("NewMsg","CoreInactiveTabs",!OffCoreInactiveTabsNewMsgCheckBox->Checked);
	Ini->WriteBool("NewMsg","InactiveTabs",InactiveTabsNewMsgCheckBox->Checked);
	Ini->WriteBool("NewMsg","InactiveNotifer",InactiveNotiferNewMsgCheckBox->Checked);
	Ini->WriteBool("NewMsg","ChatStateNotifer",ChatStateNotiferNewMsgCheckBox->Checked);
	Ini->WriteBool("NewMsg","ChatGoneNotifer",ChatGoneNotiferNewMsgCheckBox->Checked);
	Ini->WriteBool("NewMsg","ChatGoneCloudNotifer",ChatGoneCloudNotiferNewMsgCheckBox->Checked);
	Ini->WriteBool("NewMsg","ChatGoneSoundNotifer",ChatGoneSoundNotiferNewMsgCheckBox->Checked);
	Ini->WriteBool("NewMsg","ChatGoneFrmSendNotifer",ChatGoneFrmSendNotiferNewMsgCheckBox->Checked);
	Ini->WriteBool("NewMsg","ChatGoneSaveInArchive",ChatGoneSaveInArchiveCheckBox->Checked);
	Ini->WriteBool("NewMsg","TaskbarPen",!TaskbarPenCheckBox->Checked);
	//Titlebar
	Ini->WriteBool("Titlebar","TweakSend",TweakFrmSendTitlebarGroupBox->Checked);
	if(TweakFrmSendTitlebarMode1RadioButton->Checked)
		Ini->WriteInteger("Titlebar","SendMode",1);
	else if(TweakFrmSendTitlebarMode2RadioButton->Checked)
		Ini->WriteInteger("Titlebar","SendMode",2);
	else if(TweakFrmSendTitlebarMode3RadioButton->Checked)
		Ini->WriteInteger("Titlebar","SendMode",3);
	else
		Ini->WriteInteger("Titlebar","SendMode",4);
	Ini->WriteBool("Titlebar","TweakMain",TweakFrmMainTitlebarGroupBox->Checked);
	if(TweakFrmMainTitlebarMode1RadioButton->Checked)
		Ini->WriteInteger("Titlebar","MainMode",1);
	else
		Ini->WriteInteger("Titlebar","MainMode",2);
	if(TweakFrmMainTitlebarModeExComboBox->ItemIndex!=-1)
		Ini->WriteInteger("Titlebar","MainModeEx",TweakFrmMainTitlebarModeExComboBox->ItemIndex);
	Ini->WriteString("Titlebar","MainText",TweakFrmMainTitlebarMode2Edit->Text);
	//ClipTabs
	Ini->WriteBool("ClipTabs","Enabled",ClipTabsGroupBox->Checked);
	Ini->WriteBool("ClipTabs","OpenClipTabs",OpenClipTabsCheckBox->Checked);
	Ini->WriteBool("ClipTabs","InactiveClipTabs",InactiveClipTabsCheckBox->Checked);
	Ini->WriteBool("ClipTabs","Counter",CounterClipTabsCheckBox->Checked);
	Ini->WriteBool("ClipTabs","ExcludeFromTabSwitching",ExcludeClipTabsFromTabSwitchingCheckBox->Checked);
	Ini->WriteBool("ClipTabs","ExcludeFromSwitchToNewMsg",!ExcludeClipTabsFromSwitchToNewMsgCheckBox->Checked);
	Ini->WriteBool("ClipTabs","ExcludeFromTabsHotKeys",ExcludeClipTabsFromTabsHotKeysCheckBox->Checked);
	Ini->WriteBool("ClipTabs","MiniAvatars",!NoMiniAvatarsClipTabsCheckBox->Checked);
	//FavouritesTabs
	Ini->WriteBool("FavouritesTabs","Enabled",FavouritesTabsGroupBox->Checked);
	Ini->WriteBool("FavouritesTabs","FastAccess",FastAccessFavouritesTabsGroupBox->Checked);
	Ini->WriteBool("FavouritesTabs","FrmMainFastAccess",FrmMainFastAccessFavouritesTabsCheckBox->Checked);
	Ini->WriteBool("FavouritesTabs","FrmSendFastAccess",FrmSendFastAccessFavouritesTabsCheckBox->Checked);
	Ini->WriteBool("FavouritesTabs","HotKeys",FavouritesTabsHotKeysCheckBox->Checked);
	TIniFile *SessionIni = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Session.ini");
	SessionIni->EraseSection("FavouritesTabs");
	if(FavouritesTabsListView->Items->Count)
	{
		for(int Count=0;Count<FavouritesTabsListView->Items->Count;Count++)
			SessionIni->WriteString("FavouritesTabs","Tab"+IntToStr(Count+1),FavouritesTabsListView->Items->Item[Count]->SubItems->Strings[0]);
	}
	delete SessionIni;
	//SideSlide
	Ini->WriteBool("SideSlide","SlideFrmMain",SlideFrmMainGroupBox->Checked);
	Ini->WriteInteger("SideSlide","FrmMainEdge",FrmMainEdgeComboBox->ItemIndex+1);
	Ini->WriteInteger("SideSlide","FrmMainSlideInDelay",FrmMainSlideInDelaySpinEdit->Value);
	Ini->WriteInteger("SideSlide","FrmMainSlideOutDelay",FrmMainSlideOutDelaySpinEdit->Value);
	Ini->WriteInteger("SideSlide","FrmMainSlideInTime",FrmMainSlideInTimeSpinEdit->Value);
	Ini->WriteInteger("SideSlide","FrmMainSlideOutTime",FrmMainSlideOutTimeSpinEdit->Value);
	Ini->WriteBool("SideSlide","ChangeTabAfterSlideIn",ChangeTabAfterSlideInCheckBox->Checked);
	Ini->WriteBool("SideSlide","SlideFrmSend",SlideFrmSendGroupBox->Checked);
	Ini->WriteInteger("SideSlide","FrmSendEdge",FrmSendEdgeComboBox->ItemIndex+1);
	Ini->WriteInteger("SideSlide","FrmSendSlideInDelay",FrmSendSlideInDelaySpinEdit->Value);
	Ini->WriteInteger("SideSlide","FrmSendSlideOutDelay",FrmSendSlideOutDelaySpinEdit->Value);
	Ini->WriteInteger("SideSlide","FrmSendSlideInTime",FrmSendSlideInTimeSpinEdit->Value);
	Ini->WriteInteger("SideSlide","FrmSendSlideOutTime",FrmSendSlideOutTimeSpinEdit->Value);
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
	Ini->WriteInteger("Other","CloudTimeOut",CloudTimeOutSpinEdit->Value);
	Ini->WriteBool("Other","CloudTickMode",CloudTickModeComboBox->ItemIndex);
	Ini->WriteBool("Other","SearchOnList",!SearchOnListCheckBox->Checked);
	Ini->WriteBool("Other","ShortenLinks",ShortenLinksCheckBox->Checked);
	Ini->WriteInteger("Other","ShortenLinksMode",ShortenLinksModeComboBox->ItemIndex+1);

	delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsWExecute(TObject *Sender)
{
	//Sprawdzanie zaleznosci opcji
	//ClosedTabs
	FastAccessClosedTabsGroupBox->Checked = (FastAccessClosedTabsGroupBox->Checked && (FrmMainClosedTabsCheckBox->Checked || FrmSendClosedTabsCheckBox->Checked));
	//UnsentMsg
	InfoUnsentMsgGroupBox->Checked = (InfoUnsentMsgGroupBox->Checked && (CloudUnsentMsgCheckBox->Checked || TrayUnsentMsgCheckBox->Checked));
	FastAccessUnsentMsgGroupBox->Checked = (FastAccessUnsentMsgGroupBox->Checked && (FrmMainUnsentMsgCheckBox->Checked || FrmSendUnsentMsgCheckBox->Checked));
	RememberUnsentMsgGroupBox->Checked = (InfoUnsentMsgGroupBox->Checked || FastAccessUnsentMsgGroupBox->Checked);
	//FavouritesTabs
	FavouritesTabsGroupBox->Checked = (FavouritesTabsGroupBox->Checked && (FastAccessFavouritesTabsGroupBox->Checked || FavouritesTabsHotKeysCheckBox->Checked));
	//Wylaczenie przyciskow
	SaveButton->Enabled = false;
	CancelButton->Enabled = false;
	OkButton->Enabled = false;
	//Usuwanie elementow z interfejsu AQQ
	DestroyFavouritesTabs();
	DestroyFrmUnsentMsg();
	DestroyFrmClosedTabs();
	DestroyStayOnTop();
	DestroyClipTab();
	DestroyFrmSendFavouriteTab();
	DestroyFrmMainFavouriteTab();
	//Zapisywanie ustawien
	aSaveSettings->Execute();
	//Odczytywanie ustawien w rdzeniu wtyczki
	LoadSettings();
	LoadFavouritesTabs();
	//Przypisywanie globalnego hooka na klawiature
	HookGlobalKeyboard();
	//Tworzenie elementow w interfejsie AQQ
	BuildFavouritesTabs(false);
	BuildFrmUnsentMsg(false);
	BuildFrmClosedTabs(false);
	BuildStayOnTop();
	BuildClipTab();
	BuildFrmSendFavouriteTab();
	BuildFrmMainFavouriteTab();
	//Zmiana tekstu na pasku tytulowym okna rozmowy
	ChangeFrmSendTitlebar();
	//Zmiana tekstu na pasku tytulowym okna kontaktow
	ChangeFrmMainTitlebar();
	//Ukrywanie/pokazywanie elementow okna rozmowy
	ShowToolBar();
	CheckHideScrollTabButtons();
	//Odswiezenie wszystkich zakladek
	RefreshTabs();
	//Wlaczenie przyciskow
	CancelButton->Enabled = true;
	OkButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aClosedTabsChkExecute(TObject *Sender)
{
	FrmMainClosedTabsCheckBox->Enabled = (RememberClosedTabsGroupBox->Checked && FastAccessClosedTabsGroupBox->Checked);
	FrmSendClosedTabsCheckBox->Enabled = (RememberClosedTabsGroupBox->Checked && FastAccessClosedTabsGroupBox->Checked);
	ShowTimeClosedTabsCheckBox->Enabled = (RememberClosedTabsGroupBox->Checked && FastAccessClosedTabsGroupBox->Checked);
	DateFormatEdit->Enabled = (RememberClosedTabsGroupBox->Checked && ShowTimeClosedTabsCheckBox->Checked);
	FastClearClosedTabsCheckBox->Enabled = (RememberClosedTabsGroupBox->Checked && FastAccessClosedTabsGroupBox->Checked);
	ItemsCountClosedTabsSpinEdit->Enabled = (RememberClosedTabsGroupBox->Checked && FastAccessClosedTabsGroupBox->Checked);
	UnCloseTabHotKeyMode1RadioButton->Enabled = (RememberClosedTabsGroupBox->Checked && UnCloseTabHotKeyGroupBox->Checked);
	UnCloseTabHotKeyMode2RadioButton->Enabled = (RememberClosedTabsGroupBox->Checked && UnCloseTabHotKeyGroupBox->Checked);
	UnCloseTabHotKeyInput->Enabled = (RememberClosedTabsGroupBox->Checked && UnCloseTabHotKeyMode2RadioButton->Checked);
	FastAccessClosedTabsGroupBox->Enabled = RememberClosedTabsGroupBox->Checked;
	UnCloseTabHotKeyGroupBox->Enabled = RememberClosedTabsGroupBox->Checked;
	CountClosedTabsSpinEdit->Enabled = RememberClosedTabsGroupBox->Checked;
	RestoreLastMsgClosedTabsCheckBox->Enabled = RememberClosedTabsGroupBox->Checked;
	OnlyConversationTabsCheckBox->Enabled = RememberClosedTabsGroupBox->Checked;
	SaveClosedInfoInArchiveCheckBox->Enabled = (RememberClosedTabsGroupBox->Checked && OnlyConversationTabsCheckBox->Checked);
	UnCloseMouseLabel->Enabled = RememberClosedTabsGroupBox->Checked;
	UnCloseTabSPMouseCheckBox->Enabled = RememberClosedTabsGroupBox->Checked;
	UnCloseTabLPMouseCheckBox->Enabled = RememberClosedTabsGroupBox->Checked;
	UnCloseTab2xLPMouseCheckBox->Enabled = RememberClosedTabsGroupBox->Checked;

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aUnsentMsgChkExecute(TObject *Sender)
{
	InfoUnsentMsgGroupBox->Enabled = RememberUnsentMsgGroupBox->Checked;
	CloudUnsentMsgCheckBox->Enabled = (RememberUnsentMsgGroupBox->Checked && InfoUnsentMsgGroupBox->Checked);
	DetailedCloudUnsentMsgCheckBox->Enabled = (RememberUnsentMsgGroupBox->Checked && InfoUnsentMsgGroupBox->Checked && CloudUnsentMsgCheckBox->Checked);
	TrayUnsentMsgCheckBox->Enabled = (RememberUnsentMsgGroupBox->Checked && InfoUnsentMsgGroupBox->Checked);
	FastAccessUnsentMsgGroupBox->Enabled = RememberUnsentMsgGroupBox->Checked;
	FrmMainUnsentMsgCheckBox->Enabled = (RememberUnsentMsgGroupBox->Checked && FastAccessUnsentMsgGroupBox->Checked);
	FrmSendUnsentMsgCheckBox->Enabled = (RememberUnsentMsgGroupBox->Checked && FastAccessUnsentMsgGroupBox->Checked);
	FastClearUnsentMsgCheckBox->Enabled = (RememberUnsentMsgGroupBox->Checked && FastAccessUnsentMsgGroupBox->Checked);

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aTabsSwitchingChkExecute(TObject *Sender)
{
	SwitchToNewMsgMode1RadioButton->Enabled = SwitchToNewMsgGroupBox->Checked;
	SwitchToNewMsgMode2RadioButton->Enabled = SwitchToNewMsgGroupBox->Checked;
	TabsHotKeysMode1RadioButton->Enabled = TabsHotKeysGroupBox->Checked;
	TabsHotKeysMode2RadioButton->Enabled = TabsHotKeysGroupBox->Checked;
	NewMgsHoyKeyCheckBox->Enabled = TabsHotKeysGroupBox->Checked;

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSessionRememberChkExecute(TObject *Sender)
{
	ManualRestoreTabsSessionCheckBox->Enabled = RestoreTabsSessionGroupBox->Checked;
	RestoreMsgSessionCheckBox->Enabled = RestoreTabsSessionGroupBox->Checked;

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aNewMsgChkExecute(TObject *Sender)
{
	KeyboardFlasherModeComboBox->Enabled = KeyboardFlasherCheckBox->Checked;
	OffCoreInactiveTabsNewMsgCheckBox->Enabled = !InactiveTabsNewMsgCheckBox->Checked;
	TaskbarPenCheckBox->Enabled = (InactiveFrmNewMsgCheckBox->Checked || TweakFrmSendTitlebarGroupBox->Checked);
	ChatGoneNotiferNewMsgCheckBox->Enabled = ChatStateNotiferNewMsgCheckBox->Checked;
	ChatGoneCloudNotiferNewMsgCheckBox->Enabled = (ChatStateNotiferNewMsgCheckBox->Checked && ChatGoneNotiferNewMsgCheckBox->Checked);
	ChatGoneSoundNotiferNewMsgCheckBox->Enabled = (ChatStateNotiferNewMsgCheckBox->Checked && ChatGoneNotiferNewMsgCheckBox->Checked);
	ChatGoneFrmSendNotiferNewMsgCheckBox->Enabled = (ChatStateNotiferNewMsgCheckBox->Checked && ChatGoneNotiferNewMsgCheckBox->Checked);
	ChatGoneSaveInArchiveCheckBox->Enabled = (ChatStateNotiferNewMsgCheckBox->Checked && ChatGoneFrmSendNotiferNewMsgCheckBox->Checked && ChatGoneNotiferNewMsgCheckBox->Checked);

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aTitlebarTweakChkExecute(TObject *Sender)
{
	TweakFrmSendTitlebarMode1RadioButton->Enabled = TweakFrmSendTitlebarGroupBox->Checked;
	TweakFrmSendTitlebarMode2RadioButton->Enabled = TweakFrmSendTitlebarGroupBox->Checked;
	TweakFrmSendTitlebarMode3RadioButton->Enabled = TweakFrmSendTitlebarGroupBox->Checked;
	TweakFrmSendTitlebarMode4RadioButton->Enabled = TweakFrmSendTitlebarGroupBox->Checked;
	TweakFrmMainTitlebarMode1RadioButton->Enabled = TweakFrmMainTitlebarGroupBox->Checked;
	TweakFrmMainTitlebarModeExComboBox->Enabled = (TweakFrmMainTitlebarGroupBox->Checked && TweakFrmMainTitlebarMode1RadioButton->Checked);
	TweakFrmMainTitlebarModeExLabel->Enabled = TweakFrmMainTitlebarGroupBox->Checked;
	TweakFrmMainTitlebarMode2RadioButton->Enabled = TweakFrmMainTitlebarGroupBox->Checked;
	TweakFrmMainTitlebarMode2Edit->Enabled = (TweakFrmMainTitlebarGroupBox->Checked && TweakFrmMainTitlebarMode2RadioButton->Checked);

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aClipTabsChkExecute(TObject *Sender)
{
	OpenClipTabsCheckBox->Enabled = ClipTabsGroupBox->Checked;
	InactiveClipTabsCheckBox->Enabled = ClipTabsGroupBox->Checked;
	CounterClipTabsCheckBox->Enabled = ClipTabsGroupBox->Checked;
	ExcludeClipTabsFromTabSwitchingCheckBox->Enabled = ClipTabsGroupBox->Checked;
	ExcludeClipTabsFromSwitchToNewMsgCheckBox->Enabled = (ClipTabsGroupBox->Checked && ExcludeClipTabsFromTabSwitchingCheckBox->Checked);
	ExcludeClipTabsFromTabsHotKeysCheckBox->Enabled = ClipTabsGroupBox->Checked;
	NoMiniAvatarsClipTabsCheckBox->Enabled = ClipTabsGroupBox->Checked;

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSideSlideChkExecute(TObject *Sender)
{
	FrmMainEdgeLabel->Enabled = SlideFrmMainGroupBox->Checked;
	FrmMainEdgeComboBox->Enabled = SlideFrmMainGroupBox->Checked;
	FrmMainSlideInDelaySpinEdit->Enabled = SlideFrmMainGroupBox->Checked;
	FrmMainSlideOutDelaySpinEdit->Enabled = SlideFrmMainGroupBox->Checked;
	FrmMainSlideInTimeSpinEdit->Enabled = SlideFrmMainGroupBox->Checked;
	FrmMainSlideOutTimeSpinEdit->Enabled = SlideFrmMainGroupBox->Checked;
	ChangeTabAfterSlideInCheckBox->Enabled = SlideFrmMainGroupBox->Checked;
	FrmSendEdgeLabel->Enabled = SlideFrmSendGroupBox->Checked;
	FrmSendEdgeComboBox->Enabled = SlideFrmSendGroupBox->Checked;
	FrmSendSlideInDelaySpinEdit->Enabled = SlideFrmSendGroupBox->Checked;
	FrmSendSlideOutDelaySpinEdit->Enabled = SlideFrmSendGroupBox->Checked;
	FrmSendSlideInTimeSpinEdit->Enabled = SlideFrmSendGroupBox->Checked;
	FrmSendSlideOutTimeSpinEdit->Enabled = SlideFrmSendGroupBox->Checked;
	SideSlideFullScreenModeCheckBox->Enabled = (SlideFrmSendGroupBox->Checked || SlideFrmMainGroupBox->Checked);
	SideSlideFullScreenModeExceptionsButton->Enabled = (SideSlideFullScreenModeCheckBox->Enabled && SideSlideFullScreenModeCheckBox->Checked);
	SideSlideCtrlAndMousBlockCheckBox->Enabled = (SlideFrmSendGroupBox->Checked || SlideFrmMainGroupBox->Checked);

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aOtherChkExecute(TObject *Sender)
{
	CollapseImagesModeComboBox->Enabled = CollapseImagesCheckBox->Checked;
	MinimizeRestoreHotKey->Enabled = MinimizeRestoreCheckBox->Checked;
	ShortenLinksModeComboBox->Enabled = ShortenLinksCheckBox->Checked;

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aExitExecute(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::SaveButtonClick(TObject *Sender)
{
	aSaveSettingsW->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OkButtonClick(TObject *Sender)
{
	aSaveSettingsW->Execute();
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
		DeleteFile(FileListBox->Items->Strings[Count]);
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
	//Zakończenie sesji z GDIPlus
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
	TaskbarPenCheckBox->Enabled = (InactiveFrmNewMsgCheckBox->Checked || TweakFrmSendTitlebarGroupBox->Checked);
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

void __fastcall TSettingsForm::GetYouTubeTitleThreadRun(TIdThreadComponent *Sender)
{
	//Pobranie itemu z listy ID do przetworzenia
	UnicodeString ID = GetYouTubeTitleListItem();
	//Jest jakis ID do przetworzenia
	if(!ID.IsEmpty())
	{
		//Pobieranie tytulu
		UnicodeString XML = IdHTTPGet("http://gdata.youtube.com/feeds/api/videos/"+ID+"?fields=title");
		//Parsowanie pliku XML
		if(!XML.IsEmpty())
		{
			_di_IXMLDocument XMLDoc = LoadXMLData(XML);
			_di_IXMLNode MainNode = XMLDoc->DocumentElement;
			_di_IXMLNode ChildNode = MainNode->ChildNodes->GetNode(0);
			UnicodeString Title = ChildNode->GetText();
			//Zapisywanie tytulu do cache
			if(!Title.IsEmpty())
			{
				TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Session.ini");
				Ini->WriteString("YouTube",ConvertToInt(ID),EncodeBase64(Title));
				delete Ini;
			}
			//Blokowanie wskaznego ID na czas sesji
			else AddToYouTubeExcludeList(ID);
		}
		//Blokowanie wskaznego ID na czas sesji
		else AddToYouTubeExcludeList(ID);
	}
	//Brak itemow do przetworzenia
	if(!ChkYouTubeListItem())
	{
		//Zatrzymanie watku
		GetYouTubeTitleThread->Stop();
		//Wlaczenie timera
		RefreshTimer->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::RefreshTimerTimer(TObject *Sender)
{
	//Odswiezenie listy kontaktow jest dozwolone
	if(RefreshListAllowed())
	{
		//Wylaczenie timera
		RefreshTimer->Enabled = false;
		//Odswiezenie listy kontaktow
		RefreshList();
	}
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::sSkinManagerSysDlgInit(TacSysDlgData DlgData, bool &AllowSkinning)
{
	AllowSkinning = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aFavouritesTabsChkExecute(TObject *Sender)
{
	FastAccessFavouritesTabsGroupBox->Enabled = FavouritesTabsGroupBox->Checked;
	FrmMainFastAccessFavouritesTabsCheckBox->Enabled = (FavouritesTabsGroupBox->Checked && FastAccessFavouritesTabsGroupBox->Checked);
	FrmSendFastAccessFavouritesTabsCheckBox->Enabled = (FavouritesTabsGroupBox->Checked && FastAccessFavouritesTabsGroupBox->Checked);
	FavouritesTabsHotKeysCheckBox->Enabled = FavouritesTabsGroupBox->Checked;
	FavouritesTabsListView->Enabled = FavouritesTabsGroupBox->Checked;
	AddChatsFavouriteTabSpeedButton->Enabled = FavouritesTabsGroupBox->Checked;

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aReloadFavouritesTabsExecute(TObject *Sender)
{
	//Usuwanie listy ulubionych zakladek
	FavouritesTabsListView->Clear();
	//Odczyt ulubionych zakladek
	TIniFile *SessionIni = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Session.ini");
	TStringList *FavouritesTabs = new TStringList;
	SessionIni->ReadSection("FavouritesTabs",FavouritesTabs);
	int TabsCount = FavouritesTabs->Count;
	delete FavouritesTabs;
	if(TabsCount>0)
	{
		for(int Count=0;Count<TabsCount;Count++)
		{
			UnicodeString FavouriteTab = SessionIni->ReadString("FavouritesTabs", "Tab"+IntToStr(Count+1),"");
			if(!FavouriteTab.IsEmpty())
			{
				FavouritesTabsListView->Items->Add();
				FavouritesTabsListView->Items->Item[FavouritesTabsListView->Items->Count-1]->Caption = GetContactNick(FavouriteTab) + " (" + FriendlyFormatJID(FavouriteTab) + ")";
				FavouritesTabsListView->Items->Item[FavouritesTabsListView->Items->Count-1]->SubItems->Add(FavouriteTab);
			}
		}
	}
	delete SessionIni;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FavouritesTabsListViewKeyDown(TObject *Sender, WORD &Key,
					TShiftState Shift)
{
	//Wcisniecie przycisku Delete
	if(Key==46)
	{
		//Usuwanie elementu
		if(FavouritesTabsListView->ItemIndex!=-1)
		{
			FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex]->Delete();
			SaveButton->Enabled = true;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FavouritesTabsListViewSelectItem(TObject *Sender, TListItem *Item, bool Selected)
{
	if(FavouritesTabsListView->ItemIndex!=-1)
	{
		if(FavouritesTabsListView->ItemIndex!=0) MoveUpFavouriteTabSpeedButton->Enabled = true;
		if(FavouritesTabsListView->ItemIndex!=FavouritesTabsListView->Items->Count-1) MoveDownFavouriteTabSpeedButton->Enabled = true;
		RemoveFavouriteTabSpeedButton->Enabled = true;
	}
	else
	{
		MoveUpFavouriteTabSpeedButton->Enabled = false;
		MoveDownFavouriteTabSpeedButton->Enabled = false;
		RemoveFavouriteTabSpeedButton->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::AddChatsFavouriteTabSpeedButtonClick(TObject *Sender)
{
	//Plik z lista pokojow oznaczonych gwiazdka istnieje
	if(FileExists(GetUserDir() + "\\\\FavRooms.cc"))
	{
		//Odczyt listy pokojow oznaczonych gwiazdka
		FileMemo->Lines->LoadFromFile(GetUserDir() + "\\\\FavRooms.cc");
		//Plik zawiera liste pokojow
		if(!FileMemo->Text.IsEmpty())
		{
			for(int Count=0;Count<FileMemo->Lines->Count;Count++)
			{
				//Odczyt JID z pliku
				UnicodeString JID = FileMemo->Lines->Strings[Count];
				//Zapobiegawcze pobranie nazwy kanalu
				GetChannelNameW(JID);
				//Sformatowanie JID na potrzeby wtyczki
				JID = "ischat_" + JID + ":0";
				//Sprawdzenie czy JID znajduje sie juz na liscie
				bool ItemExists = false;
				for(int fCount=0;fCount<FavouritesTabsListView->Items->Count;fCount++)
				{
					if(JID==FavouritesTabsListView->Items->Item[fCount]->SubItems->Strings[0])
						ItemExists = true;
				}
				//JID nie znajduje sie na liscie
				if(!ItemExists)
				{
					//Maksymalnie 10 elementow
					if(FavouritesTabsListView->Items->Count<10)
					{
						//Dodanie elementow do listy
						FavouritesTabsListView->Items->Add();
						FavouritesTabsListView->Items->Item[FavouritesTabsListView->Items->Count-1]->Caption = GetContactNick(JID) + " (" + FriendlyFormatJID(JID) + ")";
						FavouritesTabsListView->Items->Item[FavouritesTabsListView->Items->Count-1]->SubItems->Add(JID);
						//Wlaczenie przycisku zapisu
						SaveButton->Enabled = true;
					}
					//Osiagnieto limit
					else
					{
						ShowFavouritesTabsInfo(GetLangStr("MaxFavTabs"));
						Count = FileMemo->Lines->Count;
					}
				}
			}
		}
		else ShowFavouritesTabsInfo(GetLangStr("NoMarkedRooms"));
		//Usuniecie pamieci
		FileMemo->Text = "";
	}
	//Plik nie istnieje
	else ShowFavouritesTabsInfo(GetLangStr("NoMarkedRooms"));
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FavouritesTabsListViewExit(TObject *Sender)
{
	MoveUpFavouriteTabSpeedButton->Enabled = false;
	MoveDownFavouriteTabSpeedButton->Enabled = false;
	RemoveFavouriteTabSpeedButton->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::MoveUpFavouriteTabSpeedButtonClick(TObject *Sender)
{
	UnicodeString Caption = FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex]->Caption;
	UnicodeString SubString = FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex]->SubItems->Strings[0];
	FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex]->Caption = FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex-1]->Caption;
	FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex]->SubItems->Strings[0] = FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex-1]->SubItems->Strings[0];
	FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex-1]->Caption = Caption;
	FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex-1]->SubItems->Strings[0] = SubString;
	FavouritesTabsListView->ItemIndex = FavouritesTabsListView->ItemIndex-1;
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::MoveDownFavouriteTabSpeedButtonClick(TObject *Sender)
{
	UnicodeString Caption = FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex]->Caption;
	UnicodeString SubString = FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex]->SubItems->Strings[0];
	FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex]->Caption = FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex+1]->Caption;
	FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex]->SubItems->Strings[0] = FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex+1]->SubItems->Strings[0];
	FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex+1]->Caption = Caption;
	FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex+1]->SubItems->Strings[0] = SubString;
	FavouritesTabsListView->ItemIndex = FavouritesTabsListView->ItemIndex+1;
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::RemoveFavouriteTabSpeedButtonClick(TObject *Sender)
{
	FavouritesTabsListView->Items->Item[FavouritesTabsListView->ItemIndex]->Delete();
	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::DateFormatEditChange(TObject *Sender)
{
	//Ustawienie hintu w kontrolce formatu daty
	TDateTime DateTime = TDateTime::CurrentDateTime();
	DateFormatEdit->Hint = DateTime.FormatString(DateFormatEdit->Text);

	SaveButton->Enabled = true;
}
//---------------------------------------------------------------------------


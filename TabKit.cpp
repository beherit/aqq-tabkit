//---------------------------------------------------------------------------
// Copyright (C) 2010-2013 Krzysztof Grochocki
//
// This file is part of TabKit
//
// TabKit is free software; you can redistribute it and/or modify
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
// along with GNU Radio; see the file COPYING. If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <PluginAPI.h>
#include "SettingsFrm.h"
#include <inifiles.hpp>
#include <process.h>
#include <tlhelp32.h>
#include <IdHashMessageDigest.hpp>
#include <boost/regex.hpp>
using namespace boost;
using namespace std;
#define RESOURCESCHANGER_SYSTEM_RESOURCECHANGED L"ResourcesChanger/System/ResourceChanged"
#define ALPHAWINDOWS_OLDPROC L"AlphaWindows/OldProc"
#define TABKIT_OLDPROC L"TabKit/OldProc"

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

//Uchwyt-do-formy-ustawien---------------------------------------------------
TSettingsForm *hSettingsForm;
//Struktury-glowne-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//ClosedTabs-----------------------------------------------------------------
//JID aktualnie przywracanej zakladki
UnicodeString JustUnClosedJID;
//Zakladka przywrocona ze skrotu klawiaturowego
bool UnCloseTabFromHotKey = false;
//Zmienne nt. kontatku przywracanego przez skrot klawiaturowy
UnicodeString UnCloseTabFromHotKeyJID;
int UnCloseTabFromHotKeyUserIdx;
//Przywracanie zakladki za pomoca myszki
bool TabWasChanged = false;
//Lista JID zamknietych zakladek + data zamkniecia + akceptowalne kontakty
TStringList *ClosedTabsList = new TStringList;
TStringList *ClosedTabsTimeList = new TStringList;
TStringList *AcceptClosedTabsList = new TStringList;
//TabsSwitching--------------------------------------------------------------
//Lista JID z nowymi wiadomosciami
TStringList *MsgList = new TStringList;
//Blokowanie otwierania paru zakladek jednoczesnie
bool NewMgsHoyKeyExecute = false;
//SessionRemember------------------------------------------------------------
//Status przywracania sesji
bool RestoringSession = false;
//Odznaczenie uruchomienia przywracania sesji z czatami
bool RestoringChatSession = false;
//Lista JID czatowych do przywrocenia
TStringList *ChatSessionList = new TStringList;
//NewMsg---------------------------------------------------------------------
//Licznik nowych wiadomosci na belce okna rozmowy
int InactiveFrmNewMsgCount = 0;
//Licznik nowych wiadomosci na zakladkach w oknie rozmowy
TCustomIniFile* InactiveTabsNewMsgCount = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Blokada zmiany tekstu na zakladce
bool TabWasClosed = false;
//Blokowanie notyfikatora nowych wiadomosci
bool BlockInactiveNotiferNewMsg = false;
//Stan polaczenia sieci
bool NetworkConnecting = false;
//Sciezki do ikon notyfikacji pisania wiadomosci
UnicodeString ComposingIconSmall;
UnicodeString ComposingIconBig;
UnicodeString PauseIconSmall;
UnicodeString PauseIconBig;
//Oryginalne ikony okna rozmowy
HICON hIconSmall = NULL;
HICON hIconBig = NULL;
//Poprzedni stan pisania wiadomosci
int LastChatState = 0;
//Titlebar-------------------------------------------------------------------
//Domyslny tekst okna kontaktow
wchar_t* FrmMainTitlebar = new wchar_t[16];
//Lista zmienionych tekstow belek okna rozmowy
TCustomIniFile* ChangedTitlebarList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Tekst belki okna rozmowy
UnicodeString FrmSendTitlebar;
//Nazwa aktywnego profilu
UnicodeString ProfileName;
//Nazwa komputera
UnicodeString ComputerName;
//Zasob glownego konta Jabber
UnicodeString ResourceName;
//ClipTabs-------------------------------------------------------------------
//JID kontaktu z menu popTab
UnicodeString ClipTabPopup;
//Lista JID przypietych zakladek
TStringList *ClipTabsList = new TStringList;
TCustomIniFile* ClipTabsIconList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Ostatnio aktywna zakladka przed otwieraniem przypietych czatow
UnicodeString ActiveTabBeforeOpenClipTabs;
//JID ostatniej otwartej przypietej zakladki czatowej
UnicodeString LastOpenedChatClipTab;
//SideSlide-----------------------------------------------------------------
//Oryginalny rozmiar+pozycja okna kontaktow
TRect FrmMainRect;
WINDOWINFO FrmMainInfo;
int FrmMainRealLeftPos;
int FrmMainRealRightPos;
int FrmMainRealBottomPos;
int FrmMainRealTopPos;
//Pozycja okna kontaktow podczas SlideIn/SlideOut
int FrmMainSlideLeft;
int FrmMainSlideTop;
//Aktualny status Slide okna kontaktow
bool FrmMainVisible = false;
bool FrmMainBlockSlide = true;
bool FrmMainBlockSlideWndEvent = false;
bool FrmMainBlockMousePositionTimer = false;
bool FrmMainDontBlockSlide = false;
bool PreFrmMainSlideIn = false;
bool FrmMainSlideIn = false;
bool PreFrmMainSlideOut = false;
bool CurPreFrmMainSlideOut = false;
bool FrmMainSlideOut = false;
bool FrmMainSlideOutWndActiv = false;
bool FrmMainSlideOutActivFrmSend = false;
bool FrmCreateChatExists = false;
//Dane paska menu start dla okna rozmowy
int FrmMain_Shell_TrayWndLeft;
int FrmMain_Shell_TrayWndRight;
int FrmMain_Shell_TrayWndBottom;
int FrmMain_Shell_TrayWndTop;
//Oryginalny rozmiar+pozycja okna rozmowy
TRect FrmSendRect;
WINDOWINFO FrmSendInfo;
int FrmSendRealLeftPos;
int FrmSendRealRightPos;
int FrmSendRealBottomPos;
int FrmSendRealTopPos;
//Pozycja okna rozmowy podczas SlideIn/SlideOut
int FrmSendSlideLeft;
int FrmSendSlideTop;
//Aktualny status Slide okna rozmowy
bool FrmSendVisible = false;
bool FrmSendBlockSlide = true;
bool FrmSendBlockSlideWndEvent = false;
bool FrmSendBlockSlideOnMsgComposing = false;
bool FrmSendBlockSlideOnRestore = false;
bool FrmSendBlockMousePositionTimer = false;
bool FrmSendDontBlockSlide = false;
bool PreFrmSendSlideIn = false;
bool FrmSendSlideIn = false;
bool PseudoFrmMainSlideIn = false;
bool PreFrmSendSlideOut = false;
bool FrmSendSlideOut = false;
bool FrmSendRestoreRect = false;
bool FrmSendActivate = false;
//Dane paska menu start dla okna rozmowy
int FrmSend_Shell_TrayWndLeft;
int FrmSend_Shell_TrayWndRight;
int FrmSend_Shell_TrayWndBottom;
int FrmSend_Shell_TrayWndTop;
//Tryb pelno ekranowych aplikacji
bool FullScreenMode = false;
bool FullScreenModeExeptions = false;
HWND FullScreenWindow;
//Ostatnio aktywne okno
HWND LastActiveWindow;
HWND LastActiveWindow_WmInactiveFrmSendSlide = NULL;
HWND LastActiveWindow_PreFrmSendSlideIn = NULL;
HWND LastActiveWindow_WmInactiveFrmMainSlide = NULL;
HWND LastActiveWindow_PreFrmMainSlideIn = NULL;
//Blokada okna przy aktywnym popupmenu poza obszarem okna
bool PopupMenuBlockSlide = false;
//Blokada przy zmianie pozycji okien
bool WM_ENTERSIZEMOVE_BLOCK = false;
//Lista JID z nowymi wiadomosciami
TStringList *UnOpenMsgList = new TStringList;
//Lista procesow wykluczonych z aplikacji pelnoekranowych
TStringList *SideSlideExceptions = new TStringList;
//Other----------------------------------------------------------------------
//CollapseImages
UnicodeString CollapseImagesItemURL;
TStringList *CollapseImagesList = new TStringList;
//StayOnTop
bool StayOnTopStatus;
//Pokazywanie paska narzedzi
bool ToolBarShowing = false;
//Zamykanie zakladki poprzez 2xLPM
bool LBUTTONDBLCLK = false;
//EmuTabsW
bool EmuTabsWSupport;
//INNE-----------------------------------------------------------------------
//Aktywna otwarta zakladka
UnicodeString ActiveTabJID;
UnicodeString ActiveTabJIDRes;
//Ostatnio aktywna otwarta zakladka
UnicodeString LastActiveTabJID;
//Lista JID z notyfikacjami wiadomosci
TStringList *PreMsgList = new TStringList;
TCustomIniFile* PreMsgStateList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Lista JID otwartych zakladek
TStringList *TabsList = new TStringList;
TStringList *ResTabsList = new TStringList;
//Lista JID wraz ze stanami
TCustomIniFile* ContactsStateList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Lista JID wraz z indeksami konta
TCustomIniFile* ContactsIndexList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Lista JID wraz z nickami
TCustomIniFile* ContactsNickList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Lista ID filmow YouTube do przetworzenia
TStringList *GetYouTubeTitleList = new TStringList;
//Sciezka do pliku sesji
UnicodeString SessionFileDir;
//Sciezka do pliku ustawien
UnicodeString SettingsFileDir;
//Sciezka do katalogu prywatnego wtyczek
UnicodeString PluginUserDir;
//Maksymalizowanie okna
bool FrmSendMaximized = false;
//Okno rozmowy zostalo otwarte
bool FrmSendOpening = false;
//PID procesu
DWORD ProcessPID;
//SecureMode
bool SecureMode = false;
//ID wywolania enumeracji listy kontaktow
DWORD ReplyListID = 0;
//Informacja o widocznym oknie zmiany opisu
bool FrmSetStateExist = false;
//Informacja o widocznym oknie instalowania dodatku
bool FrmInstallAddonExist = false;
//Informacja o widoczym oknie tworzenia wycinka
bool FrmPosExist = false;
//LOAD/UNLOAD-PLUGIN---------------------------------------------------------
//Gdy zostalo uruchomione zaladowanie wtyczki
bool LoadExecuted = false;
//Gdy zostalo uruchomione wyladowanie wtyczki
bool UnloadExecuted = false;
//Gdy zostalo uruchomione wyladowanie wtyczki wraz z zamknieciem komunikatora
bool ForceUnloadExecuted = false;
//THEME-CHANGING-------------------------------------------------------------
bool ThemeChanging = false;
//UCHWTY-DO-OKIEN------------------------------------------------------------
//Uchwyt do okna rozmowy
HWND hFrmSend;
//Uchwyt do pola RichEdit
HWND hRichEdit;
//Uchwyt do paska informacyjnego
HWND hStatusBar;
//Uchwyt do paska narzedzi
HWND hToolBar;
//Uchwyt do paska zakladek
HWND hTabsBar;
//Uchwyty do przyciskow strzalek do przewijania zakladek
HWND hScrollTabButton[2];
//Uchwyt do okna kontaktow
HWND hFrmMain;
//Ucwyt do kontrolki IE w oknie kontatkow
HWND hFrmMainFocus;
//Uchwyt do okna wyszukiwarki na liscie kontatkow
HWND hFrmSeekOnList;
//Uchwyt do okna archiwum
HWND hFrmArch;
//Uchwyt do okna timera
HWND hTimerFrm;
//Uchwyt do ostatnio aktywnego popupmenu
HWND hPopupMenu;
//Uchwyt do systemowego obszaru powiadomien
HWND hToolbarWindow32;
//Uchwyt do paska menu start
HWND Shell_TrayWnd;
//SYSTEMOWE-HOOK'I-ORAZ-PROCKI-----------------------------------------------
//Lokalny hook na klawiaturê
HHOOK hThreadKeyboard;
//Lokalny hook na myszke
HHOOK hThreadMouse;
//Stara procka okna rozmowy
WNDPROC OldFrmSendProc;
//Aktualna procka okna rozmowy
WNDPROC CurrentFrmSendProc;
//Stara procka okna kontaktow
WNDPROC OldFrmMainProc;
//Aktualna procka okna kontaktow
WNDPROC CurrentFrmMainProc;
//Stara procka okna wyszukiwarki
WNDPROC OldFrmSeekOnListProc;
//Aktualna procka wyszukiwarki
WNDPROC CurrentFrmSeekOnListProc;
//IKONY-W-INTERFEJSIE--------------------------------------------------------
int CLOSEDTABS;
int UNSENTMSG;
int COMPOSING;
int PAUSE;
int GONE;
int STAYONTOP_OFF;
int STAYONTOP_ON;
int FASTACCESS;
//TIMERY---------------------------------------------------------------------
#define TIMER_FINDSTATUSBARPRO 10
#define TIMER_FINDTOOLBAR 20
#define TIMER_FINDTABSBAR 30
#define TIMER_STAYONTOP 40
#define TIMER_TURNOFFMODAL 50
#define TIMER_INACTIVENOTIFER 60
#define TIMER_TOOLBAR 70
#define TIMER_RESTORESESSION 80
#define TIMER_MOVECLIPTAB 90
#define TIMER_OPENCLIPTABS 100
#define TIMER_LOADLASTCONV 110
#define TIMER_CHKSETTINGS 120
#define TIMER_CLOSEBY2XLPM 130
#define TIMER_ACTIVATERICHEDIT 140
#define TIMER_MOUSEPOSITION 150
#define TIMER_FRMSENDMOUSEPOSITION 160
#define TIMER_FRMMAINMOUSEPOSITION 170
#define TIMER_CHKACTIVEWINDOW 180
#define TIMER_PREFRMSENDSLIDEOUT 190
#define TIMER_FRMSENDSLIDEOUT 200
#define TIMER_PREFRMSENDSLIDEIN 210
#define TIMER_FRMSENDSLIDEIN 220
#define TIMER_FRMSENDBLOCKSLIDE 230
#define TIMER_FRMSENDMINIMIZED 240
#define TIMER_FRMSENDCHANGEPOS 250
#define TIMER_FRMSENDSETTOPMOST 260
#define TIMER_FRMSENDSETTOPMOSTANDSLIDE 270
#define TIMER_FRMSENDUNBLOCKMOUSE 280
#define TIMER_PREFRMMAINDSLIDEOUT 290
#define TIMER_FRMMAINSLIDEOUT 300
#define TIMER_PREFRMMAINSLIDEIN 310
#define TIMER_FRMMAINSLIDEIN 320
#define TIMER_FRMMAINBLOCKSLIDE 330
#define TIMER_FRMMAINSETTOPMOST 340
#define TIMER_FRMMAINSETTOPMOSTEX 350
#define TIMER_FRMMAINSETTOPMOSTANDSLIDE 360
//SETTINGS-------------------------------------------------------------------
//ClosedTabs
bool ClosedTabsChk;
bool FastAccessClosedTabsChk;
bool FrmMainClosedTabsChk;
bool FrmSendClosedTabsChk;
int ItemCountUnCloseTabVal;
bool ShowTimeClosedTabsChk;
bool FastClearClosedTabsChk;
bool UnCloseTabHotKeyChk;
int UnCloseTabHotKeyMode;
int UnCloseTabHotKeyDef;
bool UnCloseTabSPMouseChk;
bool UnCloseTabLPMouseChk;
int CountUnCloseTabVal;
bool RestoreLastMsgChk;
bool OnlyConversationTabsChk;
//UnsentMsg
bool UnsentMsgChk;
bool InfoUnsentMsgChk;
bool CloudUnsentMsgChk;
bool DetailedCloudUnsentMsgChk;
bool TrayUnsentMsgChk;
bool FastAccessUnsentMsgChk;
bool FrmMainUnsentMsgChk;
bool FrmSendUnsentMsgChk;
bool FastClearUnsentMsgChk;
//TabsSwitching
bool SwitchToNewMsgChk;
int SwitchToNewMsgMode;
bool TabsHotKeysChk;
int TabsHotKeysMode;
bool NewMgsHoyKeyChk;
//SessionRemember
bool RestoreTabsSessionChk;
bool ManualRestoreTabsSessionChk;
bool RestoreMsgSessionChk;
//NewMsg
bool InactiveFrmNewMsgChk;
bool InactiveTabsNewMsgChk;
bool InactiveNotiferNewMsgChk;
bool ChatStateNotiferNewMsgChk;
bool ChatGoneNotiferNewMsgChk;
bool TaskbarPenChk;
//Titlebar
bool TweakFrmSendTitlebarChk;
int TweakFrmSendTitlebarMode = 0;
bool TweakFrmMainTitlebarChk;
int TweakFrmMainTitlebarMode;
int TweakFrmMainTitlebarModeEx;
UnicodeString TweakFrmMainTitlebarText;
//ClipTabs
bool OpenClipTabsChk;
bool InactiveClipTabsChk;
bool CounterClipTabsChk;
bool ExClipTabsFromTabSwitchingChk;
bool ExClipTabsFromSwitchToNewMsgChk;
bool ExClipTabsFromTabsHotKeysChk;
bool MiniAvatarsClipTabsChk;
//SideSlide
bool FrmMainSlideChk = false;
int FrmMainSlideEdge = 2;
int FrmMainSlideHideMode;
int FrmMainSlideInDelay;
int FrmMainSlideOutDelay;
int FrmMainSlideInTime;
int FrmMainSlideOutTime;
int FrmMainStepInterval;
bool ChangeTabAfterSlideIn;
bool FrmSendSlideChk = false;
int FrmSendSlideEdge = 1;
int FrmSendSlideHideMode;
int FrmSendSlideInDelay;
int FrmSendSlideOutDelay;
int FrmSendSlideInTime;
int FrmSendSlideOutTime;
int FrmSendStepInterval;
bool SlideInAtNewMsgChk;
bool SideSlideFullScreenModeChk;
bool SideSlideCtrlAndMouseBlockChk;
//Other
bool QuickQuoteChk;
bool CollapseImagesChk;
int CollapseImagesMode;
bool MinimizeRestoreChk;
int MinimizeRestoreKey;
bool StayOnTopChk;
bool HideToolBarChk;
bool HideTabCloseButtonChk;
bool HideScrollTabButtonsChk;
bool CloseBy2xLPMChk;
bool EmuTabsWChk;
int CloudTimeOut;
bool CloudTickModeChk;
bool ShortenLinksChk = true;
int ShortenLinksMode;
//FORWARD-AQQ-HOOKS----------------------------------------------------------
int __stdcall OnActiveTab(WPARAM wParam, LPARAM lParam);
int __stdcall OnAddLine(WPARAM wParam, LPARAM lParam);
int __stdcall OnAutoSecureOn(WPARAM wParam, LPARAM lParam);
int __stdcall OnAutoSecureOff(WPARAM wParam, LPARAM lParam);
int __stdcall OnBeforeUnload(WPARAM wParam, LPARAM lParam);
int __stdcall OnCloseTab(WPARAM wParam, LPARAM lParam);
int __stdcall OnCloseTabMessage(WPARAM wParam, LPARAM lParam);
int __stdcall OnColorChange(WPARAM wParam, LPARAM lParam);
int __stdcall OnContactsUpdate(WPARAM wParam, LPARAM lParam);
int __stdcall OnFetchAllTabs(WPARAM wParam, LPARAM lParam);
int __stdcall OnFetchAllTabsW(WPARAM wParam, LPARAM lParam);
int __stdcall OnListReady(WPARAM wParam, LPARAM lParam);
int __stdcall OnMsgComposing(WPARAM wParam, LPARAM lParam);
int __stdcall OnMsgContextClose(WPARAM wParam, LPARAM lParam);
int __stdcall OnMsgContextPopup(WPARAM wParam, LPARAM lParam);
int __stdcall OnPerformCopyData(WPARAM wParam, LPARAM lParam);
int __stdcall OnPreSendMsg(WPARAM wParam, LPARAM lParam);
int __stdcall OnPrimaryTab(WPARAM wParam, LPARAM lParam);
int __stdcall OnRecvMsg(WPARAM wParam, LPARAM lParam);
int __stdcall OnRecvOldProc(WPARAM wParam, LPARAM lParam);
int __stdcall OnReplyList(WPARAM wParam, LPARAM lParam);
int __stdcall OnSystemRestart(WPARAM wParam, LPARAM lParam);
int __stdcall OnResourceChanged(WPARAM wParam, LPARAM lParam);
int __stdcall OnSetHTMLStatus(WPARAM wParam, LPARAM lParam);
int __stdcall OnSetLastState(WPARAM wParam, LPARAM lParam);
int __stdcall OnStateChange(WPARAM wParam, LPARAM lParam);
int __stdcall OnSystemPopUp(WPARAM wParam, LPARAM lParam);
int __stdcall OnTabCaption(WPARAM wParam, LPARAM lParam);
int __stdcall OnTabImage(WPARAM wParam, LPARAM lParam);
int __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam);
int __stdcall OnTrayClick(WPARAM wParam, LPARAM lParam);
int __stdcall OnWindowEvent(WPARAM wParam, LPARAM lParam);
int __stdcall OnXMLIDDebug(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItem0(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItem1(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItem2(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItem3(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItem4(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItem5(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItem6(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItem7(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItem8(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItem9(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClosedTabsItemClear(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceUnsentMsgItem0(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceUnsentMsgItem1(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceUnsentMsgItem2(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceUnsentMsgItem3(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceUnsentMsgItem4(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceUnsentMsgItemClear(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceUnsentMsgShowAllItem(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceClipTabItem(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceQuickQuoteItem(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceCollapseImagesItem(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceStayOnTopItem(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceTabKitFastSettingsItem(WPARAM wParam, LPARAM lParam);
//FORWARD-TIMER--------------------------------------------------------------
LRESULT CALLBACK TimerFrmProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//FORWARD-WINDOW-PROC--------------------------------------------------------
LRESULT CALLBACK FrmMainProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK FrmSendProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//FORWARD-OTHER-FUNCTION-----------------------------------------------------
void RefreshTabs();
void CheckHideScrollTabButtons();
void LoadSettings();
//---------------------------------------------------------------------------

//Szukanie uchwytu do okna kontaktow
bool CALLBACK FindFrmMain(HWND hWnd, LPARAM)
{
  //Pobranie klasy okna
  wchar_t WClassName[128];
  GetClassNameW(hWnd, WClassName, sizeof(WClassName));
  //Sprawdenie klasy okna
  if((UnicodeString)WClassName=="TfrmMain")
  {
	//Pobranie PID okna
	DWORD PID;
	GetWindowThreadProcessId(hWnd, &PID);
	//Porownanie PID okna
	if(PID==ProcessPID)
	{
	  //Przypisanie uchwytu
	  hFrmMain = hWnd;
	  //Pobieranie oryginalnego titlebar okna
	  GetWindowTextW(hFrmMain,FrmMainTitlebar,16);
	  return false;
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do kontrolki IE w oknie kontatkow
bool CALLBACK FindFrmMainFocus(HWND hWnd, LPARAM)
{
  //Pobranie klasy okna
  wchar_t WClassName[128];
  GetClassNameW(hWnd, WClassName, sizeof(WClassName));
  //Sprawdzenie klasy okna
  if((UnicodeString)WClassName=="Internet Explorer_Server")
  {
	//Przypisanie uchwytu
	hFrmMainFocus = hWnd;
	return false;
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do kontrolki TsRichEdit
bool CALLBACK FindRichEdit(HWND hWnd, LPARAM)
{
  //Pobranie klasy okna
  wchar_t WClassName[128];
  GetClassNameW(hWnd, WClassName, sizeof(WClassName));
  //Sprawdzenie klasy okna
  if((UnicodeString)WClassName=="TsRichEdit")
  {
	//Przypisanie uchwytu
	hRichEdit = hWnd;
	return false;
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do kontrolki TToolBar (pasek narzedzi)
bool CALLBACK FindToolBar(HWND hWnd, LPARAM)
{
  //Pobranie klasy okna
  wchar_t WClassName[128];
  GetClassNameW(hWnd, WClassName, sizeof(WClassName));
  //Sprawdzenie klasy okna
  if((UnicodeString)WClassName=="TToolBar")
  {
	//Przypisanie uchwytu
	hToolBar = hWnd;
	return false;
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do: -kontrolki gdzie znajduja sie zakladki
//-przyciskow strzalek do przewijania zakladek (part II)
bool CALLBACK FindTabsBarEx(HWND hWnd, LPARAM)
{
  //Pobranie rozmiaru okna
  TRect WindowRect;
  GetWindowRect(hWnd,&WindowRect);
  //Uchwyt do kontrolki gdzie znajduja sie zakladki
  //Sprawdzenie szerokosci okna
  if(WindowRect.Width()>22)
  {
	//Gdy kontrolka nie ma dziecka
	if(!FindWindowEx(hWnd,NULL,L"TPanel",NULL))
	{
	  //Przypisanie uchwytu
	  hTabsBar = hWnd;
	}
  }
  //Uchwyty do przyciskow strzalek przewijania zakladek
  if((!hScrollTabButton[0])||(!hScrollTabButton[1]))
  {
	//Sprawdzenie szerokosci+wysokosci okna
	if((WindowRect.Width()==20)&&(WindowRect.Height()==27))
	{
	  //Przypisanie uchwytu #1
	  if(!hScrollTabButton[0])
	   hScrollTabButton[0] = hWnd;
	  //Przypisanie uchwytu #2
	  else if(!hScrollTabButton[1])
	   hScrollTabButton[1] = hWnd;
	  //Ukrywanie/pokazywanie przyciskow do przewijania zakladek
	  CheckHideScrollTabButtons();
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do: -kontrolki gdzie znajduja sie zakladki
//-przycisku z lista aktywnych zakladek
//-przyciskow strzalek do przewijania zakladek (part I)
bool CALLBACK FindTabsBar(HWND hWnd, LPARAM)
{
  //Pobranie klasy okna
  wchar_t WClassName[128];
  GetClassNameW(hWnd, WClassName, sizeof(WClassName));
  //Sprawdenie klasy okna
  if((UnicodeString)WClassName=="TsPanel")
  {
	//Szukanie kontrolki dziecka - paska narzedzi
	HWND hTempHwnd = FindWindowEx(hWnd,NULL,L"TToolBar",NULL);
	//Porownanie kontrolek
	if(hTempHwnd)
	{
	  //Pobranie wlasciwego dziecka
	  hTempHwnd = FindWindowEx(hWnd,NULL,L"TPanel",NULL);
	  //Uruchomienie drugiej czesci szukania uchwytow
	  EnumChildWindows(hTempHwnd,(WNDENUMPROC)FindTabsBarEx,0);
	  return false;
	}
  }
  return true;
}
//---------------------------------------------------------------------------

//Konwersja ciagu znakow na potrzeby INI
UnicodeString StrToIniStr(UnicodeString Str)
{
  //Definicja zmiennych
  wchar_t Buffer[50010];
  wchar_t* B;
  wchar_t* S;
  //Przekazywanie ciagu znakow
  S = Str.w_str();
  //Ustalanie wskaznika
  B = Buffer;
  //Konwersja znakow
  while(*S!='\0')
  {
	switch(*S)
	{
	  case 13:
	  case 10:
		if((*S==13)&&(S[1]==10)) S++;
		else if((*S==10)&&(S[1] == 13)) S++;
		*B = '\\';
		B++;
		*B = 'n';
		B++;
		S++;
	  break;
	  default:
		*B = *S;
		B++;
		S++;
	  break;
	}
  }
  *B = '\0';
  //Zwracanie zkonwertowanego ciagu znakow
  return (wchar_t*)Buffer;
}
//---------------------------------------------------------------------------
UnicodeString IniStrToStr(UnicodeString Str)
{
  //Definicja zmiennych
  wchar_t Buffer[50010];
  wchar_t* B;
  wchar_t* S;
  //Przekazywanie ciagu znakow
  S = Str.w_str();
  //Ustalanie wskaznika
  B = Buffer;
  //Konwersja znakow
  while(*S!='\0')
  {
	if((S[0]=='\\')&&(S[1]=='n'))
	{
	  *B = 13;
	  B++;
	  *B = 10;
	  B++;
	  S++;
	  S++;
	}
	else
	{
	  *B = *S;
	  B++;
	  S++;
	}
  }
  *B = '\0';
  //Zwracanie zkonwertowanego ciagu znakow
  return (wchar_t*)Buffer;
}
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego wtyczek
UnicodeString GetPluginUserDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------
UnicodeString GetPluginUserDirW()
{
  return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do katalogu profilu
UnicodeString GetUserDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,(WPARAM)(HInstance),0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do katalogu aplikacji
UnicodeString GetApplicationPath()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETAPPPATH,(WPARAM)(HInstance),0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do kompozycji
UnicodeString GetThemeDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeSkinDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
}
//---------------------------------------------------------------------------

//Pobieranie sciezko do katalogu zawierajacego informacje o kontaktach
UnicodeString GetContactsUserDir()
{
  return StringReplace((wchar_t *)PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Data\\\\Contacts\\\\";
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do pliku DLL wtyczki
UnicodeString GetPluginDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(HInstance),0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Sprawdzanie czy  wlaczona jest zaawansowana stylizacja okien
bool ChkSkinEnabled()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString SkinsEnabled = Settings->ReadString("Settings","UseSkin","1");
  delete Settings;
  return StrToBool(SkinsEnabled);
}
//---------------------------------------------------------------------------

//Sprawdzanie ustawien animacji AlphaControls
bool ChkThemeAnimateWindows()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString AnimateWindowsEnabled = Settings->ReadString("Theme","ThemeAnimateWindows","1");
  delete Settings;
  return StrToBool(AnimateWindowsEnabled);
}
//---------------------------------------------------------------------------
bool ChkThemeGlowing()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString GlowingEnabled = Settings->ReadString("Theme","ThemeGlowing","1");
  delete Settings;
  return StrToBool(GlowingEnabled);
}
//---------------------------------------------------------------------------

//Pobieranie ustawien koloru AlphaControls
int GetHUE()
{
  return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETHUE,0,0);
}
//---------------------------------------------------------------------------
int GetSaturation()
{
  return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETSATURATION,0,0);
}
//---------------------------------------------------------------------------

//Pobieranie informacji o pliku (wersja itp)
UnicodeString GetFileInfo(wchar_t *ModulePath, String KeyName)
{
  LPVOID lpStr1 = NULL, lpStr2 = NULL;
  WORD* wTmp;
  DWORD dwHandlev = NULL;
  UINT dwLength;
  wchar_t sFileName[1024] = {0};
  wchar_t sTmp[1024] = {0};
  UnicodeString sInfo;
  LPBYTE *pVersionInfo;

  if(ModulePath == NULL) GetModuleFileName( NULL, sFileName, 1024);
  else wcscpy(sFileName, ModulePath);

  DWORD dwInfoSize = GetFileVersionInfoSize(sFileName, &dwHandlev);

  if(dwInfoSize)
  {
	pVersionInfo = new LPBYTE[dwInfoSize];
	if(GetFileVersionInfo(sFileName, dwHandlev, dwInfoSize, pVersionInfo))
	{
	  if(VerQueryValue(pVersionInfo, L"\\VarFileInfo\\Translation", &lpStr1, &dwLength))
	  {
		wTmp = (WORD*)lpStr1;
		swprintf(sTmp, ("\\StringFileInfo\\%04x%04x\\" + KeyName).w_str(), *wTmp, *(wTmp + 1));
		if(VerQueryValue(pVersionInfo, sTmp, &lpStr2, &dwLength)) sInfo = (LPCTSTR)lpStr2;
	  }
	}
	delete[] pVersionInfo;
  }
  return sInfo;
}
//---------------------------------------------------------------------------

//Pobieranie pseudonimu kontaktu podajac jego JID
UnicodeString GetContactNick(UnicodeString JID)
{
  UnicodeString Nick = ContactsNickList->ReadString("Nick",JID,"");
  if(Nick.IsEmpty())
  {
	if(JID.Pos("@")) JID.Delete(JID.Pos("@"),JID.Length());
	return JID;
  }
  return Nick;
}
//---------------------------------------------------------------------------

//Pobieranie indeksu kontaktu podajac jego JID
int GetContactIndex(UnicodeString JID)
{
  if(JID.Pos("/")) JID = JID.Delete(JID.Pos("/"),JID.Length());
  return ContactsIndexList->ReadInteger("Index",JID,0);
}
//---------------------------------------------------------------------------
int GetContactIndexW(UnicodeString Conference)
{
  //Parsowanie nazwy serwera
  if(Conference.Pos("@")) Conference = Conference.Delete(1,Conference.Pos("@"));
  //Informacje na temat kont uzytkownika
  TPluginStateChange PluginStateChange;
  //Pobieranie ilosci kont
  int UserIdxCount = PluginLink.CallService(AQQ_FUNCTION_GETUSEREXCOUNT,0,0);
  //Pobieranie listy kont
  for(int UserIdx=0;UserIdx<UserIdxCount;UserIdx++)
  {
	//Parsowanie nazwy serwera
	PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),UserIdx);
	UnicodeString Account = PluginStateChange.JID;
	if(Account.Pos("@")) Account = Account.Delete(1,Account.Pos("@"));
	//Nazwa serwera zawarta w JID konta
	if(Conference.Pos(Account)) return PluginStateChange.UserIdx;
  }
  //Nieznany indeks
  return 0;
}
//---------------------------------------------------------------------------

//Pobieranie stanu kontaktu podajac jego JID
int GetContactState(UnicodeString JID)
{
  //Ikona bota Blip (gdy zakladka jest przypieta)
  if(((JID=="blip@blip.pl")||(JID.Pos("202@plugin.gg")==1))&&(!UnloadExecuted))
   if(ClipTabsList->IndexOf(JID)!=-1) return 132;
  //Ikona bota tweet.IM (gdy zakladka jest przypieta)
  if((JID.Pos("@twitter.tweet.im"))&&(!UnloadExecuted))
   if(ClipTabsList->IndexOf(JID)!=-1) return 131;
  //Pobranie stanu kontatu z listy stanow zbieranej przez wtyczke
  int State = ContactsStateList->ReadInteger("State",JID,-1);
  //Jezeli stan kontaktu nie jest zapisany
  if(State==-1)
  {
	//Pobranie domyslnej ikonki dla kontatku
	TPluginContact PluginContact;
	ZeroMemory(&PluginContact, sizeof(TPluginContact));
	PluginContact.cbSize = sizeof(TPluginContact);
	PluginContact.JID = JID.w_str();
	State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));
  }
  //Zwrocenie ikonki stanu kontatku
  return State;
}
//---------------------------------------------------------------------------

//Sprawdzanie listy ID filmikow YouTube do przetworzenia
bool ChkAvatarsListItem()
{
  if(GetYouTubeTitleList->Count) return true;
  else return false;
}
//---------------------------------------------------------------------------

//Pobieranie ID filmiku YouTube do przetworzenia
UnicodeString GetYouTubeTitleListItem()
{
  if(GetYouTubeTitleList->Count)
  {
	UnicodeString Item = GetYouTubeTitleList->Strings[0];
	GetYouTubeTitleList->Delete(0);
	return Item;
  }
  else return "";
}
//---------------------------------------------------------------------------

//Odswiezanie listy kontaktow
void RefreshList()
{
  PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aRefresh");
}
//---------------------------------------------------------------------------

//Normalizacja nazw kanalow
UnicodeString NormalizeChannel(UnicodeString Channel)
{
  return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_NORMALIZE,0,(LPARAM)Channel.w_str());
}
//---------------------------------------------------------------------------

//Pobieranie sciezki procesu
UnicodeString GetPathOfProces(HWND hWnd)
{
  //Pobieranie PID procesu
  DWORD procesID;
  GetWindowThreadProcessId(hWnd, &procesID);
  //Pobieranie sciezki procesu
  if(procesID)
  {
	MODULEENTRY32 lpModuleEntry = {0};
	HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, procesID );
	if(!hSnapShot) return "";
	lpModuleEntry.dwSize = sizeof(lpModuleEntry);
	if(Module32First(hSnapShot, &lpModuleEntry))
	{
	  CloseHandle(hSnapShot);
	  return lpModuleEntry.szExePath;
	}
	CloseHandle(hSnapShot);
	return "";
  }
  return "";
}
//---------------------------------------------------------------------------

//Usuwanie interfejsu dla ostatio zamknietych zakladek
void DestroyFrmClosedTabs()
{
  //Pobieranie ilosci zamknietych zakladek
  int TabsCount = ClosedTabsList->Count;
  //Maks X elementow w popupmenu
  if(TabsCount>ItemCountUnCloseTabVal) TabsCount = ItemCountUnCloseTabVal;
  //Jezeli sa jakies ostatnio zamkniete zakladki
  if(TabsCount>0)
  {
	//Usuwanie elementow popupmenu
	for(int Count=0;Count<TabsCount;Count++)
	{
	  TPluginAction DestroyClosedTabsItem;
	  ZeroMemory(&DestroyClosedTabsItem,sizeof(TPluginAction));
	  DestroyClosedTabsItem.cbSize = sizeof(TPluginAction);
	  DestroyClosedTabsItem.pszName = ("TabKitClosedTabsItem"+IntToStr(Count)).w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyClosedTabsItem));
	}
	//Usuwanie elementow do usuwania ostatnio zamknietych zakladek
	if(FastClearClosedTabsChk)
	{
	  TPluginAction DestroyClosedTabsItem;
	  ZeroMemory(&DestroyClosedTabsItem,sizeof(TPluginAction));
	  DestroyClosedTabsItem.pszName = ("TabKitClosedTabsItem"+IntToStr(TabsCount+1)).w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyClosedTabsItem));
	  ZeroMemory(&DestroyClosedTabsItem,sizeof(TPluginAction));
	  DestroyClosedTabsItem.pszName = ("TabKitClosedTabsItem"+IntToStr(TabsCount+2)).w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyClosedTabsItem));
	}
	//Usuwanie buttona w oknie kontaktow
	TPluginAction FrmMainClosedTabsButton;
	ZeroMemory(&FrmMainClosedTabsButton,sizeof(TPluginAction));
	FrmMainClosedTabsButton.cbSize = sizeof(TPluginAction);
	FrmMainClosedTabsButton.pszName = L"TabKitFrmMainClosedTabsButton";
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmMainClosedTabsButton));
	//Usuwanie buttona w oknie rozmowy
	TPluginAction FrmSendClosedTabsButton;
	ZeroMemory(&FrmSendClosedTabsButton,sizeof(TPluginAction));
	FrmSendClosedTabsButton.cbSize = sizeof(TPluginAction);
	FrmSendClosedTabsButton.pszName = L"TabKitFrmSendClosedTabsButton";
	FrmSendClosedTabsButton.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmSendClosedTabsButton));
  }
}
//---------------------------------------------------------------------------

//Tworzenie interfejsu dla ostatnio zamknietych zakladek
void BuildFrmClosedTabs()
{
  //Jezeli interfejs ma byc w ogole tworzony
  if((ClosedTabsChk)&&(FastAccessClosedTabsChk))
  {
	//Pobieranie ilosci zamknietych zakladek
	int TabsCount = ClosedTabsList->Count;
	//Maks X elementow w popupmenu
	if(TabsCount>ItemCountUnCloseTabVal) TabsCount = ItemCountUnCloseTabVal;
	//Jezeli sa jakies ostatnio zamkniete zakladki
	if(TabsCount>0)
	{
	  //Tworzenie buttona w oknie kontaktow
	  if(FrmMainClosedTabsChk)
	  {
		TPluginAction FrmMainClosedTabsButton;
		ZeroMemory(&FrmMainClosedTabsButton,sizeof(TPluginAction));
		FrmMainClosedTabsButton.cbSize = sizeof(TPluginAction);
		FrmMainClosedTabsButton.pszName = L"TabKitFrmMainClosedTabsButton";
		FrmMainClosedTabsButton.Position = 999;
		FrmMainClosedTabsButton.IconIndex = CLOSEDTABS;
		FrmMainClosedTabsButton.pszPopupName = L"TabKitClosedTabsPopUp";
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmMainClosedTabsButton));
	  }
	  //Tworzenie buttona w oknie rozmowy
	  if((hFrmSend)&&(FrmSendClosedTabsChk))
	  {
		TPluginAction FrmSendClosedTabsButton;
		ZeroMemory(&FrmSendClosedTabsButton,sizeof(TPluginAction));
		FrmSendClosedTabsButton.cbSize = sizeof(TPluginAction);
		FrmSendClosedTabsButton.pszName = L"TabKitFrmSendClosedTabsButton";
		FrmSendClosedTabsButton.Hint = L"Ostatnio zamkniête zak³adki";
		FrmSendClosedTabsButton.IconIndex = CLOSEDTABS;
		FrmSendClosedTabsButton.pszPopupName = L"TabKitClosedTabsPopUp";
		FrmSendClosedTabsButton.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmSendClosedTabsButton));
	  }
	  //Tworzenie PopUpMenuItems
	  for(int Count=0;Count<TabsCount;Count++)
	  {
		UnicodeString ItemJID = ClosedTabsList->Strings[Count];
		if(!ItemJID.IsEmpty())
		{
		  if(!ItemJID.Pos("ischat_"))
		  {
			TPluginAction BuildClosedTabsItem;
			ZeroMemory(&BuildClosedTabsItem,sizeof(TPluginAction));
			BuildClosedTabsItem.cbSize = sizeof(TPluginAction);
			BuildClosedTabsItem.IconIndex = GetContactState(ItemJID);
			UnicodeString pszName = "TabKitClosedTabsItem"+IntToStr(Count);
			BuildClosedTabsItem.pszName = pszName.w_str();
			UnicodeString pszService = "sTabKitClosedTabsItem"+IntToStr(Count);
			BuildClosedTabsItem.pszService = pszService.w_str();
			if(ShowTimeClosedTabsChk)
			 BuildClosedTabsItem.pszCaption = (GetContactNick(ItemJID)+" ("+ClosedTabsTimeList->Strings[Count]+")").w_str();
			else
			 BuildClosedTabsItem.pszCaption = GetContactNick(ItemJID).w_str();
			BuildClosedTabsItem.Position = Count;
			BuildClosedTabsItem.pszPopupName = L"TabKitClosedTabsPopUp";
			PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildClosedTabsItem));
		  }
		  else
		  {
			ItemJID = ItemJID.Delete(1,7);
			TIniFile *Ini = new TIniFile(SessionFileDir);
			UnicodeString Channel = Ini->ReadString("Channels",ItemJID,"");
			delete Ini;
			if(Channel.IsEmpty())
			{
			  Channel = ItemJID;
			  Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
			}
			TPluginAction BuildClosedTabsItem;
			ZeroMemory(&BuildClosedTabsItem,sizeof(TPluginAction));
			BuildClosedTabsItem.cbSize = sizeof(TPluginAction);
			BuildClosedTabsItem.IconIndex = 79;
			UnicodeString pszName = "TabKitClosedTabsItem"+IntToStr(Count);
			BuildClosedTabsItem.pszName = pszName.w_str();
			UnicodeString pszService = "sTabKitClosedTabsItem"+IntToStr(Count);
			BuildClosedTabsItem.pszService = pszService.w_str();
			if(ShowTimeClosedTabsChk)
			 BuildClosedTabsItem.pszCaption = (Channel+" ("+ClosedTabsTimeList->Strings[Count]+")").w_str();
			else
			 BuildClosedTabsItem.pszCaption = Channel.w_str();
			BuildClosedTabsItem.Position = Count;
			BuildClosedTabsItem.pszPopupName = L"TabKitClosedTabsPopUp";
			PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildClosedTabsItem));
          }
		}
	  }
	  //Tworzenie elementow do usuwania ostatnio zamknietych zakladek
	  if(FastClearClosedTabsChk)
	  {
		//Tworzenie separatora
		TPluginAction BuildClosedTabsItem;
		ZeroMemory(&BuildClosedTabsItem,sizeof(TPluginAction));
		BuildClosedTabsItem.cbSize = sizeof(TPluginAction);
		BuildClosedTabsItem.IconIndex = -1;
		BuildClosedTabsItem.pszName = ("TabKitClosedTabsItem"+IntToStr(TabsCount+1)).w_str();
		BuildClosedTabsItem.pszService = L"";
		BuildClosedTabsItem.pszCaption = L"-";
		BuildClosedTabsItem.Position = TabsCount+1;
		BuildClosedTabsItem.pszPopupName = L"TabKitClosedTabsPopUp";
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildClosedTabsItem));
		//Tworzenie elementu czyszczenia
		ZeroMemory(&BuildClosedTabsItem,sizeof(TPluginAction));
		BuildClosedTabsItem.cbSize = sizeof(TPluginAction);
		BuildClosedTabsItem.IconIndex = -1;
		BuildClosedTabsItem.pszName = ("TabKitClosedTabsItem"+IntToStr(TabsCount+2)).w_str();
		BuildClosedTabsItem.pszService = L"sTabKitClosedTabsItemClear";
		BuildClosedTabsItem.pszCaption = L"Wyczyœæ";
		BuildClosedTabsItem.Position = TabsCount+1;
		BuildClosedTabsItem.pszPopupName = L"TabKitClosedTabsPopUp";
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildClosedTabsItem));
	  }
	}
  }
  //Usuwanie interfejsu dla ostatio zamknietych zakladek
  else
   DestroyFrmClosedTabs();
}
//---------------------------------------------------------------------------

//Tworzenie interfejsu dla ostatnio zamknietych zakladek w funkcjonalnosci AQQ
void BuildAQQClosedTabs()
{
   //Jezeli interfejs ma byc w ogole tworzony
  if(ClosedTabsChk)
  {
	//Pobieranie ilosci zamknietych zakladek
	int TabsCount = ClosedTabsList->Count;
	//Maks 3 zdefiniowanych elementów
	if(TabsCount>3) TabsCount = 3;
	//Jezeli sa jakies ostatnio zamkniete zakladki
	if(TabsCount>0)
	{
	  UnicodeString JID;
	  for(TabsCount;TabsCount>0;TabsCount--)
	  {
		JID = ClosedTabsList->Strings[TabsCount-1];
		if((!JID.IsEmpty())&&(!JID.Pos("ischat_")))
		{
		  PluginLink.CallService(AQQ_FUNCTION_TABWASCLOSED,0,(LPARAM)JID.w_str());
		}
	  }
	}
  }
}
//---------------------------------------------------------------------------

//Pobieranie ostatnio zamknietych zakladek do listy
void GetClosedTabs()
{
  //Jezeli funkcjonalnosc jest wlaczona
  if(ClosedTabsChk)
  {
	//Odczyt danych z pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *ClosedTabs = new TStringList;
	Ini->ReadSection("ClosedTabs",ClosedTabs);
	//Pobieranie ilosci zamknietych zakladek
	int TabsCount = ClosedTabs->Count;
	delete ClosedTabs;
	//Maks X zdefiniowanych elementów
	if(TabsCount>CountUnCloseTabVal) TabsCount = CountUnCloseTabVal;
	//Jezeli sa jakies ostatnio zamkniete zakladki
	if(TabsCount>0)
	{
      //Usuwanie listy
	  ClosedTabsList->Clear();
	  ClosedTabsTimeList->Clear();
	  //Wczytywanie pozycji na listy
	  for(int Count=0;Count<TabsCount;Count++)
	  {
		UnicodeString JID = Ini->ReadString("ClosedTabs","Tab"+IntToStr(Count+1),"");
		if(!JID.IsEmpty())
		{
		  //Jezeli zakladka z kontaktem nie jest otwarta
		  if(TabsList->IndexOf(JID)==-1)
		  {
			ClosedTabsList->Add(JID);
			UnicodeString ClosedTime = Ini->ReadString("ClosedTabs","Tab"+IntToStr(Count+1)+"Time","b/d");
			ClosedTabsTimeList->Add(ClosedTime);
		  }
		  //Usuwanie zakladki z listy
		  else
		  {
			Ini->DeleteKey("ClosedTabs","Tab"+IntToStr(Count+1));
			Ini->DeleteKey("ClosedTabs","Tab"+IntToStr(Count+1)+"Time");
          }
		}
	  }
	}
	//Usuwanie list
	else
	{
	  ClosedTabsList->Clear();
	  ClosedTabsTimeList->Clear();
	}
	delete Ini;
  }
}
//---------------------------------------------------------------------------

//Zapisywanie ostatnio zamknietych zakladek do pliku
void SaveClosedTabs()
{
  //Jezeli funkcjonalnosc jest wlaczona
  if(ClosedTabsChk)
  {
	//Usuniecie danych z pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	Ini->EraseSection("ClosedTabs");
	//Pobieranie ilosci zamknietych zakladek
	int TabsCount = ClosedTabsList->Count;
	//Maks X zdefiniowanych elementów
	if(TabsCount>CountUnCloseTabVal) TabsCount = CountUnCloseTabVal;
    //Jezeli sa jakies ostatnio zamkniete zakladki
	if(TabsCount>0)
	{
	  for(int Count=0;Count<TabsCount;Count++)
	  {
		//Jezeli pole z JID nie jest puste
		if(!ClosedTabsList->Strings[Count].IsEmpty())
		{
		  //Zapis danych do pliku sesji
		  Ini->WriteString("ClosedTabs","Tab"+IntToStr(Count+1),ClosedTabsList->Strings[Count]);
		  //Jezeli pole z data zamkniecia jest puste
		  if(ClosedTabsTimeList->Strings[Count].IsEmpty()) ClosedTabsTimeList->Strings[Count] = "b/d";
		  //Zapis danych do pliku sesji
		  Ini->WriteString("ClosedTabs","Tab"+IntToStr(Count+1)+"Time",ClosedTabsTimeList->Strings[Count]);
		}
	  }
	}
	delete Ini;
	//Pobieranie ostatnio zamknietych zakladek do list
	GetClosedTabs();
  }
}
//---------------------------------------------------------------------------

//Usuwanie listy ostatnio zamknietych zakladek
void EraseClosedTabs()
{
  //Usuwanie interfejsu
  DestroyFrmClosedTabs();
  //Usuwanie danych z pliku sesji
  TIniFile *Ini = new TIniFile(SessionFileDir);
  Ini->EraseSection("ClosedTabs");
  delete Ini;
  //Czyszczenie listy
  ClosedTabsList->Clear();
  ClosedTabsTimeList->Clear();
}
//---------------------------------------------------------------------------

//Otwieranie zostatnio zamknietej zakladki
void GetClosedTabsItem(int Item)
{
  //Pobranie JID zakladki do przywrocenia
  UnicodeString JID = ClosedTabsList->Strings[Item];
  //Zapisywanie JID aktualnie przywracanej zakladki
  JustUnClosedJID = JID;
  //Otwieranie zakladki ze zwyklym kontektem
  if(!JID.Pos("ischat_"))
  {
	if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
	else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
  }
  //Otwieranie zakladki z czatem
  else
  {
	//Ustawianie prawidlowego identyfikatora
	JID = JID.Delete(1,7);
	//Pobieranie nazwy kanalu
	TIniFile *Ini = new TIniFile(SessionFileDir);
	UnicodeString Channel = Ini->ReadString("Channels",JID,"");
	delete Ini;
	if(Channel.IsEmpty())
	{
	  Channel = JID;
	  Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
	}
	//Wypenianie struktury nt. czatu
	TPluginChatPrep PluginChatPrep;
	PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
	PluginChatPrep.UserIdx = GetContactIndexW(JID);
	PluginChatPrep.JID = JID.w_str();
	PluginChatPrep.Channel = Channel.w_str();
	PluginChatPrep.CreateNew = false;
	PluginChatPrep.Fast = true;
	//Przywracanie zakladki czatowej
	PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
  }
}
//---------------------------------------------------------------------------

//Otwieranie ponownie ostatnio zamknietej zakladki
void UnCloseTabHotKeyExecute()
{
  //Status sposobu przywracania zakladki
  UnCloseTabFromHotKey = true;
  //Pobranie JID zakladki do przywrocenia
  UnicodeString JID = ClosedTabsList->Strings[0];
  //Zapisawanie JID aktualnie przywracanej zakladki
  JustUnClosedJID = JID;
  //Przelaczanie na ostatnio zamknieta zakladke ze zwyklym kontaktem
  if(!JID.Pos("ischat_"))
  {
	if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
	else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
  }
  //Przelaczanie na ostatnio zamknieta zakladke z czatem
  else
  {
	//Ustawianie prawidlowego identyfikatora
	JID = JID.Delete(1,7);
	//Pobieranie nazwy kanalu
	TIniFile *Ini = new TIniFile(SessionFileDir);
	UnicodeString Channel = Ini->ReadString("Channels",JID,"");
	delete Ini;
	if(Channel.IsEmpty())
	{
	  Channel = JID;
	  Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
	}
	//Wypenianie struktury nt. czatu
	TPluginChatPrep PluginChatPrep;
	PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
	PluginChatPrep.UserIdx = GetContactIndexW(JID);
	PluginChatPrep.JID = JID.w_str();
	PluginChatPrep.Channel = Channel.w_str();
	PluginChatPrep.CreateNew = false;
	PluginChatPrep.Fast = true;
	//Przywracanie zakladki czatowej
	PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
  }
}
//---------------------------------------------------------------------------

//Serwisy elementow ostatnio zamknietych zakladek
int __stdcall ServiceClosedTabsItemClear(WPARAM wParam, LPARAM lParam)
{
  EraseClosedTabs();
  return 0;
}
int __stdcall ServiceClosedTabsItem0(WPARAM wParam, LPARAM lParam)
{
  GetClosedTabsItem(0);
  return 0;
}
int __stdcall ServiceClosedTabsItem1(WPARAM wParam, LPARAM lParam)
{
  GetClosedTabsItem(1);
  return 0;
}
int __stdcall ServiceClosedTabsItem2(WPARAM wParam, LPARAM lParam)
{
  GetClosedTabsItem(2);
  return 0;
}
int __stdcall ServiceClosedTabsItem3(WPARAM wParam, LPARAM lParam)
{
  GetClosedTabsItem(3);
  return 0;
}
int __stdcall ServiceClosedTabsItem4(WPARAM wParam, LPARAM lParam)
{
  GetClosedTabsItem(4);
  return 0;
}
int __stdcall ServiceClosedTabsItem5(WPARAM wParam, LPARAM lParam)
{
  GetClosedTabsItem(5);
  return 0;
}
int __stdcall ServiceClosedTabsItem6(WPARAM wParam, LPARAM lParam)
{
  GetClosedTabsItem(6);
  return 0;
}
int __stdcall ServiceClosedTabsItem7(WPARAM wParam, LPARAM lParam)
{
  GetClosedTabsItem(7);
  return 0;
}
int __stdcall ServiceClosedTabsItem8(WPARAM wParam, LPARAM lParam)
{
  GetClosedTabsItem(8);
  return 0;
}
int __stdcall ServiceClosedTabsItem9(WPARAM wParam, LPARAM lParam)
{
  GetClosedTabsItem(9);
  return 0;
}
//---------------------------------------------------------------------------

//Usuwanie szybkiego dostepu do niewyslanych wiadomosci
void DestroyFrmUnsentMsg()
{
  //Pobieranie ilosci zamknietych zakladek
  TIniFile *Ini = new TIniFile(SessionFileDir);
  TStringList *Messages = new TStringList;
  Ini->ReadSection("Messages",Messages);
  delete Ini;
  //Pobieranie ilosci niewyslanych wiadomosci
  int MsgCount = Messages->Count;
  delete Messages;
  //Maks 5 elementow w popupmenu
  if(MsgCount>5) MsgCount = 5;
  //Jezeli sa jakies zakladki z niewyslanymi wiadomosciami
  if(MsgCount>0)
  {
	//Usuwanie elementow popupmenu
	for(int Count=0;Count<MsgCount;Count++)
	{
	  TPluginAction DestroyUnsentMsgItem;
	  ZeroMemory(&DestroyUnsentMsgItem,sizeof(TPluginAction));
	  DestroyUnsentMsgItem.cbSize = sizeof(TPluginAction);
	  DestroyUnsentMsgItem.pszName = ("TabKitUnsentMsgItem"+IntToStr(Count)).w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyUnsentMsgItem));
	}
	//Usuwanie elementow do usuwania niewyslanych wiadomosci
	if(FastClearUnsentMsgChk)
	{
	  TPluginAction DestroyUnsentMsgItem;
	  ZeroMemory(&DestroyUnsentMsgItem,sizeof(TPluginAction));
	  DestroyUnsentMsgItem.cbSize = sizeof(TPluginAction);
	  DestroyUnsentMsgItem.pszName = ("TabKitUnsentMsgItem"+IntToStr(MsgCount+1)).w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyUnsentMsgItem));
	  ZeroMemory(&DestroyUnsentMsgItem,sizeof(TPluginAction));
	  DestroyUnsentMsgItem.cbSize = sizeof(TPluginAction);
	  DestroyUnsentMsgItem.pszName = ("TabKitUnsentMsgItem"+IntToStr(MsgCount+2)).w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyUnsentMsgItem));
	}
	//Usuwanie buttona w oknie kontatkow
	TPluginAction FrmMainUnsentMsgButton;
	ZeroMemory(&FrmMainUnsentMsgButton,sizeof(TPluginAction));
	FrmMainUnsentMsgButton.cbSize = sizeof(TPluginAction);
	FrmMainUnsentMsgButton.pszName = L"TabKitFrmMainUnsentMsgButton";
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmMainUnsentMsgButton));
	//Usuwanie buttona w oknie rozmowy
	TPluginAction FrmSendUnsentMsgButton;
	ZeroMemory(&FrmSendUnsentMsgButton,sizeof(TPluginAction));
	FrmSendUnsentMsgButton.cbSize = sizeof(TPluginAction);
	FrmSendUnsentMsgButton.pszName = L"TabKitFrmSendUnsentMsgButton";
	FrmSendUnsentMsgButton.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmSendUnsentMsgButton));
  }
}
//---------------------------------------------------------------------------

//Tworzenie szybkiego dostepu do niewyslanych wiadomosci
void BuildFrmUnsentMsg()
{
  //Jezeli interfejs ma byc w ogole tworzony
  if((UnsentMsgChk)&&(FastAccessUnsentMsgChk))
  {
	//Pobieranie ilosci zamknietych zakladek
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *Messages = new TStringList;
	Ini->ReadSection("Messages",Messages);
	delete Ini;
	//Pobieranie ilosci niewyslanych wiadomosci
	int MsgCount = Messages->Count;
	//Maks 5 elementow w popupmenu
	if(MsgCount>5) MsgCount = 5;
	//Jezeli sa jakies zakladki z niewyslanymi wiadomosciami
	if(MsgCount>0)
	{
	  //Tworzenie buttona w oknie kontatkow
	  if(FrmMainUnsentMsgChk)
	  {
		TPluginAction FrmMainUnsentMsgButton;
		ZeroMemory(&FrmMainUnsentMsgButton,sizeof(TPluginAction));
		FrmMainUnsentMsgButton.cbSize = sizeof(TPluginAction);
		FrmMainUnsentMsgButton.pszName = L"TabKitFrmMainUnsentMsgButton";
		FrmMainUnsentMsgButton.Position = 999;
		FrmMainUnsentMsgButton.IconIndex = UNSENTMSG;
		FrmMainUnsentMsgButton.pszPopupName = L"TabKitUnsentMsgPopUp";
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmMainUnsentMsgButton));
	  }
	  //Tworzenie buttona w oknie rozmowy
	  if((hFrmSend)&&(FrmSendUnsentMsgChk))
	  {
		TPluginAction FrmSendUnsentMsgButton;
		ZeroMemory(&FrmSendUnsentMsgButton,sizeof(TPluginAction));
		FrmSendUnsentMsgButton.cbSize = sizeof(TPluginAction);
		FrmSendUnsentMsgButton.pszName = L"TabKitFrmSendUnsentMsgButton";
		FrmSendUnsentMsgButton.Hint = L"Niewys³ane wiadomoœci";
		FrmSendUnsentMsgButton.IconIndex = UNSENTMSG;
		FrmSendUnsentMsgButton.pszPopupName = L"TabKitUnsentMsgPopUp";
		FrmSendUnsentMsgButton.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmSendUnsentMsgButton));
	  }
	  //Tworzenie PopUpMenuItems
	  for(int Count=0;Count<MsgCount;Count++)
	  {
		UnicodeString ItemJID = Messages->Strings[Count];
		if(!ItemJID.IsEmpty())
		{
		  if(!ItemJID.Pos("ischat_"))
		  {
			TPluginAction BuildUnsentMsgItem;
			ZeroMemory(&BuildUnsentMsgItem,sizeof(TPluginAction));
			BuildUnsentMsgItem.cbSize = sizeof(TPluginAction);
			BuildUnsentMsgItem.IconIndex = GetContactState(ItemJID);
			UnicodeString ItemName = "TabKitUnsentMsgItem"+IntToStr(Count);
			BuildUnsentMsgItem.pszName = ItemName.w_str();
			UnicodeString ItemService = "sTabKitUnsentMsgItem"+IntToStr(Count);
			BuildUnsentMsgItem.pszService = ItemService.w_str();
			BuildUnsentMsgItem.pszCaption = GetContactNick(ItemJID).w_str();
			BuildUnsentMsgItem.Position = Count;
			BuildUnsentMsgItem.pszPopupName = L"TabKitUnsentMsgPopUp";
			PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildUnsentMsgItem));
		  }
		  else
		  {
			ItemJID = ItemJID.Delete(1,7);
			TIniFile *Ini = new TIniFile(SessionFileDir);
			UnicodeString Channel = Ini->ReadString("Channels",ItemJID,"");
			delete Ini;
			if(Channel.IsEmpty())
			{
			  Channel = ItemJID;
			  Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
			}
			TPluginAction BuildUnsentMsgItem;
			ZeroMemory(&BuildUnsentMsgItem,sizeof(TPluginAction));
			BuildUnsentMsgItem.cbSize = sizeof(TPluginAction);
			BuildUnsentMsgItem.IconIndex = 79;
			UnicodeString ItemName = "TabKitUnsentMsgItem"+IntToStr(Count);
			BuildUnsentMsgItem.pszName = ItemName.w_str();
			UnicodeString ItemService = "sTabKitUnsentMsgItem"+IntToStr(Count);
			BuildUnsentMsgItem.pszService = ItemService.w_str();
			BuildUnsentMsgItem.pszCaption = Channel.w_str();
			BuildUnsentMsgItem.Position = Count;
			BuildUnsentMsgItem.pszPopupName = L"TabKitUnsentMsgPopUp";
			PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildUnsentMsgItem));
		  }
		}
	  }
	  //Tworzenie elementow do usuwania niewyslanych wiadomosci
	  if(FastClearUnsentMsgChk)
	  {
		//Tworzenie separatora
		TPluginAction BuildUnsentMsgItem;
		ZeroMemory(&BuildUnsentMsgItem,sizeof(TPluginAction));
		BuildUnsentMsgItem.cbSize = sizeof(TPluginAction);
		BuildUnsentMsgItem.IconIndex = -1;
		BuildUnsentMsgItem.pszName = ("TabKitUnsentMsgItem"+IntToStr(MsgCount+1)).w_str();
		BuildUnsentMsgItem.pszService = L"";
		BuildUnsentMsgItem.pszCaption = L"-";
		BuildUnsentMsgItem.Position = MsgCount+1;
		BuildUnsentMsgItem.pszPopupName = L"TabKitUnsentMsgPopUp";
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildUnsentMsgItem));
		//Tworzenie elementu czyszczenia
		ZeroMemory(&BuildUnsentMsgItem,sizeof(TPluginAction));
		BuildUnsentMsgItem.cbSize = sizeof(TPluginAction);
		BuildUnsentMsgItem.IconIndex = -1;
		BuildUnsentMsgItem.pszName = ("TabKitUnsentMsgItem"+IntToStr(MsgCount+2)).w_str();
		BuildUnsentMsgItem.pszService = L"sTabKitUnsentMsgItemClear";
		BuildUnsentMsgItem.pszCaption = L"Wyczyœæ";
		BuildUnsentMsgItem.Position = MsgCount+1;
		BuildUnsentMsgItem.pszPopupName = L"TabKitUnsentMsgPopUp";
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildUnsentMsgItem));
	  }
	}
	delete Messages;
  }
  //Usuwanie szybkiego dostepu do niewyslanych wiadomosci
  else
   DestroyFrmUnsentMsg();
}
//---------------------------------------------------------------------------

//Usuwanie listy niewyslanych wiadomosci
void EraseUnsentMsg()
{
  //Usuwanie interfejsu
  DestroyFrmUnsentMsg();
  //Usuwanie sesji z pliku
  TIniFile *Ini = new TIniFile(SessionFileDir);
  Ini->EraseSection("Messages");
  delete Ini;
}
//---------------------------------------------------------------------------

//Otwieranie zakladki z niewyslana wiadomoscia
void GetUnsentMsgItem(int Item)
{
  //Odczyt danych z pliku sesji
  TIniFile *Ini = new TIniFile(SessionFileDir);
  TStringList *Messages = new TStringList;
  Ini->ReadSection("Messages",Messages);
  delete Ini;
  UnicodeString JID = Messages->Strings[Item];
  delete Messages;
  //Otwieranie zakladki ze zwyklym kontaktem
  if(!JID.Pos("ischat_"))
  {
    if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
	else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
  }
  //Otwieranie zakladki z czatem
  else
  {
	//Ustawianie prawidlowego identyfikatora
	JID = JID.Delete(1,7);
	//Pobieranie nazwy kanalu
	TIniFile *Ini = new TIniFile(SessionFileDir);
	UnicodeString Channel = Ini->ReadString("Channels",JID,"");
	delete Ini;
	if(Channel.IsEmpty())
	{
	  Channel = JID;
	  Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
	}
	//Wypenianie struktury nt. czatu
	TPluginChatPrep PluginChatPrep;
	PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
	PluginChatPrep.UserIdx = GetContactIndexW(JID);
	PluginChatPrep.JID = JID.w_str();
	PluginChatPrep.Channel = Channel.w_str();
	PluginChatPrep.CreateNew = false;
	PluginChatPrep.Fast = true;
	//Przywracanie zakladki czatowej
	PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
  }
}
//---------------------------------------------------------------------------

//Serwisy elementow niewysylanych wiadomosci
int __stdcall ServiceUnsentMsgItemClear (WPARAM wParam, LPARAM lParam)
{
  EraseUnsentMsg();
  return 0;
}
int __stdcall ServiceUnsentMsgItem0 (WPARAM wParam, LPARAM lParam)
{
  GetUnsentMsgItem(0);
  return 0;
}
int __stdcall ServiceUnsentMsgItem1 (WPARAM wParam, LPARAM lParam)
{
  GetUnsentMsgItem(1);
  return 0;
}
int __stdcall ServiceUnsentMsgItem2 (WPARAM wParam, LPARAM lParam)
{
  GetUnsentMsgItem(2);
  return 0;
}
int __stdcall ServiceUnsentMsgItem3 (WPARAM wParam, LPARAM lParam)
{
  GetUnsentMsgItem(3);
  return 0;
}
int __stdcall ServiceUnsentMsgItem4 (WPARAM wParam, LPARAM lParam)
{
  GetUnsentMsgItem(4);
  return 0;
}
//---------------------------------------------------------------------------

//Sprawdzanie niewyslanych wiadomosci
void GetUnsentMsg()
{
  //Jezeli funkcjonalnosc jest wlaczona
  if((UnsentMsgChk)&&(InfoUnsentMsgChk))
  {
	//Odczyt danych z pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *Messages = new TStringList;
	Ini->ReadSection("Messages",Messages);
	//Pobieranie ilosci niewyslanych wiadomosci
	int MsgCount = Messages->Count;
	//Jezeli sa jakies zakladki z niewyslanymi wiadomosciami
	if(MsgCount>0)
	{
	  //Zadeklarowanie potrzebnych zmiennych
	  UnicodeString JID;
	  UnicodeString Hint;
	  UnicodeString Body;
	  //Ustawianie informacji o ilosci niewyslanych wiadomosci
	  if(MsgCount==1)
		Hint = "Masz 1 niewys³an¹ wiadomoœæ!";
	  else if(MsgCount>1&&MsgCount<5)
		Hint = "Masz " + IntToStr(MsgCount) + " niewys³ane wiadomoœci!";
	  else
	   Hint = "Masz " + IntToStr(MsgCount) + " niewys³anych wiadomoœci!";
	  //Informacja w chmurce
	  if(CloudUnsentMsgChk)
	  {
		//Informacja podstawowa
		TPluginShowInfo PluginShowInfo;
		PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
		PluginShowInfo.Event = tmeInfo;
		PluginShowInfo.Text = Hint.w_str();
		PluginShowInfo.ImagePath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,21,0);
		PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
		PluginShowInfo.ActionID = L"sTabKitUnsentMsgShowAllItem";
		PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
		//Szczegolowa informacja
		if(DetailedCloudUnsentMsgChk)
		{
		  for(int Count=0;Count<MsgCount;Count++)
		  {
			JID = Messages->Strings[Count];
			Body = UTF8ToUnicodeString((IniStrToStr(Ini->ReadString("Messages", JID, ""))).w_str());
			if(Body.Length()>25)
			 Body = Body.SetLength(25) + "...";
			PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			PluginShowInfo.Event = tmeInfo;
			if(!JID.Pos("ischat_"))
			 PluginShowInfo.Text = (GetContactNick(JID) + "\r\n" + Body).w_str();
			else
			{
			  JID = JID.Delete(1,7);
			  TIniFile *Ini = new TIniFile(SessionFileDir);
			  UnicodeString Channel = Ini->ReadString("Channels",JID,"");
			  delete Ini;
			  if(Channel.IsEmpty())
			  {
				Channel = JID;
				Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
			  }
			  PluginShowInfo.Text = (Channel + "\r\n" + Body).w_str();
            }
			PluginShowInfo.ImagePath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0);
			PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
			PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
		  }
		}
	  }
	  //Informacja w trayu
	  if(TrayUnsentMsgChk)
	  {
		//Przypisanie uchwytu do formy ustawien
		if(!hSettingsForm)
		{
		  Application->Handle = (HWND)SettingsForm;
		  hSettingsForm = new TSettingsForm(Application);
		}
		//Pobieranie ikonki
		if(!FileExists(GetThemeDir() + "\\\\TabKit\\\\UnsentMsg.ico"))
		 hSettingsForm->UnsentMsgTrayIcon->Icon->LoadFromFile(PluginUserDir + "\\\\TabKit\\\\UnsentMsg.ico");
		else
		 hSettingsForm->UnsentMsgTrayIcon->Icon->LoadFromFile(GetThemeDir() + "\\\\TabKit\\\\UnsentMsg.ico");
		//Pokazanie ikonki w obszarze powiadomien
		hSettingsForm->UnsentMsgTrayIcon->Hint = Hint;
		hSettingsForm->UnsentMsgTrayIcon->Visible = true;
	  }
	}
	delete Messages;
	delete Ini;
  }
}
//---------------------------------------------------------------------------

//Otwieranie wszystkich niewyslanych wiadomosci
bool ShowUnsentMsg()
{
  //Jesli komunikator nie jest zabezpieczony
  if(!SecureMode)
  {
	//Odczyt danych z pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *Messages = new TStringList;
	Ini->ReadSection("Messages",Messages);
	delete Ini;
	//Pobieranie ilosci niewyslanych wiadomosci
	int MsgCount = Messages->Count;
	//Jezeli sa jakies niewyslane wiadomosci
	if(MsgCount>0)
	{
	  UnicodeString JID;
	  for(int Count=0;Count<MsgCount;Count++)
	  {
		JID = Messages->Strings[Count];
		//Otwieranie zakladki z danym kontektem
		if(!JID.Pos("ischat_"))
		{
		  if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
		  else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
		}
		//Otwieranie zakladki z czatem
		else
		{
		  //Ustawianie prawidlowego identyfikatora
		  JID = JID.Delete(1,7);
		  //Pobieranie nazwy kanalu
		  TIniFile *Ini = new TIniFile(SessionFileDir);
		  UnicodeString Channel = Ini->ReadString("Channels",JID,"");
		  delete Ini;
		  if(Channel.IsEmpty())
		  {
			Channel = JID;
			Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
		  }
		  //Wypenianie struktury nt. czatu
		  TPluginChatPrep PluginChatPrep;
		  PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
		  PluginChatPrep.UserIdx = GetContactIndexW(JID);
		  PluginChatPrep.JID = JID.w_str();
		  PluginChatPrep.Channel = Channel.w_str();
		  PluginChatPrep.CreateNew = false;
		  PluginChatPrep.Fast = true;
		  //Przywracanie zakladki czatowej
		  PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
		}
	  }
	}
	delete Messages;
	return true;
  }
  //Jesli komunikator jest zabezpieczony
  else
  {
	PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aSecure");
	return false;
  }
}
//---------------------------------------------------------------------------

//Otwieranie wszystkich niewyslanych wiadomosci
int __stdcall ServiceUnsentMsgShowAllItem (WPARAM wParam, LPARAM lParam)
{
  //Otwarcie okna rozmowy wraz z niewyslanymi wiadomosciami
  if(ShowUnsentMsg())
  {
	//Przypisanie uchwytu do formy ustawien
	if(!hSettingsForm)
	{
	  Application->Handle = (HWND)SettingsForm;
	  hSettingsForm = new TSettingsForm(Application);
	}
	//Ukrycie ikonki w obszarze powiadomien
	hSettingsForm->UnsentMsgTrayIcon->Visible = false;
  }

  return 0;
}
//---------------------------------------------------------------------------

//Zmiana tekstu na pasku okna rozmowy
void ChangeFrmSendTitlebar()
{
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_PRIMARYTAB,OnPrimaryTab);
  PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
  PluginLink.UnhookEvent(OnPrimaryTab);
}
//---------------------------------------------------------------------------

//Zmiana tekstu na pasku okna kontaktow
void ChangeFrmMainTitlebar()
{
  //Jezeli funkcjonalnosc jest wlaczona
  if(TweakFrmMainTitlebarChk)
  {
	//Tryb I
	if(TweakFrmMainTitlebarMode==1)
	{
	  //Nazwa aktywnego profilu
	  if(TweakFrmMainTitlebarModeEx==0)
	   SetWindowTextW(hFrmMain,("AQQ [" + ProfileName + "]").w_str());
	  //Nazwa komputera
	  else if(TweakFrmMainTitlebarModeEx==1)
	   SetWindowTextW(hFrmMain,("AQQ [" + ComputerName + "]").w_str());
	  //Nazwa zasobu glownego konta Jabber
	  else
	   SetWindowTextW(hFrmMain,("AQQ [" + ResourceName + "]").w_str());
	}
	//Tryb II
	else
	{
	  //Wlasny tekst
	  if(!TweakFrmMainTitlebarText.IsEmpty())
	  {
		//Pusty tekst
		if(TweakFrmMainTitlebarText=="-")
		 SetWindowTextW(hFrmMain,L"");
		//Zdefiniowany tekst
		else
		 SetWindowTextW(hFrmMain,("AQQ " + TweakFrmMainTitlebarText).w_str());
	  }
	  //Brak wersji komunikatora
	  else
	   SetWindowTextW(hFrmMain,L"AQQ");
	}
  }
  //Funkcjonalnosc wylaczona - przywracanie oryginalnego tekstu na pasku okna kontaktow
  else
   SetWindowTextW(hFrmMain,FrmMainTitlebar);
}
//---------------------------------------------------------------------------

//Usuwanie elementu do przypinania/odpiniania zakladek
void DestroyClipTab()
{
  TPluginAction ClipTabItem;
  ZeroMemory(&ClipTabItem,sizeof(TPluginAction));
  ClipTabItem.cbSize = sizeof(TPluginAction);
  ClipTabItem.pszName = L"TabKitClipTabItem";
  ClipTabItem.Handle = (int)hFrmSend;
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&ClipTabItem));
}
//---------------------------------------------------------------------------

//Tworzenie elementu do przypinania/odpiniania zakladek
void BuildClipTab()
{
  if(hFrmSend)
  {
	TPluginAction ClipTabItem;
	ZeroMemory(&ClipTabItem,sizeof(TPluginAction));
	ClipTabItem.cbSize = sizeof(TPluginAction);
	ClipTabItem.pszName = L"TabKitClipTabItem";
	ClipTabItem.pszCaption = L"Przypnij/odepnij zak³adkê";;
	ClipTabItem.Position = 1;
	ClipTabItem.IconIndex = -1;
	ClipTabItem.pszService = L"sTabKitClipTabItem";
	ClipTabItem.pszPopupName = L"popTab";
	ClipTabItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&ClipTabItem));
  }
}
//---------------------------------------------------------------------------

//Serwis do przypinania/odpiniania zakladek
int __stdcall ServiceClipTabItem(WPARAM wParam, LPARAM lParam)
{
  //Pobieranie nowo zdefinowanej przypietej karty
  UnicodeString ClipTab = ClipTabPopup;
  //Jezeli zakladka nie jest przypieta
  if(ClipTabsList->IndexOf(ClipTab)==-1)
  {
	//Dodanie nowej zakladki do listy przypietych zakladek
	ClipTabsList->Add(ClipTab);
	//Zapisanie przypietej zakladki w pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *ClipTabs = new TStringList;
	Ini->ReadSection("ClipTabs",ClipTabs);
	int Count = ClipTabs->Count;
	delete ClipTabs;
	Ini->WriteString("ClipTabs", "Tab"+IntToStr(Count+1), ClipTab);
	delete Ini;
	//Zmiana miejsca zakladki
	if(!ClipTab.Pos("ischat_"))
	{
	  int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,0,(LPARAM)ClipTab.w_str());
	  if(Index)
	  {
		TPluginTriple PluginTriple;
		PluginTriple.cbSize = sizeof(TPluginTriple);
		PluginTriple.Handle1 = (int)hFrmSend;
		PluginTriple.Param1 = Index;
		PluginTriple.Param2 = 0;
		PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
	  }
	}
	else
	{
	  UnicodeString ClipTabW = ClipTab;
	  ClipTabW = ClipTabW.Delete(1,7);
	  int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,2,(LPARAM)ClipTabW.w_str());
	  if(Index)
	  {
		TPluginTriple PluginTriple;
		PluginTriple.cbSize = sizeof(TPluginTriple);
		PluginTriple.Handle1 = (int)hFrmSend;
		PluginTriple.Param1 = Index;
		PluginTriple.Param2 = 0;
		PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
	  }
	}
	//Odswiezenie wszystkich zakladek
	RefreshTabs();
  }
  //Jezeli zakladka jest juz przypieta
  else
  {
	//Usuniecie zakladki z listy przypietych zakladek
	ClipTabsList->Delete(ClipTabsList->IndexOf(ClipTab));
	//Zapisanie ponownie listy przypietych zakladek w pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	Ini->EraseSection("ClipTabs");
	if(ClipTabsList->Count>0)
	{
	  for(int Count=0;Count<ClipTabsList->Count;Count++)
	   Ini->WriteString("ClipTabs", "Tab"+IntToStr(Count+1), ClipTabsList->Strings[Count]);
	}
	delete Ini;
	//Ponieranie ilosci zakladek
	TPluginTriple PluginTriple;
	PluginTriple.cbSize = sizeof(TPluginTriple);
	PluginTriple.Handle1 = (int)hFrmSend;
	int Count = PluginLink.CallService(AQQ_FUNCTION_TABCOUNT,(WPARAM)&PluginTriple,0);
	//Zmiana miejsca zakladki
	if(Count)
	{
	  if(!ClipTab.Pos("ischat_"))
	  {
		int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,0,(LPARAM)ClipTab.w_str());
		PluginTriple.cbSize = sizeof(TPluginTriple);
		PluginTriple.Param1 = Index;
		PluginTriple.Param2 = Count-1;
		PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
	  }
	  else
	  {
		UnicodeString ClipTabW = ClipTab;
		ClipTabW = ClipTabW.Delete(1,7);
		int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,2,(LPARAM)ClipTabW.w_str());
		PluginTriple.cbSize = sizeof(TPluginTriple);
		PluginTriple.Param1 = Index;
		PluginTriple.Param2 = Count-1;
		PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
	  }
	}
	//Odswiezenie wszystkich zakladek
	RefreshTabs();
  }

  return 0;
}
//---------------------------------------------------------------------------

//Usuniecie wszystkich zdefiniowanych przypietych zakladek
void EraseClipTabs()
{
  //Usuniecie wszystkich zakladek z listy przypietych zakladek
  ClipTabsList->Clear();
  //Usuniecie wszystkich przypietych zakladek z listy przypietych zakladek w pliku sesji
  TIniFile *Ini = new TIniFile(SessionFileDir);
  Ini->EraseSection("ClipTabs");
  delete Ini;
  //Odswiezenie wszystkich zakladek
  RefreshTabs();
}
//---------------------------------------------------------------------------

//Usuniecie i aktualizacja mini awatarow przypietych zakladek
void EraseClipTabsIcons()
{
  TStringList *ClipIcons = new TStringList;
  ClipTabsIconList->ReadSection("ClipTabsIcon",ClipIcons);
  if(ClipIcons->Count)
  {
	//Wyladowanie wszystkich ikon z intefejsu AQQ
	for(int Count=0;Count<ClipIcons->Count;Count++)
	{
	  UnicodeString JID = ClipIcons->Strings[Count];
	  int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID,0);
	  if(Icon) PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)Icon);
	}
	ClipTabsIconList->EraseSection("ClipTabsIcon");
	//Odswiezenie wszystkich zakladek
	RefreshTabs();
  }
  delete ClipIcons;
}
//---------------------------------------------------------------------------

//Odczyt przypietych zakladek
void LoadClipTabs()
{
  //Definicja zmiennej przypietej zakladki
  UnicodeString ClipTab;
  //Wczytanie pliku sesji
  TIniFile *Ini = new TIniFile(SessionFileDir);
  TStringList *ClipTabs = new TStringList;
  Ini->ReadSection("ClipTabs",ClipTabs);
  int TabsCount = ClipTabs->Count;
  delete ClipTabs;
  //Jezeli mamy jakies przypiete zakladki
  if(TabsCount>0)
  {
	for(int Count=0;Count<TabsCount;Count++)
	{
	  ClipTab = Ini->ReadString("ClipTabs", "Tab"+IntToStr(Count+1),"");
	  //Dodanie zakladki do tablicy przypietych zakladek
	  if(!ClipTab.IsEmpty())
	   ClipTabsList->Add(ClipTab);
	}
  }
  delete Ini;
}
//---------------------------------------------------------------------------

//Pobranie rozmiaru+pozycji okna kontatkow
void GetFrmMainRect()
{
  //Pobranie rozmiaru+pozycji okna kontaktow
  GetWindowRect(hFrmMain,&FrmMainRect);
  //Pobranie realnego rozmiaru+pozycji oka kontaktow
  GetWindowInfo(hFrmMain,&FrmMainInfo);
  if(FrmMainSlideEdge==1)
  {
	FrmMainRealLeftPos = FrmMainInfo.rcWindow.left - FrmMainInfo.rcClient.left;
	FrmMainRealRightPos = 0;
	FrmMainRealBottomPos = 0;
	FrmMainRealTopPos = 0;
  }
  else if(FrmMainSlideEdge==2)
  {
	FrmMainRealRightPos = FrmMainInfo.rcWindow.right - FrmMainInfo.rcClient.right;
	FrmMainRealLeftPos = 0;
	FrmMainRealBottomPos = 0;
	FrmMainRealTopPos = 0;
  }
  else if(FrmMainSlideEdge==3)
  {
	FrmMainRealBottomPos = FrmMainInfo.rcWindow.bottom - FrmMainInfo.rcClient.bottom;
	FrmMainRealLeftPos = 0;
	FrmMainRealRightPos = 0;
	FrmMainRealTopPos = 0;
  }
  else
  {
	FrmMainRealTopPos = FrmMainInfo.rcWindow.top - FrmMainInfo.rcClient.top;
	FrmMainRealLeftPos = 0;
	FrmMainRealRightPos = 0;
	FrmMainRealBottomPos = 0;
  }
}
//---------------------------------------------------------------------------

//Ustawienie poprawnej pozycji okna kontaktow
void SetFrmMainPos()
{
  //Left
  if(FrmMainSlideEdge==1)
  {
	//Wlaczone obramowanie
	if(ChkSkinEnabled())
	{
	  WINDOWINFO pwi;
	  GetWindowInfo(hFrmMain,&pwi);
	  int RealLeftPos = pwi.rcWindow.left - pwi.rcClient.left;
	  SetWindowPos(hFrmMain,HWND_TOP,0+RealLeftPos+FrmMain_Shell_TrayWndLeft,FrmMainRect.Top,0,0,SWP_NOSIZE);
	}
	//Obramowanie wylaczone
	else
	 SetWindowPos(hFrmMain,HWND_TOP,0+FrmMain_Shell_TrayWndLeft,FrmMainRect.Top,0,0,SWP_NOSIZE);
  }
  //Right
  else if(FrmMainSlideEdge==2)
   SetWindowPos(hFrmMain,HWND_TOP,Screen->Width-FrmMainRect.Width()-FrmMain_Shell_TrayWndRight,FrmMainRect.Top,0,0,SWP_NOSIZE);
  //Bottom
  else if(FrmMainSlideEdge==3)
   SetWindowPos(hFrmMain,HWND_TOP,FrmMainRect.Left,Screen->Height-FrmMainRect.Height()-FrmMain_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
  //Top
  else
   SetWindowPos(hFrmMain,HWND_TOP,FrmMainRect.Left,FrmMain_Shell_TrayWndTop,0,0,SWP_NOSIZE);
}
//---------------------------------------------------------------------------

//Pobranie rozmiaru+pozycji okna rozmowy
void GetFrmSendRect()
{
  //Pobranie rozmiaru+pozycji okna rozmowy
  GetWindowRect(hFrmSend,&FrmSendRect);
  //Pobranie realnego rozmiaru+pozycji okna rozmowy
  GetWindowInfo(hFrmSend,&FrmSendInfo);
  if(FrmSendSlideEdge==1)
  {
	FrmSendRealLeftPos = FrmSendInfo.rcWindow.left - FrmSendInfo.rcClient.left;
	FrmSendRealRightPos = 0;
	FrmSendRealBottomPos = 0;
	FrmSendRealTopPos = 0;
  }
  else if(FrmSendSlideEdge==2)
  {
	FrmSendRealRightPos = FrmSendInfo.rcWindow.right - FrmSendInfo.rcClient.right;
	FrmSendRealLeftPos = 0;
	FrmSendRealBottomPos = 0;
	FrmSendRealTopPos = 0;
  }
  else if(FrmSendSlideEdge==3)
  {
	FrmSendRealBottomPos = FrmSendInfo.rcWindow.bottom - FrmSendInfo.rcClient.bottom;
	FrmSendRealLeftPos = 0;
	FrmSendRealRightPos = 0;
	FrmSendRealTopPos = 0;
  }
  else
  {
	FrmSendRealTopPos = FrmSendInfo.rcWindow.top - FrmSendInfo.rcClient.top;
	FrmSendRealLeftPos = 0;
	FrmSendRealRightPos = 0;
	FrmSendRealBottomPos = 0;
  }
}
//---------------------------------------------------------------------------

//Ustawienie poprawnej pozycji okna rozmowy
void SetFrmSendPos()
{
  //Left
  if(FrmSendSlideEdge==1)
  {
	//Wlaczone obramowanie
	if(ChkSkinEnabled())
	{
	  WINDOWINFO pwi;
	  GetWindowInfo(hFrmSend,&pwi);
	  int RealLeftPos = pwi.rcWindow.left - pwi.rcClient.left;
	  SetWindowPos(hFrmSend,HWND_TOP,0+RealLeftPos+FrmSend_Shell_TrayWndLeft,FrmSendRect.Top,0,0,SWP_NOSIZE);
	}
	//Obramowanie wylaczone
	else
	 SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,FrmSendRect.Top,0,0,SWP_NOSIZE);
  }
  //Right
  else if(FrmSendSlideEdge==2)
   SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-FrmSendRect.Width()-FrmSend_Shell_TrayWndRight,FrmSendRect.Top,0,0,SWP_NOSIZE);
  //Bottom
  else if(FrmSendSlideEdge==3)
   SetWindowPos(hFrmSend,HWND_TOP,FrmSendRect.Left,Screen->Height-FrmSendRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
  //Top
  else
   SetWindowPos(hFrmSend,HWND_TOP,FrmSendRect.Left,FrmSend_Shell_TrayWndTop,0,0,SWP_NOSIZE);
}
//---------------------------------------------------------------------------

//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
void ChkFullScreenMode()
{
  //Odznaczenie braku wyjatku
  FullScreenModeExeptions = false;
  //Pobieranie wymiarow aktywnego okna
  TRect ActiveFrmRect;
  GetWindowRect(GetForegroundWindow(),&ActiveFrmRect);;
  //Sprawdzanie szerokosci/wysokosci okna
  if((ActiveFrmRect.Width()==Screen->Width)&&(ActiveFrmRect.Height()==Screen->Height))
  {
	//Pobieranie klasy aktywnego okna
	wchar_t WClassName[128];
	GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
	//Wyjatek dla pulpitu oraz programu DeskScapes
	if(((UnicodeString)WClassName!="Progman")&&((UnicodeString)WClassName!="SysListView32")&&((UnicodeString)WClassName!="WorkerW")&&((UnicodeString)WClassName!="NDesk"))
	{
	  //Dodano jakies wyjatki w aplikacjach pelnoekranowych
	  if((SideSlideExceptions->Count)&&(SideSlideFullScreenModeChk))
	  {
		//Pobranie sciezki procesu
		UnicodeString Process = GetPathOfProces(GetForegroundWindow());
		//Proces znajduje sie na liscie wyjatkow
		if(SideSlideExceptions->IndexOf(ExtractFileName(Process).LowerCase())!=-1)
		 FullScreenModeExeptions = true;
	  }
	  //Wyjatek dla aplikacji Metro UI
	  if(AnsiPos("Windows.UI.Core.CoreWindow",(UnicodeString)WClassName))
	   FullScreenModeExeptions = true;

	  FullScreenMode = true;
	}
	else FullScreenMode = false;
  }
  else FullScreenMode = false;
}
//---------------------------------------------------------------------------

//Serwis do wklejania tekstu ze schowka jako cytat
int __stdcall ServiceQuickQuoteItem(WPARAM wParam, LPARAM lParam)
{
  //Jezeli schowek jest dostepny
  if(OpenClipboard(NULL))
  {
	//Pobieranie tekstu ze schowka
	HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
	UnicodeString ClipboardText = (wchar_t*)GlobalLock(hClipboardData);
	GlobalUnlock(hClipboardData);
	CloseClipboard();
	//Jezeli pobrano tekst
	if(!ClipboardText.IsEmpty())
	{
	  //Tekst ze schowka jako cytat
	  ClipboardText = "„" + ClipboardText + "”";
	  //Pobieranie tekstu z RichEdit
	  int iLength = GetWindowTextLengthW(hRichEdit)+1;
	  wchar_t* pBuff = new wchar_t[iLength];
	  GetWindowTextW(hRichEdit, pBuff, iLength);
	  UnicodeString Text = pBuff;
	  delete pBuff;
	  //Usuwanie tekstu zaznaczonego
	  CHARRANGE SelPos;
	  SendMessage(hRichEdit, EM_EXGETSEL, NULL, (LPARAM)&SelPos);
	  if(SelPos.cpMin!=SelPos.cpMax)
	   Text = Text.Delete(SelPos.cpMin+1,SelPos.cpMax-SelPos.cpMin);
	  //Wklejanie cytatu do tekstu
	  Text = Text.Insert(ClipboardText,SelPos.cpMin+1);
	  //Nadpisywanie tekstu w RichEdit
	  SetWindowTextW(hRichEdit, Text.w_str());
	  //Ustawianie pozycji kursora
	  SelPos.cpMin = SelPos.cpMin + ClipboardText.Length();
	  SelPos.cpMax = SelPos.cpMin;
	  SendMessage(hRichEdit, EM_EXSETSEL, NULL, (LPARAM)&SelPos);
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Serwis otwierania zawinietego obrazka w programie graficznym
int __stdcall ServiceCollapseImagesItem(WPARAM wParam, LPARAM lParam)
{
  //Otwarcie obrazka w programie graficznym
  ShellExecute(NULL, L"open", ("\""+CollapseImagesItemURL+"\"").w_str(), NULL, NULL, SW_SHOWNORMAL);
  //Usuwanie elementu
  TPluginAction CollapseImagesItem;
  ZeroMemory(&CollapseImagesItem,sizeof(TPluginAction));
  CollapseImagesItem.cbSize = sizeof(TPluginAction);
  CollapseImagesItem.pszName = L"TabKitCollapseImagesItem";
  CollapseImagesItem.Handle = (int)hFrmSend;
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&CollapseImagesItem));
  //Kasowanie URL
  CollapseImagesItemURL = "";

  return 0;
}
//---------------------------------------------------------------------------

//Usuwanie elementu trzymania okna rozmowy na wierzchu
void DestroyStayOnTop()
{
  TPluginAction StayOnTopItem;
  ZeroMemory(&StayOnTopItem,sizeof(TPluginAction));
  StayOnTopItem.cbSize = sizeof(TPluginAction);
  StayOnTopItem.pszName = L"TabKitStayOnTopItem";
  StayOnTopItem.Handle = (int)hFrmSend;
  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&StayOnTopItem));
}
//---------------------------------------------------------------------------

//Tworzenie elementu trzymania okna rozmowy na wierzchu
void BuildStayOnTop()
{
  //Jezeli funkcjonalnosc jest wlaczona i dozwolona
  if((hFrmSend)&&(StayOnTopChk))
  {
    TPluginAction StayOnTopItem;
	ZeroMemory(&StayOnTopItem,sizeof(TPluginAction));
	StayOnTopItem.cbSize = sizeof(TPluginAction);
	StayOnTopItem.pszName = L"TabKitStayOnTopItem";
	StayOnTopItem.pszCaption = L"Trzymaj okno na wierzchu";
	StayOnTopItem.Hint = L"Trzymaj okno na wierzchu";
	if(StayOnTopStatus)
	{
	  StayOnTopItem.IconIndex = STAYONTOP_ON;
	  //Funkcjonalnosc SideSlide dla okna rozmowy jest wylaczona
	  if(!FrmSendSlideChk)
	   //Tworzenie timera ustawiajacego okno na wierzchu
	   SetTimer(hTimerFrm,TIMER_STAYONTOP,100,(TIMERPROC)TimerFrmProc);
	}
	else
	{
	  StayOnTopItem.IconIndex = STAYONTOP_OFF;
	}
	StayOnTopItem.pszService = L"sTabKitStayOnTopItem";
	StayOnTopItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&StayOnTopItem));
  }
}
//---------------------------------------------------------------------------

//Serwis trzymania okna rozmowy na wierzchu
int __stdcall ServiceStayOnTopItem(WPARAM wParam, LPARAM lParam)
{
  //Funkcjonalnosc SideSlide dla okna rozmowy jest wylaczona
  if(!FrmSendSlideChk)
  {
	//Set TOPMOST
	if(!StayOnTopStatus)
	{
	  //Aktualizacja przycisku
	  TPluginAction StayOnTopItem;
	  ZeroMemory(&StayOnTopItem,sizeof(TPluginAction));
	  StayOnTopItem.cbSize = sizeof(TPluginAction);
	  StayOnTopItem.pszName = L"TabKitStayOnTopItem";
	  StayOnTopItem.pszCaption = L"Trzymaj okno na wierzchu";
	  StayOnTopItem.Hint = L"Trzymaj okno na wierzchu";
	  StayOnTopItem.IconIndex = STAYONTOP_ON;
	  StayOnTopItem.Handle = (int)hFrmSend;
	  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&StayOnTopItem));
	  //Okno rozmowy na wierzchu
	  SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  //Stan StayOnTop
	  StayOnTopStatus = true;
	}
	//Set NOTOPMOST
	else
	{
	  //Aktualizacja przycisku
	  TPluginAction StayOnTopItem;
	  ZeroMemory(&StayOnTopItem,sizeof(TPluginAction));
	  StayOnTopItem.cbSize = sizeof(TPluginAction);
	  StayOnTopItem.pszName = L"TabKitStayOnTopItem";
	  StayOnTopItem.pszCaption = L"Trzymaj okno na wierzchu";
	  StayOnTopItem.Hint = L"Trzymaj okno na wierzchu";
	  StayOnTopItem.IconIndex = STAYONTOP_OFF;
	  StayOnTopItem.Handle = (int)hFrmSend;
	  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&StayOnTopItem));
	  //Przywrocenie "normalnosci" okna
	  SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  //Stan StayOnTop
	  StayOnTopStatus = false;
	}
  }
  //Funkcjonalnosc SideSlide dla okna rozmowy jest wlaczona
  else
  {
	//Blokada SideSlide dla okna rozmowy
	if(!StayOnTopStatus)
	{
	  //Aktualizacja przycisku
	  TPluginAction StayOnTopItem;
	  ZeroMemory(&StayOnTopItem,sizeof(TPluginAction));
	  StayOnTopItem.cbSize = sizeof(TPluginAction);
	  StayOnTopItem.pszName = L"TabKitStayOnTopItem";
	  StayOnTopItem.pszCaption = L"Trzymaj okno na wierzchu";
	  StayOnTopItem.Hint = L"Trzymaj okno na wierzchu";
	  StayOnTopItem.IconIndex = STAYONTOP_ON;
	  StayOnTopItem.Handle = (int)hFrmSend;
	  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&StayOnTopItem));
	  //Okno rozmowy na wierzchu
	  if(FrmSendSlideHideMode==2) SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  //Stan StayOnTop
	  StayOnTopStatus = true;
	  //Stan blokady
	  FrmSendBlockSlide = true;
	}
	//Usuniecie blokady SideSlide dla okna rozmowy
	else
	{
	  //Aktualizacja przycisku
	  TPluginAction StayOnTopItem;
	  ZeroMemory(&StayOnTopItem,sizeof(TPluginAction));
	  StayOnTopItem.cbSize = sizeof(TPluginAction);
	  StayOnTopItem.pszName = L"TabKitStayOnTopItem";
	  StayOnTopItem.pszCaption = L"Trzymaj okno na wierzchu";
	  StayOnTopItem.Hint = L"Trzymaj okno na wierzchu";
	  StayOnTopItem.IconIndex = STAYONTOP_OFF;
	  StayOnTopItem.Handle = (int)hFrmSend;
	  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&StayOnTopItem));
	  //Okno rozmowy na wierzchu
	  if(FrmSendSlideHideMode==2) SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  //Stan StayOnTop
	  StayOnTopStatus = false;
	  //Stan blokady
	  FrmSendBlockSlide = false;
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Minimalizacja / przywracanie okna rozmowy
void MinimizeRestoreFrmSendExecute()
{
  //Jesli okno rozmowy jest otwarte
  if(hFrmSend)
  {
	//Jesli komunikator nie jest zabezpieczony
	if(!SecureMode)
	{
	  //Funkcjonalnosc SideSlide dla okna rozmowy jest wylaczona
	  if(!FrmSendSlideChk)
	  {
		//Przywracanie okna rozmowy
		if(IsIconic(hFrmSend))
		{
		  ShowWindow(hFrmSend,SW_RESTORE);
		  BringWindowToTop(hFrmSend);
		  SetForegroundWindow(hFrmSend);
		}
		//Minimalizacja okna rozmowy
		else ShowWindow(hFrmSend,SW_MINIMIZE);
	  }
	  //Funkcjonalnosc SideSlide dla okna rozmowy jest wlaczona
	  else
	  {
		//FrmSendSlideOut
		if(FrmSendVisible)
		{
          //Wylaczenie tymczasowej blokady
		  if((FrmSendSlideHideMode==3)&&(!FrmSendBlockSlideWndEvent))
		  {
			if(StayOnTopChk) FrmSendBlockSlide = StayOnTopStatus;
			else FrmSendBlockSlide = false;
		  }
		  if((!FrmSendBlockSlide)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))
		  {
			//Status FrmSendSlideOut
			FrmSendSlideOut = true;
			//Wlaczenie FrmSendSlideOut (part I)
			SetTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
		  }
		}
		//FrmSendSlideIn
		else
		{
		  if((!FrmSendBlockSlide)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))
		  {
			//Status FrmSendSlideIn
			FrmSendSlideIn = true;
			//Wlaczenie FrmSendSlideIn (part I)
			SetTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEIN,1,(TIMERPROC)TimerFrmProc);
          }
		}
      }
	}
	//Jesli komunikator jest zabezpieczony
	else
	 PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aSecure");
  }
}
//---------------------------------------------------------------------------

//Minimalizacja / przywracanie okna kontaktow + otwieranie okna rozmowy z nowa wiadomoscia
void MinimizeRestoreFrmMainExecute()
{
  //Jesli komunikator nie jest zabezpieczony
  if(!SecureMode)
  {
	//Przywracanie okna kontaktow
	if(UnOpenMsgList->Count==0)
	{
	  //FrmMainSlideOut
	  if(FrmMainVisible)
	  {
		//Wylaczenie tymczasowej blokady
		if((FrmMainSlideHideMode==3)&&(!FrmMainBlockSlideWndEvent)) FrmMainBlockSlide = false;
		//Wlaczenie FrmMainSlideOut
		if((!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
		{
		  //Status FrmMainSlideOut
		  FrmMainSlideOut = true;
		  //Wlaczenie FrmMainSlideOut (part I)
		  SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
		}
	  }
	  //FrmMainSlideIn
	  else
	  {
		if((!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
		{
		  //Status FrmMainSlideIn
		  FrmMainSlideIn = true;
		  //Wlaczenie FrmMainSlideIn (part I)
		  SetTimer(hTimerFrm,TIMER_PREFRMMAINSLIDEIN,1,(TIMERPROC)TimerFrmProc);
		}
	  }
	}
	//Otwieranie okna rozmowy z nowa wiadomoscia
	else
	{
	  UnicodeString JID = UnOpenMsgList->Strings[0];
	  //Otwieranie zakladki z danym kontektem
	  if(!JID.Pos("ischat_"))
	  {
		if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
		else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
	  }
	  //Otwieranie zakladki z czatem
	  else
	  {
		//Ustawianie prawidlowego identyfikatora
		JID = JID.Delete(1,7);
		//Pobieranie nazwy kanalu
		TIniFile *Ini = new TIniFile(SessionFileDir);
		UnicodeString Channel = Ini->ReadString("Channels",JID,"");
		delete Ini;
		if(Channel.IsEmpty())
		{
		  Channel = JID;
		  Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
		}
		//Wypenianie struktury nt. czatu
		TPluginChatPrep PluginChatPrep;
		PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
		PluginChatPrep.UserIdx = GetContactIndexW(JID);
		PluginChatPrep.JID = JID.w_str();
		PluginChatPrep.Channel = Channel.w_str();
		PluginChatPrep.CreateNew = false;
		PluginChatPrep.Fast = true;
		//Przywracanie zakladki czatowej
		PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
	  }
    }
  }
  //Jesli komunikator jest zabezpieczony
  else
   PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aSecure");
}
//---------------------------------------------------------------------------

//Pokazywanie paska narzedzi w oknie rozmowy
void ShowToolBar()
{
  if((hFrmSend)&&(hToolBar))
  {
	//Pobieranie pozycji paska narzedzi
	TRect WindowRect;
	GetWindowRect(hToolBar,&WindowRect);
	//int ToolBarHeight = WindowRect.Height();
	//Pokazanie paska
	if(!WindowRect.Height())//ToolBarHeight)
	{
	  //Pokazanie paska
	  SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),23,SWP_NOMOVE);
	  //Pobieranie pozycji okna rozmowy
	  GetWindowRect(hFrmSend,&WindowRect);
	  //Odswiezenie okna rozmowy
	  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
	  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
	}
  }
}
//---------------------------------------------------------------------------

//Ukrywanie/pokazywanie przyciskow do przewijania zakladek
void CheckHideScrollTabButtons()
{
  //Jezeli okno rozmowy jest otwarte i uchwyty do kontrolkek zostaly pobrany
  if((hFrmSend)&&((hScrollTabButton[0])&&(hScrollTabButton[1])))
  {
	//Ukrycie strzalek do przewijania zakladek
	if((HideScrollTabButtonsChk)&&(!UnloadExecuted))
	{
	  SetWindowPos(hScrollTabButton[0],NULL,0,0,0,27,SWP_NOMOVE);
	  SetWindowPos(hScrollTabButton[1],NULL,0,0,0,27,SWP_NOMOVE);
	}
	//Pokazanie strzalek do przewijania zakladek
	else
	{
	  SetWindowPos(hScrollTabButton[0],NULL,0,0,20,27,SWP_NOMOVE);
	  SetWindowPos(hScrollTabButton[1],NULL,0,0,20,27,SWP_NOMOVE);
	}
  }
}
//---------------------------------------------------------------------------

//Skracanie wyswietlania odnosnikow
UnicodeString TrimBodyLinks(UnicodeString Body)
{
  //Dodawanie specjalnego tagu do wszystkich linkow
  Body = StringReplace(Body, "</A>", "[CC_LINK_END]</A>", TReplaceFlags() << rfReplaceAll);
  //Formatowanie tresci wiadomosci
  while(Body.Pos("[CC_LINK_END]"))
  {
	//Link with [CC_LINK_END] tag
	UnicodeString URL_WithTag = Body;
	URL_WithTag.Delete(URL_WithTag.Pos("[CC_LINK_END]")+13,URL_WithTag.Length());
	while(URL_WithTag.Pos("\">")) URL_WithTag.Delete(1,URL_WithTag.Pos("\">")+1);
	//Link without [CC_LINK_END] tag
	UnicodeString URL_WithOutTag = URL_WithTag;
	URL_WithOutTag.Delete(URL_WithOutTag.Pos("[CC_LINK_END]"),URL_WithOutTag.Length());
	//Wycinanie domeny z adresow URL
	UnicodeString URL_OnlyDomain = URL_WithOutTag;
	if(URL_OnlyDomain.LowerCase().Pos("www."))
	{
	  URL_OnlyDomain.Delete(1,URL_OnlyDomain.LowerCase().Pos("www.")+3);
	  if(URL_OnlyDomain.Pos("/"))
	   URL_OnlyDomain.Delete(URL_OnlyDomain.Pos("/"),URL_OnlyDomain.Length());
	  //Formatowanie linku
	  Body = StringReplace(Body, URL_WithOutTag + "\">" + URL_WithTag, URL_WithOutTag + "\" title=\"" + URL_WithOutTag.Trim() + "\">["+ URL_OnlyDomain + "]", TReplaceFlags());
	}
	else if(URL_OnlyDomain.LowerCase().Pos("http://"))
	{
	  URL_OnlyDomain.Delete(1,URL_OnlyDomain.LowerCase().Pos("http://")+6);
	  if(URL_OnlyDomain.Pos("/"))
	   URL_OnlyDomain.Delete(URL_OnlyDomain.Pos("/"),URL_OnlyDomain.Length());
	  //Formatowanie linku
	  Body = StringReplace(Body, URL_WithOutTag + "\">" + URL_WithTag, URL_WithOutTag + "\" title=\"" + URL_WithOutTag.Trim() + "\">["+ URL_OnlyDomain + "]", TReplaceFlags());
	}
	else if(URL_OnlyDomain.LowerCase().Pos("https://"))
	{
	  URL_OnlyDomain.Delete(1,URL_OnlyDomain.LowerCase().Pos("https://")+7);
	  if(URL_OnlyDomain.Pos("/"))
	   URL_OnlyDomain.Delete(URL_OnlyDomain.Pos("/"),URL_OnlyDomain.Length());
	  //Formatowanie linku
	  Body = StringReplace(Body, URL_WithOutTag + "\">" + URL_WithTag, URL_WithOutTag + "\" title=\"" + URL_WithOutTag.Trim() + "\">["+ URL_OnlyDomain + "]", TReplaceFlags());
	}
	//Niestandardowy odnosnik
	else
	 Body = StringReplace(Body, "[CC_LINK_END]", "", TReplaceFlags());
  }
  return Body;
}
//---------------------------------------------------------------------------
UnicodeString TrimStatusLinks(UnicodeString Body)
{
  //Dodawanie specjalnego tagu do wszystkich linkow
  Body = StringReplace(Body, "</A>", "[CC_LINK_END]</A>", TReplaceFlags() << rfReplaceAll);
  //Formatowanie tresci wiadomosci
  while(Body.Pos("[CC_LINK_END]"))
  {
	//Link with [CC_LINK_END] tag
	UnicodeString URL_WithTag = Body;
	URL_WithTag.Delete(URL_WithTag.Pos("[CC_LINK_END]")+13,URL_WithTag.Length());
	while(URL_WithTag.Pos("\">")) URL_WithTag.Delete(1,URL_WithTag.Pos("\">")+1);
	//Link without [CC_LINK_END] tag
	UnicodeString URL_WithOutTag = URL_WithTag;
	URL_WithOutTag.Delete(URL_WithOutTag.Pos("[CC_LINK_END]"),URL_WithOutTag.Length());
	//Link do YouTube
	if(((URL_WithOutTag.Pos("youtube.com"))&&(((URL_WithOutTag.Pos("watch?"))&&(URL_WithOutTag.Pos("v=")))||(URL_WithOutTag.Pos("/v/"))))||(URL_WithOutTag.Pos("youtu.be")))
	{
	  //Zmienna ID
	  UnicodeString ID;
	  //Wyciaganie ID - fullscreenowy
	  if(URL_WithOutTag.Pos("/v/"))
	  {
		//Parsowanie ID
		ID = URL_WithOutTag;
		ID.Delete(1,ID.Pos("/v/")+2);		
	  }
	  //Wyciaganie ID - zwykly & mobilny
	  else if(URL_WithOutTag.Pos("youtube.com"))
	  {
		//Parsowanie ID
		ID = URL_WithOutTag;
		ID.Delete(1,ID.Pos("v=")+1);
		if(ID.Pos("&"))	ID.Delete(ID.Pos("&"),ID.Length());
	  }	  
	  //Wyciaganie ID - skrocony
	  else if(URL_WithOutTag.Pos("youtu.be"))
	  {
		//Parsowanie ID
		ID = URL_WithOutTag;
		ID.Delete(1,ID.Pos(".be/")+3);
	  }
	  //Szukanie ID w cache
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  UnicodeString TitleFromCache = IniStrToStr(Ini->ReadString("YouTube",ID,""));
	  delete Ini;
	  //Tytul z cache
	  if(!TitleFromCache.IsEmpty())
	  {
		//Formatowanie linku
		Body = StringReplace(Body, URL_WithOutTag + "\">" + URL_WithTag, URL_WithOutTag + "\" title=\"" + URL_WithOutTag.Trim() + "\">"+ TitleFromCache, TReplaceFlags());
	  }
	  //Brak tytulu w cache
	  else
	  {
		//Przypisanie uchwytu do formy ustawien
		if(!hSettingsForm)
		{
		  Application->Handle = (HWND)SettingsForm;
		  hSettingsForm = new TSettingsForm(Application);
		}
		//Dodanie ID do przetworzenia
		GetYouTubeTitleList->Add(ID);
		//Wlaczenie watku
		if(!hSettingsForm->GetYouTubeTitleThread->Active) hSettingsForm->GetYouTubeTitleThread->Start();
		//Formatowanie linku
		Body = StringReplace(Body, URL_WithOutTag + "\">" + URL_WithTag, URL_WithOutTag + "\" title=\"" + URL_WithOutTag.Trim() + "\">[Pobieranie tytu³u...]", TReplaceFlags());
	  }
	}
	//Inne linki
	else
	{
	  //Wycinanie domeny z adresow URL
	  UnicodeString URL_OnlyDomain = URL_WithOutTag;
	  if(URL_OnlyDomain.LowerCase().Pos("www."))
	  {
		URL_OnlyDomain.Delete(1,URL_OnlyDomain.LowerCase().Pos("www.")+3);
		if(URL_OnlyDomain.Pos("/"))
		 URL_OnlyDomain.Delete(URL_OnlyDomain.Pos("/"),URL_OnlyDomain.Length());
		//Formatowanie linku
		Body = StringReplace(Body, URL_WithOutTag + "\">" + URL_WithTag, URL_WithOutTag + "\" title=\"" + URL_WithOutTag.Trim() + "\">["+ URL_OnlyDomain + "]", TReplaceFlags());
	  }
	  else if(URL_OnlyDomain.LowerCase().Pos("http://"))
	  {
		URL_OnlyDomain.Delete(1,URL_OnlyDomain.LowerCase().Pos("http://")+6);
		if(URL_OnlyDomain.Pos("/"))
		 URL_OnlyDomain.Delete(URL_OnlyDomain.Pos("/"),URL_OnlyDomain.Length());
		//Formatowanie linku
		Body = StringReplace(Body, URL_WithOutTag + "\">" + URL_WithTag, URL_WithOutTag + "\" title=\"" + URL_WithOutTag.Trim() + "\">["+ URL_OnlyDomain + "]", TReplaceFlags());
	  }
	  else if(URL_OnlyDomain.LowerCase().Pos("https://"))
	  {
		URL_OnlyDomain.Delete(1,URL_OnlyDomain.LowerCase().Pos("https://")+7);
		if(URL_OnlyDomain.Pos("/"))
		 URL_OnlyDomain.Delete(URL_OnlyDomain.Pos("/"),URL_OnlyDomain.Length());
		//Formatowanie linku
		Body = StringReplace(Body, URL_WithOutTag + "\">" + URL_WithTag, URL_WithOutTag + "\" title=\"" + URL_WithOutTag.Trim() + "\">["+ URL_OnlyDomain + "]", TReplaceFlags());
	  }
	  //Niestandardowy odnosnik
	  else
	   Body = StringReplace(Body, "[CC_LINK_END]", "", TReplaceFlags());
	}
  }
  return Body;
}
//---------------------------------------------------------------------------

//Serwis szybkiego dostepu do ustawien wtyczki
int __stdcall ServiceTabKitFastSettingsItem(WPARAM wParam, LPARAM lParam)
{
  //Przypisanie uchwytu do formy ustawien
  if(!hSettingsForm)
  {
	Application->Handle = (HWND)SettingsForm;
	hSettingsForm = new TSettingsForm(Application);
  }
  //Uzupelnienie danych w komponencie niezbednym do funkcji zmiany tekstu na belce okna kontatkow
  hSettingsForm->TweakFrmMainTitlebarModeExComboBox->Items->Clear();
  hSettingsForm->TweakFrmMainTitlebarModeExComboBox->Items->Add(ProfileName);
  hSettingsForm->TweakFrmMainTitlebarModeExComboBox->Items->Add(ComputerName);
  hSettingsForm->TweakFrmMainTitlebarModeExComboBox->Items->Add(ResourceName);
  //Wlaczenie/wylaczanie obslugi EmuTabs na formie ustawien
  if(!EmuTabsWSupport) hSettingsForm->EmuTabsWCheckBox->Enabled = false;
  //Wstawienie wersji wtyczki do formy ustawien
  hSettingsForm->VersionLabel->Caption = "TabKit " + GetFileInfo(GetPluginDir().w_str(), L"FileVersion");
  //Pokaznie okna ustawien
  hSettingsForm->Show();

  return 0;
}
//---------------------------------------------------------------------------

//Usuwanie elementu szybkiego dostepu do ustawien wtyczki
void DestroyTabKitFastSettings()
{
  TPluginAction BuildTabKitFastSettingsItem;
  ZeroMemory(&BuildTabKitFastSettingsItem,sizeof(TPluginAction));
  BuildTabKitFastSettingsItem.cbSize = sizeof(TPluginAction);
  BuildTabKitFastSettingsItem.pszName = L"TabKitFastSettingsItemButton";
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&BuildTabKitFastSettingsItem));
}
//---------------------------------------------------------------------------

//Tworzenie elementu szybkiego dostepu do ustawien wtyczki
void BuildTabKitFastSettings()
{
  TPluginAction BuildTabKitFastSettingsItem;
  ZeroMemory(&BuildTabKitFastSettingsItem,sizeof(TPluginAction));
  BuildTabKitFastSettingsItem.cbSize = sizeof(TPluginAction);
  BuildTabKitFastSettingsItem.pszName = L"TabKitFastSettingsItemButton";
  BuildTabKitFastSettingsItem.pszCaption = L"TabKit";
  BuildTabKitFastSettingsItem.IconIndex = FASTACCESS;
  BuildTabKitFastSettingsItem.pszService = L"sTabKitFastSettingsItem";
  BuildTabKitFastSettingsItem.pszPopupName = L"popPlugins";
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildTabKitFastSettingsItem));
}
//---------------------------------------------------------------------------

//Procka okna timera
LRESULT CALLBACK TimerFrmProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if((uMsg==WM_TIMER)&&(!ThemeChanging)&&(!ForceUnloadExecuted))
  {
	//Szukanie paska informacyjnego
	if(wParam==TIMER_FINDSTATUSBARPRO)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_FINDSTATUSBARPRO);
	  //Szukanie paska informacyjnego
	  hStatusBar = FindWindowEx(hFrmSend,NULL,L"TsStatusBar",NULL);
	  //Ponowne wlaczenie timera
	  if(!hStatusBar)
	   SetTimer(hTimerFrm,TIMER_FINDSTATUSBARPRO,500,(TIMERPROC)TimerFrmProc);
	}
	//Szukanie paska narzedzi
	else if(wParam==TIMER_FINDTOOLBAR)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_FINDTOOLBAR);
	  //Szukanie paska narzedzi
	  if(!hToolBar) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
	  //Ponowne wlaczenie timera
	  if(!hToolBar)
	   SetTimer(hTimerFrm,TIMER_FINDTOOLBAR,500,(TIMERPROC)TimerFrmProc);
	  //Ukrywanie paska narzedzi
	  else if(HideToolBarChk)
	  {
		//Pobieranie pozycji paska narzedzi
		TRect WindowRect;
		GetWindowRect(hToolBar,&WindowRect);
		//Ukrycie paska narzedzi
		SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
		//Pobieranie pozycji okna rozmowy
		GetWindowRect(hFrmSend,&WindowRect);
		//Odswiezenie okna rozmowy
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
	  }
	}
	//Szukanie paska zakladek
	else if(wParam==TIMER_FINDTABSBAR)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_FINDTABSBAR);
	  //Szukanie paska zakladek
	  EnumChildWindows(hFrmSend,(WNDENUMPROC)FindTabsBar,0);
	  //Ponowne wlaczenie timera
	  if((!hTabsBar)||(!hScrollTabButton[0])||(!hScrollTabButton[1]))
	   SetTimer(hTimerFrm,TIMER_FINDTABSBAR,500,(TIMERPROC)TimerFrmProc);
	}
	//StayOnTop
	else if(wParam==TIMER_STAYONTOP)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_STAYONTOP);
	  //Ustawienie okna na wierzchu
	  SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	//Wylaczanie modalnosci okna wycinka
	else if(wParam==TIMER_TURNOFFMODAL)
	{
      //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_TURNOFFMODAL);
	  //Wylaczenie modalnosci dla okna kontatkow
	  EnableWindow(hFrmMain,true);
	  //Wylaczenie modalnosci dla okna rozmowy
	  EnableWindow(hFrmSend,true);
	}
	//InactiveNotifer
	else if(wParam==TIMER_INACTIVENOTIFER)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_INACTIVENOTIFER);
	  //Odblokowanie notyfikatora nowych wiadomosci
	  BlockInactiveNotiferNewMsg = false;
	}
	//Pokazywanie paska narzedzi
	else if(wParam==TIMER_TOOLBAR)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_TOOLBAR);
	  //Pobieranie aktywnej kontrolki w ktorej znajduje sie kursor
	  HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
	  //Jezeli okno rozmowy jest aktywne
	  if((GetActiveWindow()==hFrmSend)&&((hCurActiveFrm==hFrmSend)||(IsChild(hFrmSend,hCurActiveFrm))))
	  {
		//Pobieranie pozycji paska narzedzi
		TRect WindowRect;
		GetWindowRect(hToolBar,&WindowRect);
		//Sprawdzenie wysokosci paska narzedzi
		if(!WindowRect.Height())
		{
		  //Pokazanie paska
		  SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),23,SWP_NOMOVE);
		  //Pobieranie pozycji okna rozmowy
		  GetWindowRect(hFrmSend,&WindowRect);
		  //Odswiezenie okna rozmowy
		  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
		  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
		}
	  }
	}
	//Przywracanie sesji z czatami
	else if(wParam==TIMER_RESTORESESSION)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_RESTORESESSION);
	  //Sprawdzenie stanu glownego konta
	  TPluginStateChange PluginStateChange;
	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);
	  if(PluginStateChange.NewState!=0)
	  {
		//Jezeli sa jakies zakladki czatowe do przywrocenia
		if(ChatSessionList->Count)
		{
          //Pobieranie pierwszego rekordu z listy
		  UnicodeString JID = ChatSessionList->Strings[0];
		  //Zakladka z kontaktem nie jest jeszcze otwarta
		  if(TabsList->IndexOf(JID)==-1)
		  {
			//Ustawianie prawidlowego identyfikatora
			JID = JID.Delete(1,7);
			//Pobieranie nazwy kanalu
			TIniFile *Ini = new TIniFile(SessionFileDir);
			UnicodeString Channel = Ini->ReadString("Channels",JID,"");
			delete Ini;
			if(Channel.IsEmpty())
			{
			  Channel = JID;
			  Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
			}
			//Wypenianie struktury nt. czatu
			TPluginChatPrep PluginChatPrep;
			PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
			PluginChatPrep.UserIdx = GetContactIndexW(JID);
			PluginChatPrep.JID = JID.w_str();
			PluginChatPrep.Channel = Channel.w_str();
			PluginChatPrep.CreateNew = false;
			PluginChatPrep.Fast = true;
			//Przywracanie zakladki czatowej
			PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
			//Ponowne wlaczenie timera
			SetTimer(hTimerFrm,TIMER_RESTORESESSION,3000,(TIMERPROC)TimerFrmProc);
		  }
		  else
		  {
			//Usuwaniecie czatu z kolejki otwierania
			ChatSessionList->Delete(0);
			//Jezeli zostaly jeszcze jakies zakladki czatowe do przywrocenia
			if(ChatSessionList->Count)
			 SetTimer(hTimerFrm,TIMER_RESTORESESSION,1000,(TIMERPROC)TimerFrmProc);
		  }
		}
	  }
	  //Ponowne wlaczenie timera
	  else if(ChatSessionList->Count)
	   SetTimer(hTimerFrm,TIMER_RESTORESESSION,1000,(TIMERPROC)TimerFrmProc);
	}
	//Zmiana pozycji nowo otwartej przypietej zakladki
	else if(wParam==TIMER_MOVECLIPTAB)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_MOVECLIPTAB);
	  //Jezeli aktywna zakladka jest przypieta
	  if(ClipTabsList->IndexOf(ActiveTabJID)!=-1)
	  {
		//Zmiana miejsca zakladki ze zwyklym kontaktem
		if(!ActiveTabJID.Pos("ischat_"))
		{
		  int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,0,(LPARAM)ActiveTabJID.w_str());
		  if(Index)
		  {
			TPluginTriple PluginTriple;
			PluginTriple.cbSize = sizeof(TPluginTriple);
			PluginTriple.Handle1 = (int)hFrmSend;
			PluginTriple.Param1 = Index;
			PluginTriple.Param2 = 0;
			PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
		  }
		}
		//Zmiana miejsca zakladki z czatem
		else
		{
		  UnicodeString ClipTabW = ActiveTabJID;
		  ClipTabW = ClipTabW.Delete(1,7);
		  int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,2,(LPARAM)ClipTabW.w_str());
		  if(Index)
		  {
			TPluginTriple PluginTriple;
			PluginTriple.cbSize = sizeof(TPluginTriple);
			PluginTriple.Handle1 = (int)hFrmSend;
			PluginTriple.Param1 = Index;
			PluginTriple.Param2 = 0;
			PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
		  }
		  //Zakladka byla otwierana przy otwarciu okna jako ostatnia
		  if(ActiveTabJID==LastOpenedChatClipTab)
		  {
			//Usuniecie JID ostatniej otwartej przypietej zakladki czatowej
			LastOpenedChatClipTab = "";
			//Zmiana aktywnej zakladki
			if(!ActiveTabBeforeOpenClipTabs.Pos("ischat_"))
			{
			  if(!ActiveTabBeforeOpenClipTabs.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)ActiveTabBeforeOpenClipTabs.w_str());
			  else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(ActiveTabBeforeOpenClipTabs),(LPARAM)ActiveTabBeforeOpenClipTabs.w_str());
			}
			else
			{
			  UnicodeString ActiveTabW = ActiveTabBeforeOpenClipTabs;
			  ActiveTabW = ActiveTabW.Delete(1,7);
			  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(ActiveTabW),(LPARAM)ActiveTabW.w_str());
			}
			//Usuniecie JID ostatnio aktywnej zakladki przed otwieraniem przypietych czatow
			ActiveTabBeforeOpenClipTabs = "";
		  }
		}
	  }
	}
	//Otwieranie przypietych zakladek wraz z oknem rozmowy
	else if(wParam==TIMER_OPENCLIPTABS)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_OPENCLIPTABS);
	  //Jezeli sa przypiete jakies zakladki
	  if(ClipTabsList->Count)
	  {
		//Pobieranie aktualnie aktywnej zakladki
		UnicodeString ActiveTab = ActiveTabJID;
		//Otwieranie przypietych zakladek
		for(int Count=0;Count<ClipTabsList->Count;Count++)
		{
		  //Pobieranie identyfikatora przypietej zakladki
		  UnicodeString JID = ClipTabsList->Strings[Count];
		  //Przypieta zakladka nie jest jeszcze otwarta
		  if(TabsList->IndexOf(JID)==-1)
		  {
			//Otwieranie zakladki z danym kontektem
			if(!JID.Pos("ischat_"))
			{
			  if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
			  else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
			}
			//Otwieranie zakladki z czatem
			else if(!JID.Pos("@plugin"))
			{
			  //Zapisywanie JID ostatnio aktywnej zakladki przed otwieraniem przypietych czatow
			  ActiveTabBeforeOpenClipTabs = ActiveTab;
			  //Zapisywanie JID ostatniej otwartej przypietej zakladki czatowej
			  LastOpenedChatClipTab = JID;
			  //Ustawianie prawidlowego identyfikatora
			  JID = JID.Delete(1,7);
			  //Pobieranie nazwy kanalu
			  TIniFile *Ini = new TIniFile(SessionFileDir);
			  UnicodeString Channel = Ini->ReadString("Channels",JID,"");
			  delete Ini;
			  if(Channel.IsEmpty())
			  {
				Channel = JID;
				Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
			  }
			  //Wypenianie struktury nt. czatu
			  TPluginChatPrep PluginChatPrep;
			  PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
			  PluginChatPrep.UserIdx = GetContactIndexW(JID);
			  PluginChatPrep.JID = JID.w_str();
			  PluginChatPrep.Channel = Channel.w_str();
			  PluginChatPrep.CreateNew = false;
			  PluginChatPrep.Fast = true;
			  //Przywracanie zakladki czatowej
			  PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
			}
		  }
		}
		//Liczba otwartych zakladek wieksza od 1
		if(ResTabsList->Count>1)
		{
		  //Zmiana aktywnej zakladki
		  if(!ActiveTab.Pos("ischat_"))
		  {
			if(!ActiveTab.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)ActiveTab.w_str());
			else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(ActiveTab),(LPARAM)ActiveTab.w_str());
		  }
		  else
		  {
			UnicodeString ActiveTabW = ActiveTab;
			ActiveTabW = ActiveTabW.Delete(1,7);
			PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(ActiveTabW),(LPARAM)ActiveTabW.w_str());
		  }
		  //Zmiana pozycji aktywnej zakladki
		  //Ponieranie ilosci zakladek
		  TPluginTriple PluginTriple;
		  PluginTriple.cbSize = sizeof(TPluginTriple);
		  PluginTriple.Handle1 = (int)hFrmSend;
		  int Count = PluginLink.CallService(AQQ_FUNCTION_TABCOUNT,(WPARAM)&PluginTriple,0);
		  if(!ActiveTab.Pos("ischat_"))
		  {
			int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,0,(LPARAM)ActiveTab.w_str());
			PluginTriple.cbSize = sizeof(TPluginTriple);
			PluginTriple.Param1 = Index;
			PluginTriple.Param2 = Count-1;
			PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
		  }
		  else
		  {
			UnicodeString ActiveTabW = ActiveTab;
			ActiveTabW = ActiveTab.Delete(1,7);
			int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,2,(LPARAM)ActiveTab.w_str());
			PluginTriple.cbSize = sizeof(TPluginTriple);
			PluginTriple.Param1 = Index;
			PluginTriple.Param2 = Count-1;
			PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
		  }
		}
	  }
	}
	//Pobieranie ostatniej wiadomosci przy otwieraniu ponownie ostatnio zamknietej zakladki
	else if(wParam==TIMER_LOADLASTCONV)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_LOADLASTCONV);
	  //Pobieranie ostatniej wiadomoœci
	  PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)UnCloseTabFromHotKeyJID.w_str(),(LPARAM)UnCloseTabFromHotKeyUserIdx);
	}
	//Sprawdzanie czy zostaly zmienione pewne rzeczy w ustawieniach komunikatora
	else if(wParam==TIMER_CHKSETTINGS)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_CHKSETTINGS);
	  //Sprawdzanie czy zostala wlaczona jest obsluga zakladek
	  TStrings* IniList = new TStringList();
	  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	  Settings->SetStrings(IniList);
	  delete IniList;
	  UnicodeString TabsEnabled = Settings->ReadString("Settings","Tabs","0");
	  delete Settings;
	  if(!StrToBool(TabsEnabled))
	  {
		//Nowe ustawienia
		TSaveSetup SaveSetup;
		SaveSetup.Section = L"Settings";
		SaveSetup.Ident = L"Tabs";
		SaveSetup.Value = L"1";
		//Zapis ustawien
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		//Odswiezenie ustawien
		PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
		//Komunikat informacyjny
		MessageBox(Application->Handle,
		L"Wtyczka TabKit do prawid³owego dzia³ania wymaga w³¹czenia obs³ugi zak³adek w oknie rozmowy!\n"
		L"Obs³uga zak³adek w oknie rozmowy zosta³a automatycznie w³¹czona.",
		L"TabKit - obs³uga zak³adek",
		MB_OK | MB_ICONINFORMATION);
	  }
	  //Sprawdzenie czy zostal zmieniony zasob glownego konta Jabber
	  TPluginStateChange PluginStateChange;
	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);
	  UnicodeString pResourceName = (wchar_t*)PluginStateChange.Resource;
	  if(pResourceName!=ResourceName)
	  {
		//Zapisanie nowej nazwy zasobu glownego konta Jabber
		ResourceName = pResourceName;
		//Ustawianie nazwy zasobu glownego konta Jabber na pasku okna kontaktow
		if(TweakFrmMainTitlebarChk)
		{
		  if((TweakFrmMainTitlebarMode==1)&&(TweakFrmMainTitlebarModeEx==2))
		   SetWindowTextW(hFrmMain,("AQQ [" + ResourceName + "]").w_str());
		}
	  }
	  //Tworzenie timera
	  SetTimer(hTimerFrm,TIMER_CHKSETTINGS,500,(TIMERPROC)TimerFrmProc);
	}
	//Zamykanie zakladki poprzez 2xLPM
	else if(wParam==TIMER_CLOSEBY2XLPM)
	{
	  KillTimer(hTimerFrm,TIMER_CLOSEBY2XLPM);
	  if(LBUTTONDBLCLK)
	  {
		mouse_event(MOUSEEVENTF_MIDDLEUP,0,0,0,0);
		LBUTTONDBLCLK = false;
	  }
	}
	//Emulacja klikniecia myszka w pole tekstowe
	else if(wParam==TIMER_ACTIVATERICHEDIT)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_ACTIVATERICHEDIT);
	  //Emulacja klikniecia
	  TRect RichEditRect;
	  GetWindowRect(hRichEdit,&RichEditRect);
	  POINT pCur;
	  GetCursorPos(&pCur);
	  SetCursorPos(RichEditRect.Left+1,RichEditRect.Top+1);
	  mouse_event(MOUSEEVENTF_LEFTDOWN,RichEditRect.Left+1,RichEditRect.Top+1,0,0);
	  mouse_event(MOUSEEVENTF_LEFTUP,RichEditRect.Left+1,RichEditRect.Top+1,0,0);
	  SetCursorPos(pCur.x,pCur.y);
	}
	//Sprawdzanie pozycji myszki
	else if(wParam==TIMER_MOUSEPOSITION)
	{
	  //SlideSlide dla okna rozmowy
	  if((FrmSendSlideChk)&&(hFrmSend)&&(!FrmSendBlockMousePositionTimer))
	  {
		//FrmSendSlideIn (gdy kursor zblizy sie do krawedzi ekranu)
		if((!FrmSendVisible)&&(!SecureMode)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))
		{
		  //Blokada wysuwania okna przy wcisnietym klawiszu Ctrl/LPM/PPM
		  if((!SideSlideCtrlAndMouseBlockChk)||((SideSlideCtrlAndMouseBlockChk)&&(GetKeyState(VK_CONTROL)>=0)&&(GetKeyState(VK_LBUTTON)>=0)&&(GetKeyState(VK_RBUTTON)>=0))||((SideSlideCtrlAndMouseBlockChk)&&(GetKeyState(VK_CONTROL)<0)&&(GetKeyState(VK_LBUTTON)<0)))
		  {
			//Left or Right or Bottom or Top
			if(((FrmSendSlideEdge==1)&&(Mouse->CursorPos.x==0))
			||((FrmSendSlideEdge==2)&&(Mouse->CursorPos.x==Screen->Width-1))
			||((FrmSendSlideEdge==3)&&(Mouse->CursorPos.y==Screen->Height-1))
			||((FrmSendSlideEdge==4)&&(Mouse->CursorPos.y==0)))
			{
			  //Gdy kursor znajduje sie przy krawedzi ekranu z uwglednieniem miejsca schowania okna rozmowy
			  if((((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))&&((FrmSendRect.Top<=Mouse->CursorPos.y)&&(Mouse->CursorPos.y<=FrmSendRect.Bottom)))
			  ||(((FrmSendSlideEdge==3)||(FrmSendSlideEdge==4))&&((FrmSendRect.Left<=Mouse->CursorPos.x)&&(Mouse->CursorPos.x<=FrmSendRect.Right))))
			  {
				//Kursor nie znajduje sie w obrebie menu start
				wchar_t WClassName[128];
				GetClassNameW(WindowFromPoint(Mouse->CursorPos), WClassName, sizeof(WClassName));
				if(((UnicodeString)WClassName!="DesktopProgramsMFU")&&((UnicodeString)WClassName!="DV2ControlHost"))
				{
				  if(!PreFrmSendSlideIn)
				  {
					//Status FrmSendSlideIn
					PreFrmSendSlideIn = true;
					FrmSendDontBlockSlide = true;
					//Wlaczenie FrmSendSlideIn (part I)
					SetTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEIN,FrmSendSlideInDelay,(TIMERPROC)TimerFrmProc);
				  }
				}
			  }
			  else if(PreFrmSendSlideIn)
			  {
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEIN);
				//Status FrmSendSlideIn
				PreFrmSendSlideIn = false;
				FrmSendDontBlockSlide = false;
			  }
			}
			else
			{
			  //Zatrzymanie timera
			  KillTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEIN);
			  //Status FrmSendSlideIn
			  PreFrmSendSlideIn = false;
			  FrmSendDontBlockSlide = false;
			}
		  }
		  //Tymczasowa blokada wysuwania okna
		  else if(FrmSendSlideInDelay<1000)
		  {
			//Left or Right or Bottom or Top
			if(((FrmSendSlideEdge==1)&&(Mouse->CursorPos.x==0))
			||((FrmSendSlideEdge==2)&&(Mouse->CursorPos.x==Screen->Width-1))
			||((FrmSendSlideEdge==3)&&(Mouse->CursorPos.y==Screen->Height-1))
			||((FrmSendSlideEdge==4)&&(Mouse->CursorPos.y==0)))
			{
			  //Gdy kursor znajduje sie przy krawedzi ekranu z uwglednieniem miejsca schowania okna rozmowy
			  if((((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))&&((FrmSendRect.Top<=Mouse->CursorPos.y)&&(Mouse->CursorPos.y<=FrmSendRect.Bottom)))
			  ||(((FrmSendSlideEdge==3)||(FrmSendSlideEdge==4))&&((FrmSendRect.Left<=Mouse->CursorPos.x)&&(Mouse->CursorPos.x<=FrmSendRect.Right))))
			  {
				//Wlaczenie tymczasowej blokady wysuwania okna
				FrmSendBlockMousePositionTimer = true;
				//Wlaczenie timera wylaczajacego blokade
				SetTimer(hTimerFrm,TIMER_FRMSENDMOUSEPOSITION,1000,(TIMERPROC)TimerFrmProc);
			  }
			}
		  }
		}
		//FrmSendSlideOut (gdy kursor opusci okno)
		if(FrmSendSlideHideMode==3)
		{
		  //Gdy okno rozmowy jest widoczne, aktualnie nie jest chowane/wysuwane i nie jest aktywna blokada
		  if((FrmSendVisible)&&(!PreFrmSendSlideOut)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn)&&(!FrmSendBlockSlide))
		  {
			//Gdy kursor znajduje sie poza oknem rozmowy
			if((Mouse->CursorPos.y<FrmSendRect.Top+FrmSendRealTopPos-FrmSend_Shell_TrayWndTop)||(FrmSendRect.Bottom+FrmSendRealBottomPos+FrmSend_Shell_TrayWndBottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmSendRect.Left+FrmSendRealLeftPos-FrmSend_Shell_TrayWndLeft)||(FrmSendRect.Right+FrmSendRealRightPos+FrmSend_Shell_TrayWndRight<Mouse->CursorPos.x))
			{
			  //LPM nie jest wcisniety
			  if(GetKeyState(VK_LBUTTON)>=0)
			  {
				//Sprawdzenia okna w ktorym znajduje sie kursor
				HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
				wchar_t WClassName[128];
				GetClassNameW(hCurActiveFrm, WClassName, sizeof(WClassName));
				DWORD PID;
				GetWindowThreadProcessId(hCurActiveFrm, &PID);
				//Gdy kursor nie znajduje sie w obrebie menu z okna
				if(!(((UnicodeString)WClassName=="#32768")&&(PID==ProcessPID)))
				{
				  //Tymczasowa blokada FrmSendSlideOut
				  if(PopupMenuBlockSlide)
				  {
					//Zmiene blokady
					PopupMenuBlockSlide = false;
					FrmSendBlockSlide = true;
					//Wlaczenie timera wylaczanie tymczasowej blokady
					SetTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE,2000,(TIMERPROC)TimerFrmProc);
				  }
				  //FrmSendSlideOut
				  else
				  {
					//Status FrmSendSlideOut
					FrmSendSlideOut = true;
					PreFrmSendSlideOut = true;
					//Wlaczenie FrmSendSlideOut (part I)
					SetTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEOUT,FrmSendSlideOutDelay,(TIMERPROC)TimerFrmProc);
				  }
				}
				//Gdy kursor znajduje sie w obrebie menu z okna
				else if(((UnicodeString)WClassName=="#32768")&&(PID==ProcessPID)&&(FrmSendSlideHideMode==3))
				 PopupMenuBlockSlide = true;
			  }
			  //Tymczasowa blokada FrmSendSlideOut
			  else
			  {
				//Zmiene blokady
				PopupMenuBlockSlide = false;
				FrmSendBlockSlide = true;
				//Wlaczenie timera wylaczanie tymczasowej blokady
				SetTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE,2000,(TIMERPROC)TimerFrmProc);
			  }
			}
			else
			 PopupMenuBlockSlide = false;
		  }
		  else if((FrmSendVisible)&&(PreFrmSendSlideOut)&&(FrmSendSlideOut)&&(!FrmSendSlideIn)&&(!FrmSendBlockSlide))
		  {
			//Gdy kursor znajduje sie w oknie rozmowy
			if((Mouse->CursorPos.y>FrmSendRect.Top+FrmSendRealTopPos-FrmSend_Shell_TrayWndTop)&&(FrmSendRect.Bottom+FrmSendRealBottomPos+FrmSend_Shell_TrayWndBottom>Mouse->CursorPos.y)&&(Mouse->CursorPos.x>FrmSendRect.Left+FrmSendRealLeftPos-FrmSend_Shell_TrayWndLeft)&&(FrmSendRect.Right+FrmSendRealRightPos+FrmSend_Shell_TrayWndRight>Mouse->CursorPos.x))
			{
			  //Zatrzymanie timera
			  KillTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEOUT);
			  //Status FrmSendSlideOut
			  PreFrmSendSlideOut = false;
			  FrmSendSlideOut = false;
			}
		  }
		}
	  }
	  //SlideSlide dla okna kontaktow
	  if((FrmMainSlideChk)&&(!FrmMainBlockMousePositionTimer))
	  {
		//FrmMainSlideIn (gdy kursor zblizy sie do krawedzi ekranu)
		if((!FrmMainVisible)&&(!SecureMode)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
		{
		  //Blokada wysuwania okna przy wcisnietym klawiszu Ctrl/LPM/PPM
		  if((!SideSlideCtrlAndMouseBlockChk)||((SideSlideCtrlAndMouseBlockChk)&&(GetKeyState(VK_CONTROL)>=0)&&(GetKeyState(VK_LBUTTON)>=0)&&(GetKeyState(VK_RBUTTON)>=0))||((SideSlideCtrlAndMouseBlockChk)&&(GetKeyState(VK_CONTROL)<0)&&(GetKeyState(VK_LBUTTON)<0)))
		  {
			//Left or Right
			if(((FrmMainSlideEdge==1)&&(Mouse->CursorPos.x==0))
			||((FrmMainSlideEdge==2)&&(Mouse->CursorPos.x==Screen->Width-1))
			||((FrmMainSlideEdge==3)&&(Mouse->CursorPos.y==Screen->Height-1))
			||((FrmMainSlideEdge==4)&&(Mouse->CursorPos.y==0)))
			{
			  //Gdy kursor znajduje sie przy krawedzi ekranu z uwglednieniem miejsca schowania okna kontaktow
			  if((((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))&&((FrmMainRect.Top<=Mouse->CursorPos.y)&&(Mouse->CursorPos.y<=FrmMainRect.Bottom)))
			  ||(((FrmMainSlideEdge==3)||(FrmMainSlideEdge==4))&&((FrmMainRect.Left<=Mouse->CursorPos.x)&&(Mouse->CursorPos.x<=FrmMainRect.Right))))
			  {
				//Kursor nie znajduje sie w obrebie menu start
				wchar_t WClassName[128];
				GetClassNameW(WindowFromPoint(Mouse->CursorPos), WClassName, sizeof(WClassName));
				if(((UnicodeString)WClassName!="DesktopProgramsMFU")&&((UnicodeString)WClassName!="DV2ControlHost"))
				{
				  if(!PreFrmMainSlideIn)
				  {
					//Status FrmMainSlideIn
					PreFrmMainSlideIn = true;
					FrmMainDontBlockSlide = true;
					//Wlaczenie FrmMainSlideIn (part I)
					SetTimer(hTimerFrm,TIMER_PREFRMMAINSLIDEIN,FrmMainSlideInDelay,(TIMERPROC)TimerFrmProc);
				  }
				}
			  }
			  else if(PreFrmMainSlideIn)
			  {
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_PREFRMMAINSLIDEIN);
				//Status FrmMainSlideIn
				PreFrmMainSlideIn = false;
				FrmMainDontBlockSlide = false;
			  }
			}
			else
			{
			  //Zatrzymanie timera
			  KillTimer(hTimerFrm,TIMER_PREFRMMAINSLIDEIN);
			  //Status FrmMainSlideIn
			  PreFrmMainSlideIn = false;
			  FrmMainDontBlockSlide = false;
			}
		  }
		  //Tymczasowa blokada wysuwania okna
		  else if(FrmMainSlideInDelay<1000)
		  {
            //Left or Right
			if(((FrmMainSlideEdge==1)&&(Mouse->CursorPos.x==0))
			||((FrmMainSlideEdge==2)&&(Mouse->CursorPos.x==Screen->Width-1))
			||((FrmMainSlideEdge==3)&&(Mouse->CursorPos.y==Screen->Height-1))
			||((FrmMainSlideEdge==4)&&(Mouse->CursorPos.y==0)))
			{
              //Gdy kursor znajduje sie przy krawedzi ekranu z uwglednieniem miejsca schowania okna kontaktow
			  if((((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))&&((FrmMainRect.Top<=Mouse->CursorPos.y)&&(Mouse->CursorPos.y<=FrmMainRect.Bottom)))
			  ||(((FrmMainSlideEdge==3)||(FrmMainSlideEdge==4))&&((FrmMainRect.Left<=Mouse->CursorPos.x)&&(Mouse->CursorPos.x<=FrmMainRect.Right))))
			  {
				//Wlaczenie tymczasowej blokady wysuwania okna
				FrmMainBlockMousePositionTimer = true;
				//Wlaczenie timera wylaczajacego blokade
				SetTimer(hTimerFrm,TIMER_FRMMAINMOUSEPOSITION,1000,(TIMERPROC)TimerFrmProc);
              }
            }
          }
		}
		//FrmMainSlideOut (gdy kursor opusci okno)
		if(FrmMainSlideHideMode==3)
		{
		  //Gdy okno kontatkow jest widoczne, aktualnie nie jest chowane/wysuwane i nie jest aktywna blokada
		  if((FrmMainVisible)&&(!CurPreFrmMainSlideOut)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
		  {
			//Gdy kursor znajduje sie poza oknem kontaktow
			if((Mouse->CursorPos.y<FrmMainRect.Top+FrmMainRealTopPos-FrmMain_Shell_TrayWndTop)||(FrmMainRect.Bottom+FrmMainRealBottomPos+FrmMain_Shell_TrayWndBottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmMainRect.Left+FrmMainRealLeftPos-FrmMain_Shell_TrayWndLeft)||(FrmMainRect.Right+FrmMainRealRightPos+FrmMain_Shell_TrayWndRight<Mouse->CursorPos.x))
			{
			  //LPM nie jest wcisniety
			  if(GetKeyState(VK_LBUTTON)>=0)
			  {
				//Sprawdzenia okna w ktorym znajduje sie kursor
				HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
				wchar_t WClassName[128];
				GetClassNameW(hCurActiveFrm, WClassName, sizeof(WClassName));
				DWORD PID;
				GetWindowThreadProcessId(hCurActiveFrm, &PID);
				//Gdy kursor nie znajduje sie w obrebie menu z okna
				if(!(((UnicodeString)WClassName=="#32768")&&(PID==ProcessPID)))
				{
				  //Tymczasowa blokada FrmMainSlideOut
				  if(PopupMenuBlockSlide)
				  {
					PopupMenuBlockSlide = false;
					FrmMainBlockSlide = true;
					SetTimer(hTimerFrm,TIMER_FRMMAINBLOCKSLIDE,1500,(TIMERPROC)TimerFrmProc);
				  }
				  //FrmMainSlideOut
				  else
				  {
					//Status FrmMainSlideOut
					CurPreFrmMainSlideOut = true;
					//Wlaczenie FrmMainSlideOut (part I)
					SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,FrmMainSlideOutDelay,(TIMERPROC)TimerFrmProc);
				  }
				}
				//Gdy kursor znajduje sie w obrebie menu z okna
				else if(((UnicodeString)WClassName=="#32768")&&(PID==ProcessPID)&&(FrmMainSlideHideMode==3))
				 PopupMenuBlockSlide = true;
			  }
              //Tymczasowa blokada FrmMainSlideOut
			  else
			  {
				PopupMenuBlockSlide = false;
				FrmMainBlockSlide = true;
				SetTimer(hTimerFrm,TIMER_FRMMAINBLOCKSLIDE,1500,(TIMERPROC)TimerFrmProc);
			  }
			}
			else
			 PopupMenuBlockSlide = false;
		  }
		  else if((FrmMainVisible)&&(CurPreFrmMainSlideOut)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
		  {
			//Gdy kursor znajduje sie w oknie kontaktow
			if((Mouse->CursorPos.y>FrmMainRect.Top+FrmMainRealTopPos-FrmMain_Shell_TrayWndTop)&&(FrmMainRect.Bottom+FrmMainRealBottomPos+FrmMain_Shell_TrayWndBottom>Mouse->CursorPos.y)&&(Mouse->CursorPos.x>FrmMainRect.Left+FrmMainRealLeftPos-FrmMain_Shell_TrayWndLeft)&&(FrmMainRect.Right+FrmMainRealRightPos+FrmMain_Shell_TrayWndRight>Mouse->CursorPos.x))
			{
			  //Zatrzymanie timera
			  KillTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT);
			  //Status FrmMainSlideOut
			  CurPreFrmMainSlideOut = false;
			}
		  }
		}
	  }
	}
	//Wlacznie sprawdzania pozycji myszki dla okna rozmowy
	else if(wParam==TIMER_FRMSENDMOUSEPOSITION)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_FRMSENDMOUSEPOSITION);
	  //Wylaczenie tymczasowej blokady
	  FrmSendBlockMousePositionTimer = false;
	}
	//Wlacznie sprawdzania pozycji myszki dla okna kontaktow
	else if(wParam==TIMER_FRMMAINMOUSEPOSITION)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_FRMMAINMOUSEPOSITION);
	  //Wylaczenie tymczasowej blokady
	  FrmMainBlockMousePositionTimer = false;
	}
	//Sprawdzanie aktywnego okna
	else if(wParam==TIMER_CHKACTIVEWINDOW)
	{
	  //Pobieranie aktywnego okna
	  HWND hActiveFrm = GetForegroundWindow();
	  //Pobieranie klasy aktywnego okna
	  wchar_t WClassName[128];
	  GetClassNameW(hActiveFrm, WClassName, sizeof(WClassName));
	  //Ustawianie uchwytu do nowego aktywnego okna
	  if((hActiveFrm!=LastActiveWindow)&&(hActiveFrm!=hFrmSend)&&(hActiveFrm!=hFrmMain)&&(IsWindow(hActiveFrm)))
	  {
		if(((UnicodeString)WClassName!="ToolbarWindow32")
		&&((UnicodeString)WClassName!="MSTaskListWClass")
		&&((UnicodeString)WClassName!="TaskSwitcherWnd")
		&&((UnicodeString)WClassName!="Shell_TrayWnd")
		&&((UnicodeString)WClassName!="TrayShowDesktopButtonWClass"))
		 LastActiveWindow = hActiveFrm;
	  }
	  //Pobranie PID aktywnego okna
	  DWORD PID;
	  GetWindowThreadProcessId(hActiveFrm, &PID);
	  //Otworzenie okna tworzenia konferencji GG
	  if(((UnicodeString)WClassName=="TfrmCreateChat")&&(!FrmCreateChatExists)&&(PID==ProcessPID))
	  {
		//Okno istnieje
		FrmCreateChatExists = true;
		//Ustawienie statusu okna dla SideSlide
		FrmMainBlockSlide = true;
		FrmMainBlockSlideWndEvent = true;
	  }
	  //Zamkniecie okna tworzenia konferencji GG
	  else if(((UnicodeString)WClassName!="TfrmCreateChat")&&(FrmCreateChatExists)&&(PID==ProcessPID))
	  {
		//Okno nie istnieje
		FrmCreateChatExists = false;
		//Aktywacja okna kontaktow
		BringWindowToTop(hFrmMain);
		SetForegroundWindow(hFrmMain);
		SetFocus(hFrmMainFocus);
		SetActiveWindow(hFrmMain);
		//Ustawienie statusu okna dla SideSlide
		if(FrmMainSlideHideMode==3) SetTimer(hTimerFrm,TIMER_FRMMAINBLOCKSLIDE,1500,(TIMERPROC)TimerFrmProc);
		else
		{
		  FrmMainBlockSlide = false;
		  FrmMainBlockSlideWndEvent = false;
		}
	  }
	  //Chowanie gdy aplikacja straci fokus
	  if((FrmSendSlideChk)&&(FrmSendSlideHideMode==2))
	  {
		//Wlaczenie FrmSendSlideOut
		if((FrmSendVisible)&&(!FrmSendBlockSlide)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))
		{
		  //Gdy kursor znajduje sie poza oknem rozmowy
		  if((Mouse->CursorPos.y<FrmSendRect.Top)||(FrmSendRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmSendRect.Left)||(FrmSendRect.Right<Mouse->CursorPos.x))
		  {
			//Gdy okno nie jest innym oknem aplikacji
			if(PID!=ProcessPID)
			{
			  //Pobieranie klasy nowego aktywnego okna
			  GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
			  if((UnicodeString)WClassName!="TaskSwitcherWnd")
			  {
				//Status FrmSendSlideOut
				FrmSendSlideOut = true;
				//Ustawienie okna na wierzchu
				SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				//Wlaczenie FrmSendSlideOut (part I)
				SetTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
			  }
			}
		  }
		}
	  }
	  if((FrmMainSlideChk)&&(FrmMainSlideHideMode==2))
	  {
		//Wlaczenie FrmMainSlideOut
		if((FrmMainVisible)&&(!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
		{
		  //Gdy kursor znajduje sie poza oknem rozmowy
		  if((Mouse->CursorPos.y<FrmMainRect.Top)||(FrmMainRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmMainRect.Left)||(FrmMainRect.Right<Mouse->CursorPos.x))
		  {
			//Gdy okno nie jest innym oknem aplikacji
			if(PID!=ProcessPID)
			{
			  //Pobieranie klasy nowego aktywnego okna
			  GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
			  if((UnicodeString)WClassName!="TaskSwitcherWnd")
			  {
				//Status FrmMainSlideOut
				FrmMainSlideOut = true;
				//Ustawienie okna na wierzchu
				SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				//Wlaczenie FrmMainSlideOut (part I)
				SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
			  }
			}
		  }
		}
	  }
	  //Pobieranie okna w ktorym znajduje sie kursor
	  HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
	  //Pobieranie klasy okna w ktorym znajduje sie kursor
	  GetClassNameW(hCurActiveFrm, WClassName, sizeof(WClassName));
	  //Pobranie PID okna w ktorym znajduje sie kursor
	  GetWindowThreadProcessId(hCurActiveFrm, &PID);
	  //Gdy kursor znajduje sie w obrebie menu z okna aplikacji
	  if(((UnicodeString)WClassName=="#32768")&&(PID==ProcessPID))
	   hPopupMenu = hCurActiveFrm;
	}
	//FrmSendSlideOut (part I)
	else if(wParam==TIMER_PREFRMSENDSLIDEOUT)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEOUT);
	  //Status FrmSendSlideOut
	  PreFrmSendSlideOut = false;
	  //Ustawianie poprawnej pozycji okna
	  if((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))
	  {
		TRect WindowRect;
		GetWindowRect(hFrmSend,&WindowRect);
		if(WindowRect.Left!=FrmSendRect.Left)
		 SetWindowPos(hFrmSend,HWND_TOPMOST,WindowRect.Left,FrmSendRect.Top,0,0,SWP_ASYNCWINDOWPOS|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOSENDCHANGING);
	  }
	  else
	  {
		TRect WindowRect;
		GetWindowRect(hFrmSend,&WindowRect);
		if(WindowRect.Top!=FrmSendRect.Top)
		 SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendRect.Left,WindowRect.Top,0,0,SWP_ASYNCWINDOWPOS|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOSENDCHANGING);
	  }
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  GetFrmSendRect();
	  //Poczatkowa pozycja okna rozmowy podczas SlideIn/SlideOut
	  FrmSendSlideLeft = FrmSendRect.Left;
	  FrmSendSlideTop = FrmSendRect.Top;
	  //Wlacznie FrmSendSlideIn (part II)
	  SetTimer(hTimerFrm,TIMER_FRMSENDSLIDEOUT,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
	}
	//FrmSendSlideOut (part II)
	else if(wParam==TIMER_FRMSENDSLIDEOUT)
	{
	  //Ustalanie ilosci krokow
	  int Steps = FrmSendSlideOutTime / FrmSendStepInterval;
	  //Left or Right
	  if((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))
	  {
		//Ustalanie ilosci krokow
		Steps = (FrmSendRect.Width()+FrmSend_Shell_TrayWndRight+FrmSend_Shell_TrayWndLeft)/Steps;
		//Left
		if(FrmSendSlideEdge==1) FrmSendSlideLeft = FrmSendSlideLeft - Steps;
		//Right
		else FrmSendSlideLeft = FrmSendSlideLeft + Steps;
		//Zmiana pozycji okna rozmowy
		SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendSlideLeft,FrmSendRect.Top,0,0,SWP_ASYNCWINDOWPOS|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOSENDCHANGING);
	  }
	  //Bottom or Top
	  else
	  {
		//Ustalanie ilosci krokow
		Steps = (FrmSendRect.Height()+FrmSend_Shell_TrayWndBottom+FrmSend_Shell_TrayWndTop)/Steps;
		//Bottom
		if(FrmSendSlideEdge==3) FrmSendSlideTop = FrmSendSlideTop + Steps;
		//Top
		else FrmSendSlideTop = FrmSendSlideTop - Steps;
		//Zmiana pozycji okna rozmowy
		SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendRect.Left,FrmSendSlideTop,0,0,SWP_ASYNCWINDOWPOS|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOSENDCHANGING);
	  }
	  //Koncowy etap
	  if(((FrmSendSlideEdge==1)&&(FrmSendSlideLeft<(0-FrmSendRect.Right)))
	  ||((FrmSendSlideEdge==2)&&(FrmSendSlideLeft>Screen->Width))
	  ||((FrmSendSlideEdge==3)&&(FrmSendSlideTop>Screen->Height))
	  ||((FrmSendSlideEdge==4)&&(FrmSendSlideTop<(0-FrmSendRect.Bottom))))
	  {
		//Zatrzymanie timera
		KillTimer(hTimerFrm,TIMER_FRMSENDSLIDEOUT);
		//Aplikacja pelno ekranowa
		if(FullScreenMode)
		{
		  HWND hHideFrm = FindWindow(L"Shell_TrayWnd",NULL);
		  ShowWindow(hHideFrm,SW_SHOW);
		  hHideFrm = FindWindow(L"Button",L"Start");
		  if(hHideFrm) ShowWindow(hHideFrm,SW_SHOW);
		  SetForegroundWindow(FullScreenWindow);
		  FullScreenMode = false;
		}
		//Status FrmSendSlideOut
		FrmSendSlideOut = false;
		FrmSendVisible = false;
		//Aktywacja poprzedniego okna, jezeli nie jest aktywne chowanie przy utracie fokusa calej aplikacji
		if((FrmMainSlideChk)&&(FrmMainSlideHideMode==2)&&(FrmSendSlideHideMode!=2)&&(FrmMainVisible)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
		{
		  //Aktywacja okna kontaktow
		  BringWindowToTop(hFrmMain);
		  SetForegroundWindow(hFrmMain);
		  SetFocus(hFrmMainFocus);
		  SetActiveWindow(hFrmMain);
		}
		else
		{
		  //Gdy okno nie bylo chowane po straceniu fokusa calej aplikacji
		  if(FrmSendSlideHideMode!=2)
		  {
			bool ActiveFrmMain = false;
			if((FrmMainSlideChk)&&(FrmMainSlideHideMode==3)&&(FrmSendSlideHideMode==3)&&(FrmMainVisible)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
			{
			  //Gdy kursor znajduje sie w oknie kontaktow
			  if((Mouse->CursorPos.y>FrmMainRect.Top)&&(FrmMainRect.Bottom>Mouse->CursorPos.y)&&(Mouse->CursorPos.x>FrmMainRect.Left)&&(FrmMainRect.Right>Mouse->CursorPos.x))
			  {
				//Aktywacja okna kontaktow
				BringWindowToTop(hFrmMain);
				SetForegroundWindow(hFrmMain);
				SetFocus(hFrmMainFocus);
				SetActiveWindow(hFrmMain);
				ActiveFrmMain = true;
			  }
			}
			if(!ActiveFrmMain)
			{
			  DWORD PID;
			  GetWindowThreadProcessId(LastActiveWindow, &PID);
			  if(PID!=ProcessPID)
			  {
				if((LastActiveWindow_WmInactiveFrmSendSlide)&&(LastActiveWindow_WmInactiveFrmSendSlide!=hFrmMain)&&(LastActiveWindow_WmInactiveFrmSendSlide==hFrmSend))
				 SetForegroundWindow(LastActiveWindow_WmInactiveFrmSendSlide);
				else if((LastActiveWindow_PreFrmSendSlideIn)&&(LastActiveWindow_PreFrmSendSlideIn!=hFrmMain)&&(LastActiveWindow_PreFrmSendSlideIn==hFrmSend))
				 SetForegroundWindow(LastActiveWindow_PreFrmSendSlideIn);
				else if(LastActiveWindow)
				 SetForegroundWindow(LastActiveWindow);
			  }
			  else
			   SetForegroundWindow(LastActiveWindow);
			}
		  }
		  //Aktywacja okna spod kursora
		  else
		   SetForegroundWindow(WindowFromPoint(Mouse->CursorPos));
		}
		//Usuniecie uchwytow do nowego aktywnego okna
		LastActiveWindow_WmInactiveFrmSendSlide = NULL;
		LastActiveWindow_PreFrmSendSlideIn = NULL;
	  }
	}
	//FrmSendSlideIn (part I)
	else if(wParam==TIMER_PREFRMSENDSLIDEIN)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEIN);
	  //Pobieranie nowego aktywnego okna
	  wchar_t WClassName[128];
	  GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
	  if(((UnicodeString)WClassName!="ToolbarWindow32")
	  &&((UnicodeString)WClassName!="MSTaskListWClass")
	  &&((UnicodeString)WClassName!="TaskSwitcherWnd")
	  &&((UnicodeString)WClassName!="Shell_TrayWnd")
	  &&((UnicodeString)WClassName!="TrayShowDesktopButtonWClass"))
	   LastActiveWindow_PreFrmSendSlideIn = GetForegroundWindow();
	  //Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
	  ChkFullScreenMode();
	  //Blokowanie wysuwania przy aplikacji pelnoekranowej
	  if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
	  {
		//Blokada FrmSendSlideIn
		FrmSendSlideIn = false;
		PreFrmSendSlideIn = false;
		FrmSendDontBlockSlide = false;
		FullScreenMode = false;
		FullScreenModeExeptions = false;
	  }
	  else
	  {
		//Status FrmSendSlideIn
		FrmSendSlideIn = true;
		PreFrmSendSlideIn = false;
		//Pobieranie pozycji okna rozmowy
		TRect WindowRect;
		GetWindowRect(hFrmSend,&WindowRect);
		//Przywracanie okna
		if(IsIconic(hFrmSend))
		{
		  //Status SideSlide
		  FrmSendBlockSlide = true;
		  //Przywracanie okna rozmowy
		  ShowWindow(hFrmSend,SW_RESTORE);
		  BringWindowToTop(hFrmSend);
		  SetForegroundWindow(hFrmSend);
		  //Status SideSlide
		  FrmSendRestoreRect = true;
		  FrmSendBlockSlide = false;
		}
		//Odswiezenie okna rozmowy
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height()+1,SWP_NOMOVE);
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
		//Wlacznie FrmSendSlideIn (part II)
		SetTimer(hTimerFrm,TIMER_FRMSENDSLIDEIN,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
	  }
	}
	//FrmSendSlideIn (part II)
	else if(wParam==TIMER_FRMSENDSLIDEIN)
	{
	  //Przywracanie poprawnych wymiarow okna
	  if(FrmSendRestoreRect)
	  {
		FrmSendRestoreRect = false;
		SetWindowPos(hFrmSend,NULL,0,0,FrmSendRect.Width(),FrmSendRect.Height(),SWP_NOMOVE|SWP_NOACTIVATE);
	  }
	  int Steps = FrmSendSlideInTime / FrmSendStepInterval;
	  //Left or Right
	  if((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))
	  {
		//Ustalanie ilosci krokow
		Steps = (FrmSendRect.Width()+FrmSend_Shell_TrayWndRight+FrmSend_Shell_TrayWndLeft)/Steps;
		//Left
		if(FrmSendSlideEdge==1)
		{
		  FrmSendSlideLeft = FrmSendSlideLeft + Steps;
		  if(FrmSendSlideLeft>FrmSendRect.Left) FrmSendSlideLeft = FrmSendRect.Left;
		}
		//Right
		else
		{
		  FrmSendSlideLeft = FrmSendSlideLeft - Steps;
		  if(FrmSendSlideLeft<FrmSendRect.Left) FrmSendSlideLeft = FrmSendRect.Left;
		}
		if(!IsWindowVisible(hFrmSend)) ShowWindow(hFrmSend, SW_SHOWNA);
		//Zmiana pozycji okna rozmowy
		SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendSlideLeft,FrmSendRect.Top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	  }
	  else
	  {
		//Ustalanie ilosci krokow
		Steps = (FrmSendRect.Height()+FrmSend_Shell_TrayWndBottom+FrmSend_Shell_TrayWndTop)/Steps;
		//Bottom
		if(FrmSendSlideEdge==3)
		{
		  FrmSendSlideTop = FrmSendSlideTop - Steps;
		  if(FrmSendSlideTop<FrmSendRect.Top) FrmSendSlideTop = FrmSendRect.Top;
		}
		//Top
		else
		{
		  FrmSendSlideTop = FrmSendSlideTop + Steps;
		  if(FrmSendSlideTop>FrmSendRect.Top) FrmSendSlideTop = FrmSendRect.Top;
		}
		if(!IsWindowVisible(hFrmSend)) ShowWindow(hFrmSend, SW_SHOWNA);
		//Zmiana pozycji okna rozmowy
		SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendRect.Left,FrmSendSlideTop,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	  }
	  //Koncowy etap
	  if((((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))&&(FrmSendSlideLeft==FrmSendRect.Left))
	  ||(((FrmSendSlideEdge==3)||(FrmSendSlideEdge==4))&&(FrmSendSlideTop==FrmSendRect.Top)))
	  {
		//Aplikacja pelno ekranowa
		if(FullScreenMode)
		{
		  FullScreenWindow = GetForegroundWindow();
		  HWND hHideFrm = FindWindow(L"Shell_TrayWnd",NULL);
		  ShowWindow(hHideFrm,SW_HIDE);
		  hHideFrm = FindWindow(L"Button",L"Start");
		  if(hHideFrm) ShowWindow(hHideFrm,SW_HIDE);
		}
		//Aktywacja okna
		SetForegroundWindow(hFrmSend);
		//Aktywacja pola wpisywania tekstu
		if((!DragDetect(hFrmSend,Mouse->CursorPos))&&(hRichEdit))
		{
		  //Blokada lokalnego hooka na myszke
		  FrmSendActivate = true;
		  //Emulacja klikniecia myszka w pole tekstowe
		  TRect RichEditRect;
		  GetWindowRect(hRichEdit,&RichEditRect);
		  POINT pCur;
		  GetCursorPos(&pCur);
		  SetCursorPos(RichEditRect.Left+1,RichEditRect.Top+1);
		  mouse_event(MOUSEEVENTF_LEFTDOWN,RichEditRect.Left+1,RichEditRect.Top+1,0,0);
		  mouse_event(MOUSEEVENTF_LEFTUP,RichEditRect.Left+1,RichEditRect.Top+1,0,0);
		  SetCursorPos(pCur.x,pCur.y);
		  //Wlaczenie timera wylaczania blokady lokalnego hooka na myszke
		  SetTimer(hTimerFrm,TIMER_FRMSENDUNBLOCKMOUSE,100,(TIMERPROC)TimerFrmProc);
		}
		//Wylaczenie statusu okna na wierzchu
		if(FrmSendSlideHideMode==2)
		 SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		//Status FrmSendSlideIn
		FrmSendSlideIn = false;
		FrmSendVisible = true;
		//Tymczasowa blokada FrmSendSlideOut
		if((FrmSendSlideHideMode==3)&&(!FrmSendDontBlockSlide))
		{
		  FrmSendBlockSlide = true;
		  //Wlaczenie timera wylaczanie tymczasowej blokady
		  SetTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE,2000,(TIMERPROC)TimerFrmProc);
		}
		FrmSendDontBlockSlide = false;
		//Zatrzymanie timera
		KillTimer(hTimerFrm,TIMER_FRMSENDSLIDEIN);
	  }
	}
	//SideSlide - tymczasowa blokada FrmSendSlideOut
	else if(wParam==TIMER_FRMSENDBLOCKSLIDE)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE);
	  //Usuniecie blokady
	  if(StayOnTopChk) FrmSendBlockSlide = StayOnTopStatus;
	  else FrmSendBlockSlide = false;
	  FrmSendBlockSlideWndEvent = false;
	  FrmSendBlockSlideOnMsgComposing = false;
	}
	//Blokada minimalizacji okna rozmowy
	else if(wParam==TIMER_FRMSENDMINIMIZED)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_FRMSENDMINIMIZED);
	  //Aktywacja okna
	  ShowWindow(hFrmSend,SW_RESTORE);
	  //Status SideSlide
	  FrmSendBlockSlideOnRestore = false;
	  FrmSendBlockSlide = false;
	  //Status FrmSendSlideOut
	  FrmSendSlideOut = true;
	  //Wlacznie FrmSendSlideIn (part II)
	  SetTimer(hTimerFrm,TIMER_FRMSENDSLIDEOUT,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
	}
	//Ustawienie prawidlowej pozycji okna rozmowy
	else if(wParam==TIMER_FRMSENDCHANGEPOS)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_FRMSENDCHANGEPOS);
	  //Wylaczenie funkcjanalnosci
	  FrmSendSlideChk = false;
	  //Przywracanie okna rozmowy
	  if(IsIconic(hFrmSend))
	  {
		ShowWindow(hFrmSend,SW_RESTORE);
		BringWindowToTop(hFrmSend);
		SetForegroundWindow(hFrmSend);
	  }
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  TRect WindowRect;
	  GetWindowRect(hFrmSend,&WindowRect);
	  //Ustawianie wstepnej pozycji okna rozmowy
	  //Left
	  if(FrmSendSlideEdge==1)
	   SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
	  //Right
	  else if(FrmSendSlideEdge==2)
	   SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-WindowRect.Width()-FrmSend_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
	  //Bottom
	  else if(FrmSendSlideEdge==3)
	   SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
	  //Top
	  else
	   SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,FrmSend_Shell_TrayWndTop,0,0,SWP_NOSIZE);
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  GetFrmSendRect();
	  //Ustawienie poprawnej pozycji okna rozmowy
	  SetFrmSendPos();
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  GetFrmSendRect();
	  //Zapisanie pozycji okna rozmowy do ustawiem AQQ
	  TSaveSetup SaveSetup;
	  SaveSetup.Section = L"Position";
	  SaveSetup.Ident = L"MsgLeft";
	  SaveSetup.Value = IntToStr((int)FrmSendRect.Left).w_str();
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  SaveSetup.Section = L"Position";
	  SaveSetup.Ident = L"MsgTop";
	  SaveSetup.Value = IntToStr((int)FrmSendRect.Top).w_str();
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  //Status okna rozmowy
	  FrmSendVisible = true;
	  //Wlaczenie funkcjanalnosci
	  FrmSendSlideChk = true;
	  //Wlaczenie timera wylaczanie tymczasowej blokady
	  if(FrmSendSlideHideMode==3) SetTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE,2000,(TIMERPROC)TimerFrmProc);
	  //Wylaczenie tymczasowej blokady
	  else FrmSendBlockSlide = false;
	}
	//SideSlide - pobranie nowego uchwytu i aktywacja okna rozmowy
	else if(wParam==TIMER_FRMSENDSETTOPMOST)
	{
	  //Pobieranie klasy nowego okna
	  wchar_t WClassName[128];
	  GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
	  //Wlaczenie timera ustawienia okna na wierzchu
	  if(((UnicodeString)WClassName!="TaskSwitcherWnd")&&((UnicodeString)WClassName!="DV2ControlHost")&&((UnicodeString)WClassName!="Shell_TrayWnd"))
	  {
		//Zatrzymanie timera
		KillTimer(hTimerFrm,TIMER_FRMSENDSETTOPMOST);
		//Pobranie uchwytu
		LastActiveWindow_PreFrmSendSlideIn = GetForegroundWindow();
		//Ustawienie okna rozmowy na wierzchu
		SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  }
	}
	//SideSlide - pobranie nowego uchwytu i schowanie okna rozmowy
	else if(wParam==TIMER_FRMSENDSETTOPMOSTANDSLIDE)
	{
	  //Pobieranie klasy nowego okna
	  wchar_t WClassName[128];
	  GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
	  //Wlaczenie timera ustawienia okna na wierzchu
	  if(((UnicodeString)WClassName!="TaskSwitcherWnd")&&((UnicodeString)WClassName!="DV2ControlHost"))
	  {
		//Zatrzymanie timera
		KillTimer(hTimerFrm,TIMER_FRMSENDSETTOPMOSTANDSLIDE);
		if(GetForegroundWindow()==hFrmSend)
		{
		  //Status FrmSendSlideOut
		  FrmSendSlideOut = true;
		  PreFrmSendSlideOut = true;
		  //Pobieranie nowego aktywnego okna
		  LastActiveWindow_WmInactiveFrmSendSlide = GetForegroundWindow();
		  //Ponowna aktywacja okna
		  SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		  //Wlaczenie FrmSendSlideOut (part I)
		  SetTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
		}
		else
		{
		  //Pobranie uchwytu
		  LastActiveWindow_PreFrmSendSlideIn = GetForegroundWindow();
		  //Aktywacja okna
		  SetForegroundWindow(hFrmSend);
		  //Aktywacja pola wpisywania tekstu
		  if((!DragDetect(hFrmSend,Mouse->CursorPos))&&(hRichEdit))
		  {
			//Blokada lokalnego hooka na myszke
			FrmSendActivate = true;
			//Emulacja klikniecia myszka w pole tekstowe
			TRect RichEditRect;
			GetWindowRect(hRichEdit,&RichEditRect);
			POINT pCur;
			GetCursorPos(&pCur);
			SetCursorPos(RichEditRect.Left+1,RichEditRect.Top+1);
			mouse_event(MOUSEEVENTF_LEFTDOWN,RichEditRect.Left+1,RichEditRect.Top+1,0,0);
			mouse_event(MOUSEEVENTF_LEFTUP,RichEditRect.Left+1,RichEditRect.Top+1,0,0);
			SetCursorPos(pCur.x,pCur.y);
			//Wlaczenie timera wylaczania blokady lokalnego hooka na myszke
			SetTimer(hTimerFrm,TIMER_FRMSENDUNBLOCKMOUSE,100,(TIMERPROC)TimerFrmProc);
		  }
		}
	  }
	}
	//Wylaczanie blokady lokalnego hooka na myszke
	else if(wParam==TIMER_FRMSENDUNBLOCKMOUSE)
	{
      //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_FRMSENDUNBLOCKMOUSE);
	  //Wylaczenie blokady
      FrmSendActivate = false;
    }
	//FrmMainSlideOut (part I)
	else if(wParam==TIMER_PREFRMMAINDSLIDEOUT)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT);
	  //Status FrmMainSlideOut
	  FrmMainSlideOut = true;
	  PreFrmMainSlideOut = false;
	  CurPreFrmMainSlideOut = false;
	  //Ustawianie poprawnej pozycji okna
	  if((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))
	  {
		TRect WindowRect;
		GetWindowRect(hFrmMain,&WindowRect);
		if(WindowRect.Left!=FrmMainRect.Left)
		 SetWindowPos(hFrmMain,HWND_TOPMOST,WindowRect.Left,FrmMainRect.Top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	  }
	  else
	  {
		TRect WindowRect;
		GetWindowRect(hFrmMain,&WindowRect);
		if(WindowRect.Top!=FrmMainRect.Top)
		 SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainRect.Left,WindowRect.Top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	  }
	  //Pobranie rozmiaru+pozycji okna kontatkow
	  GetFrmMainRect();
	  //Poczatkowa pozycja okna kontaktow podczas SlideIn/SlideOut
	  FrmMainSlideLeft = FrmMainRect.Left;
	  FrmMainSlideTop = FrmMainRect.Top;
	  //Wlacznie FrmSendSlideIn (part II)
	  SetTimer(hTimerFrm,TIMER_FRMMAINSLIDEOUT,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
	}
	//FrmMainSlideOut (part II)
	else if(wParam==TIMER_FRMMAINSLIDEOUT)
	{
	  //Ustalanie ilosci krokow
	  int Steps = FrmMainSlideOutTime / FrmMainStepInterval;
	  //Left or Right
	  if((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))
	  {
		//Ustalanie ilosci krokow
		Steps = (FrmMainRect.Width()+FrmMain_Shell_TrayWndRight+FrmMain_Shell_TrayWndLeft)/Steps;
		//Left
		if(FrmMainSlideEdge==1) FrmMainSlideLeft = FrmMainSlideLeft - Steps;
		//Right
		else FrmMainSlideLeft = FrmMainSlideLeft + Steps;
		//Ponowna aktywacja okna kontaktow
		FrmMainSlideOutWndActiv = true;
		BringWindowToTop(hFrmMain);
		SetForegroundWindow(hFrmMain);
		SetFocus(hFrmMainFocus);
		SetActiveWindow(hFrmMain);
		FrmMainSlideOutWndActiv = false;
		//Zmiana pozycji okna kontaktow
		SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainSlideLeft,FrmMainRect.Top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	  }
	  //Bottom or Top
	  else
	  {
		//Ustalanie ilosci krokow
		Steps = (FrmMainRect.Height()+FrmMain_Shell_TrayWndBottom+FrmMain_Shell_TrayWndTop)/Steps;
		//Bottom
		if(FrmMainSlideEdge==3) FrmMainSlideTop = FrmMainSlideTop + Steps;
		//Top
		else FrmMainSlideTop = FrmMainSlideTop - Steps;
		//Ponowna aktywacja okna kontaktow
		FrmMainSlideOutWndActiv = true;
		BringWindowToTop(hFrmMain);
		SetForegroundWindow(hFrmMain);
		SetFocus(hFrmMainFocus);
		SetActiveWindow(hFrmMain);
		FrmMainSlideOutWndActiv = false;
		//Zmiana pozycji okna kontaktow
		SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainRect.Left,FrmMainSlideTop,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	  }
	  //Koncowy etap
	  if(((FrmMainSlideEdge==1)&&(FrmMainSlideLeft<(0-FrmMainRect.Right)))
	  ||((FrmMainSlideEdge==2)&&(FrmMainSlideLeft>Screen->Width))
	  ||((FrmMainSlideEdge==3)&&(FrmMainSlideTop>Screen->Height))
	  ||((FrmMainSlideEdge==4)&&(FrmMainSlideTop<(0-FrmMainRect.Bottom))))
	  {
		//Aplikacja pelno ekranowa
		if(FullScreenMode)
		{
		  HWND hHideFrm = FindWindow(L"Shell_TrayWnd",NULL);
		  ShowWindow(hHideFrm,SW_SHOW);
		  hHideFrm = FindWindow(L"Button",L"Start");
		  if(hHideFrm) ShowWindow(hHideFrm,SW_SHOW);
		  SetForegroundWindow(FullScreenWindow);
		  FullScreenMode = false;
		}
		//Status FrmMainSlideOut
		FrmMainSlideOut = false;
		FrmMainVisible = false;
		//Aktywacja poprzedniego okna
		if((FrmSendSlideChk)&&((FrmSendBlockSlideOnMsgComposing)||((FrmSendVisible)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))))
		{
		  //Okno tworzenia wycinka nie jest aktywne
		  if(!FrmPosExist)
		  {
			//Aktywacja okna rozmowy
			SetForegroundWindow(hFrmSend);
			//Aktywacja pola wpisywania tekstu
			if((!DragDetect(hFrmSend,Mouse->CursorPos))&&(hRichEdit))
			{
			  //Blokada lokalnego hooka na myszke
			  FrmSendActivate = true;
			  //Emulacja klikniecia myszka w pole tekstowe
			  TRect RichEditRect;
			  GetWindowRect(hRichEdit,&RichEditRect);
			  POINT pCur;
			  GetCursorPos(&pCur);
			  SetCursorPos(RichEditRect.Left+1,RichEditRect.Top+1);
			  mouse_event(MOUSEEVENTF_LEFTDOWN,RichEditRect.Left+1,RichEditRect.Top+1,0,0);
			  mouse_event(MOUSEEVENTF_LEFTUP,RichEditRect.Left+1,RichEditRect.Top+1,0,0);
			  SetCursorPos(pCur.x,pCur.y);
			  //Wlaczenie timera wylaczania blokady lokalnego hooka na myszke
			  SetTimer(hTimerFrm,TIMER_FRMSENDUNBLOCKMOUSE,100,(TIMERPROC)TimerFrmProc);
			}
		  }
		}
		else
		{
		  //Gdy okno nie bylo chowane po straceniu fokusa calej aplikacji
		  if(FrmMainSlideHideMode!=2)
		  {
			//Okno kontaktow zostalo schowane nie przez aktywacje z okna rozmowy
			if(!FrmMainSlideOutActivFrmSend)
			{
			  DWORD PID;
			  GetWindowThreadProcessId(LastActiveWindow, &PID);
			  if(PID!=ProcessPID)
			  {
				if((LastActiveWindow_WmInactiveFrmMainSlide)&&(LastActiveWindow_WmInactiveFrmMainSlide!=hFrmMain)&&(LastActiveWindow_WmInactiveFrmMainSlide==hFrmSend))
				 SetForegroundWindow(LastActiveWindow_WmInactiveFrmMainSlide);
				else if((LastActiveWindow_PreFrmMainSlideIn)&&(LastActiveWindow_PreFrmMainSlideIn!=hFrmMain)&&(LastActiveWindow_PreFrmMainSlideIn==hFrmSend))
				 SetForegroundWindow(LastActiveWindow_PreFrmMainSlideIn);
				else
				 SetForegroundWindow(LastActiveWindow);
			  }
			  else
			   SetForegroundWindow(LastActiveWindow);
			}
			//Okno kontaktow zostalo schowane przez aktywacje okna rozmowy
			else
			{
			  //Status FrmMainSlideOut
			  FrmMainSlideOutActivFrmSend = false;
			  //Aktywacja okna
			  SetForegroundWindow(hFrmSend);
			  //Aktywacja pola wpisywania tekstu
			  if((!DragDetect(hFrmSend,Mouse->CursorPos))&&(hRichEdit))
			  {
				//Blokada lokalnego hooka na myszke
				FrmSendActivate = true;
				//Emulacja klikniecia myszka w pole tekstowe
				TRect RichEditRect;
				GetWindowRect(hRichEdit,&RichEditRect);
				POINT pCur;
				GetCursorPos(&pCur);
				SetCursorPos(RichEditRect.Left+1,RichEditRect.Top+1);
				mouse_event(MOUSEEVENTF_LEFTDOWN,RichEditRect.Left+1,RichEditRect.Top+1,0,0);
				mouse_event(MOUSEEVENTF_LEFTUP,RichEditRect.Left+1,RichEditRect.Top+1,0,0);
				SetCursorPos(pCur.x,pCur.y);
				//Wlaczenie timera wylaczania blokady lokalnego hooka na myszke
				SetTimer(hTimerFrm,TIMER_FRMSENDUNBLOCKMOUSE,100,(TIMERPROC)TimerFrmProc);
			  }
			}
		  }
		  //Aktywacja okna spod kursora
		  else
		   SetForegroundWindow(WindowFromPoint(Mouse->CursorPos));
		}
		//Usuniecie uchwytow do nowego aktywnego okna
		LastActiveWindow_WmInactiveFrmMainSlide = NULL;
		LastActiveWindow_PreFrmMainSlideIn = NULL;
		//Zatrzymanie timera
		KillTimer(hTimerFrm,TIMER_FRMMAINSLIDEOUT);
	  }
	}
	//FrmMainSlideIn (part I)
	else if(wParam==TIMER_PREFRMMAINSLIDEIN)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_PREFRMMAINSLIDEIN);
	  //Pobieranie nowego aktywnego okna
	  wchar_t WClassName[128];
	  GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
	  if(((UnicodeString)WClassName!="ToolbarWindow32")
	  &&((UnicodeString)WClassName!="MSTaskListWClass")
	  &&((UnicodeString)WClassName!="TaskSwitcherWnd")
	  &&((UnicodeString)WClassName!="Shell_TrayWnd")
	  &&((UnicodeString)WClassName!="TrayShowDesktopButtonWClass"))
	   LastActiveWindow_PreFrmMainSlideIn = GetForegroundWindow();
	  //Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
	  ChkFullScreenMode();
	  //Blokowanie wysuwania przy aplikacji pelnoekranowej
	  if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
	  {
		//Blokada FrmMainSlideIn
		FrmMainSlideIn = false;
		PreFrmMainSlideIn = false;
		FrmMainDontBlockSlide = false;
		FullScreenMode = false;
	  }
	  else
	  {
		//Status FrmMainSlideIn
		FrmMainSlideIn = true;
		PreFrmMainSlideIn = false;
		//Aktywacja okna kontaktow
		BringWindowToTop(hFrmMain);
		SetForegroundWindow(hFrmMain);
		//Przelaczanie widoku na liste kontatkow
		if(ChangeTabAfterSlideIn) PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aJabber");
		//Wlacznie FrmMainSlideIn (part II)
		SetTimer(hTimerFrm,TIMER_FRMMAINSLIDEIN,FrmMainStepInterval,(TIMERPROC)TimerFrmProc);
	  }
	}
	//FrmMainSlideIn (part II)
	else if(wParam==TIMER_FRMMAINSLIDEIN)
	{
	  int Steps = FrmMainSlideInTime / FrmMainStepInterval;
	  //Left or Right
	  if((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))
	  {
		//Ustalanie ilosci krokow
		Steps = (FrmMainRect.Width()+FrmMain_Shell_TrayWndRight+FrmMain_Shell_TrayWndLeft)/Steps;
		//Left
		if(FrmMainSlideEdge==1)
		{
		  FrmMainSlideLeft = FrmMainSlideLeft + Steps;
		  if(FrmMainSlideLeft>FrmMainRect.Left) FrmMainSlideLeft = FrmMainRect.Left;
		}
		//Right
		else
		{
		  FrmMainSlideLeft = FrmMainSlideLeft - Steps;
		  if(FrmMainSlideLeft<FrmMainRect.Left) FrmMainSlideLeft = FrmMainRect.Left;
		}
		if(!IsWindowVisible(hFrmMain)) ShowWindow(hFrmMain, SW_SHOWNA);
		//Zmiana pozycji okna kontaktow
		SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainSlideLeft,FrmMainRect.Top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	  }
	  else
	  {
		//Ustalanie ilosci krokow
		Steps = (FrmMainRect.Height()+FrmMain_Shell_TrayWndBottom+FrmMain_Shell_TrayWndTop)/Steps;
		//Bottom
		if(FrmMainSlideEdge==3)
		{
		  FrmMainSlideTop = FrmMainSlideTop - Steps;
		  if(FrmMainSlideTop<FrmMainRect.Top) FrmMainSlideTop = FrmMainRect.Top;
		}
		//Top
		else
		{
		  FrmMainSlideTop = FrmMainSlideTop + Steps;
		  if(FrmMainSlideTop>FrmMainRect.Top) FrmMainSlideTop = FrmMainRect.Top;
		}
		if(!IsWindowVisible(hFrmMain)) ShowWindow(hFrmMain, SW_SHOWNA);
		//Zmiana pozycji okna kontaktow
		SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainRect.Left,FrmMainSlideTop,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	  }
	  //Koncowy etap
	  if((((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))&&(FrmMainSlideLeft==FrmMainRect.Left))
	  ||(((FrmMainSlideEdge==3)||(FrmMainSlideEdge==4))&&(FrmMainSlideTop==FrmMainRect.Top)))
	  {
		//Aplikacja pelno ekranowa
		if(FullScreenMode)
		{
		  FullScreenWindow = GetForegroundWindow();
		  HWND hHideFrm = FindWindow(L"Shell_TrayWnd",NULL);
		  ShowWindow(hHideFrm,SW_HIDE);
		  if(hHideFrm) hHideFrm = FindWindow(L"Button",L"Start");
		  ShowWindow(hHideFrm,SW_HIDE);
		}
		//Aktywacja okna kontaktow
		BringWindowToTop(hFrmMain);
		SetForegroundWindow(hFrmMain);
		SetFocus(hFrmMainFocus);
		SetActiveWindow(hFrmMain);
		//Wylaczenie statusu okna na wierzchu
		if(FrmMainSlideHideMode==2) SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		//Status FrmMainSlideIn
		FrmMainSlideIn = false;
		FrmMainVisible = true;
		//Tymczasowa blokada FrmMainSlideOut
		if((FrmMainSlideHideMode==3)&&(!FrmMainDontBlockSlide))
		{
		  FrmMainBlockSlide = true;
		  SetTimer(hTimerFrm,TIMER_FRMMAINBLOCKSLIDE,1500,(TIMERPROC)TimerFrmProc);
		}
		FrmMainDontBlockSlide = false;
		//Zatrzymanie timera
		KillTimer(hTimerFrm,TIMER_FRMMAINSLIDEIN);
	  }
	}
	//SideSlide - tymczasowa blokada FrmMainSlideOut
	else if(wParam==TIMER_FRMMAINBLOCKSLIDE)
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_FRMMAINBLOCKSLIDE);
	  //Usuniecie blokady
	  FrmMainBlockSlide = false;
	  FrmMainBlockSlideWndEvent = false;
	}
	//SideSlide - ustawienie okna kontaktow na wierzchu
	else if(wParam==TIMER_FRMMAINSETTOPMOST)
	{
	  //Pobieranie klasy nowego okna
	  wchar_t WClassName[128];
	  GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
	  //Wlaczenie timera ustawienia okna na wierzchu
	  if(((UnicodeString)WClassName!="TaskSwitcherWnd")&&((UnicodeString)WClassName!="DV2ControlHost")&&((UnicodeString)WClassName!="Shell_TrayWnd"))
	  {
		//Zatrzymanie timera
		KillTimer(hTimerFrm,TIMER_FRMMAINSETTOPMOST);
		//Pobranie uchwytu
		LastActiveWindow_PreFrmMainSlideIn = GetForegroundWindow();
		//Aktywacja okna kontaktow
		BringWindowToTop(hFrmMain);
		SetForegroundWindow(hFrmMain);
		SetFocus(hFrmMainFocus);
		SetActiveWindow(hFrmMain);
	  }
	}
	//SideSlide - ustawienie okna kontaktow na wierzchu
	else if(wParam==TIMER_FRMMAINSETTOPMOSTEX)
	{
	  //Pobranie PID okna
	  DWORD PID;
	  GetWindowThreadProcessId(GetForegroundWindow(), &PID);
	  //Porownanie PID okna
	  if(PID!=ProcessPID)
	  {
		//Pobieranie klasy nowego okna
		wchar_t WClassName[128];
		GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
		//Porownanie klasy nowego aktywnego okna
		if(((UnicodeString)WClassName!="TaskSwitcherWnd")&&((UnicodeString)WClassName!="DV2ControlHost"))
		 //Ustawienie okna kontaktow na wierzchu
		 SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	  }
	}
	//SideSlide - ustawienie okna kontaktow na wierzchu i schowanie go
	else if(wParam==TIMER_FRMMAINSETTOPMOSTANDSLIDE)
	{
	  //Pobieranie klasy nowego okna
	  wchar_t WClassName[128];
	  GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
	  //Wlaczenie timera ustawienia okna na wierzchu
	  if(((UnicodeString)WClassName!="TaskSwitcherWnd")&&((UnicodeString)WClassName!="DV2ControlHost"))
	  {
		//Zatrzymanie timera
		KillTimer(hTimerFrm,TIMER_FRMMAINSETTOPMOSTANDSLIDE);
		if(GetForegroundWindow()!=hFrmMain)
		{
		  //Status FrmMainSlideOut
		  FrmMainSlideOut = true;
		  PreFrmMainSlideOut = true;
		  //Pobieranie nowego aktywnego okna
		  LastActiveWindow_WmInactiveFrmMainSlide = GetForegroundWindow();
		  //Ponowna aktywacja okna
		  BringWindowToTop(hFrmMain);
		  SetForegroundWindow(hFrmMain);
		  //Ustawienie okna na wierzchu
		  SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		  //Wlaczenie FrmMainSlideOut (part I)
		  SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
		}
		else
		{
		  //Pobranie uchwytu
		  LastActiveWindow_PreFrmMainSlideIn = GetForegroundWindow();
		  //Ponowna aktywacja okna
		  BringWindowToTop(hFrmMain);
		  SetForegroundWindow(hFrmMain);
		}
	  }
	}

	return 0;
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Procka okna kontaktow
LRESULT CALLBACK FrmMainProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if((!ThemeChanging)&&(!ForceUnloadExecuted))
  {
	//Poprawka na znikanie popupmenu w oknie kontaktow podczas pisania wiadomosci przez kontakt
	if((uMsg==WM_SETICON)&&(FrmMainSlideChk)&&(FrmMainSlideHideMode!=2)&&(FrmMainVisible))
	{
	  //Pobieranie okna w ktorym znajduje sie kursor
	  HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
	  //Pobieranie klasy okna w ktorym znajduje sie kursor
	  wchar_t WClassName[128];
	  GetClassNameW(hCurActiveFrm, WClassName, sizeof(WClassName));
	  //Pobranie PID okna w ktorym znajduje sie kursor
	  DWORD PID;
	  GetWindowThreadProcessId(hCurActiveFrm, &PID);
	  //Gdy kursor znajduje sie w obrebie menu z okna aplikacji lub w oknie kontaktow
	  if((((UnicodeString)WClassName=="#32768")&&(PID==ProcessPID))||((Mouse->CursorPos.y>FrmMainRect.Top+FrmMainRealTopPos-FrmMain_Shell_TrayWndTop)&&(FrmMainRect.Bottom+FrmMainRealBottomPos+FrmMain_Shell_TrayWndBottom>Mouse->CursorPos.y)&&(Mouse->CursorPos.x>FrmMainRect.Left+FrmMainRealLeftPos-FrmMain_Shell_TrayWndLeft)&&(FrmMainRect.Right+FrmMainRealRightPos+FrmMain_Shell_TrayWndRight>Mouse->CursorPos.x)))
	   SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	  else
	   SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	}
	//Aktywacja okna kontatow
	else if((uMsg==WM_ACTIVATE)&&(!LoadExecuted))
	{
	  //SideSlide dla okna kontaktow
	  if(FrmMainSlideChk)
	  {
		//FrmMainSlideOut
		if(wParam==WA_INACTIVE)
		{
		  //Wylaczenie tymczasowej blokady
		  if((FrmMainVisible)&&(FrmMainSlideHideMode==3)&&(!FrmMainBlockSlideWndEvent)) FrmMainBlockSlide = false;
		  //Wlaczenie FrmMainSlideOut
		  if((FrmMainVisible)&&(!PreFrmMainSlideOut)&&(!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
		  {
			//Gdy kursor znajduje sie poza oknem rozmowy
			if((Mouse->CursorPos.y<FrmMainRect.Top)||(FrmMainRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmMainRect.Left)||(FrmMainRect.Right<Mouse->CursorPos.x))
			{
			  //Chowanie gdy aplikacja straci fokus
			  if(FrmMainSlideHideMode==2)
			  {
				//Pobranie PID procesu nowego okna
				DWORD PID;
				GetWindowThreadProcessId(GetForegroundWindow(), &PID);
				if(PID!=ProcessPID)
				{
				  //Pobieranie klasy nowego aktywnego okna
				  wchar_t WClassName[128];
				  GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
				  if((UnicodeString)WClassName!="TaskSwitcherWnd")
				  {
					//Status FrmMainSlideOut
					FrmMainSlideOut = true;
					PreFrmMainSlideOut = true;
					//Ustawienie okna na wierzchu
					SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
					//Wlaczenie FrmMainSlideOut (part I)
					SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
				  }
				}
			  }
			  //Chowanie gdy okno straci fokus
			  else if(FrmMainSlideHideMode==1)
			  {
				//Pobieranie klasy nowego aktywnego okna
				wchar_t WClassName[128];
				GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
				if((UnicodeString)WClassName!="TaskSwitcherWnd")
				{
				  //Status FrmMainSlideOut
				  FrmMainSlideOut = true;
				  PreFrmMainSlideOut = true;
				  //Pobieranie nowego aktywnego okna
				  wchar_t WClassName[128];
				  GetClassNameW(WindowFromPoint(Mouse->CursorPos), WClassName, sizeof(WClassName));
				  if(((UnicodeString)WClassName!="ToolbarWindow32")
				  &&((UnicodeString)WClassName!="MSTaskListWClass")
				  &&((UnicodeString)WClassName!="TaskSwitcherWnd")
				  &&((UnicodeString)WClassName!="Shell_TrayWnd")
				  &&((UnicodeString)WClassName!="TrayShowDesktopButtonWClass"))
				   LastActiveWindow_WmInactiveFrmMainSlide = WindowFromPoint(Mouse->CursorPos);
				  //Ponowna aktywacja okna
				  BringWindowToTop(hFrmMain);
				  SetForegroundWindow(hFrmMain);
				  //Ustawienie okna na wierzchu
				  SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				  //Wlaczenie FrmMainSlideOut (part I)
				  SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
				}
				else
				 SetTimer(hTimerFrm,TIMER_FRMMAINSETTOPMOSTANDSLIDE,10,(TIMERPROC)TimerFrmProc);
			  }
			  //Chowanie gdy kursor opusci okno
			  else
			  {
				//Status FrmMainSlideOut
				FrmMainSlideOut = true;
				PreFrmMainSlideOut = true;
				//Pobieranie nowego aktywnego okna
				wchar_t WClassName[128];
				GetClassNameW(WindowFromPoint(Mouse->CursorPos), WClassName, sizeof(WClassName));
				if(((UnicodeString)WClassName!="ToolbarWindow32")
				&&((UnicodeString)WClassName!="MSTaskListWClass")
				&&((UnicodeString)WClassName!="TaskSwitcherWnd")
				&&((UnicodeString)WClassName!="Shell_TrayWnd")
				&&((UnicodeString)WClassName!="TrayShowDesktopButtonWClass"))
				 LastActiveWindow_WmInactiveFrmMainSlide = WindowFromPoint(Mouse->CursorPos);
				//Ponowna aktywacja okna
				BringWindowToTop(hFrmMain);
				SetForegroundWindow(hFrmMain);
				//Ustawienie okna na wierzchu
				SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				//Wlaczenie FrmMainSlideOut (part I)
				SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
			  }
			}
			//Kursor znajduje sie w oknie ale zostalo zmienione aktywne okno
			else
			{
			  //Pobranie PID procesu nowego okna
			  DWORD PID;
			  GetWindowThreadProcessId(GetForegroundWindow(), &PID);
			  //Wlaczenie timera ustawienia okna na wierzchu i pobranie nowego uchwytu
			  if(PID!=ProcessPID) SetTimer(hTimerFrm,TIMER_FRMMAINSETTOPMOST,10,(TIMERPROC)TimerFrmProc);
			}
		  }
		  //Wylaczenie FrmMainSlideOut
		  else if((FrmMainVisible)&&(PreFrmMainSlideOut)&&(FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
		  {
			if((Mouse->CursorPos.y>FrmMainRect.Top)&&(FrmMainRect.Bottom>Mouse->CursorPos.y)&&(Mouse->CursorPos.x>FrmMainRect.Left)&&(FrmMainRect.Right>Mouse->CursorPos.x))
			{
			  //Zatrymanie timera
			  KillTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT);
			  //Status FrmMainSlideOut
			  PreFrmMainSlideOut = false;
			  FrmMainSlideOut = false;
			  //Usuniecie uchwytu do nowego aktywnego okna
			  LastActiveWindow_WmInactiveFrmMainSlide = NULL;
			}
		  }
		}
		//FrmMainSlideIn
		if(wParam==WA_ACTIVE)
		{
		  wchar_t WClassName[128];
		  GetClassNameW(LastActiveWindow, WClassName, sizeof(WClassName));
		  wchar_t WClassName2[128];
		  GetClassNameW(WindowFromPoint(Mouse->CursorPos), WClassName2, sizeof(WClassName2));
		  if((!FrmMainVisible)&&(!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!IsIconic(LastActiveWindow))&&((IsWindowVisible(LastActiveWindow))||((UnicodeString)WClassName=="TaskSwitcherWnd"))&&((UnicodeString)WClassName2!="TrayShowDesktopButtonWClass")&&((WindowFromPoint(Mouse->CursorPos)!=hToolbarWindow32)||(PseudoFrmMainSlideIn)))
		  {
			//Status pseudo FrmMainSlideIn
			PseudoFrmMainSlideIn = false;
			//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
			ChkFullScreenMode();
			//Blokowanie wysuwania przy aplikacji pelnoekranowej
			if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
			{
			  //Blokada FrmMainSlideIn
			  FullScreenMode = false;
			}
			else
			{
			  //Status FrmMainSlideIn
			  FrmMainSlideIn = true;
			  //Aktywacja okna kontaktow
			  BringWindowToTop(hFrmMain);
			  SetForegroundWindow(hFrmMain);
			  //Wlacznie FrmMainSlideIn (part II)
			  SetTimer(hTimerFrm,TIMER_FRMMAINSLIDEIN,FrmMainStepInterval,(TIMERPROC)TimerFrmProc);
			}
		  }
		}
		//Ustawianie na wierzchu okna wyszukiwarki
		if((hFrmSeekOnList)&&((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE)))
		 SetWindowPos(hFrmSeekOnList,HWND_TOPMOST,0,0,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
	  }
	}
	//Zabezpieczenie przed "zamknieciem" okna przy aktywnym SideSlide
	else if(uMsg==WM_CLOSE)
	{
	  if(FrmMainSlideChk)
	  {
		//Wylaczenie tymczasowej blokady
		if((FrmMainVisible)&&(FrmMainSlideHideMode==3)&&(!FrmMainBlockSlideWndEvent)) FrmMainBlockSlide = false;
		//Wlaczenie FrmMainSlideOut
		if((FrmMainVisible)&&(!PreFrmMainSlideOut)&&(!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
		{
		  //Status FrmMainSlideOut
		  FrmMainSlideOut = true;
		  PreFrmMainSlideOut = true;
		  //Wlaczenie FrmMainSlideOut (part I)
		  SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
		}
		//Zabezpieczenie przed "zamknieciem" okna
		return 1;
	  }
	}
	//SideSlide - wlaczenie tymczasowej blokady FrmMainSlideOut
	else if((uMsg==WM_ENTERSIZEMOVE)&&(FrmMainSlideChk)&&(FrmMainVisible)&&(!FrmMainSlideIn)&&(!FrmMainSlideOut))
	{
	  FrmMainBlockSlide = true;
	  FrmMainBlockSlideWndEvent = true;
	  WM_ENTERSIZEMOVE_BLOCK = true;
	}
	//SideSlide - wylaczenie tymczasowej blokady FrmMainSlideOut
	else if((uMsg==WM_EXITSIZEMOVE)&&(WM_ENTERSIZEMOVE_BLOCK))
	{
	  WM_ENTERSIZEMOVE_BLOCK = false;
	  //Pobranie rozmiaru+pozycji okna kontatkow
	  GetFrmMainRect();
	  //Ustawienie poprawnej pozycji okna kontaktow
	  SetFrmMainPos();
	  //Pobranie rozmiaru+pozycji okna kontatkow
	  GetFrmMainRect();
	  //Wylaczenie tymczasowej blokady
	  if(FrmMainSlideHideMode==3)
	   SetTimer(hTimerFrm,TIMER_FRMMAINBLOCKSLIDE,1500,(TIMERPROC)TimerFrmProc);
	  else
	  {
		FrmMainBlockSlide = false;
		FrmMainBlockSlideWndEvent = false;
	  }
	}
  }
  return CallWindowProc(OldFrmMainProc, hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Procka okna rozmowy
LRESULT CALLBACK FrmSendProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if((!ThemeChanging)&&(!ForceUnloadExecuted))
  {
	//Blokowanie zmiany tekstu na belce okna
	if(uMsg==WM_SETICON)
	{
	  //Wlaczony jest licznik nowych wiadomosci na oknie rozmowy oraz licznik jest rozny od 0
	  if((InactiveFrmNewMsgChk)&&(InactiveFrmNewMsgCount))
	  {
		//Jezeli okno rozmowy jest nie aktywne
		if(GetActiveWindow()!=hFrmSend)
		 SetWindowTextW(hFrmSend,("["+IntToStr(InactiveFrmNewMsgCount)+"] "+FrmSendTitlebar).w_str());
	  }
	  //Wlaczona jest opcja zmiany caption okna rozmowy
	  if(TweakFrmSendTitlebarChk)
	  {
		//Pobranie aktualnego tekstu belki okna
		wchar_t* TitlebarW = new wchar_t[512];
		GetWindowTextW(hFrmSend,TitlebarW,512);
		UnicodeString Titlebar = (wchar_t*)TitlebarW;
		//Sprawdzanie czy belka zostal juz zmieniona ostatnio
		ShortString TitlebarShort = UTF8EncodeToShortString(Titlebar);
		UnicodeString ChangedTitlebar = UTF8ToUnicodeString((IniStrToStr(ChangedTitlebarList->ReadString("Titlebar", StrToIniStr(TitlebarShort.operator AnsiString()), ""))).w_str());
		//Ustawianie nowego tekstu na belce okna
		if((!ChangedTitlebar.IsEmpty())&&(Titlebar!=ChangedTitlebar))
		 SetWindowTextW(hFrmSend,ChangedTitlebar.w_str());
	  }
	}
	//Blokowanie ustawiania domyslnego tekstu na belce okna
	else if(uMsg==0x000000ae)
	{
	  //Wlaczona jest opcja zmiany caption okna rozmowy i okno jest aktywne
	  if((TweakFrmSendTitlebarChk)&&(GetActiveWindow()==hFrmSend))
	  {
		//Pobranie aktualnego tekstu belki okna
		wchar_t* TitlebarW = new wchar_t[512];
		GetWindowTextW(hFrmSend,TitlebarW,512);
		UnicodeString Titlebar = (wchar_t*)TitlebarW;
		//Sprawdzanie czy belka zostal juz zmieniona ostatnio
		ShortString TitlebarShort = UTF8EncodeToShortString(Titlebar);
		UnicodeString ChangedTitlebar = UTF8ToUnicodeString((IniStrToStr(ChangedTitlebarList->ReadString("Titlebar", StrToIniStr(TitlebarShort.operator AnsiString()), ""))).w_str());
		//Ustawianie nowego tekstu na belce okna
		if((!ChangedTitlebar.IsEmpty())&&(Titlebar!=ChangedTitlebar))
		 SetWindowTextW(hFrmSend,ChangedTitlebar.w_str());
	  }
	}
	else if(uMsg==WM_ACTIVATE)
	{
	  //Zmiana tekstu na belce okna
	  if(InactiveFrmNewMsgChk)
	  {
		//Okno aktywne
		if((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE))
		{
		  //Przywracanie poprzedniego stanu titlebara
		  if(!FrmSendTitlebar.IsEmpty())
		  {
			SetWindowTextW(hFrmSend,FrmSendTitlebar.w_str());
			FrmSendTitlebar = "";
		  }
		  //Kasowanie licznika nowych wiadomosci
		  InactiveFrmNewMsgCount = 0;
		}
	  }
	  //Notyfikcja pisania wiadomosci
	  if(ChatStateNotiferNewMsgChk)
	  {
		//Okno aktywne
		if((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE))
		{
		  //Resetowanie poprzedniego stanu pisania wiadomosci
		  LastChatState = 0;
		  //Ustawienie oryginalnej malej ikonki
		  if(hIconSmall)
		  {
			HICON pIconSmall;
			do { pIconSmall = (HICON)SendMessage(hFrmSend, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall); }
			while(pIconSmall==hIconSmall);
			hIconSmall = NULL;
		  }
		  //Ustawienie oryginalnej duzej ikonki
		  if(hIconBig)
		  {
			HICON pIconBig;
			do { pIconBig = (HICON)SendMessage(hFrmSend, WM_SETICON, ICON_BIG, (LPARAM)hIconBig); }
			while(pIconBig==hIconBig);
			hIconBig = NULL;
		  }
		}
	  }
	  if(FrmSendOpening)
	  {
		FrmSendOpening = false;
		//Maksymalizowanie okna
		if((FrmSendMaximized)&&(!FrmSendSlideChk))
		 ShowWindow(hFrmSend,SW_MAXIMIZE);
	  }
	  //SideSlide dla okna rozmowy
	  if((FrmSendSlideChk)&&(!FrmSendBlockSlideOnRestore))
	  {
		//FrmSendSlideOut
		if((wParam==WA_INACTIVE)&&(!FrmMainSlideOutWndActiv))
		{
		  //Wylaczenie tymczasowej blokady
		  if((FrmSendVisible)&&(FrmSendSlideHideMode==3)&&(!FrmSendBlockSlideWndEvent))
		  {
			if(StayOnTopChk) FrmSendBlockSlide = StayOnTopStatus;
			else FrmSendBlockSlide = false;
		  }
		  //Wlaczenie FrmSendSlideOut
		  if((FrmSendVisible)&&(!PreFrmSendSlideOut)&&(!FrmSendBlockSlide)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))
		  {
			//Gdy kursor znajduje sie poza oknem rozmowy
			if((Mouse->CursorPos.y<FrmSendRect.Top)||(FrmSendRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmSendRect.Left)||(FrmSendRect.Right<Mouse->CursorPos.x))
			{
			  //Chowanie gdy aplikacja straci fokus
			  if(FrmSendSlideHideMode==2)
			  {
				//Pobranie PID procesu nowego okna
				DWORD PID;
				GetWindowThreadProcessId(GetForegroundWindow(), &PID);
				if(PID!=ProcessPID)
				{
				  //Pobieranie klasy nowego aktywnego okna
				  wchar_t WClassName[128];
				  GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
				  if((UnicodeString)WClassName!="TaskSwitcherWnd")
				  {
					//Status FrmSendSlideOut
					FrmSendSlideOut = true;
					PreFrmSendSlideOut = true;
					//Ustawienie okna na wierzchu
					SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
					//Wlaczenie FrmSendSlideOut (part I)
					SetTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
				  }
				}
			  }
			  //Chowanie gdy okno straci fokus
			  else if(FrmSendSlideHideMode==1)
			  {
				//Pobieranie klasy nowego aktywnego okna
				wchar_t WClassName[128];
				GetClassNameW(GetForegroundWindow(), WClassName, sizeof(WClassName));
				if((UnicodeString)WClassName!="TaskSwitcherWnd")
				{
				  //Status FrmSendSlideOut
				  FrmSendSlideOut = true;
				  PreFrmSendSlideOut = true;
				  //Pobieranie nowego aktywnego okna
				  wchar_t WClassName[128];
				  GetClassNameW(WindowFromPoint(Mouse->CursorPos), WClassName, sizeof(WClassName));
				  if(((UnicodeString)WClassName!="ToolbarWindow32")
				  &&((UnicodeString)WClassName!="MSTaskListWClass")
				  &&((UnicodeString)WClassName!="TaskSwitcherWnd")
				  &&((UnicodeString)WClassName!="Shell_TrayWnd")
				  &&((UnicodeString)WClassName!="TrayShowDesktopButtonWClass"))
				   LastActiveWindow_WmInactiveFrmSendSlide = WindowFromPoint(Mouse->CursorPos);
				  //Ponowna aktywacja okna
				  SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				  //Wlaczenie FrmSendSlideOut (part I)
				  SetTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
				}
				else
				 SetTimer(hTimerFrm,TIMER_FRMSENDSETTOPMOSTANDSLIDE,10,(TIMERPROC)TimerFrmProc);
			  }
			  //Chowanie gdy kursor opusci okno
			  else
			  {
				//Status FrmSendSlideOut
				FrmSendSlideOut = true;
				PreFrmSendSlideOut = true;
				//Pobieranie nowego aktywnego okna
				wchar_t WClassName[128];
				GetClassNameW(WindowFromPoint(Mouse->CursorPos), WClassName, sizeof(WClassName));
				if(((UnicodeString)WClassName!="ToolbarWindow32")
				&&((UnicodeString)WClassName!="MSTaskListWClass")
				&&((UnicodeString)WClassName!="TaskSwitcherWnd")
				&&((UnicodeString)WClassName!="Shell_TrayWnd")
				&&((UnicodeString)WClassName!="TrayShowDesktopButtonWClass"))
				 LastActiveWindow_WmInactiveFrmSendSlide = WindowFromPoint(Mouse->CursorPos);
				//Ponowna aktywacja okna
				SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				//Wlaczenie FrmSendSlideOut (part I)
				SetTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
			  }
			}
			//Kursor znajduje sie w oknie ale zostalo zmienione aktywne okno
			else
			{
			  //Pobranie PID procesu nowego okna
			  DWORD PID;
			  GetWindowThreadProcessId(GetForegroundWindow(), &PID);
			  //Wlaczenie timera pobranie nowego uchwytu i aktywacji okna rozmowy
			  if(PID!=ProcessPID) SetTimer(hTimerFrm,TIMER_FRMSENDSETTOPMOST,100,(TIMERPROC)TimerFrmProc);
			}
		  }
		  //Wylaczenie FrmSendSlideOut
		  else if((FrmSendVisible)&&(PreFrmSendSlideOut)&&(FrmSendSlideOut)&&(!FrmSendSlideIn)&&(!FrmSendBlockSlide))
		  {
			if((Mouse->CursorPos.y>FrmSendRect.Top)&&(FrmSendRect.Bottom>Mouse->CursorPos.y)&&(Mouse->CursorPos.x>FrmSendRect.Left)&&(FrmSendRect.Right>Mouse->CursorPos.x))
			{
			  //Zatrzymanie timera
			  KillTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEOUT);
			  //Status FrmSendSlideOut
			  PreFrmSendSlideOut = false;
			  FrmSendSlideOut = false;
			  //Usuniecie uchwytu do nowego aktywnego okna
			  LastActiveWindow_WmInactiveFrmSendSlide= NULL;
			}
		  }
		}
		//FrmSendSlideIn
		if(wParam==WA_ACTIVE)
		{
		  wchar_t WClassName[128];
		  GetClassNameW(LastActiveWindow, WClassName, sizeof(WClassName));
		  wchar_t WClassName2[128];
		  GetClassNameW(WindowFromPoint(Mouse->CursorPos), WClassName2, sizeof(WClassName2));
		  if((!FrmSendVisible)&&(!FrmSendBlockSlide)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn)&&(!IsIconic(LastActiveWindow))&&((IsWindowVisible(LastActiveWindow))||((UnicodeString)WClassName=="TaskSwitcherWnd")||((UnicodeString)WClassName2=="MSTaskListWClass")||((UnicodeString)WClassName2=="ToolbarWindow32")))
		  {
			//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
			ChkFullScreenMode();
			//Blokowanie wysuwania przy aplikacji pelnoekranowej
			if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
			{
			  //Blokada FrmSendSlideIn
			  FullScreenMode = false;
			}
			else
			{
			  //Status FrmSendSlideIn
			  FrmSendSlideIn = true;
			  //Pobieranie pozycji okna rozmowy
			  TRect WindowRect;
			  GetWindowRect(hFrmSend,&WindowRect);
			  //Odswiezenie okna rozmowy
			  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height()+1,SWP_NOMOVE);
			  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
			  //Wlacznie FrmSendSlideIn (part II)
			  SetTimer(hTimerFrm,TIMER_FRMSENDSLIDEIN,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
			}
		  }
		}
	  }
	}
	//Zabezpieczenie przed zamykaniem wielu zakladek jednoczesnie
	else if(uMsg==WM_CLOSE)
	{
	  if((EmuTabsWSupport)&&(EmuTabsWChk)&&(ResTabsList->Count>=2))
	  {
		if(Application->MessageBox(
		("Czy chcesz zamkn¹æ wszystkie otwarte zak³adki?"
		"\n\nIloœæ otwartych zak³adek: " + IntToStr(ResTabsList->Count)).w_str(),
		L"Pytanie o zak³adki",
		MB_OKCANCEL | MB_ICONASTERISK)==IDOK)
		{
		  return CallWindowProc(OldFrmSendProc, hwnd, uMsg, wParam, lParam);
		}
		else
		 return 1;
	  }
	  else
	   return CallWindowProc(OldFrmSendProc, hwnd, uMsg, wParam, lParam);
	}
	else if(uMsg==WM_SIZE)
	{
	  //"Anty maksymalizacja" okna
	  if((!FrmSendSlideChk)&&(!FrmSendSlideChk))
	  {
		if((wParam==WM_SIZE)||(wParam==SIZE_MAXIMIZED))
		 FrmSendMaximized = !FrmSendMaximized;
		else if((wParam==WM_SIZE)||(wParam==SIZE_RESTORED))
		 if(!FrmSendOpening) FrmSendMaximized = false;
	  }
	  //Blokada minimalizacji okna rozmowy
	  if((wParam==SIZE_MINIMIZED)&&(FrmSendSlideChk)&&(!FrmPosExist))
	  {
		//FrmSendSlideOut
		if(FrmSendVisible)
		{
		  //Odznaczenie StayOnTop jako wylaczone
		  if((StayOnTopChk)&&(StayOnTopStatus))
		  {
			//Aktualizacja przycisku
			TPluginAction StayOnTopItem;
			ZeroMemory(&StayOnTopItem,sizeof(TPluginAction));
			StayOnTopItem.cbSize = sizeof(TPluginAction);
			StayOnTopItem.pszName = L"TabKitStayOnTopItem";
			StayOnTopItem.pszCaption = L"Trzymaj okno na wierzchu";
			StayOnTopItem.Hint = L"Trzymaj okno na wierzchu";
			StayOnTopItem.IconIndex = STAYONTOP_OFF;
			StayOnTopItem.Handle = (int)hFrmSend;
			PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&StayOnTopItem));
			//Stan StayOnTop
			StayOnTopStatus = false;
		  }
		  //Status SideSlide
		  FrmSendBlockSlide = true;
		  FrmSendBlockSlideOnRestore = true;
		  //Wlaczenie timera
		  SetTimer(hTimerFrm,TIMER_FRMSENDMINIMIZED,10,(TIMERPROC)TimerFrmProc);
		}
		//FrmSendSlideIn
		else
		{
		  wchar_t WClassName[128];
		  GetClassNameW(WindowFromPoint(Mouse->CursorPos), WClassName, sizeof(WClassName));
		  if((UnicodeString)WClassName!="TrayShowDesktopButtonWClass")
		  {
			//Status FrmSendSlideIn
			FrmSendSlideIn = true;
			//Wlaczenie FrmSendSlideIn (part I)
			SetTimer(hTimerFrm,TIMER_PREFRMSENDSLIDEIN,1,(TIMERPROC)TimerFrmProc);
		  }
		}
	  }
	  //Blokada maksymalizacji okna rozmowy
	  if((wParam==SIZE_MAXIMIZED)&&(FrmSendSlideChk))
	  {
		//Przywrocenie normalnego rozmiaru okna
		ShowWindow(hFrmSend,SW_RESTORE);
		//Blokada
		return -1;
	  }
	}
	//Ukrywanie paska narzedzi
	else if(uMsg==WM_SETCURSOR)
	{
	  if(HideToolBarChk)
	  {
		HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
		//Jezeli okno rozmowy jest aktywne
		if((GetActiveWindow()==hFrmSend)&&((hCurActiveFrm==hFrmSend)||(IsChild(hFrmSend,hCurActiveFrm))))
		{
		  //Pobieranie pozycji okna rozmowy
		  TRect WindowRect;
		  GetWindowRect(hFrmSend,&WindowRect);
		  int WindowBottom = WindowRect.Bottom;
		  int CursorY = Mouse->CursorPos.y;
		  //Pozycja kursora w oknie rozmowy
		  int CursorPos = WindowBottom - CursorY;
		  //Wysokoœæ paska informacyjnego
		  if(!hStatusBar) hStatusBar = FindWindowEx(hFrmSend,NULL,L"TsStatusBar",NULL);
		  GetWindowRect(hStatusBar,&WindowRect);
		  int StatusBarHeight = WindowRect.Height();
		  //Pokazywanie paska narzedzi
		  if(CursorPos<32+StatusBarHeight)
		  {
			//Jezeli timer nie zostal wlaczony
			if(!ToolBarShowing)
			{
			  //Wysokosc paska narzedzi
			  GetWindowRect(hToolBar,&WindowRect);
			  if(!WindowRect.Height())
			  {
				//Tworzenie timera
				ToolBarShowing = true;
				KillTimer(hTimerFrm,TIMER_TOOLBAR);
				SetTimer(hTimerFrm,TIMER_TOOLBAR,500,(TIMERPROC)TimerFrmProc);
			  }
			}
		  }
		  //Ukrywanie paska narzedzi
		  else
		  {
			if(ToolBarShowing)
			{
			  //Zatrzymanie timera
			  KillTimer(hTimerFrm,TIMER_TOOLBAR);
			  ToolBarShowing = false;
			}
			//Wysokosc paska narzedzi
			GetWindowRect(hToolBar,&WindowRect);
			if(WindowRect.Height())
			{
			  //Ukrycie paska
			  SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
			  //Pobieranie pozycji okna rozmowy
			  GetWindowRect(hFrmSend,&WindowRect);
			  //Odswiezenie okna rozmowy
			  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
			  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
			}
		  }
		}
	  }
	}
	//SideSlide - wlaczenie tymczasowej blokady FrmSendSlideOut
	else if((uMsg==WM_ENTERSIZEMOVE)&&(FrmSendSlideChk)&&(FrmSendVisible)&&(!FrmSendSlideIn)&&(!FrmSendSlideOut)&&(!FrmSendBlockSlide))
	{
	  FrmSendBlockSlide = true;
	  FrmSendBlockSlideWndEvent = true;
	  WM_ENTERSIZEMOVE_BLOCK = true;
	}
	//SideSlide - wylaczenie tymczasowej blokady FrmSendSlideOut
	else if((uMsg==WM_EXITSIZEMOVE)&&(WM_ENTERSIZEMOVE_BLOCK))
	{
	  WM_ENTERSIZEMOVE_BLOCK = false;
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  GetFrmSendRect();
	  //Ustawienie poprawnej pozycji okna rozmowy
	  SetFrmSendPos();
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  GetFrmSendRect();
	  //Wlaczenie timera wylaczanie tymczasowej blokady
	  if(FrmSendSlideHideMode==3)
	   SetTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE,2000,(TIMERPROC)TimerFrmProc);
	  //Wylaczenie tymczasowej blokady
	  else
	  {
		FrmSendBlockSlide = false;
		FrmSendBlockSlideWndEvent = false;
	  }
	}
  }

  return CallWindowProc(OldFrmSendProc, hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Procka okna wyszukiwarki
LRESULT CALLBACK FrmSeekOnListProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if((!ThemeChanging)&&(!ForceUnloadExecuted))
  {
	if((uMsg==WM_ACTIVATE)&&(wParam==WA_INACTIVE))
	{
	  if(GetForegroundWindow()!=hFrmMain)
	  {
		//Wlaczenie timera ustawienia okna kontaktow na wierzchu
		SetTimer(hTimerFrm,TIMER_FRMMAINSETTOPMOSTEX,10,(TIMERPROC)TimerFrmProc);
	  }
	}
  }

  return CallWindowProc(OldFrmSeekOnListProc, hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Lokalny hook na klawiature
LRESULT CALLBACK ThreadKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  //Blad lub zamykanie komunikatora / wyladowanie wtyczki
  if((nCode<0)||(ForceUnloadExecuted)) return CallNextHookEx(hThreadKeyboard, nCode, wParam, lParam);

  //SideSlide: Blokowanie wszystich klawiszy gdy okno rozmowy/kontaktow jest chowane/wysuwane lub jest juz schowane i posiada fokus
  if((FrmMainSlideChk)&&((FrmMainSlideIn)||(FrmMainSlideOut)||(!FrmMainVisible)))
  {
	//Srawdzanie aktywnego okna
	if(GetActiveWindow()==hFrmMain)
	 return -1;
  }
  if((FrmSendSlideChk)&&((FrmSendSlideIn)||(FrmSendSlideOut)||(!FrmSendVisible)))
  {
	//Srawdzanie aktywnego okna
	if(GetActiveWindow()==hFrmSend)
	 return -1;
  }
  //Blokowanie klawiszu Esc przy w³aczonym SideSlide dla okna kontaktow
  if(FrmMainSlideChk)
  {
	if(GetKeyState(VK_ESCAPE)<0)
	{
	  //Srawdzanie aktywnego okna
	  if(GetActiveWindow()==hFrmMain)
	  {
		//Wylaczenie tymczasowej blokady
		if((FrmMainSlideHideMode==3)&&(!FrmMainBlockSlideWndEvent)) FrmMainBlockSlide = false;
		//Wlaczenie FrmMainSlideOut
		if((!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
		{
		  //Status FrmMainSlideOut
		  FrmMainSlideOut = true;
		  //Wlaczenie FrmMainSlideOut (part I)
		  SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
		}
		//Blokada wcisniecia klawisza Esc
		return -1;
	  }
	}
  }
  //Pomijanie przypietych zak³adek w przelaczaniu
  if((ExClipTabsFromTabSwitchingChk)&&((!ExClipTabsFromSwitchToNewMsgChk)||((ExClipTabsFromSwitchToNewMsgChk)&&(!MsgList->Count))))
  {
	//Wcisniety Ctrl+Tab
	if((GetKeyState(VK_CONTROL)<0)&&(GetKeyState(VK_LSHIFT)>=0)&&((int)wParam==9))
	{
	  if(!(HIWORD(lParam)&KF_UP))
	  {
		//Srawdzanie aktywnego okna i ilosci zakladek
		if((GetActiveWindow()==hFrmSend)&&(TabsList->Count>1))
		{
		  //Usuwanie listy zakladek
		  TabsList->Clear();
		  ResTabsList->Clear();
		  //Hook na pobieranie aktywnych zakladek
		  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs);
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
		  PluginLink.UnhookEvent(OnFetchAllTabs);
		  //Pobieranie pozycji aktywnej zakladki
		  int ActiveTabInx = ResTabsList->IndexOf(ActiveTabJIDRes);
		  //Okreslenie pozycji poprzedniej zakladki
		  int TabInx = ActiveTabInx+1;
		  //Petral z warunkiem kiedy dojdziemy do aktywnej zakladki
		  while(TabInx!=ActiveTabInx)
		  {
			//Zakladka poza lista - ustawienie indeksu ostatniej zakladki
			if(TabInx==ResTabsList->Count)
			{
			  TabInx = 0;
			  //Zakonczenie petli
			  if(TabInx==ActiveTabInx) return -1;
			}
			//Usuwanie zasobu z JID
			UnicodeString JID = ResTabsList->Strings[TabInx];
			if(JID.Pos("/")) JID = JID.Delete(JID.Pos("/"),JID.Length());
			//Jezeli zakladka nie jest przypieta
			if(ClipTabsList->IndexOf(JID)==-1)
			{
			  //Pobieranie pelnego identyfikatora nowej zakladki
			  JID = ResTabsList->Strings[TabInx];
			  //Przelaczanie na zakladke z danym kontektem
			  if(!JID.Pos("ischat_"))
			  {
				if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
		  		else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
			  }
			  //Przelaczanie na zakladke z czatem
			  else
			  {
				JID = JID.Delete(1,7);
				PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)JID.w_str());
			  }
			  return -1;
			}
			//Kolejna poprzednia zakladka
			TabInx++;
		  }
		  //Blokada przelaczenia
		  return -1;
		}
	  }
	}
	//Wcisniety Ctrl+Shift+Tab
	else if((GetKeyState(VK_CONTROL)<0)&&(GetKeyState(VK_LSHIFT)<0)&&((int)wParam==9))
	{
	  if(!(HIWORD(lParam)&KF_UP))
	  {
		//Srawdzanie aktywnego okna i ilosci zakladek
		if((GetActiveWindow()==hFrmSend)&&(TabsList->Count>1))
		{
		  //Usuwanie listy zakladek
		  TabsList->Clear();
		  ResTabsList->Clear();
		  //Hook na pobieranie aktywnych zakladek
		  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs);
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
		  PluginLink.UnhookEvent(OnFetchAllTabs);
		  //Pobieranie pozycji aktywnej zakladki
		  int ActiveTabInx = ResTabsList->IndexOf(ActiveTabJIDRes);
		  //Okreslenie pozycji poprzedniej zakladki
		  int TabInx = ActiveTabInx-1;
		  //Petral z warunkiem kiedy dojdziemy do aktywnej zakladki
		  while(TabInx!=ActiveTabInx)
		  {
			//Zakladka poza lista - ustawienie indeksu ostatniej zakladki
			if(TabInx==-1)
			{
			  TabInx = ResTabsList->Count-1;
			  //Zakonczenie petli
			  if(TabInx==ActiveTabInx) return -1;
			}
			//Usuwanie zasobu z JID
			UnicodeString JID = ResTabsList->Strings[TabInx];
			if(JID.Pos("/")) JID = JID.Delete(JID.Pos("/"),JID.Length());
			//Jezeli zakladka nie jest przypieta
			if(ClipTabsList->IndexOf(JID)==-1)
			{
			  //Pobieranie pelnego identyfikatora nowej zakladki
			  JID = ResTabsList->Strings[TabInx];
			  //Przelaczanie na zakladke z danym kontektem
			  if(!JID.Pos("ischat_"))
			  {
				if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
		  		else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
			  }
			  //Przelaczanie na zakladke z czatem
			  else
			  {
				JID = JID.Delete(1,7);
				PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)JID.w_str());
			  }
			  return -1;
			}
			//Kolejna poprzednia zakladka
			TabInx--;
		  }
		  //Blokada przelaczenia
		  return -1;
		}
	  }
	}
  }
  //Inteligentne przelaczenia zakladek
  if(SwitchToNewMsgChk)
  {
	//Wcisniety Ctrl+Tab
	if((GetKeyState(VK_CONTROL)<0)&&((int)wParam==9))
	{
	  if(!(HIWORD(lParam)&KF_UP))
	  {
		//Srawdzanie aktywnego okna
		if(GetActiveWindow()==hFrmSend)
		{
		  //Jezeli tablica cos zawiera
		  if(MsgList->Count>0)
		  {
			UnicodeString JID;
			//Sposob przelaczania
			if(SwitchToNewMsgMode==1)
			 JID = MsgList->Strings[0];
			else
			 JID = MsgList->Strings[MsgList->Count-1];
			//Pobrany identyfikator nie jest pusty
			if(!JID.IsEmpty())
			{
			  //Przelaczanie na zakladke z danym kontektem
			  if(!JID.Pos("ischat_"))
			  {
				if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
		  		else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
			  }
			  //Przelaczanie na zakladke z czatem
			  else
			  {
				JID = JID.Delete(1,7);
				PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)JID.w_str());
			  }
			  return -1;
			}
		  }
		}
	  }
	}
  }
  //Skroty do konretnych zakladek
  if(TabsHotKeysChk)
  {
	//Rodzaj skrotu
	if(TabsHotKeysMode==1)
	{
	  //F1-F12
	  if(((int)wParam>=112)&&((int)wParam<=123))
	  {
		if(!(HIWORD(lParam)&KF_UP))
		{
		  //Srawdzanie aktywnego okna
		  if(GetActiveWindow()==hFrmSend)
		  {
			//Jezeli nie wcisnieto Ctrl/Alt/Shift
			if((GetKeyState(VK_CONTROL)>=0)&&(GetKeyState(VK_MENU)>=0)&&(GetKeyState(VK_SHIFT)>=0))
			{
			  //Identyfikacja F'a
			  int Key = (int)wParam - 111;
			  //Usuwanie listy zakladek
			  TabsList->Clear();
			  ResTabsList->Clear();
			  //Hook na pobieranie aktywnych zakladek
			  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs);
			  PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
			  PluginLink.UnhookEvent(OnFetchAllTabs);
			  //Niepomijanie przypietych zakladek
			  if(!ExClipTabsFromTabsHotKeysChk)
			  {
				//Sprawdzanie czy wywolujemy zakladke "ducha"
				if(Key<=ResTabsList->Count)
				{
				  //Pobieranie JID
				  UnicodeString JID = ResTabsList->Strings[Key-1];
				  //Sprawdzanie rodzaju kontaktu
				  if(!JID.IsEmpty())
				  {
					//Przelaczanie na zakladke z danym kontektem
					if(!JID.Pos("ischat_"))
					{
					  if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
					  else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
					}
					//Przelaczanie na zakladke z czatem
					else
					{
					  JID = JID.Delete(1,7);
					  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)JID.w_str());
					}
					return -1;
				  }
				}
				//Zakladka duch - otwieranie okna rozmowy z nowa wiadomoscia
				else if(NewMgsHoyKeyChk)
				{
				  if((UnOpenMsgList->Count>0)&&(!NewMgsHoyKeyExecute))
				  {
					//Blokowanie otwierania paru zakladek jednoczesnie
					NewMgsHoyKeyExecute = true;
					//Pobieranie identyfikatora kontatku
					UnicodeString JID = UnOpenMsgList->Strings[0];
					//Otwieranie zakladki z danym kontektem
					if(!JID.Pos("ischat_"))
					{
					  if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
					  else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
					}
					//Otwieranie zakladki z czatem
					else
					{
					  //Ustawianie prawidlowego identyfikatora
					  JID = JID.Delete(1,7);
					  //Pobieranie nazwy kanalu
					  TIniFile *Ini = new TIniFile(SessionFileDir);
					  UnicodeString Channel = Ini->ReadString("Channels",JID,"");
					  delete Ini;
					  if(Channel.IsEmpty())
					  {
						Channel = JID;
						Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
					  }
					  //Wypenianie struktury nt. czatu
					  TPluginChatPrep PluginChatPrep;
					  PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
					  PluginChatPrep.UserIdx = GetContactIndexW(JID);
					  PluginChatPrep.JID = JID.w_str();
					  PluginChatPrep.Channel = Channel.w_str();
					  PluginChatPrep.CreateNew = false;
					  PluginChatPrep.Fast = true;
					  //Przywracanie zakladki czatowej
					  PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
					}
					return -1;
				  }
				}
			  }
			  //Pomijanie przypietych zakladek
			  else
			  {
				//Tworzenie listy bez przypietych zakladek
				TStringList *ExTabsList = new TStringList;
				for(int Count=0;Count<ResTabsList->Count;Count++)
				{
				  //Usuwanie zasobu z JID
				  UnicodeString JID = ResTabsList->Strings[Count];
				  if(JID.Pos("/")) JID = JID.Delete(JID.Pos("/"),JID.Length());
				  //Jezeli zakladka nie jest przypieta
				  if(ClipTabsList->IndexOf(JID)==-1)
				   //Dodawanie zakladki do nowej listy
				   ExTabsList->Add(ResTabsList->Strings[Count]);
				}
				//Sprawdzanie czy wywolujemy zakladke "ducha"
				if(Key<=ExTabsList->Count)
				{
				  //Pobieranie JID
				  UnicodeString JID = ExTabsList->Strings[Key-1];
				  delete ExTabsList;
				  //Sprawdzanie rodzaju kontaktu
				  if(!JID.IsEmpty())
				  {
					//Przelaczanie na zakladke z danym kontektem
					if(!JID.Pos("ischat_"))
					{
					  if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
					  else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
					}
					//Przelaczanie na zakladke z czatem
					else
					{
					  JID = JID.Delete(1,7);
					  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)JID.w_str());
					}
					return -1;
				  }
				}
				//Zakladka duch - otwieranie okna rozmowy z nowa wiadomoscia
				else if(NewMgsHoyKeyChk)
				{
				  delete ExTabsList;
				  if((UnOpenMsgList->Count>0)&&(!NewMgsHoyKeyExecute))
				  {
					//Blokowanie otwierania paru zakladek jednoczesnie
					NewMgsHoyKeyExecute = true;
					//Pobieranie identyfikatora kontatku
					UnicodeString JID = UnOpenMsgList->Strings[0];
					//Otwieranie zakladki z danym kontektem
					if(!JID.Pos("ischat_"))
					{
					  if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
					  else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
					}
					//Otwieranie zakladki z czatem
					else
					{
					  //Ustawianie prawidlowego identyfikatora
					  JID = JID.Delete(1,7);
					  //Pobieranie nazwy kanalu
					  TIniFile *Ini = new TIniFile(SessionFileDir);
					  UnicodeString Channel = Ini->ReadString("Channels",JID,"");
					  delete Ini;
					  if(Channel.IsEmpty())
					  {
						Channel = JID;
						Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
					  }
					  //Wypenianie struktury nt. czatu
					  TPluginChatPrep PluginChatPrep;
					  PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
					  PluginChatPrep.UserIdx = GetContactIndexW(JID);
					  PluginChatPrep.JID = JID.w_str();
					  PluginChatPrep.Channel = Channel.w_str();
					  PluginChatPrep.CreateNew = false;
					  PluginChatPrep.Fast = true;
					  //Przywracanie zakladki czatowej
					  PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
					}
					return -1;
				  }
				}
				else delete ExTabsList;
              }
			}
		  }
		}
	  }
	}
	//Ctrl+1-9
	else
	{
	  //Sprawdzanie statusu klawiszy Ctrl + 1-9
	  if((GetKeyState(VK_CONTROL)<0)&&
	   (GetKeyState(VK_MENU)>=0)&&
	   (GetKeyState(VK_SHIFT)>=0)&&
	   ((int)wParam>=49)&&
	   ((int)wParam<=57))
	  {
		if(!(HIWORD(lParam)&KF_UP))
		{
		  //Srawdzanie aktywnego okna
		  if(GetActiveWindow()==hFrmSend)
		  {
			//Identyfikacja klawisza
			int Key = (int)wParam - 48;
			//Usuwanie listy zakladek
			TabsList->Clear();
			ResTabsList->Clear();
			//Hook na pobieranie aktywnych zakladek
			PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs);
			PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
			PluginLink.UnhookEvent(OnFetchAllTabs);
			//Niepomijanie przypietych zakladek
			if(!ExClipTabsFromTabsHotKeysChk)
			{
			  //Sprawdzanie czy wywolujemy zakladke "ducha"
			  if(Key<=ResTabsList->Count)
			  {
				//Pobieranie JID
				UnicodeString JID = ResTabsList->Strings[Key-1];
				//Sprawdzanie rodzaju kontaktu
				if(!JID.IsEmpty())
				{
				  //Przelaczanie na zakladke z danym kontektem
				  if(!JID.Pos("ischat_"))
				  {
					if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
					else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
				  }
				  //Przelaczanie na zakladke z czatem
				  else
				  {
					JID = JID.Delete(1,7);
					PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)JID.w_str());
				  }
				  return -1;
				}
			  }
			  //Zakladka duch - otwieranie okna rozmowy z nowa wiadomoscia
			  else if(NewMgsHoyKeyChk)
			  {
				if((UnOpenMsgList->Count>0)&&(!NewMgsHoyKeyExecute))
				{
				  //Blokowanie otwierania paru zakladek jednoczesnie
				  NewMgsHoyKeyExecute = true;
				  //Pobieranie identyfikatora kontatku
				  UnicodeString JID = UnOpenMsgList->Strings[0];
				  //Otwieranie zakladki z danym kontektem
				  if(!JID.Pos("ischat_"))
				  {
				    if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
		  			else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
				  }
				  //Otwieranie zakladki z czatem
				  else
				  {
					//Ustawianie prawidlowego identyfikatora
					JID = JID.Delete(1,7);
					//Pobieranie nazwy kanalu
					TIniFile *Ini = new TIniFile(SessionFileDir);
					UnicodeString Channel = Ini->ReadString("Channels",JID,"");
					delete Ini;
					if(Channel.IsEmpty())
					{
					  Channel = JID;
					  Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
					}
					//Wypenianie struktury nt. czatu
					TPluginChatPrep PluginChatPrep;
					PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
					PluginChatPrep.UserIdx = GetContactIndexW(JID);
					PluginChatPrep.JID = JID.w_str();
					PluginChatPrep.Channel = Channel.w_str();
					PluginChatPrep.CreateNew = false;
					PluginChatPrep.Fast = true;
					//Przywracanie zakladki czatowej
					PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
				  }
				  return -1;
				}
			  }
			}
			//Pomijanie przypietych zakladek
			else
			{
			  //Tworzenie listy bez przypietych zakladek
			  TStringList *ExTabsList = new TStringList;
			  for(int Count=0;Count<ResTabsList->Count;Count++)
			  {
				//Usuwanie zasobu z JID
				UnicodeString JID = ResTabsList->Strings[Count];
				if(JID.Pos("/")) JID = JID.Delete(JID.Pos("/"),JID.Length());
				//Jezeli zakladka nie jest przypieta
				if(ClipTabsList->IndexOf(JID)==-1)
				 //Dodawanie zakladki do nowej listy
				 ExTabsList->Add(ResTabsList->Strings[Count]);
			  }
			  //Sprawdzanie czy wywolujemy zakladke "ducha"
			  if(Key<=ExTabsList->Count)
			  {
				//Pobieranie JID
				UnicodeString JID = ExTabsList->Strings[Key-1];
				delete ExTabsList;
				//Sprawdzanie rodzaju kontaktu
				if(!JID.IsEmpty())
				{
				  //Przelaczanie na zakladke z danym kontektem
				  if(!JID.Pos("ischat_"))
				  {
					if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
		  			else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
				  }
				  //Przelaczanie na zakladke z czatem
				  else
				  {
					JID = JID.Delete(1,7);
					PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)JID.w_str());
				  }
				  return -1;
				}
			  }
			  //Zakladka duch - otwieranie okna rozmowy z nowa wiadomoscia
			  else if(NewMgsHoyKeyChk)
			  {
				delete ExTabsList;
				if((UnOpenMsgList->Count>0)&&(!NewMgsHoyKeyExecute))
				{
				  //Blokowanie otwierania paru zakladek jednoczesnie
				  NewMgsHoyKeyExecute = true;
				  //Pobieranie identyfikatora kontatku
				  UnicodeString JID = UnOpenMsgList->Strings[0];
				  //Otwieranie zakladki z danym kontektem
				  if(!JID.Pos("ischat_"))
				  {
					if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
					else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
				  }
				  //Otwieranie zakladki z czatem
				  else
				  {
					//Ustawianie prawidlowego identyfikatora
					JID = JID.Delete(1,7);
					//Pobieranie nazwy kanalu
					TIniFile *Ini = new TIniFile(SessionFileDir);
					UnicodeString Channel = Ini->ReadString("Channels",JID,"");
					delete Ini;
					if(Channel.IsEmpty())
					{
					  Channel = JID;
					  Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
					}
					//Wypenianie struktury nt. czatu
					TPluginChatPrep PluginChatPrep;
					PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
					PluginChatPrep.UserIdx = GetContactIndexW(JID);
					PluginChatPrep.JID = JID.w_str();
					PluginChatPrep.Channel = Channel.w_str();
					PluginChatPrep.CreateNew = false;
					PluginChatPrep.Fast = true;
					//Przywracanie zakladki czatowej
					PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
				  }
				  return -1;
				}
			  }
			  else delete ExTabsList;
			}
		  }
		}
	  }
	}
  }
  //Skrot do przywracania ostatnio zamknietej zakladki
  if((ClosedTabsChk)&&(UnCloseTabHotKeyChk))
  {
	if(UnCloseTabHotKeyMode==1)
	{
	  if((GetKeyState(VK_CONTROL)<0)&&
	   (GetKeyState(VK_MENU)>=0)&&
	   (GetKeyState(VK_SHIFT)>=0)&&
	   ((int)wParam==VK_BACK))
	   {
		 if(!(HIWORD(lParam)&KF_UP))
		 {
		   //Srawdzanie aktywnego okna
		   HWND hActiveFrm = GetActiveWindow();
		   if((hActiveFrm==hFrmSend)||(hActiveFrm==hFrmMain))
		   {
			 if(ClosedTabsList->Count>0)
			 {
			   UnCloseTabHotKeyExecute();
			   return -1;
			 }
		   }
		 }
	   }
	}
	else
	{
	  if(!(HIWORD(lParam)&KF_UP))
	  {
		//Srawdzanie aktywnego okna
		HWND hActiveFrm = GetActiveWindow();
		if((hActiveFrm==hFrmSend)||(hActiveFrm==hFrmMain))
		{
		  int Mod = div(UnCloseTabHotKeyDef,256).quot;
		  int Key = UnCloseTabHotKeyDef-Mod*256;

		  if(div(Mod,32).quot==3)
		  {
			//Mod = MOD_SHIFT | MOD_CONTROL;
			if((GetKeyState(VK_CONTROL)<0)&&
			  (GetKeyState(VK_MENU)>=0)&&
			  (GetKeyState(VK_SHIFT)<0)&&
			  ((int)wParam==Key))
			{
			  if(ClosedTabsList->Count>0)
			   {
				 UnCloseTabHotKeyExecute();
				 return -1;
			   }
			}
		  }
		  else if(div(Mod,32).quot==5)
		  {
			//Mod = MOD_SHIFT | MOD_ALT;
			if((GetKeyState(VK_CONTROL)>=0)&&
			  (GetKeyState(VK_MENU)<0)&&
			  (GetKeyState(VK_SHIFT)<0)&&
			  ((int)wParam==Key))
			{
			  if(ClosedTabsList->Count>0)
			   {
				 UnCloseTabHotKeyExecute();
				 return -1;
			   }
			}
		  }
		  else if(div(Mod,32).quot==2)
		  {
			//Mod = MOD_CONTROL;
			if((GetKeyState(VK_CONTROL)<0)&&
			  (GetKeyState(VK_MENU)>=0)&&
			  (GetKeyState(VK_SHIFT)>=0)&&
			  ((int)wParam==Key))
			{
			  if(ClosedTabsList->Count>0)
			   {
				 UnCloseTabHotKeyExecute();
				 return -1;
			   }
			}
		  }
		  else if(div(Mod,32).quot==4)
		  {
			//Mod = MOD_ALT;
            if((GetKeyState(VK_CONTROL)>=0)&&
			  (GetKeyState(VK_MENU)<0)&&
			  (GetKeyState(VK_SHIFT)>=0)&&
			  ((int)wParam==Key))
			{
			  if(ClosedTabsList->Count>0)
			   {
				 UnCloseTabHotKeyExecute();
				 return -1;
			   }
			}
		  }
		  else if(div(Mod,32).quot==6)
		  {
			//Mod = MOD_ALT | MOD_CONTROL;
			if((GetKeyState(VK_CONTROL)<0)&&
			  (GetKeyState(VK_MENU)<0)&&
			  (GetKeyState(VK_SHIFT)>=0)&&
			  ((int)wParam==Key))
			{
			  if(ClosedTabsList->Count>0)
			   {
				 UnCloseTabHotKeyExecute();
				 return -1;
			   }
			}
		  }
		  else if(div(Mod,32).quot==7)
		  {
			//Mod = MOD_ALT | MOD_CONTROL | MOD_ALT;
			if((GetKeyState(VK_CONTROL)<0)&&
			  (GetKeyState(VK_MENU)<0)&&
			  (GetKeyState(VK_SHIFT)<0)&&
			  ((int)wParam==Key))
			{
			  if(ClosedTabsList->Count>0)
			   {
				 UnCloseTabHotKeyExecute();
				 return -1;
			   }
			}
		  }
		}
	  }
	}
  }
  //Wklejanie tekstu ze schowka jako cytat
  if(QuickQuoteChk)
  {
	//Sprawdzanie statusu klawiszy Ctrl + Q
	if((GetKeyState(VK_CONTROL)<0)&&
	 (GetKeyState(VK_MENU)>=0)&&
	 (GetKeyState(VK_SHIFT)>=0)&&
	 ((int)wParam==81))
	{
	  if(!(HIWORD(lParam)&KF_UP))
	  {
		//Srawdzanie aktywnego okna
		if(GetActiveWindow()==hFrmSend)
		{
		  //Jezeli schowek jest dostepny
		  if(OpenClipboard(NULL))
		  {
			//Pobieranie tekstu ze schowka
			HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
			UnicodeString ClipboardText = (wchar_t*)GlobalLock(hClipboardData);
			GlobalUnlock(hClipboardData);
			CloseClipboard();
			if(!ClipboardText.IsEmpty())
			{
			  //Tekst ze schowka jako cytat
			  ClipboardText = "„" + ClipboardText + "”";
			  //Pobieranie tekstu z RichEdit
			  int iLength = GetWindowTextLengthW(hRichEdit)+1;
			  wchar_t* pBuff = new wchar_t[iLength];
			  GetWindowTextW(hRichEdit, pBuff, iLength);
			  UnicodeString Text = pBuff;
			  delete pBuff;
			  //Usuwanie tekstu zaznaczonego
			  CHARRANGE SelPos;
			  SendMessage(hRichEdit, EM_EXGETSEL, NULL, (LPARAM)&SelPos);
			  if(SelPos.cpMin!=SelPos.cpMax)
			   Text = Text.Delete(SelPos.cpMin+1,SelPos.cpMax-SelPos.cpMin);
			  //Wklejanie cytatu do tekstu
			  Text = Text.Insert(ClipboardText,SelPos.cpMin+1);
			  //Nadpisywanie tekstu w RichEdit
			  SetWindowTextW(hRichEdit, Text.w_str());
			  //Ustawianie pozycji kursora
			  SelPos.cpMin = SelPos.cpMin + ClipboardText.Length();
			  SelPos.cpMax = SelPos.cpMin;
			  SendMessage(hRichEdit, EM_EXSETSEL, NULL, (LPARAM)&SelPos);

			  return -1;
			}
		  }
		}
	  }
	}
  }

  return CallNextHookEx(hThreadKeyboard, nCode, wParam, lParam);
}
//---------------------------------------------------------------------------

//Lokalny hook na myszke
LRESULT CALLBACK ThreadMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  //Blad
  if((nCode<0)||(ForceUnloadExecuted)) return CallNextHookEx(hThreadMouse, nCode, wParam, lParam);

  //Jezeli w tym momencie okno rozmowy nie jest aktywowane oraz nie jest aktywne popupmenu
  if((!FrmSendActivate)&&(!IsWindow(hPopupMenu)))
  {
	//Przywracanie zakladek za pomoca myszki
	if((ClosedTabsChk)&&((UnCloseTabSPMouseChk)||(UnCloseTabLPMouseChk)))
	{
	  if((nCode==HC_ACTION)&&(hTabsBar))
	  {
		//Jezeli niewywolano zamkniecia poprzez 2xLPM
		if(!LBUTTONDBLCLK)
		{
		  //Wcisniecie SPM
		  if(UnCloseTabSPMouseChk)
		  {
			if(wParam==WM_MBUTTONDOWN)
			{
			  if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
			   TabWasChanged = false;
			}
			else if(wParam==WM_MBUTTONUP)
			{
			  if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
			   if(!TabWasChanged) UnCloseTabHotKeyExecute();
			}
		  }
		  //Wcisniecie Ctrl+LPM
		  if(UnCloseTabLPMouseChk)
		  {
			if((wParam==WM_LBUTTONDOWN)&&((GetKeyState(VK_LCONTROL)<0)||(GetKeyState(VK_RCONTROL)<0)))
			{
			  if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
			   TabWasChanged = false;
			}
			if((wParam==WM_LBUTTONUP)&&((GetKeyState(VK_LCONTROL)<0)||(GetKeyState(VK_RCONTROL)<0)))
			{
			  if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
			   if(!TabWasChanged) UnCloseTabHotKeyExecute();
			}
		  }
		}
	  }
	}
	//Zamkniecie zakladki poprzez 2xLPM
	if(CloseBy2xLPMChk)
	{
	  if((nCode==HC_ACTION)&&(hTabsBar))
	  {
		if(wParam==WM_LBUTTONDBLCLK)
		{
		  if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
		  {
			LBUTTONDBLCLK = true;
			mouse_event(MOUSEEVENTF_MIDDLEDOWN,0,0,0,0);
			SetTimer(hTimerFrm,TIMER_CLOSEBY2XLPM,100,(TIMERPROC)TimerFrmProc);
		  }
		}
	  }
	}
  }

  return CallNextHookEx(hThreadMouse, nCode, wParam, lParam);
}
//---------------------------------------------------------------------------

//Przypisywanie globalnego hooka na klawiature
void HookGlobalKeyboard()
{
  //Przypisanie uchwytu do formy ustawien
  if(!hSettingsForm)
  {
	Application->Handle = (HWND)SettingsForm;
	hSettingsForm = new TSettingsForm(Application);
  }
  //Minimalizacja/przywracanie okna rozmowy
  //Wyladowanie hooka
  UnregisterHotKey(hSettingsForm->Handle, 0x0100);
  if(MinimizeRestoreChk)
  {
	//Generowanie Mod i Key
	int Mod = div(MinimizeRestoreKey,256).quot;
	int Key = MinimizeRestoreKey-Mod*256;
	if(div(Mod,32).quot==3) Mod = MOD_SHIFT | MOD_CONTROL;
	else if(div(Mod,32).quot==5) Mod = MOD_SHIFT | MOD_ALT;
	else if(div(Mod,32).quot==2) Mod = MOD_CONTROL;
	else if(div(Mod,32).quot==4) Mod = MOD_ALT;
	else if(div(Mod,32).quot==6) Mod = MOD_ALT | MOD_CONTROL;
	else if(div(Mod,32).quot==7) Mod = MOD_ALT | MOD_CONTROL | MOD_ALT;
	//Zaladowanie hooka
	RegisterHotKey(hSettingsForm->Handle, 0x0100, Mod, Key);
  }
  //SideSlide dla okna kontaktow
  //Wyladowanie hooka
  UnregisterHotKey(hSettingsForm->Handle, 0x0200);
  if(FrmMainSlideChk)
  {
	//Sprawdzanie czy skrot jest aktywny w AQQ
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	bool HotKeysShow = StrToBool(Settings->ReadString("Settings","HotKeysShow","1"));
	delete Settings;
	if(HotKeysShow)
	{
	  //Nowe ustawienia
	  TSaveSetup SaveSetup;
	  SaveSetup.Section = L"Settings";
	  SaveSetup.Ident = L"HotKeysShow";
	  SaveSetup.Value = L"0";
	  //Zapis ustawien
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  //Odswiezenie ustawien
	  PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
	  //Wyladowanie hooka w AQQ
	  UnregisterHotKey(hFrmMain, 1);
	}
	//Zaladowanie hooka
	RegisterHotKey(hSettingsForm->Handle, 0x0200, MOD_SHIFT | MOD_CONTROL, 112);
  }
}
//---------------------------------------------------------------------------

//Hook na aktwyna zakladke lub okno rozmowy
int __stdcall OnActiveTab(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
    //Przywracanie zakladki za pomoca myszki
	TabWasChanged = true;
	//Wylaczenie blokady otwierania paru zakladek jednoczesnie
	NewMgsHoyKeyExecute = false;
	//Blokada zmiany tekstu na zakladce
	TabWasClosed = false;
	//Pobieranie danych
	TPluginContact ActiveTabContact = *(PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)ActiveTabContact.JID;
	UnicodeString Resource = (wchar_t*)ActiveTabContact.Resource;
	if(!Resource.IsEmpty()) Resource = "/" + Resource;
	if(ActiveTabContact.IsChat)
	{
	  JID = "ischat_" + JID;
	  Resource = "";
	}
	//Jezeli zakladka zostala zmieniona
	if((JID+Resource)!=ActiveTabJIDRes)
	{
	  //Aktywna zakladka
	  ActiveTabJID = JID;
	  ActiveTabJIDRes = JID+Resource;
	  //SideSlide - wysuniecie okna zza krawedzi ekranu
	  if((FrmSendSlideChk)&&(!FrmSendVisible)&&(!FrmSendBlockSlide))
	  {
  	    //Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
	    if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
	    //Blokowanie wysuwania przy aplikacji pelnoekranowej
	    if((FullScreenMode)&&(!SideSlideFullScreenModeChk))
		{
		  //Blokada FrmSendSlideIn
		  FullScreenMode = false;
	    }
	    else
		{
  		  //Status FrmSendSlideIn
		  FrmSendSlideIn = true;
		  //Pobieranie pozycji okna rozmowy
		  TRect WindowRect;
		  GetWindowRect(hFrmSend,&WindowRect);
		  //Odswiezenie okna rozmowy
		  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height()+1,SWP_NOMOVE);
		  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
		  //Wlacznie FrmSendSlideIn (part II)
		  SetTimer(hTimerFrm,TIMER_FRMSENDSLIDEIN,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
		  //Schowanie okna kontaktow
		  if((FrmMainSlideChk)&&(FrmMainSlideHideMode==1))
		  {
		    if((FrmMainVisible)&&(!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
		    {
			  //Status FrmMainSlideOut
			  FrmMainSlideOut = true;
			  FrmMainSlideOutActivFrmSend = true;
			  //Wlaczenie FrmMainSlideOut (part I)
			  SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
		    }
		  }
		}
	  }
	  //Usuwanie JID z kolejki pokazywania wiadomosci przy skrocie Ctrl+Shift+F1 lub nieprzypisanym skrotem do zakladki
	  if((FrmMainSlideChk)||(NewMgsHoyKeyChk))
	  {
		if(UnOpenMsgList->IndexOf(JID+Resource)!=-1)
		 UnOpenMsgList->Delete(UnOpenMsgList->IndexOf(JID+Resource));
		if(UnOpenMsgList->IndexOf(JID)!=-1)
		 UnOpenMsgList->Delete(UnOpenMsgList->IndexOf(JID));
	  }
	  //Usuwanie JID z kolejki przelaczania sie na nowe wiadomosci
	  if(SwitchToNewMsgChk)
	  {
		if(MsgList->IndexOf(JID+Resource)!=-1)
		 MsgList->Delete(MsgList->IndexOf(JID+Resource));
		if(MsgList->IndexOf(JID)!=-1)
		 MsgList->Delete(MsgList->IndexOf(JID));
	  }
	  //Jezeli zakladka z kontaktem nie jest otwarta
	  if((ResTabsList->IndexOf(JID+Resource)==-1))
	  {
		//Dodawanie JID do tablicy zakladek
		if(TabsList->IndexOf(JID)==-1) TabsList->Add(JID);
		ResTabsList->Add(JID+Resource);
		//Pobieranie stanu kontaktu
		if(!ActiveTabContact.IsChat)
		{
		  int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&ActiveTabContact);
		  ContactsStateList->WriteInteger("State",JID,State);
		}
		//Pobieranie i zapisywanie indeksu kontatku
		if(!ActiveTabContact.IsChat)
		 ContactsIndexList->WriteInteger("Index",JID,ActiveTabContact.UserIdx);
		//Zapisywanie sesji
		if(RestoreTabsSessionChk)
		{
		  TIniFile *Ini = new TIniFile(SessionFileDir);
		  Ini->EraseSection("Session");
		  //Petla zapisywania otwartych zakladek
		  for(int Count=0;Count<TabsList->Count;Count++)
		  {
			if(((TabsList->Strings[Count].Pos("@plugin"))&&(TabsList->Strings[Count].Pos("ischat_")))||(TabsList->Strings[Count]=="aqq.eu"))
			{ /* Blokada czatu pochodzacego z wtyczki oraz bota aqq.eu */ }
			else
			 Ini->WriteString("Session","Tab"+IntToStr(Count+1),TabsList->Strings[Count]);
		  }
		  //Wczytywanie ostatnio przeprowadzonej rozmowy
		  if((RestoringSession)&&(!ActiveTabContact.IsChat))
		   PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)ActiveTabContact.UserIdx);
		  //Odczytywanie sesji wiadomosci
		  if((RestoreMsgSessionChk)&&(RestoringSession))
		  {
			UnicodeString Body = UTF8ToUnicodeString((IniStrToStr(Ini->ReadString("SessionMsg", JID, ""))).w_str());
			//Wczytanie tresci wiadomosci do pola RichEdit
			if(!Body.IsEmpty())
			{
			  //Ustawianie tekstu
			  SetWindowTextW(hRichEdit, Body.w_str());
			  //Ustawianie pozycji kursora
			  CHARRANGE SelPos;
			  SelPos.cpMin = Body.Length();
			  SelPos.cpMax = SelPos.cpMin;
			  SendMessage(hRichEdit, EM_EXSETSEL, NULL, (LPARAM)&SelPos);
			}
		  }
		  delete Ini;
		}
		//Przypiete zakladki
		if(ClipTabsList->IndexOf(JID)!=-1)
		{
		  //Wlaczanie timera do zmiany miejsca zakladki
		  if(!ActiveTabContact.IsChat) SetTimer(hTimerFrm,TIMER_MOVECLIPTAB,500,(TIMERPROC)TimerFrmProc);
		  else SetTimer(hTimerFrm,TIMER_MOVECLIPTAB,3000,(TIMERPROC)TimerFrmProc);
		}
		//Niewyslane wiadomosci
		if((UnsentMsgChk)&&(!RestoringSession))
		{
		  //Odczyt pliku sesji
		  TIniFile *Ini = new TIniFile(SessionFileDir);
		  UnicodeString Body = UTF8ToUnicodeString((IniStrToStr(Ini->ReadString("Messages", JID, ""))).w_str());
		  //Wczytanie tresci wiadomosci do pola RichEdit
		  if(!Body.IsEmpty())
		  {
			//Pobieranie ostatniej wiadomoœci
			if(!ActiveTabContact.IsChat) PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)ActiveTabContact.UserIdx);
			//Ustawianie tekstu
			SetWindowTextW(hRichEdit, Body.w_str());
			//Ustawianie pozycji kursora
			CHARRANGE SelPos;
			SelPos.cpMin = Body.Length();
			SelPos.cpMax = SelPos.cpMin;
			SendMessage(hRichEdit, EM_EXSETSEL, NULL, (LPARAM)&SelPos);
			//Szybki dostep niewyslanych wiadomosci
			DestroyFrmUnsentMsg();
			//Usuniecie zapisu sesji
			Ini->DeleteKey("Messages", JID);
			//Szybki dostep niewyslanych wiadomosci
			DestroyFrmUnsentMsg();
			BuildFrmUnsentMsg();
			//Ukrywanie ikonki z zasobnika systemowego
			if(hSettingsForm->UnsentMsgTrayIcon->Visible)
			{
			  //Pobieranie ilosci zamknietych zakladek
			  TIniFile *Ini = new TIniFile(SessionFileDir);
			  TStringList *Messages = new TStringList;
			  Ini->ReadSection("Messages",Messages);
			  delete Ini;
			  //Pobieranie ilosci niewyslanych wiadomosci
			  int MsgCount = Messages->Count;
			  delete Messages;
			  //Brak niewyslanych wiadomosci
			  if(!MsgCount) hSettingsForm->UnsentMsgTrayIcon->Visible = false;
			}
		  }
		  delete Ini;
		}
		//Zamkniete zakladki
		if((ClosedTabsChk)&&(!RestoringSession))
		{
		  //Usuwanie JID z listy ostatnio zamknietych zakladek
		  if(ClosedTabsList->IndexOf(JID)!=-1)
		  {
			//Odczytywanie JID ostatnio przywrcanej zakladki
			if((RestoreLastMsgChk)&&(JustUnClosedJID==JID))
			{
			  //Pobieranie ostatniej wiadomoœci
			  if(!ActiveTabContact.IsChat)
			  {
				//Natychmiastowe wczytanie
				if(!UnCloseTabFromHotKey) PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)ActiveTabContact.UserIdx);
				//Wczytanie w timerze
				else
				{
				  //Status sposobu przywracania zakladki
				  UnCloseTabFromHotKey = false;
				  //Przekazanie zmiennych nt. kontaktu
				  UnCloseTabFromHotKeyJID = JID;
				  UnCloseTabFromHotKeyUserIdx = ActiveTabContact.UserIdx;
				  //Wlaczenie timera
				  SetTimer(hTimerFrm,TIMER_LOADLASTCONV,500,(TIMERPROC)TimerFrmProc);
				}
			  }
			  JustUnClosedJID = "";
			}
			//Usuwanie interfejsu
			DestroyFrmClosedTabs();
			//Usuwanie JID z tablicy
			ClosedTabsTimeList->Delete(ClosedTabsList->IndexOf(JID));
			ClosedTabsList->Delete(ClosedTabsList->IndexOf(JID));
			//Maks pamietanych X elementów
			if(ClosedTabsList->Count>CountUnCloseTabVal)
			{
			  while(ClosedTabsList->Count>CountUnCloseTabVal)
			  {
				ClosedTabsList->Delete(CountUnCloseTabVal);
				ClosedTabsTimeList->Delete(CountUnCloseTabVal);
			  }
			}
			//Zapisywanie ostatnio zamknietych zakladek do pliku
			SaveClosedTabs();
			//Tworzenie interfejsu
			BuildFrmClosedTabs();
		  }
		}
	  }
	  else
	  {
		//Usuwanie licznik nowych wiadomosci na zakladkach
		if(InactiveTabsNewMsgChk)
		{
		  int Count = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID+Resource,0);
		  //Tylko dla zakladki z licznikiem nieprzeczytanych wiadomosci
		  if((Count)&&(ClipTabsList->IndexOf(JID)==-1))
		  {
			//Wyladowanie hooka na zmiane tekstu na zakladce
			PluginLink.UnhookEvent(OnTabCaption);
			//Zakladka zwykla
			if(!ActiveTabContact.IsChat)
			 PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)ActiveTabContact.Nick,(LPARAM)&ActiveTabContact);
			//Zakladka z czatem
			else
			{
			  //Czat nie ze wtyczki
			  if(!ActiveTabContact.FromPlugin)
			  {
				UnicodeString tmpJID = JID;
				tmpJID = tmpJID.Delete(1,7);
				TIniFile *Ini = new TIniFile(SessionFileDir);
				UnicodeString Channel = Ini->ReadString("Channels",tmpJID,"");
				delete Ini;
				if(Channel.IsEmpty())
				{
				  Channel = tmpJID;
				  Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
				}
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)Channel.w_str(),(LPARAM)&ActiveTabContact);
			  }
			  //Czat z wtyczki
			  else
			  {
				UnicodeString Caption = JID;
				Caption = Caption.Delete(1,7);
				Caption = Caption.Delete(Caption.Pos("@"),Caption.Length());
				//Usuwanie licznika
				wstring input = Caption.w_str();
				wregex expr(L"[^A-Za-z]");
				wstring replace = L"";
				wstring result = regex_replace( input, expr, replace, match_default | format_sed);
				Caption = result.c_str();
				//Pierwsza duza litera
				UnicodeString FirstUpper = Caption;
				Caption = Caption.Delete(1,1);
				FirstUpper = FirstUpper.Delete(2,FirstUpper.Length());
				Caption = FirstUpper.UpperCase() + Caption;
				//Rodzaj
				UnicodeString Type = JID;
				Type.Delete(1,Type.Pos("@plugin.")+7);
				Type.Delete(Type.Pos("."),Type.Length());
				Caption = Caption + " " + Type.UpperCase();
				//Numer
				UnicodeString Number = JID;
				Number = Number.Delete(1,7);
				Number = Number.Delete(Number.Pos("@"),Number.Length());
				wstring input2 = Number.c_str();
				wregex expr2(L"[^1-9]");
				wstring replace2 = L"";
				wstring result2 = regex_replace( input2, expr2, replace2, match_default | format_sed);
				Number = result2.c_str();
				Caption = Caption + " [nr" + Number + "]";
				//Ustawianie sformatowanego tekstu
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)Caption.w_str(),(LPARAM)&ActiveTabContact);
			  }
			}
			//Hook na zmiane tekstu na zakladce
			PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
		  }
		  //Resetowanie stanu nowych wiadomosci
		  InactiveTabsNewMsgCount->DeleteKey("TabsMsg",JID+Resource);
		}
		//Licznik nowych wiadomosci na oknie rozmowy
		if(InactiveFrmNewMsgChk)
		{
		  //Jezeli okno rozmowy jest aktywne
		  if(hFrmSend==GetActiveWindow())
		  {
			//Kasowanie licznika nowych wiadomosci
			InactiveFrmNewMsgCount = 0;
			//Wyczyszczenie tymczasowego tytulu okna
			FrmSendTitlebar = "";
		  }
		}
		//Zapisywanie sesji
		if(RestoreTabsSessionChk)
		{
		  TIniFile *Ini = new TIniFile(SessionFileDir);
		  Ini->EraseSection("Session");
		  //Petla zapisywania otwartych zakladek
		  for(int Count=0;Count<TabsList->Count;Count++)
		  {
			if(((TabsList->Strings[Count].Pos("@plugin"))&&(TabsList->Strings[Count].Pos("ischat_")))||(TabsList->Strings[Count]=="aqq.eu"))
			{ /* Blokada czatu pochodzacego z wtyczki oraz bota aqq.eu */ }
			else
			 Ini->WriteString("Session","Tab"+IntToStr(Count+1),TabsList->Strings[Count]);
		  }
		  //Wczytywanie ostatnio przeprowadzonej rozmowy
		  if((RestoringSession)&&(!ActiveTabContact.IsChat))
		   PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)ActiveTabContact.UserIdx);
		  //Odczytywanie sesji wiadomosci
		  if((RestoreMsgSessionChk)&&(RestoringSession))
		  {
			UnicodeString Body = UTF8ToUnicodeString((IniStrToStr(Ini->ReadString("SessionMsg", JID, ""))).w_str());
			//Wczytanie tresci wiadomosci do pola RichEdit
			if(!Body.IsEmpty())
			{
			  //Ustawianie tekstu
			  SetWindowTextW(hRichEdit, Body.w_str());
			  //Ustawianie pozycji kursora
			  CHARRANGE SelPos;
			  SelPos.cpMin = Body.Length();
			  SelPos.cpMax = SelPos.cpMin;
			  SendMessage(hRichEdit, EM_EXSETSEL, NULL, (LPARAM)&SelPos);
			}
		  }
		  delete Ini;
		}
	  }
	  //Zmiana caption okna rozmowy
	  if(TweakFrmSendTitlebarChk)
	  {
		if(!ActiveTabContact.IsChat)
		{
		  //Pobranie aktualnego tekstu belki okna
		  wchar_t* TitlebarW = new wchar_t[512];
		  GetWindowTextW(hFrmSend,TitlebarW,512);
		  UnicodeString Titlebar = (wchar_t*)TitlebarW;
		  //Zmienna zmienionego tekstu na belce
		  UnicodeString ChangedTitlebar;
		  //Pobranie pseudonimu kontaktu
		  UnicodeString Nick = (wchar_t*)ActiveTabContact.Nick;
		  //Pseudonim i opis kontatku
		  if(TweakFrmSendTitlebarMode==1)
		  {
			//Pobranie opisu kontatku
			UnicodeString Status = (wchar_t*)ActiveTabContact.Status;
			Status = StringReplace(Status, "\n", " ", TReplaceFlags() << rfReplaceAll);
			Status = StringReplace(Status, "\r", "", TReplaceFlags() << rfReplaceAll);
			//Jezeli opis nie jest pusty
			if(!Status.IsEmpty())
			 ChangedTitlebar = Nick + " - " + Status;
			else
			 ChangedTitlebar = Nick;
		  }
		  //Sam pseudonim kontatku
		  else if(TweakFrmSendTitlebarMode==2)
		   ChangedTitlebar = Nick;
		  //Pseudonim i identyfikator kontaktu
		  else if(TweakFrmSendTitlebarMode==3)
		  {
			//Przyjazniejszy wyglad identyfikatora
			UnicodeString FiendlyJID = JID;
			if(ActiveTabContact.FromPlugin)
			{
			  if(FiendlyJID.Pos("@")) FiendlyJID.Delete(FiendlyJID.Pos("@"),FiendlyJID.Length());
			}
			ChangedTitlebar = Nick + " - " + FiendlyJID;
		  }
		  //Pseudonim i identyfikator kontaktu wraz z zasobem oraz opisem
		  else if((TweakFrmSendTitlebarMode==4)&&(!ActiveTabContact.FromPlugin))
		  {
			//Jezeli zasob nie jest pusty
			if(!Resource.IsEmpty())
			{
			  //Pobranie opisu kontatku
			  UnicodeString Status = (wchar_t*)ActiveTabContact.Status;
			  Status = StringReplace(Status, "\n", " ", TReplaceFlags() << rfReplaceAll);
			  Status = StringReplace(Status, "\r", "", TReplaceFlags() << rfReplaceAll);
			  //Przyjazniejszy wyglad identyfikatora
			  UnicodeString FiendlyJID = JID;
			  if(ActiveTabContact.FromPlugin)
			  {
				if(FiendlyJID.Pos("@")) FiendlyJID.Delete(FiendlyJID.Pos("@"),FiendlyJID.Length());
			  }
			  //Jezeli opis nie jest pusty
			  if(!Status.IsEmpty())
			   ChangedTitlebar = Nick + " - " + FiendlyJID + Resource + " - " + Status;
			  else
			   ChangedTitlebar = Nick + " - " + FiendlyJID + Resource;
			}
		  }
		  //Nowy tekst na belce okna nie jest pusty
		  if(!ChangedTitlebar.IsEmpty())
		  {
			//Zmiana tekstu na belce
			SetWindowTextW(hFrmSend,ChangedTitlebar.w_str());
			//Zapisywanie zmienionego tekstu belki do cache
			ShortString TitlebarShort = UTF8EncodeToShortString(Titlebar);
			ShortString ChangedTitlebarShort = UTF8EncodeToShortString(ChangedTitlebar);
			ChangedTitlebarList->WriteString("Titlebar",StrToIniStr(TitlebarShort.operator AnsiString()),StrToIniStr(ChangedTitlebarShort.operator AnsiString()));
		  }
		}
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na pokazywane wiadomosci
int __stdcall OnAddLine(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	//Jezeli okno archiwum jest nieaktywne
	if(GetActiveWindow()!=hFrmArch)
	{
	  //Transfer plikow - pseudo nowa wiadomosc
	  if((SwitchToNewMsgChk)||(InactiveFrmNewMsgChk)||(InactiveTabsNewMsgChk)||(ClosedTabsChk)||((FrmSendSlideChk)&&(SlideInAtNewMsgChk)&&(!FrmSendVisible)&&(!FrmSendBlockSlide)))
	  {
		//Pobieranie danych wiadomosci
		TPluginMessage AddLineMessage = *(PPluginMessage)lParam;
		//Pobieranie sformatowanej tresci wiadomosci
		UnicodeString Body = (wchar_t*)AddLineMessage.Body;
		//Wyswietlona wiadomosc jest plikiem
		if(FileExists(Body))
		{
		  //Przelaczanie na zakladke z nowa wiadomoscia
		  if(SwitchToNewMsgChk)
		  {
			//Pobieranie danych kontatku
			TPluginContact AddLineContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)AddLineContact.JID;
			UnicodeString Res = (wchar_t*)AddLineContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(AddLineContact.IsChat)
			{
			  JID = "ischat_" + JID;
			  Res = "";
			}
			//Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
			if((JID!=ActiveTabJID)&&(ResTabsList->IndexOf(JID+Res)!=-1))
			{
			  //Dodawanie JID do kolejki nowych wiadomosci
			  if(MsgList->IndexOf(JID+Res)==-1)
			   MsgList->Add(JID+Res);
			}
		  }
		  //Licznik nieprzeczytanych wiadomosci na pasku tytulu okna rozmowy
		  if(InactiveFrmNewMsgChk)
		  {
			//Pobieranie danych kontatku
			TPluginContact AddLineContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)AddLineContact.JID;
			UnicodeString Res = (wchar_t*)AddLineContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(AddLineContact.IsChat)
			{
			  JID = "ischat_" + JID;
			  Res = "";
			}
			//Jezeli zakladka jest otwarta
			if(ResTabsList->IndexOf(JID+Res)!=-1)
			{
			  //Jezeli okno rozmowy jest nieaktywne
			  if(hFrmSend!=GetActiveWindow())
			  {
				//Pobranie oryginalnego tekstu paska tytulu okna rozmowy
				if((FrmSendTitlebar.IsEmpty())&&(!InactiveFrmNewMsgCount))
				{
				  wchar_t* TempTitlebarW = new wchar_t[512];
				  GetWindowTextW(hFrmSend,TempTitlebarW,512);
				  FrmSendTitlebar = (wchar_t*)TempTitlebarW;
				}
				//Dodanie 1 do licznika nieprzeczytachy wiadomosci
				InactiveFrmNewMsgCount++;
				//Ustawianie nowego tekstu paska tytulu okna rozmowy
				SetWindowTextW(hFrmSend,("[" + IntToStr(InactiveFrmNewMsgCount)+ "] " + FrmSendTitlebar).w_str());
			  }
			  else
			  {
				//Kasowanie licznika nowych wiadomosci
				InactiveFrmNewMsgCount = 0;
				//Przywracanie oryginalnego tekstu paska tytulu okna rozmowy
				if(!FrmSendTitlebar.IsEmpty())
				{
				  SetWindowTextW(hFrmSend,FrmSendTitlebar.w_str());
				  FrmSendTitlebar = "";
				}
			  }
			}
		  }
		  //Licznik nieprzeczytanych wiadomosci na zakladkach
		  if(InactiveTabsNewMsgChk)
		  {
			TPluginContact AddLineContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)AddLineContact.JID;
			UnicodeString Res = (wchar_t*)AddLineContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(AddLineContact.IsChat)
			{
			  JID = "ischat_" + JID;
			  Res = "";
			}
			//Jezeli licznik nie ma byc dodawany na przypietej zakladce
			if((ClipTabsList->IndexOf(JID)!=-1)&&(InactiveClipTabsChk))
			 goto SkipInactiveTabsNewMsgChk;
			//Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
			if((JID+Res!=ActiveTabJIDRes)&&(ResTabsList->IndexOf(JID+Res)!=-1))
			{
			  //Pobieranie stanu nowych wiadomosci
			  int InactiveTabsCount = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID+Res,0);
			  //Zmiana stanu nowych wiadomosci
			  InactiveTabsCount++;
			  //Zapisywanie stanu nowych wiadomosci
			  InactiveTabsNewMsgCount->WriteInteger("TabsMsg",JID+Res,InactiveTabsCount);
			  //Ustawianie tekstu na zakladce
			  UnicodeString TabCaption;
			  //Jezeli zakladka jest przypiera
			  if(ClipTabsList->IndexOf(JID)!=-1) TabCaption = "";
			  //Jezeli zakladka nie jest przypieta
			  else
			  {
				//Zakladka ze zwyklyum
				if(!AddLineContact.IsChat) TabCaption = (wchar_t*)AddLineContact.Nick;
				//Zakladka z czatem
				else
				{
				  //Czat nie ze wtyczki
				  if(!AddLineContact.FromPlugin)
				  {
					UnicodeString tmpJID = JID;
					tmpJID = tmpJID.Delete(1,7);
					TIniFile *Ini = new TIniFile(SessionFileDir);
					TabCaption = Ini->ReadString("Channels",tmpJID,"");
					delete Ini;
					if(TabCaption.IsEmpty())
					{
					  TabCaption = tmpJID;
					  TabCaption = TabCaption.Delete(TabCaption.Pos("@"),TabCaption.Length());
					}
				  }
				  //Czat z wtyczki
				  else
				  {
					TabCaption = JID;
					TabCaption = TabCaption.Delete(1,7);
					TabCaption = TabCaption.Delete(TabCaption.Pos("@"),TabCaption.Length());
					//Usuwanie licznika
					wstring input = TabCaption.c_str();
					wregex expr(L"[^A-Za-z]");
					wstring replace = L"";
					wstring result = regex_replace( input, expr, replace, match_default | format_sed);
					TabCaption = result.c_str();
					//Pierwsza duza litera
					UnicodeString FirstUpper = TabCaption;
					TabCaption = TabCaption.Delete(1,1);
					FirstUpper = FirstUpper.Delete(2,FirstUpper.Length());
					TabCaption = FirstUpper.UpperCase() + TabCaption;
					//Rodzaj
					UnicodeString Type = JID;
					Type.Delete(1,Type.Pos("@plugin.")+7);
					Type.Delete(Type.Pos("."),Type.Length());
					TabCaption = TabCaption + " " + Type.UpperCase();
					//Numer
					UnicodeString Number = JID;
					Number = Number.Delete(1,7);
					Number = Number.Delete(Number.Pos("@"),Number.Length());
					wstring input2 = Number.c_str();
					wregex expr2(L"[^1-9]");
					wstring replace2 = L"";
					wstring result2 = regex_replace( input2, expr2, replace2, match_default | format_sed);
					Number = result2.c_str();
					TabCaption = TabCaption + " [nr" + Number + "]";
				  }
				}
			  }
			  //Dodanie licznka do tekstu na zakladce
			  TabCaption = "[" + IntToStr(InactiveTabsCount) + "] " + TabCaption;
			  //Ustawianie nowego tekstu zakladki
			  PluginLink.UnhookEvent(OnTabCaption);
			  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)TabCaption.w_str(),(LPARAM)&AddLineContact);
			  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
			}
			SkipInactiveTabsNewMsgChk: { /* Skip */ }
		  }
		  //Dodawanie JID do listy kontaktow z ktorymy przeprowadzono rozmowe
		  if(ClosedTabsChk)
		  {
			TPluginContact AddLineContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)AddLineContact.JID;
			if(AddLineContact.IsChat) JID = "ischat_" + JID;
			if(AcceptClosedTabsList->IndexOf(JID)==-1)
			{
			  AcceptClosedTabsList->Add(JID);
			}
		  }
		  //SideSlide - wysuwanie okna rozmowy zza krawedzi ekranu
		  if((FrmSendSlideChk)&&(SlideInAtNewMsgChk)&&(!FrmSendVisible)&&(!FrmSendBlockSlide))
		  {
			TPluginContact AddLineContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)AddLineContact.JID;
			UnicodeString Res = (wchar_t*)AddLineContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(AddLineContact.IsChat) Res = "";
			//Jezeli zakladka jest otwarta
			if(ResTabsList->IndexOf(JID+Res)!=-1)
			{
			  //Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
			  if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
			  //Blokowanie wysuwania przy aplikacji pelnoekranowej
			  if((FullScreenMode)&&(!SideSlideFullScreenModeChk))
			  {
				//Blokada FrmSendSlideIn
				FullScreenMode = false;
			  }
			  else
			  {
				//Status FrmSendSlideIn
				FrmSendSlideIn = true;
				//Pobieranie pozycji okna rozmowy
				TRect WindowRect;
				GetWindowRect(hFrmSend,&WindowRect);
				//Odswiezenie okna rozmowy
				SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height()+1,SWP_NOMOVE);
				SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
				//Wlacznie FrmSendSlideIn (part II)
				SetTimer(hTimerFrm,TIMER_FRMSENDSLIDEIN,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
				//Schowanie okna kontaktow
				if((FrmMainSlideChk)&&(FrmMainSlideHideMode==1))
				{
				  if((FrmMainVisible)&&(!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
				  {
					//Status FrmMainSlideOut
					FrmMainSlideOut = true;
					FrmMainSlideOutActivFrmSend = true;
					//Wlaczenie FrmMainSlideOut (part I)
					SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
				  }
				}
			  }
			}
		  }
		}
	  }
	}

	//Zawijanie przeslanych obrazkow do formy zalacznikow
	if(CollapseImagesChk)
	{
	  TPluginMessage AddLineMessage = *(PPluginMessage)lParam;
	  UnicodeString Body = (wchar_t*)AddLineMessage.Body;
	  if(Body.Pos("<IMG CLASS=\"aqqcacheitem\""))
	  {
		//Zwijanie tylko dla wyslanych obrazkow
		if(CollapseImagesMode==2)
		{
		  UnicodeString MessageJID = (wchar_t*)AddLineMessage.JID;
		  if(MessageJID.Pos("/")>0) MessageJID.Delete(MessageJID.Pos("/"),MessageJID.Length());
		  TPluginContact AddLineContact = *(PPluginContact)wParam;
		  UnicodeString ContactJID = (wchar_t*)AddLineContact.JID;
		  if(MessageJID==ContactJID) return 0;
		}
		//Zwijanie tylko dla odebranych obrazkow
		else if(CollapseImagesMode==3)
		{
		  UnicodeString MessageJID = (wchar_t*)AddLineMessage.JID;
		  if(MessageJID.Pos("/")>0) MessageJID.Delete(MessageJID.Pos("/"),MessageJID.Length());
		  TPluginContact AddLineContact = *(PPluginContact)wParam;
		  UnicodeString ContactJID = (wchar_t*)AddLineContact.JID;
		  if(MessageJID!=ContactJID) return 0;
		}
		//Zmienna z zawinietymi obrazkami do formy zalacznika
		UnicodeString NewBody[20];
		//Ilosc obrazkow w wiadomosci
		int ItemsCount = -1;
		//Petla na wystepowanie obrazkow w wiadomosci
		while(Body.Pos("<IMG CLASS=\"aqqcacheitem\""))
		{
		  //Ustalenie ilosci obrazkow w wiadomosci
		  ItemsCount++;
		  //Tymczasowe usuwanie obrazka z wiadomosci
		  UnicodeString BodyTmp = Body;
		  BodyTmp = BodyTmp.Delete(1,BodyTmp.Pos("<A HREF")-1);
		  BodyTmp = BodyTmp.Delete(BodyTmp.Pos("</A>")+4,BodyTmp.Length());
		  Body = StringReplace(Body, BodyTmp, "[AQQCACHEITEM"+IntToStr(ItemsCount)+"]", TReplaceFlags());
		   //Definiowanie stylu formy zalacznika
		  UnicodeString BodyStyle = "<SPAN id=\"icon\">CC_ATTACH_ICON</SPAN> <SPAN id=\"caption\">CC_ATTACH_CAPTION</SPAN>: <SPAN class=\"attach attachment aqqcacheitem\" id=\"title\" onClick=\"document.getElementById('frmSend').style.overflowY='scroll';\">CC_ATTACH_SHORT</SPAN>";
		  //Generowanie ID sesji
		  UnicodeString Session = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0);
		  //Pobranie adresu URL obrazka
		  UnicodeString PhotoFileURL = BodyTmp;
		  PhotoFileURL = PhotoFileURL.Delete(1,PhotoFileURL.Pos("HREF=\"")+5);
		  PhotoFileURL = PhotoFileURL.Delete(PhotoFileURL.Pos("\">"),PhotoFileURL.Length());
		  if(CollapseImagesList->IndexOf(PhotoFileURL)==-1)
		   CollapseImagesList->Add(PhotoFileURL);
		  //Pobranie nazwy obrazka
		  UnicodeString PhotoFileName = BodyTmp;
		  PhotoFileName = PhotoFileName.Delete(1,PhotoFileName.Pos("ALT=\"")+4);
		  PhotoFileName = PhotoFileName.Delete(PhotoFileName.Pos("\""),PhotoFileName.Length());
		  //Pobieranie sciezki URL do grafiki zalacznika
		  UnicodeString ThemePNGPath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,40,0);
		  //Modyfikacja oryginalnego wygladu obrazka
		  BodyTmp = StringReplace(BodyTmp, "\\", "/", TReplaceFlags() << rfReplaceAll);
		  BodyTmp = StringReplace(BodyTmp, "A HREF=\"", "A HREF=\"image:" + Session + ":file:///", TReplaceFlags());
		  BodyTmp = BodyTmp.Delete(BodyTmp.Pos("<IMG"),BodyTmp.Length());
		  BodyTmp = BodyTmp + PhotoFileName + "</A>";
		  //Generowanie nowego wygladu obrazka juz w formie zalacznika
		  BodyStyle = StringReplace(BodyStyle, "CC_ATTACH_ICON", "<IMG border=\"0\" src=\"" + ThemePNGPath + "\">", TReplaceFlags());
		  BodyStyle = StringReplace(BodyStyle, "CC_ATTACH_CAPTION", "<SPAN id=\"id_cctext\">Obrazek</SPAN>", TReplaceFlags());
		  BodyStyle = StringReplace(BodyStyle, "CC_ATTACH_SHORT", "<SPAN id=\"id_cctext\"><SPAN id=\"" + Session + "\">" + BodyTmp + "</SPAN></SPAN>", TReplaceFlags());
		  //Zapisanie nowego wygladu obrazka juz w formie zalacznika do tablicy zmiennych
		  NewBody[ItemsCount] = BodyStyle;
		}
		//Zamienianie tagow na wlasciwa tresc
		for(int Count=0;Count<ItemsCount+1;Count++)
		 Body = StringReplace(Body, "[AQQCACHEITEM"+IntToStr(Count)+"]", NewBody[Count], TReplaceFlags());
		//Zmienianie tresci wiadomosci w notyfikacji
		AddLineMessage.Body = Body.w_str();
		memcpy((PPluginMessage)lParam,&AddLineMessage, sizeof(TPluginMessage));
		return 2;
	  }
	}
	//Skracanie wyœwietlania odnoœników w oknie rozmowy do wygodniejszej formy
	if((ShortenLinksChk)&&((ShortenLinksMode==1)||(ShortenLinksMode==2)))
	{
	  //Pobieranie danych kontatku
	  TPluginContact AddLineContact = *(PPluginContact)wParam;
	  //Pobieranie identyfikatora kontatku
	  UnicodeString ContactJID = (wchar_t*)AddLineContact.JID;
	  //Kontakt nie jest botem Blip
	  if((ContactJID!="blip@blip.pl")&&(ContactJID.Pos("202@plugin.gg")!=1))
	  {
		//Pobieranie danych wiadomosci
		TPluginMessage AddLineMessage = *(PPluginMessage)lParam;
		//Pobieranie sformatowanej tresci wiadomosci
		UnicodeString Body = (wchar_t*)AddLineMessage.Body;
		//Zapisywanie oryginalnej tresci wiadomosci
		UnicodeString BodyOrg = Body;
		//Skracanie wyswietlania odnosnikow
		Body = TrimBodyLinks(Body);
		//Zmienianie tresci wiadomosci
		if(Body!=BodyOrg)
		{
		  AddLineMessage.Body = Body.w_str();
		  memcpy((PPluginMessage)lParam,&AddLineMessage, sizeof(TPluginMessage));
		  return 2;
		}
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zabezpieczenia komunikatora
int __stdcall OnAutoSecureOn(WPARAM wParam, LPARAM lParam)
{
  //Informacja o zabezpieczeniu komunikatora
  SecureMode = true;
  //Zamkniecie okna ustawien
  if(hSettingsForm) hSettingsForm->Close();

  return 0;
}
//---------------------------------------------------------------------------

//Hook na odbezpieczenia komunikatora
int __stdcall OnAutoSecureOff(WPARAM wParam, LPARAM lParam)
{
  //Informacja o odbezpieczeniu komunikatora
  SecureMode = false;

  return 0;
}
//---------------------------------------------------------------------------

//Hook na wylaczenie komunikatora poprzez usera
int __stdcall OnBeforeUnload(WPARAM wParam, LPARAM lParam)
{
  //Info o rozpoczeciu procedury zamykania komunikatora
  ForceUnloadExecuted = true;

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie okna rozmowy lub zakladki
int __stdcall OnCloseTab(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	//Zamkniecie zakladki poprzez 2xLPM
	if(LBUTTONDBLCLK)
	{
	  KillTimer(hTimerFrm,TIMER_CLOSEBY2XLPM);
	  mouse_event(MOUSEEVENTF_MIDDLEUP,0,0,0,0);
	  LBUTTONDBLCLK = false;
	}
	//Przywracanie zakladki za pomoca myszki
	TabWasChanged = true;
	//Blokada zmiany tekstu na zakladce
	TabWasClosed = true;
	//Pobieranie danych dt. kontaktu
	TPluginContact CloseTabContact = *(PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)CloseTabContact.JID;
	UnicodeString Res = (wchar_t*)CloseTabContact.Resource;
	if(!Res.IsEmpty()) Res = "/" + Res;
	if(CloseTabContact.IsChat)
	{
	  JID = "ischat_" + JID;
	  Res = "";
	}
	//Zapisywanie stanu kontaktu
	if(!CloseTabContact.IsChat)
	{
	  int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&CloseTabContact);
	  ContactsStateList->WriteInteger("State",JID,State);
	}
	//Usuwanie JID z listy aktywnych zakladek/okien
	if(ResTabsList->IndexOf(JID+Res)!=-1)
	{
	  if(TabsList->IndexOf(JID)!=-1) TabsList->Delete(TabsList->IndexOf(JID));
	  ResTabsList->Delete(ResTabsList->IndexOf(JID+Res));
	}
	//Usuwanie JID do listy notyfikacji wiadomosci
	if(PreMsgList->IndexOf(JID+Res)!=-1)
	 PreMsgList->Delete(PreMsgList->IndexOf(JID+Res));
	PreMsgStateList->WriteInteger("PreMsgState",JID+Res,0);
	//Zapisywanie sesji
	if(RestoreTabsSessionChk)
	{
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  Ini->EraseSection("Session");
	  if(TabsList->Count>0)
	  {
		//Petla zapisywania otwartych zakladek
		for(int Count=0;Count<TabsList->Count;Count++)
		{
		  if(((TabsList->Strings[Count].Pos("@plugin"))&&(TabsList->Strings[Count].Pos("ischat_")))||(TabsList->Strings[Count]=="aqq.eu"))
		  { /* Blokada czatu pochodzacego z wtyczki oraz bota aqq.eu */ }
		  else
		   Ini->WriteString("Session","Tab"+IntToStr(Count+1),TabsList->Strings[Count]);
		}
	  }
	}
	//Usuwanie JID z kolejki
	if(SwitchToNewMsgChk)
	{
	  if(MsgList->IndexOf(JID+Res)!=-1)
	   MsgList->Delete(MsgList->IndexOf(JID+Res));
	  if(MsgList->IndexOf(JID)!=-1)
	   MsgList->Delete(MsgList->IndexOf(JID));
	}
	//Dodawanie JID do listy ostatnio zamknietych zakladek
	if(ClosedTabsChk)
	{
	  //Sprawdzanie przeprowadzono z kontaktem rozmowe
	  if(OnlyConversationTabsChk)
	  {
		if(AcceptClosedTabsList->IndexOf(JID)==-1)
		 goto SkipClosedTabsChk;
	  }
	  //Sprawdzanie czy kontakt jest czatem z wtyczki lub botem aqq.eu
	  if(((CloseTabContact.IsChat)&&(CloseTabContact.FromPlugin))||(JID=="aqq.eu"))
	   goto SkipClosedTabsChk;
	  //Jezeli kontakt nie znajduje sie na liscie ostatnio zamknietych zakladek
	  if(ClosedTabsList->IndexOf(JID)==-1)
	  {
		//Usuwanie JID z listy z ktorymi przeprowadzono rozmowe
		if(AcceptClosedTabsList->IndexOf(JID)!=-1)
		 AcceptClosedTabsList->Delete(AcceptClosedTabsList->IndexOf(JID));
		//Usuwanie interfejsu
		DestroyFrmClosedTabs();
		//Dodawanie JID do tablicy
		ClosedTabsList->Insert(0,JID);
		TDateTime ClosedTime = TDateTime::CurrentDateTime();
		UnicodeString ClosedTimeStr = ClosedTime.FormatString("ddd d mmm, h:nn");
		ClosedTabsTimeList->Insert(0,ClosedTimeStr);
		//Maks pamietanych X elementów
		if(ClosedTabsList->Count>CountUnCloseTabVal)
		{
		  while(ClosedTabsList->Count>CountUnCloseTabVal)
		  {
			ClosedTabsList->Delete(CountUnCloseTabVal);
			ClosedTabsTimeList->Delete(CountUnCloseTabVal);
		  }
		}
		SaveClosedTabs();
		//Tworzenie interfejsu
		BuildFrmClosedTabs();
	  }
	  SkipClosedTabsChk: { /* Skip */ }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie okna rozmowy wraz z wiadomoscia
int __stdcall OnCloseTabMessage(WPARAM wParam, LPARAM lParam)
{
  if(UnsentMsgChk)
  {
	TPluginContact CloseTabMessageContact = *(PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)CloseTabMessageContact.JID;
	if(CloseTabMessageContact.IsChat) JID = "ischat_" + JID;
	UnicodeString Body = (wchar_t*)wParam;
	Body = Body.Trim();
	//Gdy wiadomosc jest pusta i nie tyczy sie bota aqq.eu
	if((!Body.IsEmpty())&&(JID!="aqq.eu"))
	{
	  //Szybki dostep niewyslanych wiadomosci
	  if(!ForceUnloadExecuted) DestroyFrmUnsentMsg();
	  //Zapis pliku sesji
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  ShortString BodyShort = UTF8EncodeToShortString(Body);
	  Ini->WriteString("Messages", JID, StrToIniStr(BodyShort.operator AnsiString()));
	  delete Ini;
	  //Szybki dostep niewyslanych wiadomosci
	  if(!ForceUnloadExecuted) BuildFrmUnsentMsg();
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kolorystyki AlphaControls
int __stdcall OnColorChange(WPARAM wParam, LPARAM lParam)
{
  //Okno ustawien zostalo juz stworzone
  if(hSettingsForm)
  {
	//Wlaczona zaawansowana stylizacja okien
	if(ChkSkinEnabled())
	{
	  hSettingsForm->sSkinManager->HueOffset = wParam;
	  hSettingsForm->sSkinManager->Saturation = lParam;
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmianê stanu kontaktu
int __stdcall OnContactsUpdate(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	TPluginContact ContactsUpdateContact = *(PPluginContact)wParam;
	if(!ContactsUpdateContact.IsChat)
	{
	  //Pobieranie danych
	  UnicodeString JID = (wchar_t*)ContactsUpdateContact.JID;
	  UnicodeString Res = (wchar_t*)ContactsUpdateContact.Resource;
	  if(!Res.IsEmpty()) Res = "/" + Res;
	  //Pobieranie i zapisywanie stanu kontaktu
	  int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&ContactsUpdateContact);
	  if(State!=ContactsStateList->ReadInteger("State",JID,-1))
	  {
		//Pobieranie i zapisywanie stanu kontatku
		ContactsStateList->WriteInteger("State",JID,State);
		//Pobieranie i zapisywanie indeksu kontatku
		ContactsIndexList->WriteInteger("Index",JID,ContactsUpdateContact.UserIdx);
		//Pobieranie i zapisywanie nicku kontatku
		ContactsNickList->WriteString("Nick",JID,(wchar_t*)ContactsUpdateContact.Nick);
		//Ustawianie prawidlowej ikonki w popumenu ostatnio zamknietych zakladek
		if((ClosedTabsChk)&&(FastAccessClosedTabsChk))
		{
		  //Jezeli JID jest w ostatnio zamknietych zakladkach
		  if(ClosedTabsList->IndexOf(JID)!=-1)
		  {
			//Jezeli JID jest elementem popupmenu
			if(ClosedTabsList->IndexOf(JID)<=ItemCountUnCloseTabVal)
			{
			  //Aktualizacja statusu
			  DestroyFrmClosedTabs();
			  BuildFrmClosedTabs();
			}
		  }
		}
		//Ustawianie prawidlowej ikonki w popumenu niewyslanych wiadomosci
		if((UnsentMsgChk)&&(FastAccessUnsentMsgChk))
		{
		  TIniFile *Ini = new TIniFile(SessionFileDir);
		  TStringList *Messages = new TStringList;
		  Ini->ReadSection("Messages",Messages);
		  delete Ini;
		  int MsgCount = Messages->Count;
		  //Jezeli sa jakies niewyslane wiadomosci
		  if(MsgCount>0)
		  {
			//Jezeli JID jest w niewyslanych wiadomosciach
			if(Messages->IndexOf(JID)!=-1)
			{
			  //Aktualizacja statusu
			  DestroyFrmUnsentMsg();
			  BuildFrmUnsentMsg();
			}
		  }
		  delete Messages;
		}
	  }
	  //Zmiana caption okna rozmowy
	  if((TweakFrmSendTitlebarChk)&&(JID+Res==ActiveTabJIDRes))
	  {
		//Pobranie pseudonimu kontaktu
		UnicodeString Nick = (wchar_t*)ContactsUpdateContact.Nick;
		//Pobranie opisu kontatku
		UnicodeString Status = (wchar_t*)ContactsUpdateContact.Status;
		Status = StringReplace(Status, "\n", " ", TReplaceFlags() << rfReplaceAll);
		//Przyjazniejszy wyglad identyfikatora
		UnicodeString FiendlyJID = JID;
		if(ContactsUpdateContact.FromPlugin)
		{
		  if(FiendlyJID.Pos("@")) FiendlyJID.Delete(FiendlyJID.Pos("@"),FiendlyJID.Length());
		}
		//Generowani nowego oryginalnego tekstu belki okna
		UnicodeString Titlebar;
		if(!Status.IsEmpty())
		 Titlebar = Nick + " - " + FiendlyJID + " - " + Status;
		else
		 Titlebar = Nick + " - " + FiendlyJID;
		//Zmienna zmienionego tekstu na belce
		UnicodeString ChangedTitlebar;
		//Pseudonim i opis kontatku
		if(TweakFrmSendTitlebarMode==1)
		{
		  //Jezeli opis nie jest pusty
		  if(!Status.IsEmpty())
		   ChangedTitlebar = Nick + " - " + Status;
		  else
		   ChangedTitlebar = Nick;
		}
		//Sam pseudonim kontatku
		else if(TweakFrmSendTitlebarMode==2)
		 ChangedTitlebar = Nick;
		//Pseudonim i identyfikator kontaktu
		else if(TweakFrmSendTitlebarMode==3)
		 ChangedTitlebar = Nick + " - " + FiendlyJID;
		//Pseudonim i identyfikator kontaktu wraz z zasobem oraz opisem
		else if((TweakFrmSendTitlebarMode==4)&&(!ContactsUpdateContact.FromPlugin))
		{
		  //Pobranie zasobu kontatku
		  UnicodeString Resource = (wchar_t*)ContactsUpdateContact.Resource;
		  //Jezeli zasob nie jest pusty
		  if(!Resource.IsEmpty())
		  {
			//Jezeli opis nie jest pusty
			if(!Status.IsEmpty())
			 ChangedTitlebar = Nick + " - " + FiendlyJID + "/" + Resource + " - " + Status;
			else
			 ChangedTitlebar = Nick + " - " + FiendlyJID + "/" + Resource;
		  }
		}
		//Nowy tekst na belce okna nie jest pusty
		if(!ChangedTitlebar.IsEmpty())
		{
		  //Licznik nowych wiadomosci na oknie rozmowy jest wlaczony
		  if(InactiveFrmNewMsgChk)
		  {
			//Jezeli okno rozmowy jest nie aktywne
			if(hFrmSend!=GetActiveWindow())
			 //Ustawienie tymczasowego tytulu okna
			 FrmSendTitlebar = ChangedTitlebar;
		  }
		  //Zmiana tekstu na belce
		  SetWindowTextW(hFrmSend,ChangedTitlebar.w_str());
		  //Zapisywanie zmienionego tekstu belki do cache
		  ShortString TitlebarShort = UTF8EncodeToShortString(Titlebar);
		  ShortString ChangedTitlebarShort = UTF8EncodeToShortString(ChangedTitlebar);
		  ChangedTitlebarList->WriteString("Titlebar",StrToIniStr(TitlebarShort.operator AnsiString()),StrToIniStr(ChangedTitlebarShort.operator AnsiString()));
		}
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Pobieranie listy wszystkich otartych zakladek/okien
int __stdcall OnFetchAllTabs(WPARAM wParam, LPARAM lParam)
{
  if((wParam)&&(lParam)&&(!ForceUnloadExecuted))
  {
	//Uchwyt do okna rozmowy nie zostal jeszcze pobrany
	if(!hFrmSend)
	{
	  //Przypisanie uchwytu okna rozmowy
	  hFrmSend = (HWND)(int)wParam;
	  //Przywracanie okna rozmowy
	  if(IsIconic(hFrmSend))
	  {
		ShowWindow(hFrmSend,SW_RESTORE);
		BringWindowToTop(hFrmSend);
		SetForegroundWindow(hFrmSend);
	  }
	  //Okno rozmowy na pierwszy plan
	  SetForegroundWindow(hFrmSend);
	  //Szukanie pola wiadomosci
	  if(!hRichEdit) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	  //Szukanie paska informacyjnego
	  if(!hStatusBar) hStatusBar = FindWindowEx(hFrmSend,NULL,L"TsStatusBar",NULL);
	  //Szukanie paska narzedzi
	  if(!hToolBar) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
	  //Szukanie paska zakladek + innych pod kontrolek
	  if((!hTabsBar)||(!hScrollTabButton[0])||(!hScrollTabButton[1]))
	   EnumChildWindows(hFrmSend,(WNDENUMPROC)FindTabsBar,0);
	  //Przypisanie nowej procki dla okna rozmowy
	  OldFrmSendProc = (WNDPROC)SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG)FrmSendProc);
	  //Pobieranie aktualnej procki okna rozmowy
	  CurrentFrmSendProc = (WNDPROC)GetWindowLongPtr(hFrmSend, GWLP_WNDPROC);
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  GetFrmSendRect();
	  //Ustawienie poprawnej pozycji okna
	  if(FrmSendSlideChk)
	  {
		//Pobranie rozmiaru+pozycji okna rozmowy
		TRect WindowRect;
		GetWindowRect(hFrmSend,&WindowRect);
		//Ustawianie wstepnej pozycji okna rozmowy
		//Left
		if(FrmSendSlideEdge==1)
		 SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
		//Right
		else if(FrmSendSlideEdge==2)
		 SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-WindowRect.Width()-FrmSend_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
		//Bottom
		else if(FrmSendSlideEdge==3)
		 SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
		//Top
		else
		 SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,FrmSend_Shell_TrayWndTop,0,0,SWP_NOSIZE);
		//Pobranie rozmiaru+pozycji okna rozmowy
		GetFrmSendRect();
		//Ustawienie poprawnej pozycji okna rozmowy
		SetFrmSendPos();
		//Pobranie rozmiaru+pozycji okna rozmowy
		GetFrmSendRect();
		//Zapisanie pozycji okna rozmowy do ustawiem AQQ
		TSaveSetup SaveSetup;
		SaveSetup.Section = L"Position";
		SaveSetup.Ident = L"MsgLeft";
		SaveSetup.Value = IntToStr((int)FrmSendRect.Left).w_str();
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		SaveSetup.Section = L"Position";
		SaveSetup.Ident = L"MsgTop";
		SaveSetup.Value = IntToStr((int)FrmSendRect.Top).w_str();
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		//Status okna rozmowy
		FrmSendVisible = true;
		//Wylaczenie tymczasowej blokady
		FrmSendBlockSlide = false;
	  }
	  //Ustawienie statusu okna
	  else FrmSendVisible = true;
	   //Ukrycie paska narzedzi
	  if(HideToolBarChk)
	  {
		//Brak uchwytu do paska narzedzi
		if(!hToolBar) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
		//Pobieranie pozycji paska narzedzi
		TRect WindowRect;
		GetWindowRect(hToolBar,&WindowRect);
		//Ukrycie paska narzedzi
		SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
		//Pobieranie pozycji okna rozmowy
		GetWindowRect(hFrmSend,&WindowRect);
		//Odswiezenie okna rozmowy
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
	  }
	}
	//Pobieranie danych
	TPluginContact FetchAllTabsContact = *(PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)FetchAllTabsContact.JID;
	UnicodeString Res = (wchar_t*)FetchAllTabsContact.Resource;
	if(!Res.IsEmpty()) Res = "/" + Res;
	if(FetchAllTabsContact.IsChat)
	{
	  JID = "ischat_" + JID;
	  Res = "";
	}
	//Dodawanie JID do listy otwartych zakladek
	if(ResTabsList->IndexOf(JID+Res)==-1)
	{
	  if(TabsList->IndexOf(JID)==-1) TabsList->Add(JID);
	  ResTabsList->Add(JID+Res);
	}
	//Pobieranie stanu kontaktu
	if(!FetchAllTabsContact.IsChat)
	{
	  int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&FetchAllTabsContact);
	  ContactsStateList->WriteInteger("State",JID,State);
	}
	//Usuwanie JID z listy ostatnio zamknietych zakladek
	if(ClosedTabsChk)
	{
	  if(ClosedTabsList->IndexOf(JID)!=-1)
	  {
		//Usuwanie JID z tablicy
		ClosedTabsTimeList->Delete(ClosedTabsList->IndexOf(JID));
		ClosedTabsList->Delete(ClosedTabsList->IndexOf(JID));
		//Maks pamietanych X elementów
		if(ClosedTabsList->Count>CountUnCloseTabVal)
		{
		  while(ClosedTabsList->Count>CountUnCloseTabVal)
		  {
			ClosedTabsList->Delete(CountUnCloseTabVal);
			ClosedTabsTimeList->Delete(CountUnCloseTabVal);
		  }
		}
		//Zapisywanie ostatnio zamknietych zakladek do pliku
		SaveClosedTabs();
	  }
	}
	//Zapisywanie sesji
	if(RestoreTabsSessionChk)
	{
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  Ini->EraseSection("Session");
	  //Petla zapisywania otwartych zakladek
	  for(int Count=0;Count<TabsList->Count;Count++)
	  {
		if(((TabsList->Strings[Count].Pos("@plugin"))&&(TabsList->Strings[Count].Pos("ischat_")))||(TabsList->Strings[Count]=="aqq.eu"))
		{ /* Blokada czatu pochodzacego z wtyczki oraz bota aqq.eu */ }
		else
		 Ini->WriteString("Session","Tab"+IntToStr(Count+1),TabsList->Strings[Count]);
	  }
	  //Wczytywanie ostatnio przeprowadzonej rozmowy
	  if((RestoringSession)&&(!FetchAllTabsContact.IsChat))
	   PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)FetchAllTabsContact.UserIdx);
	  //Odczytywanie sesji wiadomosci
	  if((RestoreMsgSessionChk)&&(RestoringSession))
	  {
		UnicodeString Body = UTF8ToUnicodeString((IniStrToStr(Ini->ReadString("SessionMsg", JID, ""))).w_str());
		//Wczytanie tresci wiadomosci do pola RichEdit
		if(!Body.IsEmpty())
		{
		  //Ustawianie tekstu
		  SetWindowTextW(hRichEdit, Body.w_str());
		  //Ustawianie pozycji kursora
		  CHARRANGE SelPos;
		  SelPos.cpMin = Body.Length();
		  SelPos.cpMax = SelPos.cpMin;
		  SendMessage(hRichEdit, EM_EXSETSEL, NULL, (LPARAM)&SelPos);
		}
	  }
	  delete Ini;
	}
	//Jezeli zakladka jest przypieta
	if(ClipTabsList->IndexOf(JID)!=-1)
	{
	  //"Ustawianie" pustego tekstu na przypietej zakladce
	  PluginLink.UnhookEvent(OnTabCaption);
	  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)L"",(LPARAM)&FetchAllTabsContact);
	  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
	  //Zakladka z botem Blip
	  if((JID=="blip@blip.pl")||(JID.Pos("202@plugin.gg")==1))
	  {
		//Zmiana ikonki na zakladce
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)132,(LPARAM)&FetchAllTabsContact);
	  }
	  //Zakladka z botem tweet.IM
	  else if(JID.Pos("@twitter.tweet.im"))
	  {
		//Zmiana ikonki na zakladce
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)131,(LPARAM)&FetchAllTabsContact);
	  }
	  //Zakladka ze zwyklym kontaktem
	  else if((!JID.Pos("ischat_"))&&(MiniAvatarsClipTabsChk))
	  {
		//Pobieranie indeksu ikonki z pamieci
		int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID,0);
		//Ikona juz w interfejsie
		if(Icon)
		{
		  //Zmiana ikonki na zakladce
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
		}
		//Ikona jeszcze niezaladowana do interfejsu AQQ
		else
		{
		  //Jezeli plik PNG jest juz wygenerowany
		  if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
		  {
			//Zaladowanie ikonki do interfejsu
			Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
			//Zapisanie indeksu ikonki do pamieci
			ClipTabsIconList->WriteInteger("ClipTabsIcon",JID,Icon);
			//Zmiana ikonki na zakladce
			PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
		  }
		  //Generowanie pliku PNG 16x16 z awataru kontaktu
		  else
		  {
			TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
			//Przypisanie uchwytu do formy ustawien
			if(!hSettingsForm)
			{
			  Application->Handle = (HWND)SettingsForm;
			  hSettingsForm = new TSettingsForm(Application);
			}
			//Dekodowanie sciezki awatara
			UnicodeString Avatar = hSettingsForm->IdDecoderMIME->DecodeString(Ini->ReadString("Other","Avatar",""));
			delete Ini;
			//Jezeli sciezka awatata zostala prawidlowo pobrana
			if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
			{
			  //Zamienianie sciezki relatywnej na absolutna
			  if(Avatar.Pos("{PROFILEPATH}"))
			   Avatar = StringReplace(Avatar, "{PROFILEPATH}", GetUserDir(), TReplaceFlags());
			  else
			   Avatar = StringReplace(Avatar, "{APPPATH}", GetApplicationPath(), TReplaceFlags());
			  //Konwersja awatara do ikonki PNG 16x16
			  hSettingsForm->ConvertImage(Avatar,PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png");
			  //Jezeli konwersja przeszla prawidlowo
			  if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
			  {
				//Zaladowanie ikonki do interfejsu
				Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
				//Zapisanie indeksu ikonki do pamieci
				ClipTabsIconList->WriteInteger("ClipTabsIcon",JID,Icon);
				//Zmiana ikonki na zakladce
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
			  }
			}
		  }
		}
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------
int __stdcall OnFetchAllTabsW(WPARAM wParam, LPARAM lParam)
{
  if((wParam)&&(lParam)&&(!ForceUnloadExecuted))
  {
	//Pobieranie danych
	TPluginContact FetchAllTabsContact = *(PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)FetchAllTabsContact.JID;
	if(FetchAllTabsContact.IsChat) JID = "ischat_" + JID;
	//Jezeli zakladka jest przypieta
	if((ClipTabsList->IndexOf(JID)!=-1)&&(!UnloadExecuted))
	{
	  //Wyladowanie hooka na zmiane tekstu na zakladce
	  PluginLink.UnhookEvent(OnTabCaption);
	  //Ustawienie tekstu na zakladce
	  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)L"",(LPARAM)&FetchAllTabsContact);
	  //Hook na zmiane tekstu na zakladce
	  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
	  //Zakladka z botem Blip
	  if((JID=="blip@blip.pl")||(JID.Pos("202@plugin.gg")==1))
	  {
		//Zmiana ikonki na zakladce
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)132,(LPARAM)&FetchAllTabsContact);
	  }
	  //Zakladka z botem tweet.IM
	  else if(JID.Pos("@twitter.tweet.im"))
	  {
		//Zmiana ikonki na zakladce
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)131,(LPARAM)&FetchAllTabsContact);
	  }
	  //Zakladka ze zwyklym kontaktem
	  else if((!FetchAllTabsContact.IsChat)&&(MiniAvatarsClipTabsChk))
	  {
		//Pobieranie indeksu ikonki z pamieci
		int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID,0);
		//Ikona juz w interfejsie
		if(Icon)
		{
		  //Zmiana ikonki na zakladce
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
		}
		//Ikona jeszcze niezaladowana do interfejsu AQQ
		else
		{
		  //Jezeli plik PNG jest juz wygenerowany
		  if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
		  {
			//Zaladowanie ikonki do interfejsu
			Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
			//Zapisanie indeksu ikonki do pamieci
			ClipTabsIconList->WriteInteger("ClipTabsIcon",JID,Icon);
			//Zmiana ikonki na zakladce
			PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
		  }
		  //Generowanie pliku PNG 16x16 z awataru kontaktu
		  else
		  {
			TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
			//Przypisanie uchwytu do formy ustawien
			if(!hSettingsForm)
			{
			  Application->Handle = (HWND)SettingsForm;
			  hSettingsForm = new TSettingsForm(Application);
			}
			//Dekodowanie sciezki awatara
			UnicodeString Avatar = hSettingsForm->IdDecoderMIME->DecodeString(Ini->ReadString("Other","Avatar",""));
			delete Ini;
			//Jezeli sciezka awatata zostala prawidlowo pobrana
			if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
			{
			  //Zamienianie sciezki relatywnej na absolutna
			  if(Avatar.Pos("{PROFILEPATH}"))
			   Avatar = StringReplace(Avatar, "{PROFILEPATH}", GetUserDir(), TReplaceFlags());
			  else
			   Avatar = StringReplace(Avatar, "{APPPATH}", GetApplicationPath(), TReplaceFlags());
			  //Konwersja awatara do ikonki PNG 16x16
			  hSettingsForm->ConvertImage(Avatar,PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png");
			  //Jezeli konwersja przeszla prawidlowo
			  if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
			  {
				//Zaladowanie ikonki do interfejsu
				Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
				//Zapisanie indeksu ikonki do pamieci
				ClipTabsIconList->WriteInteger("ClipTabsIcon",JID,Icon);
				//Zmiana ikonki na zakladce
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
			  }
			}
		  }
		}
	  }
	  //Ustawienie domyslnej ikonki kontatku
	  else if((!FetchAllTabsContact.IsChat)&&(!MiniAvatarsClipTabsChk))
	   PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)GetContactState(JID),(LPARAM)&FetchAllTabsContact);
	}
	//Zakladka nie jest przypieta
	else
	{
	  //Ustawienie domyslnej ikonki kontatku
	  if(!FetchAllTabsContact.IsChat) PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)GetContactState(JID),(LPARAM)&FetchAllTabsContact);
	  //Ustawianie domyslnego tekstu na zakladce
	  //Wyladowanie hooka na zmiane tekstu na zakladce
	  PluginLink.UnhookEvent(OnTabCaption);
	  //Zakladka zwykla
	  if(!FetchAllTabsContact.IsChat)
	   PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)FetchAllTabsContact.Nick,(LPARAM)&FetchAllTabsContact);
	  //Zakladka z czatem
	  else
	  {
		//Czat nie ze wtyczki
		if(!FetchAllTabsContact.FromPlugin)
		{
		  UnicodeString tmpJID = JID;
		  tmpJID = tmpJID.Delete(1,7);
		  TIniFile *Ini = new TIniFile(SessionFileDir);
		  UnicodeString Channel = Ini->ReadString("Channels",tmpJID,"");
		  delete Ini;
		  if(Channel.IsEmpty())
		  {
			Channel = tmpJID;
			Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
		  }
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)Channel.w_str(),(LPARAM)&FetchAllTabsContact);
		}
		//Czat z wtyczki
		else
		{
		  UnicodeString Caption = JID;
		  Caption = Caption.Delete(1,7);
		  Caption = Caption.Delete(Caption.Pos("@"),Caption.Length());
		  //Usuwanie licznika
		  wstring input = Caption.w_str();
		  wregex expr(L"[^A-Za-z]");
		  wstring replace = L"";
		  wstring result = regex_replace( input, expr, replace, match_default | format_sed);
		  Caption = result.c_str();
		  //Pierwsza duza litera
		  UnicodeString FirstUpper = Caption;
		  Caption = Caption.Delete(1,1);
		  FirstUpper = FirstUpper.Delete(2,FirstUpper.Length());
		  Caption = FirstUpper.UpperCase() + Caption;
		  //Rodzaj
		  UnicodeString Type = JID;
		  Type.Delete(1,Type.Pos("@plugin.")+7);
		  Type.Delete(Type.Pos("."),Type.Length());
		  Caption = Caption + " " + Type.UpperCase();
		  //Numer
		  UnicodeString Number = JID;
		  Number = Number.Delete(1,7);
		  Number = Number.Delete(Number.Pos("@"),Number.Length());
		  wstring input2 = Number.c_str();
		  wregex expr2(L"[^1-9]");
		  wstring replace2 = L"";
		  wstring result2 = regex_replace( input2, expr2, replace2, match_default | format_sed);
		  Number = result2.c_str();
		  Caption = Caption + " [nr" + Number + "]";
		  //Ustawianie sformatowanego tekstu
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)Caption.w_str(),(LPARAM)&FetchAllTabsContact);
		}
	  }
	  //Hook na zmiane tekstu na zakladce
	  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

int __stdcall OnListReady(WPARAM wParam, LPARAM lParam)
{
  //Pobranie ID dla enumeracji kontaktów
  ReplyListID = GetTickCount();
  //Wywolanie enumeracji kontaktow
  PluginLink.CallService(AQQ_CONTACTS_REQUESTLIST,(WPARAM)ReplyListID,0);

  return 0;
}
//---------------------------------------------------------------------------

//Hook na wpisywany tekst w oknie rozmowy
int __stdcall OnMsgComposing(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	//Zapamietywanie sesji wpisywanego tekstu
	if((RestoreTabsSessionChk)&&(RestoreMsgSessionChk))
	{
	  TPluginContact MsgComposingContact = *(PPluginContact)wParam;
	  TPluginChatState ChatState = *(PPluginChatState)lParam;
	  UnicodeString JID = (wchar_t*)MsgComposingContact.JID;
	  if(MsgComposingContact.IsChat) JID = "ischat_" + JID;
	  UnicodeString Body = (wchar_t*)ChatState.Text;
	  Body = Body.Trim();
	  //Jezeli tekst nie jest pusty i nie tyczy sie bota aqq.eu
	  if((!Body.IsEmpty())&&(JID!="aqq.eu"))
	  {
		//Zapisanie sesji do pliku
		TIniFile *Ini = new TIniFile(SessionFileDir);
		ShortString BodyShort = UTF8EncodeToShortString(Body);
		Ini->WriteString("SessionMsg", JID, StrToIniStr(BodyShort.operator AnsiString()));
		delete Ini;
	  }
	  //Jezeli tekst jest pusty
	  else
	  {
		//Nie jest aktywne przywracanie sesji
		if(!RestoringSession)
		{
		  //Usuniecie sesji z pliku
		  TIniFile *Ini = new TIniFile(SessionFileDir);
		  Ini->DeleteKey("SessionMsg", JID);
		  delete Ini;
		}
	  }
	}
	//Blokowanie FrmSendSlideOut gdy kursor znajduje sie poza oknem
	if((FrmSendSlideChk)&&(FrmSendSlideHideMode==3))
	{
	  if((FrmSendVisible)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))
	  {
		//Gdy kursor znajduje sie poza oknem rozmowy
		if((Mouse->CursorPos.y<FrmSendRect.Top)||(FrmSendRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmSendRect.Left)||(FrmSendRect.Right<Mouse->CursorPos.x))
		{
		  //Zatrzymanie timera wylaczanie tymczasowej blokady
		  KillTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE);
		  //Ustawienie statusu okna dla SideSlide
		  FrmSendBlockSlide = true;
		  FrmSendBlockSlideWndEvent = true;
		  FrmSendBlockSlideOnMsgComposing = true;
		  //Wlaczenie timera wylaczanie tymczasowej blokady
		  SetTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE,2000,(TIMERPROC)TimerFrmProc);
		}
		//Gdy kursor znajduje w obszarze okna rozmowy
		else if(!FrmSendBlockSlide)
		{
		  //Zatrzymanie timera wylaczanie tymczasowej blokady
		  KillTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE);
		  //Ustawienie statusu okna dla SideSlide
		  FrmSendBlockSlide = true;
		  FrmSendBlockSlideWndEvent = true;
		  FrmSendBlockSlideOnMsgComposing = true;
		  //Wlaczenie timera wylaczanie tymczasowej blokady
		  SetTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE,500,(TIMERPROC)TimerFrmProc);
		}
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie menu kontekstowego pola wiadomosci
int __stdcall OnMsgContextClose(WPARAM wParam, LPARAM lParam)
{
  //Usuniecie elemetntu do szybkiego cytowania
  if((QuickQuoteChk)&&(!ForceUnloadExecuted))
  {
	TPluginAction QuickQuoteItem;
	ZeroMemory(&QuickQuoteItem,sizeof(TPluginAction));
	QuickQuoteItem.cbSize = sizeof(TPluginAction);
	QuickQuoteItem.pszName = L"TabKitQuickQuoteItem";
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&QuickQuoteItem));
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na otwarcie menu kontekstowego pola wiadomosci
int __stdcall OnMsgContextPopup(WPARAM wParam, LPARAM lParam)
{
  //Utworzenie elemetntu do szybkiego cytowania
  if((QuickQuoteChk)&&(!ForceUnloadExecuted))
  {
	//Jezeli schowek jest dostepny
	if(OpenClipboard(NULL))
	{
	  //Pobieranie tekstu ze schowka
	  HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
	  UnicodeString ClipboardText = (wchar_t*)GlobalLock(hClipboardData);
	  GlobalUnlock(hClipboardData);
	  CloseClipboard();
	  if(!ClipboardText.IsEmpty())
	  {
		TPluginTriple Triple = *(PPluginTriple)lParam;
		//Ustalanie pozycji dla elemenu
		TPluginItemDescriber PluginItemDescriber;
		PluginItemDescriber.cbSize = sizeof(TPluginItemDescriber);
		PluginItemDescriber.FormHandle = Triple.Handle1;
		PluginItemDescriber.ParentName = L"popRich";
		PluginItemDescriber.Name = L"Wklej1";
		TPluginAction Action = *(PPluginAction)(PluginLink.CallService(AQQ_CONTROLS_GETPOPUPMENUITEM,0,(LPARAM)(&PluginItemDescriber)));
		//Tworzenie elemtu
		TPluginAction QuickQuoteItem;
		ZeroMemory(&QuickQuoteItem,sizeof(TPluginAction));
		QuickQuoteItem.cbSize = sizeof(TPluginAction);
		QuickQuoteItem.pszName = L"TabKitQuickQuoteItem";
		QuickQuoteItem.pszCaption = L"Wklej jako cytat";
		QuickQuoteItem.IconIndex = -1;
		QuickQuoteItem.pszService = L"sTabKitQuickQuoteItem";
		QuickQuoteItem.pszPopupName = L"popRich";
		QuickQuoteItem.Position = Action.Position + 1;
		QuickQuoteItem.Handle = Triple.Handle1;
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&QuickQuoteItem));
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na pobieranie otwieranie adresow URL i przekazywanie plikow do aplikacji
int __stdcall OnPerformCopyData(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	//Domyslne usuwanie elementow
	TPluginAction CollapseImagesItem;
	ZeroMemory(&CollapseImagesItem,sizeof(TPluginAction));
	CollapseImagesItem.cbSize = sizeof(TPluginAction);
	CollapseImagesItem.pszName = L"TabKitCollapseImagesItem";
	CollapseImagesItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&CollapseImagesItem));
	//Kasowanie zapamietanego wczesniej adresu URL
	CollapseImagesItemURL = "";
	//Pobieranie adresu URL
	UnicodeString CopyData = (wchar_t*)lParam;
	//Wlaczona jest funkcjonalnosc chowania okna kontaktow i okno jest poza krawedzia ekranu
	if((FrmMainSlideChk)&&(!FrmMainVisible)&&(FrmMainSlideIn))
	{
	  //Jezeli jest to plik lokalny dodatku do AQQ
	  if((FileExists(CopyData))&&(ExtractFileExt(CopyData)==".aqq"))
	  {
		//Wylaczenie wczesniej aktywowanego wysuwania okna kontaktow
		KillTimer(hTimerFrm,TIMER_FRMMAINSLIDEIN);
		//Status FrmMainSlideIn
		FrmMainSlideIn = false;
	  }
	}
	//Funkcjonalnosc zawijania przeslanych obrazkow do formy zalacznikow jest aktywna
	if((CollapseImagesChk)&&(hFrmSend)&&(!FileExists(CopyData)))
	{
	  //Formatowanie adresu URL
	  CopyData = StringReplace(CopyData, "file:///", "", TReplaceFlags() << rfReplaceAll);
	  CopyData = StringReplace(CopyData, "/", "\\", TReplaceFlags() << rfReplaceAll);
	  CopyData = StringReplace(CopyData, "%20", " ", TReplaceFlags() << rfReplaceAll);
	  //Sprawdzanie czy jest to zawiniety obrazek
	  if(CollapseImagesList->IndexOf(CopyData)!=-1)
	  {
		//Zapisywanie adresu URL
		CollapseImagesItemURL = CopyData;
		//Tworzenie elementu w menu
		ZeroMemory(&CollapseImagesItem,sizeof(TPluginAction));
		CollapseImagesItem.cbSize = sizeof(TPluginAction);
		CollapseImagesItem.pszName = L"TabKitCollapseImagesItem";
		CollapseImagesItem.pszCaption = L"Otwórz";
		CollapseImagesItem.Position = 0;
		CollapseImagesItem.IconIndex = 40;
		CollapseImagesItem.pszService = L"sTabKitCollapseImagesItem";
		CollapseImagesItem.pszPopupName = L"popURL";
		CollapseImagesItem.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&CollapseImagesItem));
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na wysylanie nowej wiadomosci
int __stdcall OnPreSendMsg(WPARAM wParam, LPARAM lParam)
{
  //Dodawanie JID do listy kontaktow z ktorymy przeprowadzono rozmowe
  if((ClosedTabsChk)&&(!ForceUnloadExecuted))
  {
	TPluginContact PreSendMsgContact = *(PPluginContact)wParam;
	UnicodeString JID = (wchar_t*)PreSendMsgContact.JID;
	if(PreSendMsgContact.IsChat) JID = "ischat_" + JID;
	if(AcceptClosedTabsList->IndexOf(JID)==-1)
	{
	  AcceptClosedTabsList->Add(JID);
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na aktywna zakladke
int __stdcall OnPrimaryTab(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	//Jezeli nie zostala wywolana proceduta wyladowania wtyczki
	if(!UnloadExecuted)
	{
	  //Uchwyt do okna rozmowy nie zostal jeszcze pobrany
	  if(!hFrmSend)
	  {
		//Przypisanie uchwytu okna rozmowy
		hFrmSend = (HWND)(int)wParam;
		//Przywracanie okna rozmowy
		if(IsIconic(hFrmSend))
		{
		  ShowWindow(hFrmSend,SW_RESTORE);
		  BringWindowToTop(hFrmSend);
		  SetForegroundWindow(hFrmSend);
		}
		//Okno rozmowy na pierwszy plan
		SetForegroundWindow(hFrmSend);
		//Szukanie pola wiadomosci
		if(!hRichEdit) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
		//Szukanie paska informacyjnego
		if(!hStatusBar) hStatusBar = FindWindowEx(hFrmSend,NULL,L"TsStatusBar",NULL);
		//Szukanie paska narzedzi
		if(!hToolBar) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
		//Szukanie paska zakladek + innych pod kontrolek
		if((!hTabsBar)||(!hScrollTabButton[0])||(!hScrollTabButton[1]))
		 EnumChildWindows(hFrmSend,(WNDENUMPROC)FindTabsBar,0);
		//Przypisanie nowej procki dla okna rozmowy
		OldFrmSendProc = (WNDPROC)SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG)FrmSendProc);
		//Pobieranie aktualnej procki okna rozmowy
	    CurrentFrmSendProc = (WNDPROC)GetWindowLongPtr(hFrmSend, GWLP_WNDPROC);
		//Pobranie rozmiaru+pozycji okna rozmowy
		GetFrmSendRect();
		//Ustawienie poprawnej pozycji okna
		if(FrmSendSlideChk)
		{
		  //Pobranie rozmiaru+pozycji okna rozmowy
		  TRect WindowRect;
		  GetWindowRect(hFrmSend,&WindowRect);
		  //Ustawianie wstepnej pozycji okna rozmowy
		  //Left
		  if(FrmSendSlideEdge==1)
		   SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
		  //Right
		  else if(FrmSendSlideEdge==2)
		   SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-WindowRect.Width()-FrmSend_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
		  //Bottom
		  else if(FrmSendSlideEdge==3)
		   SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
		  //Top
		  else
		   SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,FrmSend_Shell_TrayWndTop,0,0,SWP_NOSIZE);
		  //Pobranie rozmiaru+pozycji okna rozmowy
		  GetFrmSendRect();
		  //Ustawienie poprawnej pozycji okna rozmowy
		  SetFrmSendPos();
		  //Pobranie rozmiaru+pozycji okna rozmowy
		  GetFrmSendRect();
		  //Zapisanie pozycji okna rozmowy do ustawiem AQQ
		  TSaveSetup SaveSetup;
		  SaveSetup.Section = L"Position";
		  SaveSetup.Ident = L"MsgLeft";
		  SaveSetup.Value = IntToStr((int)FrmSendRect.Left).w_str();
		  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		  SaveSetup.Section = L"Position";
		  SaveSetup.Ident = L"MsgTop";
		  SaveSetup.Value = IntToStr((int)FrmSendRect.Top).w_str();
		  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		  //Status okna rozmowy
		  FrmSendVisible = true;
		  //Wylaczenie tymczasowej blokady
		  FrmSendBlockSlide = false;
		}
		//Ustawienie statusu okna
		else FrmSendVisible = true;
		//Ukrycie paska narzedzi
		if(HideToolBarChk)
		{
		  //Brak uchwytu do paska narzedzi
		  if(!hToolBar) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
		  //Pobieranie pozycji paska narzedzi
		  TRect WindowRect;
		  GetWindowRect(hToolBar,&WindowRect);
		  //Ukrycie paska narzedzi
		  SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
		  //Pobieranie pozycji okna rozmowy
		  GetWindowRect(hFrmSend,&WindowRect);
		  //Odswiezenie okna rozmowy
		  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
		  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
		}
	  }
	  //Pobieranie danych
	  TPluginContact PrimaryTabContact = *(PPluginContact)lParam;
	  UnicodeString JID = (wchar_t*)PrimaryTabContact.JID;
	  UnicodeString Res = (wchar_t*)PrimaryTabContact.Resource;
	  if(!Res.IsEmpty()) Res = "/" + Res;
	  if(PrimaryTabContact.IsChat)
	  {
		JID = "ischat_" + JID;
		Res = "";
	  }
	  //Aktywna zakladka
	  ActiveTabJID = JID;
	  ActiveTabJIDRes = JID + Res;
	  //Zmiana tekstu paska tytulu okna rozmowy
	  if(!PrimaryTabContact.IsChat)
	  {
		//Pobieranie danych konatku
		UnicodeString Nick = (wchar_t*)PrimaryTabContact.Nick;
		UnicodeString Status = (wchar_t*)PrimaryTabContact.Status;
		UnicodeString FiendlyJID = JID;
		if(PrimaryTabContact.FromPlugin)
		{
		  if(FiendlyJID.Pos("@")) FiendlyJID.Delete(FiendlyJID.Pos("@"),FiendlyJID.Length());
		}
		//Jezeli funcjonalnosc jest wlaczona
		if(TweakFrmSendTitlebarChk)
		{
		  //Pobranie aktualnego tekstu belki okna
		  wchar_t* TitlebarW = new wchar_t[512];
		  GetWindowTextW(hFrmSend,TitlebarW,512);
		  UnicodeString Titlebar = (wchar_t*)TitlebarW;
		  //Zmienna zmienionego tekstu na belce
		  UnicodeString ChangedTitlebar;
		  //Pseudonim i opis kontatku
		  if(TweakFrmSendTitlebarMode==1)
		  {
			//Jezeli opis nie jest pusty
			if(!Status.IsEmpty())
			 ChangedTitlebar = Nick + " - " + Status;
			else
			 ChangedTitlebar = Nick;
		  }
		  //Sam pseudonim kontatku
		  else if(TweakFrmSendTitlebarMode==2)
		   ChangedTitlebar = Nick;
		  //Pseudonim i identyfikator kontaktu
		  else if(TweakFrmSendTitlebarMode==3)
		   ChangedTitlebar = Nick + " - " + FiendlyJID;
		  //Pseudonim i identyfikator kontaktu wraz z zasobem oraz opisem
		  else if((TweakFrmSendTitlebarMode==4)&&(!PrimaryTabContact.FromPlugin))
		  {
			//Pobranie zasobu kontaktu
			UnicodeString Resource = (wchar_t*)PrimaryTabContact.Resource;
			//Jezeli zasob nie jest pusty
			if(!Resource.IsEmpty())
			{
			  //Jezeli opis nie jest pusty
			  if(!Status.IsEmpty())
			   ChangedTitlebar = Nick + " - " + FiendlyJID + "/" + Resource + " - " + Status;
			  else
			   ChangedTitlebar = Nick + " - " + FiendlyJID + "/" + Resource;
			}
		  }
		  //Nowy tekst na belke okna nie jest pusty
		  if(!ChangedTitlebar.IsEmpty())
		  {
			//Zapisywanie zmienionego tekstu belki do cache
			ShortString TitlebarShort = UTF8EncodeToShortString(Titlebar);
			ShortString ChangedTitlebarShort = UTF8EncodeToShortString(ChangedTitlebar);
			ChangedTitlebarList->WriteString("Titlebar",StrToIniStr(TitlebarShort.operator AnsiString()),StrToIniStr(ChangedTitlebarShort.operator AnsiString()));
			//Zmiana tekstu na belce
			SetWindowTextW(hFrmSend,ChangedTitlebar.w_str());
		  }
		}
		//Jezeli funkcjonalnosc jest wylaczona
		else
		{
		  //Jezeli opis nie jest pusty
		  if(!Status.IsEmpty())
		  {
			//Parsowanie opisu
			Status = StringReplace(Status, "\n", " ", TReplaceFlags() << rfReplaceAll);
			Status = StringReplace(Status, "\r", "", TReplaceFlags() << rfReplaceAll);
			//Ustawianie nowego testku na pasku tytulu okna rozmowy
			SetWindowTextW(hFrmSend,(Nick + " - " + FiendlyJID + " - " + Status).w_str());
		  }
		  //Jezeli opis jest pusty
		  else
		   //Ustawianie nowego testku na pasku tytulu okna rozmowy
		   SetWindowTextW(hFrmSend,(Nick + " - " + FiendlyJID).w_str());
		}
	  }
	}
	//Zmiana tekstu paska tytulu okna rozmowy przy wyladowaniu wtyczki
	else if(TweakFrmSendTitlebarChk)
	{
	  TPluginContact PrimaryTabContact = *(PPluginContact)lParam;
	  //Jezeli kontakt nie jest czatem
	  if(!PrimaryTabContact.IsChat)
	  {
	    //Pobieranie danych konatku
		UnicodeString JID = (wchar_t*)PrimaryTabContact.JID;
		if(PrimaryTabContact.FromPlugin)
		{
		  if(JID.Pos("@")) JID.Delete(JID.Pos("@"),JID.Length());
		}
		UnicodeString Nick = (wchar_t*)PrimaryTabContact.Nick;
		UnicodeString Status = (wchar_t*)PrimaryTabContact.Status;
		//Jezeli opis nie jest pusty
		if(!Status.IsEmpty())
		{
		  //Parsowanie opisu
		  Status = StringReplace(Status, "\n", " ", TReplaceFlags() << rfReplaceAll);
		  Status = StringReplace(Status, "\r", "", TReplaceFlags() << rfReplaceAll);
		  //Ustawianie nowego testku na pasku tytulu okna rozmowy
		  SetWindowTextW(hFrmSend,(Nick + " - " + JID + " - " + Status).w_str());
		}
		//Jezeli opis jest pusty
		else
		 //Ustawianie nowego testku na pasku tytulu okna rozmowy
		 SetWindowTextW(hFrmSend,(Nick + " - " + JID).w_str());
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na odbieranie wiadomosci
int __stdcall OnRecvMsg(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	//Przelaczanie na zakladke z nowa wiadomoscia
	if(SwitchToNewMsgChk)
	{
	  TPluginContact RecvMsgContact = *(PPluginContact)wParam;
	  UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
	  UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
	  if(!Res.IsEmpty()) Res = "/" + Res;
	  if(RecvMsgContact.IsChat)
	  {
		JID = "ischat_" + JID;
		Res = "";
	  }
	  //Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
	  if((JID!=ActiveTabJID)&&(ResTabsList->IndexOf(JID+Res)!=-1))
	  {
		TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if(RecvMsgMessage.Kind!=MSGKIND_RTT)
		{
		  //Jezeli wiadomosc nie jest pusta
		  if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
		  {
			//Dodawanie JID do kolejki nowych wiadomosci
			if(MsgList->IndexOf(JID+Res)==-1)
			 MsgList->Add(JID+Res);
		  }
		}
	  }
	}
	//Licznik nieprzeczytanych wiadomosci na pasku tytulu okna rozmowy
	if(InactiveFrmNewMsgChk)
	{
	  TPluginContact RecvMsgContact = *(PPluginContact)wParam;
	  UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
	  UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
	  if(!Res.IsEmpty()) Res = "/" + Res;
	  if(RecvMsgContact.IsChat)
	  {
		JID = "ischat_" + JID;
		Res = "";
	  }
	  //Jezeli zakladka jest otwarta
	  if(ResTabsList->IndexOf(JID+Res)!=-1)
	  {
		//Jezeli okno rozmowy jest nieaktywne
		if(hFrmSend!=GetActiveWindow())
		{
		  TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
		  //Rodzaj wiadomosci
		  if(RecvMsgMessage.Kind!=MSGKIND_RTT)
		  {
			//Jezeli wiadomosc nie jest pusta
			if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
			{
			  //Pobranie oryginalnego tekstu paska tytulu okna rozmowy
			  if((FrmSendTitlebar.IsEmpty())&&(!InactiveFrmNewMsgCount))
			  {
				wchar_t* TempTitlebarW = new wchar_t[512];
				GetWindowTextW(hFrmSend,TempTitlebarW,512);
				FrmSendTitlebar = (wchar_t*)TempTitlebarW;
			  }
			  //Dodanie 1 do licznika nieprzeczytachy wiadomosci
			  InactiveFrmNewMsgCount++;
			  //Ustawianie nowego tekstu paska tytulu okna rozmowy
			  SetWindowTextW(hFrmSend,("[" + IntToStr(InactiveFrmNewMsgCount)+ "] " + FrmSendTitlebar).w_str());
			}
		  }
		}
		else
		{
		  //Kasowanie licznika nowych wiadomosci
		  InactiveFrmNewMsgCount = 0;
		  //Przywracanie oryginalnego tekstu paska tytulu okna rozmowy
		  if(!FrmSendTitlebar.IsEmpty())
		  {
			SetWindowTextW(hFrmSend,FrmSendTitlebar.w_str());
			FrmSendTitlebar = "";
		  }
		}
	  }
	}
	//Licznik nieprzeczytanych wiadomosci na zakladkach
	if(InactiveTabsNewMsgChk)
	{
	  TPluginContact RecvMsgContact = *(PPluginContact)wParam;
	  UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
	  UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
	  if(!Res.IsEmpty()) Res = "/" + Res;
	  if(RecvMsgContact.IsChat)
	  {
		JID = "ischat_" + JID;
		Res = "";
	  }
	  //Jezeli licznik nie ma byc dodawany na przypietej zakladce
	  if((ClipTabsList->IndexOf(JID)!=-1)&&(InactiveClipTabsChk))
	   goto SkipInactiveTabsNewMsgChk;
	  //Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
	  if((JID+Res!=ActiveTabJIDRes)&&(ResTabsList->IndexOf(JID+Res)!=-1))
	  {
		TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if(RecvMsgMessage.Kind!=MSGKIND_RTT)
		{
		  //Jezeli wiadomosc nie jest pusta
		  if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
		  {
			//Pobieranie stanu nowych wiadomosci
			int InactiveTabsCount = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID+Res,0);
			//Zmiana stanu nowych wiadomosci
			InactiveTabsCount++;
			//Zapisywanie stanu nowych wiadomosci
			InactiveTabsNewMsgCount->WriteInteger("TabsMsg",JID+Res,InactiveTabsCount);
			//Ustawianie tekstu na zakladce
			UnicodeString TabCaption;
			//Jezeli zakladka jest przypiera
			if(ClipTabsList->IndexOf(JID)!=-1) TabCaption = "";
			//Jezeli zakladka nie jest przypieta
			else
			{
			  //Zakladka ze zwyklyum
			  if(!RecvMsgContact.IsChat) TabCaption = (wchar_t*)RecvMsgContact.Nick;
			  //Zakladka z czatem
			  else
			  {
				//Czat nie ze wtyczki
				if(!RecvMsgContact.FromPlugin)
				{
				  UnicodeString tmpJID = JID;
				  tmpJID = tmpJID.Delete(1,7);
				  TIniFile *Ini = new TIniFile(SessionFileDir);
				  TabCaption = Ini->ReadString("Channels",tmpJID,"");
				  delete Ini;
				  if(TabCaption.IsEmpty())
				  {
					TabCaption = tmpJID;
					TabCaption = TabCaption.Delete(TabCaption.Pos("@"),TabCaption.Length());
				  }
				}
				//Czat z wtyczki
				else
				{
				  TabCaption = JID;
				  TabCaption = TabCaption.Delete(1,7);
				  TabCaption = TabCaption.Delete(TabCaption.Pos("@"),TabCaption.Length());
				  //Usuwanie licznika
				  wstring input = TabCaption.c_str();
				  wregex expr(L"[^A-Za-z]");
				  wstring replace = L"";
				  wstring result = regex_replace( input, expr, replace, match_default | format_sed);
				  TabCaption = result.c_str();
				  //Pierwsza duza litera
				  UnicodeString FirstUpper = TabCaption;
				  TabCaption = TabCaption.Delete(1,1);
				  FirstUpper = FirstUpper.Delete(2,FirstUpper.Length());
				  TabCaption = FirstUpper.UpperCase() + TabCaption;
				  //Rodzaj
				  UnicodeString Type = JID;
				  Type.Delete(1,Type.Pos("@plugin.")+7);
				  Type.Delete(Type.Pos("."),Type.Length());
				  TabCaption = TabCaption + " " + Type.UpperCase();
				  //Numer
				  UnicodeString Number = JID;
				  Number = Number.Delete(1,7);
				  Number = Number.Delete(Number.Pos("@"),Number.Length());
				  wstring input2 = Number.c_str();
				  wregex expr2(L"[^1-9]");
				  wstring replace2 = L"";
				  wstring result2 = regex_replace( input2, expr2, replace2, match_default | format_sed);
				  Number = result2.c_str();
				  TabCaption = TabCaption + " [nr" + Number + "]";
				}
			  }
			}
			//Dodanie licznka do tekstu na zakladce
			TabCaption = "[" + IntToStr(InactiveTabsCount) + "] " + TabCaption;
			//Ustawianie nowego tekstu zakladki
			PluginLink.UnhookEvent(OnTabCaption);
			PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)TabCaption.w_str(),(LPARAM)&RecvMsgContact);
			PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
		  }
		}
	  }
	  SkipInactiveTabsNewMsgChk: { /* Skip */ }
	}
	//Notyfikacja nowej wiadomosci w chmurce
	if((InactiveNotiferNewMsgChk)&&(!BlockInactiveNotiferNewMsg)&&(!SecureMode))
	{
	  TPluginContact RecvMsgContact = *(PPluginContact)wParam;
	  UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
	  UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
	  if(!Res.IsEmpty()) Res = "/" + Res;
	  if(RecvMsgContact.IsChat)
	  {
		JID = "ischat_" + JID;
		Res = "";
	  }
	  //Jezeli zakladka jest otwarta
	  if(ResTabsList->IndexOf(JID+Res)!=-1)
	  {
		//Jezeli JID wiadomosci jest inny niz JID aktwnej zakladki
		if(JID!=ActiveTabJID)
		{
		  TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
		  //Rodzaj wiadomosci
		  if((!RecvMsgMessage.ShowAsOutgoing)&&((RecvMsgMessage.Kind==MSGKIND_CHAT)||(RecvMsgMessage.Kind==MSGKIND_GROUPCHAT)))
		  {
			UnicodeString Body = (wchar_t*)RecvMsgMessage.Body;
			//Jezeli wiadomosc nie jest pusta
			if(!Body.IsEmpty())
			{
			  //Pobieranie danych kontaktu
			  UnicodeString Nick = (wchar_t*)RecvMsgContact.Nick;
			  int UserIdx = RecvMsgContact.UserIdx;
			  //Jezeli w wiadomosci znajduje sie obrazek
			  if(Body.Pos("<AQQ_CACHE_ITEM")>0)
			  {
				//Ilosc obrazkow w wiadomosci
				int ItemsCount = 0;
				//Pobieranie tresci wiadomosci
				UnicodeString BodyTmp = Body;
				//Petla na wystepowanie obrazkow w wiadomosci
				while(BodyTmp.Pos("<AQQ_CACHE_ITEM"))
				{
				  //Ustalenie ilosci obrazkow w wiadomosci
				  ItemsCount++;
				  //Tymczasowe usuwanie obrazka z wiadomosci
				  UnicodeString ImgBodyTmp = BodyTmp;
				  ImgBodyTmp = ImgBodyTmp.Delete(1,ImgBodyTmp.Pos("<AQQ_CACHE_ITEM")-1);
				  ImgBodyTmp = ImgBodyTmp.Delete(ImgBodyTmp.Pos(">")+1,ImgBodyTmp.Length());
				  BodyTmp = StringReplace(BodyTmp, ImgBodyTmp, "", TReplaceFlags());
				}
				//Usuwanie bialych znakow
				BodyTmp.Trim();
				//Sprawdzanie zawartosci wiadomosci
				if(BodyTmp.IsEmpty())
				{
				  //1 obrazek
				  if(ItemsCount==1)
				   Body = "[" + Nick + " przesy³a obrazek]";
				  //2-4 obrazki
				  else if((ItemsCount>1)&&(ItemsCount<5))
				   Body = "[" + Nick + " przesy³a "+IntToStr(ItemsCount)+" obrazki]";
				  //5+ obrazkow
				  else
				   Body = "[" + Nick + " przesy³a "+IntToStr(ItemsCount)+" obrazków]";
				}
				//Obrazki wraz z tekstem
				else
				{
				  //Petla na wystepowanie obrazkow w wiadomosci
				  while(Body.Pos("<AQQ_CACHE_ITEM"))
				  {
					//Tymczasowe usuwanie obrazka z wiadomosci
					UnicodeString ImgBodyTmp = BodyTmp;
					ImgBodyTmp = ImgBodyTmp.Delete(1,ImgBodyTmp.Pos("<AQQ_CACHE_ITEM")-1);
					ImgBodyTmp = ImgBodyTmp.Delete(ImgBodyTmp.Pos(">")+1,ImgBodyTmp.Length());
					Body = StringReplace(Body, ImgBodyTmp, "[Obrazek]", TReplaceFlags());
				  }
				}
			  }
			  //Generowanie ID dla chmurek
			  DWORD TickID = GetTickCount();
			  //Nick
			  TPluginShowInfo PluginShowInfo;
			  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			  if(CloudTickModeChk) PluginShowInfo.Event = tmeMsg;
			  else PluginShowInfo.Event = tmePseudoMsg;
			  PluginShowInfo.Text = Nick.w_str();
			  PluginShowInfo.ImagePath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0);
			  PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
			  if(CloudTickModeChk) PluginShowInfo.Tick = TickID;
			  else PluginShowInfo.Tick = 0;
			  PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			  PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
			  //Body
			  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			  if(CloudTickModeChk) PluginShowInfo.Event = tmeMsgCap;
			  else PluginShowInfo.Event = tmePseudoMsgCap;
			  PluginShowInfo.Text = Body.w_str();
			  PluginShowInfo.ImagePath = L"";
			  PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
			  if(CloudTickModeChk) PluginShowInfo.Tick = TickID;
			  else PluginShowInfo.Tick = 0;
			  PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			  PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
			}
		  }
		}
		//Jezeli okno rozmowy nie jest aktywne
		else if(hFrmSend!=GetActiveWindow())
		{
		  TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
		  //Rodzaj wiadomosci
		  if((!RecvMsgMessage.ShowAsOutgoing)&&((RecvMsgMessage.Kind==MSGKIND_CHAT)||(RecvMsgMessage.Kind==MSGKIND_GROUPCHAT)))
		  {
			UnicodeString Body = (wchar_t*)RecvMsgMessage.Body;
			//Jezeli wiadomosc nie jest pusta
			if(!Body.IsEmpty())
			{
			  //Pobieranie danych kontaktu
			  UnicodeString Nick = (wchar_t*)RecvMsgContact.Nick;
			  int UserIdx = RecvMsgContact.UserIdx;
			  //Jezeli w wiadomosci znajduje sie obrazek
			  if(Body.Pos("<AQQ_CACHE_ITEM")>0)
			  {
				//Ilosc obrazkow w wiadomosci
				int ItemsCount = 0;
				//Pobieranie tresci wiadomosci
				UnicodeString BodyTmp = Body;
				//Petla na wystepowanie obrazkow w wiadomosci
				while(BodyTmp.Pos("<AQQ_CACHE_ITEM"))
				{
				  //Ustalenie ilosci obrazkow w wiadomosci
				  ItemsCount++;
				  //Tymczasowe usuwanie obrazka z wiadomosci
				  UnicodeString ImgBodyTmp = BodyTmp;
				  ImgBodyTmp = ImgBodyTmp.Delete(1,ImgBodyTmp.Pos("<AQQ_CACHE_ITEM")-1);
				  ImgBodyTmp = ImgBodyTmp.Delete(ImgBodyTmp.Pos(">")+1,ImgBodyTmp.Length());
				  BodyTmp = StringReplace(BodyTmp, ImgBodyTmp, "", TReplaceFlags());
				}
				//Usuwanie bialych znakow
				BodyTmp.Trim();
				//Sprawdzanie zawartosci wiadomosci
				if(BodyTmp.IsEmpty())
				{
				  //1 obrazek
				  if(ItemsCount==1)
				   Body = "[" + Nick + " przesy³a obrazek]";
				  //2-4 obrazki
				  else if((ItemsCount>1)&&(ItemsCount<5))
				   Body = "[" + Nick + " przesy³a "+IntToStr(ItemsCount)+" obrazki]";
				  //5+ obrazkow
				  else
				   Body = "[" + Nick + " przesy³a "+IntToStr(ItemsCount)+" obrazków]";
				}
				//Obrazki wraz z tekstem
				else
				{
				  //Petla na wystepowanie obrazkow w wiadomosci
				  while(Body.Pos("<AQQ_CACHE_ITEM"))
				  {
					//Tymczasowe usuwanie obrazka z wiadomosci
					UnicodeString ImgBodyTmp = Body;
					ImgBodyTmp = ImgBodyTmp.Delete(1,ImgBodyTmp.Pos("<AQQ_CACHE_ITEM")-1);
					ImgBodyTmp = ImgBodyTmp.Delete(ImgBodyTmp.Pos(">")+1,ImgBodyTmp.Length());
					Body = StringReplace(Body, ImgBodyTmp, "[Obrazek]", TReplaceFlags());
				  }
				}
			  }
			  //Generowanie ID dla chmurek
			  DWORD TickID = GetTickCount();
			  //Nick
			  TPluginShowInfo PluginShowInfo;
			  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			  if(CloudTickModeChk) PluginShowInfo.Event = tmeMsg;
			  else PluginShowInfo.Event = tmePseudoMsg;
			  PluginShowInfo.Text = Nick.w_str();
			  PluginShowInfo.ImagePath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0);
			  PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
			  if(CloudTickModeChk) PluginShowInfo.Tick = TickID;
			  else PluginShowInfo.Tick = 0;
			  PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			  PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
			  //Body
			  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			  if(CloudTickModeChk) PluginShowInfo.Event = tmeMsgCap;
			  else PluginShowInfo.Event = tmePseudoMsgCap;
			  PluginShowInfo.Text = Body.w_str();
			  PluginShowInfo.ImagePath = L"";
			  PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
			  if(CloudTickModeChk) PluginShowInfo.Tick = TickID;
			  else PluginShowInfo.Tick = 0;
			  PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			  PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
			}
		  }
		}
	  }
	}
	//Notyfikcja pisania wiadomosci
	if(ChatStateNotiferNewMsgChk)
	{
	  TPluginContact RecvMsgContact = *(PPluginContact)wParam;
	  UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
	  UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
	  if(!Res.IsEmpty()) Res = "/" + Res;
	  //Jezeli zakladka jest otwarta & kontakt nie jest czatem
	  if((ResTabsList->IndexOf(JID+Res)!=-1)&&(!RecvMsgContact.IsChat))
	  {
		TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
		int ChatState = RecvMsgMessage.ChatState;
		//Jezeli okno rozmowy jest nieaktywne
		if(hFrmSend!=GetActiveWindow())
		{
		  //Pisanie wiadomosci
		  if((ChatState==CHAT_COMPOSING)&&(LastChatState!=ChatState))
		  {
			//Zapamietywanie aktualnego stanu pisania wiadomosci
			LastChatState = ChatState;
			//Pobranie aktualnych ikonek
			while(!hIconSmall) hIconSmall = (HICON)SendMessage(hFrmSend, WM_GETICON, ICON_SMALL, 0);
			while(!hIconBig) hIconBig = (HICON)SendMessage(hFrmSend, WM_GETICON, ICON_BIG, 0);
			//Ustawienie nowej malej ikonki
			SendMessage(hFrmSend, WM_SETICON, ICON_SMALL, (LPARAM)LoadImage(0, ComposingIconSmall.w_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
			//Ustawienie nowej duzej ikonki
			SendMessage(hFrmSend, WM_SETICON, ICON_BIG, (LPARAM)LoadImage(0, ComposingIconBig.w_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE));
		  }
		  //Spauzowanie
		  else if((ChatState==CHAT_PAUSED)&&(LastChatState!=ChatState))
		  {
			//Zapamietywanie aktualnego stanu pisania wiadomosci
			LastChatState = ChatState;
			//Pobranie aktualnych ikonek
			while(!hIconSmall) hIconSmall = (HICON)SendMessage(hFrmSend, WM_GETICON, ICON_SMALL, 0);
			while(!hIconBig) hIconBig = (HICON)SendMessage(hFrmSend, WM_GETICON, ICON_BIG, 0);
			//Ustawienie nowej malej ikonki
			SendMessage(hFrmSend, WM_SETICON, ICON_SMALL, (LPARAM)LoadImage(0, PauseIconSmall.w_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
			//Ustawienie nowej duzej ikonki
			SendMessage(hFrmSend, WM_SETICON, ICON_BIG, (LPARAM)LoadImage(0, PauseIconBig.w_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE));
		  }
		  //Inny stan
		  else if(LastChatState!=ChatState)
		  {
			//Zapamietywanie aktualnego stanu pisania wiadomosci
			LastChatState = ChatState;
			//Ustawienie oryginalnej malej ikonki
			if(hIconSmall)
			{
			  HICON pIconSmall;
			  do { pIconSmall = (HICON)SendMessage(hFrmSend, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall); }
			  while(pIconSmall==hIconSmall);
			  hIconSmall = NULL;
			}
			//Ustawienie oryginalnej duzej ikonki
			if(hIconBig)
			{
			  HICON pIconBig;
			  do { pIconBig = (HICON)SendMessage(hFrmSend, WM_SETICON, ICON_BIG, (LPARAM)hIconBig); }
			  while(pIconBig==hIconBig);
			  hIconBig = NULL;
			}
		  }
		}
		//Notyfikacja na zakladkach
		//Pisanie wiadomosci
		if((ChatState==CHAT_COMPOSING)&&(ChatState!=PreMsgStateList->ReadInteger("PreMsgState",JID+Res,0)))
		{
		  //Dodawanie JID do listy notyfikacji wiadomosci
		  if(PreMsgList->IndexOf(JID+Res)==-1)
		   PreMsgList->Add(JID+Res);
		  PreMsgStateList->WriteInteger("PreMsgState",JID+Res,CHAT_COMPOSING);
		  //Zmiana ikonki na zakladce
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)COMPOSING,(LPARAM)&RecvMsgContact);
		}
		//Spauzowanie
		else if((ChatState==CHAT_PAUSED)&&(ChatState!=PreMsgStateList->ReadInteger("PreMsgState",JID+Res,0)))
		{
		  //Dodawanie JID do listy notyfikacji wiadomosci
		  if(PreMsgList->IndexOf(JID+Res)==-1)
		   PreMsgList->Add(JID+Res);
		  PreMsgStateList->WriteInteger("PreMsgState",JID+Res,CHAT_PAUSED);
		  //Zmiana ikonki na zakladce
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)PAUSE,(LPARAM)&RecvMsgContact);
		}
		//Zamkniecie okna rozmowy
		else if((ChatState==CHAT_GONE)&&(ChatState!=PreMsgStateList->ReadInteger("PreMsgState",JID+Res,0))&&(ChatGoneNotiferNewMsgChk))
		{
		  //Dodawanie JID do listy notyfikacji wiadomosci
		  if(PreMsgList->IndexOf(JID+Res)==-1)
		   PreMsgList->Add(JID+Res);
		  PreMsgStateList->WriteInteger("PreMsgState",JID+Res,CHAT_GONE);
		  //Zmiana ikonki na zakladce
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)GONE,(LPARAM)&RecvMsgContact);
		}
		//Inny stan
		else if(ChatState!=PreMsgStateList->ReadInteger("PreMsgState",JID+Res,0))
		{
		  //Usuwanie JID do listy notyfikacji wiadomosci
		  if(PreMsgList->IndexOf(JID+Res)!=-1)
		   PreMsgList->Delete(PreMsgList->IndexOf(JID+Res));
		  PreMsgStateList->WriteInteger("PreMsgState",JID+Res,0);
		  //Zmiana ikonki na zakladce
		  ChatState = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&RecvMsgContact);
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)ChatState,(LPARAM)&RecvMsgContact);
		}
	  }
	}
	//Dodawanie JID do listy kontaktow z ktorymy przeprowadzono rozmowe
	if(ClosedTabsChk)
	{
	  TPluginContact RecvMsgContact = *(PPluginContact)wParam;
	  UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
	  if(RecvMsgContact.IsChat) JID = "ischat_" + JID;
	  if(AcceptClosedTabsList->IndexOf(JID)==-1)
	  {
		AcceptClosedTabsList->Add(JID);
	  }
	}
	//SideSlide - wysuwanie okna rozmowy zza krawedzi ekranu
	if((FrmSendSlideChk)&&(SlideInAtNewMsgChk)&&(!FrmSendVisible)&&(!FrmSendBlockSlide))
	{
	  TPluginContact RecvMsgContact = *(PPluginContact)wParam;
	  UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
	  UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
	  if(!Res.IsEmpty()) Res = "/" + Res;
	  if(RecvMsgContact.IsChat) Res = "";
	  //Jezeli zakladka jest otwarta
	  if(ResTabsList->IndexOf(JID+Res)!=-1)
	  {
		TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if((!RecvMsgMessage.ShowAsOutgoing)&&((RecvMsgMessage.Kind==MSGKIND_CHAT)||(RecvMsgMessage.Kind==MSGKIND_GROUPCHAT)))
		{
		  //Jezeli wiadomosc nie jest pusta
		  if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
		  {
			//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
			if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
			//Blokowanie wysuwania przy aplikacji pelnoekranowej
			if((FullScreenMode)&&(!SideSlideFullScreenModeChk))
			{
			  //Blokada FrmSendSlideIn
			  FullScreenMode = false;
			}
			else
			{
			  //Status FrmSendSlideIn
			  FrmSendSlideIn = true;
			  //Pobieranie pozycji okna rozmowy
			  TRect WindowRect;
			  GetWindowRect(hFrmSend,&WindowRect);
			  //Odswiezenie okna rozmowy
			  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height()+1,SWP_NOMOVE);
			  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
			  //Wlacznie FrmSendSlideIn (part II)
			  SetTimer(hTimerFrm,TIMER_FRMSENDSLIDEIN,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
			  //Schowanie okna kontaktow
			  if((FrmMainSlideChk)&&(FrmMainSlideHideMode==1))
			  {
				if((FrmMainVisible)&&(!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
				{
				  //Status FrmMainSlideOut
				  FrmMainSlideOut = true;
				  FrmMainSlideOutActivFrmSend = true;
				  //Wlaczenie FrmMainSlideOut (part I)
				  SetTimer(hTimerFrm,TIMER_PREFRMMAINDSLIDEOUT,1,(TIMERPROC)TimerFrmProc);
				}
			  }
			}
		  }
		}
	  }
	}
	//Dodawanie JID do kolejki pokazywania wiadomosci przy skrocie Ctrl+Shift+F1 lub nieprzypisanym skrotem do zakladki
	if((FrmMainSlideChk)||(NewMgsHoyKeyChk))
	{
	  TPluginContact RecvMsgContact = *(PPluginContact)wParam;
	  UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
	  UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
	  if(!Res.IsEmpty()) Res = "/" + Res;
	  if(RecvMsgContact.IsChat)
	  {
		JID = "ischat_" + JID;
		Res = "";
	  }
	  //Jezeli zakladka nie jest otwarta
	  if(ResTabsList->IndexOf(JID+Res)==-1)
	  {
		TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if(RecvMsgMessage.Kind!=MSGKIND_RTT)
		{
		  //Jezeli wiadomosc nie jest pusta
		  if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
		  {
			//Dodawanie JID do kolejki nowych wiadomosci
			if(UnOpenMsgList->IndexOf(JID+Res)==-1)
			 UnOpenMsgList->Add(JID+Res);
			//Pobieranie i zapisywanie indeksu kontatku
			if(!RecvMsgContact.IsChat)
			 ContactsIndexList->WriteInteger("Index",JID,RecvMsgContact.UserIdx);
		  }
		}
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na odbieranie starej procki przekazanej przez wtyczke AlphaWindows
int __stdcall OnRecvOldProc(WPARAM wParam, LPARAM lParam)
{
  //Pobieranie przekazanego uchwytu do okna
  HWND hwnd = (HWND)lParam;
  //Okno kontaktow
  if(hwnd==hFrmMain)
  {
	if(CurrentFrmMainProc==(WNDPROC)GetWindowLongPtr(hFrmMain, GWLP_WNDPROC))
	 OldFrmMainProc = (WNDPROC)wParam;
  }
  //Okno rozmowy
  else if(hwnd==hFrmSend)
  {
	if(CurrentFrmSendProc==(WNDPROC)GetWindowLongPtr(hFrmSend, GWLP_WNDPROC))
	 OldFrmSendProc = (WNDPROC)wParam;
  }
  //Okno wyszukiwarki na liscie kontatkow
  else if(hwnd==hFrmSeekOnList)
  {
	if(CurrentFrmSeekOnListProc==(WNDPROC)GetWindowLongPtr(hFrmSeekOnList, GWLP_WNDPROC))
	 OldFrmSeekOnListProc = (WNDPROC)wParam;
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na enumeracje listy kontatkow
int __stdcall OnReplyList(WPARAM wParam, LPARAM lParam)
{
  //Sprawdzanie ID wywolania enumeracji
  if((wParam==ReplyListID)&&(!ForceUnloadExecuted))
  {
	TPluginContact ReplyListContact = *(PPluginContact)lParam;
	if(!ReplyListContact.IsChat)
	{
	  UnicodeString JID = (wchar_t*)ReplyListContact.JID;
	  //Pobieranie i zapisywanie stanu kontaktu
	  int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&ReplyListContact);
	  if(State!=ContactsStateList->ReadInteger("State",JID,-1))
	  {
		//Pobieranie i zapisywanie stanu kontatku
		ContactsStateList->WriteInteger("State",JID,State);
		//Pobieranie i zapisywanie indeksu kontatku
		ContactsIndexList->WriteInteger("Index",JID,ReplyListContact.UserIdx);
		//Pobieranie i zapisywanie nicku kontatku
		ContactsNickList->WriteString("Nick",JID,(wchar_t*)ReplyListContact.Nick);
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na restart komunikatora
int __stdcall OnSystemRestart(WPARAM wParam, LPARAM lParam)
{
  if((RestoreTabsSessionChk)&&(!ManualRestoreTabsSessionChk))
   ManualRestoreTabsSessionChk = true;

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane nazwy zasobu przez wtyczke ResourcesChanger
int __stdcall OnResourceChanged(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	//Pobranie nowej nazwy zasobu z notyfikacji
	ResourceName = (wchar_t*)lParam;
	//Ustawianie nazwy zasobu glownego konta Jabber na pasku okna kontaktow
	if(TweakFrmMainTitlebarChk)
	{
	  if((TweakFrmMainTitlebarMode==1)&&(TweakFrmMainTitlebarModeEx==2))
	   SetWindowTextW(hFrmMain,("AQQ [" + ResourceName + "]").w_str());
	}
	//Ustawienie poprawnej pozycji okna kontaktow
	if(FrmMainSlideChk)
	{
	  //Wylaczenie funkcjanalnosci
	  FrmMainSlideChk = false;
	  //Przywracanie okna kontatow
	  if(IsIconic(hFrmMain))
	  {
		ShowWindow(hFrmMain,SW_RESTORE);
		BringWindowToTop(hFrmMain);
	  }
	  //Pobranie rozmiaru+pozycji okna kontatkow
	  TRect WindowRect;
	  GetWindowRect(hFrmMain,&WindowRect);
	  //Ustawianie wstepnej pozycji okna rozmowy
	  //Left
	  if(FrmMainSlideEdge==1)
	   SetWindowPos(hFrmMain,HWND_TOP,0+FrmMain_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
	  //Right
	  else if(FrmMainSlideEdge==2)
	   SetWindowPos(hFrmMain,HWND_TOP,Screen->Width-WindowRect.Width()-FrmMain_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
	  //Bottom
	  else if(FrmMainSlideEdge==3)
	   SetWindowPos(hFrmMain,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmMain_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
	  //Top
	  else
	   SetWindowPos(hFrmMain,HWND_TOP,WindowRect.Left,FrmMain_Shell_TrayWndTop,0,0,SWP_NOSIZE);
	  //Pobranie rozmiaru+pozycji okna kontatkow
	  GetFrmMainRect();
	  //Ustawienie poprawnej pozycji okna kontaktow
	  SetFrmMainPos();
	  //Pobranie rozmiaru+pozycji okna kontatkow
	  GetFrmMainRect();
	  //Zapisanie pozycji okna kontaktow do ustawiem AQQ
	  TSaveSetup SaveSetup;
	  SaveSetup.Section = L"Position";
	  SaveSetup.Ident = L"MainLeft";
	  SaveSetup.Value = IntToStr((int)FrmMainRect.Left).w_str();
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  SaveSetup.Section = L"Position";
	  SaveSetup.Ident = L"MainTop";
	  SaveSetup.Value = IntToStr((int)FrmMainRect.Top).w_str();
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  //Status okna kontatkow
	  FrmMainVisible = true;
	  //Wlaczenie funkcjanalnosci
	  FrmMainSlideChk = true;
	}
	//Ustawienie poprawnej pozycji okna rozmowy
	if((FrmSendSlideChk)&&(hFrmSend))
	{
	  //Wylaczenie funkcjanalnosci
	  FrmSendSlideChk = false;
	  //Przywracanie okna rozmowy
	  if(IsIconic(hFrmSend))
	  {
		ShowWindow(hFrmSend,SW_RESTORE);
		BringWindowToTop(hFrmSend);
		SetForegroundWindow(hFrmSend);
	  }
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  TRect WindowRect;
	  GetWindowRect(hFrmSend,&WindowRect);
	  //Ustawianie wstepnej pozycji okna rozmowy
	  //Left
	  if(FrmSendSlideEdge==1)
	   SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
	  //Right
	  else if(FrmSendSlideEdge==2)
	   SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-WindowRect.Width()-FrmSend_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
	  //Bottom
	  else if(FrmSendSlideEdge==3)
	   SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
	  //Top
	  else
	   SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,FrmSend_Shell_TrayWndTop,0,0,SWP_NOSIZE);
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  GetFrmSendRect();
	  //Ustawienie poprawnej pozycji okna rozmowy
	  SetFrmSendPos();
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  GetFrmSendRect();
	  //Zapisanie pozycji okna rozmowy do ustawiem AQQ
	  TSaveSetup SaveSetup;
	  SaveSetup.Section = L"Position";
	  SaveSetup.Ident = L"MsgLeft";
	  SaveSetup.Value = IntToStr((int)FrmSendRect.Left).w_str();
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  SaveSetup.Section = L"Position";
	  SaveSetup.Ident = L"MsgTop";
	  SaveSetup.Value = IntToStr((int)FrmSendRect.Top).w_str();
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  //Status okna rozmowy
	  FrmSendVisible = true;
	  //Wlaczenie funkcjanalnosci
	  FrmSendSlideChk = true;
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane widocznego opisu kontaktu na liscie kontatkow
int __stdcall OnSetHTMLStatus(WPARAM wParam, LPARAM lParam)
{
  //Skracanie wyœwietlania odnoœników w oknie kontatkow do wygodniejszej formy
  if((ShortenLinksChk)&&((ShortenLinksMode==1)||(ShortenLinksMode==3))&&(!ForceUnloadExecuted))
  {
	//Pobieranie sformatowanego opisu
    UnicodeString Body = (wchar_t*)lParam;
    //Jezeli opis cos zawiera
    if(!Body.IsEmpty())
    {
	  //Zapisywanie oryginalnego opisu
	  UnicodeString BodyOrg = Body;
	  //Skracanie wyswietlania odnosnikow
	  Body = TrimStatusLinks(Body);
	  //Zmienianie opisu na liscie kontatkow
	  if(Body!=BodyOrg)
	   return (LPARAM)Body.w_str();
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na polaczenie sieci przy starcie AQQA
int __stdcall OnSetLastState(WPARAM wParam, LPARAM lParam)
{
  if((InactiveNotiferNewMsgChk)||(RestoreTabsSessionChk))
  {
	//Pobieranie ilosci kont
	int UserIdxCount = PluginLink.CallService(AQQ_FUNCTION_GETUSEREXCOUNT,0,0);
	//Sprawdzanie stanu sieci
	for(int UserIdx=0;UserIdx<UserIdxCount;UserIdx++)
	{
	  //Pobieranie stanu sieci
	  TPluginStateChange PluginStateChange;
	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),UserIdx);
	  int NewState = PluginStateChange.NewState;
	  //Connected
	  if(NewState)
	  {
		//Blokowanie notyfikacji nowych wiadomosci
		if(InactiveNotiferNewMsgChk)
		{
		  //Blokowanie notyfikatora nowych wiadomosci
		  BlockInactiveNotiferNewMsg = true;
		  //Tworzenie timera do odblokowania notyfikatora
		  SetTimer(hTimerFrm,TIMER_INACTIVENOTIFER,20000,(TIMERPROC)TimerFrmProc);
		}
		//Przywracanie sesji z czatami
		if(RestoreTabsSessionChk)
		{
		  //Odznaczenie uruchomienia przywracania sesji z czatami
		  RestoringChatSession = true;
		  //Tworzenie timera przywracania sesji z czatami
		  SetTimer(hTimerFrm,TIMER_RESTORESESSION,10000,(TIMERPROC)TimerFrmProc);
		}
		//Zakonczenie petli
		UserIdx = UserIdxCount;
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany stanu
int __stdcall OnStateChange(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	if((InactiveNotiferNewMsgChk)||((RestoreTabsSessionChk)||(!RestoringChatSession)))
	{
	  //Definicja niezbednych zmiennych
	  TPluginStateChange StateChange = *(PPluginStateChange)lParam;
	  int NewState = StateChange.NewState;
	  bool Authorized = StateChange.Authorized;
	  //Connecting
	  if((!Authorized)&&(NewState))
	   //Ustawianie stanu polaczenia sieci
	   NetworkConnecting = true;
	  //Connected
	  else if((NetworkConnecting)&&(Authorized)&&(NewState))
	  {
		//Blokowanie notyfikacji nowych wiadomosci
		if(InactiveNotiferNewMsgChk)
		{
		  //Blokowanie notyfikatora nowych wiadomosci
		  BlockInactiveNotiferNewMsg = true;
		  //Tworzenie timera
		  KillTimer(hTimerFrm,TIMER_INACTIVENOTIFER);
		  SetTimer(hTimerFrm,TIMER_INACTIVENOTIFER,20000,(TIMERPROC)TimerFrmProc);
		}
		//Przywracanie sesji z czatami
		if((RestoreTabsSessionChk)||(!RestoringChatSession))
		{
		  //Niby blokada, lol :D
		  RestoringChatSession = true;
		  //Tworzenie timera
		  SetTimer(hTimerFrm,TIMER_RESTORESESSION,10000,(TIMERPROC)TimerFrmProc);
		}
		//Ustawianie stanu polaczenia sieci
		NetworkConnecting = false;
	  }
	  //Disconnected
	  else if((NetworkConnecting)&&(Authorized)&&(!NewState))
	   //Ustawianie stanu polaczenia sieci
	   NetworkConnecting = false;
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na pokazywanie popumenu
int __stdcall OnSystemPopUp(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	TPluginPopUp PopUp = *(PPluginPopUp)lParam;
	//Pobieranie nazwy popupmenu
	UnicodeString PopUpName = (wchar_t*)PopUp.Name;
	//Popupmenu dostepne spod PPM na zakladce w oknie rozmowy
	if(PopUpName=="popTab")
	{
	  TPluginContact SystemPopUContact = *(PPluginContact)wParam;
	  //Pobieranie JID kontaktu z zakladki
	  UnicodeString JID = (wchar_t*)SystemPopUContact.JID;
	  if(SystemPopUContact.IsChat) JID = "ischat_" + JID;
	  //Zapisanie JID do zmiennej globalnej
	  ClipTabPopup = JID;
	  //Jezeli zakladka nie jest przypieta
	  if(ClipTabsList->IndexOf(ClipTabPopup)==-1)
	  {
		TPluginActionEdit PluginActionEdit;
		PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
		PluginActionEdit.pszName = L"TabKitClipTabItem";
		PluginActionEdit.Caption = L"Przypnij zak³adkê";
		PluginActionEdit.Enabled = true;
		PluginActionEdit.Visible = true;
		PluginActionEdit.IconIndex = -1;
		PluginActionEdit.Checked = false;
		PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
	  }
	  //Jezeli zakladka jest juz przypieta
	  else
	  {
		TPluginActionEdit PluginActionEdit;
		PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
		PluginActionEdit.pszName = L"TabKitClipTabItem";
		PluginActionEdit.Caption = L"Odepnij zak³adkê";
		PluginActionEdit.Enabled = true;
		PluginActionEdit.Visible = true;
		PluginActionEdit.IconIndex = -1;
		PluginActionEdit.Checked = false;
		PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
	  }
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane tekstu na zakladce
int __stdcall OnTabCaption(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	//Pobieranie danych
	TPluginContact TabCaptionContact = *(PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)TabCaptionContact.JID;
	UnicodeString Res = (wchar_t*)TabCaptionContact.Resource;
	if(!Res.IsEmpty()) Res = "/" + Res;
	if(TabCaptionContact.IsChat)
	{
	  JID = "ischat_" + JID;
	  Res = "";
	}
	//Zmienna z tekstem zakladki
	UnicodeString TabCaption = (wchar_t*)wParam;
	//Przypiete zakladki bez licznika znakow
	if((ClipTabsList->IndexOf(JID)!=-1)&&((!CounterClipTabsChk)||(TabCaptionContact.IsChat)))
	 TabCaption = "";
	//Przypiete zakladki z licznikiem znakow
	else if((ClipTabsList->IndexOf(JID)!=-1)&&(CounterClipTabsChk)&&(!TabCaptionContact.IsChat))
	{
	  UnicodeString Nick = (wchar_t*)TabCaptionContact.Nick;
	  TabCaption = StringReplace(TabCaption, Nick + " ", "", TReplaceFlags());
	  TabCaption = StringReplace(TabCaption, Nick, "", TReplaceFlags());
	  TabCaption = TabCaption.Trim();
	}
	//Licznik nieprzeczytanych wiadomosci
	if((InactiveTabsNewMsgChk)&&(!TabWasClosed))
	{
	  int Count = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID+Res,0);
	  if(Count)
	  {
		//Licznik umieszczany na przypietej zakladce
		if((ClipTabsList->IndexOf(JID)!=-1)&&(TabCaption!="["+IntToStr(Count)+"]"))
		{
		  TabCaption = "[" + IntToStr(Count) + "] " + TabCaption;
		  TabCaption = TabCaption.Trim();
		}
		//Licznik umieszczany na nie przypietej zakladce
		else if((ClipTabsList->IndexOf(JID)==-1)&&(TabCaption.Pos("[" + IntToStr(Count) + "] ")!=1))
		{
		 TabCaption = "[" + IntToStr(Count) + "] " + TabCaption;
		  TabCaption = TabCaption.Trim();
		}
	  }
	}
	//Nieprzypieta zakladka czatowa i normalizacja nazw
	if((ClipTabsList->IndexOf(JID)==-1)&&(TabCaptionContact.IsChat)&&(!TabCaptionContact.FromPlugin))
	{
	  int Count = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID+Res,0);
	  if(!Count)
	  {
		UnicodeString tmpJID = JID;
		tmpJID = tmpJID.Delete(1,7);
		TIniFile *Ini = new TIniFile(SessionFileDir);
		TabCaption = Ini->ReadString("Channels",tmpJID,"");
		delete Ini;
		if(TabCaption.IsEmpty())
		{
		  TabCaption = tmpJID;
		  TabCaption = TabCaption.Delete(TabCaption.Pos("@"),TabCaption.Length());
		}
	  }
	}
	//Zwrot zmienionego caption
	return (WPARAM)TabCaption.w_str();
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane ikonki na zakladce
int __stdcall OnTabImage(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	//Notyfikcja pisania wiadomosci
	if(PreMsgList->Count>0)
	{
	  TPluginContact TabImageContact = *(PPluginContact)lParam;
	  //Nie jest to zakladka czatowa
	  if(!TabImageContact.IsChat)
	  {
		UnicodeString JID = (wchar_t*)TabImageContact.JID;
		UnicodeString Res = (wchar_t*)TabImageContact.Resource;
		if(!Res.IsEmpty()) Res = "/" + Res;
		//Jezeli zakladka dotyczy notyfikacji pisania wiadomosci
		if(PreMsgList->IndexOf(JID+Res)!=-1)
		{
		  int TabImage = (int)wParam;
		  if((TabImage!=COMPOSING)&&(TabImage!=PAUSE)&&(TabImage!=GONE)&&(TabImage!=8))
		  {
			int ChatState = PreMsgStateList->ReadInteger("PreMsgState",JID+Res,0);
			//Pisanie wiadomosci
			if(ChatState==CHAT_COMPOSING)
			 return COMPOSING;
			//Spauzowanie
			else if(ChatState==CHAT_PAUSED)
			 return PAUSE;
			//Zamkniecie okna rozmowy
			else if(ChatState==CHAT_GONE)
			 return GONE;
			//Domyslna ikona
			else
			 //Usuwanie JID z listy notyfikacji wiadomosci
			 PreMsgList->Delete(PreMsgList->IndexOf(JID+Res));
		  }
		}
	  }
	}
	//Przypiete zakladki
	if(ClipTabsList->Count>0)
	{
	  TPluginContact TabImageContact = *(PPluginContact)lParam;
	  //Nie jest to zakladka czatowa
	  if(!TabImageContact.IsChat)
	  {
		UnicodeString JID = (wchar_t*)TabImageContact.JID;
		//Jezeli zakladka jest przypieta
		if(ClipTabsList->IndexOf(JID)!=-1)
		{
		  int TabImage = (int)wParam;
		  //Jezeli nie jest zmieniane na ikonke nowej wiadomosci itp
		  if((TabImage!=8)&&(TabImage!=COMPOSING)&&(TabImage!=PAUSE))
		  {
			//Zakladka z botem Blip
			if((JID=="blip@blip.pl")||(JID.Pos("202@plugin.gg")==1))
			{
			  return 132;
			}
			//Zakladka z botem tweet.IM
			else if(JID.Pos("@twitter.tweet.im"))
			{
			  return 131;
			}
			//Zakladka ze zwyklym kontaktem
			else if(MiniAvatarsClipTabsChk)
			{
			  //Pobieranie indeksu ikonki z pamieci
			  int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID,0);
			  //Ikona juz w interfejsie
			  if(Icon)
			   return Icon;
			  //Ikona jeszcze niezaladowana do interfejsu
			  else
			  {
				//Jezeli plik PNG jest juz wygenerowany
				if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
				{
				  //Zaladowanie ikonki do interfejsu
				  Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
				  //Zapisanie indeksu ikonki do pamieci
				  ClipTabsIconList->WriteInteger("ClipTabsIcon",JID,Icon);
				  return Icon;
				}
				//Generowanie pliku PNG 16x16 z awataru kontaktu
				else
				{
				  TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
				  //Przypisanie uchwytu do formy ustawien
				  if(!hSettingsForm)
				  {
					Application->Handle = (HWND)SettingsForm;
					hSettingsForm = new TSettingsForm(Application);
				  }
				  //Dekodowanie sciezki awatara
				  UnicodeString Avatar = hSettingsForm->IdDecoderMIME->DecodeString(Ini->ReadString("Other","Avatar",""));
				  delete Ini;
				  //Jezeli sciezka awatata zostala prawidlowo pobrana
				  if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
				  {
					//Zamienianie sciezki relatywnej na absolutna
					if(Avatar.Pos("{PROFILEPATH}"))
					 Avatar = StringReplace(Avatar, "{PROFILEPATH}", GetUserDir(), TReplaceFlags());
					else
					 Avatar = StringReplace(Avatar, "{APPPATH}", GetApplicationPath(), TReplaceFlags());
					//Konwersja awatara do ikonki PNG 16x16
					hSettingsForm->ConvertImage(Avatar,PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png");
					//Jezeli konwersja przeszla prawidlowo
					if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
					{
					  //Zaladowanie ikonki do interfejsu
					  Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
					  //Zapisanie indeksu ikonki do pamieci
					  ClipTabsIconList->WriteInteger("ClipTabsIcon",JID,Icon);
					  return Icon;
					}
				  }
				}
			  }
			}
		  }
	    }
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmianê kompozycji
int __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam)
{
  //Informacja o zmianie kompozycji
  ThemeChanging = true;
  //Pobieranie sciezki nowej aktywnej kompozycji
  UnicodeString ThemeDir = StringReplace((wchar_t*)lParam, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  //Okno ustawien zostalo juz stworzone
  if(hSettingsForm)
  {
	//Wlaczona zaawansowana stylizacja okien
	if(ChkSkinEnabled())
	{
	  UnicodeString ThemeSkinDir = ThemeDir + "\\\\Skin";
	  //Plik zaawansowanej stylizacji okien istnieje
	  if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
	  {
		//Dane pliku zaawansowanej stylizacji okien
		ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
		hSettingsForm->sSkinManager->SkinDirectory = ThemeSkinDir;
		hSettingsForm->sSkinManager->SkinName = "Skin.asz";
		//Ustawianie animacji AlphaControls
		if(ChkThemeAnimateWindows()) hSettingsForm->sSkinManager->AnimEffects->FormShow->Time = 200;
		else hSettingsForm->sSkinManager->AnimEffects->FormShow->Time = 0;
		hSettingsForm->sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
		//Zmiana kolorystyki AlphaControls
        hSettingsForm->sSkinManager->HueOffset = GetHUE();
	    hSettingsForm->sSkinManager->Saturation = GetSaturation();
		//Aktywacja skorkowania AlphaControls
		hSettingsForm->sSkinManager->Active = true;
	  }
	  //Brak pliku zaawansowanej stylizacji okien
	  else hSettingsForm->sSkinManager->Active = false;
	}
	//Zaawansowana stylizacja okien wylaczona
	else hSettingsForm->sSkinManager->Active = false;
	//Aktywne skorkowanie AlphaControls
	if(hSettingsForm->sSkinManager->Active)
	{
	  //Kolor WebLabel'ow
	  hSettingsForm->EmailWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->EmailWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->JabberWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->JabberWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->URLWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->URLWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->ForumWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->ForumWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->BugWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->BugWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->StarWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->StarWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->OtherPaymentsWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	  hSettingsForm->OtherPaymentsWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
	}
	//Nieaktywne skorkowanie AlphaControls
	else
	{
	  //Kolor WebLabel'ow
	  hSettingsForm->EmailWebLabel->Font->Color = clWindowText;
	  hSettingsForm->EmailWebLabel->HoverFont->Color = clWindowText;
	  hSettingsForm->JabberWebLabel->Font->Color = clWindowText;
	  hSettingsForm->JabberWebLabel->HoverFont->Color = clWindowText;
	  hSettingsForm->URLWebLabel->Font->Color = clWindowText;
	  hSettingsForm->URLWebLabel->HoverFont->Color = clWindowText;
	  hSettingsForm->ForumWebLabel->Font->Color = clWindowText;
	  hSettingsForm->ForumWebLabel->HoverFont->Color = clWindowText;
	  hSettingsForm->BugWebLabel->Font->Color = clWindowText;
	  hSettingsForm->BugWebLabel->HoverFont->Color = clWindowText;
	  hSettingsForm->StarWebLabel->Font->Color = clWindowText;
	  hSettingsForm->StarWebLabel->HoverFont->Color = clWindowText;
	  hSettingsForm->OtherPaymentsWebLabel->Font->Color = clWindowText;
	  hSettingsForm->OtherPaymentsWebLabel->HoverFont->Color = clWindowText;
	}
  }
  //Aktualizacja ikon z interfejsu
  //CLOSEDTABS
  if(FileExists(ThemeDir + "\\\\TabKit\\\\ClosedTabsButton.png"))
   CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,CLOSEDTABS, (LPARAM)(ThemeDir + "\\\\TabKit\\\\ClosedTabsButton.png").w_str());
  else
   CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,CLOSEDTABS, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\ClosedTabsButton.png").w_str());
  //UNSENTMSG
  if(FileExists(ThemeDir + "\\\\TabKit\\\\UnsentMsg.png"))
   UNSENTMSG = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,UNSENTMSG, (LPARAM)(ThemeDir + "\\\\TabKit\\\\UnsentMsg.png").w_str());
  else
   UNSENTMSG = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,UNSENTMSG, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\UnsentMsg.png").w_str());
  //COMPOSING
  if(FileExists(ThemeDir + "\\\\TabKit\\\\Composing.png"))
   COMPOSING = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,COMPOSING, (LPARAM)(ThemeDir + "\\\\TabKit\\\\Composing.png").w_str());
  else
   COMPOSING = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,COMPOSING, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\Composing.png").w_str());
  //PAUSE
  if(FileExists(ThemeDir + "\\\\TabKit\\\\Pause.png"))
   PAUSE = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,PAUSE, (LPARAM)(ThemeDir + "\\\\TabKit\\\\Pause.png").w_str());
  else
   PAUSE = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,PAUSE, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\Pause.png").w_str());
  //GONE
  if(FileExists(ThemeDir + "\\\\TabKit\\\\Gone.png"))
   GONE = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,PAUSE, (LPARAM)(ThemeDir + "\\\\TabKit\\\\Gone.png").w_str());
  else
   GONE = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,PAUSE, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\Gone.png").w_str());
  //STAYONTOP_OFF
  if(FileExists(ThemeDir + "\\\\TabKit\\\\StayOnTopOff.png"))
   STAYONTOP_OFF = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,STAYONTOP_OFF, (LPARAM)(ThemeDir + "\\\\TabKit\\\\StayOnTopOff.png").w_str());
  else
   STAYONTOP_OFF = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,STAYONTOP_OFF, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\StayOnTopOff.png").w_str());
  //STAYONTOP_ON
  if(FileExists(ThemeDir + "\\\\TabKit\\\\StayOnTopOn.png"))
   STAYONTOP_ON = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,STAYONTOP_ON, (LPARAM)(ThemeDir + "\\\\TabKit\\\\StayOnTopOn.png").w_str());
  else
   STAYONTOP_ON = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,STAYONTOP_ON, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\StayOnTopOn.png").w_str());
  //FASTACCESS
  if(FileExists(ThemeDir + "\\\\TabKit\\\\FastAccess.png"))
   FASTACCESS = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,FASTACCESS, (LPARAM)(ThemeDir + "\\\\TabKit\\\\FastAccess.png").w_str());
  else
   FASTACCESS = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,FASTACCESS, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\FastAccess.png").w_str());
  //Przypisanie nowych sciezek do ikon
  //Composing_Small
  if(FileExists(ThemeDir + "\\\\TabKit\\\\Composing_Small.ico"))
   ComposingIconSmall = ThemeDir + "\\\\TabKit\\\\Composing_Small.ico";
  else
   ComposingIconSmall = PluginUserDir + "\\\\TabKit\\\\Composing_Small.ico";
  //Composing_Big
  if(FileExists(ThemeDir + "\\\\TabKit\\\\Composing_Big.ico"))
   ComposingIconBig = ThemeDir + "\\\\TabKit\\\\Composing_Big.ico";
  else
   ComposingIconBig = PluginUserDir + "\\\\TabKit\\\\Composing_Big.ico";
  //Pause_Small
  if(FileExists(ThemeDir + "\\\\TabKit\\\\Pause_Small.ico"))
   PauseIconSmall = ThemeDir + "\\\\TabKit\\\\Pause_Small.ico";
  else
   PauseIconSmall = PluginUserDir + "\\\\TabKit\\\\Pause_Small.ico";
  //Pause_Big
  if(FileExists(ThemeDir + "\\\\TabKit\\\\Pause_Big.ico"))
   PauseIconBig = ThemeDir + "\\\\TabKit\\\\Pause_Big.ico";
  else
   PauseIconBig = PluginUserDir + "\\\\TabKit\\\\Pause_Big.ico";
  //Zmiany w oknie rozmowy
  if(hFrmSend)
  {
	//Szukanie pola wiadomosci
	EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
  }
  //Zmiany w oknie kontaktow
  //Szukanie uchwytu do kontrolki IE w oknie kontatkow
  EnumChildWindows(hFrmMain,(WNDENUMPROC)FindFrmMainFocus,0);
  //Ustawianie poprawnej pozycji okna rozmowy
  if((FrmSendSlideChk)&&(hFrmSend))
  {
	//Wylaczenie funkcjanalnosci
	FrmSendSlideChk = false;
	//Przywracanie okna rozmowy
	if(IsIconic(hFrmSend))
	{
	  ShowWindow(hFrmSend,SW_RESTORE);
	  BringWindowToTop(hFrmSend);
	  SetForegroundWindow(hFrmSend);
	}
	//Pobranie rozmiaru+pozycji okna rozmowy
	TRect WindowRect;
	GetWindowRect(hFrmSend,&WindowRect);
	//Ustawianie wstepnej pozycji okna rozmowy
	//Left
	if(FrmSendSlideEdge==1)
	 SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
	//Right
	else if(FrmSendSlideEdge==2)
	 SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-WindowRect.Width()-FrmSend_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
	//Bottom
	else if(FrmSendSlideEdge==3)
	 SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
	//Top
	else
	 SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,FrmSend_Shell_TrayWndTop,0,0,SWP_NOSIZE);
	//Pobranie rozmiaru+pozycji okna rozmowy
	GetFrmSendRect();
	//Ustawienie poprawnej pozycji okna rozmowy
	SetFrmSendPos();
	//Pobranie rozmiaru+pozycji okna rozmowy
	GetFrmSendRect();
	//Zapisanie pozycji okna rozmowy do ustawiem AQQ
	TSaveSetup SaveSetup;
	SaveSetup.Section = L"Position";
	SaveSetup.Ident = L"MsgLeft";
	SaveSetup.Value = IntToStr((int)FrmSendRect.Left).w_str();
	PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	SaveSetup.Section = L"Position";
	SaveSetup.Ident = L"MsgTop";
	SaveSetup.Value = IntToStr((int)FrmSendRect.Top).w_str();
	PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	//Status okna rozmowy
	FrmSendSlideIn = false;
	PreFrmSendSlideOut = false;
	FrmSendSlideOut = false;
	FrmSendVisible = true;
	FrmSendBlockSlide = false;
	//Wlaczenie funkcjanalnosci
	FrmSendSlideChk = true;
  }
  //Pobranie rozmiaru+pozycji okna rozmowy
  else if(hFrmSend)
   GetFrmSendRect();
  //Ustawienie poprawnej pozycji okna kontatkow
  if(FrmMainSlideChk)
  {
	//Wylaczenie funkcjanalnosci
	FrmMainSlideChk = false;
	//Przywracanie okna kontatow
	if(IsIconic(hFrmMain))
	{
	  ShowWindow(hFrmMain,SW_RESTORE);
	  BringWindowToTop(hFrmMain);
	}
	//Pobranie rozmiaru+pozycji okna kontatkow
	TRect WindowRect;
	GetWindowRect(hFrmMain,&WindowRect);
	//Ustawianie wstepnej pozycji okna rozmowy
	//Left
	if(FrmMainSlideEdge==1)
	 SetWindowPos(hFrmMain,HWND_TOP,0+FrmMain_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
	//Right
	else if(FrmMainSlideEdge==2)
	 SetWindowPos(hFrmMain,HWND_TOP,Screen->Width-WindowRect.Width()-FrmMain_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
	//Bottom
	else if(FrmMainSlideEdge==3)
	 SetWindowPos(hFrmMain,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmMain_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
	//Top
	else
	 SetWindowPos(hFrmMain,HWND_TOP,WindowRect.Left,FrmMain_Shell_TrayWndTop,0,0,SWP_NOSIZE);
	//Pobranie rozmiaru+pozycji okna kontatkow
	GetFrmMainRect();
	//Ustawienie poprawnej pozycji okna kontaktow
	SetFrmMainPos();
	//Pobranie rozmiaru+pozycji okna kontatkow
	GetFrmMainRect();
	//Zapisanie pozycji okna kontaktow do ustawiem AQQ
	TSaveSetup SaveSetup;
	SaveSetup.Section = L"Position";
	SaveSetup.Ident = L"MainLeft";
	SaveSetup.Value = IntToStr((int)FrmMainRect.Left).w_str();
	PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	SaveSetup.Section = L"Position";
	SaveSetup.Ident = L"MainTop";
	SaveSetup.Value = IntToStr((int)FrmMainRect.Top).w_str();
	PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	//Status okna kontatkow
	FrmMainSlideIn = false;
	PreFrmMainSlideOut = false;
	CurPreFrmMainSlideOut = false;
	FrmMainSlideOut = false;
	FrmMainVisible = true;
	FrmMainBlockSlide = false;
	//Wlaczenie funkcjanalnosci
	FrmMainSlideChk = true;
  }
  //Pobranie rozmiaru+pozycji okna kontatkow
  else
   GetFrmMainRect();
  //Usuniecie info o zmianie kompozycji
  ThemeChanging = false;

  return 0;
}
//---------------------------------------------------------------------------

//Hook na klikniecie LPM w ikonke tray
int __stdcall OnTrayClick(WPARAM wParam, LPARAM lParam)
{
  //Funkcjonalnosc SideSlide dla okna kontaktow jest wlaczona
  if((FrmMainSlideChk)&&(!ForceUnloadExecuted))
  {
	//Wizyczne kliniecie LPM w tray
	if((lParam==0)&&(wParam==MB_LEFT))
	{
	  //Jezeli w tray nie miga kopertka
	  if(UnOpenMsgList->Count==0)
	  {
		//Minimalizacja / przywracanie okna kontaktow + otwieranie okna rozmowy z nowa wiadomoscia
		MinimizeRestoreFrmMainExecute();
		//Blokada domyslnych akcji AQQ
		return 1;
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie/otwarcie okien
int __stdcall OnWindowEvent(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	//Pobranie informacji o oknie i eventcie
	TPluginWindowEvent WindowEvent = *(PPluginWindowEvent)lParam;
	int Event = WindowEvent.WindowEvent;
	UnicodeString ClassName = (wchar_t*)WindowEvent.ClassName;

	//Otwarcie okna kontaktow = zaladowanie w pelni listy kontatkow
	if((ClassName=="TfrmMain")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Przypisanie uchwytu do okna glownego
	  hFrmMain = (HWND)(int)WindowEvent.Handle;
	  //Pobieranie oryginalnego titlebar glownego okna
	  GetWindowTextW(hFrmMain,FrmMainTitlebar,16);
	  //Szukanie uchwytu do kontrolki IE w oknie kontatkow
	  EnumChildWindows(hFrmMain,(WNDENUMPROC)FindFrmMainFocus,0);
	  //Pobranie rozmiaru+pozycji okna kontatkow
	  GetFrmMainRect();
	  //Zmiana pozycji okna kontatkow
	  if(FrmMainSlideChk)
	  {
		//Zmiana stanu okna kontatkow
		SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		//Ustawienie poprawnej pozycji okna kontaktow
		SetFrmMainPos();
		//Pobranie rozmiaru+pozycji okna kontatkow
		GetFrmMainRect();
		//Aktywacja okna kontaktow
		BringWindowToTop(hFrmMain);
		SetForegroundWindow(hFrmMain);
		SetFocus(hFrmMainFocus);
		SetActiveWindow(hFrmMain);
		//Zapisanie pozycji okna kontaktow do ustawiem AQQ
		TSaveSetup SaveSetup;
		SaveSetup.Section = L"Position";
		SaveSetup.Ident = L"MainLeft";
		SaveSetup.Value = IntToStr((int)FrmMainRect.Left).w_str();
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		SaveSetup.Section = L"Position";
		SaveSetup.Ident = L"MainTop";
		SaveSetup.Value = IntToStr((int)FrmMainRect.Top).w_str();
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		//Odswiezenie ustawien
		PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
	  }
	  //Ustawienie statusu okna dla SideSlide
	  FrmMainVisible = true;
	  FrmMainBlockSlide = false;
	  //Przypisanie nowej procki dla okna kontatkow
	  OldFrmMainProc = (WNDPROC)SetWindowLongPtrW(hFrmMain, GWLP_WNDPROC,(LONG)FrmMainProc);
	  //Pobieranie aktualnej procki okna kontaktow
	  CurrentFrmMainProc = (WNDPROC)GetWindowLongPtr(hFrmMain, GWLP_WNDPROC);
	  //Odczytywanie sesji
	  if(RestoreTabsSessionChk)
	  {
		TIniFile *Ini = new TIniFile(SessionFileDir);
		TStringList *Session = new TStringList;
		Ini->ReadSection("Session",Session);
		//Jezeli sa jakies zakladki do przywrocenia
		if(Session->Count>0)
		{
		  //Status przywracania sesji
		  RestoringSession = true;
		  //Odczyt sesji
		  for(int Count=0;Count<Session->Count;Count++)
		   Session->Strings[Count] = Ini->ReadString("Session","Tab"+IntToStr(Count+1),"");
		  //Przywracanie zakladek
		  for(int Count=0;Count<Session->Count;Count++)
		  {
			UnicodeString JID = Session->Strings[Count];
			//Otwieranie zakladki z kontektem
			if(!JID.IsEmpty())
			{
			  //Otwieranie zakladki ze zwyklym kontektem
			  if(!JID.Pos("ischat_"))
			  {
				if(!JID.Pos("/")) PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG_NOPRIORITY,0,(LPARAM)JID.w_str());
				else PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG_NOPRIORITY,(WPARAM)GetContactIndex(JID),(LPARAM)JID.w_str());
			  }
			  //"Otwieranie" zakladki z czatem
			  else
			   ChatSessionList->Add(JID);
			}
		  }
		  //Usuwanie sesji wiadomosci
		  Ini->EraseSection("SessionMsg");
		  //Kasowanie uchwytu do ostatnio aktywnego okna - anty never endig SlideIn FrmMain
		  LastActiveWindow = NULL;
		  //Status przywracania sesji
		  RestoringSession = false;
		}
		delete Session;
		delete Ini;
	  }
	  //Pobieranie ostatnio zamknietych zakladek
	  GetClosedTabs();
	  //Tworzenie interfesju dla ostatnio zamknietych zakladek
	  BuildFrmClosedTabs();
	  //Tworzenie interfesju w AQQ dla ostatnio zamknietych zakladek
	  BuildAQQClosedTabs();
	  //Sprawdzanie niewyslanych wiadomosci
	  GetUnsentMsg();
	  //Szybki dostep niewyslanych wiadomosci
	  DestroyFrmUnsentMsg();
	  BuildFrmUnsentMsg();
	  //Pobieranie nazwy komputera
	  wchar_t compName[256];
	  DWORD len = sizeof(compName);
	  GetComputerNameEx(ComputerNameDnsHostname,compName,&len);
	  ComputerName = compName;
	  //Ustawianie tekstu na pasku okna kontaktow
	  if(TweakFrmMainTitlebarChk)
	  {
	    //Tryb I
	    if(TweakFrmMainTitlebarMode==1)
	    {
		  //Nazwa aktywnego profilu
		  if(TweakFrmMainTitlebarModeEx==0)
		   SetWindowTextW(hFrmMain,("AQQ [" + ProfileName + "]").w_str());
		  //Nazwa komputera
		  else if(TweakFrmMainTitlebarModeEx==1)
		   SetWindowTextW(hFrmMain,("AQQ [" + ComputerName + "]").w_str());
		  //Nazwa zasobu glownego konta Jabber
		  else
		   SetWindowTextW(hFrmMain,("AQQ [" + ResourceName + "]").w_str());
		}
		//Tryb II
		else if(TweakFrmMainTitlebarMode==2)
		{
		  //Wlasny tekst
		  if(!TweakFrmMainTitlebarText.IsEmpty())
		  {
			//Pusty tekst
			if(TweakFrmMainTitlebarText=="-")
			 SetWindowTextW(hFrmMain,L"");
			//Zdefiniowany tekst
			else
			 SetWindowTextW(hFrmMain,("AQQ " + TweakFrmMainTitlebarText).w_str());
		  }
		  //Brak wersji komunikatora
		  else
		   SetWindowTextW(hFrmMain,L"AQQ");
		}
	  }
	}
    //Zamkniecie okna kontatkow
	if((ClassName=="TfrmMain")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Przypisanie starej procki do okna rozmowy
	  if(OldFrmMainProc)
	  {
		//Przywrocenie wczesniej zapisanej procki
		if(CurrentFrmMainProc==(WNDPROC)GetWindowLongPtr(hFrmMain, GWLP_WNDPROC))
		 SetWindowLongPtrW(hFrmMain, GWLP_WNDPROC,(LONG)OldFrmMainProc);
		//Samo wyrejestrowanie hooka
		else
		{
		  //Przekazanie starej procki przez API
		  TPluginHook PluginHook;
		  PluginHook.HookName = TABKIT_OLDPROC;
		  PluginHook.wParam = (WPARAM)OldFrmMainProc;
		  PluginHook.lParam = (LPARAM)hFrmMain;
		  PluginLink.CallService(AQQ_SYSTEM_SENDHOOK,(WPARAM)(&PluginHook),0);
		  //Wyrejestrowanie hooka
		  SetWindowLongPtrW(hFrmMain, GWLP_WNDPROC,(LONG)(WNDPROC)GetWindowLongPtr(hFrmMain, GWLP_WNDPROC));
		}
		//Skasowanie procek
		OldFrmMainProc = NULL;
		CurrentFrmMainProc = NULL;
	  }
	  //Usuniecie uchwytu do okna kontaktow
	  hFrmMain = NULL;
	}

	//Otwarcie okna rozmowy
	if((ClassName=="TfrmSend")&&(Event==WINDOW_EVENT_CREATE))
	{
	  if(!hFrmSend)
	  {
		//Ustawianie stanu okna
		FrmSendOpening = true;
		//Przypisanie uchwytu do okna rozmowy
		hFrmSend = (HWND)WindowEvent.Handle;
		//Szukanie pola wiadomosci
		if(!hRichEdit) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
		//Szukanie paska informacyjnego
		if(!hStatusBar) SetTimer(hTimerFrm,TIMER_FINDSTATUSBARPRO,500,(TIMERPROC)TimerFrmProc);
		//Szuknie paska narzedzi
		if(!hToolBar) SetTimer(hTimerFrm,TIMER_FINDTOOLBAR,500,(TIMERPROC)TimerFrmProc);
		//Wlaczenie timera szukania paska zakladek + innych pod kontrolek
		if((!hTabsBar)||(!hScrollTabButton[0])||(!hScrollTabButton[1]))
		 SetTimer(hTimerFrm,TIMER_FINDTABSBAR,500,(TIMERPROC)TimerFrmProc);
		//Pobranie rozmiaru+pozycji okna rozmowy
		GetFrmSendRect();
		//Ustawienie poprawnej pozycji okna
		if(FrmSendSlideChk)
		{
		  //Tymczasowa blokada FrmSendSlideOut
		  FrmSendBlockSlide = true;
		  //Wlaczenie timera
		  SetTimer(hTimerFrm,TIMER_FRMSENDCHANGEPOS,2000,(TIMERPROC)TimerFrmProc);
		}
		//Ustawienie statusu okna
		else FrmSendVisible = true;
		//Przypisanie nowej procki dla okna rozmowy
		OldFrmSendProc = (WNDPROC)SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG)FrmSendProc);
		//Pobieranie aktualnej procki okna rozmowy
		CurrentFrmSendProc = (WNDPROC)GetWindowLongPtr(hFrmSend, GWLP_WNDPROC);
		//Tworzenie interfejsu tworzenia okna rozmowy na wierzchu
		BuildStayOnTop();
		//Tworzenie elementu przypinania zakladek
		BuildClipTab();
		if(!RestoringSession)
		{
		  //Szybki dostep niewyslanych wiadomosci
		  DestroyFrmUnsentMsg();
		  BuildFrmUnsentMsg();
		  //Szybki dostep do ostatnio zamknietych zakladek
		  DestroyFrmClosedTabs();
		  BuildFrmClosedTabs();
		}
		//Resetowanie poprzedniego stanu pisania wiadomosci
		LastChatState = 0;
		//Usuniêcie uchwytow do ikonek okna rozmowy
		hIconSmall = NULL;
		hIconBig = NULL;
		//Otwieranie przypietych zakladek
		if((OpenClipTabsChk)&&(!RestoringSession))
		{
		  if(ClipTabsList->Count) SetTimer(hTimerFrm,TIMER_OPENCLIPTABS,2000,(TIMERPROC)TimerFrmProc);
		}
	  }
	}
	//Zamkniecie okna rozmowy
	if((ClassName=="TfrmSend")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Przypisanie starej procki do okna rozmowy
	  if(OldFrmSendProc)
	  {
		//Przywrocenie wczesniej zapisanej procki
		if(CurrentFrmSendProc==(WNDPROC)GetWindowLongPtr(hFrmSend, GWLP_WNDPROC))
		 SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG)OldFrmSendProc);
		//Samo wyrejestrowanie hooka
		else
		{
		  //Przekazanie starej procki przez API
		  TPluginHook PluginHook;
		  PluginHook.HookName = TABKIT_OLDPROC;
		  PluginHook.wParam = (WPARAM)OldFrmSendProc;
		  PluginHook.lParam = (LPARAM)hFrmSend;
		  PluginLink.CallService(AQQ_SYSTEM_SENDHOOK,(WPARAM)(&PluginHook),0);
		  //Wyrejestrowanie hooka
		  SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG)(WNDPROC)GetWindowLongPtr(hFrmSend, GWLP_WNDPROC));
		}
		//Skasowanie procek
		OldFrmSendProc = NULL;
		CurrentFrmSendProc = NULL;
	  }
	  //Ustawienie statusu okna dla SideSlide
	  FrmSendSlideIn = false;
	  FrmSendSlideOut = false;
	  FrmSendVisible = false;
	  FrmSendBlockSlide = true;
	  //Resetowanie zmiennej aktwnie otwartej zakladki
	  ActiveTabJID = "";
	  ActiveTabJIDRes = "";
	  //Szybki dostep niewyslanych wiadomosci
	  DestroyFrmUnsentMsg();
	  BuildFrmUnsentMsg();
	  //Szybki dostep do ostatnio zamknietych zakladek
	  DestroyFrmClosedTabs();
	  BuildFrmClosedTabs();
	  //Usuwanie interfejsu tworzenia okna rozmowy na wierzchu
	  DestroyStayOnTop();
	  //Usuwanie elementu przypinania zakladek
	  DestroyClipTab();
	  //Usuniecie uchwytu do okna rozmowy
	  hFrmSend = NULL;
	}

	//Otwarcie okna ustawien
	if((ClassName=="TfrmSettings")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Tworzenie timera
	  SetTimer(hTimerFrm,TIMER_CHKSETTINGS,500,(TIMERPROC)TimerFrmProc);
	}
	//Zamkniecie okna ustawien
	if((ClassName=="TfrmSettings")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Zatrzymanie timera
	  KillTimer(hTimerFrm,TIMER_CHKSETTINGS);
	  //Sprawdzanie czy zostala wlaczona jest obsluga zakladek
	  TStrings* IniList = new TStringList();
	  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	  Settings->SetStrings(IniList);
	  UnicodeString TabsEnabled = Settings->ReadString("Settings","Tabs","0");
	  if(!StrToBool(TabsEnabled))
	  {
		//Nowe ustawienia
		TSaveSetup SaveSetup;
		SaveSetup.Section = L"Settings";
		SaveSetup.Ident = L"Tabs";
		SaveSetup.Value = L"1";
		//Zapis ustawien
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		//Odswiezenie ustawien
		PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
		//Komunikat informacyjny
		MessageBox(Application->Handle,
		L"Wtyczka TabKit do prawid³owego dzia³ania wymaga w³¹czenia obs³ugi zak³adek w oknie rozmowy!\n"
		L"Obs³uga zak³adek w oknie rozmowy zosta³a automatycznie w³¹czona.",
		L"TabKit - obs³uga zak³adek",
		MB_OK | MB_ICONINFORMATION);
	  }
	  //Sprawdzenie czy zostal zmieniony zasob glownego konta Jabber
	  TPluginStateChange PluginStateChange;
	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);
	  UnicodeString pResourceName = (wchar_t*)PluginStateChange.Resource;
	  if(pResourceName!=ResourceName)
	  {
		ResourceName = pResourceName;
		//Ustawianie tekstu na pasku okna kontaktow
		if(TweakFrmMainTitlebarChk)
		{
		  if((TweakFrmMainTitlebarMode==1)&&(TweakFrmMainTitlebarModeEx==2))
		   SetWindowTextW(hFrmMain,("AQQ [" + ResourceName + "]").w_str());
		}
	  }
	  //Ponowne przypisanie globalnych uchwytow
	  HookGlobalKeyboard();
	}

	//Otworzenie okna emotek
	if((ClassName=="TfrmGraphic")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Zatrzymanie timerow wylaczanie tymczasowej blokady
	  KillTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE);
	  KillTimer(hTimerFrm,TIMER_FRMMAINBLOCKSLIDE);
	  //Ustawienie statusu okna dla SideSlide
	  FrmSendBlockSlide = true;
	  FrmMainBlockSlide = true;
	  FrmSendBlockSlideWndEvent = true;
	  FrmMainBlockSlideWndEvent = true;
	  //Trzymanie okna na wierzchu
	  if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk))
	  {
		SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		SetWindowPos((HWND)(int)WindowEvent.Handle,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  }
	}
	//Zamkniecie okna emotek
	if((ClassName=="TfrmGraphic")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Ustawienie statusu okna dla SideSlide
	  if((FrmSendSlideChk)&&(!StayOnTopStatus)) FrmSendBlockSlide = false;
	  FrmMainBlockSlide = false;
	  FrmSendBlockSlideWndEvent = false;
	  FrmMainBlockSlideWndEvent = false;
	  //Trzymanie okna na wierzchu
	  if(((StayOnTopChk)&&(StayOnTopStatus))||((FrmSendSlideChk)&&(FrmSendSlideHideMode!=2)))
	   SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  //Tworzenie timera emulacji klikniecia myszka w pole tekstowe
	  if((!FrmSetStateExist)&&(hRichEdit))
	   SetTimer(hTimerFrm,TIMER_ACTIVATERICHEDIT,500,(TIMERPROC)TimerFrmProc);
	}

	//Otworzenie okna szybkich emotek
	if((ClassName=="TfrmCompletion")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Zatrzymanie timera wylaczanie tymczasowej blokady
	  KillTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE);
	  //Ustawienie statusu okna dla SideSlide
	  FrmSendBlockSlide = true;
	  FrmSendBlockSlideWndEvent = true;
	  FrmSendBlockSlideOnMsgComposing = true;
	  //Trzymanie okna na wierzchu
	  if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk))
	   SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	//Zamkniecie okna szybkich emotek
	if((ClassName=="TfrmCompletion")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Trzymanie okna na wierzchu
	  if(((StayOnTopChk)&&(StayOnTopStatus))||((FrmSendSlideChk)&&(FrmSendSlideHideMode!=2)))
	   SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  //Gdy kursor znajduje sie poza oknem rozmowy
	  if((Mouse->CursorPos.y<FrmSendRect.Top)||(FrmSendRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmSendRect.Left)||(FrmSendRect.Right<Mouse->CursorPos.x))
	  {
		//Wlaczenie timera wylaczanie tymczasowej blokady
		SetTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE,2000,(TIMERPROC)TimerFrmProc);
	  }
	  //Wylaczenie tymczasowej blokady
	  else
	  {
		FrmSendBlockSlide = false;
		FrmSendBlockSlideWndEvent = false;
	  }
	}

	//Otworzenie okna tworzenia wycinka
	if((ClassName=="TfrmPos")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Zatrzymanie timera wylaczanie tymczasowej blokady
	  KillTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE);
	  //Ustawienie statusu okna rozmowy dla SideSlide
	  FrmSendBlockSlide = true;
	  FrmSendBlockSlideWndEvent = true;
	  //Informacja o otwarciu okna tworzenia wycinka
	  FrmPosExist = true;
	  //Trzymanie okna na wierzchu
	  if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk)||(FrmMainSlideChk))
	  {
		//Normalizacja okna rozmowy/kontaktow
		if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk)) SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		if((FrmMainSlideChk)&&(FrmMainVisible)) SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  }
	  //Timer wylaczenia modalnosci okna wycinka
	  SetTimer(hTimerFrm,TIMER_TURNOFFMODAL,500,(TIMERPROC)TimerFrmProc);
	}
	//Zamkniecie okna tworzenia wycinka
	if((ClassName=="TfrmPos")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Informacja o zamknieciu okna tworzenia wycinka
	  FrmPosExist = false;
	  //Trzymanie okna na wierzchu
	  if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk)||(FrmMainSlideChk))
	  {
		//Ustawienie okna rozmowy/kontaktow na wierzchu
		if(((StayOnTopChk)&&(StayOnTopStatus))||((FrmSendSlideChk)&&(FrmSendSlideHideMode!=2))) SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		if((FrmMainSlideChk)&&(FrmMainVisible)&&(FrmMainSlideHideMode!=2)) SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  }
	  //Tymczasowa blokada FrmSendSlideOut
	  if((FrmSendSlideChk)&&(FrmSendSlideHideMode==3))
	  {
		FrmSendBlockSlide = true;
		//Wlaczenie timera wylaczanie tymczasowej blokady
		SetTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE,2000,(TIMERPROC)TimerFrmProc);
	  }
	  //Ustawienie statusu okna dla SideSlide
	  else
	  {
		if((FrmSendSlideChk)&&(!StayOnTopStatus)) FrmSendBlockSlide = false;
		FrmSendBlockSlideWndEvent = false;
	  }
	}

	//Otworzenie okna Centrum Akcji
	if((ClassName=="TfrmFindAction")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Zatrzymanie timera wylaczanie tymczasowej blokady
	  KillTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE);
	  //Ustawienie statusu okna dla SideSlide
	  FrmSendBlockSlide = true;
	  FrmSendBlockSlideWndEvent = true;
	  //Trzymanie okna na wierzchu
	  if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk))
	  {
		SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		SetWindowPos((HWND)(int)WindowEvent.Handle,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	  }
	}
	//Zamkniecie okna Centrum Akcji
	if((ClassName=="TfrmFindAction")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Ustawienie statusu okna dla SideSlide
	  if((FrmSendSlideChk)&&(!StayOnTopStatus)) FrmSendBlockSlide = false;
	  FrmSendBlockSlideWndEvent = false;
	  //Trzymanie okna na wierzchu
	  if(((StayOnTopChk)&&(StayOnTopStatus))||((FrmSendSlideChk)&&(FrmSendSlideHideMode!=2)))
	   SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}

	//Otworzenie okna wysy³ania obrazka metod¹ Drag&Drop
	if((ClassName=="TfrmDeliveryType")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Zatrzymanie timera wylaczanie tymczasowej blokady
	  KillTimer(hTimerFrm,TIMER_FRMSENDBLOCKSLIDE);
	  //Ustawienie statusu okna dla SideSlide
	  FrmSendBlockSlide = true;
	  FrmSendBlockSlideWndEvent = true;
	  //Trzymanie okna na wierzchu
	  if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk))
	   SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	//Zamkniecie okna wysy³ania obrazka metod¹ Drag&Drop
	if((ClassName=="TfrmDeliveryType")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Ustawienie statusu okna dla SideSlide
	  if((FrmSendSlideChk)&&(!StayOnTopStatus)) FrmSendBlockSlide = false;
	  FrmSendBlockSlideWndEvent = false;
	  //Trzymanie okna na wierzchu
	  if(((StayOnTopChk)&&(StayOnTopStatus))||((FrmSendSlideChk)&&(FrmSendSlideHideMode!=2)))
	   SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}

	//Otworzenie okna zmiany opisu
	if((ClassName=="TfrmSetState")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Zatrzymanie timera wylaczanie tymczasowej blokady
	  KillTimer(hTimerFrm,TIMER_FRMMAINBLOCKSLIDE);
	  //Informacja o otwarciu okna zmiany opisu
	  FrmSetStateExist = true;
	  //Ustawienie statusu okna dla SideSlide
	  FrmMainBlockSlide = true;
	  FrmMainBlockSlideWndEvent = true;
	}
	//Zamkniecie okna zmiany opisu
	if((ClassName=="TfrmSetState")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Informacja o zamknieciu okna zmiany opisu
	  FrmSetStateExist = false;
	  //Ustawienie statusu okna dla SideSlide
	  SetTimer(hTimerFrm,TIMER_FRMMAINBLOCKSLIDE,1500,(TIMERPROC)TimerFrmProc);
	  //Wlaczenie timera ustawienia okna na wierzchu
	  SetTimer(hTimerFrm,TIMER_FRMMAINSETTOPMOST,10,(TIMERPROC)TimerFrmProc);
	}

	//Otworzenie okna wyszukiwarki kontaktow
	if((ClassName=="TfrmSeekOnList")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Zatrzymanie timera wylaczanie tymczasowej blokady
	  KillTimer(hTimerFrm,TIMER_FRMMAINBLOCKSLIDE);
	  //Ustawienie statusu okna dla SideSlide
	  FrmMainBlockSlide = true;
	  FrmMainBlockSlideWndEvent = true;
	  //Zabezpieczenie przed chowaniem okna kontaktow
	  if(FrmMainSlideChk)
	  {
		//Pobieranie uchwytu do okna wyszukiwarki
		hFrmSeekOnList = (HWND)(int)WindowEvent.Handle;
		//Ustawienie okna wyszukiwarki na wierzchu
		SetWindowPos(hFrmSeekOnList,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		//Przypisanie nowej procki dla okna wyszukiwarki
		OldFrmSeekOnListProc = (WNDPROC)SetWindowLongPtrW(hFrmSeekOnList, GWLP_WNDPROC,(LONG)FrmSeekOnListProc);
		//Pobieranie aktualnej procki okna wyszukiwarki
		CurrentFrmSeekOnListProc = (WNDPROC)GetWindowLongPtr(hFrmSeekOnList, GWLP_WNDPROC);
	  }
	}
	//Zamkniecie okna wyszukiwarki kontaktow
	if((ClassName=="TfrmSeekOnList")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Ustawienie statusu okna dla SideSlide
	  FrmMainBlockSlide = false;
	  FrmMainBlockSlideWndEvent = false;
	  //Zabezpieczenie przed chowaniem okna kontaktow
	  if(FrmMainSlideChk)
	  {
		//Przywrocenie wczesniej zapisanej procki
		if(CurrentFrmSeekOnListProc==(WNDPROC)GetWindowLongPtr(hFrmSeekOnList, GWLP_WNDPROC))
		 SetWindowLongPtrW(hFrmSeekOnList, GWLP_WNDPROC,(LONG)OldFrmSeekOnListProc);
		//Samo wyrejestrowanie hooka
		else
		{
          //Przekazanie starej procki przez API
		  TPluginHook PluginHook;
		  PluginHook.HookName = TABKIT_OLDPROC;
		  PluginHook.wParam = (WPARAM)OldFrmSeekOnListProc;
		  PluginHook.lParam = (LPARAM)hFrmSeekOnList;
		  PluginLink.CallService(AQQ_SYSTEM_SENDHOOK,(WPARAM)(&PluginHook),0);
		  //Wyrejestrowanie hooka
		  SetWindowLongPtrW(hFrmSeekOnList, GWLP_WNDPROC,(LONG)(WNDPROC)GetWindowLongPtr(hFrmSeekOnList, GWLP_WNDPROC));
		}
		//Skasowanie procek
		OldFrmSeekOnListProc = NULL;
		CurrentFrmSeekOnListProc = NULL;
		//Zatrzymanie timera
		KillTimer(hTimerFrm,TIMER_FRMMAINSETTOPMOSTEX);
		//Usuniecie uchwytu do okna wyszukiwarki
		hFrmSeekOnList = NULL;
		//Ustawienie okna kontaktow na wierzchu
		if(FrmMainSlideHideMode!=2)
		 SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		else
		 SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	  }
	}

	//Otworzenie okna instalowania dodatku
	if((ClassName=="TfrmInstallAddon")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Informacja o otwarciu okna instalowania dodatku
	  FrmInstallAddonExist = true;
	  //Timer wylaczenia modalnosci okna instalowania dodatkow
	  SetTimer(hTimerFrm,TIMER_TURNOFFMODAL,500,(TIMERPROC)TimerFrmProc);
	}
	//Zamkniecie okna instalowania dodatku
	if((ClassName=="TfrmInstallAddon")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Informacja o zamknieciu okna instalowania dodatku
	  FrmInstallAddonExist = false;
	}

	//Otworzenie okna archiwum
	if((ClassName=="TfrmArch")&&(Event==WINDOW_EVENT_CREATE))
	{
	  //Przypisanie uchwytu do okna archiwum
	  hFrmArch = (HWND)WindowEvent.Handle;
	}
	//Zamkniecie okna archiwum
	if((ClassName=="TfrmArch")&&(Event==WINDOW_EVENT_CLOSE))
	{
	  //Usuniecie uchwytu do okna archiwum
	  hFrmArch = NULL;
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na odbieranie pakietow XML zawierajace ID
int __stdcall OnXMLIDDebug(WPARAM wParam, LPARAM lParam)
{
  if(!ForceUnloadExecuted)
  {
	UnicodeString XML = (wchar_t*)wParam;
	//Jezeli jest to pakiet z lista czatow
	if(XML.Pos("<query xmlns='http://jabber.org/protocol/disco#items'>"))
	{
	  //Przypisanie uchwytu do formy ustawien
	  if(!hSettingsForm)
	  {
		Application->Handle = (HWND)SettingsForm;
		hSettingsForm = new TSettingsForm(Application);
	  }
	  //Przekazanie XMLa na forme
	  hSettingsForm->XML = XML;
	  //Uruchomienie watku
	  hSettingsForm->PrepareXMLThread->Start();
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Odswiezenie wszystkich zakladek
void RefreshTabs()
{
  //Odswiezenie zakladek
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabsW);
  PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
  PluginLink.UnhookEvent(OnFetchAllTabsW);
}
//---------------------------------------------------------------------------

//Pobieranie zdefiniowanego skrotu globalnego do minimalizowania/przywracania okna rozmowy
int GetMinimizeRestoreFrmSendKey()
{
  return MinimizeRestoreKey;
}
//---------------------------------------------------------------------------

//Sprawdzanie czy funkcjonalnosc EmuTabs ma byc dostepna
bool CheckEmuTabsWSupport()
{
  OSVERSIONINFO osvi;
  BOOL bIsWindowsXPorLater;
  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);
  if(osvi.dwMajorVersion==5)
   return true;
  else
   return false;
}
//---------------------------------------------------------------------------

//Zapisywanie zasobów
void ExtractRes(wchar_t* FileName, wchar_t* ResName, wchar_t* ResType)
{
  TPluginTwoFlagParams PluginTwoFlagParams;
  PluginTwoFlagParams.cbSize = sizeof(TPluginTwoFlagParams);
  PluginTwoFlagParams.Param1 = ResName;
  PluginTwoFlagParams.Param2 = ResType;
  PluginTwoFlagParams.Flag1 = (int)HInstance;
  PluginLink.CallService(AQQ_FUNCTION_SAVERESOURCE,(WPARAM)&PluginTwoFlagParams,(LPARAM)FileName);
}
//---------------------------------------------------------------------------

//Obliczanie sumy kontrolnej pliku
UnicodeString MD5File(UnicodeString FileName)
{
  if(FileExists(FileName))
  {
	UnicodeString Result;
    TFileStream *fs;

	fs = new TFileStream(FileName, fmOpenRead | fmShareDenyWrite);
	try
	{
	  TIdHashMessageDigest5 *idmd5= new TIdHashMessageDigest5();
	  try
	  {
	    Result = idmd5->HashStreamAsHex(fs);
	  }
	  __finally
	  {
	    delete idmd5;
	  }
    }
	__finally
    {
	  delete fs;
    }

    return Result;
  }
  else
   return 0;
}
//---------------------------------------------------------------------------

//Odswiezanie wyjatkow aplikacji pelnoekranowych wylaczonych z wysuwania
void RefreshSideSlideExceptions()
{
  SideSlideExceptions->Clear();
  TIniFile *Ini = new TIniFile(SettingsFileDir);
  TStringList *ProcessList = new TStringList;
  Ini->ReadSection("SideSlideExceptions",ProcessList);
  int ProcessListCount = ProcessList->Count;
  delete ProcessList;
  if(ProcessListCount>0)
  {
	for(int Count=0;Count<ProcessListCount;Count++)
	{
	  UnicodeString Process = Ini->ReadString("SideSlideExceptions",("Process"+IntToStr(Count+1)), "");
	  if(!Process.IsEmpty()) SideSlideExceptions->Add(Process.LowerCase());
	}
  }
  delete Ini;
}
//---------------------------------------------------------------------------

//Odczyt ustawien
void LoadSettings()
{
  TIniFile *Ini = new TIniFile(SettingsFileDir);
  //ClosedTabs
  ClosedTabsChk = Ini->ReadBool("ClosedTabs","Enable",true);
  FastAccessClosedTabsChk =  Ini->ReadBool("ClosedTabs","FastAccess",true);
  FrmMainClosedTabsChk =  Ini->ReadBool("ClosedTabs","FrmMain",true);
  FrmSendClosedTabsChk =  Ini->ReadBool("ClosedTabs","FrmSend",true);
  ItemCountUnCloseTabVal = Ini->ReadInteger("ClosedTabs","ItemsCount",5);
  ShowTimeClosedTabsChk = Ini->ReadBool("ClosedTabs","ClosedTime",false);
  FastClearClosedTabsChk = Ini->ReadBool("ClosedTabs","FastClear",true);
  UnCloseTabHotKeyChk =  Ini->ReadBool("ClosedTabs","HotKey",false);
  UnCloseTabHotKeyMode = Ini->ReadInteger("ClosedTabs","HotKeyMode",1);
  UnCloseTabHotKeyDef = Ini->ReadInteger("ClosedTabs","HotKeyDef",0);
  UnCloseTabSPMouseChk = Ini->ReadBool("ClosedTabs","SPMouse",true);
  UnCloseTabLPMouseChk = Ini->ReadBool("ClosedTabs","LPMouse",false);
  CountUnCloseTabVal = Ini->ReadInteger("ClosedTabs","Count",10);
  RestoreLastMsgChk = Ini->ReadBool("ClosedTabs","RestoreLastMsg",false);
  OnlyConversationTabsChk = Ini->ReadBool("ClosedTabs","OnlyConversationTabs",false);
  //UnsentMsg
  UnsentMsgChk = Ini->ReadBool("UnsentMsg","Enable",true);
  InfoUnsentMsgChk = Ini->ReadBool("UnsentMsg","Info",true);
  CloudUnsentMsgChk = Ini->ReadBool("UnsentMsg","Cloud",true);
  DetailedCloudUnsentMsgChk = Ini->ReadBool("UnsentMsg","DetailedCloud",false);
  TrayUnsentMsgChk =  Ini->ReadBool("UnsentMsg","Tray",true);
  FastAccessUnsentMsgChk =  Ini->ReadBool("UnsentMsg","FastAccess",true);
  FrmMainUnsentMsgChk =  Ini->ReadBool("UnsentMsg","FrmMain",true);
  FrmSendUnsentMsgChk =  Ini->ReadBool("UnsentMsg","FrmSend",true);
  FastClearUnsentMsgChk = Ini->ReadBool("UnsentMsg","FastClear",true);
  //TabsSwitching
  SwitchToNewMsgChk =  Ini->ReadBool("TabsSwitching","SwitchToNewMsg",true);
  SwitchToNewMsgMode = Ini->ReadInteger("TabsSwitching","SwitchToNewMsgMode",1);
  TabsHotKeysChk = Ini->ReadBool("TabsSwitching","TabsHotKeys",true);
  TabsHotKeysMode = Ini->ReadInteger("TabsSwitching","TabsHotKeysMode",2);
  NewMgsHoyKeyChk = Ini->ReadBool("TabsSwitching","NewMgsHoyKey",false);
  //SessionRemember
  RestoreTabsSessionChk = Ini->ReadBool("SessionRemember","RestoreTabs",true);
  ManualRestoreTabsSessionChk = Ini->ReadBool("SessionRemember","ManualRestoreTabs",false);
  RestoreMsgSessionChk = Ini->ReadBool("SessionRemember","RestoreMsg",false);
  //NewMsg
  InactiveFrmNewMsgChk = Ini->ReadBool("NewMsg","InactiveFrm",true);
  bool CoreInactiveTabsNewMsgChk = Ini->ReadBool("NewMsg","CoreInactiveTabs",true);
  PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_MSGCOUNTER,CoreInactiveTabsNewMsgChk);
  InactiveTabsNewMsgChk = Ini->ReadBool("NewMsg","InactiveTabs",false);
  if((CoreInactiveTabsNewMsgChk)&&(InactiveTabsNewMsgChk)) PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_MSGCOUNTER,0);
  InactiveNotiferNewMsgChk = Ini->ReadBool("NewMsg","InactiveNotifer",false);
  ChatStateNotiferNewMsgChk = Ini->ReadBool("NewMsg","ChatStateNotifer",true);
  ChatGoneNotiferNewMsgChk = Ini->ReadBool("NewMsg","ChatGoneNotifer",false);
  TaskbarPenChk = Ini->ReadBool("NewMsg","TaskbarPen",true);
  //Titlebar
  TweakFrmSendTitlebarChk = Ini->ReadBool("Titlebar","TweakSend",false);
  if(!LoadExecuted)
  {
    int pTweakFrmSendTitlebarMode = TweakFrmSendTitlebarMode;
    TweakFrmSendTitlebarMode = Ini->ReadInteger("Titlebar","SendMode",1);
	//Usuniecie cache zmienionych tekstow belki okna rozmowy
	if(pTweakFrmSendTitlebarMode!=TweakFrmSendTitlebarMode)
	 ChangedTitlebarList->EraseSection("Titlebar");
  }
  else
   TweakFrmSendTitlebarMode = Ini->ReadInteger("Titlebar","SendMode",1);  
  TweakFrmMainTitlebarChk = Ini->ReadBool("Titlebar","TweakMain",false);
  TweakFrmMainTitlebarMode = Ini->ReadInteger("Titlebar","MainMode",1);
  TweakFrmMainTitlebarModeEx = Ini->ReadInteger("Titlebar","MainModeEx",0);
  TweakFrmMainTitlebarText = Ini->ReadString("Titlebar","MainText","");
  //ClipTabs
  OpenClipTabsChk = Ini->ReadBool("ClipTabs","OpenClipTabs",true);
  InactiveClipTabsChk = Ini->ReadBool("ClipTabs","InactiveClipTabs",false);
  CounterClipTabsChk = Ini->ReadBool("ClipTabs","Counter",false);
  ExClipTabsFromTabSwitchingChk = Ini->ReadBool("ClipTabs","ExcludeFromTabSwitching",false);
  ExClipTabsFromSwitchToNewMsgChk = !Ini->ReadBool("ClipTabs","ExcludeFromSwitchToNewMsg",true);
  ExClipTabsFromTabsHotKeysChk = Ini->ReadBool("ClipTabs","ExcludeFromTabsHotKeys",false);
  MiniAvatarsClipTabsChk = Ini->ReadBool("ClipTabs","MiniAvatars",true);
  //SideSlide
  bool pFrmMainSlideChk = FrmMainSlideChk;
  FrmMainSlideChk = Ini->ReadBool("SideSlide","SlideFrmMain",false);
  int pFrmMainSlideEdge = FrmMainSlideEdge;
  FrmMainSlideEdge = Ini->ReadInteger("SideSlide","FrmMainEdge",2);
  FrmMainSlideHideMode = Ini->ReadInteger("SideSlide","FrmMainHideMode",3);
  FrmMainSlideInDelay = Ini->ReadInteger("SideSlide","FrmMainSlideInDelay",1000);
  FrmMainSlideOutDelay = Ini->ReadInteger("SideSlide","FrmMainSlideOutDelay",1);
  FrmMainSlideInTime = Ini->ReadInteger("SideSlide","FrmMainSlideInTime",300);
  FrmMainSlideOutTime = Ini->ReadInteger("SideSlide","FrmMainSlideOutTime",500);
  FrmMainStepInterval = Ini->ReadInteger("SideSlide","FrmMainStepInterval",30);
  ChangeTabAfterSlideIn = Ini->ReadBool("SideSlide","ChangeTabAfterSlideIn",true);
  if(FrmMainStepInterval==30) Ini->WriteInteger("SideSlide","FrmMainStepInterval",30);
  //Wy³¹czenia opcji uruchamiania komunikatora w postaci zminimalizowanej.
  if(FrmMainSlideChk)
  {
	//Sprawdzanie czy opcja jest wlaczona
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	bool TrayStart = StrToBool(Settings->ReadString("Settings","TrayStart","0"));
	delete Settings;
	if(TrayStart)
	{
	  //Nowe ustawienia
	  TSaveSetup SaveSetup;
	  SaveSetup.Section = L"Settings";
	  SaveSetup.Ident = L"TrayStart";
	  SaveSetup.Value = L"0";
	  //Zapis ustawien
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  //Odswiezenie ustawien
	  PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
	}
  }
  //Pobieranie pozycji paska menu start
  TRect Shell_TrayWndRect;
  GetWindowRect(Shell_TrayWnd,&Shell_TrayWndRect);
  //Ustawianie danych paska menu start dla okna rozmowy
  //Left
  if((FrmMainSlideEdge==1)&&((Shell_TrayWndRect.Height()==Screen->Height)&&(Shell_TrayWndRect.Left==0)))
  {
	FrmMain_Shell_TrayWndLeft = Shell_TrayWndRect.Width();
	FrmMain_Shell_TrayWndRight = 0;
	FrmMain_Shell_TrayWndBottom = 0;
	FrmMain_Shell_TrayWndTop = 0;
  }
  //Right
  else if((FrmMainSlideEdge==2)&&((Shell_TrayWndRect.Height()==Screen->Height)&&(Shell_TrayWndRect.Right==Screen->Width)))
  {
	FrmMain_Shell_TrayWndRight = Shell_TrayWndRect.Width();
	FrmMain_Shell_TrayWndLeft = 0;
	FrmMain_Shell_TrayWndBottom = 0;
	FrmMain_Shell_TrayWndTop = 0;
  }
  //Bottom
  else if((FrmMainSlideEdge==3)&&((Shell_TrayWndRect.Width()==Screen->Width)&&(Shell_TrayWndRect.Bottom==Screen->Height)))
  {
	FrmMain_Shell_TrayWndBottom = Shell_TrayWndRect.Height();
	FrmMain_Shell_TrayWndLeft = 0;
	FrmMain_Shell_TrayWndRight = 0;
	FrmMain_Shell_TrayWndTop = 0;
  }
  //Top
  else if((FrmMainSlideEdge==4)&&((Shell_TrayWndRect.Width()==Screen->Width)&&(Shell_TrayWndRect.Top==0)))
  {
	FrmMain_Shell_TrayWndTop = Shell_TrayWndRect.Height();
	FrmMain_Shell_TrayWndLeft = 0;
	FrmMain_Shell_TrayWndRight = 0;
	FrmMain_Shell_TrayWndBottom = 0;
  }
  //Inne
  else
  {
	FrmMain_Shell_TrayWndLeft = 0;
	FrmMain_Shell_TrayWndRight = 0;
	FrmMain_Shell_TrayWndBottom = 0;
	FrmMain_Shell_TrayWndTop = 0;
  }
  //Zmiana pozycji okna kontaktow
  if(((pFrmMainSlideChk!=FrmMainSlideChk)||(pFrmMainSlideEdge!=FrmMainSlideEdge))&&(!LoadExecuted))
  {
    //Pobranie rozmiaru+pozycji okna kontatkow
	GetFrmMainRect();
	//Blokada SideSlide
	FrmMainBlockSlide = true;
	//Zmiana stanu okna
	SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	//Przywrocenie poczatkowej pozycji okna przy zmianie prostopadlych krawedzi
	if(((pFrmMainSlideEdge==1)||(pFrmMainSlideEdge==2))&&((FrmMainSlideEdge==3)||(FrmMainSlideEdge==4)))
	{
	  //Left
	  if(pFrmMainSlideEdge==1)
	  {
		//Zmienna z informacjami o oknie
		WINDOWINFO pwi;
		GetWindowInfo(hFrmMain,&pwi);
		int RealLeftPos = pwi.rcWindow.left - pwi.rcClient.left;
		SetWindowPos(hFrmMain,HWND_TOP,0+RealLeftPos+FrmMain_Shell_TrayWndLeft,FrmMainRect.Top,0,0,SWP_NOSIZE);
	  }
	  //Right
	  else
	   SetWindowPos(hFrmMain,HWND_TOP,Screen->Width-FrmMainRect.Width()-FrmMain_Shell_TrayWndRight,FrmMainRect.Top,0,0,SWP_NOSIZE);
	  //Pobranie rozmiaru+pozycji okna kontatkow
	  GetFrmMainRect();
	}
	else if(((pFrmMainSlideEdge==3)||(pFrmMainSlideEdge==4))&&((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2)))
	{
	  //Bottom
	  if(FrmMainSlideEdge==3)
	   SetWindowPos(hFrmMain,HWND_TOP,FrmMainRect.Left,Screen->Height-FrmMainRect.Height()-FrmMain_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
	  //Top
	  else
	   SetWindowPos(hFrmMain,HWND_TOP,FrmMainRect.Left,FrmMain_Shell_TrayWndTop,0,0,SWP_NOSIZE);
	  //Pobranie rozmiaru+pozycji okna kontatkow
	  GetFrmMainRect();
	}
	//Ustawienie poprawnej pozycji okna kontaktow
	SetFrmMainPos();
	//Status okna kontatkow
	FrmMainSlideIn = false;
	PreFrmMainSlideOut = false;
	CurPreFrmMainSlideOut = false;
	FrmMainSlideOut = false;
	FrmMainVisible = true;
	FrmMainBlockSlide = false;
	//Pobranie rozmiaru+pozycji okna kontatkow
	GetFrmMainRect();
	//Zapisanie pozycji okna kontaktow do ustawiem AQQ
	TSaveSetup SaveSetup;
	SaveSetup.Section = L"Position";
	SaveSetup.Ident = L"MainLeft";
	SaveSetup.Value = IntToStr((int)FrmMainRect.Left).w_str();
	PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	SaveSetup.Section = L"Position";
	SaveSetup.Ident = L"MainTop";
	SaveSetup.Value = IntToStr((int)FrmMainRect.Top).w_str();
	PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	//Odswiezenie ustawien
	PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
  }
  bool pFrmSendSlideChk = FrmSendSlideChk;
  FrmSendSlideChk = Ini->ReadBool("SideSlide","SlideFrmSend",false);
  int pFrmSendSlideEdge = FrmSendSlideEdge;
  FrmSendSlideEdge = Ini->ReadInteger("SideSlide","FrmSendEdge",1);
  FrmSendSlideHideMode = Ini->ReadInteger("SideSlide","FrmSendHideMode",1);
  FrmSendSlideInDelay = Ini->ReadInteger("SideSlide","FrmSendSlideInDelay",1000);
  FrmSendSlideOutDelay = Ini->ReadInteger("SideSlide","FrmSendSlideOutDelay",1);
  FrmSendSlideInTime = Ini->ReadInteger("SideSlide","FrmSendSlideInTime",300);
  FrmSendSlideOutTime = Ini->ReadInteger("SideSlide","FrmSendSlideOutTime",500);
  FrmSendStepInterval = Ini->ReadInteger("SideSlide","FrmSendStepInterval",30);
  SlideInAtNewMsgChk = Ini->ReadBool("SideSlide","SlideInAtNewMsg",false);
  if(FrmSendStepInterval==30) Ini->WriteInteger("SideSlide","FrmSendStepInterval",30);
  //Wy³¹czenia opcji uruchamiania okna rozmowy w postaci zminimalizowanej po przyjœciu nowej wiadomoœci
  if(FrmSendSlideChk)
  {
	//Sprawdzanie czy opcja jest wlaczona
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	bool AutoMin = StrToBool(Settings->ReadString("Settings","AutoMin","0"));
	delete Settings;
	if(AutoMin)
	{
	  //Nowe ustawienia
	  TSaveSetup SaveSetup;
	  SaveSetup.Section = L"Settings";
	  SaveSetup.Ident = L"AutoMin";
	  SaveSetup.Value = L"0";
	  //Zapis ustawien
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  //Odswiezenie ustawien
	  PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
	}
  }
  //Przywracanie okna na wierzch
  if((hFrmSend)&&(pFrmSendSlideChk!=FrmSendSlideChk))
  {
	//Przywracanie okna rozmowy
	if(IsIconic(hFrmSend))
	{
	  ShowWindow(hFrmSend,SW_RESTORE);
	  BringWindowToTop(hFrmSend);
	  SetForegroundWindow(hFrmSend);
	}
  }
  //Ustawianie danych paska menu start dla okna rozmowy
  //Left
  if((FrmSendSlideEdge==1)&&((Shell_TrayWndRect.Height()==Screen->Height)&&(Shell_TrayWndRect.Left==0)))
  {
	FrmSend_Shell_TrayWndLeft = Shell_TrayWndRect.Width();
	FrmSend_Shell_TrayWndRight = 0;
	FrmSend_Shell_TrayWndBottom = 0;
	FrmSend_Shell_TrayWndTop = 0;
  }
  //Right
  else if((FrmSendSlideEdge==2)&&((Shell_TrayWndRect.Height()==Screen->Height)&&(Shell_TrayWndRect.Right==Screen->Width)))
  {
	FrmSend_Shell_TrayWndRight = Shell_TrayWndRect.Width();
	FrmSend_Shell_TrayWndLeft = 0;
	FrmSend_Shell_TrayWndBottom = 0;
	FrmSend_Shell_TrayWndTop = 0;
  }
  //Bottom
  else if((FrmSendSlideEdge==3)&&((Shell_TrayWndRect.Width()==Screen->Width)&&(Shell_TrayWndRect.Bottom==Screen->Height)))
  {
	FrmSend_Shell_TrayWndBottom = Shell_TrayWndRect.Height();
	FrmSend_Shell_TrayWndLeft = 0;
	FrmSend_Shell_TrayWndRight = 0;
	FrmSend_Shell_TrayWndTop = 0;
  }
  //Top
  else if((FrmSendSlideEdge==4)&&((Shell_TrayWndRect.Width()==Screen->Width)&&(Shell_TrayWndRect.Top==0)))
  {
	FrmSend_Shell_TrayWndTop = Shell_TrayWndRect.Height();
	FrmSend_Shell_TrayWndLeft = 0;
	FrmSend_Shell_TrayWndRight = 0;
	FrmSend_Shell_TrayWndBottom = 0;
  }
  //Inne
  else
  {
	FrmSend_Shell_TrayWndLeft = 0;
	FrmSend_Shell_TrayWndRight = 0;
	FrmSend_Shell_TrayWndBottom = 0;
	FrmSend_Shell_TrayWndTop = 0;
  }
  //Zmiana pozycji okna rozmowy
  if((hFrmSend)&&(!LoadExecuted))
  {
	if((pFrmSendSlideChk!=FrmSendSlideChk)||(pFrmSendSlideEdge!=FrmSendSlideEdge))
	{
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  GetFrmSendRect();
	  //Blokada SideSlide
	  FrmSendBlockSlide = true;
	  //Zmiana stanu okna
	  SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	  //Przywrocenie poczatkowej pozycji okna przy zmianie prostopadlych krawedzi
	  if(((pFrmSendSlideEdge==1)||(pFrmSendSlideEdge==2))&&((FrmSendSlideEdge==3)||(FrmSendSlideEdge==4)))
	  {
		//Left
		if(pFrmSendSlideEdge==1)
		{
		  //Zmienna z informacjami o oknie
		  WINDOWINFO pwi;
		  GetWindowInfo(hFrmSend,&pwi);
		  int RealLeftPos = pwi.rcWindow.left - pwi.rcClient.left;
		  SetWindowPos(hFrmSend,HWND_TOP,0+RealLeftPos+FrmSend_Shell_TrayWndLeft,FrmSendRect.Top,0,0,SWP_NOSIZE);
		}
		//Right
		else
		 SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-FrmSendRect.Width()-FrmSend_Shell_TrayWndRight,FrmSendRect.Top,0,0,SWP_NOSIZE);
		//Pobranie rozmiaru+pozycji okna rozmowy
		GetFrmSendRect();
	  }
	  else if(((pFrmSendSlideEdge==3)||(pFrmSendSlideEdge==4))&&((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2)))
	  {
		//Bottom
		if(pFrmSendSlideEdge==3)
		 SetWindowPos(hFrmSend,HWND_TOP,FrmSendRect.Left,Screen->Height-FrmSendRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
		//Top
		else
		 SetWindowPos(hFrmSend,HWND_TOP,FrmSendRect.Left,FrmSend_Shell_TrayWndTop,0,0,SWP_NOSIZE);
		//Pobranie rozmiaru+pozycji okna rozmowy
		GetFrmSendRect();
	  }
	  //Ustawienie poprawnej pozycji okna rozmowy
	  SetFrmSendPos();
	  //Status okna rozmowy
	  FrmSendSlideIn = false;
	  PreFrmSendSlideOut = false;
	  FrmSendSlideOut = false;
	  FrmSendVisible = true;
	  FrmSendBlockSlide = false;
	  //Pobranie rozmiaru+pozycji okna rozmowy
	  GetFrmSendRect();
	  //Zapisanie pozycji okna rozmowy do ustawiem AQQ
	  TSaveSetup SaveSetup;
	  SaveSetup.Section = L"Position";
	  SaveSetup.Ident = L"MsgLeft";
	  SaveSetup.Value = IntToStr((int)FrmSendRect.Left).w_str();
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  SaveSetup.Section = L"Position";
	  SaveSetup.Ident = L"MsgTop";
	  SaveSetup.Value = IntToStr((int)FrmSendRect.Top).w_str();
	  PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	  //Odswiezenie ustawien
	  PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
	}
  }
  SideSlideFullScreenModeChk = Ini->ReadBool("SideSlide","FullScreenMode",true);
  RefreshSideSlideExceptions();
  SideSlideCtrlAndMouseBlockChk = Ini->ReadBool("SideSlide","CtrlAndMouseBlock",true);
  //Other
  QuickQuoteChk = Ini->ReadBool("Other","QuickQuote",true);
  CollapseImagesChk = Ini->ReadBool("Other","CollapseImages",true);
  CollapseImagesMode = Ini->ReadInteger("Other","CollapseImagesMode",1);
  PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_ANTISPIM_LEN,Ini->ReadBool("Other","AntiSpim",false));
  MinimizeRestoreChk = Ini->ReadBool("Other","MinimizeRestore",false);
  MinimizeRestoreKey = Ini->ReadInteger("Other","MinimizeRestoreHotKey",24689);
  StayOnTopChk = Ini->ReadBool("Other","StayOnTop",false);
  HideToolBarChk = Ini->ReadBool("Other","HideToolBar",false);
  HideTabCloseButtonChk = Ini->ReadBool("Other","HideTabCloseButton",false);
  PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_CLOSEBTN,!HideTabCloseButtonChk);
  HideScrollTabButtonsChk = Ini->ReadBool("Other","HideScrollTabButtons",false);
  CloseBy2xLPMChk = Ini->ReadBool("Other","CloseBy2xLPM",false);
  EmuTabsWChk = Ini->ReadBool("Other","EmuTabsW",false);
  if(!EmuTabsWSupport) EmuTabsWChk = false;
  CloudTimeOut = Ini->ReadInteger("Other","CloudTimeOut",6);
  CloudTickModeChk = Ini->ReadBool("Other","CloudTickMode",true);
  PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_SEARCHONLIST,Ini->ReadBool("Other","SearchOnList",true));
  bool pShortenLinksChk = ShortenLinksChk;
  ShortenLinksChk = Ini->ReadBool("Other","ShortenLinks",true);
  bool pShortenLinksMode = ShortenLinksMode;
  ShortenLinksMode = Ini->ReadInteger("Other","ShortenLinksMode",1);
  //Odswiezenie listy kontaktow
  if(((ShortenLinksChk!=pShortenLinksChk)||(ShortenLinksMode!=pShortenLinksMode))&&((ShortenLinksMode==1)||(ShortenLinksMode==3))&&(!LoadExecuted)) PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aRefresh");
  //Wylaczanie funkcji pisaka na pasku tytu³u okna rozmowy
  if((InactiveFrmNewMsgChk)||(TweakFrmSendTitlebarChk)||(!TaskbarPenChk))
   PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_TASKBARPEN,0);
  else
   PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_TASKBARPEN,1);

  delete Ini;
}
//---------------------------------------------------------------------------

//Zaladowanie wtyczki
extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  //Info o rozpoczeciu procedury ladowania
  LoadExecuted = true;
  //Linkowanie wtyczki z komunikatorem
  PluginLink = *Link;
  //Sprawdzanie czy wlaczona jest obsluga zakladek
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  UnicodeString TabsEnabled = Settings->ReadString("Settings","Tabs","0");
  if(!StrToBool(TabsEnabled))
  {
	//Nowe ustawienia
	TSaveSetup SaveSetup;
	SaveSetup.Section = L"Settings";
	SaveSetup.Ident = L"Tabs";
	SaveSetup.Value = L"1";
	//Zapis ustawien
	PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
	//Odswiezenie ustawien
	PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
  }
  //Sprawdzanie czy uzywany OS to XP
  EmuTabsWSupport = CheckEmuTabsWSupport();
  //Pobranie PID procesu AQQ
  ProcessPID = getpid();
  //Pobranie sciezki do katalogu prywatnego uzytkownika
  PluginUserDir = GetPluginUserDir();
  //Pobranie sciezki do pliku sesji
  SessionFileDir = PluginUserDir + "\\\\TabKit\\\\Session.ini";
  //Pobranie sciezki do pliku ustawien
  SettingsFileDir = PluginUserDir + "\\\\TabKit\\\\Settings.ini";
  //Pobranie sciezki do katalogu ikonek wtyczki w aktywnej kompozycji
  UnicodeString ThemeDir = GetThemeDir() + "\\\\TabKit\\\\";
  //Folder z ustawieniami wtyczki etc
  if(!DirectoryExists(PluginUserDir + "\\\\TabKit"))
   CreateDir(PluginUserDir + "\\\\TabKit");
  if(!DirectoryExists(PluginUserDir + "\\\\TabKit\\\\Avatars"))
   CreateDir(PluginUserDir + "\\\\TabKit\\\\Avatars");
  //Wypakiwanie ikonki ClosedTabsButton.png
  //1298BD768C95081257A09FF8A5590536
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\ClosedTabsButton.png"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\ClosedTabsButton.png").w_str(),L"CLOSEDTABS",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\ClosedTabsButton.png")!="1298BD768C95081257A09FF8A5590536")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\ClosedTabsButton.png").w_str(),L"CLOSEDTABS",L"DATA");
  //Wypakiwanie ikonki UnsentMsg.png
  //8D6126252C378C198BDA6A2F249B2B7C
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\UnsentMsg.png"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\UnsentMsg.png").w_str(),L"UNSENTMSG",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\UnsentMsg.png")!="8D6126252C378C198BDA6A2F249B2B7C")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\UnsentMsg.png").w_str(),L"UNSENTMSG",L"DATA");
  //Wypakiwanie ikonki UnsentMsg.ico
  //B2E390A8457BF6AED7B68240C0034566
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\UnsentMsg.ico"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\UnsentMsg.ico").w_str(),L"UNSENTMSGICO",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\UnsentMsg.ico")!="B2E390A8457BF6AED7B68240C0034566")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\UnsentMsg.ico").w_str(),L"UNSENTMSGICO",L"DATA");
  //Wypakiwanie ikonki Composing.png
  //73BB34FBD1BE53599C470D9A408EF854
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\Composing.png"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Composing.png").w_str(),L"COMPOSING",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\Composing.png")!="73BB34FBD1BE53599C470D9A408EF854")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Composing.png").w_str(),L"COMPOSING",L"DATA");
  //Wypakiwanie ikonki Pause.png
  //756722004F72913D6248D58176EBB788
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\Pause.png"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Pause.png").w_str(),L"PAUSE",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\Pause.png")!="756722004F72913D6248D58176EBB788")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Pause.png").w_str(),L"PAUSE",L"DATA");
  //Wypakiwanie ikonki Gone.png
  //A20977562D3A786D94B8BE92B14BDF20
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\Gone.png"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Gone.png").w_str(),L"GONE",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\Gone.png")!="A20977562D3A786D94B8BE92B14BDF20")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Gone.png").w_str(),L"GONE",L"DATA");
  //Wypakiwanie ikonki Composing_Small.ico
  //986AD6EB91C11CC47833CDC0778523F5
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\Composing_Small.ico"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Composing_Small.ico").w_str(),L"COMPOSING_SMALL",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\Composing_Small.ico")!="986AD6EB91C11CC47833CDC0778523F5")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Composing_Small.ico").w_str(),L"COMPOSING_SMALL",L"DATA");
  //Wypakiwanie ikonki Composing_Big.ico
  //BEC55EC214FB058B97AFE463D15818D4
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\Composing_Big.ico"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Composing_Big.ico").w_str(),L"COMPOSING_BIG",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\Composing_Big.ico")!="BEC55EC214FB058B97AFE463D15818D4")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Composing_Big.ico").w_str(),L"COMPOSING_BIG",L"DATA");
  //Wypakiwanie ikonki Pause_Small.ico
  //46D62DD885A071E17F9BF8CCE101649F
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\Pause_Small.ico"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Pause_Small.ico").w_str(),L"PAUSE_SMALL",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\Pause_Small.ico")!="46D62DD885A071E17F9BF8CCE101649F")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Pause_Small.ico").w_str(),L"PAUSE_SMALL",L"DATA");
  //Wypakiwanie ikonki Pause_Big.ico
  //F1FF8A4FF8DCD54BE7A0A34F4187CF48
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\Pause_Big.ico"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Pause_Big.ico").w_str(),L"PAUSE_BIG",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\Pause_Big.ico")!="F1FF8A4FF8DCD54BE7A0A34F4187CF48")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\Pause_Big.ico").w_str(),L"PAUSE_BIG",L"DATA");
  //Wypakowanie ikonki StayOnTopOff.png
  //439D5BC24FDC9CBB95EC4D4D9B7D5B57
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\StayOnTopOff.png"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\StayOnTopOff.png").w_str(),L"STAYONTOP_OFF",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\StayOnTopOff.png")!="439D5BC24FDC9CBB95EC4D4D9B7D5B57")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\StayOnTopOff.png").w_str(),L"STAYONTOP_OFF",L"DATA");
  //Wypakiwanie ikonki StayOnTopOn.png
  //2B252A5BFB248BE10277CB97DB0AD437
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\StayOnTopOn.png"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\StayOnTopOn.png").w_str(),L"STAYONTOP_ON",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\StayOnTopOn.png")!="2B252A5BFB248BE10277CB97DB0AD437")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\StayOnTopOn.png").w_str(),L"STAYONTOP_ON",L"DATA");
  //Wypakiwanie ikonki TabKit.dll.png
  //992F252A9087DBD036BA98B6D5DE2A08
  if(!DirectoryExists(PluginUserDir + "\\\\Shared"))
   CreateDir(PluginUserDir + "\\\\Shared");
  if(!FileExists(PluginUserDir + "\\\\Shared\\\\TabKit.dll.png"))
   ExtractRes((PluginUserDir + "\\\\Shared\\\\TabKit.dll.png").w_str(),L"PLUGIN_RES",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\Shared\\\\TabKit.dll.png")!="992F252A9087DBD036BA98B6D5DE2A08")
   ExtractRes((PluginUserDir + "\\\\Shared\\\\TabKit.dll.png").w_str(),L"PLUGIN_RES",L"DATA");
  //Wypakiwanie ikonki FastAccess.png
  //1DF8A978FA63D5C1EBEDB23BA94A1C3D
  if(!FileExists(PluginUserDir + "\\\\TabKit\\\\FastAccess.png"))
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\FastAccess.png").w_str(),L"FASTACCESS",L"DATA");
  else if(MD5File(PluginUserDir + "\\\\TabKit\\\\FastAccess.png")!="1DF8A978FA63D5C1EBEDB23BA94A1C3D")
   ExtractRes((PluginUserDir + "\\\\TabKit\\\\FastAccess.png").w_str(),L"FASTACCESS",L"DATA");
  //Przypisanie ikonek do interfejsu AQQ
  //CLOSEDTABS
  if(!FileExists(ThemeDir + "ClosedTabsButton.png"))
   CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\ClosedTabsButton.png").w_str());
  else
   CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "ClosedTabsButton.png").w_str());
  //UNSENTMSG
  if(!FileExists(ThemeDir + "UnsentMsg.png"))
   UNSENTMSG = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\UnsentMsg.png").w_str());
  else
   UNSENTMSG = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "UnsentMsg.png").w_str());
  //COMPOSING
  if(!FileExists(ThemeDir + "Composing.png"))
   COMPOSING = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\Composing.png").w_str());
  else
   COMPOSING = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "Composing.png").w_str());
  //PAUSE
  if(!FileExists(ThemeDir + "Pause.png"))
   PAUSE = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\Pause.png").w_str());
  else
   PAUSE = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "Pause.png").w_str());
  //GONE
  if(!FileExists(ThemeDir + "Gone.png"))
   GONE = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\Gone.png").w_str());
  else
   GONE = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "Gone.png").w_str());
  //STAYONTOP_OFF
  if(!FileExists(ThemeDir + "StayOnTopOff.png"))
   STAYONTOP_OFF = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\StayOnTopOff.png").w_str());
  else
   STAYONTOP_OFF = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "StayOnTopOff.png").w_str());
  //STAYONTOP_ON
  if(!FileExists(ThemeDir + "StayOnTopOn.png"))
   STAYONTOP_ON = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\StayOnTopOn.png").w_str());
  else
   STAYONTOP_ON = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "StayOnTopOn.png").w_str());
  //FASTACCESS
  if(!FileExists(ThemeDir + "FastAccess.png"))
   FASTACCESS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\FastAccess.png").w_str());
  else
   FASTACCESS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "FastAccess.png").w_str());
  //Przypisanie sciezek do ikon
  //Composing_Small
  if(!FileExists(ThemeDir + "Composing_Small.ico"))
   ComposingIconSmall = PluginUserDir + "\\\\TabKit\\\\Composing_Small.ico";
  else
   ComposingIconSmall = ThemeDir + "Composing_Small.ico";
  //Composing_Big
  if(!FileExists(ThemeDir + "Composing_Big.ico"))
   ComposingIconBig = PluginUserDir + "\\\\TabKit\\\\Composing_Big.ico";
  else
   ComposingIconBig = ThemeDir + "Composing_Big.ico";
  //Pause_Small
  if(!FileExists(ThemeDir + "Pause_Small.ico"))
   PauseIconSmall = PluginUserDir + "\\\\TabKit\\\\Pause_Small.ico";
  else
   PauseIconSmall = ThemeDir + "Pause_Small.ico";
  //Pause_Big
  if(!FileExists(ThemeDir + "Pause_Big.ico"))
   PauseIconBig = PluginUserDir + "\\\\TabKit\\\\Pause_Big.ico";
  else
   PauseIconBig = ThemeDir + "Pause_Big.ico";
  //Definiowanie nazwy uzywanego profilu
  ProfileName = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,(WPARAM)0,0);
  if(ProfileName.Pos("\\")>0)
   while(ProfileName.Pos("\\")>0)
	ProfileName.Delete(1,ProfileName.Pos("\\"));
  //Pobieranie zasobu glownego konta Jabber
  TPluginStateChange PluginStateChange;
  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);
  ResourceName = (wchar_t*)PluginStateChange.Resource;
  //Tworzenie PopUpMenu
  //Szybki dostep ostatnio zamknietych zakladek
  TPluginAction ClosedTabsPopUp;
  ZeroMemory(&ClosedTabsPopUp,sizeof(TPluginAction));
  ClosedTabsPopUp.cbSize = sizeof(TPluginAction);
  ClosedTabsPopUp.pszName = L"TabKitClosedTabsPopUp";
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENU,0,(LPARAM)(&ClosedTabsPopUp));
  //Szybki dostep niewyslanych wiadomosci
  TPluginAction UnsentMsgPopUp;
  ZeroMemory(&UnsentMsgPopUp,sizeof(TPluginAction));
  UnsentMsgPopUp.cbSize = sizeof(TPluginAction);
  UnsentMsgPopUp.pszName = L"TabKitUnsentMsgPopUp";
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENU,0,(LPARAM)(&UnsentMsgPopUp));
  //Tworzenie serwisow
  //Szybki dostep ostatnio zamknietych zakladek
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItem0",ServiceClosedTabsItem0);
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItem1",ServiceClosedTabsItem1);
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItem2",ServiceClosedTabsItem2);
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItem3",ServiceClosedTabsItem3);
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItem4",ServiceClosedTabsItem4);
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItem5",ServiceClosedTabsItem5);
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItem6",ServiceClosedTabsItem6);
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItem7",ServiceClosedTabsItem7);
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItem8",ServiceClosedTabsItem8);
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItem9",ServiceClosedTabsItem9);
  PluginLink.CreateServiceFunction(L"sTabKitClosedTabsItemClear",ServiceClosedTabsItemClear);
  //Szybki dostep niewyslanych wiadomosci
  PluginLink.CreateServiceFunction(L"sTabKitUnsentMsgItem0",ServiceUnsentMsgItem0);
  PluginLink.CreateServiceFunction(L"sTabKitUnsentMsgItem1",ServiceUnsentMsgItem1);
  PluginLink.CreateServiceFunction(L"sTabKitUnsentMsgItem2",ServiceUnsentMsgItem2);
  PluginLink.CreateServiceFunction(L"sTabKitUnsentMsgItem3",ServiceUnsentMsgItem3);
  PluginLink.CreateServiceFunction(L"sTabKitUnsentMsgItem4",ServiceUnsentMsgItem4);
  PluginLink.CreateServiceFunction(L"sTabKitUnsentMsgItemClear",ServiceUnsentMsgItemClear);
  PluginLink.CreateServiceFunction(L"sTabKitUnsentMsgShowAllItem",ServiceUnsentMsgShowAllItem);
  //Przypinanie zakladek
  PluginLink.CreateServiceFunction(L"sTabKitClipTabItem",ServiceClipTabItem);
  //Wklejanie tekstu ze schowka jako cytat
  PluginLink.CreateServiceFunction(L"sTabKitQuickQuoteItem",ServiceQuickQuoteItem);
  //Otwieranie zawinietego obrazka w programie graficznym
  PluginLink.CreateServiceFunction(L"sTabKitCollapseImagesItem",ServiceCollapseImagesItem);
  //Trzymanie okna na wierzchu
  PluginLink.CreateServiceFunction(L"sTabKitStayOnTopItem",ServiceStayOnTopItem);
  //Szybki dostep do ustawien wtyczki
  PluginLink.CreateServiceFunction(L"sTabKitFastSettingsItem",ServiceTabKitFastSettingsItem);
  //Hook na aktwyna zakladke lub okno rozmowy
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_ACTIVETAB,OnActiveTab);
  //Hook na pokazywane wiadomosci
  PluginLink.HookEvent(AQQ_CONTACTS_ADDLINE,OnAddLine);
  //Hook na zabezpieczenia komunikatora
  PluginLink.HookEvent(AQQ_SYSTEM_AUTOSECURE_ON,OnAutoSecureOn);
  //Hook na odbezpieczenia komunikatora
  PluginLink.HookEvent(AQQ_SYSTEM_AUTOSECURE_OFF,OnAutoSecureOff);
  //Hook na wylaczenie komunikatora poprzez usera
  PluginLink.HookEvent(AQQ_SYSTEM_BEFOREUNLOAD,OnBeforeUnload);
  //Hook na zamkniecie okna rozmowy lub zakladki
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_CLOSETAB,OnCloseTab);
  //Hook na zamkniecie okna rozmowy lub zakladki wraz z wiadomoscia
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_CLOSETABMESSAGE,OnCloseTabMessage);
  //Hook na zmiane kolorystyki AlphaControls
  PluginLink.HookEvent(AQQ_SYSTEM_COLORCHANGE,OnColorChange);
  //Hook na zmianê stanu kontaktu
  PluginLink.HookEvent(AQQ_CONTACTS_UPDATE,OnContactsUpdate);
  //Hook na zakonczenie ladowania listy kontaktow przy starcie AQQ
  PluginLink.HookEvent(AQQ_CONTACTS_LISTREADY,OnListReady);
  //Hook na wpisywany tekst w oknie rozmowy
  PluginLink.HookEvent(AQQ_SYSTEM_MSGCOMPOSING,OnMsgComposing);
  //Hook na zamkniecie menu kontekstowego pola wiadomosci
  PluginLink.HookEvent(AQQ_SYSTEM_MSGCONTEXT_CLOSE,OnMsgContextClose);
  //Hook na otwarcie menu kontekstowego pola wiadomosci
  PluginLink.HookEvent(AQQ_SYSTEM_MSGCONTEXT_POPUP,OnMsgContextPopup);
  //Hook na pobieranie otwieranie adresow URL i przekazywanie plikow do aplikacji
  PluginLink.HookEvent(AQQ_SYSTEM_PERFORM_COPYDATA,OnPerformCopyData);
  //Hook na wysylanie nowej wiadomosci
  PluginLink.HookEvent(AQQ_CONTACTS_PRESENDMSG,OnPreSendMsg);
  //Hook na odbieranie nowej wiadomosci
  PluginLink.HookEvent(AQQ_CONTACTS_RECVMSG,OnRecvMsg);
  //Hook na odbieranie starej procki przekazanej przez wtyczke AlphaWindows
  PluginLink.HookEvent(ALPHAWINDOWS_OLDPROC,OnRecvOldProc);
  //Hook na enumeracje listy kontatkow
  PluginLink.HookEvent(AQQ_CONTACTS_REPLYLIST,OnReplyList);
  //Hook na restart komunikatora
  PluginLink.HookEvent(AQQ_SYSTEM_RESTART,OnSystemRestart);
  //Hook na zmiane nazwy zasobu przez wtyczke ResourcesChanger
  PluginLink.HookEvent(RESOURCESCHANGER_SYSTEM_RESOURCECHANGED,OnResourceChanged);
  //Hook na zmiane widocznego opisu kontaktu na liscie kontatkow
  PluginLink.HookEvent(AQQ_CONTACTS_SETHTMLSTATUS,OnSetHTMLStatus);
  //Hook na polaczenie sieci przy starcie AQQ
  PluginLink.HookEvent(AQQ_SYSTEM_SETLASTSTATE,OnSetLastState);
  //Hook dla zmiany stanu
  PluginLink.HookEvent(AQQ_SYSTEM_STATECHANGE,OnStateChange);
  //Hook na pokazywanie popumenu
  PluginLink.HookEvent(AQQ_SYSTEM_POPUP,OnSystemPopUp);
  //Hook na zmiane tekstu na zakladce
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
  //Hook na zmiane ikonki na zakladce
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABIMAGE,OnTabImage);
  //Hook na zmiane kompozycji
  PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED, OnThemeChanged);
  //Hook na klikniecie LPM w ikonke tray
  PluginLink.HookEvent(AQQ_SYSTEM_TRAY_CLICK, OnTrayClick);
  //Hook na zamkniecie/otwarcie okien
  PluginLink.HookEvent(AQQ_SYSTEM_WINDOWEVENT,OnWindowEvent);
  //Hook na odbieranie pakietow XML zawierajace ID
  PluginLink.HookEvent(AQQ_SYSTEM_XMLIDDEBUG,OnXMLIDDebug);
  //Hook lokalny na klawiature
  hThreadKeyboard = SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)ThreadKeyboardProc,HInstance,GetCurrentThreadId());
  //Hook lokalny na myszke
  hThreadMouse = SetWindowsHookEx(WH_MOUSE,(HOOKPROC)ThreadMouseProc,HInstance,GetCurrentThreadId());
  //Przypisanie uchwytu do paska menu start
  Shell_TrayWnd = FindWindow(L"Shell_TrayWnd",NULL);
  //Przypisanie uchwytu do systemowego obszaru powiadomien
  hToolbarWindow32 = FindWindowEx(Shell_TrayWnd,NULL,L"TrayNotifyWnd",NULL);
  hToolbarWindow32 = FindWindowEx(hToolbarWindow32,NULL,L"SysPager",NULL);
  hToolbarWindow32 = FindWindowEx(hToolbarWindow32,NULL,L"ToolbarWindow32",NULL);
  //Wczytanie ustawien
  LoadSettings();
  //Hook globalny na klawiature
  HookGlobalKeyboard();
  //Ladowanie przypietych zakladek
  LoadClipTabs();
  //Tworzenie interfejsu szybkiego dostepu do ustawien wtyczki
  BuildTabKitFastSettings();
  //Wszystkie moduly zostaly zaladowane
  if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0))
  {
    //Pobranie ID dla enumeracji kontaktów
	ReplyListID = GetTickCount();
	//Wywolanie enumeracji kontaktow
	PluginLink.CallService(AQQ_CONTACTS_REQUESTLIST,(WPARAM)ReplyListID,0);
	//Szukanie uchwytu do okna kontaktowa
	EnumWindows((WNDENUMPROC)FindFrmMain,0);
	//Szukanie uchwytu do kontrolki IE w oknie kontatkow
	EnumChildWindows(hFrmMain,(WNDENUMPROC)FindFrmMainFocus,0);
	//Pobranie rozmiaru+pozycji okna kontaktow
	GetFrmMainRect();
	//Zmiana pozycji okna kontatkow
	if(FrmMainSlideChk)
	{
	  //Ustawienie poprawnej pozycji okna kontaktow
	  SetFrmMainPos();
	  //Pobranie rozmiaru+pozycji okna kontatkow
	  GetFrmMainRect();
	}
	//Ustawienie statusu okna dla SideSlide
	FrmMainVisible = true;
	FrmMainBlockSlide = false;
	//Przypisanie nowej procki dla okna kontaktow
	OldFrmMainProc = (WNDPROC)SetWindowLongPtrW(hFrmMain, GWLP_WNDPROC,(LONG)FrmMainProc);
	//Pobieranie aktualnej procki okna kontaktow
	CurrentFrmMainProc = (WNDPROC)GetWindowLongPtr(hFrmMain, GWLP_WNDPROC);
	//Pobieranie ostatnio zamknietych zakladek
	GetClosedTabs();
	//Hook na pobieranie aktywnych zakladek
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs);
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_PRIMARYTAB,OnPrimaryTab);
	//Pobieranie aktywnych zakladek
	PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
	//Usuniecie hooka na pobieranie aktywnych zakladek
	PluginLink.UnhookEvent(OnPrimaryTab);
	PluginLink.UnhookEvent(OnFetchAllTabs);
	//Tworzenie interfejsu tworzenia okna rozmowy na wierzchu
	BuildStayOnTop();
	//Tworzenie interfesju dla ostatnio zamknietych zakladek
	BuildFrmClosedTabs();
	//Tworzenie interfesju w AQQ dla ostatnio zamknietych zakladek
	BuildAQQClosedTabs();
	//Sprawdzanie niewyslanych wiadomosci
	GetUnsentMsg();
	//Szybki dostep niewyslanych wiadomosci
	BuildFrmUnsentMsg();
	//Element przypinania zakladek
	BuildClipTab();
	//Brak przycisku zamkniecia i odawiezenie wszystkich zakladek
	if(HideTabCloseButtonChk) RefreshTabs();
	//Pobieranie nazwy komputera
	wchar_t compName[256];
	DWORD len = sizeof(compName);
	GetComputerNameEx(ComputerNameDnsHostname,compName,&len);
	ComputerName = compName;
	//Ustawianie tekstu na pasku okna kontaktow
	if(TweakFrmMainTitlebarChk)
	{
	  //Tryb I
	  if(TweakFrmMainTitlebarMode==1)
	  {
		//Nazwa aktywnego profilu
		if(TweakFrmMainTitlebarModeEx==0)
		 SetWindowTextW(hFrmMain,("AQQ [" + ProfileName + "]").w_str());
		//Nazwa komputera
		else if(TweakFrmMainTitlebarModeEx==1)
		 SetWindowTextW(hFrmMain,("AQQ [" + ComputerName + "]").w_str());
		//Nazwa zasobu glownego konta Jabber
		else
		 SetWindowTextW(hFrmMain,("AQQ [" + ResourceName + "]").w_str());
	  }
	  //Tryb II
	  else if(TweakFrmMainTitlebarMode==2)
	  {
		//Wlasny tekst
		if(!TweakFrmMainTitlebarText.IsEmpty())
		{
		  //Pusty tekst
		  if(TweakFrmMainTitlebarText=="-")
		   SetWindowTextW(hFrmMain,L"");
		  //Zdefiniowany tekst
		  else
		   SetWindowTextW(hFrmMain,("AQQ " + TweakFrmMainTitlebarText).w_str());
		}
		//Brak wersji komunikatora
		else
		 SetWindowTextW(hFrmMain,L"AQQ");
	  }
	}
	//Odswiezenie listy kontaktow - skracanie wyœwietlania odnoœników na liscie kontaktow do wygodniejszej formy
	if((ShortenLinksChk)&&((ShortenLinksMode==1)||(ShortenLinksMode==3))) PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aRefresh");
  }
  //Rejestowanie klasy okna timera
  WNDCLASSEX wincl;
  wincl.cbSize = sizeof (WNDCLASSEX);
  wincl.style = 0;
  wincl.lpfnWndProc = TimerFrmProc;
  wincl.cbClsExtra = 0;
  wincl.cbWndExtra = 0;
  wincl.hInstance = HInstance;
  wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
  wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
  wincl.lpszMenuName = NULL;
  wincl.lpszClassName = L"TTabKitTimer";
  wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  RegisterClassEx(&wincl);
  //Tworzenie okna timera
  hTimerFrm = CreateWindowEx(0, L"TTabKitTimer", L"",	0, 0, 0, 0, 0, NULL, NULL, HInstance, NULL);
  //Timer na sprawdzanie pozycji myszki
  SetTimer(hTimerFrm,TIMER_MOUSEPOSITION,25,(TIMERPROC)TimerFrmProc);
  //Timer na sprawdzanie aktywnego okna
  SetTimer(hTimerFrm,TIMER_CHKACTIVEWINDOW,25,(TIMERPROC)TimerFrmProc);
  //Info o zakonczeniu procedury ladowania
  LoadExecuted = false;
  return 0;
}
//---------------------------------------------------------------------------

//Wyladowanie wtyczki
extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Info o rozpoczeciu procedury wyladowania
  UnloadExecuted = true;
  //Usuniecie lokalnego hooka na klawiature
  if(hThreadKeyboard) UnhookWindowsHookEx(hThreadKeyboard);
  //Usuniecie globalnego hooka na klawiature
  if(hSettingsForm)
  {
	UnregisterHotKey(hSettingsForm->Handle, 0x0100);
	UnregisterHotKey(hSettingsForm->Handle, 0x0200);
  }
  //Usuniecie lokalnego hooka na myszke
  if(hThreadMouse) UnhookWindowsHookEx(hThreadMouse);
  //Przypisanie starej procki do okna rozmowy
  if(OldFrmSendProc)
  {
	//Przywrocenie wczesniej zapisanej procki
	if(CurrentFrmSendProc==(WNDPROC)GetWindowLongPtr(hFrmSend, GWLP_WNDPROC))
	 SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG)OldFrmSendProc);
	//Samo wyrejestrowanie hooka
	else
	{
      //Przekazanie starej procki przez API
	  TPluginHook PluginHook;
	  PluginHook.HookName = TABKIT_OLDPROC;
	  PluginHook.wParam = (WPARAM)OldFrmSendProc;
	  PluginHook.lParam = (LPARAM)hFrmSend;
	  PluginLink.CallService(AQQ_SYSTEM_SENDHOOK,(WPARAM)(&PluginHook),0);
	  //Wyrejestrowanie hooka
	  SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG)(WNDPROC)GetWindowLongPtr(hFrmSend, GWLP_WNDPROC));
	}
	//Skasowanie procek
	OldFrmSendProc = NULL;
	CurrentFrmSendProc = NULL;
  }
  //Przypisanie starej procki do okna kontaktow
  if(OldFrmMainProc)
  {
	//Przywrocenie wczesniej zapisanej procki
	if(CurrentFrmMainProc==(WNDPROC)GetWindowLongPtr(hFrmMain, GWLP_WNDPROC))
	 SetWindowLongPtrW(hFrmMain, GWLP_WNDPROC,(LONG)OldFrmMainProc);
	//Samo wyrejestrowanie hooka
	else
	{
      //Przekazanie starej procki przez API
	  TPluginHook PluginHook;
	  PluginHook.HookName = TABKIT_OLDPROC;
	  PluginHook.wParam = (WPARAM)OldFrmMainProc;
	  PluginHook.lParam = (LPARAM)hFrmMain;
	  PluginLink.CallService(AQQ_SYSTEM_SENDHOOK,(WPARAM)(&PluginHook),0);
	  //Wyrejestrowanie hooka
	  SetWindowLongPtrW(hFrmMain, GWLP_WNDPROC,(LONG)(WNDPROC)GetWindowLongPtr(hFrmMain, GWLP_WNDPROC));
	}
	//Skasowanie procek
	OldFrmMainProc = NULL;
	CurrentFrmMainProc = NULL;
  }
  //Wyladowanie timerow
  for(int TimerID=10;TimerID<=360;TimerID=TimerID+10) KillTimer(hTimerFrm,TimerID);
  //Usuwanie okna timera
  DestroyWindow(hTimerFrm);
  //Wyrejestowanie klasy okna timera
  UnregisterClass(L"TTabKitTimer",HInstance);
  //Usuniecie elementow w interfejsie komunikatora
  if(!ForceUnloadExecuted)
  {
	//Szybki dostep ostatnio zamknietych zakladek
	//Usuwanie interfejsu
	DestroyFrmClosedTabs();
	//Usuwanie serwisow
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItem0);
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItem1);
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItem2);
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItem3);
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItem4);
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItem5);
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItem6);
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItem7);
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItem8);
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItem9);
	PluginLink.DestroyServiceFunction(ServiceClosedTabsItemClear);
	//Usuwanie PopUpMenu
	TPluginAction ClosedTabsPopUp;
	ZeroMemory(&ClosedTabsPopUp,sizeof(TPluginAction));
	ClosedTabsPopUp.cbSize = sizeof(TPluginAction);
	ClosedTabsPopUp.pszName = L"TabKitClosedTabsPopUp";
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU,0,(LPARAM)(&ClosedTabsPopUp));
	//Szybki dostep niewyslanych wiadomosci
	//Usuwanie interwejsu
	DestroyFrmUnsentMsg();
	//Usuwanie serwisow
	PluginLink.DestroyServiceFunction(ServiceUnsentMsgItem0);
	PluginLink.DestroyServiceFunction(ServiceUnsentMsgItem1);
	PluginLink.DestroyServiceFunction(ServiceUnsentMsgItem2);
	PluginLink.DestroyServiceFunction(ServiceUnsentMsgItem3);
	PluginLink.DestroyServiceFunction(ServiceUnsentMsgItem4);
	PluginLink.DestroyServiceFunction(ServiceUnsentMsgItemClear);
	PluginLink.DestroyServiceFunction(ServiceUnsentMsgShowAllItem);
	//Usuwanie PopUpMenu
	TPluginAction UnsentMsgPopUp;
	ZeroMemory(&UnsentMsgPopUp,sizeof(TPluginAction));
	UnsentMsgPopUp.cbSize = sizeof(TPluginAction);
	UnsentMsgPopUp.pszName = L"TabKitUnsentMsgPopUp";
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU ,0,(LPARAM)(&UnsentMsgPopUp));
	//Przypinanie zakladek
	//Usuwanie imterfejsu
	DestroyClipTab();
	//Usuwanie serwisu
	PluginLink.DestroyServiceFunction(ServiceClipTabItem);
	//Trzymanie okna rozmowy na wierzchu
	//Usuwanie imterfejsu
	DestroyStayOnTop();
	//Usuwanie serwisu
	PluginLink.DestroyServiceFunction(ServiceStayOnTopItem);
	//Usuwanie serwisu wstawiania tekstu ze schowka jako cytat
	PluginLink.DestroyServiceFunction(ServiceQuickQuoteItem);
	//Usuwanie seriwisu otwierania zawinietego obrazka w programie graficznym
	PluginLink.DestroyServiceFunction(ServiceCollapseImagesItem);
	//Szybki dostepu do ustawien wtyczki
	//Usuwanie interwejsu
	DestroyTabKitFastSettings();
	//Usuwanie serwisu
	PluginLink.DestroyServiceFunction(ServiceTabKitFastSettingsItem);
  }
  //Wyladowanie wszystkich hookow
  PluginLink.UnhookEvent(OnActiveTab);
  PluginLink.UnhookEvent(OnAddLine);
  PluginLink.UnhookEvent(OnAutoSecureOn);
  PluginLink.UnhookEvent(OnAutoSecureOff);
  PluginLink.UnhookEvent(OnBeforeUnload);
  PluginLink.UnhookEvent(OnCloseTab);
  PluginLink.UnhookEvent(OnCloseTabMessage);
  PluginLink.UnhookEvent(OnColorChange);
  PluginLink.UnhookEvent(OnContactsUpdate);
  PluginLink.UnhookEvent(OnFetchAllTabs);
  PluginLink.UnhookEvent(OnListReady);
  PluginLink.UnhookEvent(OnMsgComposing);
  PluginLink.UnhookEvent(OnMsgContextPopup);
  PluginLink.UnhookEvent(OnMsgContextClose);
  PluginLink.UnhookEvent(OnPreSendMsg);
  PluginLink.UnhookEvent(OnPerformCopyData);
  PluginLink.UnhookEvent(OnPrimaryTab);
  PluginLink.UnhookEvent(OnRecvMsg);
  PluginLink.UnhookEvent(OnRecvOldProc);
  PluginLink.UnhookEvent(OnReplyList);
  PluginLink.UnhookEvent(OnSystemRestart);
  PluginLink.UnhookEvent(OnResourceChanged);
  PluginLink.UnhookEvent(OnSetHTMLStatus);
  PluginLink.UnhookEvent(OnSetLastState);
  PluginLink.UnhookEvent(OnStateChange);
  PluginLink.UnhookEvent(OnSystemPopUp);
  PluginLink.UnhookEvent(OnTabCaption);
  PluginLink.UnhookEvent(OnTabImage);
  PluginLink.UnhookEvent(OnThemeChanged);
  PluginLink.UnhookEvent(OnTrayClick);
  PluginLink.UnhookEvent(OnWindowEvent);
  PluginLink.UnhookEvent(OnXMLIDDebug);
  //Wyladowanie ikonek z intefejsu
  if(!ForceUnloadExecuted)
  {
	PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)CLOSEDTABS);
	PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)UNSENTMSG);
	PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)COMPOSING);
	PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)PAUSE);
	PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)GONE);
	TStringList *ClipIcons = new TStringList;
	ClipTabsIconList->ReadSection("ClipTabsIcon",ClipIcons);
	if(ClipIcons->Count)
	{
	  for(int Count=0;Count<ClipIcons->Count;Count++)
	  {
		UnicodeString JID = ClipIcons->Strings[Count];
		int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID,0);
		if(Icon) PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)Icon);
	  }
	}
	delete ClipIcons;
	PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)STAYONTOP_OFF);
	PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)STAYONTOP_ON);
	PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)FASTACCESS);
  }
  //Przywracanie domyslnych ustawien komunikatora
  if((!ForceUnloadExecuted)&&(!FrmInstallAddonExist))
  {
	//Wlaczanie powiadomienia o pisaniu na pasku przy zminimalizowanym oknie rozmowy
	PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_TASKBARPEN,1);
	//Wlaczenie AntiSpim
	PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_ANTISPIM_LEN,1);
	//Wlaczenie przycisku zamykania zakladek
	PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_CLOSEBTN,1);
	//Wlaczenie graficznego licznika nieprzeczytanych wiadomosci
	PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_MSGCOUNTER,1);
  }
  //Sprawdzanie czy wtyczka ma pamietac sesje
  if((RestoreTabsSessionChk)&&(!ManualRestoreTabsSessionChk))
  {
	TIniFile *Ini = new TIniFile(SessionFileDir);
	Ini->EraseSection("Session");
	delete Ini;
  }
  //Dodawanie aktywnych zakladek do ostatnio zamknietych
  if((ForceUnloadExecuted)&&(TabsList->Count)&&(ClosedTabsChk)&&((!RestoreTabsSessionChk)||((RestoreTabsSessionChk)&&(!ManualRestoreTabsSessionChk))))
  {
	//Dodawanie JID do listy ostatnio zamknietych zakladek
	for(int Count=0;Count<TabsList->Count;Count++)
	{
	  UnicodeString JID = TabsList->Strings[Count];
	  //Sprawdzanie przeprowadzono z kontaktem rozmowe
	  if(OnlyConversationTabsChk)
	  {
		if(AcceptClosedTabsList->IndexOf(JID)==-1)
		 goto SkipClosedTabsChk;
	  }
	  //Sprawdzanie czy kontakt jest czatem z wtyczki
	  if((JID.Pos("@plugin"))&&(JID.Pos("ischat_")))
	   goto SkipClosedTabsChk;
	  //Jezeli kontakt nie znajduje sie na liscie ostatnio zamknietych zakladek
	  if(ClosedTabsList->IndexOf(JID)==-1)
	  {
		//Dodawanie JID do tablicy
		ClosedTabsList->Insert(0,JID);
		TDateTime ClosedTime = TDateTime::CurrentDateTime();
		UnicodeString ClosedTimeStr = ClosedTime.FormatString("ddd d mmm, h:nn");
		ClosedTabsTimeList->Insert(0,ClosedTimeStr);
		//Maks pamietanych X elementów
		if(ClosedTabsList->Count>CountUnCloseTabVal)
		{
		  while(ClosedTabsList->Count>CountUnCloseTabVal)
		  {
			ClosedTabsList->Delete(CountUnCloseTabVal);
			ClosedTabsTimeList->Delete(CountUnCloseTabVal);
		  }
		}
	  }
	  SkipClosedTabsChk: { /* Skip */ }
	}
	//Zapisywanie ostatnio zamknietych zakladek
	TIniFile *Ini = new TIniFile(SessionFileDir);
	Ini->EraseSection("ClosedTabs");
	int TabsCount = ClosedTabsList->Count;
	//Maks X zdefiniowanych elementów
	if(TabsCount>CountUnCloseTabVal) TabsCount = CountUnCloseTabVal;
	if(TabsCount>0)
	{
	  for(int Count=0;Count<TabsCount;Count++)
	  {
		if(!ClosedTabsList->Strings[Count].IsEmpty())
		{
		  Ini->WriteString("ClosedTabs","Tab"+IntToStr(Count+1),ClosedTabsList->Strings[Count]);
		  if(ClosedTabsTimeList->Strings[Count].IsEmpty()) ClosedTabsTimeList->Strings[Count] = "b/d";
		  Ini->WriteString("ClosedTabs","Tab"+IntToStr(Count+1)+"Time",ClosedTabsTimeList->Strings[Count]);
		}
	  }
	}
	delete Ini;
  }
  //Przywracanie oryginalnego tekstu na pasku okna kontaktow
  if((TweakFrmMainTitlebarChk)&&(!ForceUnloadExecuted)&&(!FrmInstallAddonExist))
   SetWindowTextW(hFrmMain,FrmMainTitlebar);
  //Przywracanie oryginalnego tekstu na pasku okna rozmowy
  if((TweakFrmSendTitlebarChk)&&(!ForceUnloadExecuted)&&(!FrmInstallAddonExist))
  {
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_PRIMARYTAB,OnPrimaryTab);
	PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
	PluginLink.UnhookEvent(OnPrimaryTab);
  }
  //Przywracanie przyciskow strzalek do przewijania zakladek
  if(!ForceUnloadExecuted) CheckHideScrollTabButtons();
  //Przywracanie okna rozmowy do "normalnosci"
  if(hFrmSend) SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
  if((hFrmSend)&&(FrmSendSlideChk))
  {
	//Blokowanie SideSlide
	FrmSendBlockSlide = true;
	//Przywracanie okna rozmowy
	if(IsIconic(hFrmSend))
	{
	  ShowWindow(hFrmSend,SW_RESTORE);
	  BringWindowToTop(hFrmSend);
	  SetForegroundWindow(hFrmSend);
	}
	//Ustawienie poprawnej pozycji okna rozmowy
	SetFrmSendPos();
  }
  //Przywracanie paska narzedzi do "normalnosci"
  if((HideToolBarChk)&&(hToolBar)&&(!ForceUnloadExecuted)&&(!FrmInstallAddonExist))
  {
	//Pobieranie pozycji paska narzedzi
	TRect WindowRect;
	GetWindowRect(hToolBar,&WindowRect);
	//Pokazanie paska
	if(!WindowRect.Height())
	 SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),23,SWP_NOMOVE);
	else
	 HideToolBarChk = false;
	//Pobieranie pozycji okna rozmowy
	GetWindowRect(hFrmSend,&WindowRect);
	//Odswiezenie okna rozmowy
	SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
	SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
  }
  //Odswiezenie wszystkich zakladek
  if(((ClipTabsList->Count)||(HideTabCloseButtonChk))&&(!ForceUnloadExecuted)&&(!FrmInstallAddonExist)) RefreshTabs();
  //Przywracanie okna kontatkow do "normalnosci"
  if(hFrmMain) SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
  if((hFrmMain)&&(FrmMainSlideChk))
  {
	//Blokowanie SideSlide
	FrmMainBlockSlide = true;
	//Przywracanie okna kontatow
	if(IsIconic(hFrmMain))
	{
	  ShowWindow(hFrmMain,SW_RESTORE);
	  BringWindowToTop(hFrmMain);
	}
	//Ustawienie poprawnej pozycji okna kontaktow
	SetFrmMainPos();
  }
  //Odswiezenie listy kontaktow - przywrocenie nie skroconych odnosnikow na liscie kontaktow
  if((ShortenLinksChk)&&((ShortenLinksMode==1)||(ShortenLinksMode==3))&&(!ForceUnloadExecuted)&&(!FrmInstallAddonExist))
   PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aRefresh");
  //Info o zakonczeniu procedury wyladowania
  UnloadExecuted = false;
  return 0;
}
//---------------------------------------------------------------------------

//Ustawienia wtyczki
extern "C" int __declspec(dllexport)__stdcall Settings()
{
  //Przypisanie uchwytu do formy ustawien
  if(!hSettingsForm)
  {
	Application->Handle = (HWND)SettingsForm;
	hSettingsForm = new TSettingsForm(Application);
  }
  //Uzupelnienie danych w komponencie niezbednym do funkcji zmiany tekstu na belce okna kontatkow
  hSettingsForm->TweakFrmMainTitlebarModeExComboBox->Items->Clear();
  hSettingsForm->TweakFrmMainTitlebarModeExComboBox->Items->Add(ProfileName);
  hSettingsForm->TweakFrmMainTitlebarModeExComboBox->Items->Add(ComputerName);
  hSettingsForm->TweakFrmMainTitlebarModeExComboBox->Items->Add(ResourceName);
  //Wlaczenie/wylaczanie obslugi EmuTabs na formie ustawien
  if(!EmuTabsWSupport) hSettingsForm->EmuTabsWCheckBox->Enabled = false;
  //Wstawienie wersji wtyczki do formy ustawien
  hSettingsForm->VersionLabel->Caption = "TabKit " + GetFileInfo(GetPluginDir().w_str(), L"FileVersion");
  //Pokaznie okna ustawien
  hSettingsForm->Show();

  return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" PPluginInfo __declspec(dllexport) __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = L"TabKit";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,7,6,0);
  PluginInfo.Description = L"Wtyczka oferuje masê funkcjonalnoœci usprawniaj¹cych korzystanie z komunikatora np. zapamiêtywanie zamkniêtych zak³adek, inteligentne prze³¹czanie, zapamiêtywanie sesji.";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl";
  PluginInfo.Flag = 0;
  PluginInfo.ReplaceDefaultModule = 0;

  return &PluginInfo;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Copyright (C) 2010-2015 Krzysztof Grochocki
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
#include <inifiles.hpp>
#include <process.h>
#include <tlhelp32.h>
#include <XMLDoc.hpp>
#include <DateUtils.hpp>
#include <IdHashMessageDigest.hpp>
#include <boost/regex.hpp>
#include <PluginAPI.h>
#include <LangAPI.hpp>
#pragma hdrstop
#include "SettingsFrm.h"
#include "KeyboardLights.h"
using namespace boost;
using namespace std;
#define RESOURCESCHANGER_SYSTEM_RESOURCECHANGED L"ResourcesChanger/System/ResourceChanged"
#define FLASHER L"TabKitFlasherThread"

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
//JID ostatnio przywroconej zakladki
UnicodeString JustUnClosedJID;
//Zmienne kontaktu do wczytywanie ostatnio przeprowadzonej rozmowy
UnicodeString LoadLastConvJID;
int LoadLastConvUserIdx;
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
TMemIniFile* InactiveTabsNewMsgCount = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
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
HICON hIconSmall = 0;
HICON hIconBig = 0;
//Poprzedni stan pisania wiadomosci
int LastChatState = 0;
//Miganie diodami LED klawiatury
HANDLE hFlasherThread;
HANDLE hFlasherKeyboardThread;
TStringList* UnreadMsgList = new TStringList;
//Titlebar-------------------------------------------------------------------
//Domyslny tekst okna kontaktow
wchar_t FrmMainTitlebar[16];
//Lista zmienionych tekstow belek okna rozmowy
TMemIniFile* ChangedTitlebarList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Tekst belki okna rozmowy
UnicodeString FrmSendTitlebar;
//Nazwa aktywnego profilu
UnicodeString ProfileName;
//Nazwa komputera
UnicodeString ComputerName;
//Zasob glownego konta Jabber
UnicodeString ResourceName;
//ClipTabs-------------------------------------------------------------------
//Lista JID przypietych zakladek
TStringList *ClipTabsList = new TStringList;
TMemIniFile* ClipTabsIconList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Ostatnio aktywna zakladka przed otwieraniem przypietych czatow
UnicodeString ActiveTabBeforeOpenClipTabs;
//JID ostatniej otwartej przypietej zakladki czatowej
UnicodeString LastOpenedChatClipTab;
//FavouritesTabs------------------------------------------------------------
 //Lista JID ulubionych zakladek
TStringList *FavouritesTabsList = new TStringList;
//SideSlide-----------------------------------------------------------------
//Oryginalny rozmiar+pozycja okna kontaktow
TRect FrmMainRect;
WINDOWINFO FrmMainInfo;
int FrmMainRealLeftPos;
int FrmMainRealRightPos;
int FrmMainRealBottomPos;
int FrmMainRealTopPos;
//Pozycja okna kontaktow podczas chowania/pokazywania
int FrmMainSlideLeft;
int FrmMainSlideTop;
//Aktualny status okna kontaktow
bool FrmMainVisible = false;
bool FrmMainBlockSlide = true;
bool FrmMainUnBlockSlide = false;
bool FrmMainDontBlockSlide = false;
bool PreFrmMainSlideIn = false;
bool FrmMainSlideIn = false;
bool PreFrmMainSlideOut = false;
bool FrmMainSlideOut = false;
bool FrmMainSlideOutActiveFrmSend = false;
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
//Pozycja okna rozmowy podczas chowania/pokazywania
int FrmSendSlideLeft;
int FrmSendSlideTop;
//Pozycja kursora w oknie rozmowy
CHARRANGE hRichEditSelPos;
//Aktualny status okna rozmowy
bool FrmSendVisible = false;
bool FrmSendBlockSlide = true;
bool FrmSendUnBlockSlide = false;
bool FrmSendUnBlockSlideEx = true;
bool FrmSendBlockSlideOnMsgComposing = false;
bool FrmSendDontBlockSlide = false;
bool PreFrmSendSlideIn = false;
bool FrmSendSlideIn = false;
bool PreFrmSendSlideOut = false;
bool FrmSendSlideOut = false;
bool FrmCompletionExists = false;
bool FrmSendShownByThumbnail = false;
bool FrmSendShownByThumbnailTimer = false;
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
HWND LastActiveWindow_WmInactiveFrmSendSlide;
HWND LastActiveWindow_PreFrmSendSlideIn;
HWND LastActiveWindow_WmInactiveFrmMainSlide;
HWND LastActiveWindow_PreFrmMainSlideIn;
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
UnicodeString AttachmentStyle;
UnicodeString CollapseImagesItemURL;
TStringList *CollapseImagesList = new TStringList;
//Status funkcji StayOnTop
bool StayOnTopStatus = false;
//Pokazywanie paska narzedzi
bool ToolBarShowing = false;
//Zamykanie zakladki poprzez 2xLPM
bool LBUTTONDBLCLK_EXECUTED = false;
//Zmiana miejsca zakladek
bool LBUTTONDOWN_ON_TABSBAR_EXECUTED = false;
int CursorPosX, CursorPosY;
//Blokowanie lokalnego hooka na myszke
bool BlockThreadMouseProc = false;
//Blokowanie lokalnego hooka na klawiature
bool BlockThreadKeyboardProc = false;
//INNE-----------------------------------------------------------------------
//Aktywna otwarta zakladka
UnicodeString ActiveTabJID;
UnicodeString ActiveTabJIDEx;
//Ostatnio aktywna otwarta zakladka
UnicodeString LastActiveTabJID;
//JID kontaktu z menu popTab
UnicodeString PopupTab;
UnicodeString PopupTabEx;
//Lista JID z notyfikacjami wiadomosci
TStringList *PreMsgList = new TStringList;
TMemIniFile* PreMsgStateList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Lista JID otwartych zakladek
TStringList *TabsList = new TStringList;
TStringList *TabsListEx = new TStringList;
//Lista JID wraz ze stanami
TMemIniFile* ContactsStateList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Lista JID wraz z nickami
TMemIniFile* ContactsNickList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Lista ID filmow YouTube do przetworzenia
TStringList *GetYouTubeTitleList = new TStringList;
//Lista ID filmow YouTube wykluczonych na czas sesji
TStringList *YouTubeExcludeList = new TStringList;
//Zezwolenie na odswiezenie listy kontaktow
bool AllowRefreshList = false;
//Tablica ID pakietow XML
TStringList *XMLIDList = new TStringList;
//Sciezka do pliku sesji
UnicodeString SessionFileDir;
//Sciezka do pliku ustawien
UnicodeString SettingsFileDir;
//Sciezka do katalogu prywatnego wtyczek
UnicodeString PluginUserDir;
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
//Informacja o widocznym oknie dolaczania do konferencji
bool FrmChatJoinExist = false;
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
//Lokalny hook na klawiature
HHOOK hThreadKeyboard;
//Lokalny hook na myszke
HHOOK hThreadMouse;
//Stara procka okna rozmowy
WNDPROC OldFrmSendProc;
//Stara procka okna kontaktow
WNDPROC OldFrmMainProc;
//Stara procka okna wyszukiwarki
WNDPROC OldFrmSeekOnListProc;
//IKONY-W-INTERFEJSIE--------------------------------------------------------
int CLOSEDTABS;
int UNSENTMSG;
int COMPOSING;
int PAUSE;
int GONE;
int STAYONTOP_OFF;
int STAYONTOP_ON;
int FASTACCESS;
//int BLABLER;
//TIMERY---------------------------------------------------------------------
#define TIMER_FIND_STATUSBAR 10
#define TIMER_FIND_TOOLBAR 20
#define TIMER_SHOW_TOOLBAR 30
#define TIMER_FIND_TABSBAR 40
#define TIMER_STAYONTOP 50
#define TIMER_DISABLE_MODAL 60
#define TIMER_UNBLOCK_INACTIVENOTIFER 70
#define TIMER_RESTORE_SESSION 80
#define TIMER_LOADLASTCONV 90
#define TIMER_CLIPTABS_MOVE 100
#define TIMER_CLIPTABS_OPEN 110
#define TIMER_CHKSETTINGS 120
#define TIMER_UNCLOSEBY2XLPM 130
#define TIMER_CLOSEBY2XLPM 140
#define TIMER_REBUILD_TABS_LIST 150
#define TIMER_MOUSE_POSITION 160
#define TIMER_UNBLOCK_MOUSE_PROC 170
#define TIMER_EXSETSEL 380
#define TIMER_ACTIVE_WINDOW 180
#define TIMER_FRMSEND_PRE_SLIDEOUT 190
#define TIMER_FRMSEND_SLIDEOUT 200
#define TIMER_FRMSEND_PRE_SLIDEIN 210
#define TIMER_FRMSEND_SLIDEIN 220
#define TIMER_FRMSEND_UNBLOCK_SLIDE 230
#define TIMER_FRMSEND_MINIMIZED 240
#define TIMER_FRMSEND_CHANGEPOS 250
#define TIMER_FRMSEND_TOPMOST 260
#define TIMER_FRMSEND_TOPMOST_AND_SLIDEOUT 270
#define TIMER_FRMSEND_FOCUS_RICHEDIT 280
#define TIMER_FRMSEND_UNBLOCK_THUMBNAIL 290
#define TIMER_FRMMAIN_PRE_SLIDEOUT 300
#define TIMER_FRMMAIN_SLIDEOUT 310
#define TIMER_FRMMAIN_PRE_SLIDEIN 320
#define TIMER_FRMMAIN_SLIDEIN 330
#define TIMER_FRMMAIN_UNBLOCK_SLIDE 340
#define TIMER_FRMMAIN_TOPMOST 350
#define TIMER_FRMMAIN_TOPMOST_EX 360
#define TIMER_FRMMAIN_TOPMOST_AND_SLIDEOUT 370
//SETTINGS-------------------------------------------------------------------
//ClosedTabs
bool ClosedTabsChk;
bool FastAccessClosedTabsChk;
bool FrmMainClosedTabsChk;
bool FrmSendClosedTabsChk;
int ItemCountUnCloseTabVal;
bool ShowTimeClosedTabsChk;
UnicodeString DateFormatOnClosedTabs;
bool FastClearClosedTabsChk;
bool UnCloseTabHotKeyChk;
int UnCloseTabHotKeyMode;
int UnCloseTabHotKeyDef;
bool UnCloseTabSPMouseChk;
bool UnCloseTabLPMouseChk;
bool UnCloseTab2xLPMouseChk;
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
bool KeyboardFlasherChk;
int KeyboardFlasherModeChk;
bool InactiveTabsNewMsgChk;
bool InactiveNotiferNewMsgChk;
bool ChatStateNotiferNewMsgChk;
bool ChatGoneNotiferNewMsgChk;
bool ChatGoneCloudNotiferNewMsgChk;
bool ChatGoneSoundNotiferNewMsgChk;
bool TaskbarPenChk;
//Titlebar
bool TweakFrmSendTitlebarChk;
int TweakFrmSendTitlebarMode = 0;
bool TweakFrmMainTitlebarChk;
int TweakFrmMainTitlebarMode;
int TweakFrmMainTitlebarModeEx;
UnicodeString TweakFrmMainTitlebarText;
//ClipTabs
bool ClipTabsChk;
bool OpenClipTabsChk;
bool InactiveClipTabsChk;
bool CounterClipTabsChk;
bool ExClipTabsFromTabSwitchingChk;
bool ExClipTabsFromSwitchToNewMsgChk;
bool ExClipTabsFromTabsHotKeysChk;
bool MiniAvatarsClipTabsChk;
//FavouritesTabs
bool FavouritesTabsChk;
bool FastAccessFavouritesTabsChk;
bool FrmMainFastAccessFavouritesTabsChk;
bool FrmSendFastAccessFavouritesTabsChk;
bool FavouritesTabsHotKeysChk;
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
int CloudTimeOut;
bool CloudTickModeChk;
bool ShortenLinksChk = true;
int ShortenLinksMode;
//FORWARD-AQQ-HOOKS----------------------------------------------------------
INT_PTR __stdcall OnActiveTab(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnAddLine(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnAutoSecureOn(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnAutoSecureOff(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnBeforeUnload(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnCloseTab(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnCloseTabMessage(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnColorChange(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnContactsUpdate(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnFetchAllTabs(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnFetchAllTabs_GetOnlyList(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnFetchAllTabs_RefreshTabs(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnLangCodeChanged(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnListReady(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnMsgComposing(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnMsgContextClose(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnMsgContextPopup(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnNotificationClosed(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnPerformCopyData(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnPreSendMsg(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnPrimaryTab(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnPrimaryTab_GetOnlyList(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnRecvMsg(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnReplyList(WPARAM wParam, LPARAM lParam);
//INT_PTR __stdcall OnSystemRestart(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnResourceChanged(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnSetHTMLStatus(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnSetLastState(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnStateChange(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnSystemPopUp(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnTabCaption(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnTabImage(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnTrayClick(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnWindowEvent(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnXMLIDDebug(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItem0(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItem1(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItem2(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItem3(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItem4(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItem5(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItem6(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItem7(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItem8(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItem9(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClosedTabsItemClear(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceUnsentMsgItem0(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceUnsentMsgItem1(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceUnsentMsgItem2(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceUnsentMsgItem3(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceUnsentMsgItem4(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceUnsentMsgItemClear(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceUnsentMsgShowAllItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClipTabItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceClipTabCaptionItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouriteTabItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouritesTabsItem0(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouritesTabsItem1(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouritesTabsItem2(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouritesTabsItem3(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouritesTabsItem4(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouritesTabsItem5(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouritesTabsItem6(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouritesTabsItem7(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouritesTabsItem8(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceFavouritesTabsItem9(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceQuickQuoteItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceCollapseImagesItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceStayOnTopItem(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceTabKitFastSettingsItem(WPARAM wParam, LPARAM lParam);
//FORWARD-TIMER--------------------------------------------------------------
LRESULT CALLBACK TimerFrmProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//FORWARD-WINDOW-PROC--------------------------------------------------------
LRESULT CALLBACK FrmMainProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK FrmSendProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//FORWARD-OTHER-FUNCTION-----------------------------------------------------
void CheckHideScrollTabButtons();
void OpenNewTab(UnicodeString JID);
UnicodeString GetChannelName(UnicodeString JID);
void DestroyFrmUnsentMsg();
void BuildFrmUnsentMsg(bool FixPosition);
void DestroyFrmClosedTabs();
void BuildFrmClosedTabs(bool FixPosition);
void DestroyFavouritesTabs();
void BuildFavouritesTabs(bool FixPosition);
void DestroyStayOnTop();
void BuildStayOnTop();
UnicodeString MD5(UnicodeString Text);
void RefreshTabs();
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

//Pobieranie sciezki katalogu profilu
UnicodeString GetUserDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu aplikacji
UnicodeString GetApplicationPath()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETAPPPATH,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki kompozycji
UnicodeString GetThemeDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki skorki kompozycji
UnicodeString GetThemeSkinDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
}
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu zawierajacego informacje o kontaktach
UnicodeString GetContactsUserDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Data\\\\Contacts\\\\";
}
//---------------------------------------------------------------------------

//Pobieranie sciezki pliku DLL wtyczki
UnicodeString GetPluginDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINDIR,(WPARAM)(HInstance),0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki ikony z interfejsu AQQ
UnicodeString GetIconPath(int Icon)
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,Icon,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//--------------------------------------------------------------------------

//Sprawdzanie czy dzwieki w AQQ sa wlaczone
bool ChkSoundEnabled()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString SoundOff = Settings->ReadString("Sound","SoundOff","0");
	delete Settings;
	return !StrToBool(SoundOff);
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczona jest zaawansowana stylizacja okien
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

//Pobieranie ustawien animacji AlphaControls
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
int GetBrightness()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETBRIGHTNESS,0,0);
}
//---------------------------------------------------------------------------

//Pobieranie informacji o pliku
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

	if(ModulePath==NULL) GetModuleFileName(NULL, sFileName, 1024);
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

//Pobieranie sciezki procesu wskazanego okna
UnicodeString GetPathOfProces(HWND hWnd)
{
	//Pobieranie PID procesu wskazanego okna
	DWORD PID;
	GetWindowThreadProcessId(hWnd, &PID);
	//Pobieranie sciezki procesu
	if(PID)
	{
		MODULEENTRY32 lpModuleEntry = {0};
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
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

//Pobranie stylu zalacznika
void GetAttachmentStyle()
{
	//URL do aktuanie uzywanej kompozycji
	UnicodeString ThemeURL = GetThemeDir();
	//URL do domyslnej kompozycji
	UnicodeString ThemeURLW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETAPPPATH,0,0));
	ThemeURLW = StringReplace(ThemeURLW, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
	ThemeURLW = ThemeURLW + "\\\\System\\\\Shared\\\\Themes\\\\Standard";
	//Przypisanie uchwytu do formy
	Application->Handle = (HWND)SettingsForm;
	TSettingsForm *hModalSettingsForm = new TSettingsForm(Application);
	//Pobieranie stylu zalacznika
	if(FileExists(ThemeURL + "\\\\Message\\\\Attachment.htm"))
		hModalSettingsForm->FileMemo->Lines->LoadFromFile(ThemeURL + "\\\\Message\\\\Attachment.htm");
	else
		hModalSettingsForm->FileMemo->Lines->LoadFromFile(ThemeURLW + "\\\\Message\\\\Attachment.htm");
	//Wyciaganie wlasciwego stylu
	int LinesCount = hModalSettingsForm->FileMemo->Lines->Count;
	for(int Count=0;Count<LinesCount;Count++)
	{
		if(hModalSettingsForm->FileMemo->Lines->Strings[Count].Pos("CC_ATTACH_ICON"))
		{
			//Pobranie stylu zalacznika
			AttachmentStyle = hModalSettingsForm->FileMemo->Lines->Strings[Count];
			AttachmentStyle = AttachmentStyle.Trim();
			//Zakonczenie petli
			Count = LinesCount;
		}
	}
	//Brak wczytanego stylu z kompozycji - styl domyslny
	if(AttachmentStyle.IsEmpty())
		AttachmentStyle = "<SPAN class=\"attach_icon\">CC_ATTACH_ICON</SPAN><SPAN id=\"attach_caption\">CC_ATTACH_CAPTION</SPAN>: <SPAN class=\"attach_short\">CC_ATTACH_SHORT</SPAN>";
	//Usuwanie zbednego formatowana ze stylu
	else if(AttachmentStyle.LowerCase().Pos("<div")==1)
	{
		AttachmentStyle.Delete(1,AttachmentStyle.Pos(">"));
		AttachmentStyle.Delete(AttachmentStyle.LowerCase().Pos("</div>"),AttachmentStyle.Length());
		AttachmentStyle = AttachmentStyle.Trim();
	}
	//Usuniecie uchwytu do formy
	delete hModalSettingsForm;
}
//---------------------------------------------------------------------------

//Otwieranie okna ustawien wtyczki
void OpenPluginSettings()
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
	//Wstawienie wersji wtyczki do formy ustawien
	hSettingsForm->VersionLabel->Caption = "TabKit " + GetFileInfo(GetPluginDir().w_str(), L"FileVersion");
	//Pokaznie okna ustawien
	hSettingsForm->Show();
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do okna kontaktow
bool CALLBACK FindFrmMain(HWND hWnd, LPARAM lParam)
{
	//Pobranie klasy okna
	wchar_t WindowClassNameW[128];
	GetClassNameW(hWnd, WindowClassNameW, sizeof(WindowClassNameW));
	UnicodeString WindowClassName = WindowClassNameW;
	//Sprawdenie klasy okna
	if(WindowClassName=="TfrmMain")
	{
		//Pobranie PID procesu wskazanego okna
		DWORD PID;
		GetWindowThreadProcessId(hWnd, &PID);
		//Porownanie PID okna
		if(PID==ProcessPID)
		{
			//Przypisanie uchwytu
			hFrmMain = hWnd;
			//Pobieranie oryginalnego titlebar okna
			GetWindowTextW(hFrmMain,FrmMainTitlebar,sizeof(FrmMainTitlebar));
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do kontrolki IE w oknie kontatkow
bool CALLBACK FindFrmMainFocus(HWND hWnd, LPARAM lParam)
{
	//Pobranie klasy okna
	wchar_t WindowClassNameW[128];
	GetClassNameW(hWnd, WindowClassNameW, sizeof(WindowClassNameW));
	UnicodeString WindowClassName = WindowClassNameW;
	//Sprawdzenie klasy okna
	if(WindowClassName=="Internet Explorer_Server")
	{
		//Pobieranie uchwytu do okna rodzica
		HWND hWndEx = FindWindowEx(NULL,hWnd,L"Shell DocObject View",NULL);
		if(!hWndEx)
		{
			//Pobieranie uchwytu do okna rodzica
			hWndEx = FindWindowEx(NULL,hWndEx,L"Shell Embedding",NULL);
			if(!hWndEx)
			{
				//Pobieranie uchwytu do okna rodzica
				hWndEx = FindWindowEx(NULL,hWndEx,L"TsPanel",L"ListaPanel");
				if(!hWndEx)
				{
					//Przypisanie uchwytu
					hFrmMainFocus = hWnd;
					return false;
				}
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do kontrolki TsRichEdit
bool CALLBACK FindRichEdit(HWND hWnd, LPARAM lParam)
{
	//Pobranie klasy okna
	wchar_t WindowClassNameW[128];
	GetClassNameW(hWnd, WindowClassNameW, sizeof(WindowClassNameW));
	UnicodeString WindowClassName = WindowClassNameW;
	//Sprawdzenie klasy okna
	if(WindowClassName=="TsRichEdit")
	{
		//Przypisanie uchwytu
		hRichEdit = hWnd;
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do kontrolki TToolBar (pasek narzedzi)
bool CALLBACK FindToolBar(HWND hWnd, LPARAM lParam)
{
	//Pobranie klasy okna
	wchar_t WindowClassNameW[128];
	GetClassNameW(hWnd, WindowClassNameW, sizeof(WindowClassNameW));
	UnicodeString WindowClassName = WindowClassNameW;
	//Sprawdzenie klasy okna
	if(WindowClassName=="TToolBar")
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
bool CALLBACK FindTabsBarEx(HWND hWnd, LPARAM lParam)
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
//-przyciskow strzalek do przewijania zakladek (part I)
bool CALLBACK FindTabsBar(HWND hWnd, LPARAM lParam)
{
	//Pobranie klasy okna
	wchar_t WindowClassNameW[128];
	GetClassNameW(hWnd, WindowClassNameW, sizeof(WindowClassNameW));
	UnicodeString WindowClassName = WindowClassNameW;
	//Sprawdenie klasy okna
	if(WindowClassName=="TsPanel")
	{
		//Szukanie paska narzedzi
		HWND hTempHwnd = FindWindowEx(hWnd,NULL,L"TToolBar",NULL);
		//Pasek narzedzi znaleziony
		if(hTempHwnd)
		{
			//Pobranie wlasciwego okna dziecka
			hTempHwnd = FindWindowEx(hWnd,NULL,L"TPanel",NULL);
			//Uruchomienie drugiej czesci szukania uchwytow
			EnumChildWindows(hTempHwnd,(WNDENUMPROC)FindTabsBarEx,0);
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------

//Aktywacja okna kontaktow + nadanie fokusa kontrolce IE
void ActivateAndFocusFrmMain()
{
	//Aktywacja okna
	SetForegroundWindow(hFrmMain);
	SetActiveWindow(hFrmMain);
	SetFocus(hFrmMain);
	//Ustawienie fokusa
	SetFocus(hFrmMainFocus);
}
//---------------------------------------------------------------------------

//Aktywacja okna kontaktow + ustawienie okna na wierzchu
void ActivateAndSetTopmostFrmMain()
{
	//Aktywacja okna
	SetForegroundWindow(hFrmMain);
	SetActiveWindow(hFrmMain);
	SetFocus(hFrmMain);
	//Ustawienie okna na wierzchu
	SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
}
//---------------------------------------------------------------------------

//Pobranie rozmiaru + pozycji okna kontatkow
void GetFrmMainRect()
{
	//Pobranie rozmiaru + pozycji okna kontaktow
	GetWindowRect(hFrmMain,&FrmMainRect);
	//Pobranie realnego rozmiaru + pozycji oka kontaktow
	GetWindowInfo(hFrmMain,&FrmMainInfo);
	//Okno kontaktow chowane za lewa krawedzia ekranu
	if(FrmMainSlideEdge==1)
	{
		FrmMainRealLeftPos = FrmMainInfo.rcWindow.left - FrmMainInfo.rcClient.left;
		FrmMainRealRightPos = 0;
		FrmMainRealBottomPos = 0;
		FrmMainRealTopPos = 0;
	}
	//Okno kontaktow chowane za prawa krawedzia ekranu
	else if(FrmMainSlideEdge==2)
	{
		FrmMainRealRightPos = FrmMainInfo.rcWindow.right - FrmMainInfo.rcClient.right;
		FrmMainRealLeftPos = 0;
		FrmMainRealBottomPos = 0;
		FrmMainRealTopPos = 0;
	}
	//Okno kontaktow chowane za dolna krawedzia ekranu
	else if(FrmMainSlideEdge==3)
	{
		FrmMainRealBottomPos = FrmMainInfo.rcWindow.bottom - FrmMainInfo.rcClient.bottom;
		FrmMainRealLeftPos = 0;
		FrmMainRealRightPos = 0;
		FrmMainRealTopPos = 0;
	}
	//Okno kontaktow chowane za gorna krawedzia ekranu
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
	//Okno kontaktow chowane za lewa krawedzia ekranu
	if(FrmMainSlideEdge==1)
		SetWindowPos(hFrmMain,HWND_TOP,0+FrmMain_Shell_TrayWndLeft,FrmMainRect.Top,0,0,SWP_NOSIZE);
	//Okno kontaktow chowane za prawa krawedzia ekranu
	else if(FrmMainSlideEdge==2)
		SetWindowPos(hFrmMain,HWND_TOP,Screen->Width-FrmMainRect.Width()-FrmMain_Shell_TrayWndRight,FrmMainRect.Top,0,0,SWP_NOSIZE);
	//Okno kontaktow chowane za gorna krawedzia ekranu
	else if(FrmMainSlideEdge==3)
		SetWindowPos(hFrmMain,HWND_TOP,FrmMainRect.Left,Screen->Height-FrmMainRect.Height()-FrmMain_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
	//Okno kontaktow chowane za dolna krawedzia ekranu
	else
		SetWindowPos(hFrmMain,HWND_TOP,FrmMainRect.Left,FrmMain_Shell_TrayWndTop,0,0,SWP_NOSIZE);
}
//---------------------------------------------------------------------------

//Sztywne wysuniecie okna kontaktow zza krawedzi
void ShowFrmMain()
{
	//Okno kontaktow chowane za lewa krawedzia ekranu
	if(FrmMainSlideEdge==1)
		SetWindowPos(hFrmMain,HWND_BOTTOM,0+FrmMain_Shell_TrayWndLeft,FrmMainRect.Top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	//Okno kontaktow chowane za prawa krawedzia ekranu
	else if(FrmMainSlideEdge==2)
		SetWindowPos(hFrmMain,HWND_BOTTOM,Screen->Width-FrmMainRect.Width()-FrmMain_Shell_TrayWndRight,FrmMainRect.Top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	//Okno kontaktow chowane za dolna krawedzia ekranu
	else if(FrmMainSlideEdge==3)
		SetWindowPos(hFrmMain,HWND_BOTTOM,FrmMainRect.Left,Screen->Height-FrmMainRect.Height()-FrmMain_Shell_TrayWndBottom,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	//Okno kontaktow chowane za gorna krawedzia ekranu
	else
		SetWindowPos(hFrmMain,HWND_BOTTOM,FrmMainRect.Left,FrmMain_Shell_TrayWndTop,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
}
//---------------------------------------------------------------------------

//Sztywne schowanie okna kontaktow za krawedz
void HideFrmMain()
{
	//Okno kontaktow chowane za lewa/prawa krawedzia ekranu
	if((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))
		SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainSlideLeft,FrmMainRect.Top,0,0,SWP_NOSIZE);
	//Okno kontaktow chowane za dolna/gorna krawedzia ekranu
	else
		SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainRect.Left,FrmMainSlideTop,0,0,SWP_NOSIZE);
}
//---------------------------------------------------------------------------

//Zatrzymanie timera pokazywania okna kontaktow (part I)
void StopPreFrmMainSlideIn()
{
	//Status pre-wysuwania okna kontaktow zza krawedzi ekranu
	PreFrmMainSlideIn = true;
	//Brak tymczasowej blokady po wysunieciu okna
	FrmMainDontBlockSlide = true;
	//Wlaczenie pokazywania okna kontaktow (part I)
	SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEIN,FrmMainSlideInDelay,(TIMERPROC)TimerFrmProc);
}
//---------------------------------------------------------------------------

//Ustawienie fokusa na polu wpisywania wiadomosci
void FocusRichEdit()
{
	//Blokada lokalnego hooka na myszke
	BlockThreadMouseProc = true;
	//Pobieranie pozycji kursora
	SendMessage(hRichEdit, EM_EXGETSEL, NULL, (LPARAM)&hRichEditSelPos);
	//Emulacja klikniecia
	TRect RichEditRect;
	GetWindowRect(hRichEdit,&RichEditRect);
	POINT pCur;
	GetCursorPos(&pCur);
	SetCursorPos(RichEditRect.Right-5,RichEditRect.Bottom-5);
	mouse_event(MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP,0,0,0,0);
	SetCursorPos(pCur.x,pCur.y);
	//Pobieranie tekstu z RichEdit
	int iLength = GetWindowTextLengthW(hRichEdit)+1;
	wchar_t* pBuff = new wchar_t[iLength];
	GetWindowTextW(hRichEdit, pBuff, iLength);
	UnicodeString Text = pBuff;
	delete pBuff;
	//Pozycja kursora inna niz na koncu tekstu
	if(hRichEditSelPos.cpMin!=Text.Length())
	{
		//Blokowanie lokalnego hooka na klawiature
		BlockThreadKeyboardProc = true;
		//Wlaczenie timera ustawiania starej pozycji kursora
		SetTimer(hTimerFrm,TIMER_EXSETSEL,100,(TIMERPROC)TimerFrmProc);
	}
	//Wlaczenie timera wylaczania blokady lokalnego hooka na myszke
	SetTimer(hTimerFrm,TIMER_UNBLOCK_MOUSE_PROC,100,(TIMERPROC)TimerFrmProc);
}
//---------------------------------------------------------------------------

//Aktywcja okna rozmowy + nadanie fokusa na polu wpisywania wiadomosci + usuniecie licznika nowych wiadomosci
void ActivateAndFocusFrmSend()
{
	//Aktywacja okna
	SetForegroundWindow(hFrmSend);
	//Brak przeciagania na pole wpisywania wiadomosci
	if((!DragDetect(hFrmSend,Mouse->CursorPos))&&(hRichEdit))
	{
		//Ustawianie fokusa
		FocusRichEdit();
	}
	//Zmiana tekstu na belce okna
	if((InactiveFrmNewMsgChk)&&(InactiveFrmNewMsgCount))
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
	//Notyfikcja pisania wiadomosci
	if(ChatStateNotiferNewMsgChk)
	{
		//Resetowanie poprzedniego stanu pisania wiadomosci
		LastChatState = 0;
		//Ustawienie oryginalnej malej ikonki
		if(hIconSmall)
		{
			SendMessage(hFrmSend, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
			hIconSmall = 0;
		}
		//Ustawienie oryginalnej duzej ikonki
		if(hIconBig)
		{
			SendMessage(hFrmSend, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
			hIconBig = 0;
		}
	}
	//Miganie diodami LED klawiatury - wylaczanie mrugania
	if((KeyboardFlasherChk)&&(hFlasherThread)&&(hFlasherKeyboardThread))
	{
		//Usuwanie z listy nieprzeczytanych wiadomosci aktywnej zakladki
		if(UnreadMsgList->IndexOf(ActiveTabJIDEx)!=-1)
			UnreadMsgList->Delete(UnreadMsgList->IndexOf(ActiveTabJIDEx));
		//Nie ma juz nieprzeczytanych wiadomosci
		if(!UnreadMsgList->Count)
		{
			SetEvent(hFlasherThread);
			WaitForSingleObject(hFlasherKeyboardThread, 30000);
			CloseHandle(hFlasherThread);
			hFlasherThread = NULL;
			hFlasherKeyboardThread = NULL;
		}
	}
}
//---------------------------------------------------------------------------

//Aktywacja okna rozmowy + ustawienie okna na wierzchu
void ActivateAndSetTopmostFrmSend()
{
	//Aktywacja okna
	SetForegroundWindow(hFrmSend);
	SetActiveWindow(hFrmSend);
	SetFocus(hFrmSend);
	//Ustawienie okna na wierzchu
	SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
}
//---------------------------------------------------------------------------

//Odswiezenie okna rozmowy
void RefreshFrmSend()
{
	//Pobieranie pozycji okna rozmowy
	TRect WindowRect;
	GetWindowRect(hFrmSend,&WindowRect);
	//Odswiezenie okna rozmowy
	SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
	SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
}
//---------------------------------------------------------------------------

//Pobranie rozmiaru + pozycji okna rozmowy
void GetFrmSendRect()
{
	//Pobranie rozmiaru + pozycji okna rozmowy
	GetWindowRect(hFrmSend,&FrmSendRect);
	//Pobranie realnego rozmiaru + pozycji okna rozmowy
	GetWindowInfo(hFrmSend,&FrmSendInfo);
	//Okno rozmowy chowane za lewa krawedzia ekranu
	if(FrmSendSlideEdge==1)
	{
		FrmSendRealLeftPos = FrmSendInfo.rcWindow.left - FrmSendInfo.rcClient.left;
		FrmSendRealRightPos = 0;
		FrmSendRealBottomPos = 0;
		FrmSendRealTopPos = 0;
	}
	//Okno rozmowy chowane za prawa krawedzia ekranu
	else if(FrmSendSlideEdge==2)
	{
		FrmSendRealRightPos = FrmSendInfo.rcWindow.right - FrmSendInfo.rcClient.right;
		FrmSendRealLeftPos = 0;
		FrmSendRealBottomPos = 0;
		FrmSendRealTopPos = 0;
	}
	//Okno rozmowy chowane za dolna krawedzia ekranu
	else if(FrmSendSlideEdge==3)
	{
		FrmSendRealBottomPos = FrmSendInfo.rcWindow.bottom - FrmSendInfo.rcClient.bottom;
		FrmSendRealLeftPos = 0;
		FrmSendRealRightPos = 0;
		FrmSendRealTopPos = 0;
	}
	//Okno rozmowy chowane za gorna krawedzia ekranu
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
	//Okno rozmowy chowane za lewa krawedzia ekranu
	if(FrmSendSlideEdge==1)
		SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,FrmSendRect.Top,0,0,SWP_NOSIZE);
	//Okno rozmowy chowane za prawa krawedzia ekranu
	else if(FrmSendSlideEdge==2)
		SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-FrmSendRect.Width()-FrmSend_Shell_TrayWndRight,FrmSendRect.Top,0,0,SWP_NOSIZE);
	//Okno rozmowy chowane za dolna krawedzia ekranu
	else if(FrmSendSlideEdge==3)
		SetWindowPos(hFrmSend,HWND_TOP,FrmSendRect.Left,Screen->Height-FrmSendRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
	//Okno rozmowy chowane za gorna krawedzia ekranu
	else
		SetWindowPos(hFrmSend,HWND_TOP,FrmSendRect.Left,FrmSend_Shell_TrayWndTop,0,0,SWP_NOSIZE);
}
//---------------------------------------------------------------------------

//Sztywne wysuniecie okna rozmowy zza krawedzi
void ShowFrmSend()
{
	//Okno rozmowy chowane za lewa krawedzia ekranu
	if(FrmSendSlideEdge==1)
		SetWindowPos(hFrmSend,HWND_BOTTOM,0+FrmSend_Shell_TrayWndLeft,FrmSendRect.Top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	//Okno rozmowy chowane za prawa krawedzia ekranu
	else if(FrmSendSlideEdge==2)
		SetWindowPos(hFrmSend,HWND_BOTTOM,Screen->Width-FrmSendRect.Width()-FrmSend_Shell_TrayWndRight,FrmSendRect.Top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	//Okno rozmowy chowane za dolna krawedzia ekranu
	else if(FrmSendSlideEdge==3)
		SetWindowPos(hFrmSend,HWND_BOTTOM,FrmSendRect.Left,Screen->Height-FrmSendRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
	//Okno rozmowy chowane za gorna krawedzia ekranu
	else
		SetWindowPos(hFrmSend,HWND_BOTTOM,FrmSendRect.Left,FrmSend_Shell_TrayWndTop,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
}
//---------------------------------------------------------------------------

//Sztywne schowanie okna rozmowy za krawedz
void HideFrmSend()
{
	//Okno rozmowy chowane za lewa.prawa krawedzia ekranu
	if((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))
		SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendSlideLeft,FrmSendRect.Top,0,0,SWP_NOSIZE);
	//Okno rozmowy chowane za dolna/gorna krawedzia ekranu
	else
		SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendRect.Left,FrmSendSlideTop,0,0,SWP_NOSIZE);
}
//---------------------------------------------------------------------------

//Zatrzymanie timera pokazywania okna rozmowy (part I)
void StopPreFrmSendSlideIn()
{
	//Zatrzymanie timera
	KillTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEIN);
	//Status pre-wysuwania okna rozmowy zza krawedzi ekranu
	PreFrmSendSlideIn = false;
	//Brak tymczasowej blokady po wysunieciu okna
	FrmSendDontBlockSlide = false;
}
//---------------------------------------------------------------------------

//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
void ChkFullScreenMode()
{
	//Odznaczenie braku wyjatku
	FullScreenModeExeptions = false;
	//Pobieranie wymiarow aktywnego okna
	TRect ActiveFrmRect;
	GetWindowRect(GetForegroundWindow(),&ActiveFrmRect);
	//Sprawdzanie szerokosci/wysokosci okna
	if((ActiveFrmRect.Width()==Screen->Width)&&(ActiveFrmRect.Height()==Screen->Height))
	{
		//Pobieranie klasy aktywnego okna
		wchar_t WindowClassNameW[128];
		GetClassNameW(GetForegroundWindow(), WindowClassNameW, sizeof(WindowClassNameW));
		UnicodeString WindowClassName = WindowClassNameW;
		//Wyjatek dla pulpitu oraz programu DeskScapes
		if((WindowClassName!="Progman")&&(WindowClassName!="SysListView32")&&(WindowClassName!="WorkerW")&&(WindowClassName!="NDesk"))
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
			if(WindowClassName.Pos("Windows.UI.Core.CoreWindow"))
				FullScreenModeExeptions = true;
			//Aplikacja pelnoekranowa
			FullScreenMode = true;
		}
		else FullScreenMode = false;
	}
	else FullScreenMode = false;
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
	//Funkcjonalnosc wlaczona
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

//Minimalizacja / przywracanie okna rozmowy
void MinimizeRestoreFrmSendExecute()
{
	//Okno rozmowy jest otwarte
	if(hFrmSend)
	{
		//Komunikator nie jest zabezpieczony
		if(!SecureMode)
		{
			//Funkcjonalnosc chowania okna rozmowy jest wylaczona
			if(!FrmSendSlideChk)
			{
				//Przywracanie okna rozmowy
				if(IsIconic(hFrmSend))
				{
					ShowWindow(hFrmSend,SW_RESTORE);
					SetForegroundWindow(hFrmSend);
				}
				//Minimalizacja okna rozmowy
				else ShowWindow(hFrmSend,SW_MINIMIZE);
			}
			//Funkcjonalnosc chowania okna rozmowy jest wlaczona
			else
			{
				//Chowanie okna rozmowy
				if(FrmSendVisible)
				{
					//Wylaczenie tymczasowej blokady chowania/pokazywania okna rozmowy
					if((FrmSendSlideHideMode==3)&&(FrmSendUnBlockSlide))
					{
						//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
						KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE);
						//Tymczasowa blokada chowania/pokazywania okna rozmowy
						if(StayOnTopChk) FrmSendBlockSlide = StayOnTopStatus;
						else FrmSendBlockSlide = false;
						//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
						FrmSendUnBlockSlide = false;
					}
					//Chowanie okna nie jest zablokowane
					if((!FrmSendBlockSlide)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))
					{
						//Status chowania okna rozmowy za krawedz ekranu
						FrmSendSlideOut = true;
						//Wlaczenie chowania okna rozmowy (part I)
						SetTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
					}
				}
				//Pokazywanie okna rozmowy
				else
				{
					//Pokazywanie okna nie jest zablokowane
					if((!FrmSendBlockSlide)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))
					{
						//Status wysuwania okna rozmowy zza krawedzi ekranu
						FrmSendSlideIn = true;
						//Wlaczenie pokazywania okna rozmowy (part I)
						SetTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEIN,1,(TIMERPROC)TimerFrmProc);
					}
				}
			}
		}
		//Komunikator jest zabezpieczony
		else PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aSecure");
	}
}
//---------------------------------------------------------------------------

//Minimalizacja / przywracanie okna kontaktow + otwieranie okna rozmowy z nowa wiadomoscia
void MinimizeRestoreFrmMainExecute()
{
	//Komunikator nie jest zabezpieczony
	if(!SecureMode)
	{
		//Przywracanie okna kontaktow
		if(UnOpenMsgList->Count==0)
		{
			//Chowanie okna kontaktow
			if(FrmMainVisible)
			{
				//Wylaczenie tymczasowej blokady chowania/pokazywania okna kontaktow
				if((FrmMainSlideHideMode==3)&&(FrmMainUnBlockSlide))
				{
					//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna kontaktow
					KillTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE);
					//Tymczasowa blokada chowania/pokazywania okna kontaktow
					FrmMainBlockSlide = false;
					//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
					FrmMainUnBlockSlide = false;
				}
				//Chowanie okna nie jest zablokowane
				if((!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
				{
					//Status chowania okna kontaktow za krawedz ekranu
					FrmMainSlideOut = true;
					//Wlaczenie chowania okna kontaktow (part I)
					SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
				}
			}
			//Pokazywanie okna kontaktow
			else
			{
				//Pokazywanie okna nie jest zablokowane
				if((!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
				{
					//Status wysuwania okna kontaktow zza krawedzi ekranu
					FrmMainSlideIn = true;
					//Wlaczenie pokazywania okna kontaktow (part I)
					SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEIN,1,(TIMERPROC)TimerFrmProc);
				}
			}
		}
		//Otwieranie okna rozmowy z nowa wiadomoscia
		else
		{
			//Pobranie pierwszej z kolei nieprzeczytanej wiadomosci
			UnicodeString JID = UnOpenMsgList->Strings[0];
			//Otwieranie zakladki z podanym kontaktem/czatem
			OpenNewTab(JID);
		}
	}
	//Komunikator jest zabezpieczony
	else PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aSecure");
}
//---------------------------------------------------------------------------

//Pokazywanie paska narzedzi w oknie rozmowy
void ShowToolBar()
{
	//Okno rozmowy jest otwarte i uchwyt do paska narzedzi zostal pobrany
	if((hFrmSend)&&(hToolBar))
	{
		//Pobieranie pozycji paska narzedzi
		TRect WindowRect;
		GetWindowRect(hToolBar,&WindowRect);
		//Pasek narzedzi niewidoczny
		if(!WindowRect.Height())
		{
			//Pokazanie paska
			SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),23,SWP_NOMOVE);
			//Odswiezenie okna rozmowy
			RefreshFrmSend();
		}
	}
}
//---------------------------------------------------------------------------

//Ukrywanie paska narzedzi w oknie rozmowy
void HideToolBar()
{
	//Okno rozmowy jest otwarte i uchwyt do paska narzedzi zostal pobrany
	if((hFrmSend)&&(hToolBar))
	{
		//Pobieranie pozycji paska narzedzi
		TRect WindowRect;
		GetWindowRect(hToolBar,&WindowRect);
		//Pasek narzedzi widoczny
		if(WindowRect.Height())
		{
			//Ukrycie paska narzedzi
			SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
			//Odswiezenie okna rozmowy
			RefreshFrmSend();
		}
	}
}
//---------------------------------------------------------------------------

//Ukrywanie/pokazywanie przyciskow do przewijania zakladek
void CheckHideScrollTabButtons()
{
	//Okno rozmowy jest otwarte i uchwyty do kontrolek zostaly pobrane
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

//Konwersja tekstu na liczbe
int Convert(UnicodeString Char)
{
	for(int IntChar=-113;IntChar<=255;IntChar++)
		if(Char==CHAR(IntChar)) return IntChar;
	return 0;
}
//---------------------------------------------------------------------------
UnicodeString ConvertToInt(UnicodeString Text)
{
	UnicodeString ConvertedText;
	for(int Count=1;Count<=Text.Length();Count++)
	{
		UnicodeString tmpStr = Text.SubString(Count, 1);
		int tmpInt = Convert(tmpStr);
		ConvertedText = ConvertedText + IntToStr(tmpInt);
	}
	return ConvertedText;
}
//---------------------------------------------------------------------------

//Kodowanie ciagu znakow do Base64
UnicodeString EncodeBase64(UnicodeString Str)
{
	return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_BASE64,(WPARAM)Str.w_str(),3);
}
//---------------------------------------------------------------------------

//Dekodowanie ciagu znakow z Base64
UnicodeString DecodeBase64(UnicodeString Str)
{
	return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_BASE64,(WPARAM)Str.w_str(),2);
}
//---------------------------------------------------------------------------

//Skracanie wyswietlania odnosnikow
UnicodeString TrimLinks(UnicodeString Body, bool Status)
{
	//Dodawanie specjalnego tagu do wszystkich linkow
	Body = StringReplace(Body, "<A HREF", "[CC_LINK_START]<A HREF", TReplaceFlags() << rfReplaceAll);
	Body = StringReplace(Body, "</A>", "</A>[CC_LINK_END]", TReplaceFlags() << rfReplaceAll);
	//Formatowanie tresci wiadomosci
	while(Body.Pos("[CC_LINK_END]"))
	{
		//Wyciagniecie kodu HTML odnosnika
		UnicodeString URL = Body;
		URL.Delete(1,URL.Pos("[CC_LINK_START]")+14);
		URL.Delete(URL.Pos("[CC_LINK_END]"),URL.Length());
		//Wyciaganie tekstu odnosnika
		UnicodeString Text = URL;
		Text.Delete(Text.Pos("</A>"),Text.Length());
		Text.Delete(1,Text.Pos("\">")+1);
		//Link do filmu YouTube (tylko dla opisow)
		if((Status)&&(((Text.Pos("youtube.com"))&&(((Text.Pos("watch?"))&&(Text.Pos("v=")))||(Text.Pos("/v/"))))||(Text.Pos("youtu.be"))))
		{
			//Zmienna ID
			UnicodeString ID;
			//Wyciaganie ID - fullscreenowy
			if(Text.Pos("/v/"))
			{
				//Parsowanie ID
				ID = Text;
				ID.Delete(1,ID.Pos("/v/")+2);
			}
			//Wyciaganie ID - zwykly & mobilny
			else if(Text.Pos("youtube.com"))
			{
				//Parsowanie ID
				ID = Text;
				ID.Delete(1,ID.Pos("v=")+1);
				if(ID.Pos("&"))	ID.Delete(ID.Pos("&"),ID.Length());
				if(ID.Pos("#"))	ID.Delete(ID.Pos("#"),ID.Length());
			}
			//Wyciaganie ID - skrocony
			else if(Text.Pos("youtu.be"))
			{
				//Parsowanie ID
				ID = Text;
				ID.Delete(1,ID.Pos(".be/")+3);
			}
			//Id nie znajduje sie na liscie ID filmow YouTube wykluczonych na czas sesji
			if(YouTubeExcludeList->IndexOf(ID)==-1)
			{
				//Szukanie ID w cache
				TIniFile *Ini = new TIniFile(SessionFileDir);
				UnicodeString Title = DecodeBase64(Ini->ReadString("YouTube",ConvertToInt(ID),""));
				delete Ini;
				//Tytul pobrany z cache
				if(!Title.IsEmpty())
				{
					//Odnosnik z parametrem title
					if(URL.Pos("title="))
						Body = StringReplace(Body, "\">" + Text, "\">" + Title, TReplaceFlags());
					//Odnosnik bez parametru title
					else
						Body = StringReplace(Body, "\">" + Text, "\" title=\"" + Text + "\">" + Title, TReplaceFlags());
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
					//Odnosnik z parametrem title
					if(URL.Pos("title="))
						Body = StringReplace(Body, "\">" + Text, "\">["+GetLangStr("YouTubeTemp")+"...]", TReplaceFlags());
					//Odnosnik bez parametru title
					else
						Body = StringReplace(Body, "\">" + Text, "\" title=\"" + Text + "\">["+GetLangStr("YouTubeTemp")+"...]", TReplaceFlags());
				}
			}
			//Przejscie do normalnego skracana linkow
			else goto NormalTrim;
		}
		//Inne linki
		else
		{
			//Skok do normnalnego skracania linkow
			NormalTrim: { /* Only Jump */ }
			//Wycinanie domeny z adresu URL
			UnicodeString Domain = Text;
			if(Domain.LowerCase().Pos("http://"))
			{
				Domain.Delete(1,Domain.LowerCase().Pos("http://")+6);
				if(Domain.Pos("/")) Domain.Delete(Domain.Pos("/"),Domain.Length());
			}
			else if(Domain.LowerCase().Pos("https://"))
			{
				Domain.Delete(1,Domain.LowerCase().Pos("https://")+7);
				if(Domain.Pos("/")) Domain.Delete(Domain.Pos("/"),Domain.Length());
			}
			else if(Domain.LowerCase().Pos("www."))
			{
				Domain.Delete(1,Domain.LowerCase().Pos("www.")+3);
				if(Domain.Pos("/")) Domain.Delete(Domain.Pos("/"),Domain.Length());
			}
			else Domain = "";
			//Wyciagnieto prawidlowo nazwe domeny z adresu URL
			if(!Domain.IsEmpty())
			{
				//Usuniecie subdomeny WWW
				if(Domain.LowerCase().Pos("www.")) Domain.Delete(Domain.LowerCase().Pos("www."),Domain.LowerCase().Pos("www.")+3);
				//Odnosnik z parametrem title
				if(URL.Pos("title="))
					Body = StringReplace(Body, "\">" + Text, "\">[" + Domain + "]", TReplaceFlags());
				//Odnosnik bez parametru title
				else
					Body = StringReplace(Body, "\">" + Text, "\" title=\"" + Text + "\">[" + Domain + "]", TReplaceFlags());
			}
		}
		//Usuwanie wczesniej dodanych tagow
		Body = StringReplace(Body, "[CC_LINK_START]", "", TReplaceFlags());
		Body = StringReplace(Body, "[CC_LINK_END]", "", TReplaceFlags());
	}
	return Body;
}
//---------------------------------------------------------------------------

//Pobieranie pseudonimu kontaktu podajac jego JID
UnicodeString GetContactNick(UnicodeString JID)
{
	//Zwykly kontakt
	if(!JID.Pos("ischat_"))
	{
		//Odczyt pseudonimu z pliku INI
		UnicodeString Nick = ContactsNickList->ReadString("Nick",JID,"");
		//Pseudonim nie zostal pobrany
		if(Nick.IsEmpty())
		{
			//Skracanie JID do ladniejszej formy
			if(JID.Pos("@")) JID.Delete(JID.Pos("@"),JID.Length());
			if(JID.Pos(":")) JID.Delete(JID.Pos(":"),JID.Length());
			return JID;
		}
		return Nick;
	}
	//Czat
	return GetChannelName(JID);
}
//---------------------------------------------------------------------------

//Przyjazniejsze formatowanie JID
UnicodeString FriendlyFormatJID(UnicodeString JID)
{
	//nk.pl
	if(JID.Pos("@nktalk.pl")) return "nk.pl";
	//Facebook
	if(JID.Pos("@chat.facebook.com")) return "Facebook";
	//GTalk
	if(JID.Pos("@public.talk.google.com")) return "GTalk";
	//Skype
	if(JID.Pos("@skype.plugin.aqq.eu")) return "Skype";
	//GG
	if(JID.Pos("@plugin.gg.aqq.eu")) return "GG";
	//Inne wtyczki
	if(JID.Pos("@plugin"))
	{
		JID.Delete(JID.Pos("@"),JID.Length());
		return JID;
	}
	//Usuwanie indeksu konta z JID
	if(JID.Pos(":")) JID.Delete(JID.Pos(":"),JID.Length());
	//Ustawianie prawidlowego identyfikatora dla kontaktow czatowych
	if(JID.Pos("ischat_")) JID.Delete(1,7);
	//Pozostale kontakty
	return JID;
}
//---------------------------------------------------------------------------

//Pobieranie stanu kontaktu podajac jego JID
int GetContactState(UnicodeString JID)
{
	//Zwykly kontakt
	if(!JID.Pos("ischat_"))
	{
		//Ikona bota Blabler (gdy zakladka jest przypieta)
		//if(((JID=="blabler.k2t.eu")||(JID.Pos("48263287@plugin.gg")==1))&&(!UnloadExecuted))
		//{
			//Definicja tymczasowego JID
			//UnicodeString tmpJID = JID;
			//Usuwanie zasobu z JID
			//if(tmpJID.Pos("/")) tmpJID.Delete(tmpJID.Pos("/"),tmpJID.Length());
			//Kontakt jest przypiety
			//if(ClipTabsList->IndexOf(tmpJID)!=-1) return 131;
		//}
		//Ikona bota tweet.IM (gdy zakladka jest przypieta)
		if((JID.Pos("@twitter.tweet.im"))&&(!UnloadExecuted))
		{
			//Definicja tymczasowego JID
			UnicodeString tmpJID = JID;
			//Usuwanie zasobu z JID
			if(tmpJID.Pos("/")) tmpJID.Delete(tmpJID.Pos("/"),tmpJID.Length());
			//Kontakt jest przypiety
			if(ClipTabsList->IndexOf(tmpJID)!=-1) return 131;
		}
		//Pobranie stanu kontatu z listy stanow zbieranej przez wtyczke
		int State = ContactsStateList->ReadInteger("State",JID,-1);
		//Jezeli stan kontaktu nie jest zapisany
		if(State==-1)
		{
			//Usuwanie zasobu z JID
			if(JID.Pos("/")) JID.Delete(JID.Pos("/"),JID.Length());
			//Usuwanie indeksu konta z JID
			if(JID.Pos(":")) JID.Delete(JID.Pos(":"),JID.Length());
			//Pobranie domyslnej ikonki dla kontaktu
			TPluginContact PluginContact;
			ZeroMemory(&PluginContact, sizeof(TPluginContact));
			PluginContact.cbSize = sizeof(TPluginContact);
			PluginContact.JID = JID.w_str();
			State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));
		}
		//Zwrocenie ikonki stanu kontaktu
		return State;
	}
	//Czat
	return 79;
}
//---------------------------------------------------------------------------

//Sprawdzanie plci kontaktu na podstawie danych w wizytowce
bool ChkContactGender(UnicodeString JID)
{
	//Otwieranie pliku INI kontatku
	TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
	//Pobieranie informacji o plci kontaktu
	UnicodeString Gender = Ini->ReadString("Buddy","Gender","");
	//Zamkniecie pliku ini
	delete Ini;
	//Kobieta
	if(Gender=="RkVNQUxF") return false;
	//Mezczyzna
	else return true;
}
//---------------------------------------------------------------------------

//Konwersja Unix'owego formatu czasu na TDateTime
UnicodeString TimestampToDate(int Timestamp)
{
	if(Timestamp)
	{
		//Formatowanie daty z timestamp
		TDateTime DateTime = UnixToDateTime(StrToInt(Timestamp));
		//Pobranie aktualnego czasu + ustalenie ostatniej polnocy
		int CurrentTimestamp = DateTimeToUnix(TDateTime::CurrentDateTime(), true);
		CurrentTimestamp = CurrentTimestamp - (CurrentTimestamp % (24*60*60));
		//Dzisiaj
		if(Timestamp >= CurrentTimestamp)
			return GetLangStr("Today")+", "+DateTime.FormatString("h:nn");
		//Wczoraj
		else if(Timestamp >= (CurrentTimestamp - (24*60*60)))
			return GetLangStr("Yesterday")+", "+DateTime.FormatString("h:nn");
		//I póŸniej :)
		else
			return DateTime.FormatString(DateFormatOnClosedTabs);
	}
	return GetLangStr("NoData");
}
//---------------------------------------------------------------------------

//Przebudowa kolejnosci zakladek w pliku sesji
void RebuilSessionTabsList()
{
	//Usuwanie listy zakladek
	TabsList->Clear();
	TabsListEx->Clear();
	//Hook na pobieranie aktywnych zakladek
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs_GetOnlyList);
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_PRIMARYTAB,OnPrimaryTab_GetOnlyList);
	PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
	PluginLink.UnhookEvent(OnPrimaryTab_GetOnlyList);
	PluginLink.UnhookEvent(OnFetchAllTabs_GetOnlyList);
}
//---------------------------------------------------------------------------

//Otwieranie zakladki z podanym kontaktem/czatem
void OpenNewTab(UnicodeString JID)
{
	//Otwieranie zakladki ze zwyklym kontaktem
	if(!JID.Pos("ischat_"))
	{
		//Definicja domyslnego indeksu konta
		UnicodeString UserIdx = "0";
		//JID zawiera indeks konta
		if(JID.Pos(":"))
		{
			//Wyciagniecie indeksu konta
			UserIdx = JID;
			UserIdx.Delete(1,UserIdx.Pos(":"));
			//Usuniecie indeksu konta z JID
			JID.Delete(JID.Pos(":"),JID.Length());
		}
		//Wypelenie struktury do zmiany aktywnej zakladki
		TPluginExecMsg PluginExecMsg;
		PluginExecMsg.JID = JID.w_str();
		PluginExecMsg.UserIdx = StrToInt(UserIdx);
		PluginExecMsg.ActionSwitchTo = true;
		PluginExecMsg.ActionCloseWindow = false;
		PluginExecMsg.ActionTabIndex = false;
		PluginExecMsg.ActionTabWasClosed = false;
		PluginExecMsg.IsPriority = true;
		PluginExecMsg.IsFromPlugin = false;
		//Wywolanie funkcji zmiany zakladki
		PluginLink.CallService(AQQ_FUNCTION_MSGWINDOW,0,(LPARAM)(&PluginExecMsg));
	}
	//Otwieranie zakladki z czatem
	else
	{
		//Definicja domyslnego indeksu konta
		UnicodeString UserIdx = "0";
		//JID zawiera indeks konta
		if(JID.Pos(":"))
		{
			//Wyciagniecie indeksu konta
			UserIdx = JID;
			UserIdx.Delete(1,UserIdx.Pos(":"));
			//Usuniecie indeksu konta z JID
			JID.Delete(JID.Pos(":"),JID.Length());
		}
		//Sprawdzenie stanu konta przypisanego do czatu
		TPluginStateChange PluginStateChange;
		PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),(LPARAM)StrToInt(UserIdx));
		//Konto jest polaczone z siecia
		if(PluginStateChange.NewState!=0)
		{
			//Ustawianie prawidlowego identyfikatora
			JID.Delete(1,7);
			//Wypenianie struktury czatu
			TPluginChatPrep PluginChatPrep;
			PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
			PluginChatPrep.UserIdx = StrToInt(UserIdx);
			PluginChatPrep.JID = JID.w_str();
			PluginChatPrep.Channel = GetChannelName(JID).w_str();
			PluginChatPrep.CreateNew = false;
			PluginChatPrep.Fast = true;
			//Otwieranie zakladki czatowej
			PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
		}
	}
}
//---------------------------------------------------------------------------

//Zmiana aktywnej zakladki na wskazany kontakt/czat
void ChangeActiveTab(UnicodeString JID)
{
	//Definicja domyslnego indeksu konta
	UnicodeString UserIdx = "0";
	//JID zawiera indeks konta
	if(JID.Pos(":"))
	{
		//Wyciagniecie indeksu konta
		UserIdx = JID;
		UserIdx.Delete(1,UserIdx.Pos(":"));
		//Usuniecie indeksu konta z JID
		JID.Delete(JID.Pos(":"),JID.Length());
	}
	//Ustawianie prawidlowego identyfikatora dla kontaktow czatowych
	if(JID.Pos("ischat_")) JID.Delete(1,7);
	//Wypelenie struktury do zmiany aktywnej zakladki
	TPluginExecMsg PluginExecMsg;
	PluginExecMsg.JID = JID.w_str();
	PluginExecMsg.UserIdx = StrToInt(UserIdx);
	PluginExecMsg.ActionSwitchTo = true;
	PluginExecMsg.ActionCloseWindow = false;
	PluginExecMsg.ActionTabIndex = false;
	PluginExecMsg.ActionTabWasClosed = false;
	PluginExecMsg.IsPriority = true;
	PluginExecMsg.IsFromPlugin = false;
	//Wywolanie funkcji zmiany zakladki
	PluginLink.CallService(AQQ_FUNCTION_MSGWINDOW,0,(LPARAM)(&PluginExecMsg));
}
//---------------------------------------------------------------------------

//Pobieranie indeksu zakladki ze wskazanym kontaktem
int GetTabIndex(UnicodeString JID)
{
	//Definicja domyslnego indeksu konta
	UnicodeString UserIdx = "0";
	//JID zawiera indeks konta
	if(JID.Pos(":"))
	{
		//Wyciagniecie indeksu konta
		UserIdx = JID;
		UserIdx.Delete(1,UserIdx.Pos(":"));
		//Usuniecie indeksu konta z JID
		JID.Delete(JID.Pos(":"),JID.Length());
	}
	//Ustawianie prawidlowego identyfikatora dla kontaktow czatowych
	if(JID.Pos("ischat_")) JID.Delete(1,7);
	//Wypelenie struktury do pobierania aktualnej pozycji zakladki
	TPluginExecMsg PluginExecMsg;
	PluginExecMsg.JID = JID.w_str();
	PluginExecMsg.UserIdx = StrToInt(UserIdx);
	PluginExecMsg.ActionSwitchTo = false;
	PluginExecMsg.ActionCloseWindow = false;
	PluginExecMsg.ActionTabIndex = true;
	PluginExecMsg.ActionTabWasClosed = false;
	PluginExecMsg.IsPriority = true;
	PluginExecMsg.IsFromPlugin = false;
	//Wywolanie funkcji pobierania aktualnej pozycji zakladki i zwrocenie indeksu
	return PluginLink.CallService(AQQ_FUNCTION_MSGWINDOW,0,(LPARAM)(&PluginExecMsg));
}
//---------------------------------------------------------------------------

//Zmiana pozycji przypietej zakladki
void ChangeClipTabPos(UnicodeString JID,UnicodeString JIDEx)
{
	//Wczytanie pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *ClipTabs = new TStringList;
	Ini->ReadSection("ClipTabs",ClipTabs);
	int TabsCount = ClipTabs->Count;
	//Wyczyszczenie zmiennej sesji
	ClipTabs->Clear();
	//Sa jakies przypiete zakladki
	if(TabsCount>0)
	{
		//Odczyt kolejno wszystkich rekordow
		for(int Count=0;Count<TabsCount;Count++)
		{
			//Odczytanie JID przypietej zakladki
			UnicodeString ClipTab = Ini->ReadString("ClipTabs", "Tab"+IntToStr(Count+1),"");
			//Dodanie zakladki do listy przypietych zakladek
			if((!ClipTab.IsEmpty())&&(TabsList->IndexOf(ClipTab)!=-1))
				ClipTabs->Add(ClipTab);
		}
	}
	//Zamiana miejsca przypietej zakladki
	TPluginTriple PluginTriple;
	PluginTriple.cbSize = sizeof(TPluginTriple);
	PluginTriple.Handle1 = (int)hFrmSend;
	PluginTriple.Param1 = GetTabIndex(JIDEx);
	PluginTriple.Param2 = ClipTabs->IndexOf(JID);
	PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
	//Usuniecie zmiennej sesji
	delete ClipTabs;
}
//---------------------------------------------------------------------------

//Przebowa listy przypietych zakladek
void RebuildClipTabs()
{
	//Wczytanie pliku sesji i usuniecie listy przypietych zakladek
	TIniFile *Ini = new TIniFile(SessionFileDir);
	Ini->EraseSection("ClipTabs");
	//Zapisywanie nowej listy
	if(ClipTabsList->Count>0)
	{
		//Licznik przypietych zakladek
		int ClipTabsCount = 1;
		//Zapisywanie przypietych zakladek z niewidoczna nazwa zakladki
		for(int Count=0;Count<ClipTabsList->Count;Count++)
		{
			//Nazwa zakladki niewidoczna
			if(!Ini->ValueExists("ClipTabsEx",ClipTabsList->Strings[Count]))
			{
				Ini->WriteString("ClipTabs", "Tab"+IntToStr(ClipTabsCount), ClipTabsList->Strings[Count]);
				ClipTabsCount++;
			}
		}
		//Zapisywanie przypietych zakladek z widoczna nazwa zakladki
		for(int Count=0;Count<ClipTabsList->Count;Count++)
		{
			//Nazwa zakladki niewidoczna
			if(Ini->ValueExists("ClipTabsEx",ClipTabsList->Strings[Count]))
			{
				Ini->WriteString("ClipTabs", "Tab"+IntToStr(ClipTabsCount), ClipTabsList->Strings[Count]);
				ClipTabsCount++;
			}
		}
	}
	//Ponowne odczytanie pozycji przypietych zakladek z pliku
	ClipTabsList->Clear();
	TStringList *ClipTabs = new TStringList;
	Ini->ReadSection("ClipTabs",ClipTabs);
	int TabsCount = ClipTabs->Count;
	delete ClipTabs;
	//Sa jakies przypiete zakladki
	if(TabsCount>0)
	{
		//Odczyt kolejno wszystkich rekordow
		for(int Count=0;Count<TabsCount;Count++)
		{
			//Odczytanie JID przypietej zakladki
			UnicodeString ClipTab = Ini->ReadString("ClipTabs", "Tab"+IntToStr(Count+1),"");
			//Dodanie zakladki do listy przypietych zakladek
			if(!ClipTab.IsEmpty()) ClipTabsList->Add(ClipTab);
		}
	}
	delete Ini;
}
//---------------------------------------------------------------------------
void RebuildClipTabsEx()
{
	//Wczytanie pliku sesji i usuniecie listy przypietych zakladek
	TIniFile *Ini = new TIniFile(SessionFileDir);
	Ini->EraseSection("ClipTabs");
	//Zapisywanie nowej listy
	if(ClipTabsList->Count>0)
	{
		//Licznik przypietych zakladek
		int ClipTabsCount = 1;
		//Zapisywanie przypietych zakladek z niewidoczna nazwa zakladki
		for(int Count=0;Count<TabsList->Count;Count++)
		{
			//Zakladka jest przypieta i nazwa zakladki jest niewidoczna
			if((ClipTabsList->IndexOf(TabsList->Strings[Count])!=-1)&&(!Ini->ValueExists("ClipTabsEx",TabsList->Strings[Count])))
			{
				Ini->WriteString("ClipTabs", "Tab"+IntToStr(ClipTabsCount), TabsList->Strings[Count]);
				ClipTabsCount++;
			}
		}
		for(int Count=0;Count<ClipTabsList->Count;Count++)
		{
			//Nie otwarta przypieta zakladka z niewidoczna nazwa zakladki
			if(((TabsList->IndexOf(ClipTabsList->Strings[Count])==-1))&&(!Ini->ValueExists("ClipTabsEx",ClipTabsList->Strings[Count])))
			{
				Ini->WriteString("ClipTabs", "Tab"+IntToStr(ClipTabsCount), ClipTabsList->Strings[Count]);
				ClipTabsCount++;
			}
		}
		//Zapisywanie przypietych zakladek z widoczna nazwa zakladki
		for(int Count=0;Count<TabsList->Count;Count++)
		{
			//Zakladka jest przypieta i nazwa zakladki jest niewidoczna
			if((ClipTabsList->IndexOf(TabsList->Strings[Count])!=-1)&&(Ini->ValueExists("ClipTabsEx",TabsList->Strings[Count])))
			{
				Ini->WriteString("ClipTabs", "Tab"+IntToStr(ClipTabsCount), TabsList->Strings[Count]);
				ClipTabsCount++;
			}
		}
		for(int Count=0;Count<ClipTabsList->Count;Count++)
		{
			//Nie otwarta przypieta zakladka z niewidoczna nazwa zakladki
			if(((TabsList->IndexOf(ClipTabsList->Strings[Count])==-1))&&(Ini->ValueExists("ClipTabsEx",ClipTabsList->Strings[Count])))
			{
				Ini->WriteString("ClipTabs", "Tab"+IntToStr(ClipTabsCount), ClipTabsList->Strings[Count]);
				ClipTabsCount++;
			}
		}
	}
	//Ponowne odczytanie pozycji przypietych zakladek z pliku
	ClipTabsList->Clear();
	TStringList *ClipTabs = new TStringList;
	Ini->ReadSection("ClipTabs",ClipTabs);
	int TabsCount = ClipTabs->Count;
	delete ClipTabs;
	//Sa jakies przypiete zakladki
	if(TabsCount>0)
	{
		//Odczyt kolejno wszystkich rekordow
		for(int Count=0;Count<TabsCount;Count++)
		{
			//Odczytanie JID przypietej zakladki
			UnicodeString ClipTab = Ini->ReadString("ClipTabs", "Tab"+IntToStr(Count+1),"");
			//Dodanie zakladki do listy przypietych zakladek
			if(!ClipTab.IsEmpty()) ClipTabsList->Add(ClipTab);
		}
	}
	delete Ini;
}
//---------------------------------------------------------------------------

//Sprawdzanie listy ID filmow YouTube do przetworzenia
bool ChkYouTubeListItem()
{
	if(GetYouTubeTitleList->Count) return true;
	else return false;
}
//---------------------------------------------------------------------------

//Pobieranie ID filmu YouTube do przetworzenia
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

//Dodawanie ID filmu YouTube do listy wykluczonych na czas sesji
void AddToYouTubeExcludeList(UnicodeString ID)
{
	YouTubeExcludeList->Add(ID);
}
//---------------------------------------------------------------------------

//Odswiezenie listy kontaktow jest dozwolone
bool RefreshListAllowed()
{
	return AllowRefreshList;
}
//---------------------------------------------------------------------------

//Odswiezanie listy kontaktow
void RefreshList()
{
	PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aRefresh");
}
//---------------------------------------------------------------------------

//Pobieranie nazwy konta przez podanie jego indeksu
UnicodeString ReceiveAccountName(int UserIdx)
{
	TPluginStateChange PluginStateChange;
	PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),(LPARAM)UserIdx);
	UnicodeString Name = (wchar_t*)PluginStateChange.JID;
	Name = Name + "/" + (wchar_t*)PluginStateChange.Resource;
	return Name;
}
//---------------------------------------------------------------------------

//Normalizacja nazw kanalow
UnicodeString NormalizeChannel(UnicodeString Channel)
{
	return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_NORMALIZE,0,(LPARAM)Channel.w_str());
}
//---------------------------------------------------------------------------

//Pobieranie nazwy kanalu
UnicodeString GetChannelName(UnicodeString JID)
{
	//Ustawianie prawidlowego identyfikatora
	if(JID.Pos("ischat_")) JID.Delete(1,7);
	//Usuwanie zasobu z JID
	if(JID.Pos("/")) JID.Delete(JID.Pos("/"),JID.Length());
	//Usuwanie indeksu konta z JID
	if(JID.Pos(":")) JID.Delete(JID.Pos(":"),JID.Length());
	//Pobieranie nazwy kanalu
	TIniFile *Ini = new TIniFile(SessionFileDir);
	UnicodeString Channel = DecodeBase64(Ini->ReadString("Channels",JID,""));
	delete Ini;
	if(Channel.IsEmpty())
	{
		Channel = JID;
		Channel.Delete(Channel.Pos("@"),Channel.Length());
	}
	//Zwrocenie nazwy kanalu
	return Channel;
}
//---------------------------------------------------------------------------
void GetChannelNameW(UnicodeString JID)
{
	//Generowanie unikatowego ID
	UnicodeString XMLID = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0);
	XMLIDList->Add(XMLID);
	//Generowane pakietu XML
	UnicodeString XML = "<iq from=\""+ReceiveAccountName(0)+"\" to=\""+JID+"\" id=\""+XMLID+"\" type=\"get\" xml:lang=\"pl\"><query xmlns=\"http://jabber.org/protocol/disco#info\"/></iq>";
	//Wyslanie pakietu XML na wskazane konto
	PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)XML.w_str(),0);
}
//---------------------------------------------------------------------------

//Aktualizacja pozycji wszystkich przyciskow w oknie rozmowy
void FixButtonsPosition()
{
	//Przywracanie sesji nie jest aktywne
	if(!RestoringSession)
	{
		//Szybki dostep do ulubionych zakladek
		DestroyFavouritesTabs();
		BuildFavouritesTabs(false);
		//Szybki dostep niewyslanych wiadomosci
		DestroyFrmUnsentMsg();
		BuildFrmUnsentMsg(false);
		//Szybki dostep do ostatnio zamknietych zakladek
		DestroyFrmClosedTabs();
		BuildFrmClosedTabs(false);
		//Trzymanie okna rozmowy na wierzchu
		DestroyStayOnTop();
		BuildStayOnTop();
	}
}
//---------------------------------------------------------------------------

//Usuwanie interfejsu dla ostatio zamknietych zakladek
void DestroyFrmClosedTabs()
{
	//Pobieranie ilosci zamknietych zakladek
	int TabsCount = ClosedTabsList->Count;
	//Sa jakies ostatnio zamkniete zakladki
	if(TabsCount>0)
	{
		//Maks X elementow w popupmenu
		if(TabsCount>ItemCountUnCloseTabVal) TabsCount = ItemCountUnCloseTabVal;
		//Usuwanie elementow popupmenu
		for(int Count=0;Count<TabsCount;Count++)
		{
			TPluginAction DestroyClosedTabsItem;
			ZeroMemory(&DestroyClosedTabsItem,sizeof(TPluginAction));
			DestroyClosedTabsItem.cbSize = sizeof(TPluginAction);
			UnicodeString ItemName = "TabKitClosedTabsItem"+IntToStr(Count);
			DestroyClosedTabsItem.pszName = ItemName.w_str();
			PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&DestroyClosedTabsItem));
		}
		//Usuwanie elementow do usuwania ostatnio zamknietych zakladek
		if(FastClearClosedTabsChk)
		{
			TPluginAction DestroyClosedTabsItem;
			ZeroMemory(&DestroyClosedTabsItem,sizeof(TPluginAction));
			UnicodeString ItemName = "TabKitClosedTabsItem"+IntToStr(TabsCount);
			DestroyClosedTabsItem.pszName = ItemName.w_str();
			PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyClosedTabsItem));
			ZeroMemory(&DestroyClosedTabsItem,sizeof(TPluginAction));
			ItemName = "TabKitClosedTabsItem"+IntToStr(TabsCount+1);
			DestroyClosedTabsItem.pszName = ItemName.w_str();
			PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&DestroyClosedTabsItem));
		}
		//Usuwanie buttona w oknie kontaktow
		TPluginAction FrmMainClosedTabsButton;
		ZeroMemory(&FrmMainClosedTabsButton,sizeof(TPluginAction));
		FrmMainClosedTabsButton.cbSize = sizeof(TPluginAction);
		FrmMainClosedTabsButton.pszName = L"TabKitFrmMainClosedTabsButton";
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON,0,(LPARAM)(&FrmMainClosedTabsButton));
		//Usuwanie buttona w oknie rozmowy
		TPluginAction FrmSendClosedTabsButton;
		ZeroMemory(&FrmSendClosedTabsButton,sizeof(TPluginAction));
		FrmSendClosedTabsButton.cbSize = sizeof(TPluginAction);
		FrmSendClosedTabsButton.pszName = L"TabKitFrmSendClosedTabsButton";
		FrmSendClosedTabsButton.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON,0,(LPARAM)(&FrmSendClosedTabsButton));
	}
}
//---------------------------------------------------------------------------

//Tworzenie interfejsu dla ostatnio zamknietych zakladek
void BuildFrmClosedTabs(bool FixPosition)
{
	//Interfejs ma byc stworzony
	if((ClosedTabsChk)&&(FastAccessClosedTabsChk))
	{
		//Pobieranie ilosci zamknietych zakladek
		int TabsCount = ClosedTabsList->Count;
		//Sa jakies ostatnio zamkniete zakladki
		if(TabsCount>0)
		{
			//Natychmiastowe tworzenie przyciskow
			if((!FixPosition)||((!hFrmSend)&&(FixPosition)))
			{
				//Maks X elementow w popupmenu
				if(TabsCount>ItemCountUnCloseTabVal) TabsCount = ItemCountUnCloseTabVal;
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
					FrmSendClosedTabsButton.Hint = GetLangStr("ClosedTabsButton").w_str();
					FrmSendClosedTabsButton.IconIndex = CLOSEDTABS;
					FrmSendClosedTabsButton.pszPopupName = L"TabKitClosedTabsPopUp";
					FrmSendClosedTabsButton.Handle = (int)hFrmSend;
					PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmSendClosedTabsButton));
				}
				//Tworzenie elementow popupmenu
				for(int Count=0;Count<TabsCount;Count++)
				{
					UnicodeString ItemJID = ClosedTabsList->Strings[Count];
					if(!ItemJID.IsEmpty())
					{
						TPluginAction BuildClosedTabsItem;
						ZeroMemory(&BuildClosedTabsItem,sizeof(TPluginAction));
						BuildClosedTabsItem.cbSize = sizeof(TPluginAction);
						BuildClosedTabsItem.IconIndex = GetContactState(ItemJID);
						UnicodeString ItemName = "TabKitClosedTabsItem"+IntToStr(Count);
						BuildClosedTabsItem.pszName = ItemName.w_str();
						UnicodeString ServiceName = "sTabKitClosedTabsItem"+IntToStr(Count);
						BuildClosedTabsItem.pszService = ServiceName.w_str();
						if(ShowTimeClosedTabsChk)
							BuildClosedTabsItem.pszCaption = (GetContactNick(ItemJID)+" ("+TimestampToDate(StrToInt(ClosedTabsTimeList->Strings[Count]))+")").w_str();
						else
							BuildClosedTabsItem.pszCaption = GetContactNick(ItemJID).w_str();
						BuildClosedTabsItem.Position = Count;
						BuildClosedTabsItem.pszPopupName = L"TabKitClosedTabsPopUp";
						PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildClosedTabsItem));
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
					UnicodeString ItemName = "TabKitClosedTabsItem"+IntToStr(TabsCount);
					BuildClosedTabsItem.pszName = ItemName.w_str();
					BuildClosedTabsItem.pszService = L"";
					BuildClosedTabsItem.pszCaption = L"-";
					BuildClosedTabsItem.Position = TabsCount;
					BuildClosedTabsItem.pszPopupName = L"TabKitClosedTabsPopUp";
					PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildClosedTabsItem));
					//Tworzenie elementu czyszczenia
					ZeroMemory(&BuildClosedTabsItem,sizeof(TPluginAction));
					BuildClosedTabsItem.cbSize = sizeof(TPluginAction);
					BuildClosedTabsItem.IconIndex = -1;
					ItemName = "TabKitClosedTabsItem"+IntToStr(TabsCount+1);
					BuildClosedTabsItem.pszName = ItemName.w_str();
					BuildClosedTabsItem.pszService = L"sTabKitClosedTabsItemClear";
					BuildClosedTabsItem.pszCaption = GetLangStr("Clear").w_str();
					BuildClosedTabsItem.Position = TabsCount+1;
					BuildClosedTabsItem.pszPopupName = L"TabKitClosedTabsPopUp";
					PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildClosedTabsItem));
				}
			}
			//Aktualizacja pozycji wszystkich przyciskow w oknie rozmowy
			else FixButtonsPosition();
		}
	}
}
//---------------------------------------------------------------------------

//Aktualizacja popupmenu ostatnio zamknietych zakladek
void RebuildFrmClosedTabsPopupmenu()
{
	//Pobieranie ilosci zamknietych zakladek
	int TabsCount = ClosedTabsList->Count;
	//Sa jakies ostatnio zamkniete zakladki
	if(TabsCount>0)
	{
		//Maks X elementow w popupmenu
		if(TabsCount>ItemCountUnCloseTabVal) TabsCount = ItemCountUnCloseTabVal;
		//Aktualizacja elementow popupmenu
		for(int Count=0;Count<TabsCount;Count++)
		{
			UnicodeString ItemJID = ClosedTabsList->Strings[Count];
			TPluginActionEdit RebuildClosedTabsItem;
			ZeroMemory(&RebuildClosedTabsItem,sizeof(TPluginActionEdit));
			RebuildClosedTabsItem.cbSize = sizeof(TPluginActionEdit);
			RebuildClosedTabsItem.IconIndex = GetContactState(ItemJID);
			UnicodeString ItemName = "TabKitClosedTabsItem"+IntToStr(Count);
			RebuildClosedTabsItem.pszName = ItemName.w_str();
			if(ShowTimeClosedTabsChk)
				RebuildClosedTabsItem.Caption = (GetContactNick(ItemJID)+" ("+TimestampToDate(StrToInt(ClosedTabsTimeList->Strings[Count]))+")").w_str();
			else
				RebuildClosedTabsItem.Caption = GetContactNick(ItemJID).w_str();
			RebuildClosedTabsItem.Enabled = true;
			RebuildClosedTabsItem.Visible = true;
			RebuildClosedTabsItem.Checked = false;
			PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&RebuildClosedTabsItem));
		}
	}
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
		//Sa jakies ostatnio zamkniete zakladki
		if(TabsCount>0)
		{
			//Maks 3 zdefiniowanych elementow
			if(TabsCount>3) TabsCount = 3;
			//Dodawanie ostatnio zamknietych zakladek do interfesju
			for(TabsCount;TabsCount>0;TabsCount--)
			{
				UnicodeString JID = ClosedTabsList->Strings[TabsCount-1];
				if((!JID.IsEmpty())&&(!JID.Pos("ischat_")))
				{
					//Definicja domyslnego indeksu konta
					UnicodeString UserIdx = "0";
					//JID zawiera indeks konta
					if(JID.Pos(":"))
					{
						//Wyciagniecie indeksu konta
						UserIdx = JID;
						UserIdx.Delete(1,UserIdx.Pos(":"));
						//Usuniecie indeksu konta z JID
						JID.Delete(JID.Pos(":"),JID.Length());
					}
					//Wypelenie struktury do zmiany aktywnej zakladki
					TPluginExecMsg PluginExecMsg;
					PluginExecMsg.JID = JID.w_str();
					PluginExecMsg.UserIdx = StrToInt(UserIdx);
					PluginExecMsg.ActionSwitchTo = false;
					PluginExecMsg.ActionCloseWindow = false;
					PluginExecMsg.ActionTabIndex = false;
					PluginExecMsg.ActionTabWasClosed = true;
					PluginExecMsg.IsPriority = true;
					PluginExecMsg.IsFromPlugin = false;
					//Wywolanie funkcji dodawania zakladki do listy ostatnio zamknietych zakladek w AQQ
					PluginLink.CallService(AQQ_FUNCTION_MSGWINDOW,0,(LPARAM)(&PluginExecMsg));
				}
			}
		}
	}
}
//---------------------------------------------------------------------------

//Pobieranie ostatnio zamknietych zakladek do listy
void GetClosedTabs()
{
	//Funkcjonalnosc jest wlaczona
	if(ClosedTabsChk)
	{
		//Odczyt danych z pliku sesji
		TIniFile *Ini = new TIniFile(SessionFileDir);
		TStringList *ClosedTabs = new TStringList;
		Ini->ReadSection("ClosedTabs",ClosedTabs);
		//Pobieranie ilosci zamknietych zakladek
		int TabsCount = ClosedTabs->Count;
		delete ClosedTabs;
		//Sa jakies ostatnio zamkniete zakladki
		if(TabsCount>0)
		{
			//Maks X zdefiniowanych elementow
			if(TabsCount>CountUnCloseTabVal) TabsCount = CountUnCloseTabVal;
			//Usuwanie listy
			ClosedTabsList->Clear();
			ClosedTabsTimeList->Clear();
			//Wczytywanie pozycji na listy
			for(int Count=0;Count<TabsCount;Count++)
			{
				//Pobieranie JID z pliku INI
				UnicodeString JID = Ini->ReadString("ClosedTabs","Tab"+IntToStr(Count+1),"");
				//JID zostal pobrany
				if(!JID.IsEmpty())
				{
					//Zakladka z kontaktem nie jest otwarta
					if(TabsList->IndexOf(JID)==-1)
					{
						ClosedTabsList->Add(JID);
						int ClosedTime = Ini->ReadInteger("ClosedTabs","Tab"+IntToStr(Count+1)+"Timestamp",0);
						ClosedTabsTimeList->Add(ClosedTime);
					}
					//Usuwanie zakladki z pliku INI
					else
					{
						Ini->DeleteKey("ClosedTabs","Tab"+IntToStr(Count+1));
						Ini->DeleteKey("ClosedTabs","Tab"+IntToStr(Count+1)+"Timestamp");
					}
				}
				//Usuwanie pustych danych z pliku INI
				else
				{
					Ini->DeleteKey("ClosedTabs","Tab"+IntToStr(Count+1));
					Ini->DeleteKey("ClosedTabs","Tab"+IntToStr(Count+1)+"Timestamp");
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
	//Funkcjonalnosc jest wlaczona
	if(ClosedTabsChk)
	{
		//Usuniecie danych z pliku sesji
		TIniFile *Ini = new TIniFile(SessionFileDir);
		Ini->EraseSection("ClosedTabs");
		//Pobieranie ilosci zamknietych zakladek
		int TabsCount = ClosedTabsList->Count;
		//Sa jakies ostatnio zamkniete zakladki
		if(TabsCount>0)
		{
			//Maks X zdefiniowanych elementow
			if(TabsCount>CountUnCloseTabVal) TabsCount = CountUnCloseTabVal;
			//Sprawdzenie wszystkich rekordow
			for(int Count=0;Count<TabsCount;Count++)
			{
				//Pole z JID nie jest puste
				if(!ClosedTabsList->Strings[Count].IsEmpty())
				{
					//Zapis danych do pliku sesji
					Ini->WriteString("ClosedTabs","Tab"+IntToStr(Count+1),ClosedTabsList->Strings[Count]);
					//Pole z data zamkniecia jest puste
					if(ClosedTabsTimeList->Strings[Count].IsEmpty()) ClosedTabsTimeList->Strings[Count] = 0;
					//Zapis danych do pliku sesji
					Ini->WriteInteger("ClosedTabs","Tab"+IntToStr(Count+1)+"Timestamp",StrToInt(ClosedTabsTimeList->Strings[Count]));
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
	//Otwieranie zakladki z podanym kontaktem/czatem
	OpenNewTab(JID);
}
//---------------------------------------------------------------------------

//Serwisy elementow ostatnio zamknietych zakladek
INT_PTR __stdcall ServiceClosedTabsItemClear(WPARAM wParam, LPARAM lParam)
{
	EraseClosedTabs();
	return 0;
}
INT_PTR __stdcall ServiceClosedTabsItem0(WPARAM wParam, LPARAM lParam)
{
	GetClosedTabsItem(0);
	return 0;
}
INT_PTR __stdcall ServiceClosedTabsItem1(WPARAM wParam, LPARAM lParam)
{
	GetClosedTabsItem(1);
	return 0;
}
INT_PTR __stdcall ServiceClosedTabsItem2(WPARAM wParam, LPARAM lParam)
{
	GetClosedTabsItem(2);
	return 0;
}
INT_PTR __stdcall ServiceClosedTabsItem3(WPARAM wParam, LPARAM lParam)
{
	GetClosedTabsItem(3);
	return 0;
}
INT_PTR __stdcall ServiceClosedTabsItem4(WPARAM wParam, LPARAM lParam)
{
	GetClosedTabsItem(4);
	return 0;
}
INT_PTR __stdcall ServiceClosedTabsItem5(WPARAM wParam, LPARAM lParam)
{
	GetClosedTabsItem(5);
	return 0;
}
INT_PTR __stdcall ServiceClosedTabsItem6(WPARAM wParam, LPARAM lParam)
{
	GetClosedTabsItem(6);
	return 0;
}
INT_PTR __stdcall ServiceClosedTabsItem7(WPARAM wParam, LPARAM lParam)
{
	GetClosedTabsItem(7);
	return 0;
}
INT_PTR __stdcall ServiceClosedTabsItem8(WPARAM wParam, LPARAM lParam)
{
	GetClosedTabsItem(8);
	return 0;
}
INT_PTR __stdcall ServiceClosedTabsItem9(WPARAM wParam, LPARAM lParam)
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
	//Sa jakies zakladki z niewyslanymi wiadomosciami
	if(MsgCount>0)
	{
		//Maks 5 elementow w popupmenu
		if(MsgCount>5) MsgCount = 5;
		//Usuwanie elementow popupmenu
		for(int Count=0;Count<MsgCount;Count++)
		{
			TPluginAction DestroyUnsentMsgItem;
			ZeroMemory(&DestroyUnsentMsgItem,sizeof(TPluginAction));
			DestroyUnsentMsgItem.cbSize = sizeof(TPluginAction);
			UnicodeString ItemName = "TabKitUnsentMsgItem"+IntToStr(Count);
			DestroyUnsentMsgItem.pszName = ItemName.w_str();
			PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyUnsentMsgItem));
		}
		//Usuwanie elementow do usuwania niewyslanych wiadomosci
		if(FastClearUnsentMsgChk)
		{
			TPluginAction DestroyUnsentMsgItem;
			ZeroMemory(&DestroyUnsentMsgItem,sizeof(TPluginAction));
			DestroyUnsentMsgItem.cbSize = sizeof(TPluginAction);
			UnicodeString ItemName = "TabKitUnsentMsgItem"+IntToStr(MsgCount);
			DestroyUnsentMsgItem.pszName = ItemName.w_str();
			PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyUnsentMsgItem));
			ZeroMemory(&DestroyUnsentMsgItem,sizeof(TPluginAction));
			DestroyUnsentMsgItem.cbSize = sizeof(TPluginAction);
			ItemName = "TabKitUnsentMsgItem"+IntToStr(MsgCount+1);
			DestroyUnsentMsgItem.pszName = ItemName.w_str();
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
void BuildFrmUnsentMsg(bool FixPosition)
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
		//Sa jakies zakladki z niewyslanymi wiadomosciami
		if(MsgCount>0)
		{
			//Natychmiastowe tworzenie przyciskow
			if((!FixPosition)||((!hFrmSend)&&(FixPosition)))
			{
				//Maks 5 elementow w popupmenu
				if(MsgCount>5) MsgCount = 5;
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
					FrmSendUnsentMsgButton.Hint = GetLangStr("UnsentMsg").w_str();
					FrmSendUnsentMsgButton.IconIndex = UNSENTMSG;
					FrmSendUnsentMsgButton.pszPopupName = L"TabKitUnsentMsgPopUp";
					FrmSendUnsentMsgButton.Handle = (int)hFrmSend;
					PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmSendUnsentMsgButton));
				}
				//Tworzenie elementow popupmenu
				for(int Count=0;Count<MsgCount;Count++)
				{
					UnicodeString ItemJID = Messages->Strings[Count];
					if(!ItemJID.IsEmpty())
					{
						TPluginAction BuildUnsentMsgItem;
						ZeroMemory(&BuildUnsentMsgItem,sizeof(TPluginAction));
						BuildUnsentMsgItem.cbSize = sizeof(TPluginAction);
						BuildUnsentMsgItem.IconIndex = GetContactState(ItemJID);
						UnicodeString ItemName = "TabKitUnsentMsgItem"+IntToStr(Count);
						BuildUnsentMsgItem.pszName = ItemName.w_str();
						UnicodeString ServiceName = "sTabKitUnsentMsgItem"+IntToStr(Count);
						BuildUnsentMsgItem.pszService = ServiceName.w_str();
						BuildUnsentMsgItem.pszCaption = GetContactNick(ItemJID).w_str();
						BuildUnsentMsgItem.Position = Count;
						BuildUnsentMsgItem.pszPopupName = L"TabKitUnsentMsgPopUp";
						PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildUnsentMsgItem));
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
					UnicodeString ItemName = "TabKitUnsentMsgItem"+IntToStr(MsgCount);
					BuildUnsentMsgItem.pszName = ItemName.w_str();
					BuildUnsentMsgItem.pszService = L"";
					BuildUnsentMsgItem.pszCaption = L"-";
					BuildUnsentMsgItem.Position = MsgCount;
					BuildUnsentMsgItem.pszPopupName = L"TabKitUnsentMsgPopUp";
					PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildUnsentMsgItem));
					//Tworzenie elementu czyszczenia
					ZeroMemory(&BuildUnsentMsgItem,sizeof(TPluginAction));
					BuildUnsentMsgItem.cbSize = sizeof(TPluginAction);
					BuildUnsentMsgItem.IconIndex = -1;
					ItemName = "TabKitUnsentMsgItem"+IntToStr(MsgCount+1);
					BuildUnsentMsgItem.pszName = ItemName.w_str();
					BuildUnsentMsgItem.pszService = L"sTabKitUnsentMsgItemClear";
					BuildUnsentMsgItem.pszCaption = GetLangStr("Clear").w_str();
					BuildUnsentMsgItem.Position = MsgCount+1;
					BuildUnsentMsgItem.pszPopupName = L"TabKitUnsentMsgPopUp";
					PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildUnsentMsgItem));
				}
			}
			//Aktualizacja pozycji wszystkich przyciskow w oknie rozmowy
			else FixButtonsPosition();
		}
		delete Messages;
	}
}
//---------------------------------------------------------------------------

//Aktualizacja popupmenu niewyslanych wiadomosci
void RebuildFrmUnsentMsgPopupmenu()
{
	//Pobieranie ilosci zamknietych zakladek
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *Messages = new TStringList;
	Ini->ReadSection("Messages",Messages);
	delete Ini;
	//Pobieranie ilosci niewyslanych wiadomosci
	int MsgCount = Messages->Count;
	//Sa jakies zakladki z niewyslanymi wiadomosciami
	if(MsgCount>0)
	{
		//Maks 5 elementow w popupmenu
		if(MsgCount>5) MsgCount = 5;
		//Aktualizacja elementow popupmenu
		for(int Count=0;Count<MsgCount;Count++)
		{
			UnicodeString ItemJID = Messages->Strings[Count];
			TPluginActionEdit RebuildUnsentMsgItem;
			ZeroMemory(&RebuildUnsentMsgItem,sizeof(TPluginActionEdit));
			RebuildUnsentMsgItem.cbSize = sizeof(TPluginActionEdit);
			RebuildUnsentMsgItem.IconIndex = GetContactState(ItemJID);
			UnicodeString ItemName = "TabKitUnsentMsgItem"+IntToStr(Count);
			RebuildUnsentMsgItem.pszName = ItemName.w_str();
			RebuildUnsentMsgItem.Caption = GetContactNick(ItemJID).w_str();
			RebuildUnsentMsgItem.Enabled = true;
			RebuildUnsentMsgItem.Visible = true;
			RebuildUnsentMsgItem.Checked = false;
			PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&RebuildUnsentMsgItem));
		}
	}
	delete Messages;
}
//---------------------------------------------------------------------------

//Usuwanie listy niewyslanych wiadomosci
void EraseUnsentMsg()
{
	//Usuwanie interfejsu
	DestroyFrmUnsentMsg();
	//Usuwanie danych z pliku sesji
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
	//Otwieranie zakladki z podanym kontaktem/czatem
	OpenNewTab(JID);
}
//---------------------------------------------------------------------------

//Serwisy elementow niewysylanych wiadomosci
INT_PTR __stdcall ServiceUnsentMsgItemClear(WPARAM wParam, LPARAM lParam)
{
	EraseUnsentMsg();
	return 0;
}
INT_PTR __stdcall ServiceUnsentMsgItem0(WPARAM wParam, LPARAM lParam)
{
	GetUnsentMsgItem(0);
	return 0;
}
INT_PTR __stdcall ServiceUnsentMsgItem1(WPARAM wParam, LPARAM lParam)
{
	GetUnsentMsgItem(1);
	return 0;
}
INT_PTR __stdcall ServiceUnsentMsgItem2(WPARAM wParam, LPARAM lParam)
{
	GetUnsentMsgItem(2);
	return 0;
}
INT_PTR __stdcall ServiceUnsentMsgItem3(WPARAM wParam, LPARAM lParam)
{
	GetUnsentMsgItem(3);
	return 0;
}
INT_PTR __stdcall ServiceUnsentMsgItem4(WPARAM wParam, LPARAM lParam)
{
	GetUnsentMsgItem(4);
	return 0;
}
//---------------------------------------------------------------------------

//Sprawdzanie niewyslanych wiadomosci
void GetUnsentMsg()
{
	//Funkcjonalnosc jest wlaczona
	if((UnsentMsgChk)&&(InfoUnsentMsgChk))
	{
		//Odczyt danych z pliku sesji
		TIniFile *Ini = new TIniFile(SessionFileDir);
		TStringList *Messages = new TStringList;
		Ini->ReadSection("Messages",Messages);
		//Pobieranie ilosci niewyslanych wiadomosci
		int MsgCount = Messages->Count;
		//Sa jakies zakladki z niewyslanymi wiadomosciami
		if(MsgCount>0)
		{
			//Informacja o ilosci niewyslanych wiadomosci
			UnicodeString Hint;
			if(MsgCount==1) Hint = GetLangStr("UnsentMsgHint1");
			else if((MsgCount>1)&&(MsgCount<5)) Hint = GetLangStr("UnsentMsgHint2");
			else Hint = GetLangStr("UnsentMsgHint3");
			Hint = StringReplace(Hint, "CC_COUNT", IntToStr(MsgCount), TReplaceFlags());
			//Informacja w chmurce
			if(CloudUnsentMsgChk)
			{
				//Informacja podstawowa
				TPluginShowInfo PluginShowInfo;
				PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
				PluginShowInfo.Event = tmePseudoMsgCap;
				PluginShowInfo.Text = Hint.w_str();
				PluginShowInfo.ImagePath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,21,0);
				PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
				PluginShowInfo.ActionID = L"sTabKitUnsentMsgShowAllItem";
				PluginShowInfo.Tick = 0;
				PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
				//Szczegolowa informacja
				if(DetailedCloudUnsentMsgChk)
				{
					for(int Count=0;Count<MsgCount;Count++)
					{
						UnicodeString JID = Messages->Strings[Count];
						UnicodeString Body = DecodeBase64(Ini->ReadString("Messages", JID, ""));
						if(Body.Length()>25) Body = Body.SetLength(25) + "...";
						PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
						PluginShowInfo.Event = tmeInfo;
						PluginShowInfo.Text = (GetContactNick(JID) + "\r\n" + Body).w_str();
						PluginShowInfo.ImagePath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0);
						PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
						PluginShowInfo.Tick = 0;
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
	//Komunikator nie jest zabezpieczony
	if(!SecureMode)
	{
		//Odczyt danych z pliku sesji
		TIniFile *Ini = new TIniFile(SessionFileDir);
		TStringList *Messages = new TStringList;
		Ini->ReadSection("Messages",Messages);
		delete Ini;
		//Pobieranie ilosci niewyslanych wiadomosci
		int MsgCount = Messages->Count;
		//Sa jakies niewyslane wiadomosci
		if(MsgCount>0)
		{
			for(int Count=0;Count<MsgCount;Count++)
			{
				//Pobieranie JID kontaktu
				UnicodeString JID = Messages->Strings[Count];
				//Otwieranie zakladki z podanym kontaktem/czatem
				OpenNewTab(JID);
			}
		}
		delete Messages;
		return true;
	}
	//Komunikator jest zabezpieczony
	else
	{
		PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aSecure");
		return false;
	}
}
//---------------------------------------------------------------------------

//Otwieranie wszystkich niewyslanych wiadomosci
INT_PTR __stdcall ServiceUnsentMsgShowAllItem(WPARAM wParam, LPARAM lParam)
{
	//Otwarcie okna rozmowy wraz z niewyslanymi wiadomosciami
	if((ShowUnsentMsg())&&(hSettingsForm))
	{
		//Ukrycie ikonki w obszarze powiadomien
		hSettingsForm->UnsentMsgTrayIcon->Visible = false;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Usuwanie elementu do przypinania/odpiniania zakladek oraz pokazywania/ukrywania caption zakladki
void DestroyClipTab()
{
	//Element przypinania zakladki
	TPluginAction ClipTabItem;
	ZeroMemory(&ClipTabItem,sizeof(TPluginAction));
	ClipTabItem.cbSize = sizeof(TPluginAction);
	ClipTabItem.pszName = L"TabKitClipTabItem";
	ClipTabItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&ClipTabItem));
	//Element ukrywania caption zakladki
	TPluginAction ClipTabCaptionItem;
	ZeroMemory(&ClipTabCaptionItem,sizeof(TPluginAction));
	ClipTabCaptionItem.cbSize = sizeof(TPluginAction);
	ClipTabCaptionItem.pszName = L"TabKitClipTabCaptionItem";
	ClipTabCaptionItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&ClipTabCaptionItem));
}
//---------------------------------------------------------------------------

//Tworzenie elementu przypinania/odpiniania zakladek oraz pokazywania/ukrywania caption zakladki
void BuildClipTab()
{
	//Okno rozmowy jest otwarte
	if((ClipTabsChk)&&(hFrmSend))
	{
		//Element przypinania zakladki
		TPluginAction ClipTabItem;
		ZeroMemory(&ClipTabItem,sizeof(TPluginAction));
		ClipTabItem.cbSize = sizeof(TPluginAction);
		ClipTabItem.pszName = L"TabKitClipTabItem";
		ClipTabItem.pszCaption = GetLangStr("ClipTab").w_str();
		ClipTabItem.Position = 1;
		ClipTabItem.IconIndex = -1;
		ClipTabItem.pszService = L"sTabKitClipTabItem";
		ClipTabItem.pszPopupName = L"popTab";
		ClipTabItem.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&ClipTabItem));
		//Element ukrywania caption zakladki
		TPluginAction ClipTabCaptionItem;
		ZeroMemory(&ClipTabCaptionItem,sizeof(TPluginAction));
		ClipTabCaptionItem.cbSize = sizeof(TPluginAction);
		ClipTabCaptionItem.pszName = L"TabKitClipTabCaptionItem";
		ClipTabCaptionItem.pszCaption = GetLangStr("HideCaption").w_str();
		ClipTabCaptionItem.Position = 2;
		ClipTabCaptionItem.IconIndex = -1;
		ClipTabCaptionItem.pszService = L"sTabKitClipTabCaptionItem";
		ClipTabCaptionItem.pszPopupName = L"popTab";
		ClipTabCaptionItem.Checked = true;
		ClipTabCaptionItem.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&ClipTabCaptionItem));
	}
}
//---------------------------------------------------------------------------

//Serwis do przypinania/odpiniania zakladek
INT_PTR __stdcall ServiceClipTabItem(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie JID kontaktu z menu popTab
	UnicodeString ClipTab = PopupTab;
	//Zakladka nie jest przypieta
	if(ClipTabsList->IndexOf(ClipTab)==-1)
	{
		//Dodanie nowej zakladki do listy przypietych zakladek
		ClipTabsList->Add(ClipTab);
		//Przebudowanie listy przypietych zakladek
		RebuildClipTabs();
		//Zmiana miejsca zakladki
		ChangeClipTabPos(PopupTab,PopupTabEx);
		//Odswiezenie wszystkich zakladek
		RefreshTabs();
	}
	//Zakladka jest juz przypieta
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
		//Pobieranie ilosci zakladek
		TPluginTriple PluginTriple;
		ZeroMemory(&PluginTriple, sizeof(TPluginTriple));
		PluginTriple.cbSize = sizeof(TPluginTriple);
		PluginTriple.Handle1 = (int)hFrmSend;
		int TabsCount = PluginLink.CallService(AQQ_FUNCTION_TABCOUNT,(WPARAM)&PluginTriple,0);
		//Zmiana miejsca zakladki
		if(TabsCount)
		{
			//Pobieranie aktualnej pozycji zakladki
			int Index = GetTabIndex(ClipTab);
			//Zmiana miejsca zakladki
			if(Index!=(TabsCount-1))
			{
				ZeroMemory(&PluginTriple, sizeof(TPluginTriple));
				PluginTriple.cbSize = sizeof(TPluginTriple);
				PluginTriple.Handle1 = (int)hFrmSend;
				PluginTriple.Param1 = Index;
				PluginTriple.Param2 = TabsCount-1;
				PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
			}
		}
		//Odswiezenie wszystkich zakladek
		RefreshTabs();
	}

	return 0;
}
//---------------------------------------------------------------------------

//Serwis do pokazywania/ukrywania nazwy przypietych zakladek
INT_PTR __stdcall ServiceClipTabCaptionItem(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie JID kontaktu z menu popTab
	UnicodeString ClipTab = PopupTab;
	//Odczyt stanu z pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	//Nazwa zakladki widoczna
	if(Ini->ValueExists("ClipTabsEx",ClipTab))
	{
		//Zapisanie nowego stanu
		Ini->DeleteKey("ClipTabsEx",ClipTab);
	}
	//Nazwa zakladki ukryta
	else
	{
		//Zapisanie nowego stanu
		Ini->WriteBool("ClipTabsEx",ClipTab,true);
	}
	//Zamkniecie pliku sesji
	delete Ini;
	//Przebudowanie listy przypietych zakladek
	RebuildClipTabs();
	//Zmiana miejsca zakladki
	ChangeClipTabPos(PopupTab,PopupTabEx);
	//Odswiezenie wszystkich zakladek
	RefreshTabs();

	return 0;
}
//---------------------------------------------------------------------------

//Usuniecie wszystkich zdefiniowanych przypietych zakladek
void EraseClipTabs()
{
	//Usuniecie wszystkich zakladek z listy przypietych zakladek
	ClipTabsList->Clear();
	//Usuniecie danych z pliku sesji
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
	//Odczyt danych sesji
	TStringList *ClipIcons = new TStringList;
	ClipTabsIconList->ReadSection("ClipTabsIcon",ClipIcons);
	//Zostaly wczytane jakies ikony do intefejsu
	if(ClipIcons->Count)
	{
		//Wyladowanie wszystkich ikon z intefejsu
		for(int Count=0;Count<ClipIcons->Count;Count++)
		{
			UnicodeString JID = ClipIcons->Strings[Count];
			int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID,0);
			if(Icon) PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)Icon);
		}
		//Usuniecie zapamietanych indeksow ikon
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
	//Wczytanie pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *ClipTabs = new TStringList;
	Ini->ReadSection("ClipTabs",ClipTabs);
	int TabsCount = ClipTabs->Count;
	delete ClipTabs;
	//Sa jakies przypiete zakladki
	if(TabsCount>0)
	{
		//Odczyt kolejno wszystkich rekordow
		for(int Count=0;Count<TabsCount;Count++)
		{
			//Odczytanie JID przypietej zakladki
			UnicodeString ClipTab = Ini->ReadString("ClipTabs", "Tab"+IntToStr(Count+1),"");
			//Dodanie zakladki do listy przypietych zakladek
			if(!ClipTab.IsEmpty()) ClipTabsList->Add(ClipTab);
		}
	}
	delete Ini;
}
//---------------------------------------------------------------------------

//Chmurka informacyjna dotyczaca ulubionych zakladek
void ShowFavouritesTabsInfo(UnicodeString Text)
{
	//Generowanie ID dla chmurek
	DWORD TickID = GetTickCount();
	//Naglowek chmurki
	TPluginShowInfo PluginShowInfo;
	PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
	PluginShowInfo.Event = tmeMsgCap;
	PluginShowInfo.Text = GetLangStr("FavTabs").w_str();
	PluginShowInfo.ImagePath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,125,0);
	PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
	PluginShowInfo.ActionID = L"";
	PluginShowInfo.Tick = TickID;
	PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
	//Tekst chmurki
	PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
	PluginShowInfo.Event = tmeInfo;
	PluginShowInfo.Text = Text.w_str();
	PluginShowInfo.ImagePath = L"";
	PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
	PluginShowInfo.ActionID = L"";
	PluginShowInfo.Tick = TickID;
	PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
}
//---------------------------------------------------------------------------

//Usuwanie interfejsu dodwania/usuwania ulubionej zakladki
void DestroyFrmSendFavouriteTab()
{
	//Element dowania/usuwania ulubionej zakladki
	TPluginAction FavouriteTabItem;
	ZeroMemory(&FavouriteTabItem,sizeof(TPluginAction));
	FavouriteTabItem.cbSize = sizeof(TPluginAction);
	FavouriteTabItem.pszName = L"TabKitFrmSendFavouriteTabItem";
	FavouriteTabItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&FavouriteTabItem));
}
//---------------------------------------------------------------------------
void DestroyFrmMainFavouriteTab()
{
	//Element dowania/usuwania ulubionej zakladki
	TPluginAction FavouriteTabItem;
	ZeroMemory(&FavouriteTabItem,sizeof(TPluginAction));
	FavouriteTabItem.cbSize = sizeof(TPluginAction);
	FavouriteTabItem.pszName = L"TabKitFrmMainFavouriteTabItem";
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&FavouriteTabItem));
}
//---------------------------------------------------------------------------

//Usuwanie interfejsu szybkiego dostepu do ulubionych zakladek
void DestroyFavouritesTabs()
{
	//Pobieranie ilosci ulubionych zakladek
	int TabsCount = FavouritesTabsList->Count;
	//Sa jakies ulubione zakladki
	if(TabsCount>0)
	{
		//Usuwanie elementow popupmenu
		for(int Count=0;Count<TabsCount;Count++)
		{
			TPluginAction DestroyFavouritesTabsItem;
			ZeroMemory(&DestroyFavouritesTabsItem,sizeof(TPluginAction));
			DestroyFavouritesTabsItem.cbSize = sizeof(TPluginAction);
			UnicodeString ItemName = "TabKitFavouritesTabsItem"+IntToStr(Count);
			DestroyFavouritesTabsItem.pszName = ItemName.w_str();
			PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyFavouritesTabsItem));
		}
		//Usuwanie buttona w oknie kontaktow
		TPluginAction FrmMainFavouritesTabsButton;
		ZeroMemory(&FrmMainFavouritesTabsButton,sizeof(TPluginAction));
		FrmMainFavouritesTabsButton.cbSize = sizeof(TPluginAction);
		FrmMainFavouritesTabsButton.pszName = L"TabKitFrmMainFavouritesTabsButton";
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmMainFavouritesTabsButton));
		//Usuwanie buttona w oknie rozmowy
		TPluginAction FrmSendFavouritesTabsButton;
		ZeroMemory(&FrmSendFavouritesTabsButton,sizeof(TPluginAction));
		FrmSendFavouritesTabsButton.cbSize = sizeof(TPluginAction);
		FrmSendFavouritesTabsButton.pszName = L"TabKitFrmSendFavouritesTabsButton";
		FrmSendFavouritesTabsButton.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmSendFavouritesTabsButton));
	}
}
//---------------------------------------------------------------------------

//Tworzenie interfejsu dodawania/usuwania ulubionej zakladki
void BuildFrmSendFavouriteTab()
{
	//Okno rozmowy jest otwarte
	if((FavouritesTabsChk)&&(hFrmSend))
	{
		//Element dowania/usuwania ulubionej zakladki
		TPluginAction FavouriteTabItem;
		ZeroMemory(&FavouriteTabItem,sizeof(TPluginAction));
		FavouriteTabItem.cbSize = sizeof(TPluginAction);
		FavouriteTabItem.pszName = L"TabKitFrmSendFavouriteTabItem";
		FavouriteTabItem.pszCaption = GetLangStr("AddToFav").w_str();
		FavouriteTabItem.Position = 1;
		FavouriteTabItem.IconIndex = 125;
		FavouriteTabItem.pszService = L"sTabKitFavouriteTabItem";
		FavouriteTabItem.pszPopupName = L"popTab";
		FavouriteTabItem.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&FavouriteTabItem));
	}
}
//---------------------------------------------------------------------------
void BuildFrmMainFavouriteTab()
{
	if(FavouritesTabsChk)
	{
		//Ustalanie pozycji elementu "Wizytowka"
		TPluginItemDescriber PluginItemDescriber;
		PluginItemDescriber.cbSize = sizeof(TPluginItemDescriber);
		PluginItemDescriber.FormHandle = 0;
		PluginItemDescriber.ParentName = L"muItem";
		PluginItemDescriber.Name = L"muProfile";
		PPluginAction Action = (PPluginAction)PluginLink.CallService(AQQ_CONTROLS_GETPOPUPMENUITEM,0,(LPARAM)(&PluginItemDescriber));
		int Position = Action->Position;
		//Element dowania/usuwania ulubionej zakladki
		TPluginAction FavouriteTabItem;
		ZeroMemory(&FavouriteTabItem,sizeof(TPluginAction));
		FavouriteTabItem.cbSize = sizeof(TPluginAction);
		FavouriteTabItem.pszName = L"TabKitFrmMainFavouriteTabItem";
		FavouriteTabItem.pszCaption = GetLangStr("AddToFav").w_str();
		FavouriteTabItem.Position = Position + 1;
		FavouriteTabItem.IconIndex = 125;
		FavouriteTabItem.pszService = L"sTabKitFavouriteTabItem";
		FavouriteTabItem.pszPopupName = L"muItem";
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&FavouriteTabItem));
	}
}
//---------------------------------------------------------------------------

//Tworzenie interfejsu szybkiego dostepu do ulubionych zakladek
void BuildFavouritesTabs(bool FixPosition)
{
	//Interfejs ma byc stworzony
	if((FavouritesTabsChk)&&(FastAccessFavouritesTabsChk))
	{
		//Pobieranie ilosci ulubionych zakladek
		int TabsCount = FavouritesTabsList->Count;
		//Sa jakies ulubione zakladki
		if(TabsCount>0)
		{
			//Natychmiastowe tworzenie przyciskow
			if((!FixPosition)||((!hFrmSend)&&(FixPosition)))
			{
				//Tworzenie buttona w oknie kontaktow
				if(FrmMainFastAccessFavouritesTabsChk)
				{
					TPluginAction FrmMainFavouritesTabsButton;
					ZeroMemory(&FrmMainFavouritesTabsButton,sizeof(TPluginAction));
					FrmMainFavouritesTabsButton.cbSize = sizeof(TPluginAction);
					FrmMainFavouritesTabsButton.pszName = L"TabKitFrmMainFavouritesTabsButton";
					FrmMainFavouritesTabsButton.Position = 999;
					FrmMainFavouritesTabsButton.IconIndex = 125;
					FrmMainFavouritesTabsButton.pszPopupName = L"TabKitFavouritesTabsPopUp";
					PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmMainFavouritesTabsButton));
				}
				//Tworzenie buttona w oknie rozmowy
				if((hFrmSend)&&(FrmSendFastAccessFavouritesTabsChk))
				{
					//Tworzenie buttona w oknie rozmowy
					TPluginAction FrmSendFavouritesTabsButton;
					ZeroMemory(&FrmSendFavouritesTabsButton,sizeof(TPluginAction));
					FrmSendFavouritesTabsButton.cbSize = sizeof(TPluginAction);
					FrmSendFavouritesTabsButton.pszName = L"TabKitFrmSendFavouritesTabsButton";
					FrmSendFavouritesTabsButton.Hint = GetLangStr("FavTabs").w_str();
					FrmSendFavouritesTabsButton.IconIndex = 125;
					FrmSendFavouritesTabsButton.pszPopupName = L"TabKitFavouritesTabsPopUp";
					FrmSendFavouritesTabsButton.Handle = (int)hFrmSend;
					PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmSendFavouritesTabsButton));
				}
				//Tworzenie elementow popupmenu
				for(int Count=0;Count<TabsCount;Count++)
				{
					UnicodeString ItemJID = FavouritesTabsList->Strings[Count];
					if(!ItemJID.IsEmpty())
					{
						TPluginAction BuildFavouritesTabsItem;
						ZeroMemory(&BuildFavouritesTabsItem,sizeof(TPluginAction));
						BuildFavouritesTabsItem.cbSize = sizeof(TPluginAction);
						BuildFavouritesTabsItem.IconIndex = GetContactState(ItemJID);
						UnicodeString ItemName = "TabKitFavouritesTabsItem"+IntToStr(Count);
						BuildFavouritesTabsItem.pszName = ItemName.w_str();
						UnicodeString ServiceName = "sTabKitFavouritesTabsItem"+IntToStr(Count);
						BuildFavouritesTabsItem.pszService = ServiceName.w_str();
						BuildFavouritesTabsItem.pszCaption = GetContactNick(ItemJID).w_str();
						BuildFavouritesTabsItem.Position = Count;
						BuildFavouritesTabsItem.pszPopupName = L"TabKitFavouritesTabsPopUp";
						PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFavouritesTabsItem));
					}
				}
			}
			//Aktualizacja pozycji wszystkich przyciskow w oknie rozmowy
			else FixButtonsPosition();
		}
	}
}
//---------------------------------------------------------------------------

//Aktualizacja popupmenu ulubionych zakladek
void RebuildFavouritesTabsPopupmenu()
{
	//Pobieranie ilosci ulubionych zakladek
	int TabsCount = FavouritesTabsList->Count;
	//Sa jakies ulubione zakladki
	if(TabsCount>0)
	{
		//Aktualizacja elementow popupmenu
		for(int Count=0;Count<TabsCount;Count++)
		{
			UnicodeString ItemJID = FavouritesTabsList->Strings[Count];
			TPluginActionEdit RebuildFavouritesTabsItem;
			ZeroMemory(&RebuildFavouritesTabsItem,sizeof(TPluginActionEdit));
			RebuildFavouritesTabsItem.cbSize = sizeof(TPluginActionEdit);
			RebuildFavouritesTabsItem.IconIndex = GetContactState(ItemJID);
			UnicodeString ItemName = "TabKitFavouritesTabsItem"+IntToStr(Count);
			RebuildFavouritesTabsItem.pszName = ItemName.w_str();
			RebuildFavouritesTabsItem.Caption = GetContactNick(ItemJID).w_str();
			RebuildFavouritesTabsItem.Enabled = true;
			RebuildFavouritesTabsItem.Visible = true;
			RebuildFavouritesTabsItem.Checked = false;
			PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&RebuildFavouritesTabsItem));
		}
	}
}
//---------------------------------------------------------------------------

//Odczyt ulubionych zakladek
void LoadFavouritesTabs()
{
	//Usuniecie wlisty ulubionych zakladek
	FavouritesTabsList->Clear();
	//Wczytanie pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *FavouritesTabs = new TStringList;
	Ini->ReadSection("FavouritesTabs",FavouritesTabs);
	int TabsCount = FavouritesTabs->Count;
	delete FavouritesTabs;
	//Sa jakies przypiete zakladki
	if(TabsCount>0)
	{
		//Odczyt kolejno wszystkich rekordow
		for(int Count=0;Count<TabsCount;Count++)
		{
			//Odczytanie JID przypietej zakladki
			UnicodeString FavouriteTab = Ini->ReadString("FavouritesTabs", "Tab"+IntToStr(Count+1),"");
			//Dodanie zakladki do listy przypietych zakladek
			if(!FavouriteTab.IsEmpty()) FavouritesTabsList->Add(FavouriteTab);
		}
	}
	delete Ini;
}
//---------------------------------------------------------------------------

//Zapis ulubionych zakladek
void SaveFavouritesTabs()
{
	//Zapisanie ponownie listy ulubionych zakladek w pliku sesji
	TIniFile *Ini = new TIniFile(SessionFileDir);
	Ini->EraseSection("FavouritesTabs");
	if(FavouritesTabsList->Count>0)
	{
		for(int Count=0;Count<FavouritesTabsList->Count;Count++)
			Ini->WriteString("FavouritesTabs", "Tab"+IntToStr(Count+1), FavouritesTabsList->Strings[Count]);
	}
	delete Ini;
}
//---------------------------------------------------------------------------

//Serwis dodawania/usuwania ulubionej zakladki
INT_PTR __stdcall ServiceFavouriteTabItem(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie JID kontaktu z menu popTab
	UnicodeString FavouriteTab = PopupTab;
	//Zakladka nie jest dodana do ulubionych
	if(FavouritesTabsList->IndexOf(FavouriteTab)==-1)
	{
		//Maksymalnie 10 ulubionych zakladek
		if(FavouritesTabsList->Count<10)
		{
			//Usuwanie elementu z lista ulubionych zakladek
			DestroyFavouritesTabs();
			//Dodanie nowej zakladki do ulubionych zakladek
			FavouritesTabsList->Add(FavouriteTab);
			//Zapis ulubionych zakladek
			SaveFavouritesTabs();
			//Ponowne tworzenie elementu z lista ulubionych zakladek
			BuildFavouritesTabs(true);
		}
		//Osiagnieto maksymalna ilosc ulubionych zakladek
		else ShowFavouritesTabsInfo(GetLangStr("MaxFavTabs"));
	}
	//Zaklada jest dodana do ulubionych
	else
	{
		//Usuwanie elementu z lista ulubionych zakladek
		DestroyFavouritesTabs();
		//Usuniecie zakladki z ulubionych zakladek
		FavouritesTabsList->Delete(FavouritesTabsList->IndexOf(FavouriteTab));
		//Zapis ulubionych zakladek
		SaveFavouritesTabs();
		//Ponowne tworzenie elementu z lista ulubionych zakladek
		BuildFavouritesTabs(true);
	}
	//Odswiezenie listy ulubionych zakladek w ustawieniach
	if(hSettingsForm) hSettingsForm->aReloadFavouritesTabs->Execute();

	return 0;
}
//---------------------------------------------------------------------------

//Otwieranie ulubionej zakladki
void GetFavouritesTabsItem(int Item)
{
	//Pobranie JID zakladki do przywrocenia
	UnicodeString JID = FavouritesTabsList->Strings[Item];
	//Otwieranie zakladki z podanym kontaktem/czatem
	OpenNewTab(JID);
}
//---------------------------------------------------------------------------

//Serwisy elementow ostatnio zamknietych zakladek
INT_PTR __stdcall ServiceFavouritesTabsItem0(WPARAM wParam, LPARAM lParam)
{
	GetFavouritesTabsItem(0);
	return 0;
}
INT_PTR __stdcall ServiceFavouritesTabsItem1(WPARAM wParam, LPARAM lParam)
{
	GetFavouritesTabsItem(1);
	return 0;
}
INT_PTR __stdcall ServiceFavouritesTabsItem2(WPARAM wParam, LPARAM lParam)
{
	GetFavouritesTabsItem(2);
	return 0;
}
INT_PTR __stdcall ServiceFavouritesTabsItem3(WPARAM wParam, LPARAM lParam)
{
	GetFavouritesTabsItem(3);
	return 0;
}
INT_PTR __stdcall ServiceFavouritesTabsItem4(WPARAM wParam, LPARAM lParam)
{
	GetFavouritesTabsItem(4);
	return 0;
}
INT_PTR __stdcall ServiceFavouritesTabsItem5(WPARAM wParam, LPARAM lParam)
{
	GetFavouritesTabsItem(5);
	return 0;
}
INT_PTR __stdcall ServiceFavouritesTabsItem6(WPARAM wParam, LPARAM lParam)
{
	GetFavouritesTabsItem(6);
	return 0;
}
INT_PTR __stdcall ServiceFavouritesTabsItem7(WPARAM wParam, LPARAM lParam)
{
	GetFavouritesTabsItem(7);
	return 0;
}
INT_PTR __stdcall ServiceFavouritesTabsItem8(WPARAM wParam, LPARAM lParam)
{
	GetFavouritesTabsItem(8);
	return 0;
}
INT_PTR __stdcall ServiceFavouritesTabsItem9(WPARAM wParam, LPARAM lParam)
{
	GetFavouritesTabsItem(9);
	return 0;
}
//---------------------------------------------------------------------------

//Serwis do wklejania tekstu ze schowka jako cytat
INT_PTR __stdcall ServiceQuickQuoteItem(WPARAM wParam, LPARAM lParam)
{
	//Schowek jest dostepny
	if(OpenClipboard(NULL))
	{
		//Pobieranie tekstu ze schowka
		HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
		UnicodeString ClipboardText = (wchar_t*)GlobalLock(hClipboardData);
		GlobalUnlock(hClipboardData);
		CloseClipboard();
		//Pobrano tekst ze schowka
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
				Text.Delete(SelPos.cpMin+1,SelPos.cpMax-SelPos.cpMin);
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
INT_PTR __stdcall ServiceCollapseImagesItem(WPARAM wParam, LPARAM lParam)
{
	//Otwarcie pliku w domyslnym programie
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
	//Funkcjonalnosc jest wlaczona, okno rozmowy otwarte
	if((hFrmSend)&&(StayOnTopChk))
	{
		TPluginAction StayOnTopItem;
		ZeroMemory(&StayOnTopItem,sizeof(TPluginAction));
		StayOnTopItem.cbSize = sizeof(TPluginAction);
		StayOnTopItem.pszName = L"TabKitStayOnTopItem";
		StayOnTopItem.pszCaption = GetLangStr("HoldOnTop").w_str();
		StayOnTopItem.Hint = GetLangStr("HoldOnTop").w_str();
		//Okno ustawione na wierzchu
		if(StayOnTopStatus)
		{
			StayOnTopItem.IconIndex = STAYONTOP_ON;
			//Tworzenie timera ustawiajacego okno rozmowy na wierzchu
			if(!FrmSendSlideChk) SetTimer(hTimerFrm,TIMER_STAYONTOP,100,(TIMERPROC)TimerFrmProc);
		}
		//Okno nie ustawione na wierzchu
		else StayOnTopItem.IconIndex = STAYONTOP_OFF;
		StayOnTopItem.pszService = L"sTabKitStayOnTopItem";
		StayOnTopItem.Handle = (int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&StayOnTopItem));
	}
}
//---------------------------------------------------------------------------

//Serwis trzymania okna rozmowy na wierzchu
INT_PTR __stdcall ServiceStayOnTopItem(WPARAM wParam, LPARAM lParam)
{
	//Status funkcji StayOnTop
	StayOnTopStatus = !StayOnTopStatus;
	//Aktualizacja przycisku
	TPluginAction StayOnTopItem;
	ZeroMemory(&StayOnTopItem,sizeof(TPluginAction));
	StayOnTopItem.cbSize = sizeof(TPluginAction);
	StayOnTopItem.pszName = L"TabKitStayOnTopItem";
	StayOnTopItem.pszCaption = GetLangStr("HoldOnTop").w_str();
	StayOnTopItem.Hint = GetLangStr("HoldOnTop").w_str();
	if(StayOnTopStatus) StayOnTopItem.IconIndex = STAYONTOP_ON;
	else StayOnTopItem.IconIndex = STAYONTOP_OFF;
	StayOnTopItem.Handle = (int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&StayOnTopItem));
	//Ustawianie okna na wierzchu
	if(StayOnTopStatus)
	{
		//Okno rozmowy na wierzchu
		if((!FrmSendSlideChk)||((FrmSendSlideChk)&&(FrmSendSlideHideMode==2))) SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	//Przywracanie okna do normalnosci
	else
	{
		//Przywrocenie "normalnosci" okna
		if((!FrmSendSlideChk)||((FrmSendSlideChk)&&(FrmSendSlideHideMode==2))) SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	//Tymczasowa blokada chowania/pokazywania okna rozmowy
	if(FrmSendSlideChk) FrmSendBlockSlide = StayOnTopStatus;

	return 0;
}
//---------------------------------------------------------------------------

//Serwis szybkiego dostepu do ustawien wtyczki
INT_PTR __stdcall ServiceTabKitFastSettingsItem(WPARAM wParam, LPARAM lParam)
{
	//Otwieranie okna ustawien wtyczki
	OpenPluginSettings();

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
	//Kompozycja nie jest zmieniana / komunikator nie jest zamykany
	if((uMsg==WM_TIMER)&&(!ThemeChanging)&&(!ForceUnloadExecuted))
	{
		//Szukanie paska informacyjnego
		if(wParam==TIMER_FIND_STATUSBAR)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FIND_STATUSBAR);
			//Szukanie paska informacyjnego
			hStatusBar = FindWindowEx(hFrmSend,NULL,L"TsStatusBar",NULL);
			//Ponowne wlaczenie timera
			if(!hStatusBar) SetTimer(hTimerFrm,TIMER_FIND_STATUSBAR,500,(TIMERPROC)TimerFrmProc);
		}
		//Szukanie paska narzedzi
		else if(wParam==TIMER_FIND_TOOLBAR)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FIND_TOOLBAR);
			//Szukanie paska narzedzi
			if(!hToolBar) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
			//Ponowne timera
			if(!hToolBar) SetTimer(hTimerFrm,TIMER_FIND_TOOLBAR,500,(TIMERPROC)TimerFrmProc);
			//Ukrywanie paska narzedzi
			else if(HideToolBarChk) HideToolBar();
		}
		//Pokazywanie paska narzedzi
		else if(wParam==TIMER_SHOW_TOOLBAR)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_SHOW_TOOLBAR);
			//Pobieranie aktywnej kontrolki w ktorej znajduje sie kursor
			HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
			//Jezeli okno rozmowy jest aktywne
			if((GetForegroundWindow()==hFrmSend)&&((hCurActiveFrm==hFrmSend)||(IsChild(hFrmSend,hCurActiveFrm))))
				//Pokazanie paska narzedzi
				ShowToolBar();
		}
		//Szukanie paska zakladek
		else if(wParam==TIMER_FIND_TABSBAR)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FIND_TABSBAR);
			//Szukanie paska zakladek
			EnumChildWindows(hFrmSend,(WNDENUMPROC)FindTabsBar,0);
			//Ponowne wlaczenie timera
			if((!hTabsBar)||(!hScrollTabButton[0])||(!hScrollTabButton[1]))
				SetTimer(hTimerFrm,TIMER_FIND_TABSBAR,500,(TIMERPROC)TimerFrmProc);
		}
		//Ustawienie okna rozmowy na wierzchu
		else if(wParam==TIMER_STAYONTOP)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_STAYONTOP);
			//Ustawienie okna na wierzchu
			SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}
		//Wylaczanie modalnosci oknien
		else if(wParam==TIMER_DISABLE_MODAL)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_DISABLE_MODAL);
			//Wylaczenie modalnosci dla okna kontatkow
			EnableWindow(hFrmMain,true);
			//Wylaczenie modalnosci dla okna rozmowy
			EnableWindow(hFrmSend,true);
		}
		//Odblokowanie notyfikatora nowej wiadomosci
		else if(wParam==TIMER_UNBLOCK_INACTIVENOTIFER)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_UNBLOCK_INACTIVENOTIFER);
			//Odblokowanie notyfikatora nowych wiadomosci
			BlockInactiveNotiferNewMsg = false;
		}
		//Przywracanie sesji z czatami
		else if(wParam==TIMER_RESTORE_SESSION)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_RESTORE_SESSION);
			//Sa jakies zakladki czatowe do przywrocenia
			if(ChatSessionList->Count)
			{
				//Pobieranie pierwszego rekordu z listy
				UnicodeString JID = ChatSessionList->Strings[0];
				//Zakladka z kontaktem nie jest jeszcze otwarta
				if(TabsList->IndexOf(JID)==-1)
				{
					//Okno dolaczania do konferencji nie jest aktywne
					if(!FrmChatJoinExist)
					{
						//Definicja domyslnego indeksu konta
						UnicodeString UserIdx = "0";
						//JID zawiera indeks konta
						if(JID.Pos(":"))
						{
							//Wyciagniecie indeksu konta
							UserIdx = JID;
							UserIdx.Delete(1,UserIdx.Pos(":"));
							//Usuniecie indeksu konta z JID
							JID.Delete(JID.Pos(":"),JID.Length());
						}
						//Sprawdzenie stanu konta przypisanego do czatu
						TPluginStateChange PluginStateChange;
						PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),(LPARAM)StrToInt(UserIdx));
						//Konto jest polaczone z siecia
						if(PluginStateChange.NewState!=0)
						{
							//Ustawianie prawidlowego identyfikatora
							JID.Delete(1,7);
							//Wypenianie struktury czatu
							TPluginChatPrep PluginChatPrep;
							PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
							PluginChatPrep.UserIdx = StrToInt(UserIdx);
							PluginChatPrep.JID = JID.w_str();
							PluginChatPrep.Channel = GetChannelName(JID).w_str();
							PluginChatPrep.CreateNew = false;
							PluginChatPrep.Fast = true;
							//Przywracanie zakladki czatowej
							PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
						}
						//Ponowne wlaczenie timera
						SetTimer(hTimerFrm,TIMER_RESTORE_SESSION,3000,(TIMERPROC)TimerFrmProc);
					}
					//Ponowne wlaczenie timera
					else SetTimer(hTimerFrm,TIMER_RESTORE_SESSION,1000,(TIMERPROC)TimerFrmProc);
				}
				else
				{
					//Usuwaniecie czatu z kolejki otwierania
					ChatSessionList->Delete(0);
					//Zostaly jeszcze jakies zakladki czatowe do przywrocenia
					if(ChatSessionList->Count)
						SetTimer(hTimerFrm,TIMER_RESTORE_SESSION,1000,(TIMERPROC)TimerFrmProc);
				}
			}
		}
		//Wczytanie ostatniej wiadomosci do okna rozmowy
		else if(wParam==TIMER_LOADLASTCONV)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_LOADLASTCONV);
			//Wczytywanie ostatnio przeprowadzonej rozmowy
			PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)LoadLastConvJID.w_str(),(LPARAM)LoadLastConvUserIdx);
		}
		//Zmiana pozycji nowo otwartej przypietej zakladki
		else if(wParam==TIMER_CLIPTABS_MOVE)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_CLIPTABS_MOVE);
			//Definicja tymczasowego JID aktywnej zakladki
			UnicodeString JID = ActiveTabJID;
			//Aktywna zakladka jest przypieta
			if(ClipTabsList->IndexOf(JID)!=-1)
			{
				//Zmiana miejsca zakladki
				ChangeClipTabPos(ActiveTabJID,ActiveTabJIDEx);
				//Zakladka byla otwierana przy otwarciu okna jako ostatnia
				if(JID==LastOpenedChatClipTab)
				{
					//Usuniecie JID ostatniej otwartej przypietej zakladki czatowej
					LastOpenedChatClipTab = "";
					//Zmiana aktywnej zakladki na wskazany kontakt/czat
					ChangeActiveTab(ActiveTabBeforeOpenClipTabs);
					//Usuniecie JID ostatnio aktywnej zakladki przed otwieraniem przypietych czatow
					ActiveTabBeforeOpenClipTabs = "";
				}
				//Przebudowa kolejnosci zakladek w pliku sesji
				RebuilSessionTabsList();
			}
		}
		//Otwieranie przypietych zakladek wraz z oknem rozmowy
		else if(wParam==TIMER_CLIPTABS_OPEN)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_CLIPTABS_OPEN);
			//Sa jakies przypiete zakladki
			if(ClipTabsList->Count)
			{
				//Pobieranie aktualnie aktywnej zakladki
				UnicodeString ActiveTab = ActiveTabJIDEx;
				//Otwieranie przypietych zakladek
				for(int Count=0;Count<ClipTabsList->Count;Count++)
				{
					//Pobieranie identyfikatora przypietej zakladki
					UnicodeString JID = ClipTabsList->Strings[Count];
					//Przypieta zakladka nie jest jeszcze otwarta
					if(TabsList->IndexOf(JID)==-1)
					{
						//Otwieranie zakladki z danym kontaktem
						if(!JID.Pos("ischat_")) OpenNewTab(JID);
						//Otwieranie zakladki z czatem
						else if(!JID.Pos("@plugin"))
						{
							//Zapisywanie JID ostatnio aktywnej zakladki przed otwieraniem przypietych czatow
							ActiveTabBeforeOpenClipTabs = ActiveTab;
							//Zapisywanie JID ostatniej otwartej przypietej zakladki czatowej
							LastOpenedChatClipTab = JID;
							//Otwieranie zakladki
							OpenNewTab(JID);
						}
						//Zmiana miejsca zakladki
						ChangeClipTabPos(ActiveTabJID,ActiveTabJIDEx);
					}
				}
				//Liczba otwartych zakladek wieksza od 1
				if(TabsListEx->Count>1)
				{
					//Zmiana aktywnej zakladki na wskazany kontakt/czat
					ChangeActiveTab(ActiveTab);
					//Przebudowa kolejnosci zakladek w pliku sesji
					RebuilSessionTabsList();
				}
			}
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
				MessageBox(Application->Handle,	GetLangStr("TabsWarning").w_str(), GetLangStr("TabsWarningTitle").w_str(), MB_OK | MB_ICONINFORMATION);
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
		//Przywracanie zakladki poprzez 2xLPM
		else if(wParam==TIMER_UNCLOSEBY2XLPM)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_UNCLOSEBY2XLPM);
			//Przywrocenie zakladki
			if(!TabWasChanged) GetClosedTabsItem(0);
		}
		//Zamykanie zakladki poprzez 2xLPM
		else if(wParam==TIMER_CLOSEBY2XLPM)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_CLOSEBY2XLPM);
			//Zostal wcisniety 2xLPM
			if(LBUTTONDBLCLK_EXECUTED)
			{
				//Odcisniecie SPM
				mouse_event(MOUSEEVENTF_MIDDLEUP,0,0,0,0);
				//Odznaczenie wcisniecia 2xLPM
				LBUTTONDBLCLK_EXECUTED = false;
			}
		}
		//Przebudowa kolejnosci zakladek w pliku sesji
		else if(wParam==TIMER_REBUILD_TABS_LIST)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_REBUILD_TABS_LIST);
			//Przebudowa kolejnosci zakladek w pliku sesji
			RebuilSessionTabsList();
			//Przebudowanie listy przypietych zakladek
			RebuildClipTabsEx();
			//Zmiana miejsca aktywnej przypietej zakladki
			if(ClipTabsList->IndexOf(ActiveTabJID)!=-1)
				ChangeClipTabPos(ActiveTabJID,ActiveTabJIDEx);
		}
		//Sprawdzanie pozycji myszki
		else if(wParam==TIMER_MOUSE_POSITION)
		{
			//Chowanie/pokazywanie okna rozmowy
			if((FrmSendSlideChk)&&(hFrmSend))
			{
				//Pokazywanie okna rozmowy (gdy kursor zblizy sie do krawedzi ekranu)
				if((!FrmSendVisible)&&(!SecureMode)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn)&&(!FrmSendBlockSlide))
				{
					//Blokada wysuwania okna przy wcisnietym klawiszu Ctrl/LPM/PPM
					if((!SideSlideCtrlAndMouseBlockChk)||((SideSlideCtrlAndMouseBlockChk)&&(GetKeyState(VK_CONTROL)>=0)&&(GetKeyState(VK_LBUTTON)>=0)&&(GetKeyState(VK_RBUTTON)>=0))||((SideSlideCtrlAndMouseBlockChk)&&(GetKeyState(VK_CONTROL)<0)&&(GetKeyState(VK_LBUTTON)<0)))
					{
						//Kursor znajduje sie przy krawedzi ekranu za ktora zostalo schowane okno
						if(((FrmSendSlideEdge==1)&&(Mouse->CursorPos.x==0))
						||((FrmSendSlideEdge==2)&&(Mouse->CursorPos.x==Screen->Width-1))
						||((FrmSendSlideEdge==3)&&(Mouse->CursorPos.y==Screen->Height-1))
						||((FrmSendSlideEdge==4)&&(Mouse->CursorPos.y==0)))
						{
							//Kursor znajduje sie przy krawedzi ekranu z uwglednieniem miejsca schowania okna rozmowy
							if((((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))&&((FrmSendRect.Top<=Mouse->CursorPos.y)&&(Mouse->CursorPos.y<=FrmSendRect.Bottom)))
							||(((FrmSendSlideEdge==3)||(FrmSendSlideEdge==4))&&((FrmSendRect.Left<=Mouse->CursorPos.x)&&(Mouse->CursorPos.x<=FrmSendRect.Right))))
							{
								//Pobranie klasy okna w ktorym znajduje sie kursor
								wchar_t WindowCaptionNameW[128];
								GetClassNameW(WindowFromPoint(Mouse->CursorPos), WindowCaptionNameW, sizeof(WindowCaptionNameW));
								UnicodeString WindowCaptionName = WindowCaptionNameW;
								//Kursor nie znajduje sie w obrebie menu start
								if(WindowCaptionName!="DV2ControlHost")
								{
									if(!PreFrmSendSlideIn)
									{
										//Status pre-wysuwania okna rozmowy zza krawedzi ekranu
										PreFrmSendSlideIn = true;
										//Brak tymczasowej blokady po wysunieciu okna
										FrmSendDontBlockSlide = true;
										//Wlaczenie pokazywania okna rozmowy (part I)
										SetTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEIN,FrmSendSlideInDelay,(TIMERPROC)TimerFrmProc);
									}
								}
								//Kursor znajduje sie w obrebie menu start
								else if(PreFrmSendSlideIn) StopPreFrmSendSlideIn();
							}
							//Kursor nie znajduje sie przy krawedzi ekranu z uwglednieniem miejsca schowania okna rozmowy
							else if(PreFrmSendSlideIn) StopPreFrmSendSlideIn();
						}
						//Kursor nie znajduje sie przy krawedzi ekranu za ktora zostalo schowane okno
						else if(PreFrmSendSlideIn) StopPreFrmSendSlideIn();
					}
					//Wcisniete klawisze Ctrl/LPM/PPM
					else
					{
						//Zatrzymanie procedury wysuwania okna
						if(PreFrmSendSlideIn) StopPreFrmSendSlideIn();
						//Tymczasowa blokada wysuwania okna
						if(FrmSendSlideInDelay<1000)
						{
							//Kursor znajduje sie przy krawedzi ekranu za ktora zostalo schowane okno
							if(((FrmSendSlideEdge==1)&&(Mouse->CursorPos.x==0))
							||((FrmSendSlideEdge==2)&&(Mouse->CursorPos.x==Screen->Width-1))
							||((FrmSendSlideEdge==3)&&(Mouse->CursorPos.y==Screen->Height-1))
							||((FrmSendSlideEdge==4)&&(Mouse->CursorPos.y==0)))
							{
								//Kursor znajduje sie przy krawedzi ekranu z uwglednieniem miejsca schowania okna rozmowy
								if((((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))&&((FrmSendRect.Top<=Mouse->CursorPos.y)&&(Mouse->CursorPos.y<=FrmSendRect.Bottom)))
								||(((FrmSendSlideEdge==3)||(FrmSendSlideEdge==4))&&((FrmSendRect.Left<=Mouse->CursorPos.x)&&(Mouse->CursorPos.x<=FrmSendRect.Right))))
								{
									//Tymczasowa blokada chowania/pokazywania okna rozmowy
									FrmSendBlockSlide = true;
									//Wlaczenie timera wylaczajacego blokade
									SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,1000,(TIMERPROC)TimerFrmProc);
								}
							}
						}
					}
				}
				//Chowanie okna rozmowy (gdy kursor opusci okno)
				if(FrmSendSlideHideMode==3)
				{
					//Okno rozmowy jest widoczne, aktualnie nie jest chowane/wysuwane, nie jest aktywna blokada
					if((FrmSendVisible)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn)&&(!FrmSendBlockSlide))
					{
						//Kursor znajduje sie poza oknem rozmowy
						if((Mouse->CursorPos.y<FrmSendRect.Top+FrmSendRealTopPos-FrmSend_Shell_TrayWndTop)||(FrmSendRect.Bottom+FrmSendRealBottomPos+FrmSend_Shell_TrayWndBottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmSendRect.Left+FrmSendRealLeftPos-FrmSend_Shell_TrayWndLeft)||(FrmSendRect.Right+FrmSendRealRightPos+FrmSend_Shell_TrayWndRight<Mouse->CursorPos.x))
						{
							//LPM nie jest wcisniety
							if(GetKeyState(VK_LBUTTON)>=0)
							{
								//Pobranie klasy okna w ktorym znajduje sie kursor
								HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
								wchar_t WindowCaptionNameW[128];
								GetClassNameW(hCurActiveFrm, WindowCaptionNameW, sizeof(WindowCaptionNameW));
								UnicodeString WindowCaptionName = WindowCaptionNameW;
								//Pobieranie PID procesu wskazanego okna
								DWORD PID;
								GetWindowThreadProcessId(hCurActiveFrm, &PID);
								//Kursor nie znajduje sie w obrebie menu z okna
								if(!((WindowCaptionName=="#32768")&&(PID==ProcessPID)))
								{
									//Tymczasowa blokada chowania/pokazywania okna rozmowy
									if(PopupMenuBlockSlide)
									{
										//Kursor w obrebie menu z okna
										PopupMenuBlockSlide = false;
										//Tymczasowa blokada chowania/pokazywania okna rozmowy
										FrmSendBlockSlide = true;
										//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
										SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,2000,(TIMERPROC)TimerFrmProc);
									}
									//Chowanie okna rozmowy
									else if(!PreFrmSendSlideOut)
									{
										//Status pre-chowania okna rozmowy za krawedz ekranu
										PreFrmSendSlideOut = true;
										//Wlaczenie chowania okna rozmowy (part I)
										SetTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEOUT,FrmSendSlideOutDelay,(TIMERPROC)TimerFrmProc);
									}
								}
								//Kursor znajduje sie w obrebie menu z okna
								else PopupMenuBlockSlide = true;
							}
							//Tymczasowa blokada chowania/pokazywania okna rozmowy
							else
							{
								//Kursor w obrebie menu z okna
								PopupMenuBlockSlide = false;
								//Tymczasowa blokada chowania/pokazywania okna rozmowy
								FrmSendBlockSlide = true;
								//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
								SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,2000,(TIMERPROC)TimerFrmProc);
							}
						}
						//Kursor znajduje sie w oknie rozmowy
						else if(PreFrmSendSlideOut)
						{
							//Zatrzymanie timera
							KillTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEOUT);
							//Status pre-chowania okna rozmowy za krawedz ekranu
							PreFrmSendSlideOut = false;
						}
					}
				}
			}
			//Chowanie/pokazywanie okna kontaktow
			if(FrmMainSlideChk)
			{
				//Pokazywanie okna kontaktow (gdy kursor zblizy sie do krawedzi ekranu)
				if((!FrmMainVisible)&&(!SecureMode)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
				{
					//Blokada wysuwania okna przy wcisnietym klawiszu Ctrl/LPM/PPM
					if((!SideSlideCtrlAndMouseBlockChk)||((SideSlideCtrlAndMouseBlockChk)&&(GetKeyState(VK_CONTROL)>=0)&&(GetKeyState(VK_LBUTTON)>=0)&&(GetKeyState(VK_RBUTTON)>=0))||((SideSlideCtrlAndMouseBlockChk)&&(GetKeyState(VK_CONTROL)<0)&&(GetKeyState(VK_LBUTTON)<0)))
					{
						//Kursor znajduje sie przy krawedzi ekranu za ktora zostalo schowane okno
						if(((FrmMainSlideEdge==1)&&(Mouse->CursorPos.x==0))
						||((FrmMainSlideEdge==2)&&(Mouse->CursorPos.x==Screen->Width-1))
						||((FrmMainSlideEdge==3)&&(Mouse->CursorPos.y==Screen->Height-1))
						||((FrmMainSlideEdge==4)&&(Mouse->CursorPos.y==0)))
						{
							//Kursor znajduje sie przy krawedzi ekranu z uwglednieniem miejsca schowania okna kontaktow
							if((((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))&&((FrmMainRect.Top<=Mouse->CursorPos.y)&&(Mouse->CursorPos.y<=FrmMainRect.Bottom)))
							||(((FrmMainSlideEdge==3)||(FrmMainSlideEdge==4))&&((FrmMainRect.Left<=Mouse->CursorPos.x)&&(Mouse->CursorPos.x<=FrmMainRect.Right))))
							{
								//Pobranie klasy okna w ktorym znajduje sie kursor
								wchar_t WindowCaptionNameW[128];
								GetClassNameW(WindowFromPoint(Mouse->CursorPos), WindowCaptionNameW, sizeof(WindowCaptionNameW));
								UnicodeString WindowCaptionName = WindowCaptionNameW;
								//Kursor nie znajduje sie w obrebie menu start
								if(WindowCaptionName!="DV2ControlHost")
								{
									if(!PreFrmMainSlideIn)
									{
										//Status pre-wysuwania okna kontaktow zza krawedzi ekranu
										PreFrmMainSlideIn = true;
										//Brak tymczasowej blokady po wysunieciu okna
										FrmMainDontBlockSlide = true;
										//Wlaczenie pokazywania okna kontaktow (part I)
										SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEIN,FrmMainSlideInDelay,(TIMERPROC)TimerFrmProc);
									}
								}
								//Kursor znajduje sie w obrebie menu start
								else if(!PreFrmMainSlideIn) StopPreFrmMainSlideIn();
							}
							//Kursor nie znajduje sie przy krawedzi ekranu z uwglednieniem miejsca schowania okna kontaktow
							else if(PreFrmMainSlideIn) StopPreFrmMainSlideIn();
						}
						//Kursor nie znajduje sie przy krawedzi ekranu za ktora zostalo schowane okno
						else if(PreFrmMainSlideIn) StopPreFrmMainSlideIn();
					}
					//Wcisniete klawisze Ctrl/LPM/PPM
					else
					{
						//Zatrzymanie procedury wysuwania okna
						if(PreFrmMainSlideIn) StopPreFrmMainSlideIn();
						//Tymczasowa blokada wysuwania okna
						else if(FrmMainSlideInDelay<1000)
						{
							//Kursor znajduje sie przy krawedzi ekranu za ktora zostalo schowane okno
							if(((FrmMainSlideEdge==1)&&(Mouse->CursorPos.x==0))
							||((FrmMainSlideEdge==2)&&(Mouse->CursorPos.x==Screen->Width-1))
							||((FrmMainSlideEdge==3)&&(Mouse->CursorPos.y==Screen->Height-1))
							||((FrmMainSlideEdge==4)&&(Mouse->CursorPos.y==0)))
							{
								//Kursor znajduje sie przy krawedzi ekranu z uwglednieniem miejsca schowania okna kontaktow
								if((((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))&&((FrmMainRect.Top<=Mouse->CursorPos.y)&&(Mouse->CursorPos.y<=FrmMainRect.Bottom)))
								||(((FrmMainSlideEdge==3)||(FrmMainSlideEdge==4))&&((FrmMainRect.Left<=Mouse->CursorPos.x)&&(Mouse->CursorPos.x<=FrmMainRect.Right))))
								{
									//Tymczasowa blokada chowania/pokazywania okna kontaktow
									FrmMainBlockSlide = true;
									//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna kontaktow
									SetTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE,1000,(TIMERPROC)TimerFrmProc);
								}
							}
						}
					}
				}
				//Chowanie okna kontaktow (gdy kursor opusci okno)
				if(FrmMainSlideHideMode==3)
				{
					//Okno kontatkow jest widoczne, aktualnie nie jest chowane/wysuwane, nie jest aktywna blokada
					if((FrmMainVisible)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
					{
						//Kursor znajduje sie poza oknem kontaktow
						if((Mouse->CursorPos.y<FrmMainRect.Top+FrmMainRealTopPos-FrmMain_Shell_TrayWndTop)||(FrmMainRect.Bottom+FrmMainRealBottomPos+FrmMain_Shell_TrayWndBottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmMainRect.Left+FrmMainRealLeftPos-FrmMain_Shell_TrayWndLeft)||(FrmMainRect.Right+FrmMainRealRightPos+FrmMain_Shell_TrayWndRight<Mouse->CursorPos.x))
						{
							//LPM nie jest wcisniety
							if(GetKeyState(VK_LBUTTON)>=0)
							{
								//Pobranie klasy okna w ktorym znajduje sie kursor
								HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
								wchar_t WindowCaptionNameW[128];
								GetClassNameW(hCurActiveFrm, WindowCaptionNameW, sizeof(WindowCaptionNameW));
								UnicodeString WindowCaptionName = WindowCaptionNameW;
								//Pobieranie PID procesu wskazanego okna
								DWORD PID;
								GetWindowThreadProcessId(hCurActiveFrm, &PID);
								//Kursor nie znajduje sie w obrebie menu z okna
								if(!((WindowCaptionName=="#32768")&&(PID==ProcessPID)))
								{
									//Tymczasowa blokada chowania/pokazywania okna kontaktow
									if(PopupMenuBlockSlide)
									{
										//Kursor w obrebie menu z okna
										PopupMenuBlockSlide = false;
										//Tymczasowa blokada chowania/pokazywania okna kontaktow
										FrmMainBlockSlide = true;
										//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna kontaktow
										SetTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE,1500,(TIMERPROC)TimerFrmProc);
									}
									//Chowanie okna kontaktow
									else if(!PreFrmMainSlideOut)
									{
										//Status pre-chowania okna kontaktow za krawedz ekranu
										PreFrmMainSlideOut = true;
										//Wlaczenie chowania okna kontaktow (part I)
										SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,FrmMainSlideOutDelay,(TIMERPROC)TimerFrmProc);
									}
								}
								//Kursor znajduje sie w obrebie menu z okna
								else PopupMenuBlockSlide = true;
							}
							//Tymczasowa blokada chowania/pokazywania okna kontaktow
							else
							{
								//Kursor w obrebie menu z okna
								PopupMenuBlockSlide = false;
								//Tymczasowa blokada chowania/pokazywania okna kontaktow
								FrmMainBlockSlide = true;
								//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna kontaktow
								SetTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE,1500,(TIMERPROC)TimerFrmProc);
							}
						}
						//Kursor znajduje sie w oknie kontaktow
						else if(PreFrmMainSlideOut)
						{
							//Zatrzymanie timera
							KillTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT);
							//Status pre-chowania okna kontaktow za krawedz ekranu
							PreFrmMainSlideOut = false;
						}
					}
				}
			}
		}
		//Wylaczanie blokady lokalnego hooka na myszke
		else if(wParam==TIMER_UNBLOCK_MOUSE_PROC)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_UNBLOCK_MOUSE_PROC);
			//Wylaczenie blokady
			BlockThreadMouseProc = false;
		}
		//Ustawianie starej pozycju kursora
		else if(wParam==TIMER_EXSETSEL)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_EXSETSEL);
			//Ustawianie starej pozycju kursora
			SendMessage(hRichEdit, EM_EXSETSEL, NULL, (LPARAM)&hRichEditSelPos);
			//Wylaczenie blokady lokalnego hooka na klawiature
			BlockThreadKeyboardProc = false;
		}
		//Sprawdzanie aktywnego okna
		else if(wParam==TIMER_ACTIVE_WINDOW)
		{
			//Pobieranie aktywnego okna
			HWND hActiveFrm = GetForegroundWindow();
			//Pobieranie klasy aktywnego okna
			wchar_t WindowCaptionNameW[128];
			GetClassNameW(hActiveFrm, WindowCaptionNameW, sizeof(WindowCaptionNameW));
			UnicodeString WindowCaptionName = WindowCaptionNameW;
			//Ustawianie uchwytu do nowego aktywnego okna
			if((hActiveFrm!=LastActiveWindow)&&(hActiveFrm!=hFrmSend)&&(hActiveFrm!=hFrmMain)&&(IsWindow(hActiveFrm)))
			{
				if((WindowCaptionName!="Shell_TrayWnd")
				&&(WindowCaptionName!="MSTaskListWClass")
				&&(WindowCaptionName!="NotifyIconOverflowWindow")
				&&(WindowCaptionName!="ClockFlyoutWindow")
				&&(WindowCaptionName!="DV2ControlHost")
				&&(WindowCaptionName!="TaskSwitcherWnd"))
					LastActiveWindow = hActiveFrm;
			}
			//Pobranie PID aktywnego okna
			DWORD PID;
			GetWindowThreadProcessId(hActiveFrm, &PID);
			//Otworzenie okna tworzenia konferencji GG
			if((WindowCaptionName=="TfrmCreateChat")&&(!FrmCreateChatExists)&&(PID==ProcessPID))
			{
				//Okno istnieje
				FrmCreateChatExists = true;
				//Tymczasowa blokada chowania/pokazywania okna kontaktow
				FrmMainBlockSlide = true;
			}
			//Zamkniecie okna tworzenia konferencji GG
			else if((WindowCaptionName!="TfrmCreateChat")&&(FrmCreateChatExists)&&(PID==ProcessPID))
			{
				//Okno nie istnieje
				FrmCreateChatExists = false;
				//Aktywacja okna kontaktow + nadanie fokusa kontrolce IE
				ActivateAndFocusFrmMain();
				//Tymczasowa blokada chowania/pokazywania okna kontaktow
				if(FrmMainSlideHideMode==3) SetTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE,1500,(TIMERPROC)TimerFrmProc);
				else FrmMainBlockSlide = false;
			}
			//Chowanie okna rozmowy gdy aplikacja straci fokus
			if((FrmSendSlideChk)&&(FrmSendSlideHideMode==2))
			{
				//Okno rozmowy jest widoczne, aktualnie nie jest chowane/wysuwane, nie jest aktywna blokada
				if((FrmSendVisible)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn)&&(!FrmSendBlockSlide))
				{
					//Kursor znajduje sie poza oknem rozmowy
					if((Mouse->CursorPos.y<FrmSendRect.Top)||(FrmSendRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmSendRect.Left)||(FrmSendRect.Right<Mouse->CursorPos.x))
					{
						//Okno nie jest innym oknem aplikacji
						if(PID!=ProcessPID)
						{
							if((WindowCaptionName!="Shell_TrayWnd")
							&&(WindowCaptionName!="MSTaskListWClass")
							&&(WindowCaptionName!="NotifyIconOverflowWindow")
							&&(WindowCaptionName!="ClockFlyoutWindow")
							&&(WindowCaptionName!="DV2ControlHost")
							&&(WindowCaptionName!="TaskSwitcherWnd"))
							{
								//Status chowania okna rozmowy za krawedz ekranu
								FrmSendSlideOut = true;
								//Wlaczenie chowania okna rozmowy (part I)
								SetTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
							}
						}
					}
				}
			}
			//Chowanie okna kontaktow gdy aplikacja straci fokus
			if((FrmMainSlideChk)&&(FrmMainSlideHideMode==2))
			{
				//Okno kontaktow jest widoczne, aktualnie nie jest chowane/wysuwane, nie jest aktywna blokada
				if((FrmMainVisible)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
				{
					//Kursor znajduje sie poza oknem rozmowy
					if((Mouse->CursorPos.y<FrmMainRect.Top)||(FrmMainRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmMainRect.Left)||(FrmMainRect.Right<Mouse->CursorPos.x))
					{
						//Okno nie jest innym oknem aplikacji
						if(PID!=ProcessPID)
						{
							if((WindowCaptionName!="Shell_TrayWnd")
							&&(WindowCaptionName!="MSTaskListWClass")
							&&(WindowCaptionName!="NotifyIconOverflowWindow")
							&&(WindowCaptionName!="ClockFlyoutWindow")
							&&(WindowCaptionName!="DV2ControlHost")
							&&(WindowCaptionName!="TaskSwitcherWnd"))
							{
								//Status chowania okna kontaktow za krawedz ekranu
								FrmMainSlideOut = true;
								//Wlaczenie chowania okna kontaktow (part I)
								SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
							}
						}
					}
				}
			}
			//Pobieranie okna w ktorym znajduje sie kursor
			HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
			//Pobieranie klasy okna w ktorym znajduje sie kursor
			GetClassNameW(hCurActiveFrm, WindowCaptionNameW, sizeof(WindowCaptionNameW));
			WindowCaptionName = WindowCaptionNameW;
			//Pobranie PID okna w ktorym znajduje sie kursor
			GetWindowThreadProcessId(hCurActiveFrm, &PID);
			//Gdy kursor znajduje sie w obrebie menu z okna aplikacji
			if((WindowCaptionName=="#32768")&&(PID==ProcessPID))
				hPopupMenu = hCurActiveFrm;
			//Pokazywanie okna rozmowy poprzez miniaturke z paska zadan
			if(FrmSendSlideChk)
			{
				//Kursor znajduje sie w obrebie miniaturki z paska zadan
				if(WindowCaptionName=="TaskListThumbnailWnd")
				{
					//Okno rozmowy jest schowane za krawedzia ekranu
					if((!FrmSendVisible)&&(!FrmSendShownByThumbnail))
					{
						//Odznaczenie pokazania okna rozmowy za pomoca miniaturki
						FrmSendShownByThumbnail = true;
						//Wysuniecie okna rozmowy zza krawedzi ekranu
						ShowFrmSend();
						//Tymczasowa blokada chowania/pokazywania okna rozmowy
						FrmSendBlockSlide = true;
						//Stan widocznosci okna rozmowy
						FrmSendVisible = true;
					}
				}
				//Kursor nie znajduje sie juz w obrebie miniaturki z paska zadan
				else if(FrmSendShownByThumbnail)
				{
					//Okno rozmowy zostalo aktywowane z miniaturku z paska zadan
					if(hActiveFrm==hFrmSend)
					{
						//Zatrzymanie timera odblokowania pokazywania okna rozmowy poprzez miniaturke z paska zadan
						KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_THUMBNAIL);
						//Odznaczenie wlaczenia timera odblokowania pokazywania okna rozmowy poprzez miniaturke z paska zadan
						FrmSendShownByThumbnailTimer = false;
						//Odznaczenie pokazania okna rozmowy za pomoca miniaturki
						FrmSendShownByThumbnail = false;
						//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
						if(FrmSendSlideHideMode==3) FrmSendUnBlockSlide = true;
						//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
						if(FrmSendSlideHideMode==3) SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,2000,(TIMERPROC)TimerFrmProc);
					}
					//Wlaczenie timera odblokowania pokazywania okna rozmowy poprzez miniaturke z paska zadan
					else if(!FrmSendShownByThumbnailTimer)
					{
						//Odznaczenie wlaczenia timera
						FrmSendShownByThumbnailTimer = true;
						//Wlaczenie timera
						SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_THUMBNAIL,100,(TIMERPROC)TimerFrmProc);
					}
				}
			}
		}
		//Chowanie okna rozmowy (part I)
		else if(wParam==TIMER_FRMSEND_PRE_SLIDEOUT)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEOUT);
			//Status pre-chowania okna rozmowy za krawedz ekranu
			PreFrmSendSlideOut = false;
			//Status chowania okna rozmowy za krawedz ekranu
			FrmSendSlideOut = true;
			//Ustawianie poprawnej pozycji okna
			//Okno rozmowy chowane za lewa/prawa krawedzia ekranu
			if((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))
			{
				//Pobranie pozycji okna
				TRect WindowRect;
				GetWindowRect(hFrmSend,&WindowRect);
				//Pozycja inna od zapamietanej
				if(WindowRect.Left!=FrmSendRect.Left)
					SetWindowPos(hFrmSend,HWND_TOPMOST,WindowRect.Left,FrmSendRect.Top,0,0,SWP_NOSIZE);
			}
			//Okno rozmowy chowane za dolna/gorna krawedzia ekranu
			else
			{
				//Pobranie pozycji okna
				TRect WindowRect;
				GetWindowRect(hFrmSend,&WindowRect);
				//Pozycja inna od zapamietanej
				if(WindowRect.Top!=FrmSendRect.Top)
					SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendRect.Left,WindowRect.Top,0,0,SWP_NOSIZE);
			}
			//Pobranie rozmiaru+pozycji okna rozmowy
			GetFrmSendRect();
			//Poczatkowa pozycja okna rozmowy podczas chowania/pokazywania
			FrmSendSlideLeft = FrmSendRect.Left;
			FrmSendSlideTop = FrmSendRect.Top;
			//Aktywacja okna rozmowy + ustawienie okna na wierzchu
			ActivateAndSetTopmostFrmSend();
			//Wlacznie chowania okna rozmowy (part II)
			SetTimer(hTimerFrm,TIMER_FRMSEND_SLIDEOUT,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
		}
		//Chowanie okna rozmowy (part II)
		else if(wParam==TIMER_FRMSEND_SLIDEOUT)
		{
			//Ustalanie ilosci krokow
			int Steps = FrmSendSlideOutTime / FrmSendStepInterval;
			//Okno rozmowy chowane za lewa/prawa krawedzia ekranu
			if((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))
			{
				//Ustalanie ilosci krokow
				Steps = (FrmSendRect.Width()+FrmSend_Shell_TrayWndRight+FrmSend_Shell_TrayWndLeft)/Steps;
				//Okno rozmowy chowane za lewa krawedzia ekranu
				if(FrmSendSlideEdge==1) FrmSendSlideLeft = FrmSendSlideLeft - Steps;
				//Okno rozmowy chowane za prawa krawedzia ekranu
				else FrmSendSlideLeft = FrmSendSlideLeft + Steps;
				//Zmiana pozycji okna rozmowy
				SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendSlideLeft,FrmSendRect.Top,0,0,SWP_NOSIZE);
			}
			//Okno rozmowy chowane za dolna/gorna krawedzia ekranu
			else
			{
				//Ustalanie ilosci krokow
				Steps = (FrmSendRect.Height()+FrmSend_Shell_TrayWndBottom+FrmSend_Shell_TrayWndTop)/Steps;
				//Okno rozmowy chowane za dolna krawedzia ekranu
				if(FrmSendSlideEdge==3) FrmSendSlideTop = FrmSendSlideTop + Steps;
				//Okno rozmowy chowane za gorna krawedzia ekranu
				else FrmSendSlideTop = FrmSendSlideTop - Steps;
				//Zmiana pozycji okna rozmowy
				SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendRect.Left,FrmSendSlideTop,0,0,SWP_NOSIZE);
			}
			//Koncowy etap
			if(((FrmSendSlideEdge==1)&&(FrmSendSlideLeft<(0-FrmSendRect.Right)))
			||((FrmSendSlideEdge==2)&&(FrmSendSlideLeft>Screen->Width))
			||((FrmSendSlideEdge==3)&&(FrmSendSlideTop>Screen->Height))
			||((FrmSendSlideEdge==4)&&(FrmSendSlideTop<(0-FrmSendRect.Bottom))))
			{
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_FRMSEND_SLIDEOUT);
				//Aplikacja pelno ekranowa
				if(FullScreenMode)
				{
					HWND hHideFrm = FindWindow(L"Shell_TrayWnd",NULL);
					ShowWindow(hHideFrm,SW_SHOW);
					hHideFrm = FindWindow(L"Button",L"Start");
					if(hHideFrm) ShowWindow(hHideFrm,SW_SHOW);
					SetForegroundWindow(FullScreenWindow);
				}
				//Status chowania okna rozmowy za krawedz ekranu
				FrmSendSlideOut = false;
				//Stan widocznosci okna rozmowy
				FrmSendVisible = false;
				//Aktywacja poprzedniego okna, jezeli nie jest aktywne chowanie przy utracie fokusa calej aplikacji
				if((FrmMainSlideChk)&&(FrmMainSlideHideMode==2)&&(FrmSendSlideHideMode!=2)&&(FrmMainVisible)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
				{
					//Aktywacja okna kontaktow + nadanie fokusa kontrolce IE
					ActivateAndFocusFrmMain();
				}
				else
				{
					//Okno nie bylo chowane po straceniu fokusa calej aplikacji
					if(FrmSendSlideHideMode!=2)
					{
						//Aktywacja okna kontaktow?
						bool ActiveFrmMain = false;
						//Aktywacja okna kontaktow przy chowaniu obu okien po opuszczeniu kursora
						if((FrmMainSlideChk)&&(FrmMainSlideHideMode==3)&&(FrmSendSlideHideMode==3)&&(FrmMainVisible)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
						{
							//Kursor znajduje sie w oknie kontaktow
							if((Mouse->CursorPos.y>FrmMainRect.Top)&&(FrmMainRect.Bottom>Mouse->CursorPos.y)&&(Mouse->CursorPos.x>FrmMainRect.Left)&&(FrmMainRect.Right>Mouse->CursorPos.x))
							{
								//Aktywacja okna kontaktow + nadanie fokusa kontrolce IE
								ActivateAndFocusFrmMain();
								//Odznaczenie aktywacji okna kontaktow
								ActiveFrmMain = true;
							}
						}
						//Okno kontatkow nie zostalo aktywowane
						if(!ActiveFrmMain)
						{
							//Pobieranie PID procesu wskazanego okna
							DWORD PID;
							GetWindowThreadProcessId(LastActiveWindow, &PID);
							//Okno z innej aplikacji
							if(PID!=ProcessPID)
							{
								if((LastActiveWindow_WmInactiveFrmSendSlide)&&(LastActiveWindow_WmInactiveFrmSendSlide!=hFrmMain)&&(LastActiveWindow_WmInactiveFrmSendSlide==hFrmSend))
									SetForegroundWindow(LastActiveWindow_WmInactiveFrmSendSlide);
								else if((LastActiveWindow_PreFrmSendSlideIn)&&(LastActiveWindow_PreFrmSendSlideIn!=hFrmMain)&&(LastActiveWindow_PreFrmSendSlideIn==hFrmSend))
									SetForegroundWindow(LastActiveWindow_PreFrmSendSlideIn);
								else if(LastActiveWindow)
									SetForegroundWindow(LastActiveWindow);
							}
							//Aktywacja okna w AQQ
							else SetForegroundWindow(LastActiveWindow);
						}
					}
					//Aktywacja okna spod kursora
					else SetForegroundWindow(WindowFromPoint(Mouse->CursorPos));
				}
				//Usuniecie uchwytow do nowego aktywnego okna
				LastActiveWindow_WmInactiveFrmSendSlide = NULL;
				LastActiveWindow_PreFrmSendSlideIn = NULL;
			}
		}
		//Pokazywanie okna rozmowy (part I)
		else if(wParam==TIMER_FRMSEND_PRE_SLIDEIN)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEIN);
			//Status pre-wysuwania okna rozmowy zza krawedzi ekranu
			PreFrmSendSlideIn = false;
			//Pobieranie nowego aktywnego okna
			wchar_t WindowCaptionNameW[128];
			GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
			UnicodeString WindowCaptionName = WindowCaptionNameW;
			if((WindowCaptionName!="Shell_TrayWnd")
			&&(WindowCaptionName!="MSTaskListWClass")
			&&(WindowCaptionName!="NotifyIconOverflowWindow")
			&&(WindowCaptionName!="ClockFlyoutWindow")
			&&(WindowCaptionName!="DV2ControlHost")
			&&(WindowCaptionName!="TaskSwitcherWnd"))
				LastActiveWindow_PreFrmSendSlideIn = GetForegroundWindow();
			//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
			ChkFullScreenMode();
			//Blokowanie wysuwania przy aplikacji pelnoekranowej
			if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
			{
				//Status wysuwania okna rozmowy zza krawedzi ekranu
				FrmSendSlideIn = false;
				//Brak tymczasowej blokady po wysunieciu okna
				FrmSendDontBlockSlide = false;
			}
			else
			{
				//Status wysuwania okna rozmowy zza krawedzi ekranu
				FrmSendSlideIn = true;
				//Przywracanie okna
				if(IsIconic(hFrmSend))
				{
					//Tymczasowa blokada chowania/pokazywania okna rozmowy
					FrmSendBlockSlide = true;
					//Przywracanie okna rozmowy
					ShowWindow(hFrmSend,SW_RESTORE);
					SetForegroundWindow(hFrmSend);
					//Przywracanie poprawnych wymiarow okna
					SetWindowPos(hFrmSend,NULL,0,0,FrmSendRect.Width(),FrmSendRect.Height(),SWP_NOMOVE);
					//Tymczasowa blokada chowania/pokazywania okna rozmowy
					FrmSendBlockSlide = false;
				}
				//Odswiezenie okna rozmowy
				RefreshFrmSend();
				//Aktywacja okna rozmowy + ustawienie okna na wierzchu
				ActivateAndSetTopmostFrmSend();
				//Wlaczenie pokazywania okna rozmowy (part II)
				SetTimer(hTimerFrm,TIMER_FRMSEND_SLIDEIN,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
			}
		}
		//Pokazywanie okna rozmowy (part II)
		else if(wParam==TIMER_FRMSEND_SLIDEIN)
		{
			//Ustalane ilosci krokow
			int Steps = FrmSendSlideInTime / FrmSendStepInterval;
			//Okno rozmowy chowane za lewa/prawa krawedzia ekranu
			if((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2))
			{
				//Ustalanie ilosci krokow
				Steps = (FrmSendRect.Width()+FrmSend_Shell_TrayWndRight+FrmSend_Shell_TrayWndLeft)/Steps;
				//Okno rozmowy chowane za lewa krawedzia ekranu
				if(FrmSendSlideEdge==1)
				{
					FrmSendSlideLeft = FrmSendSlideLeft + Steps;
					if(FrmSendSlideLeft>FrmSendRect.Left) FrmSendSlideLeft = FrmSendRect.Left;
				}
				//Okno rozmowy chowane za prawa krawedzia ekranu
				else
				{
					FrmSendSlideLeft = FrmSendSlideLeft - Steps;
					if(FrmSendSlideLeft<FrmSendRect.Left) FrmSendSlideLeft = FrmSendRect.Left;
				}
				if(!IsWindowVisible(hFrmSend)) ShowWindow(hFrmSend, SW_SHOWNA);
				//Zmiana pozycji okna rozmowy
				SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendSlideLeft,FrmSendRect.Top,0,0,SWP_NOSIZE);
			}
			//Okno rozmowy chowane za dolna/gorna krawedzia ekranu
			else
			{
				//Ustalanie ilosci krokow
				Steps = (FrmSendRect.Height()+FrmSend_Shell_TrayWndBottom+FrmSend_Shell_TrayWndTop)/Steps;
				//Okno rozmowy chowane za dolna krawedzia ekranu
				if(FrmSendSlideEdge==3)
				{
					FrmSendSlideTop = FrmSendSlideTop - Steps;
					if(FrmSendSlideTop<FrmSendRect.Top) FrmSendSlideTop = FrmSendRect.Top;
				}
				//Okno rozmowy chowane za gorna krawedzia ekranu
				else
				{
					FrmSendSlideTop = FrmSendSlideTop + Steps;
					if(FrmSendSlideTop>FrmSendRect.Top) FrmSendSlideTop = FrmSendRect.Top;
				}
				if(!IsWindowVisible(hFrmSend)) ShowWindow(hFrmSend, SW_SHOWNA);
				//Zmiana pozycji okna rozmowy
				SetWindowPos(hFrmSend,HWND_TOPMOST,FrmSendRect.Left,FrmSendSlideTop,0,0,SWP_NOSIZE);
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
				//Wylaczenie statusu okna na wierzchu
				if(FrmSendSlideHideMode==2) SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				//Aktywcja okna rozmowy + nadanie fokusa na polu wpisywania wiadomosci
				ActivateAndFocusFrmSend();
				//Status wysuwania okna rozmowy zza krawedzi ekranu
				FrmSendSlideIn = false;
				//Stan widocznosci okna rozmowy
				FrmSendVisible = true;
				//Tymczasowa blokada chowania okna rozmowy
				if((FrmSendSlideHideMode==3)&&(!FrmSendDontBlockSlide))
				{
					//Tymczasowa blokada chowania/pokazywania okna rozmowy
					FrmSendBlockSlide = true;
					//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
					if(FrmSendUnBlockSlideEx) FrmSendUnBlockSlide = true;
					else FrmSendUnBlockSlideEx = true;
					//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
					SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,2000,(TIMERPROC)TimerFrmProc);
				}
				//Brak tymczasowej blokady po wysunieciu okna
				FrmSendDontBlockSlide = false;
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_FRMSEND_SLIDEIN);
			}
		}
		//Wylaczenie tymczasowej blokady chowania/pokazywania okna rozmowy
		else if(wParam==TIMER_FRMSEND_UNBLOCK_SLIDE)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE);
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			if(StayOnTopChk) FrmSendBlockSlide = StayOnTopStatus;
			else FrmSendBlockSlide = false;
			//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
			FrmSendUnBlockSlide = false;
			//Wylaczanie tymczasowej blokady podczas tworzenia wiadomosci
			FrmSendBlockSlideOnMsgComposing = false;
		}
		//Blokada minimalizacji okna rozmowy
		else if(wParam==TIMER_FRMSEND_MINIMIZED)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FRMSEND_MINIMIZED);
			//Aktywacja okna
			ShowWindow(hFrmSend,SW_RESTORE);
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			FrmSendBlockSlide = false;
			//Status chowania okna rozmowy za krawedz ekranu
			FrmSendSlideOut = true;
			//Wlacznie chowania okna rozmowy (part II)
			SetTimer(hTimerFrm,TIMER_FRMSEND_SLIDEOUT,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
		}
		//Ustawienie prawidlowej pozycji okna rozmowy
		else if(wParam==TIMER_FRMSEND_CHANGEPOS)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FRMSEND_CHANGEPOS);
			//Wylaczenie funkcjanalnosci
			FrmSendSlideChk = false;
			//Przywracanie okna rozmowy
			if(IsIconic(hFrmSend))
			{
				ShowWindow(hFrmSend,SW_RESTORE);
				SetForegroundWindow(hFrmSend);
			}
			//Pobranie rozmiaru+pozycji okna rozmowy
			TRect WindowRect;
			GetWindowRect(hFrmSend,&WindowRect);
			//Aktywacja okna
			SetForegroundWindow(hFrmSend);
			//Ustawianie wstepnej pozycji okna rozmowy
			//Okno rozmowy chowane za lewa krawedzia ekranu
			if(FrmSendSlideEdge==1)
				SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
			//Okno rozmowy chowane za prawa krawedzia ekranu
			else if(FrmSendSlideEdge==2)
				SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-WindowRect.Width()-FrmSend_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
			//Okno rozmowy chowane za dolna krawedzia ekranu
			else if(FrmSendSlideEdge==3)
				SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
			//Okno rozmowy chowane za gorna krawedzia ekranu
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
			UnicodeString KeyValue = IntToStr((int)FrmSendRect.Left);
			SaveSetup.Value = KeyValue.w_str();
			PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
			SaveSetup.Section = L"Position";
			SaveSetup.Ident = L"MsgTop";
			KeyValue = IntToStr((int)FrmSendRect.Top);
			SaveSetup.Value = KeyValue.w_str();
			PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
			//Stan widocznosci okna rozmowy
			FrmSendVisible = true;
			//Wlaczenie funkcjanalnosci
			FrmSendSlideChk = true;
			//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
			if(FrmSendSlideHideMode==3) SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,2000,(TIMERPROC)TimerFrmProc);
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			else FrmSendBlockSlide = false;
		}
		//Ustawienie okna rozmowy na wierzchu
		else if(wParam==TIMER_FRMSEND_TOPMOST)
		{
			//Pobieranie klasy nowego okna
			wchar_t WindowCaptionNameW[128];
			GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
			UnicodeString WindowCaptionName = WindowCaptionNameW;
			//Wlaczenie timera ustawienia okna na wierzchu
			if((WindowCaptionName!="TaskSwitcherWnd")&&(WindowCaptionName!="DV2ControlHost")&&(WindowCaptionName!="CabinetWClass")&&(WindowCaptionName!="Shell_TrayWnd"))
			{
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_FRMSEND_TOPMOST);
				//Pobranie uchwytu
				LastActiveWindow_PreFrmSendSlideIn = GetForegroundWindow();
				//Ustawienie okna rozmowy na wierzchu
				SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			}
			else SetWindowPos(GetForegroundWindow(),HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}
		//Ustawienie okna rozmowy na wierzchu i je schowanie
		else if(wParam==TIMER_FRMSEND_TOPMOST_AND_SLIDEOUT)
		{
			//Pobieranie klasy nowego okna
			wchar_t WindowCaptionNameW[128];
			GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
			UnicodeString WindowCaptionName = WindowCaptionNameW;
			//Wlaczenie timera ustawienia okna na wierzchu
			if(WindowCaptionName!="TaskSwitcherWnd")
			{
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_FRMSEND_TOPMOST_AND_SLIDEOUT);
				if(GetForegroundWindow()==hFrmSend)
				{
					//Pobieranie nowego aktywnego okna
					LastActiveWindow_WmInactiveFrmSendSlide = GetForegroundWindow();
					//Status chowania okna rozmowy za krawedz ekranu
					FrmSendSlideOut = true;
					//Ustawienie okna na wierzchu
					SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
					//Wlaczenie chowania okna rozmowy (part I)
					SetTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
				}
				else
				{
					//Pobranie uchwytu
					LastActiveWindow_PreFrmSendSlideIn = GetForegroundWindow();
					//Aktywcja okna rozmowy + nadanie fokusa na polu wpisywania wiadomosci
					ActivateAndFocusFrmSend();
				}
			}
		}
		//Aktywacja pola wpisywania tekstu
		else if(wParam==TIMER_FRMSEND_FOCUS_RICHEDIT)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FRMSEND_FOCUS_RICHEDIT);
			//Ustawianie fokusa
			FocusRichEdit();
		}
		//Odblokowanie pokazywania okna rozmowy poprzez miniaturke z paska zadan
		else if(wParam==TIMER_FRMSEND_UNBLOCK_THUMBNAIL)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_THUMBNAIL);
			//Pokazywanie okna rozmowy poprzez miniaturke z paska zadan zablokowane
			if(FrmSendShownByThumbnail)
			{
				//Odznaczenie pokazania okna rozmowy za pomoca miniaturki
				FrmSendShownByThumbnail = false;
				//Schowanie okna rozmowy za krawedz ekranu
				HideFrmSend();
				//Stan widocznosci okna rozmowy
				FrmSendVisible = false;
				//Tymczasowa blokada chowania/pokazywania okna rozmowy
				FrmSendBlockSlide = false;
			}
			//Odznaczenie wlaczenia timera
			FrmSendShownByThumbnailTimer = false;
		}
		//Chowanie okna kontaktow (part I)
		else if(wParam==TIMER_FRMMAIN_PRE_SLIDEOUT)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT);
			//Status pre-chowania okna kontaktow za krawedz ekranu
			PreFrmMainSlideOut = false;
			//Status chowania okna kontaktow za krawedz ekranu
			FrmMainSlideOut = true;
			//Ustawianie poprawnej pozycji okna
			if((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))
			{
				//Pobranie pozycji okna
				TRect WindowRect;
				GetWindowRect(hFrmMain,&WindowRect);
				//Pozycja inna od zapamietanej
				if(WindowRect.Left!=FrmMainRect.Left)
					SetWindowPos(hFrmMain,HWND_TOPMOST,WindowRect.Left,FrmMainRect.Top,0,0,SWP_NOSIZE);
			}
			else
			{
				//Pobranie pozycji okna
				TRect WindowRect;
				GetWindowRect(hFrmMain,&WindowRect);
				//Pozycja inna od zapamietanej
				if(WindowRect.Top!=FrmMainRect.Top)
					SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainRect.Left,WindowRect.Top,0,0,SWP_NOSIZE);
			}
			//Pobranie rozmiaru+pozycji okna kontatkow
			GetFrmMainRect();
			//Poczatkowa pozycja okna kontaktow podczas pokazywania/chowania
			FrmMainSlideLeft = FrmMainRect.Left;
			FrmMainSlideTop = FrmMainRect.Top;
			//Aktywacja okna kontaktow + ustawienie okna na wierzchu
			ActivateAndSetTopmostFrmMain();
			//Wlaczenie chowania okna rozmowy (part II)
			SetTimer(hTimerFrm,TIMER_FRMMAIN_SLIDEOUT,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
		}
		//Chowanie okna kontaktow (part I)
		else if(wParam==TIMER_FRMMAIN_SLIDEOUT)
		{
			//Ustalanie ilosci krokow
			int Steps = FrmMainSlideOutTime / FrmMainStepInterval;
			//Okno kontaktow chowane za lewa/prawa krawedzia ekranu
			if((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))
			{
				//Ustalanie ilosci krokow
				Steps = (FrmMainRect.Width()+FrmMain_Shell_TrayWndRight+FrmMain_Shell_TrayWndLeft)/Steps;
				//Okno kontaktow chowane za lewa krawedzia ekranu
				if(FrmMainSlideEdge==1) FrmMainSlideLeft = FrmMainSlideLeft - Steps;
				//Okno kontaktow chowane za prawa krawedzia ekranu
				else FrmMainSlideLeft = FrmMainSlideLeft + Steps;
				//Zmiana pozycji okna kontaktow
				SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainSlideLeft,FrmMainRect.Top,0,0,SWP_NOSIZE);
			}
			//Okno kontaktow chowane za dolna/gorna krawedzia ekranu
			else
			{
				//Ustalanie ilosci krokow
				Steps = (FrmMainRect.Height()+FrmMain_Shell_TrayWndBottom+FrmMain_Shell_TrayWndTop)/Steps;
				//Okno kontaktow chowane za dolna krawedzia ekranu
				if(FrmMainSlideEdge==3) FrmMainSlideTop = FrmMainSlideTop + Steps;
				//Okno kontaktow chowane za gorna krawedzia ekranu
				else FrmMainSlideTop = FrmMainSlideTop - Steps;
				//Zmiana pozycji okna kontaktow
				SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainRect.Left,FrmMainSlideTop,0,0,SWP_NOSIZE);
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
				}
				//Status chowania okna kontaktow
				FrmMainSlideOut = false;
				//Stan widocznosci okna kontaktow
				FrmMainVisible = false;
				//Aktywacja poprzedniego okna
				if((FrmSendSlideChk)&&((FrmSendBlockSlideOnMsgComposing)||((FrmSendVisible)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))))
				{
					//Okno tworzenia wycinka nie jest aktywne
					if(!FrmPosExist)
					{
						//Aktywcja okna rozmowy + nadanie fokusa na polu wpisywania wiadomosci
						ActivateAndFocusFrmSend();
					}
				}
				else
				{
					//Gdy okno nie bylo chowane po straceniu fokusa calej aplikacji
					if(FrmMainSlideHideMode!=2)
					{
						//Okno kontaktow nie zostalo schowane przy aktywacji okna rozmowy
						if(!FrmMainSlideOutActiveFrmSend)
						{
							//Pobieranie PID procesu wskazanego okna
							DWORD PID;
							GetWindowThreadProcessId(LastActiveWindow, &PID);
							//Okno z innej aplikacji
							if(PID!=ProcessPID)
							{
								if((LastActiveWindow_WmInactiveFrmMainSlide)&&(LastActiveWindow_WmInactiveFrmMainSlide!=hFrmMain)&&(LastActiveWindow_WmInactiveFrmMainSlide==hFrmSend))
									SetForegroundWindow(LastActiveWindow_WmInactiveFrmMainSlide);
								else if((LastActiveWindow_PreFrmMainSlideIn)&&(LastActiveWindow_PreFrmMainSlideIn!=hFrmMain)&&(LastActiveWindow_PreFrmMainSlideIn==hFrmSend))
									SetForegroundWindow(LastActiveWindow_PreFrmMainSlideIn);
								else
									SetForegroundWindow(LastActiveWindow);
							}
							//Aktywacja okna w AQQ
							else SetForegroundWindow(LastActiveWindow);
						}
						//Okno kontaktow zostalo schowane przy aktywacji okna rozmowy
						else
						{
							//Schowanie okna kontaktow przy aktywacji okna rozmowy
							FrmMainSlideOutActiveFrmSend = false;
							//Aktywcja okna rozmowy + nadanie fokusa na polu wpisywania wiadomosci
							ActivateAndFocusFrmSend();
						}
					}
					//Aktywacja okna spod kursora
					else SetForegroundWindow(WindowFromPoint(Mouse->CursorPos));
				}
				//Usuniecie uchwytow do nowego aktywnego okna
				LastActiveWindow_WmInactiveFrmMainSlide = NULL;
				LastActiveWindow_PreFrmMainSlideIn = NULL;
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_FRMMAIN_SLIDEOUT);
			}
		}
		//Pokazywanie okna kontaktow (part I)
		else if(wParam==TIMER_FRMMAIN_PRE_SLIDEIN)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEIN);
			//Status pre-wysuwania okna kontaktow zza krawedzi ekranu
			PreFrmMainSlideIn = false;
			//Pobieranie nowego aktywnego okna
			wchar_t WindowCaptionNameW[128];
			GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
			UnicodeString WindowCaptionName = WindowCaptionNameW;
			if((WindowCaptionName!="Shell_TrayWnd")
			&&(WindowCaptionName!="MSTaskListWClass")
			&&(WindowCaptionName!="NotifyIconOverflowWindow")
			&&(WindowCaptionName!="ClockFlyoutWindow")
			&&(WindowCaptionName!="DV2ControlHost")
			&&(WindowCaptionName!="TaskSwitcherWnd"))
				LastActiveWindow_PreFrmMainSlideIn = GetForegroundWindow();
			//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
			ChkFullScreenMode();
			//Blokowanie wysuwania przy aplikacji pelnoekranowej
			if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
			{
				//Status wysuwania okna kontaktow zza krawedzi ekranu
				FrmMainSlideIn = false;
				//Brak tymczasowej blokady po wysunieciu okna
				FrmMainDontBlockSlide = false;
			}
			else
			{
				//Status wysuwania okna kontaktow zza krawedzi ekranu
				FrmMainSlideIn = true;
				//Aktywacja okna kontaktow + ustawienie okna na wierzchu
				ActivateAndSetTopmostFrmMain();
				//Przelaczanie widoku na liste kontatkow
				if(ChangeTabAfterSlideIn) PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)L"aJabber");
				//Wlacznie FrmMainSlideIn (part II)
				SetTimer(hTimerFrm,TIMER_FRMMAIN_SLIDEIN,FrmMainStepInterval,(TIMERPROC)TimerFrmProc);
			}
		}
		//Pokazywanie okna kontaktow (part II)
		else if(wParam==TIMER_FRMMAIN_SLIDEIN)
		{
			//Ustalane ilosci krokow
			int Steps = FrmMainSlideInTime / FrmMainStepInterval;
			//Okno kontaktow chowane za lewa/prawa krawedzia ekranu
			if((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2))
			{
				//Ustalanie ilosci krokow
				Steps = (FrmMainRect.Width()+FrmMain_Shell_TrayWndRight+FrmMain_Shell_TrayWndLeft)/Steps;
				//Okno kontaktow chowane za lewa krawedzia ekranu
				if(FrmMainSlideEdge==1)
				{
					FrmMainSlideLeft = FrmMainSlideLeft + Steps;
					if(FrmMainSlideLeft>FrmMainRect.Left) FrmMainSlideLeft = FrmMainRect.Left;
				}
				//Okno kontaktow chowane za prawa krawedzia ekranu
				else
				{
					FrmMainSlideLeft = FrmMainSlideLeft - Steps;
					if(FrmMainSlideLeft<FrmMainRect.Left) FrmMainSlideLeft = FrmMainRect.Left;
				}
				if(!IsWindowVisible(hFrmMain)) ShowWindow(hFrmMain, SW_SHOWNA);
				//Zmiana pozycji okna kontaktow
				SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainSlideLeft,FrmMainRect.Top,0,0,SWP_NOSIZE);
			}
			//Okno kontaktow chowane za dolna/gorna krawedzia ekranu
			else
			{
				//Ustalanie ilosci krokow
				Steps = (FrmMainRect.Height()+FrmMain_Shell_TrayWndBottom+FrmMain_Shell_TrayWndTop)/Steps;
				//Okno kontaktow chowane za dolna krawedzia ekranu
				if(FrmMainSlideEdge==3)
				{
					FrmMainSlideTop = FrmMainSlideTop - Steps;
					if(FrmMainSlideTop<FrmMainRect.Top) FrmMainSlideTop = FrmMainRect.Top;
				}
				//Okno kontaktow chowane za gorna krawedzia ekranu
				else
				{
					FrmMainSlideTop = FrmMainSlideTop + Steps;
					if(FrmMainSlideTop>FrmMainRect.Top) FrmMainSlideTop = FrmMainRect.Top;
				}
				if(!IsWindowVisible(hFrmMain)) ShowWindow(hFrmMain, SW_SHOWNA);
				//Zmiana pozycji okna kontaktow
				SetWindowPos(hFrmMain,HWND_TOPMOST,FrmMainRect.Left,FrmMainSlideTop,0,0,SWP_NOSIZE);
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
				//Wylaczenie statusu okna na wierzchu
				if(FrmMainSlideHideMode==2) SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				//Aktywacja okna kontaktow + nadanie fokusa kontrolce IE
				ActivateAndFocusFrmMain();
				//Status wysuwania okna kontaktow zza krawedzi ekranu
				FrmMainSlideIn = false;
				//Stan widocznosci okna kontaktow
				FrmMainVisible = true;
				//Tymczasowa blokada chowania okna kontaktow
				if((FrmMainSlideHideMode==3)&&(!FrmMainDontBlockSlide))
				{
					//Tymczasowa blokada chowania/pokazywania okna kontaktow
					FrmMainBlockSlide = true;
					//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
					FrmMainUnBlockSlide = true;
					//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna kontaktow
					SetTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE,1500,(TIMERPROC)TimerFrmProc);
				}
				//Brak tymczasowej blokady po wysunieciu okna
				FrmMainDontBlockSlide = false;
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_FRMMAIN_SLIDEIN);
			}
		}
		//Wylaczenie tymczasowej blokady chowania/pokazywania okna kontaktow
		else if(wParam==TIMER_FRMMAIN_UNBLOCK_SLIDE)
		{
			//Zatrzymanie timera
			KillTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE);
			//Tymczasowa blokada chowania/pokazywania okna kontaktow
			FrmMainBlockSlide = false;
			//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
			FrmMainUnBlockSlide = false;
		}
		//Ustawienie okna kontaktow na wierzchu
		else if(wParam==TIMER_FRMMAIN_TOPMOST)
		{
			//Pobieranie klasy nowego okna
			wchar_t WindowCaptionNameW[128];
			GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
			UnicodeString WindowCaptionName = WindowCaptionNameW;
			//Wlaczenie timera ustawienia okna na wierzchu
			if((WindowCaptionName!="TaskSwitcherWnd")&&(WindowCaptionName!="DV2ControlHost")&&(WindowCaptionName!="CabinetWClass")&&(WindowCaptionName!="Shell_TrayWnd"))
			{
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_FRMMAIN_TOPMOST);
				//Pobranie uchwytu
				LastActiveWindow_PreFrmMainSlideIn = GetForegroundWindow();
				//Aktywacja okna kontaktow + nadanie fokusa kontrolce IE
				ActivateAndFocusFrmMain();
			}
		}
		//Ustawienie okna kontaktow na wierzchu przy dezaktywacji okna wyszukiwarki
		else if(wParam==TIMER_FRMMAIN_TOPMOST_EX)
		{
			//Pobranie PID okna
			DWORD PID;
			GetWindowThreadProcessId(GetForegroundWindow(), &PID);
			//Porownanie PID okna
			if(PID!=ProcessPID)
			{
				//Pobieranie klasy nowego okna
				wchar_t WindowCaptionNameW[128];
				GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
				UnicodeString WindowCaptionName = WindowCaptionNameW;
				//Porownanie klasy nowego aktywnego okna
				if((WindowCaptionName!="TaskSwitcherWnd")&&(WindowCaptionName!="DV2ControlHost")&&(WindowCaptionName!="CabinetWClass")&&(WindowCaptionName!="Shell_TrayWnd"))
				{
					//Zatrzymanie timera
					KillTimer(hTimerFrm,TIMER_FRMMAIN_TOPMOST_EX);
					//Ustawienie okna kontaktow na wierzchu
					SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				}
			}
		}
		//Ustawienie okna kontaktow na wierzchu i schowanie go
		else if(wParam==TIMER_FRMMAIN_TOPMOST_AND_SLIDEOUT)
		{
			//Pobieranie klasy nowego okna
			wchar_t WindowCaptionNameW[128];
			GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
			UnicodeString WindowCaptionName = WindowCaptionNameW;
			//Wlaczenie timera ustawienia okna na wierzchu
			if(WindowCaptionName!="TaskSwitcherWnd")
			{
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_FRMMAIN_TOPMOST_AND_SLIDEOUT);
				if(GetForegroundWindow()!=hFrmMain)
				{
					//Pobieranie nowego aktywnego okna
					LastActiveWindow_WmInactiveFrmMainSlide = GetForegroundWindow();
					//Status chowania okna kontaktow za krawedz ekranu
					FrmMainSlideOut = true;
					//Wlaczenie chowania okna kontaktow (part I)
					SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
				}
				else
				{
					//Pobranie uchwytu
					LastActiveWindow_PreFrmMainSlideIn = GetForegroundWindow();
					//Aktywacja okna kontaktow + nadanie fokusa kontrolce IE
					ActivateAndFocusFrmMain();
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
	//Kompozycja nie jest zmieniana / komunikator nie jest zamykany
	if((!ThemeChanging)&&(!ForceUnloadExecuted))
	{
		if((uMsg==WM_SETICON)&&(FrmMainSlideChk)&&(FrmMainSlideHideMode!=2)&&(FrmMainVisible))
		{
			//Pobieranie okna w ktorym znajduje sie kursor
			HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
			//Pobieranie klasy okna w ktorym znajduje sie kursor
			wchar_t WindowCaptionNameW[128];
			GetClassNameW(hCurActiveFrm, WindowCaptionNameW, sizeof(WindowCaptionNameW));
			UnicodeString WindowCaptionName = WindowCaptionNameW;
			//Pobranie PID okna w ktorym znajduje sie kursor
			DWORD PID;
			GetWindowThreadProcessId(hCurActiveFrm, &PID);
			//Gdy kursor znajduje sie w obrebie menu z okna aplikacji lub w oknie kontaktow
			if(((WindowCaptionName=="#32768")&&(PID==ProcessPID))||((Mouse->CursorPos.y>FrmMainRect.Top+FrmMainRealTopPos-FrmMain_Shell_TrayWndTop)&&(FrmMainRect.Bottom+FrmMainRealBottomPos+FrmMain_Shell_TrayWndBottom>Mouse->CursorPos.y)&&(Mouse->CursorPos.x>FrmMainRect.Left+FrmMainRealLeftPos-FrmMain_Shell_TrayWndLeft)&&(FrmMainRect.Right+FrmMainRealRightPos+FrmMain_Shell_TrayWndRight>Mouse->CursorPos.x)))
				SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			else
				SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}
		//Aktywacja lub dezaktywacja okna kontatow
		else if((uMsg==WM_ACTIVATE)&&(!LoadExecuted))
		{
			//Funkcjonalnosc chowania okna kontaktow
			if(FrmMainSlideChk)
			{
				//Okno zostalo zdezaktywowane przy aktywnej wyszukiwarce
				if((wParam==WA_INACTIVE)&&(hFrmSeekOnList))
				{
					//Nonwym aktywnym oknem nie jest okno wyszukiwarki
					if((HWND)lParam!=hFrmSeekOnList)
						//Wlaczenie timera ustawienia okna kontaktow na wierzchu
						SetTimer(hTimerFrm,TIMER_FRMMAIN_TOPMOST_EX,10,(TIMERPROC)TimerFrmProc);
				}
				//Okno zostalo dezaktywowane - schowanie okna
				if(wParam==WA_INACTIVE)
				{
					//Tymczasowa blokada chowania/pokazywania okna kontaktow
					if((FrmMainVisible)&&(FrmMainSlideHideMode==3)&&(FrmMainUnBlockSlide)) FrmMainBlockSlide = false;
					//Wlaczenie chowania okna kontaktow
					if((FrmMainVisible)&&(!PreFrmMainSlideOut)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
					{
						//Kursor znajduje sie poza oknem rozmowy
						if((Mouse->CursorPos.y<FrmMainRect.Top)||(FrmMainRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmMainRect.Left)||(FrmMainRect.Right<Mouse->CursorPos.x))
						{
							//Chowanie gdy aplikacja straci fokus
							if(FrmMainSlideHideMode==2)
							{
								//Pobranie PID procesu nowego okna
								DWORD PID;
								GetWindowThreadProcessId(GetForegroundWindow(), &PID);
								//Aktywne okno z innego procesu
								if(PID!=ProcessPID)
								{
									//Pobieranie klasy nowego aktywnego okna
									wchar_t WindowCaptionNameW[128];
									GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
									UnicodeString WindowCaptionName = WindowCaptionNameW;
									if((WindowCaptionName!="Shell_TrayWnd")
									&&(WindowCaptionName!="MSTaskListWClass")
									&&(WindowCaptionName!="NotifyIconOverflowWindow")
									&&(WindowCaptionName!="ClockFlyoutWindow")
									&&(WindowCaptionName!="DV2ControlHost")
									&&(WindowCaptionName!="TaskSwitcherWnd"))
									{
										//Status chowania okna kontaktow za krawedz ekranu
										FrmMainSlideOut = true;
										//Wlaczenie chowania okna kontaktow (part I)
										SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
									}
								}
							}
							//Chowanie gdy okno straci fokus
							else if(FrmMainSlideHideMode==1)
							{
								//Pobieranie klasy nowego aktywnego okna
								wchar_t WindowCaptionNameW[128];
								GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
								UnicodeString WindowCaptionName = WindowCaptionNameW;
								if(WindowCaptionName!="TaskSwitcherWnd")
								{
									//Pobieranie nowego aktywnego okna
									GetClassNameW(WindowFromPoint(Mouse->CursorPos), WindowCaptionNameW, sizeof(WindowCaptionNameW));
									UnicodeString WindowCaptionName = WindowCaptionNameW;
									if((WindowCaptionName!="Shell_TrayWnd")
									&&(WindowCaptionName!="EdgeUiInputWndClass")
									&&(WindowCaptionName!="ReBarWindow32")
									&&(WindowCaptionName!="MSTaskListWClass")
									&&(WindowCaptionName!="TrayNotifyWnd")
									&&(WindowCaptionName!="ToolbarWindow32")
									&&(WindowCaptionName!="TrayClockWClass")
									&&(WindowCaptionName!="TrayShowDesktopButtonWClass")
									&&(WindowCaptionName!="DV2ControlHost"))
										LastActiveWindow_WmInactiveFrmMainSlide = WindowFromPoint(Mouse->CursorPos);
									//Status chowania okna kontaktow za krawedz ekranu
									FrmMainSlideOut = true;
									//Wlaczenie chowania okna kontaktow (part I)
									SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
								}
								else
									SetTimer(hTimerFrm,TIMER_FRMMAIN_TOPMOST_AND_SLIDEOUT,10,(TIMERPROC)TimerFrmProc);
							}
							//Chowanie gdy kursor opusci okno
							else
							{
								//Pobieranie nowego aktywnego okna
								wchar_t WindowCaptionNameW[128];
								GetClassNameW(WindowFromPoint(Mouse->CursorPos), WindowCaptionNameW, sizeof(WindowCaptionNameW));
								UnicodeString WindowCaptionName = WindowCaptionNameW;
								if((WindowCaptionName!="Shell_TrayWnd")
								&&(WindowCaptionName!="EdgeUiInputWndClass")
								&&(WindowCaptionName!="ReBarWindow32")
								&&(WindowCaptionName!="MSTaskListWClass")
								&&(WindowCaptionName!="TrayNotifyWnd")
								&&(WindowCaptionName!="ToolbarWindow32")
								&&(WindowCaptionName!="TrayClockWClass")
								&&(WindowCaptionName!="TrayShowDesktopButtonWClass")
								&&(WindowCaptionName!="DV2ControlHost")
								&&(WindowCaptionName!="TaskSwitcherWnd"))
									LastActiveWindow_WmInactiveFrmMainSlide = WindowFromPoint(Mouse->CursorPos);
								//Status chowania okna kontaktow za krawedz ekranu
								FrmMainSlideOut = true;
								//Wlaczenie chowania okna kontaktow (part I)
								SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
							}
						}
						//Kursor znajduje sie w oknie ale zostalo zmienione aktywne okno
						else
						{
							//Pobranie PID procesu nowego okna
							DWORD PID;
							GetWindowThreadProcessId(GetForegroundWindow(), &PID);
							//Wlaczenie timera ustawienia okna na wierzchu i pobranie nowego uchwytu
							if(PID!=ProcessPID) SetTimer(hTimerFrm,TIMER_FRMMAIN_TOPMOST,10,(TIMERPROC)TimerFrmProc);
						}
					}
					//Wylaczenie chowania okna kontaktow
					else if((FrmMainVisible)&&(PreFrmMainSlideOut)&&(FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
					{
						//Kursor znajduje sie w oknie rozmowy
						if((Mouse->CursorPos.y>FrmMainRect.Top)&&(FrmMainRect.Bottom>Mouse->CursorPos.y)&&(Mouse->CursorPos.x>FrmMainRect.Left)&&(FrmMainRect.Right>Mouse->CursorPos.x))
						{
							//Zatrymanie timera
							KillTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT);
							//Status pre-chowania okna kontaktow za krawedz ekranu
							PreFrmMainSlideOut = false;
							//Status chowania okna kontaktow za krawedz ekranu
							FrmMainSlideOut = false;
							//Usuniecie uchwytu do nowego aktywnego okna
							LastActiveWindow_WmInactiveFrmMainSlide = NULL;
						}
					}
				}
				//Okno zostalo aktywowane - pokazanie okna
				if(wParam==WA_ACTIVE)
				{
					//Pobranie klasy poprzednio aktywnego okna
					wchar_t WindowCaptionNameW[128];
					GetClassNameW(LastActiveWindow, WindowCaptionNameW, sizeof(WindowCaptionNameW));
					UnicodeString WindowCaptionName = WindowCaptionNameW;
					//Pobranie klasy okna spod kursora
					wchar_t WindowCaptionNameW2[128];
					GetClassNameW(WindowFromPoint(Mouse->CursorPos), WindowCaptionNameW2, sizeof(WindowCaptionNameW2));
					UnicodeString WindowCaptionName2 = WindowCaptionNameW2;
					//Okno jest schowane i spelnia inne ponizsze warunki
					if((!FrmMainVisible)&&(!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!IsIconic(LastActiveWindow))&&((IsWindowVisible(LastActiveWindow))||(WindowCaptionName=="TaskSwitcherWnd"))&&(WindowCaptionName2!="TrayShowDesktopButtonWClass")&&((WindowFromPoint(Mouse->CursorPos)!=hToolbarWindow32)))
					{
						//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
						ChkFullScreenMode();
						//Blokowanie wysuwania przy aplikacji pelnoekranowej
						if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
						{ /* Blokada */ }
						else
						{
							//Status wysuwania okna kontaktow zza krawedzi ekranu
							FrmMainSlideIn = true;
							//Wlacznie FrmMainSlideIn (part II)
							SetTimer(hTimerFrm,TIMER_FRMMAIN_SLIDEIN,FrmMainStepInterval,(TIMERPROC)TimerFrmProc);
						}
					}
				}
			}
		}
		//Zabezpieczenie przed "zamknieciem" okna przy aktywnym chowaniu okna za krawedz ekranu
		else if((uMsg==WM_CLOSE)&&(FrmMainSlideChk))
		{
			//Wylaczenie tymczasowej blokady
			if((FrmMainVisible)&&(FrmMainSlideHideMode==3)&&(FrmMainUnBlockSlide)) FrmMainBlockSlide = false;
			//Wlaczenie chowania okna kontaktow
			if((FrmMainVisible)&&(!PreFrmMainSlideOut)&&(!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
			{
				//Status chowania okna kontaktow za krawedz ekranu
				FrmMainSlideOut = true;
				//Wlaczenie chowania okna kontaktow (part I)
				SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
			}
			//Zabezpieczenie przed "zamknieciem" okna
			return 1;
		}
		//Wlaczenie tymczasowej blokady chowania okna kontatkow
		else if((uMsg==WM_ENTERSIZEMOVE)&&(FrmMainSlideChk)&&(FrmMainVisible)&&(!FrmMainSlideIn)&&(!FrmMainSlideOut))
		{
			//Tymczasowa blokada chowania/pokazywania okna kontaktow
			FrmMainBlockSlide = true;
			//Odznaczenie zdarzenia zmiany rozmiaru okna
			WM_ENTERSIZEMOVE_BLOCK = true;
		}
		//Wylaczenie tymczasowej blokady chowania okna kontatkow
		else if((uMsg==WM_EXITSIZEMOVE)&&(WM_ENTERSIZEMOVE_BLOCK))
		{
			//Odznaczenie zdarzenia zmiany rozmiaru okna
			WM_ENTERSIZEMOVE_BLOCK = false;
			//Pobranie rozmiaru+pozycji okna kontatkow
			GetFrmMainRect();
			//Ustawienie poprawnej pozycji okna kontaktow
			SetFrmMainPos();
			//Pobranie rozmiaru+pozycji okna kontatkow
			GetFrmMainRect();
			//Wylaczenie tymczasowej blokady
			if(FrmMainSlideHideMode==3)
			{
				//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
				FrmMainUnBlockSlide = true;
				//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna kontaktow
				SetTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE,1500,(TIMERPROC)TimerFrmProc);
			}
			//Tymczasowa blokada chowania/pokazywania okna kontaktow
			else FrmMainBlockSlide = false;
		}
	}
	//Przypisanie starej procki do okna kontaktow
	if(uMsg==WM_CLOSE)
	{
		//Procka nie zostala jeszcze przywrocona
		if(OldFrmMainProc)
		{
			//Przywrocenie wczesniej zapisanej procki
			SetWindowLongPtrW(hwnd, GWLP_WNDPROC,(LONG_PTR)OldFrmMainProc);
			//Skopiowanie procki do zmiennej tymczasowej
			WNDPROC tmpOldFrmMainProc = OldFrmMainProc;
			//Skasowanie procki
			OldFrmMainProc = NULL;
			//Zwrot w funkcji
			return CallWindowProc(tmpOldFrmMainProc, hwnd, uMsg, wParam, lParam);
		}
	}

	return CallWindowProc(OldFrmMainProc, hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Procka okna rozmowy
LRESULT CALLBACK FrmSendProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Kompozycja nie jest zmieniana / komunikator nie jest zamykany
	if((!ThemeChanging)&&(!ForceUnloadExecuted))
	{
		if(uMsg==WM_SETICON)
		{
			//Wlaczony jest licznik nowych wiadomosci na oknie rozmowy oraz licznik jest rozny od 0
			if((InactiveFrmNewMsgChk)&&(InactiveFrmNewMsgCount))
			{
				//Okno rozmowy jest nie aktywne
				if(GetForegroundWindow()!=hFrmSend)
					SetWindowTextW(hFrmSend,("["+IntToStr(InactiveFrmNewMsgCount)+"] "+FrmSendTitlebar).w_str());
			}
			//Wlaczona jest opcja zmiany caption okna rozmowy
			if(TweakFrmSendTitlebarChk)
			{
				//Pobranie aktualnego tekstu belki okna
				wchar_t TitlebarW[512];
				GetWindowTextW(hFrmSend, TitlebarW, sizeof(TitlebarW));
				UnicodeString Titlebar = TitlebarW;
				Titlebar = StringReplace(Titlebar, "\r\n", "", TReplaceFlags() << rfReplaceAll);
				//Sprawdzanie czy belka zostal juz zmieniona ostatnio
				UnicodeString ChangedTitlebar = DecodeBase64(ChangedTitlebarList->ReadString("Titlebar", MD5(Titlebar), ""));
				//Ustawianie nowego tekstu na belce okna
				if((!ChangedTitlebar.IsEmpty())&&(Titlebar!=ChangedTitlebar))
					SetWindowTextW(hFrmSend,ChangedTitlebar.w_str());
			}
		}
		//Blokowanie ustawiania domyslnego tekstu na belce okna
		else if(uMsg==0x000000ae)
		{
			//Wlaczona jest opcja zmiany caption okna rozmowy i okno jest aktywne
			if((TweakFrmSendTitlebarChk))//&&(GetActiveWindow()==hFrmSend))
			{
				//Pobranie aktualnego tekstu belki okna
				wchar_t TitlebarW[512];
				GetWindowTextW(hFrmSend, TitlebarW, sizeof(TitlebarW));
				UnicodeString Titlebar = TitlebarW;
				Titlebar = StringReplace(Titlebar, "\r\n", "", TReplaceFlags() << rfReplaceAll);
				Titlebar = StringReplace(Titlebar, "	", " ", TReplaceFlags() << rfReplaceAll);
				//Sprawdzanie czy belka zostal juz zmieniona ostatnio
				UnicodeString ChangedTitlebar = DecodeBase64(ChangedTitlebarList->ReadString("Titlebar", MD5(Titlebar), ""));
				//Ustawianie nowego tekstu na belce okna
				if((!ChangedTitlebar.IsEmpty())&&(Titlebar!=ChangedTitlebar))
					SetWindowTextW(hFrmSend,ChangedTitlebar.w_str());
			}
		}
		//Aktywacja lub dezaktywacja okna rozmowy
		else if(uMsg==WM_ACTIVATE)
		{
			//Okno zostalo aktywowane za pomoca miniaturki z paska zadan
			if((FrmSendShownByThumbnail)&&((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE)))
			{
				//Zatrzymanie timera odblokowania pokazywania okna rozmowy poprzez miniaturke z paska zadan
				KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_THUMBNAIL);
				//Odznaczenie wlaczenia timera odblokowania pokazywania okna rozmowy poprzez miniaturke z paska zadan
				FrmSendShownByThumbnailTimer = false;
				//Odznaczenie pokazania okna rozmowy za pomoca miniaturki
				FrmSendShownByThumbnail = false;
				//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
				if(FrmSendSlideHideMode==3) FrmSendUnBlockSlide = true;
				//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
				if(FrmSendSlideHideMode==3) SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,2000,(TIMERPROC)TimerFrmProc);
			}
			//Zmiana tekstu na belce okna
			if(InactiveFrmNewMsgChk)
			{
				//Okno zostalo aktywowane
				if((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE))
				{
					//Przywracanie poprzedniego tekstu na belce okna
					if(!FrmSendTitlebar.IsEmpty())
					{
						SetWindowTextW(hFrmSend,FrmSendTitlebar.w_str());
						FrmSendTitlebar = "";
					}
					//Kasowanie licznika nowych wiadomosci
					InactiveFrmNewMsgCount = 0;
				}
			}
			//Miganie diodami LED klawiatury - wylaczanie mrugania
			if((KeyboardFlasherChk)&&(hFlasherThread)&&(hFlasherKeyboardThread))
			{
				//Okno zostalo aktywowane
				if((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE))
				{
					//Usuwanie z listy nieprzeczytanych wiadomosci aktywnej zakladki
					if(UnreadMsgList->IndexOf(ActiveTabJIDEx)!=-1)
						UnreadMsgList->Delete(UnreadMsgList->IndexOf(ActiveTabJIDEx));
					//Nie ma juz nieprzeczytanych wiadomosci
					if(!UnreadMsgList->Count)
					{
						SetEvent(hFlasherThread);
						WaitForSingleObject(hFlasherKeyboardThread, 30000);
						CloseHandle(hFlasherThread);
						hFlasherThread = NULL;
						hFlasherKeyboardThread = NULL;
					}
				}
			}
			//Notyfikcja pisania wiadomosci
			if(ChatStateNotiferNewMsgChk)
			{
				//Okno zostalo aktywowane
				if((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE))
				{
					//Resetowanie poprzedniego stanu pisania wiadomosci
					LastChatState = 0;
					//Ustawienie oryginalnej malej ikonki
					if(hIconSmall)
					{
						SendMessage(hFrmSend, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
						hIconSmall = 0;
					}
					//Ustawienie oryginalnej duzej ikonki
					if(hIconBig)
					{
						SendMessage(hFrmSend, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
						hIconBig = 0;
					}
				}
			}
			//Funkcjonalnosc chowania okna rozmowy
			if(FrmSendSlideChk)
			{
				//Okno zostalo dezaktywowane - schowanie okna
				if(wParam==WA_INACTIVE)
				{
					//Wylaczenie tymczasowej blokady
					if((FrmSendVisible)&&(FrmSendSlideHideMode==3)&&(FrmSendUnBlockSlide))
					{
						//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
						KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE);
						//Tymczasowa blokada chowania/pokazywania okna rozmowy
						if(StayOnTopChk) FrmSendBlockSlide = StayOnTopStatus;
						else FrmSendBlockSlide = false;
						//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
						FrmSendUnBlockSlide = false;
					}
					//Wlaczenie chowania okna rozmowy
					if((FrmSendVisible)&&(!PreFrmSendSlideOut)&&(!FrmSendBlockSlide)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))
					{
						//Kursor znajduje sie poza oknem rozmowy
						if((Mouse->CursorPos.y<FrmSendRect.Top)||(FrmSendRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmSendRect.Left)||(FrmSendRect.Right<Mouse->CursorPos.x))
						{
							//Chowanie gdy aplikacja straci fokus
							if(FrmSendSlideHideMode==2)
							{
								//Pobranie PID procesu nowego okna
								DWORD PID;
								GetWindowThreadProcessId(GetForegroundWindow(), &PID);
								//Aktywne okno z innego procesu
								if(PID!=ProcessPID)
								{
									//Pobieranie klasy nowego aktywnego okna
									wchar_t WindowCaptionNameW[128];
									GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
									UnicodeString WindowCaptionName = WindowCaptionNameW;
									if((WindowCaptionName!="Shell_TrayWnd")
									&&(WindowCaptionName!="MSTaskListWClass")
									&&(WindowCaptionName!="NotifyIconOverflowWindow")
									&&(WindowCaptionName!="ClockFlyoutWindow")
									&&(WindowCaptionName!="DV2ControlHost")
									&&(WindowCaptionName!="TaskSwitcherWnd"))
									{
										//Status chowania okna rozmowy za krawedz ekranu
										FrmSendSlideOut = true;
										//Wlaczenie chowania okna rozmowy (part I)
										SetTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
									}
								}
							}
							//Chowanie gdy okno straci fokus
							else if(FrmSendSlideHideMode==1)
							{
								//Pobieranie klasy nowego aktywnego okna
								wchar_t WindowCaptionNameW[128];
								GetClassNameW(GetForegroundWindow(), WindowCaptionNameW, sizeof(WindowCaptionNameW));
								UnicodeString WindowCaptionName = WindowCaptionNameW;
								if(WindowCaptionName!="TaskSwitcherWnd")
								{
									//Pobieranie nowego aktywnego okna
									GetClassNameW(WindowFromPoint(Mouse->CursorPos), WindowCaptionNameW, sizeof(WindowCaptionNameW));
									WindowCaptionName = WindowCaptionNameW;
									if((WindowCaptionName!="Shell_TrayWnd")
									&&(WindowCaptionName!="EdgeUiInputWndClass")
									&&(WindowCaptionName!="ReBarWindow32")
									&&(WindowCaptionName!="MSTaskListWClass")
									&&(WindowCaptionName!="TrayNotifyWnd")
									&&(WindowCaptionName!="ToolbarWindow32")
									&&(WindowCaptionName!="TrayClockWClass")
									&&(WindowCaptionName!="TrayShowDesktopButtonWClass")
									&&(WindowCaptionName!="DV2ControlHost"))
										LastActiveWindow_WmInactiveFrmSendSlide = WindowFromPoint(Mouse->CursorPos);
									//Status chowania okna rozmowy za krawedz ekranu
									FrmSendSlideOut = true;
									//Wlaczenie chowania okna rozmowy (part I)
									SetTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
								}
								else SetTimer(hTimerFrm,TIMER_FRMSEND_TOPMOST_AND_SLIDEOUT,10,(TIMERPROC)TimerFrmProc);
							}
							//Chowanie gdy kursor opusci okno
							else
							{
								//Pobieranie nowego aktywnego okna
								wchar_t WindowCaptionNameW[128];
								GetClassNameW(WindowFromPoint(Mouse->CursorPos), WindowCaptionNameW, sizeof(WindowCaptionNameW));
								UnicodeString WindowCaptionName = WindowCaptionNameW;
								if((WindowCaptionName!="Shell_TrayWnd")
								&&(WindowCaptionName!="EdgeUiInputWndClass")
								&&(WindowCaptionName!="ReBarWindow32")
								&&(WindowCaptionName!="MSTaskListWClass")
								&&(WindowCaptionName!="TrayNotifyWnd")
								&&(WindowCaptionName!="ToolbarWindow32")
								&&(WindowCaptionName!="TrayClockWClass")
								&&(WindowCaptionName!="TrayShowDesktopButtonWClass")
								&&(WindowCaptionName!="DV2ControlHost"))
									LastActiveWindow_WmInactiveFrmSendSlide = WindowFromPoint(Mouse->CursorPos);
								//Status chowania okna rozmowy za krawedz ekranu
								FrmSendSlideOut = true;
								//Wlaczenie chowania okna rozmowy (part I)
								SetTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
							}
						}
						//Kursor znajduje sie w oknie ale zostalo zmienione aktywne okno
						else
						{
							//Pobranie PID procesu nowego okna
							DWORD PID;
							GetWindowThreadProcessId(GetForegroundWindow(), &PID);
							//Wlaczenie timera pobranie nowego uchwytu i aktywacji okna rozmowy
							if(PID!=ProcessPID) SetTimer(hTimerFrm,TIMER_FRMSEND_TOPMOST,100,(TIMERPROC)TimerFrmProc);
						}
					}
					//Wylaczenie chowania okna rozmowy
					else if((FrmSendVisible)&&(PreFrmSendSlideOut)&&(FrmSendSlideOut)&&(!FrmSendSlideIn)&&(!FrmSendBlockSlide))
					{
						if((Mouse->CursorPos.y>FrmSendRect.Top)&&(FrmSendRect.Bottom>Mouse->CursorPos.y)&&(Mouse->CursorPos.x>FrmSendRect.Left)&&(FrmSendRect.Right>Mouse->CursorPos.x))
						{
							//Zatrzymanie timera
							KillTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEOUT);
							//Status chowania okna rozmowy za krawedz ekranu
							FrmSendSlideOut = false;
							//Status pre-chowania okna rozmowy za krawedz ekranu
							PreFrmSendSlideOut = false;
							//Usuniecie uchwytu do nowego aktywnego okna
							LastActiveWindow_WmInactiveFrmSendSlide= NULL;
						}
					}
				}
				//Okno zostalo aktywowane - pokazanie okna
				if(wParam==WA_ACTIVE)
				{
					//Pobranie klasy poprzednio aktywnego okna
					wchar_t WindowCaptionNameW[128];
					GetClassNameW(LastActiveWindow, WindowCaptionNameW, sizeof(WindowCaptionNameW));
					UnicodeString WindowCaptionName = WindowCaptionNameW;
					//Pobranie klasy okna spod kursora
					wchar_t WindowCaptionNameW2[128];
					GetClassNameW(WindowFromPoint(Mouse->CursorPos), WindowCaptionNameW2, sizeof(WindowCaptionNameW2));
					UnicodeString WindowCaptionName2 = WindowCaptionNameW2;
					//Okno jest schowane i spelnia inne ponizsze warunki
					if((!FrmSendVisible)&&(!FrmSendBlockSlide)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn)&&(!IsIconic(LastActiveWindow))&&((IsWindowVisible(LastActiveWindow))||(WindowCaptionName=="TaskSwitcherWnd")||(WindowCaptionName2=="MSTaskListWClass")||(WindowCaptionName2=="ToolbarWindow32"))&&(WindowCaptionName2!="TrayShowDesktopButtonWClass"))
					{
						//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
						ChkFullScreenMode();
						//Blokowanie wysuwania przy aplikacji pelnoekranowej
						if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
						{ /* Blokada */ }
						else
						{
							//Status wysuwania okna rozmowy zza krawedzi ekranu
							FrmSendSlideIn = true;
							//Pobieranie pozycji okna rozmowy
							TRect WindowRect;
							GetWindowRect(hFrmSend,&WindowRect);
							//Odswiezenie okna rozmowy
							SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height()+1,SWP_NOMOVE);
							SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
							//Wlaczenie pokazywania okna rozmowy (part II)
							SetTimer(hTimerFrm,TIMER_FRMSEND_SLIDEIN,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
						}
					}
				}
			}
		}
		//Ulegl zmianie rozmiar okna
		else if(uMsg==WM_SIZE)
		{
			//Blokada minimalizacji okna rozmowy
			if((wParam==SIZE_MINIMIZED)&&(FrmSendSlideChk)&&(!FrmPosExist))
			{
				//Okno rozmowy jest widoczne
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
						StayOnTopItem.pszCaption = GetLangStr("HoldOnTop").w_str();
						StayOnTopItem.Hint = GetLangStr("HoldOnTop").w_str();
						StayOnTopItem.IconIndex = STAYONTOP_OFF;
						StayOnTopItem.Handle = (int)hFrmSend;
						PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&StayOnTopItem));
						//Stan StayOnTop
						StayOnTopStatus = false;
					}
					//Tymczasowa blokada chowania/pokazywania okna rozmowy
					FrmSendBlockSlide = true;
					//Wlaczenie timera
					SetTimer(hTimerFrm,TIMER_FRMSEND_MINIMIZED,10,(TIMERPROC)TimerFrmProc);
				}
				//Okno rozmowy jest niewidoczne
				else
				{
					//Sprawdzenie klasy okna spod kursora
					wchar_t WindowCaptionNameW[128];
					GetClassNameW(WindowFromPoint(Mouse->CursorPos), WindowCaptionNameW, sizeof(WindowCaptionNameW));
					UnicodeString WindowCaptionName = WindowCaptionNameW;
					if(WindowCaptionName!="TrayShowDesktopButtonWClass")
					{
						//Status wysuwania okna rozmowy zza krawedzi ekranu
						FrmSendSlideIn = true;
						//Wlaczenie pokazywania okna rozmowy (part I)
						SetTimer(hTimerFrm,TIMER_FRMSEND_PRE_SLIDEIN,1,(TIMERPROC)TimerFrmProc);
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
		//Ukrywanie/pokazywanie paska narzedzi
		else if(uMsg==WM_SETCURSOR)
		{
			//Funkcjonalnosc ukrywania paska narzedzi jest wlaczona
			if(HideToolBarChk)
			{
				//Pobieranie okna spod kursora
				HWND hCurActiveFrm = WindowFromPoint(Mouse->CursorPos);
				//Okno rozmowy jest aktywne i kursor znajduje sie w oknie
				if((GetForegroundWindow()==hFrmSend)&&((hCurActiveFrm==hFrmSend)||(IsChild(hFrmSend,hCurActiveFrm))))
				{
					//Pobieranie pozycji okna rozmowy
					TRect WindowRect;
					GetWindowRect(hFrmSend,&WindowRect);
					//Pozycja dolnej krawedzi okna
					int WindowBottom = WindowRect.Bottom;
					//Pozycja Y kursora
					int CursorY = Mouse->CursorPos.y;
					//Pozycja kursora w oknie rozmowy
					int CursorPos = WindowBottom - CursorY;
					//Wysokosc paska informacyjnego
					if(!hStatusBar) hStatusBar = FindWindowEx(hFrmSend,NULL,L"TsStatusBar",NULL);
					GetWindowRect(hStatusBar,&WindowRect);
					int StatusBarHeight = WindowRect.Height();
					//Pokazywanie paska narzedzi
					if(CursorPos<32+StatusBarHeight)
					{
						//Timer pokazywania paska narzedzi nie zostal wlaczony
						if(!ToolBarShowing)
						{
							//Pobieranie pozycji paska narzedzi
							GetWindowRect(hToolBar,&WindowRect);
							//Pasek narzedzi jest ukryty
							if(!WindowRect.Height())
							{
								//Odznaczenie wlaczenia timera pokazywania paska narzedzi
								ToolBarShowing = true;
								//Wylaczenie timera pokazywania paska narzedzi
								KillTimer(hTimerFrm,TIMER_SHOW_TOOLBAR);
								//Wlaczenie timera pokazywania paska narzedzi
								SetTimer(hTimerFrm,TIMER_SHOW_TOOLBAR,500,(TIMERPROC)TimerFrmProc);
							}
						}
					}
					//Ukrywanie paska narzedzi
					else
					{
						//Timer pokazywania paska narzedzi zostal wlaczony
						if(ToolBarShowing)
						{
							//Zatrzymanie timera pokazywania paska narzedzi
							KillTimer(hTimerFrm,TIMER_SHOW_TOOLBAR);
							//Odznaczenie wlaczenia timera pokazywania paska narzedzi
							ToolBarShowing = false;
						}
						//Ukrycie paska narzedzi
						HideToolBar();
					}
				}
			}
		}
		//Wlaczenie tymczasowej blokady chowania okna rozmowy
		else if((uMsg==WM_ENTERSIZEMOVE)&&(FrmSendSlideChk)&&(FrmSendVisible)&&(!FrmSendSlideIn)&&(!FrmSendSlideOut)&&(!FrmSendBlockSlide))
		{
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			FrmSendBlockSlide = true;
			//Odznaczenie zdarzenia zmiany rozmiaru okna
			WM_ENTERSIZEMOVE_BLOCK = true;
		}
		//Wylaczenie tymczasowej blokady chowania okna kontatkow
		else if((uMsg==WM_EXITSIZEMOVE)&&(WM_ENTERSIZEMOVE_BLOCK))
		{
			//Odznaczenie zdarzenia zmiany rozmiaru okna
			WM_ENTERSIZEMOVE_BLOCK = false;
			//Pobranie rozmiaru+pozycji okna rozmowy
			GetFrmSendRect();
			//Ustawienie poprawnej pozycji okna rozmowy
			SetFrmSendPos();
			//Pobranie rozmiaru+pozycji okna rozmowy
			GetFrmSendRect();
			//Wlaczenie timera wylaczania tymczasowej blokady chowania/pokazywania okna rozmowy
			if(FrmSendSlideHideMode==3)
			{
				//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
				FrmSendUnBlockSlide = true;
				//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
				SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,2000,(TIMERPROC)TimerFrmProc);
			}
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			else FrmSendBlockSlide = false;
		}
	}
	//Przypisanie starej procki do okna rozmowy
	if(uMsg==WM_CLOSE)
	{
		//Procka nie zostala jeszcze przywrocona
		if(OldFrmSendProc)
		{
			//Przywrocenie wczesniej zapisanej procki
			SetWindowLongPtrW(hwnd, GWLP_WNDPROC,(LONG_PTR)OldFrmSendProc);
			//Skopiowanie procki do zmiennej tymczasowej
			WNDPROC tmpOldFrmSendProc = OldFrmSendProc;
			//Skasowanie procki
			OldFrmSendProc = NULL;
			//Zwrot w funkcji
			return CallWindowProc(tmpOldFrmSendProc, hwnd, uMsg, wParam, lParam);
		}
	}

	return CallWindowProc(OldFrmSendProc, hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Procka okna wyszukiwarki
LRESULT CALLBACK FrmSeekOnListProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Kompozycja nie jest zmieniana / komunikator nie jest zamykany
	if((!ThemeChanging)&&(!ForceUnloadExecuted))
	{
		//Okno wyszukiwarki zostalo zdezaktywowane
		if((uMsg==WM_ACTIVATE)&&(wParam==WA_INACTIVE)&&(FrmMainSlideChk))
		{
			//Nonwym aktywnym oknem nie jest okno kontatkow
			if((HWND)lParam!=hFrmMain)
				//Wlaczenie timera ustawienia okna kontaktow na wierzchu
				SetTimer(hTimerFrm,TIMER_FRMMAIN_TOPMOST_EX,10,(TIMERPROC)TimerFrmProc);
		}
	}
	//Przypisanie starej procki do okna wyszukiwarki
	if(uMsg==WM_CLOSE)
	{
		//Procka nie zostala jeszcze przywrocona
		if(OldFrmSeekOnListProc)
		{
			//Przywrocenie wczesniej zapisanej procki
			SetWindowLongPtrW(hwnd, GWLP_WNDPROC,(LONG_PTR)OldFrmSeekOnListProc);
			//Skopiowanie procki do zmiennej tymczasowej
			WNDPROC tmpOldFrmSeekOnListProc = OldFrmSeekOnListProc;
			//Skasowanie procki
			OldFrmSeekOnListProc = NULL;
			//Zwrot w funkcji
			return CallWindowProc(tmpOldFrmSeekOnListProc, hwnd, uMsg, wParam, lParam);
		}
	}

	return CallWindowProc(OldFrmSeekOnListProc, hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Lokalny hook na klawiature
LRESULT CALLBACK ThreadKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//Blad lub komunikator jest zamykany
	if((nCode<0)||(ForceUnloadExecuted)) return CallNextHookEx(hThreadKeyboard, nCode, wParam, lParam);
	//wParam i lParam zawieraja informacje o stanie klawiatury
	else if((nCode==HC_ACTION)&&(!(HIWORD(lParam)&KF_UP)))
	{
		//Blokowanie wszystich klawiszy gdy okno kontaktow jest chowane/wysuwane lub jest juz schowane i posiada fokus
		if((FrmMainSlideChk)&&((FrmMainSlideIn)||(FrmMainSlideOut)||(!FrmMainVisible)))
		{
			//Sprawdzanie aktywnego okna
			if(GetForegroundWindow()==hFrmMain)
				//Blokada wcisniecia klawiszy
				return -1;
		}
		//Blokowanie wszystich klawiszy gdy okno rozmowy jest chowane/wysuwane lub jest juz schowane i posiada fokus
		if((FrmSendSlideChk)&&((FrmSendSlideIn)||(FrmSendSlideOut)||(!FrmSendVisible)||(BlockThreadKeyboardProc)))
		{
			//Sprawdzanie aktywnego okna
			if(GetForegroundWindow()==hFrmSend)
				//Blokada wcisniecia klawiszy
				return -1;
		}
		//Blokowanie klawiszu Esc przy wlaczonym chowaniu okna kontaktow
		if(FrmMainSlideChk)
		{
			//Wcisniecie klawisza ESC
			if(wParam==VK_ESCAPE)
			{
				//Sprawdzanie aktywnego okna
				if(GetForegroundWindow()==hFrmMain)
				{
					//Tymczasowa blokada chowania/pokazywania okna kontaktow
					if((FrmMainSlideHideMode==3)&&(FrmMainUnBlockSlide)) FrmMainBlockSlide = false;
					//Wlaczenie FrmMainSlideOut
					if((!FrmMainBlockSlide)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn))
					{
						//Status chowania okna kontaktow za krawedz ekranu
						FrmMainSlideOut = true;
						//Wlaczenie chowania okna kontaktow (part I)
						SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
					}
					//Blokada wcisniecia klawiszy
					return -1;
				}
			}
		}
		//Pomijanie przypietych zakladek w przelaczaniu
		if((ClipTabsChk)&&(ExClipTabsFromTabSwitchingChk)&&((!ExClipTabsFromSwitchToNewMsgChk)||((ExClipTabsFromSwitchToNewMsgChk)&&(!MsgList->Count))))
		{
			//Wcisniety Ctrl+Tab lub Ctrl+Shift+Tab
			if(((GetKeyState(VK_CONTROL)<0)&&(GetKeyState(VK_LSHIFT)>=0)&&(wParam==VK_TAB))
			||((GetKeyState(VK_CONTROL)<0)&&(GetKeyState(VK_LSHIFT)<0)&&(wParam==VK_TAB)))
			{
				//Sprawdzanie aktywnego okna i ilosci zakladek
				if((GetForegroundWindow()==hFrmSend)&&(TabsList->Count>1))
				{
					//Przelaczanie w przod lub w tyl
					bool Shift;
					if(GetKeyState(VK_LSHIFT)<0) Shift = true;
					else Shift = false;
					//Pobieranie pozycji aktywnej zakladki
					int ActiveTabInx = TabsListEx->IndexOf(ActiveTabJIDEx);
					//Okreslenie pozycji poprzedniej zakladki
					int TabInx;
					if(Shift) TabInx = ActiveTabInx-1;
					else TabInx = ActiveTabInx+1;
					//Petla z warunkiem kiedy dojdziemy do aktywnej zakladki
					while(TabInx!=ActiveTabInx)
					{
						//Zakladka poza lista - ustawienie indeksu ostatniej zakladki
						if(((!Shift)&&(TabInx==TabsListEx->Count))||((Shift)&&(TabInx==-1)))
						{
							//Indeks ostatniej zakladki
							if(Shift) TabInx = TabsListEx->Count-1;
							else TabInx = 0;
							//Zakonczenie petli
							if(TabInx==ActiveTabInx) return -1;
						}
						//Pobieranie JID zakladki
						UnicodeString JID = TabsListEx->Strings[TabInx];
						//Usuwanie zasobu z JID
						if(JID.Pos("/"))
						{
							UnicodeString UserIdx = JID;
							while(UserIdx.Pos(":")) UserIdx.Delete(1,UserIdx.Pos(":"));
							JID.Delete(JID.Pos("/"),JID.Length());
							JID = JID + ":" + UserIdx;
						}
						//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
						TIniFile *Ini = new TIniFile(SessionFileDir);
						bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID);
						delete Ini;
						//Jezeli zakladka nie jest przypieta lub jest przypieta i ma widocza nazwe
						if((ClipTabsList->IndexOf(JID)==-1)||((ClipTabsList->IndexOf(JID)!=-1)&&(ClipTabsEx)))
						{
							//Pobieranie pelnego identyfikatora nowej zakladki
							JID = TabsListEx->Strings[TabInx];
							//Zmiana aktywnej zakladki na wskazany kontakt/czat
							ChangeActiveTab(JID);
							//Blokada wcisniecia klawiszy
							return -1;
						}
						//Kolejna zakladka
						if(Shift) TabInx--;
						else TabInx++;
					}
					//Blokada wcisniecia klawiszy
					return -1;
				}
			}
		}
		//Inteligentne przelaczenia zakladek
		if(SwitchToNewMsgChk)
		{
			//Wcisniety Ctrl+Tab
			if((GetKeyState(VK_CONTROL)<0)&&(wParam==VK_TAB))
			{
				//Sprawdzanie aktywnego okna
				if(GetForegroundWindow()==hFrmSend)
				{
					//Jezeli tablica cos zawiera
					if(MsgList->Count>0)
					{
						//Zmienna identyfikatora
						UnicodeString JID;
						//Sposob przelaczania
						if(SwitchToNewMsgMode==1)
							JID = MsgList->Strings[0];
						else
							JID = MsgList->Strings[MsgList->Count-1];
						//Pobrany identyfikator nie jest pusty
						if(!JID.IsEmpty())
						{
							//Zmiana aktywnej zakladki na wskazany kontakt/czat
							ChangeActiveTab(JID);
							//Blokada wcisniecia klawiszy
							return -1;
						}
					}
				}
			}
		}
		//Skroty do konretnych zakladek
		if(TabsHotKeysChk)
		{
			//Wscisniete przyciski F1-F12 lub Ctrl + 1-9
			if(((GetKeyState(VK_MENU)>=0)&&(GetKeyState(VK_SHIFT)>=0))&&
			(((TabsHotKeysMode==1)&&((GetKeyState(VK_CONTROL)>=0)&&((int)wParam>=112)&&((int)wParam<=123)))
			||((TabsHotKeysMode==2)&&((GetKeyState(VK_CONTROL)<0)&&((int)wParam>=49)&&((int)wParam<=57)))))
			{
				//Sprawdzanie aktywnego okna
				if(GetForegroundWindow()==hFrmSend)
				{
					//Identyfikacja klawisza
					int Key;
					if(TabsHotKeysMode==1) Key = (int)wParam - 111;
					else Key = (int)wParam - 48;
					//Niepomijanie przypietych zakladek
					if((!ClipTabsChk)||((ClipTabsChk)&&(!ExClipTabsFromTabsHotKeysChk)))
					{
						//Sprawdzanie czy wywolujemy zakladke "ducha"
						if(Key<=TabsListEx->Count)
						{
							//Pobieranie JID
							UnicodeString JID = TabsListEx->Strings[Key-1];
							//Sprawdzanie rodzaju kontaktu
							if(!JID.IsEmpty())
							{
								//Zmiana aktywnej zakladki na wskazany kontakt/czat
								ChangeActiveTab(JID);
								//Blokada wcisniecia klawiszy
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
								//Pobieranie identyfikatora kontaktu
								UnicodeString JID = UnOpenMsgList->Strings[0];
								//Otwieranie zakladki z podanym kontaktem/czatem
								OpenNewTab(JID);
								//Blokada wcisniecia klawiszy
								return -1;
							}
						}
					}
					//Pomijanie przypietych zakladek
					else
					{
						//Tworzenie listy bez przypietych zakladek
						TStringList *ExTabsList = new TStringList;
						for(int Count=0;Count<TabsListEx->Count;Count++)
						{
							//Pobieranie JID zakladki
							UnicodeString JID = TabsListEx->Strings[Count];
							//Usuwanie zasobu z JID
							if(JID.Pos("/"))
							{
								UnicodeString UserIdx = JID;
								while(UserIdx.Pos(":")) UserIdx.Delete(1,UserIdx.Pos(":"));
								JID.Delete(JID.Pos("/"),JID.Length());
								JID = JID + ":" + UserIdx;
							}
							//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
							TIniFile *Ini = new TIniFile(SessionFileDir);
							bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID);
							delete Ini;
							//Jezeli zakladka nie jest przypieta lub jest przypieta i ma widocza nazwe
							if((ClipTabsList->IndexOf(JID)==-1)||((ClipTabsList->IndexOf(JID)!=-1)&&(ClipTabsEx)))
								//Dodawanie zakladki do nowej listy
								ExTabsList->Add(TabsListEx->Strings[Count]);
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
								//Zmiana aktywnej zakladki na wskazany kontakt/czat
								ChangeActiveTab(JID);
								//Blokada wcisniecia klawiszy
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
								//Pobieranie identyfikatora kontaktu
								UnicodeString JID = UnOpenMsgList->Strings[0];
								//Otwieranie zakladki z podanym kontaktem/czatem
								OpenNewTab(JID);
								//Blokada wcisniecia klawiszy
								return -1;
							}
						}
						else delete ExTabsList;
					}
				}
			}
			//Wcisniete przyciski Shift + F1-F12 lub Ctr + Shift + 1-9
			else if(((ExClipTabsFromTabsHotKeysChk)&&(GetKeyState(VK_MENU)>=0)&&(GetKeyState(VK_SHIFT)<0))&&
			(((TabsHotKeysMode==1)&&((GetKeyState(VK_CONTROL)>=0)&&((int)wParam>=112)&&((int)wParam<=123)))
			||((TabsHotKeysMode==2)&&((GetKeyState(VK_CONTROL)<0)&&((int)wParam>=49)&&((int)wParam<=57)))))
			{
				//Sprawdzanie aktywnego okna
				if(GetForegroundWindow()==hFrmSend)
				{
					//Identyfikacja klawisza
					int Key;
					if(TabsHotKeysMode==1) Key = (int)wParam - 111;
					else Key = (int)wParam - 48;
					//Tworzenie listy z przypietymi zakladkami
					TStringList *ExTabsList = new TStringList;
					for(int Count=0;Count<TabsListEx->Count;Count++)
					{
						//Pobieranie JID zakladki
						UnicodeString JID = TabsListEx->Strings[Count];
						//Usuwanie zasobu z JID
						if(JID.Pos("/"))
						{
							UnicodeString UserIdx = JID;
							while(UserIdx.Pos(":")) UserIdx.Delete(1,UserIdx.Pos(":"));
							JID.Delete(JID.Pos("/"),JID.Length());
							JID = JID + ":" + UserIdx;
						}
						//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
						TIniFile *Ini = new TIniFile(SessionFileDir);
						bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID);
						delete Ini;
						//Jezeli zakladka jest przypieta i nie ma widoczej nazwy
						if((ClipTabsList->IndexOf(JID)!=-1)&&(!ClipTabsEx))
							//Dodawanie zakladki do nowej listy
							ExTabsList->Add(TabsListEx->Strings[Count]);
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
							//Zmiana aktywnej zakladki na wskazany kontakt/czat
							ChangeActiveTab(JID);
							//Blokada wcisniecia klawiszy
							return -1;
						}
					}
				}
			}
		}
		//Skrot do przywracania ostatnio zamknietej zakladki
		if((ClosedTabsChk)&&(UnCloseTabHotKeyChk)&&(ClosedTabsList->Count>0))
		{
			//Ctrl + Backspace
			if(UnCloseTabHotKeyMode==1)
			{
				//Wcisniecie Ctrl + Backspace
				if((GetKeyState(VK_CONTROL)<0)&&
				(GetKeyState(VK_MENU)>=0)&&
				(GetKeyState(VK_SHIFT)>=0)&&
				(wParam==VK_BACK))
				{
					//Sprawdzanie aktywnego okna
					HWND hActiveFrm = GetForegroundWindow();
					if((hActiveFrm==hFrmSend)||(hActiveFrm==hFrmMain))
					{
						GetClosedTabsItem(0);
						//Blokada wcisniecia klawiszy
						return -1;
					}
				}
			}
			//Skrot zdefiniowany przez uzytkownika
			else
			{
				//Sprawdzanie aktywnego okna
				HWND hActiveFrm = GetForegroundWindow();
				if((hActiveFrm==hFrmSend)||(hActiveFrm==hFrmMain))
				{
					//Odkodowanie zdefiniowanego przez uzytkownika skrotu
					int Mod = div(UnCloseTabHotKeyDef,256).quot;
					int Key = UnCloseTabHotKeyDef-Mod*256;
					//MOD_SHIFT | MOD_CONTROL
					if(div(Mod,32).quot==3)
					{
						if((GetKeyState(VK_CONTROL)<0)&&
						(GetKeyState(VK_MENU)>=0)&&
						(GetKeyState(VK_SHIFT)<0)&&
						((int)wParam==Key))
						{
							GetClosedTabsItem(0);
							//Blokada wcisniecia klawiszy
							return -1;
						}
					}
					//MOD_SHIFT | MOD_ALT
					else if(div(Mod,32).quot==5)
					{
						if((GetKeyState(VK_CONTROL)>=0)&&
						(GetKeyState(VK_MENU)<0)&&
						(GetKeyState(VK_SHIFT)<0)&&
						((int)wParam==Key))
						{
							GetClosedTabsItem(0);
							//Blokada wcisniecia klawiszy
							return -1;
						}
					}
					//MOD_CONTROL
					else if(div(Mod,32).quot==2)
					{
						if((GetKeyState(VK_CONTROL)<0)&&
						(GetKeyState(VK_MENU)>=0)&&
						(GetKeyState(VK_SHIFT)>=0)&&
						((int)wParam==Key))
						{
							GetClosedTabsItem(0);
							//Blokada wcisniecia klawiszy
							return -1;
						}
					}
					//MOD_ALT
					else if(div(Mod,32).quot==4)
					{
						if((GetKeyState(VK_CONTROL)>=0)&&
						(GetKeyState(VK_MENU)<0)&&
						(GetKeyState(VK_SHIFT)>=0)&&
						((int)wParam==Key))
						{
							GetClosedTabsItem(0);
							//Blokada wcisniecia klawiszy
							return -1;
						}
					}
					//MOD_ALT | MOD_CONTROL
					else if(div(Mod,32).quot==6)
					{
						if((GetKeyState(VK_CONTROL)<0)&&
						(GetKeyState(VK_MENU)<0)&&
						(GetKeyState(VK_SHIFT)>=0)&&
						((int)wParam==Key))
						{
							GetClosedTabsItem(0);
							//Blokada wcisniecia klawiszy
							return -1;
						}
					}
					//MOD_ALT | MOD_CONTROL | MOD_ALT
					else if(div(Mod,32).quot==7)
					{
						if((GetKeyState(VK_CONTROL)<0)&&
						(GetKeyState(VK_MENU)<0)&&
						(GetKeyState(VK_SHIFT)<0)&&
						((int)wParam==Key))
						{
							GetClosedTabsItem(0);
							//Blokada wcisniecia klawiszy
							return -1;
						}
					}
				}
			}
		}
		//Skroty do ulubionych zakladek
		if((FavouritesTabsChk)&&(FavouritesTabsHotKeysChk))
		{
			//Wscisniete przyciski Alt + 1-0
			if((((GetKeyState(VK_LMENU)<0)&&(GetKeyState(VK_CONTROL)>=0)&&(GetKeyState(VK_SHIFT)>=0))||((GetKeyState(VK_RMENU)<0)&&(GetKeyState(VK_CONTROL)<0)&&(GetKeyState(VK_SHIFT)>=0)))&&(((int)wParam>=48)&&((int)wParam<=57)))
			{
				//Sprawdzanie aktywnego okna
				if(GetForegroundWindow()==hFrmSend)
				{
					//Identyfikacja klawisza
					int Key = (int)wParam - 48;
					if(Key==0) Key = 10;
					//Sprawdzanie czy wywolujemy zakladke "ducha"
					if(Key<=FavouritesTabsList->Count)
					{
						//Otwieranie ulubionej zakladki
						GetFavouritesTabsItem(Key-1);
						//Blokada wcisniecia klawiszy
						return -1;
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
				//Sprawdzanie aktywnego okna
				if(GetForegroundWindow()==hFrmSend)
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
								Text.Delete(SelPos.cpMin+1,SelPos.cpMax-SelPos.cpMin);
							//Wklejanie cytatu do tekstu
							Text = Text.Insert(ClipboardText,SelPos.cpMin+1);
							//Nadpisywanie tekstu w RichEdit
							SetWindowTextW(hRichEdit, Text.w_str());
							//Ustawianie pozycji kursora
							SelPos.cpMin = SelPos.cpMin + ClipboardText.Length();
							SelPos.cpMax = SelPos.cpMin;
							SendMessage(hRichEdit, EM_EXSETSEL, NULL, (LPARAM)&SelPos);
							//Blokada wcisniecia klawiszy
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
	//Blad lub komunikator jest zamykany
	if((nCode<0)||(ForceUnloadExecuted)) return CallNextHookEx(hThreadMouse, nCode, wParam, lParam);
	//wParam i lParam zawieraja informacje o stanie myszki
	//Uchwyt do paska zakladek zostal pobrany, nie jest aktywne popupmenu, hook nie jest zablokowany
	else if((nCode==HC_ACTION)&&(hTabsBar)&&(!IsWindow(hPopupMenu))&&(!BlockThreadMouseProc))
	{
		//Przywracanie zakladek za pomoca myszki
		if((ClosedTabsChk)&&((UnCloseTabSPMouseChk)||(UnCloseTabLPMouseChk)||(UnCloseTab2xLPMouseChk)))
		{
			//Niewywolano zamkniecia poprzez 2xLPM
			if(!LBUTTONDBLCLK_EXECUTED)
			{
				//Przywracanie zakladki za pomoca SPM
				if(UnCloseTabSPMouseChk)
				{
					//Wcisniecie SPM
					if(wParam==WM_MBUTTONDOWN)
					{
						//Kusor znajduje sie w obrebie paska zakladek
						if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
							TabWasChanged = false;
					}
					//Odcisniecie SPM
					else if(wParam==WM_MBUTTONUP)
					{
						//Kusor znajduje sie w obrebie paska zakladek
						if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
						{
							if(!TabWasChanged) GetClosedTabsItem(0);
						}
					}
				}
				//Przywracanie zakladki za pomoca Ctrl+LPM
				if(UnCloseTabLPMouseChk)
				{
					//Wcisniecie LPM
					if((wParam==WM_LBUTTONDOWN)&&((GetKeyState(VK_LCONTROL)<0)||(GetKeyState(VK_RCONTROL)<0)))
					{
						//Kusor znajduje sie w obrebie paska zakladek
						if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
							TabWasChanged = false;
					}
					//Odcisniecie LPM
					if((wParam==WM_LBUTTONUP)&&((GetKeyState(VK_LCONTROL)<0)||(GetKeyState(VK_RCONTROL)<0)))
					{
						//Kusor znajduje sie w obrebie paska zakladek
						if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
						{
							if(!TabWasChanged) GetClosedTabsItem(0);
						}
					}
				}
				//Przywracanie zakladki za pomoca 2xLPM
				if(UnCloseTab2xLPMouseChk)
				{
					//Wcisniecie 2xLPM
					if(wParam==WM_LBUTTONDBLCLK)
					{
						//Kusor znajduje sie w obrebie paska zakladek
						if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
						{
							//Zmienna pomocnicza przywracania zakladki za pomoca myszki
							TabWasChanged = false;
							//Wlaczenie timera przywracania zakladki poprzez 2xLPM
							SetTimer(hTimerFrm,TIMER_UNCLOSEBY2XLPM,100,(TIMERPROC)TimerFrmProc);
						}
					}
				}
			}
		}
		//Zamkniecie zakladki poprzez 2xLPM
		if(CloseBy2xLPMChk)
		{
			//Wcisniecie 2xLPM
			if(wParam==WM_LBUTTONDBLCLK)
			{
				//Kusor znajduje sie w obrebie paska zakladek
				if(WindowFromPoint(Mouse->CursorPos)==hTabsBar)
				{
					//Odznaczenie wcisniecia 2xLPM
					LBUTTONDBLCLK_EXECUTED = true;
					//Wcisniecie SPM
					mouse_event(MOUSEEVENTF_MIDDLEDOWN,0,0,0,0);
					//Wlaczenie timera zamykania zakladki poprzez 2xLPM
					SetTimer(hTimerFrm,TIMER_CLOSEBY2XLPM,100,(TIMERPROC)TimerFrmProc);
				}
			}
		}
		//Zmiana miejsca zakladek
		if((wParam==WM_LBUTTONDOWN)&&(WindowFromPoint(Mouse->CursorPos)==hTabsBar))
		{
			//Odznaczenie wcisniecia LPM
			LBUTTONDOWN_ON_TABSBAR_EXECUTED = true;
			//Zapamietanie pozycji myszki
			CursorPosX = Mouse->CursorPos.X;
			CursorPosY = Mouse->CursorPos.Y;
		}
		if((wParam==WM_LBUTTONUP)&&(LBUTTONDOWN_ON_TABSBAR_EXECUTED))
		{
			//Pozycja myszki zostala zmieniona
			if((Mouse->CursorPos.X!=CursorPosX)||(Mouse->CursorPos.Y!=CursorPosY))
			{
				//Przebudowa kolejnosci zakladek w pliku sesji
				SetTimer(hTimerFrm,TIMER_REBUILD_TABS_LIST,100,(TIMERPROC)TimerFrmProc);
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
	//Ponowne zaladowanie hooka
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
	//Chowanie okna kontaktow
	//Wyladowanie hooka
	UnregisterHotKey(hSettingsForm->Handle, 0x0200);
	//Ponowne zaladowanie hooka
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
		//Skrot aktywny w AQQ
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
			//Wyladowanie hooka z AQQ
			UnregisterHotKey(hFrmMain, 1);
		}
		//Zaladowanie hooka
		RegisterHotKey(hSettingsForm->Handle, 0x0200, MOD_SHIFT | MOD_CONTROL, 112);
	}
}
//---------------------------------------------------------------------------

//Hook na aktwyna zakladke lub okno rozmowy
INT_PTR __stdcall OnActiveTab(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Przywracanie zakladki za pomoca myszki
		TabWasChanged = true;
		//Wylaczenie blokady otwierania paru zakladek jednoczesnie
		NewMgsHoyKeyExecute = false;
		//Blokada zmiany tekstu na zakladce
		TabWasClosed = false;
		//Pobieranie danych kontaktku
		TPluginContact ActiveTabContact = *(PPluginContact)lParam;
		UnicodeString JID = (wchar_t*)ActiveTabContact.JID;
		UnicodeString Res = (wchar_t*)ActiveTabContact.Resource;
		if(!Res.IsEmpty()) Res = "/" + Res;
		if(ActiveTabContact.IsChat)
		{
			JID = "ischat_" + JID;
			Res = "";
		}
		UnicodeString UserIdx = ":" + IntToStr(ActiveTabContact.UserIdx);
		//Zakladka zostala zmieniona
		if((JID+Res+UserIdx)!=ActiveTabJIDEx)
		{
			//Zapamietanie aktywnej zakladki
			ActiveTabJID = JID+UserIdx;
			ActiveTabJIDEx = JID+Res+UserIdx;
			//Wysuniecie okna rozmowy zza krawedzi ekranu
			if((FrmSendSlideChk)&&(!FrmSendVisible)&&(!FrmSendBlockSlide)&&(!FrmSendSlideIn)&&(!FrmSendSlideOut))
			{
				//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
				ChkFullScreenMode();
				//Blokowanie wysuwania przy aplikacji pelnoekranowej
				if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
				{ /* Blokada */ }
				else
				{
					//Status wysuwania okna rozmowy zza krawedzi ekranu
					FrmSendSlideIn = true;
					//Zablokowanie mozliwosci odblokowania tymczasowej blokady
					FrmSendUnBlockSlideEx = false;
					//Odswiezenie okna rozmowy
					RefreshFrmSend();
					//Aktywacja okna rozmowy + ustawienie okna na wierzchu
					ActivateAndSetTopmostFrmSend();
					//Wlaczenie pokazywania okna rozmowy (part II)
					SetTimer(hTimerFrm,TIMER_FRMSEND_SLIDEIN,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
					//Schowanie okna kontaktow (gdy okno ustraci fokus)
					if((FrmMainSlideChk)&&(FrmMainSlideHideMode==1))
					{
						//Okno kontatkow jest widoczne, aktualnie nie jest chowane/wysuwane, nie jest aktywna blokada
						if((FrmMainVisible)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
						{
							//Status chowania okna kontaktow za krawedz ekranu
							FrmMainSlideOut = true;
							//Schowanie okna kontaktow przy aktywacji okna rozmowy
							FrmMainSlideOutActiveFrmSend = true;
							//Wlaczenie chowania okna kontaktow (part I)
							SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
						}
					}
				}
			}
			//Usuwanie JID z kolejki pokazywania wiadomosci przy skrocie Ctrl+Shift+F1 lub nieprzypisanym skrotem do zakladki
			if((FrmMainSlideChk)||(NewMgsHoyKeyChk))
			{
				if(UnOpenMsgList->IndexOf(JID+Res+UserIdx)!=-1)
					UnOpenMsgList->Delete(UnOpenMsgList->IndexOf(JID+Res+UserIdx));
				if(UnOpenMsgList->IndexOf(JID+UserIdx)!=-1)
					UnOpenMsgList->Delete(UnOpenMsgList->IndexOf(JID+UserIdx));
			}
			//Usuwanie JID z kolejki przelaczania sie na nowe wiadomosci
			if(SwitchToNewMsgChk)
			{
				if(MsgList->IndexOf(JID+Res+UserIdx)!=-1)
					MsgList->Delete(MsgList->IndexOf(JID+Res+UserIdx));
				if(MsgList->IndexOf(JID+UserIdx)!=-1)
					MsgList->Delete(MsgList->IndexOf(JID+UserIdx));
			}
			//Miganie diodami LED klawiatury - wylaczanie mrugania
			if((KeyboardFlasherChk)&&(hFlasherThread)&&(hFlasherKeyboardThread))
			{
				//Usuwanie z listy nieprzeczytanych wiadomosci aktywnej zakladki
				if(UnreadMsgList->IndexOf(ActiveTabJIDEx)!=-1)
					UnreadMsgList->Delete(UnreadMsgList->IndexOf(ActiveTabJIDEx));
				if(UnreadMsgList->IndexOf(ActiveTabJID)!=-1)
					UnreadMsgList->Delete(UnreadMsgList->IndexOf(ActiveTabJID));
				//Nie ma juz nieprzeczytanych wiadomosci
				if(!UnreadMsgList->Count)
				{
					SetEvent(hFlasherThread);
					WaitForSingleObject(hFlasherKeyboardThread, 30000);
					CloseHandle(hFlasherThread);
					hFlasherThread = NULL;
					hFlasherKeyboardThread = NULL;
				}
			}
			//Zakladka z kontaktem nie byla otwarta
			if((TabsListEx->IndexOf(JID+Res+UserIdx)==-1))
			{
				//Dodawanie JID do tablicy zakladek
				if(TabsList->IndexOf(JID+UserIdx)==-1) TabsList->Add(JID+UserIdx);
				TabsListEx->Add(JID+Res+UserIdx);
				//Pobieranie i zapisywanie stanu kontaktu
				if(!ActiveTabContact.IsChat)
				{
					int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&ActiveTabContact);
					ContactsStateList->WriteInteger("State",JID+UserIdx,State);
				}
				//Przypiete zakladki
				if((ClipTabsList->IndexOf(JID+UserIdx)!=-1)&&(!RestoringSession))
				{
					//Wlaczanie timera do zmiany miejsca zakladki
					if(!ActiveTabContact.IsChat) SetTimer(hTimerFrm,TIMER_CLIPTABS_MOVE,500,(TIMERPROC)TimerFrmProc);
					else SetTimer(hTimerFrm,TIMER_CLIPTABS_MOVE,3000,(TIMERPROC)TimerFrmProc);
				}
				//Niewyslane wiadomosci
				if((UnsentMsgChk)&&(!RestoringSession))
				{
					//Odczyt pliku sesji
					TIniFile *Ini = new TIniFile(SessionFileDir);
					UnicodeString Body = DecodeBase64(Ini->ReadString("Messages", JID+UserIdx, ""));
					//Wczytanie tresci wiadomosci do pola RichEdit
					if(!Body.IsEmpty())
					{
						//Wczytanie ostatniej wiadomosci do okna rozmowy
						if(!ActiveTabContact.IsChat)
						{
							//Przekazanie zmiennych kontaktu
							LoadLastConvJID = JID;
							LoadLastConvUserIdx = ActiveTabContact.UserIdx;
							//Wlaczenie timera
							SetTimer(hTimerFrm,TIMER_LOADLASTCONV,500,(TIMERPROC)TimerFrmProc);
						}
						//Ustawianie tekstu
						SetWindowTextW(hRichEdit, Body.w_str());
						//Ustawianie pozycji kursora
						CHARRANGE SelPos;
						SelPos.cpMin = Body.Length();
						SelPos.cpMax = SelPos.cpMin;
						SendMessage(hRichEdit, EM_EXSETSEL, NULL, (LPARAM)&SelPos);
						//Szybki dostep niewyslanych wiadomosci
						DestroyFrmUnsentMsg();
						//Usuniecie wiadomosci z pliku sesji
						Ini->DeleteKey("Messages", JID+UserIdx);
						//Szybki dostep niewyslanych wiadomosci
						DestroyFrmUnsentMsg();
						BuildFrmUnsentMsg(true);
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
					if(ClosedTabsList->IndexOf(JID+UserIdx)!=-1)
					{
						//Odczytywanie JID ostatnio przywroconej zakladki
						if((RestoreLastMsgChk)&&(JustUnClosedJID==JID+UserIdx))
						{
							//Wczytanie ostatniej wiadomosci do okna rozmowy
							if(!ActiveTabContact.IsChat)
							{
								//Przekazanie zmiennych kontaktu
								LoadLastConvJID = JID;
								LoadLastConvUserIdx = ActiveTabContact.UserIdx;
								//Wlaczenie timera
								SetTimer(hTimerFrm,TIMER_LOADLASTCONV,500,(TIMERPROC)TimerFrmProc);
							}
							//Skasowanie JID ostatnio przywroconej zakladki
							JustUnClosedJID = "";
						}
						//Usuwanie interfejsu
						DestroyFrmClosedTabs();
						//Usuwanie JID z tablicy
						ClosedTabsTimeList->Delete(ClosedTabsList->IndexOf(JID+UserIdx));
						ClosedTabsList->Delete(ClosedTabsList->IndexOf(JID+UserIdx));
						//Maks pamietanych X elementow
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
						BuildFrmClosedTabs(true);
					}
				}
				//Pobieranie aktualnej nazwy kanalu zakladki czatowej
				if((ActiveTabContact.IsChat)&&(!ActiveTabContact.FromPlugin))
				{
					//Pobranie identyfikatora czatu
					UnicodeString ChatJID = (wchar_t*)ActiveTabContact.JID;
					//Pobieranie indeksu konta
					int AccountUserIdx = ActiveTabContact.UserIdx;
					//Pobieranie nazwy konta
					UnicodeString Account = ReceiveAccountName(AccountUserIdx);
					//Generowanie unikatowego ID
					UnicodeString XMLID = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0);
					XMLIDList->Add(XMLID);
					//Generowane pakietu XML
					UnicodeString XML = "<iq from=\""+Account+"\" to=\""+ChatJID+"\" id=\""+XMLID+"\" type=\"get\" xml:lang=\"pl\"><query xmlns=\"http://jabber.org/protocol/disco#info\"/></iq>";
					//Wyslanie pakietu XML na wskazane konto
					PluginLink.CallService(AQQ_SYSTEM_SENDXML,(WPARAM)XML.w_str(),AccountUserIdx);
				}
			}
			//Zakladka z kontaktem jest juz otwarta
			else
			{
				//Usuwanie licznika nowych wiadomosci na zakladkach
				if(InactiveTabsNewMsgChk)
				{
					//Pobranie ilosci nieprzeczytanych wiadomosci
					int Count = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID+Res+UserIdx,0);
					//Tylko dla zakladki z licznikiem nieprzeczytanych wiadomosci
					if(Count)
					{
						//Wyladowanie hooka na zmiane tekstu na zakladce
						PluginLink.UnhookEvent(OnTabCaption);
						//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
						TIniFile *Ini = new TIniFile(SessionFileDir);
						bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID+UserIdx);
						delete Ini;
						//Zakladka nie jest przypieta lub jest przypieta z widoczna nazwa
						if((ClipTabsList->IndexOf(JID+UserIdx)==-1)||((ClipTabsList->IndexOf(JID+UserIdx)!=-1)&&(ClipTabsEx)))
						{
							//Zakladka ze zwyklym kontatem
							if(!ActiveTabContact.IsChat)
								PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)ActiveTabContact.Nick,(LPARAM)&ActiveTabContact);
							//Zakladka z czatem
							else
								PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)GetChannelName(JID).w_str(),(LPARAM)&ActiveTabContact);
						}
						//Zakladka przypieta
						else PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)L"",(LPARAM)&ActiveTabContact);
						//Hook na zmiane tekstu na zakladce
						PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
					}
					//Resetowanie stanu nowych wiadomosci
					InactiveTabsNewMsgCount->DeleteKey("TabsMsg",JID+Res+UserIdx);
				}
				//Licznik nowych wiadomosci na oknie rozmowy
				if(InactiveFrmNewMsgChk)
				{
					//Jezeli okno rozmowy jest aktywne
					if(hFrmSend==GetForegroundWindow())
					{
						//Kasowanie licznika nowych wiadomosci
						InactiveFrmNewMsgCount = 0;
						//Wyczyszczenie tymczasowego tytulu okna
						FrmSendTitlebar = "";
					}
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
					if((TabsList->Strings[Count].Pos("@plugin"))&&(TabsList->Strings[Count].Pos("ischat_")))
					{ /* Blokada czatu pochodzacego z wtyczki */ }
					else
						Ini->WriteString("Session","Tab"+IntToStr(Count+1),TabsList->Strings[Count]);
				}
				//Zapisywanie aktywnej zakladki
				if(!ActiveTabContact.IsChat)
					Ini->WriteString("SessionEx","ActiveTab",JID+UserIdx);
				else
					Ini->DeleteKey("SessionEx","ActiveTab");
				//Odczytywanie sesji wiadomosci
				if((RestoreMsgSessionChk)&&(RestoringSession))
				{
					UnicodeString Body = DecodeBase64(Ini->ReadString("SessionMsg", JID+UserIdx, ""));
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
			//Zmiana caption okna rozmowy
			if(TweakFrmSendTitlebarChk)
			{
				if(!ActiveTabContact.IsChat)
				{
					//Pobranie aktualnego tekstu belki okna
					wchar_t TitlebarW[512];
					GetWindowTextW(hFrmSend, TitlebarW, sizeof(TitlebarW));
					UnicodeString Titlebar = TitlebarW;
					Titlebar = StringReplace(Titlebar, "\r\n", "", TReplaceFlags() << rfReplaceAll);
					//Zmienna zmienionego tekstu na belce
					UnicodeString ChangedTitlebar;
					//Pobranie pseudonimu kontaktu
					UnicodeString Nick = (wchar_t*)ActiveTabContact.Nick;
					//Pseudonim i opis kontaktu
					if(TweakFrmSendTitlebarMode==1)
					{
						//Pobranie opisu kontaktu
						UnicodeString Status = (wchar_t*)ActiveTabContact.Status;
						Status = StringReplace(Status, "\r\n", " ", TReplaceFlags() << rfReplaceAll);
						//Jezeli opis nie jest pusty
						if(!Status.IsEmpty())
							ChangedTitlebar = Nick + " - " + Status;
						else
							ChangedTitlebar = Nick;
					}
					//Sam pseudonim kontaktu
					else if(TweakFrmSendTitlebarMode==2)
						ChangedTitlebar = Nick;
					//Pseudonim i identyfikator kontaktu
					else if(TweakFrmSendTitlebarMode==3)
					{
						//Przyjazniejszy wyglad identyfikatora
						UnicodeString FriendlyJID = JID;
						if(ActiveTabContact.FromPlugin)
						{
							if(FriendlyJID.Pos("@")) FriendlyJID.Delete(FriendlyJID.Pos("@"),FriendlyJID.Length());
						}
						ChangedTitlebar = Nick + " - " + FriendlyJID;
					}
					//Pseudonim i identyfikator kontaktu wraz z zasobem oraz opisem
					else if((TweakFrmSendTitlebarMode==4)&&(!ActiveTabContact.FromPlugin))
					{
						//Jezeli zasob nie jest pusty
						if(!Res.IsEmpty())
						{
							//Pobranie opisu kontaktu
							UnicodeString Status = (wchar_t*)ActiveTabContact.Status;
							Status = StringReplace(Status, "\r\n", " ", TReplaceFlags() << rfReplaceAll);
							//Przyjazniejszy wyglad identyfikatora
							UnicodeString FriendlyJID = JID;
							if(ActiveTabContact.FromPlugin)
							{
								if(FriendlyJID.Pos("@")) FriendlyJID.Delete(FriendlyJID.Pos("@"),FriendlyJID.Length());
							}
							//Jezeli opis nie jest pusty
							if(!Status.IsEmpty())
								ChangedTitlebar = Nick + " - " + FriendlyJID + Res + " - " + Status;
							else
								ChangedTitlebar = Nick + " - " + FriendlyJID + Res;
						}
					}
					//Nowy tekst na belce okna nie jest pusty
					if(!ChangedTitlebar.IsEmpty())
					{
					//Zmiana tekstu na belce
					SetWindowTextW(hFrmSend,ChangedTitlebar.w_str());
					//Zapisywanie zmienionego tekstu belki do cache
					ChangedTitlebarList->WriteString("Titlebar",MD5(Titlebar),EncodeBase64(Titlebar));
					}
				}
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na pokazywane wiadomosci
INT_PTR __stdcall OnAddLine(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Okno archiwum jest nieaktywne
		if(GetForegroundWindow()!=hFrmArch)
		{
			//Transfer plikow - pseudo nowa wiadomosc
			if((SwitchToNewMsgChk)||(InactiveFrmNewMsgChk)||(KeyboardFlasherChk)||(InactiveTabsNewMsgChk)||(ClosedTabsChk)||((FrmSendSlideChk)&&(SlideInAtNewMsgChk)&&(!FrmSendVisible)&&(!FrmSendBlockSlide)&&(!FrmSendSlideIn)&&(!FrmSendSlideOut)))
			{
				//Pobieranie danych wiadomosci
				TPluginMessage AddLineMessage = *(PPluginMessage)lParam;
				//Pobieranie sformatowanej tresci wiadomosci
				UnicodeString Body = (wchar_t*)AddLineMessage.Body;
				wstring input = Body.w_str();
				//Wyrazenie regularne na sciezki plikow
				wregex exrp(L"^[a-zA-Z]:(.*?)[.]+[a-zA-Z0-9]+$");
				//Wyswietlona wiadomosc jest sciezka przeslanego pliku
				if(regex_match(input, exrp))
				{
					if(FileExists(Body))
					{
						//Przelaczanie na zakladke z nowa wiadomoscia
						if(SwitchToNewMsgChk)
						{
							//Pobieranie danych kontaktu
							TPluginContact AddLineContact = *(PPluginContact)wParam;
							UnicodeString JID = (wchar_t*)AddLineContact.JID;
							UnicodeString Res = (wchar_t*)AddLineContact.Resource;
							if(!Res.IsEmpty()) Res = "/" + Res;
							if(AddLineContact.IsChat)
							{
								JID = "ischat_" + JID;
								Res = "";
							}
							UnicodeString UserIdx = ":" + IntToStr(AddLineContact.UserIdx);
							//JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
							if((JID+Res+UserIdx!=ActiveTabJIDEx)&&(TabsListEx->IndexOf(JID+Res+UserIdx)!=-1))
							{
								//Dodawanie JID do kolejki nowych wiadomosci
								if(MsgList->IndexOf(JID+Res+UserIdx)==-1)
									MsgList->Add(JID+Res+UserIdx);
							}
						}
						//Licznik nieprzeczytanych wiadomosci na pasku tytulu okna rozmowy
						if(InactiveFrmNewMsgChk)
						{
							//Pobieranie danych kontaktu
							TPluginContact AddLineContact = *(PPluginContact)wParam;
							UnicodeString JID = (wchar_t*)AddLineContact.JID;
							UnicodeString Res = (wchar_t*)AddLineContact.Resource;
							if(!Res.IsEmpty()) Res = "/" + Res;
							if(AddLineContact.IsChat)
							{
								JID = "ischat_" + JID;
								Res = "";
							}
							UnicodeString UserIdx = ":" + IntToStr(AddLineContact.UserIdx);
							//Zakladka jest otwarta
							if(TabsListEx->IndexOf(JID+Res+UserIdx)!=-1)
							{
								//Okno rozmowy jest nieaktywne
								if(hFrmSend!=GetForegroundWindow())
								{
									//Pobranie oryginalnego tekstu paska tytulu okna rozmowy
									if((FrmSendTitlebar.IsEmpty())&&(!InactiveFrmNewMsgCount))
									{
										wchar_t TitlebarW[512];
										GetWindowTextW(hFrmSend, TitlebarW, sizeof(TitlebarW));
										FrmSendTitlebar = TitlebarW;
									}
									//Dodanie 1 do licznika nieprzeczytanych wiadomosci
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
						//Miganie diodami LED klawiatury - wlaczenie mrugania
						if(KeyboardFlasherChk)
						{
							//Pobieranie danych kontaktu
							TPluginContact AddLineContact = *(PPluginContact)wParam;
							UnicodeString JID = (wchar_t*)AddLineContact.JID;
							UnicodeString Res = (wchar_t*)AddLineContact.Resource;
							if(!Res.IsEmpty()) Res = "/" + Res;
							if(AddLineContact.IsChat)
							{
								JID = "ischat_" + JID;
								Res = "";
							}
							UnicodeString UserIdx = ":" + IntToStr(AddLineContact.UserIdx);
							//Okno rozmowy jest nieaktywne lub wiadomoc nie pochodzi z aktywnej zakladki
							if((GetForegroundWindow()!=hFrmSend)||((JID+Res+UserIdx)!=ActiveTabJIDEx))
							{
								//Dodawanie JID do listy nieprzeczytanych wiadomosci
								if(UnreadMsgList->IndexOf(JID+Res+UserIdx)==-1)
									UnreadMsgList->Add(JID+Res+UserIdx);
								//Wlaczenie mrugania
								if((!hFlasherThread)&&(!hFlasherKeyboardThread))
								{
									hFlasherThread = CreateEvent(NULL, FALSE, FALSE, FLASHER);
									if(KeyboardFlasherModeChk==0) hFlasherKeyboardThread = FlashKeyboardLightInThread(KEYBOARD_SCROLL_LOCK_ON, 250, FLASHER);
									else if(KeyboardFlasherModeChk==1) hFlasherKeyboardThread = FlashKeyboardLightInThread(KEYBOARD_NUM_LOCK_ON, 250, FLASHER);
									else hFlasherKeyboardThread = FlashKeyboardLightInThread(KEYBOARD_CAPS_LOCK_ON, 250, FLASHER);
								}
							}
						}
						//Licznik nieprzeczytanych wiadomosci na zakladkach
						if(InactiveTabsNewMsgChk)
						{
							//Pobieranie danych kontaktu
							TPluginContact AddLineContact = *(PPluginContact)wParam;
							UnicodeString JID = (wchar_t*)AddLineContact.JID;
							UnicodeString Res = (wchar_t*)AddLineContact.Resource;
							if(!Res.IsEmpty()) Res = "/" + Res;
							if(AddLineContact.IsChat)
							{
								JID = "ischat_" + JID;
								Res = "";
							}
							UnicodeString UserIdx = ":" + IntToStr(AddLineContact.UserIdx);
							//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
							TIniFile *Ini = new TIniFile(SessionFileDir);
							bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID+UserIdx);
							delete Ini;
							//Jezeli licznik nie ma byc dodawany na przypietej zakladce
							if((ClipTabsChk)&&(InactiveClipTabsChk)&&(ClipTabsList->IndexOf(JID+UserIdx)!=-1)&&(!ClipTabsEx))
								goto SkipInactiveTabsNewMsgChk;
							//Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
							if((JID+Res+UserIdx!=ActiveTabJIDEx)&&(TabsListEx->IndexOf(JID+Res+UserIdx)!=-1))
							{
								//Pobieranie stanu nowych wiadomosci
								int InactiveTabsCount = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID+Res+UserIdx,0);
								//Zmiana stanu nowych wiadomosci
								InactiveTabsCount++;
								//Zapisywanie stanu nowych wiadomosci
								InactiveTabsNewMsgCount->WriteInteger("TabsMsg",JID+Res+UserIdx,InactiveTabsCount);
								//Ustawianie tekstu na zakladce
								UnicodeString TabCaption;
								//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
								TIniFile *Ini = new TIniFile(SessionFileDir);
								bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID+UserIdx);
								delete Ini;
								//Zakladka jest przypieta z niewidoczna nazwa
								if((ClipTabsList->IndexOf(JID+UserIdx)!=-1)&&(!ClipTabsEx)) TabCaption = "";
								//Zakladka nie jest przypieta lub jest przypieta z widoczna nazwa
								else
								{
									//Zakladka ze zwyklyum
									if(!AddLineContact.IsChat) TabCaption = (wchar_t*)AddLineContact.Nick;
									//Zakladka z czatem
									else TabCaption = GetChannelName(JID);
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
							UnicodeString UserIdx = ":" + IntToStr(AddLineContact.UserIdx);
							if(AcceptClosedTabsList->IndexOf(JID+UserIdx)==-1)
							{
								AcceptClosedTabsList->Add(JID+UserIdx);
							}
						}
						//Wysuwanie okna rozmowy zza krawedzi ekranu przy przyjsciu nowej wiadomosci
						if((FrmSendSlideChk)&&(SlideInAtNewMsgChk)&&(!FrmSendVisible)&&(!FrmSendBlockSlide)&&(!FrmSendSlideIn)&&(!FrmSendSlideOut))
						{
							//Pobieranie danych kontaktu
							TPluginContact AddLineContact = *(PPluginContact)wParam;
							UnicodeString JID = (wchar_t*)AddLineContact.JID;
							UnicodeString Res = (wchar_t*)AddLineContact.Resource;
							if(!Res.IsEmpty()) Res = "/" + Res;
							if(AddLineContact.IsChat) Res = "";
							UnicodeString UserIdx = ":" + IntToStr(AddLineContact.UserIdx);
							//Zakladka jest otwarta
							if(TabsListEx->IndexOf(JID+Res+UserIdx)!=-1)
							{
								//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
								ChkFullScreenMode();
								//Blokowanie wysuwania przy aplikacji pelnoekranowej
								if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
								{ /* Blokada */ }
								else
								{
									//Status wysuwania okna rozmowy zza krawedzi ekranu
									FrmSendSlideIn = true;
									//Odswiezenie okna rozmowy
									RefreshFrmSend();
									//Aktywacja okna rozmowy + ustawienie okna na wierzchu
									ActivateAndSetTopmostFrmSend();
									//Wlaczenie pokazywania okna rozmowy (part II)
									SetTimer(hTimerFrm,TIMER_FRMSEND_SLIDEIN,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
									//Schowanie okna kontaktow (gdy okno ustraci fokus)
									if((FrmMainSlideChk)&&(FrmMainSlideHideMode==1))
									{
										//Okno kontatkow jest widoczne, aktualnie nie jest chowane/wysuwane, nie jest aktywna blokada
										if((FrmMainVisible)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
										{
											//Status chowania okna kontaktow za krawedz ekranu
											FrmMainSlideOut = true;
											//Schowanie okna kontaktow przy aktywacji okna rozmowy
											FrmMainSlideOutActiveFrmSend = true;
											//Wlaczenie chowania okna kontaktow (part I)
											SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
										}
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
			//Pobieranie danych wiadomosci
			TPluginMessage AddLineMessage = *(PPluginMessage)lParam;
			//Pobieranie sformatowanej tresci wiadomosci
			UnicodeString Body = (wchar_t*)AddLineMessage.Body;
			//Wiadomosc zawiera przeslany obrazek
			if(Body.Pos("<IMG CLASS=\"aqqcacheitem\""))
			{
				//Zwijanie tylko dla wyslanych obrazkow
				if(CollapseImagesMode==2)
				{
					//Pobieranie JID nadawcy wiadomosci
					UnicodeString MessageJID = (wchar_t*)AddLineMessage.JID;
					if(MessageJID.Pos("/")>0) MessageJID.Delete(MessageJID.Pos("/"),MessageJID.Length());
					//Pobieranie danych kontaktu
					TPluginContact AddLineContact = *(PPluginContact)wParam;
					UnicodeString ContactJID = (wchar_t*)AddLineContact.JID;
					//Wiadomosc przychodzaca
					if(MessageJID==ContactJID) return 0;
				}
				//Zwijanie tylko dla odebranych obrazkow
				else if(CollapseImagesMode==3)
				{
					//Pobieranie JID nadawcy wiadomosci
					UnicodeString MessageJID = (wchar_t*)AddLineMessage.JID;
					if(MessageJID.Pos("/")>0) MessageJID.Delete(MessageJID.Pos("/"),MessageJID.Length());
					//Pobieranie danych kontaktu
					TPluginContact AddLineContact = *(PPluginContact)wParam;
					UnicodeString ContactJID = (wchar_t*)AddLineContact.JID;
					//Wiadomosc wychodzaca
					if(MessageJID!=ContactJID) return 0;
				}
				//Zmienna ze zawinietymi obrazkami do formy zalacznika
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
					BodyTmp.Delete(1,BodyTmp.Pos("<A HREF")-1);
					BodyTmp.Delete(BodyTmp.Pos("</A>")+4,BodyTmp.Length());
					Body = StringReplace(Body, BodyTmp, "[AQQCACHEITEM"+IntToStr(ItemsCount)+"]", TReplaceFlags());
					//Stylu formy zalacznika
					UnicodeString BodyStyle = AttachmentStyle;
					//Generowanie ID sesji
					UnicodeString Session = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0);
					//Pobranie adresu URL obrazka
					UnicodeString PhotoFileURL = BodyTmp;
					PhotoFileURL.Delete(1,PhotoFileURL.Pos("HREF=\"")+5);
					PhotoFileURL.Delete(PhotoFileURL.Pos("\">"),PhotoFileURL.Length());
					if(CollapseImagesList->IndexOf(PhotoFileURL)==-1)
						CollapseImagesList->Add(PhotoFileURL);
					//Pobranie nazwy obrazka
					UnicodeString PhotoFileName = BodyTmp;
					PhotoFileName.Delete(1,PhotoFileName.Pos("TITLE=\"")+6);
					PhotoFileName.Delete(PhotoFileName.Pos("\""),PhotoFileName.Length());
					//Pobieranie sciezki URL do grafiki zalacznika
					UnicodeString ThemePNGPath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,40,0);
					//Modyfikacja oryginalnego wygladu obrazka
					BodyTmp = StringReplace(BodyTmp, "\\", "/", TReplaceFlags() << rfReplaceAll);
					BodyTmp = StringReplace(BodyTmp, "A HREF=\"", "A HREF=\"image:" + Session + ":file:///", TReplaceFlags());
					BodyTmp.Delete(BodyTmp.Pos("<IMG"),BodyTmp.Length());
					BodyTmp = BodyTmp + PhotoFileName + "</A>";
					//Generowanie nowego wygladu obrazka juz w formie zalacznika
					BodyStyle = StringReplace(BodyStyle, "CC_ATTACH_ICON", "<IMG src=\"" + ThemePNGPath + "\" border=\"0\">", TReplaceFlags());
					BodyStyle = StringReplace(BodyStyle, "CC_ATTACH_CAPTION", "<SPAN id=\"id_cctext\">"+GetLangStr("Img")+"</SPAN>", TReplaceFlags());
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
		//Skracanie wyswietlania odnosnikow w oknie rozmowy do wygodniejszej formy
		if((ShortenLinksChk)&&((ShortenLinksMode==1)||(ShortenLinksMode==2)))
		{
			//Pobieranie danych wiadomosci
			TPluginMessage AddLineMessage = *(PPluginMessage)lParam;
			//Pobieranie sformatowanej tresci wiadomosci
			UnicodeString Body = (wchar_t*)AddLineMessage.Body;
			//Zapisywanie oryginalnej tresci wiadomosci
			UnicodeString BodyOrg = Body;
			//Skracanie wyswietlania odnosnikow
			Body = TrimLinks(Body,false);
			//Zmienianie tresci wiadomosci
			if(Body!=BodyOrg)
			{
				AddLineMessage.Body = Body.w_str();
				memcpy((PPluginMessage)lParam,&AddLineMessage, sizeof(TPluginMessage));
				return 2;
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zabezpieczenia komunikatora
INT_PTR __stdcall OnAutoSecureOn(WPARAM wParam, LPARAM lParam)
{
	//Informacja o zabezpieczeniu komunikatora
	SecureMode = true;
	//Zamkniecie okna ustawien
	if(hSettingsForm) hSettingsForm->Close();

	return 0;
}
//---------------------------------------------------------------------------

//Hook na odbezpieczenia komunikatora
INT_PTR __stdcall OnAutoSecureOff(WPARAM wParam, LPARAM lParam)
{
	//Informacja o odbezpieczeniu komunikatora
	SecureMode = false;

	return 0;
}
//---------------------------------------------------------------------------

//Hook na wylaczenie komunikatora poprzez usera
INT_PTR __stdcall OnBeforeUnload(WPARAM wParam, LPARAM lParam)
{
	//Info o rozpoczeciu procedury zamykania komunikatora
	ForceUnloadExecuted = true;

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie okna rozmowy lub zakladki
INT_PTR __stdcall OnCloseTab(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Zamkniecie zakladki poprzez 2xLPM
		if(LBUTTONDBLCLK_EXECUTED)
		{
			//Zatrzymanie timera zamykania zakladki poprzez 2xLPM
			KillTimer(hTimerFrm,TIMER_CLOSEBY2XLPM);
			//Odcisniecie SPM
			mouse_event(MOUSEEVENTF_MIDDLEUP,0,0,0,0);
			//Odznaczenie wcisniecia 2xLPM
			LBUTTONDBLCLK_EXECUTED = false;
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
		UnicodeString UserIdx = ":" + IntToStr(CloseTabContact.UserIdx);
		//Zapisywanie stanu kontaktu
		if(!CloseTabContact.IsChat)
		{
			int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&CloseTabContact);
			ContactsStateList->WriteInteger("State",JID+UserIdx,State);
		}
		//Usuwanie JID z listy aktywnych zakladek/okien
		if(TabsListEx->IndexOf(JID+Res+UserIdx)!=-1)
		{
			if(TabsList->IndexOf(JID+UserIdx)!=-1) TabsList->Delete(TabsList->IndexOf(JID+UserIdx));
			TabsListEx->Delete(TabsListEx->IndexOf(JID+Res+UserIdx));
		}
		//Usuwanie JID z listy notyfikacji wiadomosci
		if(PreMsgList->IndexOf(JID+Res+UserIdx)!=-1)
			PreMsgList->Delete(PreMsgList->IndexOf(JID+Res+UserIdx));
		PreMsgStateList->WriteInteger("PreMsgState",JID+Res+UserIdx,0);
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
					if((TabsList->Strings[Count].Pos("@plugin"))&&(TabsList->Strings[Count].Pos("ischat_")))
					{ /* Blokada czatu pochodzacego z wtyczki */ }
					else
						Ini->WriteString("Session","Tab"+IntToStr(Count+1),TabsList->Strings[Count]);
				}
			}
			//Usuwanie aktywnej zakladki
			if(JID+UserIdx==ActiveTabJIDEx) Ini->DeleteKey("SessionEx","ActiveTab");
		}
		//Usuwanie JID z kolejki przelaczania sie na nowe wiadomosci
		if(SwitchToNewMsgChk)
		{
			if(MsgList->IndexOf(JID+Res+UserIdx)!=-1)
				MsgList->Delete(MsgList->IndexOf(JID+Res+UserIdx));
			if(MsgList->IndexOf(JID)!=-1)
				MsgList->Delete(MsgList->IndexOf(JID));
		}
		//Dodawanie JID do listy ostatnio zamknietych zakladek
		if(ClosedTabsChk)
		{
			//Sprawdzanie przeprowadzono z kontaktem rozmowe
			if(OnlyConversationTabsChk)
			{
				if(AcceptClosedTabsList->IndexOf(JID+UserIdx)==-1)
					goto SkipClosedTabsChk;
			}
			//Sprawdzanie czy kontakt jest czatem z wtyczki
			if((CloseTabContact.IsChat)&&(CloseTabContact.FromPlugin))
				goto SkipClosedTabsChk;
			//Kontakt nie znajduje sie na liscie ostatnio zamknietych zakladek
			if(ClosedTabsList->IndexOf(JID+UserIdx)==-1)
			{
				//Usuwanie JID z listy z ktorymi przeprowadzono rozmowe
				if(AcceptClosedTabsList->IndexOf(JID+UserIdx)!=-1)
					AcceptClosedTabsList->Delete(AcceptClosedTabsList->IndexOf(JID+UserIdx));
				//Usuwanie interfejsu
				DestroyFrmClosedTabs();
				//Dodawanie JID do tablicy
				ClosedTabsList->Insert(0,JID+UserIdx);
				TDateTime ClosedTime = TDateTime::CurrentDateTime();
				int ClosedTimeStr = DateTimeToUnix(ClosedTime, true);
				ClosedTabsTimeList->Insert(0,IntToStr(ClosedTimeStr));
				//Maks pamietanych X elementow
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
				BuildFrmClosedTabs(true);
			}
			SkipClosedTabsChk: { /* Skip */ }
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie okna rozmowy wraz z wiadomoscia
INT_PTR __stdcall OnCloseTabMessage(WPARAM wParam, LPARAM lParam)
{
	//Zapamietanej niewyslanej wiadomosci
	if(UnsentMsgChk)
	{
		//Pobranie informacji kontaktu i wiadomosci
		TPluginContact CloseTabMessageContact = *(PPluginContact)lParam;
		UnicodeString JID = (wchar_t*)CloseTabMessageContact.JID;
		if(CloseTabMessageContact.IsChat) JID = "ischat_" + JID;
		UnicodeString UserIdx = ":" + IntToStr(CloseTabMessageContact.UserIdx);
		UnicodeString Body = (wchar_t*)wParam;
		Body = Body.Trim();
		//Wiadomosc nie jest pusta
		if(!Body.IsEmpty())
		{
			//Szybki dostep niewyslanych wiadomosci
			if(!ForceUnloadExecuted) DestroyFrmUnsentMsg();
			//Zapis pliku sesji
			TIniFile *Ini = new TIniFile(SessionFileDir);
			Ini->WriteString("Messages", JID+UserIdx, EncodeBase64(Body));
			delete Ini;
			//Szybki dostep niewyslanych wiadomosci
			if(!ForceUnloadExecuted) BuildFrmUnsentMsg(true);
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane kolorystyki AlphaControls
INT_PTR __stdcall OnColorChange(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Okno ustawien zostalo juz stworzone
		if(hSettingsForm)
		{
			//Wlaczona zaawansowana stylizacja okien
			if(ChkSkinEnabled())
			{
				TPluginColorChange ColorChange = *(PPluginColorChange)wParam;
				hSettingsForm->sSkinManager->HueOffset = ColorChange.Hue;
				hSettingsForm->sSkinManager->Saturation = ColorChange.Saturation;
				hSettingsForm->sSkinManager->Brightness = ColorChange.Brightness;
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane stanu kontaktu
INT_PTR __stdcall OnContactsUpdate(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Pobieranie danych kontaktu
		TPluginContact ContactsUpdateContact = *(PPluginContact)wParam;
		//Kontakt nie jest czatem
		if(!ContactsUpdateContact.IsChat)
		{
			//Pobieranie danych kontaktu
			UnicodeString JID = (wchar_t*)ContactsUpdateContact.JID;
			UnicodeString Res = (wchar_t*)ContactsUpdateContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			UnicodeString UserIdx = ":" + IntToStr(ContactsUpdateContact.UserIdx);
			//Pobieranie i zapisywanie stanu kontaktu
			int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&ContactsUpdateContact);
			ContactsStateList->WriteInteger("State",JID+UserIdx,State);
			//Pobieranie i zapisywanie nicku kontaktu
			UnicodeString Nick = (wchar_t*)ContactsUpdateContact.Nick;
			ContactsNickList->WriteString("Nick",JID+UserIdx,Nick);
			//Przywracanie sesji nie jest aktywne
			if(!RestoringSession)
			{
				//Ustawianie prawidlowej ikonki w popupmenu ostatnio zamknietych zakladek
				if((ClosedTabsChk)&&(FastAccessClosedTabsChk))
				{
					//Jezeli JID jest w ostatnio zamknietych zakladkach
					if(ClosedTabsList->IndexOf(JID+UserIdx)!=-1)
					{
						//Jezeli JID jest elementem popupmenu
						if(ClosedTabsList->IndexOf(JID+UserIdx)<ItemCountUnCloseTabVal)
						{
							//Aktualizacja popupmenu
							RebuildFrmClosedTabsPopupmenu();
						}
					}
				}
				//Ustawianie prawidlowej ikonki w popupmenu niewyslanych wiadomosci
				if((UnsentMsgChk)&&(FastAccessUnsentMsgChk))
				{
					//Pobieranie ilosci zamknietych zakladek
					TIniFile *Ini = new TIniFile(SessionFileDir);
					TStringList *Messages = new TStringList;
					Ini->ReadSection("Messages",Messages);
					delete Ini;
					int MsgCount = Messages->Count;
					//Jezeli sa jakies niewyslane wiadomosci
					if(MsgCount>0)
					{
						//Jezeli JID jest w niewyslanych wiadomosciach
						if(Messages->IndexOf(JID+UserIdx)!=-1)
						{
							//Jezeli JID jest elementem popupmenu
							if(Messages->IndexOf(JID+UserIdx)<5)
							{
								//Aktualizacja popupmenu
								RebuildFrmUnsentMsgPopupmenu();
							}
						}
					}
					delete Messages;
				}
				//Ustawianie prawidlowej ikonki w popupmenu ulubionych zakladek
				if((FavouritesTabsChk)&&(FastAccessFavouritesTabsChk))
				{
					//Jezeli JID jest na liscie ulubionych zakladek
					if(FavouritesTabsList->IndexOf(JID+UserIdx)!=-1)
						//Aktualizacja popupmenu
						RebuildFavouritesTabsPopupmenu();
				}
			}
			//Zmiana caption okna rozmowy
			if((TweakFrmSendTitlebarChk)&&(JID+Res+UserIdx==ActiveTabJIDEx))
			{
				//Pobranie pseudonimu kontaktu
				UnicodeString Nick = (wchar_t*)ContactsUpdateContact.Nick;
				//Pobranie opisu kontaktu
				UnicodeString Status = (wchar_t*)ContactsUpdateContact.Status;
				UnicodeString StatusEx = Status;
				Status = StringReplace(Status, "\r\n", " ", TReplaceFlags() << rfReplaceAll);
				//Przyjazniejszy wyglad identyfikatora
				UnicodeString FriendlyJID = JID;
				if(ContactsUpdateContact.FromPlugin)
				{
					if(FriendlyJID.Pos("@")) FriendlyJID.Delete(FriendlyJID.Pos("@"),FriendlyJID.Length());
				}
				//Generowani oryginalnego tekstu belki okna
				UnicodeString Titlebar;
				if(!Status.IsEmpty())
					Titlebar = Nick + " - " + FriendlyJID + " - " + Status;
				else
					Titlebar = Nick + " - " + FriendlyJID;
				//Zmienna zmienionego tekstu na belce
				UnicodeString ChangedTitlebar;
				//Pseudonim i opis kontaktu
				if(TweakFrmSendTitlebarMode==1)
				{
					//Jezeli opis nie jest pusty
					if(!Status.IsEmpty())
						ChangedTitlebar = Nick + " - " + Status;
					else
						ChangedTitlebar = Nick;
				}
				//Sam pseudonim kontaktu
				else if(TweakFrmSendTitlebarMode==2)
					ChangedTitlebar = Nick;
				//Pseudonim i identyfikator kontaktu
				else if(TweakFrmSendTitlebarMode==3)
					ChangedTitlebar = Nick + " - " + FriendlyJID;
				//Pseudonim i identyfikator kontaktu wraz z zasobem oraz opisem
				else if((TweakFrmSendTitlebarMode==4)&&(!ContactsUpdateContact.FromPlugin))
				{
					//Pobranie zasobu kontaktu
					UnicodeString Res = (wchar_t*)ContactsUpdateContact.Resource;
					//Jezeli zasob nie jest pusty
					if(!Res.IsEmpty())
					{
						//Jezeli opis nie jest pusty
						if(!Status.IsEmpty())
							ChangedTitlebar = Nick + " - " + FriendlyJID + "/" + Res + " - " + Status;
						else
							ChangedTitlebar = Nick + " - " + FriendlyJID + "/" + Res;
					}
				}
				//Nowy tekst na belce okna nie jest pusty
				if(!ChangedTitlebar.IsEmpty())
				{
					//Licznik nowych wiadomosci na oknie rozmowy jest wlaczony
					if(InactiveFrmNewMsgChk)
					{
						//Jezeli okno rozmowy jest nie aktywne
						if(hFrmSend!=GetForegroundWindow())
							//Ustawienie tymczasowego tytulu okna
							FrmSendTitlebar = ChangedTitlebar;
					}
					//Zapisywanie zmienionego tekstu belki do cache
					ChangedTitlebarList->WriteString("Titlebar",MD5(Titlebar),EncodeBase64(ChangedTitlebar));
					//Zmiana tekstu na belce
					SetWindowTextW(hFrmSend,ChangedTitlebar.w_str());
				}
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Pobieranie listy wszystkich otartych zakladek/okien
INT_PTR __stdcall OnFetchAllTabs(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
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
			if(!OldFrmSendProc) OldFrmSendProc = (WNDPROC)SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG_PTR)FrmSendProc);
			//Pobranie rozmiaru+pozycji okna rozmowy
			GetFrmSendRect();
			//Ustawienie poprawnej pozycji okna
			if(FrmSendSlideChk)
			{
				//Pobranie rozmiaru+pozycji okna rozmowy
				TRect WindowRect;
				GetWindowRect(hFrmSend,&WindowRect);
				//Ustawianie wstepnej pozycji okna rozmowy
				//Okno rozmowy chowane za lewa krawedzia ekranu
				if(FrmSendSlideEdge==1)
					SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
				//Okno rozmowy chowane za prawa krawedzia ekranu
				else if(FrmSendSlideEdge==2)
					SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-WindowRect.Width()-FrmSend_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
				//Okno rozmowy chowane za dolna krawedzia ekranu
				else if(FrmSendSlideEdge==3)
					SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
				//Okno rozmowy chowane za gorna krawedzia ekranu
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
				UnicodeString KeyValue = IntToStr((int)FrmSendRect.Left);
				SaveSetup.Value = KeyValue.w_str();
				PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
				SaveSetup.Section = L"Position";
				SaveSetup.Ident = L"MsgTop";
				KeyValue = IntToStr((int)FrmSendRect.Top);
				SaveSetup.Value = KeyValue.w_str();
				PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
				//Stan widocznosci okna rozmowy
				FrmSendVisible = true;
				//Tymczasowa blokada chowania/pokazywania okna rozmowy
				FrmSendBlockSlide = false;
			}
			//Stan widocznosci okna rozmowy
			else FrmSendVisible = true;
			//Ukrycie paska narzedzi
			if(HideToolBarChk) HideToolBar();
		}
		//Pobieranie danych kontatku
		TPluginContact FetchAllTabsContact = *(PPluginContact)lParam;
		UnicodeString JID = (wchar_t*)FetchAllTabsContact.JID;
		UnicodeString Res = (wchar_t*)FetchAllTabsContact.Resource;
		if(!Res.IsEmpty()) Res = "/" + Res;
		if(FetchAllTabsContact.IsChat)
		{
			JID = "ischat_" + JID;
			Res = "";
		}
		UnicodeString UserIdx = ":" + IntToStr(FetchAllTabsContact.UserIdx);
		//Dodawanie JID do listy otwartych zakladek
		if(TabsListEx->IndexOf(JID+Res+UserIdx)==-1)
		{
			if(TabsList->IndexOf(JID+UserIdx)==-1) TabsList->Add(JID+UserIdx);
			TabsListEx->Add(JID+Res+UserIdx);
		}
		//Pobieranie stanu kontaktu
		if(!FetchAllTabsContact.IsChat)
		{
			int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&FetchAllTabsContact);
			ContactsStateList->WriteInteger("State",JID+UserIdx,State);
		}
		//Usuwanie JID z listy ostatnio zamknietych zakladek
		if(ClosedTabsChk)
		{
			if(ClosedTabsList->IndexOf(JID+UserIdx)!=-1)
			{
				//Usuwanie JID z tablicy
				ClosedTabsTimeList->Delete(ClosedTabsList->IndexOf(JID+UserIdx));
				ClosedTabsList->Delete(ClosedTabsList->IndexOf(JID+UserIdx));
				//Maks pamietanych X elementow
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
				if((TabsList->Strings[Count].Pos("@plugin"))&&(TabsList->Strings[Count].Pos("ischat_")))
				{ /* Blokada czatu pochodzacego z wtyczki */ }
				else
					Ini->WriteString("Session","Tab"+IntToStr(Count+1),TabsList->Strings[Count]);
			}
			//Odczytywanie sesji wiadomosci
			if((RestoreMsgSessionChk)&&(RestoringSession))
			{
				UnicodeString Body = DecodeBase64(Ini->ReadString("SessionMsg", JID+UserIdx, ""));
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
		//Zakladka jest przypieta
		if((ClipTabsChk)&&(ClipTabsList->IndexOf(JID+UserIdx)!=-1))
		{
			//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
			TIniFile *Ini = new TIniFile(SessionFileDir);
			bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID+UserIdx);
			delete Ini;
			//Ustawianie pustego tekstu na przypietej zakladce
			if(!ClipTabsEx)
			{
				PluginLink.UnhookEvent(OnTabCaption);
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)L"",(LPARAM)&FetchAllTabsContact);
				PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
			}
			//Zakladka z botem Blabler
			/*if((JID=="blabler.k2t.eu")||(JID.Pos("48263287@plugin.gg")==1))
			{
				//Zmiana ikonki na zakladce
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)BLABLER,(LPARAM)&FetchAllTabsContact);
			}
			//Zakladka z botem tweet.IM
			else*/ if(JID.Pos("@twitter.tweet.im"))
			{
				//Zmiana ikonki na zakladce
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)131,(LPARAM)&FetchAllTabsContact);
			}
			//Zakladka ze zwyklym kontaktem
			else if((!JID.Pos("ischat_"))&&(MiniAvatarsClipTabsChk)&&(!ClipTabsEx))
			{
				//Pobieranie indeksu ikonki z pamieci
				int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID+UserIdx,0);
				//Ikona juz w interfejsie
				if(Icon)
				{
					//Zmiana ikonki na zakladce
					PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
				}
				//Ikona jeszcze niezaladowana do interfejsu AQQ
				else
				{
					//Plik PNG jest juz wygenerowany
					if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
					{
						//Zaladowanie ikonki do interfejsu
						Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
						//Zapisanie indeksu ikonki do pamieci
						ClipTabsIconList->WriteInteger("ClipTabsIcon",JID+UserIdx,Icon);
						//Zmiana ikonki na zakladce
						PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
					}
					//Generowanie pliku PNG 16x16 z awataru kontaktu
					else
					{
						//Odczyt pliku INI z danymi kontaktu
						TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
						//Dekodowanie sciezki awatara
						UnicodeString Avatar = DecodeBase64(Ini->ReadString("Other","Avatar",""));
						delete Ini;
						//Sciezka awatata zostala prawidlowo pobrana
						if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
						{
							//Zamienianie sciezki relatywnej na absolutna
							if(Avatar.Pos("{PROFILEPATH}"))
								Avatar = StringReplace(Avatar, "{PROFILEPATH}", GetUserDir(), TReplaceFlags());
							else
								Avatar = StringReplace(Avatar, "{APPPATH}", GetApplicationPath(), TReplaceFlags());
							//Przypisanie uchwytu do formy
							Application->Handle = (HWND)SettingsForm;
							TSettingsForm *hModalSettingsForm = new TSettingsForm(Application);
							//Konwersja awatara do ikonki PNG 16x16
							hModalSettingsForm->ConvertImage(Avatar,PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png");
							//Usuniecie uchwytu do formy
							delete hModalSettingsForm;
							//Konwersja przeszla prawidlowo
							if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
							{
								//Zaladowanie ikonki do interfejsu
								Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
								//Zapisanie indeksu ikonki do pamieci
								ClipTabsIconList->WriteInteger("ClipTabsIcon",JID+UserIdx,Icon);
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
INT_PTR __stdcall OnFetchAllTabs_GetOnlyList(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Pobieranie danych kontatku
		TPluginContact FetchAllTabsContact = *(PPluginContact)lParam;
		UnicodeString JID = (wchar_t*)FetchAllTabsContact.JID;
		UnicodeString Res = (wchar_t*)FetchAllTabsContact.Resource;
		if(!Res.IsEmpty()) Res = "/" + Res;
		if(FetchAllTabsContact.IsChat)
		{
			JID = "ischat_" + JID;
			Res = "";
		}
		UnicodeString UserIdx = ":" + IntToStr(FetchAllTabsContact.UserIdx);
		//Dodawanie JID do listy otwartych zakladek
		if(TabsListEx->IndexOf(JID+Res+UserIdx)==-1)
		{
			if(TabsList->IndexOf(JID+UserIdx)==-1) TabsList->Add(JID+UserIdx);
			TabsListEx->Add(JID+Res+UserIdx);
		}
		//Zapisywanie sesji
		if(RestoreTabsSessionChk)
		{
			TIniFile *Ini = new TIniFile(SessionFileDir);
			Ini->EraseSection("Session");
			//Petla zapisywania otwartych zakladek
			for(int Count=0;Count<TabsList->Count;Count++)
			{
				if((TabsList->Strings[Count].Pos("@plugin"))&&(TabsList->Strings[Count].Pos("ischat_")))
				{ /* Blokada czatu pochodzacego z wtyczki */ }
				else
					Ini->WriteString("Session","Tab"+IntToStr(Count+1),TabsList->Strings[Count]);
			}
			delete Ini;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------
INT_PTR __stdcall OnFetchAllTabs_RefreshTabs(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Pobieranie danych kontatku
		TPluginContact FetchAllTabsContact = *(PPluginContact)lParam;
		UnicodeString JID = (wchar_t*)FetchAllTabsContact.JID;
		if(FetchAllTabsContact.IsChat) JID = "ischat_" + JID;
		UnicodeString UserIdx = ":" + IntToStr(FetchAllTabsContact.UserIdx);
		//Jezeli zakladka jest przypieta
		if((ClipTabsChk)&&(ClipTabsList->IndexOf(JID+UserIdx)!=-1)&&(!UnloadExecuted))
		{
			//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
			TIniFile *Ini = new TIniFile(SessionFileDir);
			bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID+UserIdx);
			delete Ini;
			//Wyladowanie hooka na zmiane tekstu na zakladce
			PluginLink.UnhookEvent(OnTabCaption);
			//Ustawienie pustego tekstu na zakladce
			if(!ClipTabsEx) PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)L"",(LPARAM)&FetchAllTabsContact);
			//Ustawianie domyslnego tekstu na zakladce
			else
			{
				//Zakladka ze zwyklym kontaktem
				if(!FetchAllTabsContact.IsChat)
					PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)FetchAllTabsContact.Nick,(LPARAM)&FetchAllTabsContact);
				//Zakladka z czatem
				else
					PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)GetChannelName(JID).w_str(),(LPARAM)&FetchAllTabsContact);
			}
			//Hook na zmiane tekstu na zakladce
			PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
			//Zakladka z botem Blabler
			/*if((JID=="blabler.k2t.eu")||(JID.Pos("48263287@plugin.gg")==1))
			{
				//Zmiana ikonki na zakladce
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)BLABLER,(LPARAM)&FetchAllTabsContact);
			}
			//Zakladka z botem tweet.IM
			else*/ if(JID.Pos("@twitter.tweet.im"))
			{
				//Zmiana ikonki na zakladce
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)131,(LPARAM)&FetchAllTabsContact);
			}
			//Zakladka ze zwyklym kontaktem
			else if((!FetchAllTabsContact.IsChat)&&(MiniAvatarsClipTabsChk)&&(!ClipTabsEx))
			{
				//Pobieranie indeksu ikonki z pamieci
				int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID+UserIdx,0);
				//Ikona juz w interfejsie
				if(Icon)
				{
					//Zmiana ikonki na zakladce
					PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
				}
				//Ikona jeszcze niezaladowana do interfejsu AQQ
				else
				{
					//Plik PNG jest juz wygenerowany
					if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
					{
						//Zaladowanie ikonki do interfejsu
						Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
						//Zapisanie indeksu ikonki do pamieci
						ClipTabsIconList->WriteInteger("ClipTabsIcon",JID+UserIdx,Icon);
						//Zmiana ikonki na zakladce
						PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
					}
					//Generowanie pliku PNG 16x16 z awataru kontaktu
					else
					{
						//Odczyt pliku INI z danymi kontaktu
						TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
						//Dekodowanie sciezki awatara
						UnicodeString Avatar = DecodeBase64(Ini->ReadString("Other","Avatar",""));
						delete Ini;
						//Sciezka awatata zostala prawidlowo pobrana
						if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
						{
							//Zamienianie sciezki relatywnej na absolutna
							if(Avatar.Pos("{PROFILEPATH}"))
								Avatar = StringReplace(Avatar, "{PROFILEPATH}", GetUserDir(), TReplaceFlags());
							else
								Avatar = StringReplace(Avatar, "{APPPATH}", GetApplicationPath(), TReplaceFlags());
							//Przypisanie uchwytu do formy
							Application->Handle = (HWND)SettingsForm;
							TSettingsForm *hModalSettingsForm = new TSettingsForm(Application);
							//Konwersja awatara do ikonki PNG 16x16
							hModalSettingsForm->ConvertImage(Avatar,PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png");
							//Usuniecie uchwytu do formy
							delete hModalSettingsForm;
							//Konwersja przeszla prawidlowo
							if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
							{
								//Zaladowanie ikonki do interfejsu
								Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
								//Zapisanie indeksu ikonki do pamieci
								ClipTabsIconList->WriteInteger("ClipTabsIcon",JID+UserIdx,Icon);
								//Zmiana ikonki na zakladce
								PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&FetchAllTabsContact);
							}
						}
					}
				}
			}
			//Ustawienie domyslnej ikonki kontaktu
			else if(!FetchAllTabsContact.IsChat)
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)GetContactState(JID+UserIdx),(LPARAM)&FetchAllTabsContact);
		}
		//Zakladka nie jest przypieta
		else
		{
			//Ustawienie domyslnej ikonki kontaktu
			if(!FetchAllTabsContact.IsChat) PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)GetContactState(JID+UserIdx),(LPARAM)&FetchAllTabsContact);
			//Ustawianie domyslnego tekstu na zakladce
			//Wyladowanie hooka na zmiane tekstu na zakladce
			PluginLink.UnhookEvent(OnTabCaption);
			//Zakladka ze zwyklym kontaktem
			if(!FetchAllTabsContact.IsChat)
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)FetchAllTabsContact.Nick,(LPARAM)&FetchAllTabsContact);
			//Zakladka z czatem
			else
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)GetChannelName(JID).w_str(),(LPARAM)&FetchAllTabsContact);
			//Hook na zmiane tekstu na zakladce
			PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane lokalizacji
INT_PTR __stdcall OnLangCodeChanged(WPARAM wParam, LPARAM lParam)
{
	//Czyszczenie cache lokalizacji
	ClearLngCache();
	//Pobranie sciezki do katalogu prywatnego uzytkownika
	UnicodeString PluginUserDir = GetPluginUserDir();
	//Ustawienie sciezki lokalizacji wtyczki
	UnicodeString LangCode = (wchar_t*)lParam;
	LangPath = PluginUserDir + "\\\\Languages\\\\TabKit\\\\" + LangCode + "\\\\";
	if(!DirectoryExists(LangPath))
	{
		LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETDEFLANGCODE,0,0);
		LangPath = PluginUserDir + "\\\\Languages\\\\TabKit\\\\" + LangCode + "\\\\";
	}
	//Aktualizacja lokalizacji form wtyczki
	for(int i=0;i<Screen->FormCount;i++)
		LangForm(Screen->Forms[i]);
	//Poprawka pozycji komponentu
	if(hSettingsForm)
	{
		hSettingsForm->ItemsCountClosedTabsSpinEdit->Left = hSettingsForm->Canvas->TextWidth(hSettingsForm->ItemsCountClosedTabsSpinEdit->BoundLabel->Caption) + 20;
		hSettingsForm->UnCloseTabHotKeyInput->Left = hSettingsForm->UnCloseTabHotKeyMode2RadioButton->Left + hSettingsForm->Canvas->TextWidth(hSettingsForm->UnCloseTabHotKeyMode2RadioButton->Caption) + 26;
		hSettingsForm->CountClosedTabsSpinEdit->Left = hSettingsForm->Canvas->TextWidth(hSettingsForm->CountClosedTabsSpinEdit->BoundLabel->Caption) + 20;
		hSettingsForm->FrmMainEdgeComboBox->Left = hSettingsForm->FrmMainEdgeLabel->Left + hSettingsForm->Canvas->TextWidth(hSettingsForm->FrmMainEdgeLabel->Caption) + 6;
		hSettingsForm->FrmMainHideModeComboBox->Left = hSettingsForm->FrmMainHideModeLabel->Left + hSettingsForm->Canvas->TextWidth(hSettingsForm->FrmMainHideModeLabel->Caption) + 6;
		hSettingsForm->FrmSendEdgeComboBox->Left = hSettingsForm->FrmSendEdgeLabel->Left + hSettingsForm->Canvas->TextWidth(hSettingsForm->FrmSendEdgeLabel->Caption) + 6;
		hSettingsForm->FrmSendHideModeComboBox->Left = hSettingsForm->FrmSendHideModeLabel->Left + hSettingsForm->Canvas->TextWidth(hSettingsForm->FrmSendHideModeLabel->Caption) + 6;
		hSettingsForm->SideSlideFullScreenModeExceptionsButton->Left = hSettingsForm->SideSlideFullScreenModeCheckBox->Left + hSettingsForm->Canvas->TextWidth(hSettingsForm->SideSlideFullScreenModeCheckBox->Caption) + 26;
		hSettingsForm->CollapseImagesModeComboBox->Left = hSettingsForm->CollapseImagesCheckBox->Left + hSettingsForm->Canvas->TextWidth(hSettingsForm->CollapseImagesCheckBox->Caption) + 26;
		hSettingsForm->ShortenLinksModeComboBox->Left = hSettingsForm->ShortenLinksCheckBox->Left + hSettingsForm->Canvas->TextWidth(hSettingsForm->ShortenLinksCheckBox->Caption) + 26;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zakonczenie ladowania listy kontaktow przy starcie AQQ
INT_PTR __stdcall OnListReady(WPARAM wParam, LPARAM lParam)
{
	//Pobranie ID dla enumeracji kontaktow
	ReplyListID = GetTickCount();
	//Wywolanie enumeracji kontaktow
	PluginLink.CallService(AQQ_CONTACTS_REQUESTLIST,(WPARAM)ReplyListID,0);

	return 0;
}
//---------------------------------------------------------------------------

//Hook na wpisywany tekst w oknie rozmowy
INT_PTR __stdcall OnMsgComposing(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Zapamietywanie sesji wpisywanego tekstu
		if((RestoreTabsSessionChk)&&(RestoreMsgSessionChk))
		{
			//Pobieranie danych kontatku
			TPluginContact MsgComposingContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)MsgComposingContact.JID;
			if(MsgComposingContact.IsChat) JID = "ischat_" + JID;
			UnicodeString UserIdx = ":" + IntToStr(MsgComposingContact.UserIdx);
			//Pobieranie danych wiadomosci
			TPluginChatState ChatState = *(PPluginChatState)lParam;
			UnicodeString Body = (wchar_t*)ChatState.Text;
			Body = Body.Trim();
			//Tekst nie jest pusty
			if(!Body.IsEmpty())
			{
				//Zapisanie sesji do pliku
				TIniFile *Ini = new TIniFile(SessionFileDir);
				Ini->WriteString("SessionMsg", JID+UserIdx, EncodeBase64(Body));
				delete Ini;
			}
			//Tekst jest pusty
			else
			{
				//Nie jest aktywne przywracanie sesji
				if(!RestoringSession)
				{
					//Usuniecie sesji z pliku
					TIniFile *Ini = new TIniFile(SessionFileDir);
					if(Ini->ValueExists("SessionMsg", JID+UserIdx))
						Ini->DeleteKey("SessionMsg", JID+UserIdx);
					delete Ini;
				}
			}
		}
		//Blokowanie chowania okna rozmowy gdy kursor znajduje sie poza oknem
		if((FrmSendSlideChk)&&(FrmSendSlideHideMode==3)&&(!FrmCompletionExists))
		{
			//Okno jest widoczne i nie jest chowane/pokazywane
			if((FrmSendVisible)&&(!FrmSendSlideOut)&&(!FrmSendSlideIn))
			{
				//Kursor znajduje sie poza oknem rozmowy
				if((Mouse->CursorPos.y<FrmSendRect.Top)||(FrmSendRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmSendRect.Left)||(FrmSendRect.Right<Mouse->CursorPos.x))
				{
					//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
					KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE);
					//Tymczasowa blokada chowania/pokazywania okna rozmowy
					FrmSendBlockSlide = true;
					//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
					FrmSendUnBlockSlide = true;
					//Wlaczanie tymczasowej blokady podczas tworzenia wiadomosci
					FrmSendBlockSlideOnMsgComposing = true;
					//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
					SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,2000,(TIMERPROC)TimerFrmProc);
				}
				//Kursor znajduje w obszarze okna rozmowy
				else if(!FrmSendBlockSlide)
				{
					//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
					KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE);
					//Tymczasowa blokada chowania/pokazywania okna rozmowy
					FrmSendBlockSlide = true;
					//Wylaczenie/wylaczenie mozliwosci odblokowania tymczasowej blokady
					FrmSendUnBlockSlide = true;
					//Wlaczanie tymczasowej blokady podczas tworzenia wiadomosci
					FrmSendBlockSlideOnMsgComposing = true;
					//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
					SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,500,(TIMERPROC)TimerFrmProc);
				}
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie menu kontekstowego pola wiadomosci
INT_PTR __stdcall OnMsgContextClose(WPARAM wParam, LPARAM lParam)
{
	//Usuniecie elemetntu do szybkiego cytowania / komunikator nie jest zamykany
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
INT_PTR __stdcall OnMsgContextPopup(WPARAM wParam, LPARAM lParam)
{
	//Utworzenie elemetntu do szybkiego cytowania / komunikator nie jest zamykany
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
			//Pobrano tekst ze schowka
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
				QuickQuoteItem.pszCaption = GetLangStr("PasteAsQuote").w_str();
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

//Hook na zamkniecie notyfikacji o nowej wiadomosci w zasobniku systemowym
INT_PTR __stdcall OnNotificationClosed(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie identyfikatora kontaktu
	UnicodeString JID = (wchar_t*)wParam;
	//Pobieranie indeksu konta kontaktu
	UnicodeString UserIdx =	":" + IntToStr((int)lParam);
	//Usuniecie nieprzeczytanej wiadomosci z kolejki
	if(UnOpenMsgList->IndexOf(JID+UserIdx)!=-1)
		UnOpenMsgList->Delete(UnOpenMsgList->IndexOf(JID+UserIdx));

	return 0;
}
//---------------------------------------------------------------------------

//Hook na pobieranie otwieranie adresow URL i przekazywanie plikow do aplikacji
INT_PTR __stdcall OnPerformCopyData(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
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
		//Wlaczona zostala funkcjonalnosc chowania okna kontaktow i okno jest jeszcze poza krawedzia ekranu
		if((FrmMainSlideChk)&&(!FrmMainVisible)&&(FrmMainSlideIn))
		{
			//Jest to plik lokalny dodatku do AQQ
			if((FileExists(CopyData))&&(ExtractFileExt(CopyData)==".aqq"))
			{
				//Wylaczenie wczesniej aktywowanego wysuwania okna kontaktow
				KillTimer(hTimerFrm,TIMER_FRMMAIN_SLIDEIN);
				//Status wysuwania okna kontaktow zza krawedzi ekranu
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
				CollapseImagesItem.pszCaption = GetLangStr("Open").w_str();
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
INT_PTR __stdcall OnPreSendMsg(WPARAM wParam, LPARAM lParam)
{
	//Dodawanie JID do listy kontaktow z ktorymy przeprowadzono rozmowe / komunikator nie jest zamykany
	if((ClosedTabsChk)&&(!ForceUnloadExecuted))
	{
		//Pobranie danych kontaktu
		TPluginContact PreSendMsgContact = *(PPluginContact)wParam;
		UnicodeString JID = (wchar_t*)PreSendMsgContact.JID;
		if(PreSendMsgContact.IsChat) JID = "ischat_" + JID;
		UnicodeString UserIdx = ":" + IntToStr(PreSendMsgContact.UserIdx);
		//Dodawanie JID do listy
		if(AcceptClosedTabsList->IndexOf(JID+UserIdx)==-1)
			AcceptClosedTabsList->Add(JID+UserIdx);
	}
	//Usuwanie zapamietanej sesji wpisanego w oknie rozmowy tekstu
	if((RestoreTabsSessionChk)&&(RestoreMsgSessionChk))
	{
		//Pobranie danych kontaktu
		TPluginContact PreSendMsgContact = *(PPluginContact)wParam;
		UnicodeString JID = (wchar_t*)PreSendMsgContact.JID;
		if(PreSendMsgContact.IsChat) JID = "ischat_" + JID;
		UnicodeString UserIdx = ":" + IntToStr(PreSendMsgContact.UserIdx);
		//Usuniecie sesji z pliku
		TIniFile *Ini = new TIniFile(SessionFileDir);
		if(Ini->ValueExists("SessionMsg", JID+UserIdx))
			Ini->DeleteKey("SessionMsg", JID+UserIdx);
		delete Ini;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na aktywna zakladke
INT_PTR __stdcall OnPrimaryTab(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Nie zostala wywolana proceduta wyladowania wtyczki
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
				if(!OldFrmSendProc) OldFrmSendProc = (WNDPROC)SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG_PTR)FrmSendProc);
				//Pobranie rozmiaru+pozycji okna rozmowy
				GetFrmSendRect();
				//Ustawienie poprawnej pozycji okna
				if(FrmSendSlideChk)
				{
					//Pobranie rozmiaru+pozycji okna rozmowy
					TRect WindowRect;
					GetWindowRect(hFrmSend,&WindowRect);
					//Ustawianie wstepnej pozycji okna rozmowy
					//Okno rozmowy chowane za lewa krawedzia ekranu
					if(FrmSendSlideEdge==1)
						SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
					//Okno rozmowy chowane za prawa krawedzia ekranu
					else if(FrmSendSlideEdge==2)
						SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-WindowRect.Width()-FrmSend_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
					//Okno rozmowy chowane za dolna krawedzia ekranu
					else if(FrmSendSlideEdge==3)
						SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
					//Okno rozmowy chowane za gorna krawedzia ekranu
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
					UnicodeString KeyValue = IntToStr((int)FrmSendRect.Left);
					SaveSetup.Value = KeyValue.w_str();
					PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
					SaveSetup.Section = L"Position";
					SaveSetup.Ident = L"MsgTop";
					KeyValue = IntToStr((int)FrmSendRect.Top);
					SaveSetup.Value = KeyValue.w_str();
					PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
					//Stan widocznosci okna rozmowy
					FrmSendVisible = true;
					//Tymczasowa blokada chowania/pokazywania okna rozmowy
					FrmSendBlockSlide = false;
				}
				//Stan widocznosci okna rozmowy
				else FrmSendVisible = true;
				//Ukrycie paska narzedzi
				if(HideToolBarChk) HideToolBar();
			}
			//Pobieranie danych kontaktu
			TPluginContact PrimaryTabContact = *(PPluginContact)lParam;
			UnicodeString JID = (wchar_t*)PrimaryTabContact.JID;
			UnicodeString Res = (wchar_t*)PrimaryTabContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(PrimaryTabContact.IsChat)
			{
				JID = "ischat_" + JID;
				Res = "";
			}
			UnicodeString UserIdx = ":" + IntToStr(PrimaryTabContact.UserIdx);
			//Aktywna zakladka
			ActiveTabJID = JID+UserIdx;
			ActiveTabJIDEx = JID+Res+UserIdx;
			//Zapisywanie sesji
			if(RestoreTabsSessionChk)
			{
				//Zapisywanie aktywnej zakladki
				TIniFile *Ini = new TIniFile(SessionFileDir);
				if(!PrimaryTabContact.IsChat)
					Ini->WriteString("SessionEx","ActiveTab",ActiveTabJID);
				else
					Ini->DeleteKey("SessionEx","ActiveTab");
				delete Ini;
			}
			//Zmiana tekstu paska tytulu okna rozmowy
			if(!PrimaryTabContact.IsChat)
			{
				//Pobieranie danych konatku
				UnicodeString Nick = (wchar_t*)PrimaryTabContact.Nick;
				UnicodeString Status = (wchar_t*)PrimaryTabContact.Status;
				Status = StringReplace(Status, "\r\n", " ", TReplaceFlags() << rfReplaceAll);
				UnicodeString FriendlyJID = JID;
				if(PrimaryTabContact.FromPlugin)
				{
					if(FriendlyJID.Pos("@")) FriendlyJID.Delete(FriendlyJID.Pos("@"),FriendlyJID.Length());
				}
				//Jezeli funcjonalnosc jest wlaczona
				if(TweakFrmSendTitlebarChk)
				{
					//Pobranie aktualnego tekstu belki okna
					wchar_t TitlebarW[512];
					GetWindowTextW(hFrmSend, TitlebarW, sizeof(TitlebarW));
					UnicodeString Titlebar = TitlebarW;
					Titlebar = StringReplace(Titlebar, "\r\n", "", TReplaceFlags() << rfReplaceAll);
					//Zmienna zmienionego tekstu na belce
					UnicodeString ChangedTitlebar;
					//Pseudonim i opis kontaktu
					if(TweakFrmSendTitlebarMode==1)
					{
						//Jezeli opis nie jest pusty
						if(!Status.IsEmpty())
							ChangedTitlebar = Nick + " - " + Status;
						else
							ChangedTitlebar = Nick;
					}
					//Sam pseudonim kontaktu
					else if(TweakFrmSendTitlebarMode==2)
						ChangedTitlebar = Nick;
					//Pseudonim i identyfikator kontaktu
					else if(TweakFrmSendTitlebarMode==3)
						ChangedTitlebar = Nick + " - " + FriendlyJID;
					//Pseudonim i identyfikator kontaktu wraz z zasobem oraz opisem
					else if((TweakFrmSendTitlebarMode==4)&&(!PrimaryTabContact.FromPlugin))
					{
						//Pobranie zasobu kontaktu
						UnicodeString Res = (wchar_t*)PrimaryTabContact.Resource;
						//Jezeli zasob nie jest pusty
						if(!Res.IsEmpty())
						{
							//Jezeli opis nie jest pusty
							if(!Status.IsEmpty())
								ChangedTitlebar = Nick + " - " + FriendlyJID + "/" + Res + " - " + Status;
							else
								ChangedTitlebar = Nick + " - " + FriendlyJID + "/" + Res;
						}
					}
					//Nowy tekst na belke okna nie jest pusty
					if(!ChangedTitlebar.IsEmpty())
					{
						//Zapisywanie zmienionego tekstu belki do cache
						ChangedTitlebarList->WriteString("Titlebar",MD5(Titlebar),EncodeBase64(ChangedTitlebar));
						//Zmiana tekstu na belce
						SetWindowTextW(hFrmSend,ChangedTitlebar.w_str());
					}
				}
				//Jezeli funkcjonalnosc jest wylaczona
				else
				{
					//Jezeli opis nie jest pusty
					if(!Status.IsEmpty())
						//Ustawianie nowego testku na pasku tytulu okna rozmowy
						SetWindowTextW(hFrmSend,(Nick + " - " + FriendlyJID + " - " + Status).w_str());
					//Jezeli opis jest pusty
					else
						//Ustawianie nowego testku na pasku tytulu okna rozmowy
						SetWindowTextW(hFrmSend,(Nick + " - " + FriendlyJID).w_str());
				}
			}
		}
		//Zmiana tekstu paska tytulu okna rozmowy przy wyladowaniu wtyczki
		else if(TweakFrmSendTitlebarChk)
		{
			//Pobieranie danych kontaktu
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
				Status = StringReplace(Status, "\r\n", " ", TReplaceFlags() << rfReplaceAll);
				//Jezeli opis nie jest pusty
				if(!Status.IsEmpty())
					//Ustawianie nowego testku na pasku tytulu okna rozmowy
					SetWindowTextW(hFrmSend,(Nick + " - " + JID + " - " + Status).w_str());
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
INT_PTR __stdcall OnPrimaryTab_GetOnlyList(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Pobieranie danych kontaktu
		TPluginContact PrimaryTabContact = *(PPluginContact)lParam;
		//Zapisywanie sesji
		if(RestoreTabsSessionChk)
		{
			//Zapisywanie aktywnej zakladki
			TIniFile *Ini = new TIniFile(SessionFileDir);
			if(!PrimaryTabContact.IsChat)
			{
				UnicodeString JID = (wchar_t*)PrimaryTabContact.JID;
				UnicodeString Res = (wchar_t*)PrimaryTabContact.Resource;
				UnicodeString UserIdx = ":" + IntToStr(PrimaryTabContact.UserIdx);
				Ini->WriteString("SessionEx","ActiveTab",JID+UserIdx);
			}
			else
				Ini->DeleteKey("SessionEx","ActiveTab");
			delete Ini;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na odbieranie wiadomosci
INT_PTR __stdcall OnRecvMsg(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Przelaczanie na zakladke z nowa wiadomoscia
		if(SwitchToNewMsgChk)
		{
			//Pobieranie danych kontaktu
			TPluginContact RecvMsgContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
			UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(RecvMsgContact.IsChat)
			{
				JID = "ischat_" + JID;
				Res = "";
			}
			UnicodeString UserIdx = ":" + IntToStr(RecvMsgContact.UserIdx);
			//JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
			if((JID+Res+UserIdx!=ActiveTabJIDEx)&&(TabsListEx->IndexOf(JID+Res+UserIdx)!=-1))
			{
				//Pobieranie danych wiadomosci
				TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
				//Rodzaj wiadomosci
				if(RecvMsgMessage.Kind!=MSGKIND_RTT)
				{
					//Wiadomosc nie jest pusta
					if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
					{
						//Dodawanie JID do kolejki nowych wiadomosci
						if(MsgList->IndexOf(JID+Res+UserIdx)==-1)
							MsgList->Add(JID+Res+UserIdx);
					}
				}
			}
		}
		//Licznik nieprzeczytanych wiadomosci na pasku tytulu okna rozmowy
		if(InactiveFrmNewMsgChk)
		{
			//Pobieranie danych kontaktu
			TPluginContact RecvMsgContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
			UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(RecvMsgContact.IsChat)
			{
				JID = "ischat_" + JID;
				Res = "";
			}
			UnicodeString UserIdx = ":" + IntToStr(RecvMsgContact.UserIdx);
			//Zakladka jest otwarta
			if(TabsListEx->IndexOf(JID+Res+UserIdx)!=-1)
			{
				//Okno rozmowy jest nieaktywne
				if(hFrmSend!=GetForegroundWindow())
				{
					//Pobieranie danych wiadomosci
					TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
					//Rodzaj wiadomosci
					if(RecvMsgMessage.Kind!=MSGKIND_RTT)
					{
						//Wiadomosc nie jest pusta
						if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
						{
							//Pobranie oryginalnego tekstu paska tytulu okna rozmowy
							if((FrmSendTitlebar.IsEmpty())&&(!InactiveFrmNewMsgCount))
							{
								wchar_t TitlebarW[512];
								GetWindowTextW(hFrmSend, TitlebarW, sizeof(TitlebarW));
								FrmSendTitlebar = TitlebarW;
							}
							//Dodanie 1 do licznika nieprzeczytanych wiadomosci
							InactiveFrmNewMsgCount++;
							//Ustawianie nowego tekstu paska tytulu okna rozmowy
							SetWindowTextW(hFrmSend,("[" + IntToStr(InactiveFrmNewMsgCount)+ "] " + FrmSendTitlebar).w_str());
						}
					}
				}
				//Okno rozmowy jest aktywne
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
		//Miganie diodami LED klawiatury - wlaczenie mrugania
		if(KeyboardFlasherChk)
		{
			//Pobieranie danych kontaktu
			TPluginContact RecvMsgContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
			UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(RecvMsgContact.IsChat)
			{
				JID = "ischat_" + JID;
				Res = "";
			}
			UnicodeString UserIdx = ":" + IntToStr(RecvMsgContact.UserIdx);
			//Okno rozmowy jest nieaktywne lub wiadomoc nie pochodzi z aktywnej zakladki
			if((GetForegroundWindow()!=hFrmSend)||((JID+Res+UserIdx)!=ActiveTabJIDEx))
			{
				//Pobieranie danych wiadomosci
				TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
				//Rodzaj wiadomosci
				if(RecvMsgMessage.Kind!=MSGKIND_RTT)
				{
					//Wiadomosc nie jest pusta
					if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
					{
						//Dodawanie JID do listy nieprzeczytanych wiadomosci
						if(UnreadMsgList->IndexOf(JID+Res+UserIdx)==-1)
							UnreadMsgList->Add(JID+Res+UserIdx);
						//Wlaczenie mrugania
						if((!hFlasherThread)&&(!hFlasherKeyboardThread))
						{
							hFlasherThread = CreateEvent(NULL, FALSE, FALSE, FLASHER);
							if(KeyboardFlasherModeChk==0) hFlasherKeyboardThread = FlashKeyboardLightInThread(KEYBOARD_SCROLL_LOCK_ON, 250, FLASHER);
							else if(KeyboardFlasherModeChk==1) hFlasherKeyboardThread = FlashKeyboardLightInThread(KEYBOARD_NUM_LOCK_ON, 250, FLASHER);
							else hFlasherKeyboardThread = FlashKeyboardLightInThread(KEYBOARD_CAPS_LOCK_ON, 250, FLASHER);
						}
					}
				}
			}
		}
		//Licznik nieprzeczytanych wiadomosci na zakladkach
		if(InactiveTabsNewMsgChk)
		{
			//Pobieranie danych kontaktu
			TPluginContact RecvMsgContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
			UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(RecvMsgContact.IsChat)
			{
				JID = "ischat_" + JID;
				Res = "";
			}
			UnicodeString UserIdx = ":" + IntToStr(RecvMsgContact.UserIdx);
			//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
			TIniFile *Ini = new TIniFile(SessionFileDir);
			bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID+UserIdx);
			delete Ini;
			//Licznik nie ma byc dodawany na przypietej zakladce
			if((ClipTabsChk)&&(InactiveClipTabsChk)&&(ClipTabsList->IndexOf(JID+UserIdx)!=-1)&&(!ClipTabsEx))
				goto SkipInactiveTabsNewMsgChk;
			//JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
			if((JID+Res+UserIdx!=ActiveTabJIDEx)&&(TabsListEx->IndexOf(JID+Res+UserIdx)!=-1))
			{
				//Pobieranie danych wiadomosci
				TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
				//Rodzaj wiadomosci
				if(RecvMsgMessage.Kind!=MSGKIND_RTT)
				{
					//Wiadomosc nie jest pusta
					if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
					{
						//Pobieranie stanu nowych wiadomosci
						int InactiveTabsCount = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID+Res+UserIdx,0);
						//Zmiana stanu nowych wiadomosci
						InactiveTabsCount++;
						//Zapisywanie stanu nowych wiadomosci
						InactiveTabsNewMsgCount->WriteInteger("TabsMsg",JID+Res+UserIdx,InactiveTabsCount);
						//Ustawianie tekstu na zakladce
						UnicodeString TabCaption;
						//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
						TIniFile *Ini = new TIniFile(SessionFileDir);
						bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID+UserIdx);
						delete Ini;
						//Zakladka jest przypiera i nazwa zakladki jest ukryta
						if((ClipTabsList->IndexOf(JID+UserIdx)!=-1)&&(!ClipTabsEx)) TabCaption = "";
						//Zakladka nie jest przypieta
						else
						{
							//Zakladka ze zwyklym kontaktem
							if(!RecvMsgContact.IsChat) TabCaption = (wchar_t*)RecvMsgContact.Nick;
							//Zakladka z czatem
							else TabCaption = GetChannelName(JID);
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
			//Pobieranie danych kontaktu
			TPluginContact RecvMsgContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
			UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(RecvMsgContact.IsChat)
			{
				JID = "ischat_" + JID;
				Res = "";
			}
			UnicodeString UserIdx = ":" + IntToStr(RecvMsgContact.UserIdx);
			//Zakladka jest otwarta
			if(TabsListEx->IndexOf(JID+Res+UserIdx)!=-1)
			{
				//JID wiadomosci jest inny niz JID aktwnej zakladki lub okno rozmowy jest nieaktywne
				if((JID+Res+UserIdx!=ActiveTabJIDEx)||(hFrmSend!=GetForegroundWindow()))
				{
					//Pobieranie danych wiadomosci
					TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
					//Rodzaj wiadomosci
					if((!RecvMsgMessage.ShowAsOutgoing)&&((RecvMsgMessage.Kind==MSGKIND_CHAT)||(RecvMsgMessage.Kind==MSGKIND_GROUPCHAT)))
					{
						UnicodeString Body = (wchar_t*)RecvMsgMessage.Body;
						//Wiadomosc nie jest pusta
						if(!Body.IsEmpty())
						{
							//Pobieranie danych kontaktu
							UnicodeString Nick = (wchar_t*)RecvMsgContact.Nick;
							int UserIdx = RecvMsgContact.UserIdx;
							//W wiadomosci znajduje sie obrazek
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
									ImgBodyTmp.Delete(1,ImgBodyTmp.Pos("<AQQ_CACHE_ITEM")-1);
									ImgBodyTmp.Delete(ImgBodyTmp.Pos(">")+1,ImgBodyTmp.Length());
									BodyTmp = StringReplace(BodyTmp, ImgBodyTmp, "", TReplaceFlags());
								}
								//Usuwanie bialych znakow
								BodyTmp.Trim();
								//Sam obrazek w tresci wiadomosci
								if(BodyTmp.IsEmpty())
								{
									//Ustawianie tresci na podstawie plci kontaktu
									if(ChkContactGender(JID))
									{
										if(ItemsCount==1) Body = "[" + GetLangStr("SentImg1M") + "]";
										else if((ItemsCount>1)&&(ItemsCount<5)) Body = "[" + GetLangStr("SentImg2M") + "]";
										else Body = "[" + GetLangStr("SentImg3M") + "]";
									}
									else
									{
										if(ItemsCount==1) Body = "[" + GetLangStr("SentImg1F") + "]";
										else if((ItemsCount>1)&&(ItemsCount<5)) Body = "[" + GetLangStr("SentImg2F") + "]";
										else Body = "[" + GetLangStr("SentImg3F") + "]";
									}
									Body = StringReplace(Body, "CC_NICK", Nick, TReplaceFlags());
									Body = StringReplace(Body, "CC_COUNT", IntToStr(ItemsCount), TReplaceFlags());
								}
								//Obrazki wraz z tekstem
								else
								{
									//Petla na wystepowanie obrazkow w wiadomosci
									while(Body.Pos("<AQQ_CACHE_ITEM"))
									{
										//Tymczasowe usuwanie obrazka z wiadomosci
										UnicodeString ImgBodyTmp = BodyTmp;
										ImgBodyTmp.Delete(1,ImgBodyTmp.Pos("<AQQ_CACHE_ITEM")-1);
										ImgBodyTmp.Delete(ImgBodyTmp.Pos(">")+1,ImgBodyTmp.Length());
										Body = StringReplace(Body, ImgBodyTmp, "["+GetLangStr("Img")+"]", TReplaceFlags());
									}
								}
							}
							//Generowanie ID dla chmurek
							DWORD TickID = GetTickCount();
							//Nick
							TPluginShowInfo PluginShowInfo;
							PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
							if(CloudTickModeChk) PluginShowInfo.Event = tmeMsgCap;
							else PluginShowInfo.Event = tmePseudoMsgCap;
							PluginShowInfo.Text = Nick.w_str();
							PluginShowInfo.ImagePath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0);
							PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
							if(CloudTickModeChk) PluginShowInfo.Tick = TickID;
							else PluginShowInfo.Tick = 0;
							UnicodeString ActionName = "EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res;
							PluginShowInfo.ActionID = ActionName.w_str();
							PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
							//Body
							PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
							PluginShowInfo.Event = tmeInfo;
							PluginShowInfo.Text = Body.w_str();
							PluginShowInfo.ImagePath = L"";
							PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
							if(CloudTickModeChk) PluginShowInfo.Tick = TickID;
							else PluginShowInfo.Tick = 0;
							ActionName = "EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res;
							PluginShowInfo.ActionID = ActionName.w_str();
							PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
						}
					}
				}
			}
		}
		//Notyfikcja pisania wiadomosci
		if(ChatStateNotiferNewMsgChk)
		{
			//Pobieranie danych kontaktu
			TPluginContact RecvMsgContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
			UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			UnicodeString UserIdx = ":" + IntToStr(RecvMsgContact.UserIdx);
			//Zakladka jest otwarta i kontakt nie jest czatem
			if((TabsListEx->IndexOf(JID+Res+UserIdx)!=-1)&&(!RecvMsgContact.IsChat))
			{
				//Pobieranie danych wiadomosci
				TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
				int ChatState = RecvMsgMessage.ChatState;
				//Okno rozmowy jest nieaktywne
				if(hFrmSend!=GetForegroundWindow())
				{
					//Pisanie wiadomosci
					if((ChatState==CHAT_COMPOSING)&&(LastChatState!=ChatState))
					{
						//Zapamietywanie aktualnego stanu pisania wiadomosci
						LastChatState = ChatState;
						//Pobranie aktualnych ikonek
						if(!hIconSmall) hIconSmall = (HICON)SendMessage(hFrmSend, WM_GETICON, ICON_SMALL, 0);
						if(!hIconBig) hIconBig = (HICON)SendMessage(hFrmSend, WM_GETICON, ICON_BIG, 0);
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
						if(!hIconSmall) hIconSmall = (HICON)SendMessage(hFrmSend, WM_GETICON, ICON_SMALL, 0);
						if(!hIconBig) hIconBig = (HICON)SendMessage(hFrmSend, WM_GETICON, ICON_BIG, 0);
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
							SendMessage(hFrmSend, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
							hIconSmall = 0;
						}
						//Ustawienie oryginalnej duzej ikonki
						if(hIconBig)
						{
							SendMessage(hFrmSend, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
							hIconBig = 0;
						}
					}
				}
				//Notyfikacja na zakladkach
				//Pisanie wiadomosci
				if((ChatState==CHAT_COMPOSING)&&(ChatState!=PreMsgStateList->ReadInteger("PreMsgState",JID+Res+UserIdx,0)))
				{
					//Dodawanie JID do listy notyfikacji wiadomosci
					if(PreMsgList->IndexOf(JID+Res+UserIdx)==-1)
						PreMsgList->Add(JID+Res+UserIdx);
					PreMsgStateList->WriteInteger("PreMsgState",JID+Res+UserIdx,CHAT_COMPOSING);
					//Zmiana ikonki na zakladce
					PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)COMPOSING,(LPARAM)&RecvMsgContact);
				}
				//Spauzowanie
				else if((ChatState==CHAT_PAUSED)&&(ChatState!=PreMsgStateList->ReadInteger("PreMsgState",JID+Res+UserIdx,0)))
				{
					//Dodawanie JID do listy notyfikacji wiadomosci
					if(PreMsgList->IndexOf(JID+Res+UserIdx)==-1)
						PreMsgList->Add(JID+Res+UserIdx);
					PreMsgStateList->WriteInteger("PreMsgState",JID+Res+UserIdx,CHAT_PAUSED);
					//Zmiana ikonki na zakladce
					PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)PAUSE,(LPARAM)&RecvMsgContact);
				}
				//Zamkniecie okna rozmowy
				else if((ChatState==CHAT_GONE)&&(ChatState!=PreMsgStateList->ReadInteger("PreMsgState",JID+Res+UserIdx,0))&&(ChatGoneNotiferNewMsgChk))
				{
					//Dodawanie JID do listy notyfikacji wiadomosci
					if(PreMsgList->IndexOf(JID+Res+UserIdx)==-1)
						PreMsgList->Add(JID+Res+UserIdx);
					PreMsgStateList->WriteInteger("PreMsgState",JID+Res+UserIdx,CHAT_GONE);
					//Zmiana ikonki na zakladce
					PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)GONE,(LPARAM)&RecvMsgContact);
					//Pokazywanie chmurki informacyjnej
					if(ChatGoneCloudNotiferNewMsgChk)
					{
						//Ustawianie tresci powiadomienia na podstawie plci kontaktu
						UnicodeString Text;
						if(ChkContactGender(JID)) Text = GetLangStr("ClosedWndM");
						else Text = GetLangStr("ClosedWndF");
            //Pobieranie pseudonimu kontaktu
						UnicodeString Nick = (wchar_t*)RecvMsgContact.Nick;
						Text = StringReplace(Text, "CC_NICK", Nick, TReplaceFlags());
						//Pokazanie chmurki informacyjnej
						TPluginShowInfo PluginShowInfo;
						PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
						PluginShowInfo.Event = tmeInfo;
						PluginShowInfo.Text = Text.w_str();
						PluginShowInfo.ImagePath = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,23,0);
						PluginShowInfo.TimeOut = 1000 * CloudTimeOut;
						PluginShowInfo.ActionID = L"";
						PluginShowInfo.Tick = 0;
						PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
					}
					//Odtworzenie dzwieku
					if((ChatGoneSoundNotiferNewMsgChk)&&(ChkSoundEnabled())) PluginLink.CallService(AQQ_SYSTEM_PLAYSOUND,SOUND_NEWS,1);
				}
				//Inny stan
				else if(ChatState!=PreMsgStateList->ReadInteger("PreMsgState",JID+Res+UserIdx,0))
				{
					//Usuwanie JID do listy notyfikacji wiadomosci
					if(PreMsgList->IndexOf(JID+Res+UserIdx)!=-1)
						PreMsgList->Delete(PreMsgList->IndexOf(JID+Res+UserIdx));
					PreMsgStateList->WriteInteger("PreMsgState",JID+Res+UserIdx,0);
					//Zmiana ikonki na zakladce
					ChatState = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&RecvMsgContact);
					PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)ChatState,(LPARAM)&RecvMsgContact);
				}
			}
		}
		//Dodawanie JID do listy kontaktow z ktorymy przeprowadzono rozmowe
		if(ClosedTabsChk)
		{
			//Pobieranie danych kontaktu
			TPluginContact RecvMsgContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
			if(RecvMsgContact.IsChat) JID = "ischat_" + JID;
			UnicodeString UserIdx = ":" + IntToStr(RecvMsgContact.UserIdx);
			if(AcceptClosedTabsList->IndexOf(JID+UserIdx)==-1)
				AcceptClosedTabsList->Add(JID+UserIdx);
		}
		//Wysuwanie okna rozmowy zza krawedzi ekranu przy przyjsciu nowej wiadomosci
		if((FrmSendSlideChk)&&(SlideInAtNewMsgChk)&&(!FrmSendVisible)&&(!FrmSendBlockSlide))
		{
			//Pobieranie danych kontaktu
			TPluginContact RecvMsgContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
			UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(RecvMsgContact.IsChat) Res = "";
			UnicodeString UserIdx = ":" + IntToStr(RecvMsgContact.UserIdx);
			//Jezeli zakladka jest otwarta
			if(TabsListEx->IndexOf(JID+Res+UserIdx)!=-1)
			{
				//Pobieranie danych wiadomosci
				TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
				//Rodzaj wiadomosci
				if((!RecvMsgMessage.ShowAsOutgoing)&&((RecvMsgMessage.Kind==MSGKIND_CHAT)||(RecvMsgMessage.Kind==MSGKIND_GROUPCHAT)))
				{
					//Jezeli wiadomosc nie jest pusta
					if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
					{
						//Sprawdzanie czy aktywna jest aplikacja pelno ekranowa
						ChkFullScreenMode();
						//Blokowanie wysuwania przy aplikacji pelnoekranowej
						if(((FullScreenMode)&&(!SideSlideFullScreenModeChk))||((FullScreenModeExeptions)&&(SideSlideFullScreenModeChk)))
						{ /* Blokada */ }
						else
						{
							//Status wysuwania okna rozmowy zza krawedzi ekranu
							FrmSendSlideIn = true;
							//Odswiezenie okna rozmowy
							RefreshFrmSend();
							//Aktywacja okna rozmowy + ustawienie okna na wierzchu
							ActivateAndSetTopmostFrmSend();
							//Wlaczenie pokazywania okna rozmowy (part II)
							SetTimer(hTimerFrm,TIMER_FRMSEND_SLIDEIN,FrmSendStepInterval,(TIMERPROC)TimerFrmProc);
							//Schowanie okna kontaktow (gdy okno ustraci fokus)
							if((FrmMainSlideChk)&&(FrmMainSlideHideMode==1))
							{
								//Okno kontatkow jest widoczne, aktualnie nie jest chowane/wysuwane, nie jest aktywna blokada
								if((FrmMainVisible)&&(!FrmMainSlideOut)&&(!FrmMainSlideIn)&&(!FrmMainBlockSlide))
								{
									//Status chowania okna kontaktow za krawedz ekranu
									FrmMainSlideOut = true;
									//Schowanie okna kontaktow przy aktywacji okna rozmowy
									FrmMainSlideOutActiveFrmSend = true;
									//Wlaczenie chowania okna kontaktow (part I)
									SetTimer(hTimerFrm,TIMER_FRMMAIN_PRE_SLIDEOUT,1,(TIMERPROC)TimerFrmProc);
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
			//Pobieranie danych kontaktu
			TPluginContact RecvMsgContact = *(PPluginContact)wParam;
			UnicodeString JID = (wchar_t*)RecvMsgContact.JID;
			UnicodeString Res = (wchar_t*)RecvMsgContact.Resource;
			if(!Res.IsEmpty()) Res = "/" + Res;
			if(RecvMsgContact.IsChat)
			{
				JID = "ischat_" + JID;
				Res = "";
			}
			UnicodeString UserIdx = ":" + IntToStr(RecvMsgContact.UserIdx);
			//Jezeli zakladka nie jest otwarta
			if(TabsListEx->IndexOf(JID+Res+UserIdx)==-1)
			{
				//Pobieranie danych wiadomosci
				TPluginMessage RecvMsgMessage = *(PPluginMessage)lParam;
				//Rodzaj wiadomosci
				if(RecvMsgMessage.Kind!=MSGKIND_RTT)
				{
					//Wiadomosc nie jest pusta
					if(!((UnicodeString)((wchar_t*)RecvMsgMessage.Body)).IsEmpty())
					{
						//Dodawanie JID do kolejki nowych wiadomosci
						if(UnOpenMsgList->IndexOf(JID+Res+UserIdx)==-1)
							UnOpenMsgList->Add(JID+Res+UserIdx);
					}
				}
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na enumeracje listy kontatkow
INT_PTR __stdcall OnReplyList(WPARAM wParam, LPARAM lParam)
{
	//Sprawdzanie ID wywolania enumeracji / komunikator nie jest zamykany
	if((wParam==ReplyListID)&&(!ForceUnloadExecuted))
	{
		//Pobieranie danych kontaktu
		TPluginContact ReplyListContact = *(PPluginContact)lParam;
		//Kontakt nie jest czatem
		if(!ReplyListContact.IsChat)
		{
			//Pobieranie identyfikatora
			UnicodeString JID = (wchar_t*)ReplyListContact.JID;
			//Pobieranie indeksu konta
			UnicodeString UserIdx = ":" + IntToStr(ReplyListContact.UserIdx);
			//Pobieranie i zapisywanie stanu kontaktu
			int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)&ReplyListContact);
			ContactsStateList->WriteInteger("State",JID+UserIdx,State);
			//Pobieranie i zapisywanie nicku kontaktu
			UnicodeString Nick = (wchar_t*)ReplyListContact.Nick;
			ContactsNickList->WriteString("Nick",JID+UserIdx,Nick);
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na restart komunikatora
/*INT_PTR __stdcall OnSystemRestart(WPARAM wParam, LPARAM lParam)
{
	if((RestoreTabsSessionChk)&&(!ManualRestoreTabsSessionChk))
		ManualRestoreTabsSessionChk = true;

	return 0;
}*/
//---------------------------------------------------------------------------

//Hook na zmiane nazwy zasobu przez wtyczke ResourcesChanger
INT_PTR __stdcall OnResourceChanged(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
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
				SetForegroundWindow(hFrmMain);
			}
			//Pobranie rozmiaru+pozycji okna kontatkow
			TRect WindowRect;
			GetWindowRect(hFrmMain,&WindowRect);
			//Ustawianie wstepnej pozycji okna rozmowy
			//Okno kontaktow chowane za lewa krawedzia ekranu
			if(FrmMainSlideEdge==1)
				SetWindowPos(hFrmMain,HWND_TOP,0+FrmMain_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
			//Okno kontaktow chowane za prawa krawedzia ekranu
			else if(FrmMainSlideEdge==2)
				SetWindowPos(hFrmMain,HWND_TOP,Screen->Width-WindowRect.Width()-FrmMain_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
			//Okno kontaktow chowane za dolna krawedzia ekranu
			else if(FrmMainSlideEdge==3)
				SetWindowPos(hFrmMain,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmMain_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
			//Okno kontaktow chowane za gorna krawedzia ekranu
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
			UnicodeString KeyValue = IntToStr((int)FrmMainRect.Left);
			SaveSetup.Value = KeyValue.w_str();
			PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
			SaveSetup.Section = L"Position";
			SaveSetup.Ident = L"MainTop";
			KeyValue = IntToStr((int)FrmMainRect.Top);
			SaveSetup.Value = KeyValue.w_str();
			PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
			//Stan widocznosci okna kontaktow
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
				SetForegroundWindow(hFrmSend);
			}
			//Pobranie rozmiaru+pozycji okna rozmowy
			TRect WindowRect;
			GetWindowRect(hFrmSend,&WindowRect);
			//Ustawianie wstepnej pozycji okna rozmowy
			//Okno rozmowy chowane za lewa krawedzia ekranu
			if(FrmSendSlideEdge==1)
				SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
			//Okno rozmowy chowane za prawa krawedzia ekranu
			else if(FrmSendSlideEdge==2)
				SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-WindowRect.Width()-FrmSend_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
			//Okno rozmowy chowane za dolna krawedzia ekranu
			else if(FrmSendSlideEdge==3)
				SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
			//Okno rozmowy chowane za gorna krawedzia ekranu
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
			UnicodeString KeyValue = IntToStr((int)FrmSendRect.Left);
			SaveSetup.Value = KeyValue.w_str();
			PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
			SaveSetup.Section = L"Position";
			SaveSetup.Ident = L"MsgTop";
			KeyValue = IntToStr((int)FrmSendRect.Top);
			SaveSetup.Value = KeyValue.w_str();
			PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
			//Stan widocznosci okna rozmowy
			FrmSendVisible = true;
			//Wlaczenie funkcjanalnosci
			FrmSendSlideChk = true;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane widocznego opisu kontaktu na liscie kontatkow
INT_PTR __stdcall OnSetHTMLStatus(WPARAM wParam, LPARAM lParam)
{
	//Skracanie wyswietlania odnosnikow w oknie kontatkow do wygodniejszej formy / komunikator nie jest zamykany
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
			Body = TrimLinks(Body,true);
			//Zmienianie opisu na liscie kontatkow
			if(Body!=BodyOrg)
				return (LPARAM)Body.w_str();
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na polaczenie sieci przy starcie AQQ
INT_PTR __stdcall OnSetLastState(WPARAM wParam, LPARAM lParam)
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
					SetTimer(hTimerFrm,TIMER_UNBLOCK_INACTIVENOTIFER,20000,(TIMERPROC)TimerFrmProc);
				}
				//Przywracanie sesji z czatami
				if(RestoreTabsSessionChk)
				{
					//Odznaczenie uruchomienia przywracania sesji z czatami
					RestoringChatSession = true;
					//Tworzenie timera przywracania sesji z czatami
					SetTimer(hTimerFrm,TIMER_RESTORE_SESSION,10000,(TIMERPROC)TimerFrmProc);
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
INT_PTR __stdcall OnStateChange(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
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
					KillTimer(hTimerFrm,TIMER_UNBLOCK_INACTIVENOTIFER);
					SetTimer(hTimerFrm,TIMER_UNBLOCK_INACTIVENOTIFER,20000,(TIMERPROC)TimerFrmProc);
				}
				//Przywracanie sesji z czatami
				if((RestoreTabsSessionChk)||(!RestoringChatSession))
				{
					//Niby blokada, lol :D
					RestoringChatSession = true;
					//Tworzenie timera
					SetTimer(hTimerFrm,TIMER_RESTORE_SESSION,10000,(TIMERPROC)TimerFrmProc);
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

//Hook na pokazywanie popupmenu
INT_PTR __stdcall OnSystemPopUp(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Pobieranie danych popupmenu
		TPluginPopUp PopUp = *(PPluginPopUp)lParam;
		//Pobieranie nazwy popupmenu
		UnicodeString PopUpName = (wchar_t*)PopUp.Name;
		//Popupmenu dostepne spod PPM na zakladce w oknie rozmowy
		if(PopUpName=="popTab")
		{
			//Pobieranie danych kontaktku
			TPluginContact SystemPopUContact = *(PPluginContact)wParam;
			//Niedotyczy czatu z wtyczki
			if(!((SystemPopUContact.IsChat)&&(SystemPopUContact.FromPlugin)))
			{
				//Pobieranie JID kontaktu z zakladki
				UnicodeString JID = (wchar_t*)SystemPopUContact.JID;
				//Pobieranie zasobu kontaktu
				UnicodeString Res = (wchar_t*)SystemPopUContact.Resource;
				if(!Res.IsEmpty()) Res = "/" + Res;
				if(SystemPopUContact.IsChat)
				{
					JID = "ischat_" + JID;
					Res = "";
				}
				//Pobieranie indeksu konta kontaktu z zakladki
				UnicodeString UserIdx = ":" + IntToStr(SystemPopUContact.UserIdx);
				//Zapisanie JID do zmiennej globalnej
				PopupTab = JID+UserIdx;
				PopupTabEx = JID+Res+UserIdx;
				//Zakladka nie jest przypieta
				if(ClipTabsList->IndexOf(JID+UserIdx)==-1)
				{
					//Element przypinania zakladki
					TPluginActionEdit PluginActionEdit;
					ZeroMemory(&PluginActionEdit,sizeof(TPluginActionEdit));
					PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
					PluginActionEdit.pszName = L"TabKitClipTabItem";
					PluginActionEdit.Caption = GetLangStr("ClipTab").w_str();
					PluginActionEdit.Enabled = true;
					PluginActionEdit.Visible = true;
					PluginActionEdit.IconIndex = -1;
					PluginActionEdit.Checked = false;
					PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
					//Element ukrywania caption zakladki
					ZeroMemory(&PluginActionEdit,sizeof(TPluginActionEdit));
					PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
					PluginActionEdit.pszName = L"TabKitClipTabCaptionItem";
					PluginActionEdit.Caption = GetLangStr("HideCaption").w_str();
					PluginActionEdit.Enabled = true;
					PluginActionEdit.Visible = false;
					PluginActionEdit.IconIndex = -1;
					PluginActionEdit.Checked = true;
					PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
				}
				//Zakladka jest juz przypieta
				else
				{
					//Element przypinania zakladki
					TPluginActionEdit PluginActionEdit;
					ZeroMemory(&PluginActionEdit,sizeof(TPluginActionEdit));
					PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
					PluginActionEdit.pszName = L"TabKitClipTabItem";
					PluginActionEdit.Caption = GetLangStr("UnClipTab").w_str();
					PluginActionEdit.Enabled = true;
					PluginActionEdit.Visible = true;
					PluginActionEdit.IconIndex = -1;
					PluginActionEdit.Checked = false;
					PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
					//Element ukrywania nazwy zakladki
					ZeroMemory(&PluginActionEdit,sizeof(TPluginActionEdit));
					PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
					PluginActionEdit.pszName = L"TabKitClipTabCaptionItem";
					PluginActionEdit.Caption = GetLangStr("HideCaption").w_str();
					PluginActionEdit.Enabled = true;
					PluginActionEdit.Visible = true;
					PluginActionEdit.IconIndex = -1;
					TIniFile *Ini = new TIniFile(SessionFileDir);
					if(Ini->ValueExists("ClipTabsEx",JID+UserIdx))
						PluginActionEdit.Checked = false;
					else
						PluginActionEdit.Checked = true;
					delete Ini;
					PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
				}
				//Zakladka nie jest dodana do ulubionych
				if(FavouritesTabsList->IndexOf(JID+UserIdx)==-1)
				{
					//Element dodawania zakladki do ulubionych
					TPluginActionEdit PluginActionEdit;
					ZeroMemory(&PluginActionEdit,sizeof(TPluginActionEdit));
					PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
					PluginActionEdit.pszName = L"TabKitFrmSendFavouriteTabItem";
					PluginActionEdit.Caption = GetLangStr("AddToFav").w_str();
					PluginActionEdit.Enabled = true;
					PluginActionEdit.Visible = true;
					PluginActionEdit.IconIndex = 125;
					PluginActionEdit.Checked = false;
					PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
				}
				//Zaklada jest dodana do ulubionych
				else
				{
					//Element usuwania zakladki z ulubionych
					TPluginActionEdit PluginActionEdit;
					ZeroMemory(&PluginActionEdit,sizeof(TPluginActionEdit));
					PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
					PluginActionEdit.pszName = L"TabKitFrmSendFavouriteTabItem";
					PluginActionEdit.Caption = GetLangStr("RemoveFromFav").w_str();
					PluginActionEdit.Enabled = true;
					PluginActionEdit.Visible = true;
					PluginActionEdit.IconIndex = 125;
					PluginActionEdit.Checked = false;
					PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
				}
			}
		}
		//Popupmenu dostepne spod PPM na kontakcie w oknie kontaktow
		else if(PopUpName=="muItem")
		{
			//Pobieranie danych kontaktku
			TPluginContact SystemPopUContact = *(PPluginContact)wParam;
			//Niedotyczy czatu z wtyczki
			if(!((SystemPopUContact.IsChat)&&(SystemPopUContact.FromPlugin)))
			{
				//Pobieranie JID kontaktu z zakladki
				UnicodeString JID = (wchar_t*)SystemPopUContact.JID;
				//Pobieranie zasobu kontaktu
				UnicodeString Res = (wchar_t*)SystemPopUContact.Resource;
				if(!Res.IsEmpty()) Res = "/" + Res;
				if(SystemPopUContact.IsChat)
				{
					JID = "ischat_" + JID;
					Res = "";
				}
				//Pobieranie indeksu konta kontaktu z zakladki
				UnicodeString UserIdx = ":" + IntToStr(SystemPopUContact.UserIdx);
				//Zapisanie JID do zmiennej globalnej
				PopupTab = JID+UserIdx;
				PopupTabEx = JID+Res+UserIdx;
				//Zakladka nie jest dodana do ulubionych
				if(FavouritesTabsList->IndexOf(JID+UserIdx)==-1)
				{
					//Element dodawania zakladki do ulubionych
					TPluginActionEdit PluginActionEdit;
					ZeroMemory(&PluginActionEdit,sizeof(TPluginActionEdit));
					PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
					PluginActionEdit.pszName = L"TabKitFrmMainFavouriteTabItem";
					PluginActionEdit.Caption = GetLangStr("AddToFav").w_str();
					PluginActionEdit.Enabled = true;
					PluginActionEdit.Visible = true;
					PluginActionEdit.IconIndex = 125;
					PluginActionEdit.Checked = false;
					PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
				}
				//Zaklada jest dodana do ulubionych
				else
				{
					//Element usuwania zakladki z ulubionych
					TPluginActionEdit PluginActionEdit;
					ZeroMemory(&PluginActionEdit,sizeof(TPluginActionEdit));
					PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
					PluginActionEdit.pszName = L"TabKitFrmMainFavouriteTabItem";
					PluginActionEdit.Caption = GetLangStr("RemoveFromFav").w_str();
					PluginActionEdit.Enabled = true;
					PluginActionEdit.Visible = true;
					PluginActionEdit.IconIndex = 125;
					PluginActionEdit.Checked = false;
					PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
				}
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane tekstu na zakladce
INT_PTR __stdcall OnTabCaption(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Pobieranie danych kontaktku
		TPluginContact TabCaptionContact = *(PPluginContact)lParam;
		UnicodeString JID = (wchar_t*)TabCaptionContact.JID;
		UnicodeString Res = (wchar_t*)TabCaptionContact.Resource;
		if(!Res.IsEmpty()) Res = "/" + Res;
		if(TabCaptionContact.IsChat)
		{
			JID = "ischat_" + JID;
			Res = "";
		}
		UnicodeString UserIdx = ":" + IntToStr(TabCaptionContact.UserIdx);
		//Zmienna z tekstem zakladki
		UnicodeString TabCaption = (wchar_t*)wParam;
		//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
		TIniFile *Ini = new TIniFile(SessionFileDir);
		bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID+UserIdx);
		delete Ini;
		//Przypiete zakladki z ukryta nazwa
		if((ClipTabsChk)&&(ClipTabsList->IndexOf(JID+UserIdx)!=-1)&&(!ClipTabsEx))
		{
			//Przypiete zakladki bez licznika znakow
			if((!CounterClipTabsChk)||(TabCaptionContact.IsChat))
				TabCaption = "";
			//Przypiete zakladki z licznikiem znakow
			else if((CounterClipTabsChk)&&(!TabCaptionContact.IsChat))
			{
				UnicodeString Nick = (wchar_t*)TabCaptionContact.Nick;
				TabCaption = StringReplace(TabCaption, Nick + " ", "", TReplaceFlags());
				TabCaption = StringReplace(TabCaption, Nick, "", TReplaceFlags());
				TabCaption = TabCaption.Trim();
			}
		}
		//Licznik nieprzeczytanych wiadomosci
		if((InactiveTabsNewMsgChk)&&(!TabWasClosed))
		{
			int Count = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID+Res+UserIdx,0);
			if(Count)
			{
				//Licznik nie zostal jeszcze umieszczany na zakladce
				if(TabCaption.Pos("[" + IntToStr(Count) + "] ")!=1)
				{
					TabCaption = "[" + IntToStr(Count) + "] " + TabCaption;
					TabCaption = TabCaption.Trim();
				}
			}
		}
		//Nieprzypieta zakladka czatowa i normalizacja nazw
		if((ClipTabsList->IndexOf(JID+UserIdx)==-1)&&(TabCaptionContact.IsChat))
		{
			int Count = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID+Res+UserIdx,0);
			if(!Count)
			{
				//Normalizacja nazwy kanalu
				if(!TabCaptionContact.FromPlugin) TabCaption = NormalizeChannel(TabCaption);
				//Ustawianie prawidlowego identyfikatora
				JID.Delete(1,7);
				//Zapisywanie nazwy kanalu
				TIniFile *Ini = new TIniFile(SessionFileDir);
				Ini->WriteString("Channels",JID,EncodeBase64(TabCaption));
				delete Ini;
			}
			else
			{
				//Pobieranie nazwy kanalu
				UnicodeString OrgChannel = TabCaption;
				OrgChannel = StringReplace(OrgChannel, "[" + IntToStr(Count) + "] ", "", TReplaceFlags());
				//Normalizacja nazwy kanalu
				UnicodeString NormChannel = OrgChannel;
				if(!TabCaptionContact.FromPlugin) NormChannel = NormalizeChannel(NormChannel);
				//Ustawianie prawidlowego identyfikatora
				JID.Delete(1,7);
				//Zapisywanie nazwy kanalu
				TIniFile *Ini = new TIniFile(SessionFileDir);
				Ini->WriteString("Channels",JID,EncodeBase64(NormChannel));
				delete Ini;
				//Podmienianie nazwy kanalu
				TabCaption = StringReplace(TabCaption, OrgChannel, NormChannel, TReplaceFlags());
			}
		}
		//Zwrot zmienionego caption
		return (WPARAM)TabCaption.w_str();
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane ikonki na zakladce
INT_PTR __stdcall OnTabImage(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Notyfikcja pisania wiadomosci
		if(PreMsgList->Count>0)
		{
			//Pobieranie danych kontaktku
			TPluginContact TabImageContact = *(PPluginContact)lParam;
			//Zakladka ze zwyklym kontaktem
			if(!TabImageContact.IsChat)
			{
				//Pobieranie JID kontaktu
				UnicodeString JID = (wchar_t*)TabImageContact.JID;
				//Pobieranie zasobu kontaktu
				UnicodeString Res = (wchar_t*)TabImageContact.Resource;
				if(!Res.IsEmpty()) Res = "/" + Res;
				//Pobieranie indeksu konta
				UnicodeString UserIdx = ":" + IntToStr(TabImageContact.UserIdx);
				//Zakladka dotyczy notyfikacji pisania wiadomosci
				if(PreMsgList->IndexOf(JID+Res+UserIdx)!=-1)
				{
					//Pobranie indeksu aktualnej ikonki zakladki
					int TabImage = (int)wParam;
					//Ustawiona jest ikonka nieustawiona przez wtyczke
					if((TabImage!=COMPOSING)&&(TabImage!=PAUSE)&&(TabImage!=GONE)&&(TabImage!=8))
					{
						//Pobranie stanu notyfikacji wiadomosci
						int ChatState = PreMsgStateList->ReadInteger("PreMsgState",JID+Res+UserIdx,0);
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
							PreMsgList->Delete(PreMsgList->IndexOf(JID+Res+UserIdx));
					}
				}
			}
		}
		//Przypiete zakladki
		if((ClipTabsChk)&&(ClipTabsList->Count>0))
		{
			//Pobieranie danych kontaktku
			TPluginContact TabImageContact = *(PPluginContact)lParam;
			//Nie jest to zakladka czatowa
			if(!TabImageContact.IsChat)
			{
				//Pobieranie JID kontaktu
				UnicodeString JID = (wchar_t*)TabImageContact.JID;
				//Pobieranie indeksu konta kontaktu
				UnicodeString UserIdx = ":" + IntToStr(TabImageContact.UserIdx);
				//Zakladka jest przypieta
				if(ClipTabsList->IndexOf(JID+UserIdx)!=-1)
				{
					//Pobranie indeksu aktualnej ikonki zakladki
					int TabImage = (int)wParam;
					//Ustawiona jest ikonka nieustawiona przez wtyczke
					if((TabImage!=COMPOSING)&&(TabImage!=PAUSE)&&(TabImage!=GONE)&&(TabImage!=8))
					{
						//Sprawdzanie stanu pokazywania nazwy przypietej zakladki
						TIniFile *Ini = new TIniFile(SessionFileDir);
						bool ClipTabsEx = Ini->ValueExists("ClipTabsEx",JID+UserIdx);
						delete Ini;
						//Zakladka z botem Blabler
						/*if((JID=="blabler.k2t.eu")||(JID.Pos("48263287@plugin.gg")==1))
						{
							return BLABLER;
						}
						//Zakladka z botem tweet.IM
						else*/ if(JID.Pos("@twitter.tweet.im"))
						{
							return 131;
						}
						//Zakladka ze zwyklym kontaktem
						else if((MiniAvatarsClipTabsChk)&&(!ClipTabsEx))
						{
							//Pobieranie indeksu ikonki z pamieci
							int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID+UserIdx,0);
							//Ikona juz w interfejsie
							if(Icon) return Icon;
							//Ikona jeszcze niezaladowana do interfejsu
							else
							{
								//Plik PNG jest juz wygenerowany
								if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
								{
									//Zaladowanie ikonki do interfejsu
									Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
									//Zapisanie indeksu ikonki do pamieci
									ClipTabsIconList->WriteInteger("ClipTabsIcon",JID+UserIdx,Icon);
									//Ustawienie ikonki na zakladce
									return Icon;
								}
								//Generowanie pliku PNG 16x16 z awataru kontaktu
								else
								{
									//Odczyt pliku INI z danymi kontaktu
									TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
									//Dekodowanie sciezki awatara
									UnicodeString Avatar = DecodeBase64(Ini->ReadString("Other","Avatar",""));
									delete Ini;
									//Sciezka awatata zostala prawidlowo pobrana
									if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
									{
										//Zamienianie sciezki relatywnej na absolutna
										if(Avatar.Pos("{PROFILEPATH}"))
											Avatar = StringReplace(Avatar, "{PROFILEPATH}", GetUserDir(), TReplaceFlags());
										else
											Avatar = StringReplace(Avatar, "{APPPATH}", GetApplicationPath(), TReplaceFlags());
										//Przypisanie uchwytu do formy
										Application->Handle = (HWND)SettingsForm;
										TSettingsForm *hModalSettingsForm = new TSettingsForm(Application);
										//Konwersja awatara do ikonki PNG 16x16
										hModalSettingsForm->ConvertImage(Avatar,PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png");
										//Usuniecie uchwytu do formy
										delete hModalSettingsForm;
										//Konwersja przeszla prawidlowo
										if(FileExists(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
										{
											//Zaladowanie ikonki do interfejsu
											Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
											//Zapisanie indeksu ikonki do pamieci
											ClipTabsIconList->WriteInteger("ClipTabsIcon",JID+UserIdx,Icon);
											//Ustawienie ikonki na zakladce
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

//Hook na zmiane kompozycji
INT_PTR __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam)
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
				hSettingsForm->sSkinManager->Brightness = GetBrightness();
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
			hSettingsForm->XMPPWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
			hSettingsForm->XMPPWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
			hSettingsForm->URLWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
			hSettingsForm->URLWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
			hSettingsForm->ForumWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
			hSettingsForm->ForumWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
			hSettingsForm->BugWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
			hSettingsForm->BugWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
			hSettingsForm->StarWebLabel->Font->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();
			hSettingsForm->StarWebLabel->HoverFont->Color = hSettingsForm->sSkinManager->GetGlobalFontColor();;
		}
		//Nieaktywne skorkowanie AlphaControls
		else
		{
			//Kolor WebLabel'ow
			hSettingsForm->EmailWebLabel->Font->Color = clWindowText;
			hSettingsForm->EmailWebLabel->HoverFont->Color = clWindowText;
			hSettingsForm->XMPPWebLabel->Font->Color = clWindowText;
			hSettingsForm->XMPPWebLabel->HoverFont->Color = clWindowText;
			hSettingsForm->URLWebLabel->Font->Color = clWindowText;
			hSettingsForm->URLWebLabel->HoverFont->Color = clWindowText;
			hSettingsForm->ForumWebLabel->Font->Color = clWindowText;
			hSettingsForm->ForumWebLabel->HoverFont->Color = clWindowText;
			hSettingsForm->BugWebLabel->Font->Color = clWindowText;
			hSettingsForm->BugWebLabel->HoverFont->Color = clWindowText;
			hSettingsForm->StarWebLabel->Font->Color = clWindowText;
			hSettingsForm->StarWebLabel->HoverFont->Color = clWindowText;
		}
		//Aktualizacja ikonek na formie
		hSettingsForm->FavouritesTabsAlphaImageList->Clear();
		hSettingsForm->FavouritesTabsAlphaImageList->LoadFromFile(GetIconPath(98));
		hSettingsForm->FavouritesTabsAlphaImageList->LoadFromFile(GetIconPath(99));
		hSettingsForm->FavouritesTabsAlphaImageList->LoadFromFile(GetIconPath(15));
		hSettingsForm->FavouritesTabsAlphaImageList->LoadFromFile(GetIconPath(79));
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
			SetForegroundWindow(hFrmSend);
		}
		//Pobranie rozmiaru+pozycji okna rozmowy
		TRect WindowRect;
		GetWindowRect(hFrmSend,&WindowRect);
		//Ustawianie wstepnej pozycji okna rozmowy
		//Okno rozmowy chowane za lewa krawedzia ekranu
		if(FrmSendSlideEdge==1)
			SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
		//Okno rozmowy chowane za prawa krawedzia ekranu
		else if(FrmSendSlideEdge==2)
			SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-WindowRect.Width()-FrmSend_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
		//Okno rozmowy chowane za dolna krawedzia ekranu
		else if(FrmSendSlideEdge==3)
			SetWindowPos(hFrmSend,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
		//Okno rozmowy chowane za gorna krawedzia ekranu
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
		UnicodeString KeyValue = IntToStr((int)FrmSendRect.Left);
		SaveSetup.Value = KeyValue.w_str();
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		SaveSetup.Section = L"Position";
		SaveSetup.Ident = L"MsgTop";
		KeyValue = IntToStr((int)FrmSendRect.Top);
		SaveSetup.Value = KeyValue.w_str();
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		//Status pre-wysuwania okna rozmowy zza krawedzi ekranu
		PreFrmSendSlideIn = false;
		//Status wysuwania okna rozmowy zza krawedzi ekranu
		FrmSendSlideIn = false;
		//Status pre-chowania okna rozmowy za krawedz ekranu
		PreFrmSendSlideOut = false;
		//Status chowania okna rozmowy za krawedz ekranu
		FrmSendSlideOut = false;
		//Stan widocznosci okna rozmowy
		FrmSendVisible = true;
		//Tymczasowa blokada chowania/pokazywania okna rozmowy
		if(StayOnTopChk) FrmSendBlockSlide = StayOnTopStatus;
		else FrmSendBlockSlide = false;
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
			SetForegroundWindow(hFrmMain);
		}
		//Pobranie rozmiaru+pozycji okna kontatkow
		TRect WindowRect;
		GetWindowRect(hFrmMain,&WindowRect);
		//Ustawianie wstepnej pozycji okna rozmowy
		//Okno kontaktow chowane za lewa krawedzia ekranu
		if(FrmMainSlideEdge==1)
			SetWindowPos(hFrmMain,HWND_TOP,0+FrmMain_Shell_TrayWndLeft,WindowRect.Top,0,0,SWP_NOSIZE);
		//Okno kontaktow chowane za prawa krawedzia ekranu
		else if(FrmMainSlideEdge==2)
			SetWindowPos(hFrmMain,HWND_TOP,Screen->Width-WindowRect.Width()-FrmMain_Shell_TrayWndRight,WindowRect.Top,0,0,SWP_NOSIZE);
		//Okno kontaktow chowane za dolna krawedzia ekranu
		else if(FrmMainSlideEdge==3)
			SetWindowPos(hFrmMain,HWND_TOP,WindowRect.Left,Screen->Height-WindowRect.Height()-FrmMain_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
		//Okno kontaktow chowane za gorna krawedzia ekranu
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
		UnicodeString KeyValue = IntToStr((int)FrmMainRect.Left);
		SaveSetup.Value = KeyValue.w_str();
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		SaveSetup.Section = L"Position";
		SaveSetup.Ident = L"MainTop";
		KeyValue = IntToStr((int)FrmMainRect.Top);
		SaveSetup.Value = KeyValue.w_str();
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		//Status pre-wysuwania okna kontaktow zza krawedzi ekranu
		PreFrmMainSlideIn = false;
		//Status wysuwania okna kontaktow zza krawedzi ekranu
		FrmMainSlideIn = false;
		//Status pre-chowania okna kontaktow za krawedz ekranu
		PreFrmMainSlideOut = false;
		//Status chowania okna kontaktow za krawedz ekranu
		FrmMainSlideOut = false;
		//Stan widocznosci okna kontaktow
		FrmMainVisible = true;
		//Tymczasowa blokada chowania/pokazywania okna kontaktow
		FrmMainBlockSlide = false;
		//Wlaczenie funkcjanalnosci
		FrmMainSlideChk = true;
	}
	//Pobranie rozmiaru+pozycji okna kontatkow
	else
		GetFrmMainRect();
	//Pobranie stylu zalacznika
	GetAttachmentStyle();
	//Usuniecie info o zmianie kompozycji
	ThemeChanging = false;

	return 0;
}
//---------------------------------------------------------------------------

//Hook na klikniecie LPM w ikonke tray
INT_PTR __stdcall OnTrayClick(WPARAM wParam, LPARAM lParam)
{
	//Funkcjonalnosc chowania okna kontaktow jest wlaczona / komunikator nie jest zamykany
	if((FrmMainSlideChk)&&(!ForceUnloadExecuted))
	{
		//Fizyczne kliniecie LPM w tray
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
INT_PTR __stdcall OnWindowEvent(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
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
				//Aktywacja okna kontaktow + nadanie fokusa kontrolce IE
				ActivateAndFocusFrmMain();
				//Zapisanie pozycji okna kontaktow do ustawiem AQQ
				TSaveSetup SaveSetup;
				SaveSetup.Section = L"Position";
				SaveSetup.Ident = L"MainLeft";
				UnicodeString KeyValue = IntToStr((int)FrmMainRect.Left);
				SaveSetup.Value = KeyValue.w_str();
				PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
				SaveSetup.Section = L"Position";
				SaveSetup.Ident = L"MainTop";
				KeyValue = IntToStr((int)FrmMainRect.Top);
				SaveSetup.Value = KeyValue.w_str();
				PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
				//Odswiezenie ustawien
				PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
			}
			//Stan widocznosci okna kontaktow
			FrmMainVisible = true;
			//Tymczasowa blokada chowania/pokazywania okna kontaktow
			FrmMainBlockSlide = false;
			//Przypisanie nowej procki dla okna kontatkow
			if(!OldFrmMainProc) OldFrmMainProc = (WNDPROC)SetWindowLongPtrW(hFrmMain, GWLP_WNDPROC,(LONG_PTR)FrmMainProc);
			//Szybki dostep do ulubionych zakladek
			BuildFavouritesTabs(false);
			//Szybki dostep niewyslanych wiadomosci
			GetUnsentMsg();
			BuildFrmUnsentMsg(false);
			//Szybki dostep do ostatnio zamknietych zakladek
			GetClosedTabs();
			BuildFrmClosedTabs(false);
			//Tworzenie interfesju w AQQ dla ostatnio zamknietych zakladek
			BuildAQQClosedTabs();
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
						//Otwieranie zakladki z kontaktem
						if(!JID.IsEmpty())
						{
							//Otwieranie zakladki ze zwyklym kontaktem
							if(!JID.Pos("ischat_")) OpenNewTab(JID);
							//"Otwieranie" zakladki z czatem
							else ChatSessionList->Add(JID);
						}
					}
					//Zmiana aktywnej zakladki na pierwsza
					if(TabsListEx->Count) ChangeActiveTab(Ini->ReadString("SessionEx","ActiveTab",TabsList->Strings[0]));
					//Kasowanie uchwytu do ostatnio aktywnego okna - anty never endig SlideIn FrmMain
					LastActiveWindow = NULL;
					//Wczytywanie ostatnio przeprowadzonych rozmow w przywroconych zakladkach
					for(int Count=0;Count<TabsList->Count;Count++)
					{
						//Kontakt nie jest czatem
						if(!TabsList->Strings[Count].Pos("ischat_"))
						{
							//Pobranie JID kontaktu
							UnicodeString JID = TabsList->Strings[Count];
							//Definicja domyslnego indeksu konta
							UnicodeString UserIdx = "0";
							//JID zawiera indeks konta
							if(JID.Pos(":"))
							{
								//Wyciagniecie indeksu konta
								UserIdx = JID;
								UserIdx.Delete(1,UserIdx.Pos(":"));
								//Usuniecie indeksu konta z JID
								JID.Delete(JID.Pos(":"),JID.Length());
							}
							//Wczytywanie ostatnio przeprowadzonej rozmowy
							PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)StrToInt(UserIdx));
						}
					}
					//Status przywracania sesji
					RestoringSession = false;
				}
				delete Session;
				delete Ini;
			}
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
			//Zezwolenie na odswiezenie listy kontaktow
			AllowRefreshList = true;
		}
		//Zamkniecie okna kontatkow
		if((ClassName=="TfrmMain")&&(Event==WINDOW_EVENT_CLOSE))
		{
			//Przypisanie starej procki do okna kontatkow ma miejsce w WM_CLOSE
			//Usuniecie uchwytu do okna kontaktow
			hFrmMain = NULL;
		}

		//Otwarcie okna rozmowy
		if((ClassName=="TfrmSend")&&(Event==WINDOW_EVENT_CREATE))
		{
			if(!hFrmSend)
			{
				//Przypisanie uchwytu do okna rozmowy
				hFrmSend = (HWND)WindowEvent.Handle;
				//Szukanie pola wiadomosci
				if(!hRichEdit) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
				//Szukanie paska informacyjnego
				if(!hStatusBar) SetTimer(hTimerFrm,TIMER_FIND_STATUSBAR,500,(TIMERPROC)TimerFrmProc);
				//Szuknie paska narzedzi
				if(!hToolBar) SetTimer(hTimerFrm,TIMER_FIND_TOOLBAR,500,(TIMERPROC)TimerFrmProc);
				//Wlaczenie timera szukania paska zakladek + innych pod kontrolek
				if((!hTabsBar)||(!hScrollTabButton[0])||(!hScrollTabButton[1]))
					SetTimer(hTimerFrm,TIMER_FIND_TABSBAR,500,(TIMERPROC)TimerFrmProc);
				//Pobranie rozmiaru+pozycji okna rozmowy
				GetFrmSendRect();
				//Ustawienie poprawnej pozycji okna
				if(FrmSendSlideChk)
				{
					//Tymczasowa blokada chowania/pokazywania okna rozmowy
					FrmSendBlockSlide = true;
					//Wlaczenie timera
					SetTimer(hTimerFrm,TIMER_FRMSEND_CHANGEPOS,2000,(TIMERPROC)TimerFrmProc);
				}
				//Stan widocznosci okna rozmowy
				else FrmSendVisible = true;
				//Przypisanie nowej procki dla okna rozmowy
				if(!OldFrmSendProc)	OldFrmSendProc = (WNDPROC)SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG_PTR)FrmSendProc);
				//Szybki dostep do ulubionych zakladek
				DestroyFavouritesTabs();
				BuildFavouritesTabs(false);
				//Szybki dostep niewyslanych wiadomosci
				DestroyFrmUnsentMsg();
				BuildFrmUnsentMsg(false);
				//Szybki dostep do ostatnio zamknietych zakladek
				DestroyFrmClosedTabs();
				BuildFrmClosedTabs(false);
				//Tworzenie interfejsu trzymania okna rozmowy na wierzchu
				BuildStayOnTop();
				//Tworzenie elementu przypinania/odpiniania zakladek oraz pokazywania/ukrywania caption zakladki
				BuildClipTab();
				//Tworzenie elementu dodawania/usuwania ulubionej zakladki
				BuildFrmSendFavouriteTab();
				//Resetowanie poprzedniego stanu pisania wiadomosci
				LastChatState = 0;
				//Usuniecie uchwytow do ikonek okna rozmowy
				hIconSmall = 0;
				hIconBig = 0;
				//Otwieranie przypietych zakladek
				if((ClipTabsChk)&&(OpenClipTabsChk)&&(!RestoringSession))
				{
					if(ClipTabsList->Count) SetTimer(hTimerFrm,TIMER_CLIPTABS_OPEN,2000,(TIMERPROC)TimerFrmProc);
				}
			}
		}
		//Zamkniecie okna rozmowy
		if((ClassName=="TfrmSend")&&(Event==WINDOW_EVENT_CLOSE))
		{
			//Przypisanie starej procki do okna rozmowy ma miejsce w WM_CLOSE
			//Status pre-wysuwania okna rozmowy zza krawedzi ekranu
			PreFrmSendSlideIn = false;
			//Status wysuwania okna rozmowy zza krawedzi ekranu
			FrmSendSlideIn = false;
			//Status pre-chowania okna rozmowy za krawedz ekranu
			PreFrmSendSlideOut = false;
			//Status chowania okna rozmowy za krawedz ekranu
			FrmSendSlideOut = false;
			//Stan widocznosci okna rozmowy
			FrmSendVisible = false;
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			FrmSendBlockSlide = true;
			//Resetowanie zmiennej aktwnie otwartej zakladki
			ActiveTabJID = "";
			ActiveTabJIDEx = "";
			//Szybki dostep niewyslanych wiadomosci
			DestroyFrmUnsentMsg();
			BuildFrmUnsentMsg(false);
			//Szybki dostep do ostatnio zamknietych zakladek
			DestroyFrmClosedTabs();
			BuildFrmClosedTabs(false);
			//Usuwanie interfejsu trzymania okna rozmowy na wierzchu
			DestroyStayOnTop();
			//Usuwanie elementu przypinania zakladek
			DestroyClipTab();
			//Usuwanie elementu dodawania/usuwania ulubionej zakladki
			DestroyFrmSendFavouriteTab();
			//Szybki dostep do ulubionych zakladek
			DestroyFavouritesTabs();
			BuildFavouritesTabs(false);
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
				MessageBox(Application->Handle,	GetLangStr("TabsWarning").w_str(), GetLangStr("TabsWarningTitle").w_str(), MB_OK | MB_ICONINFORMATION);
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
			//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
			KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE);
			//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna kontaktow
			KillTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE);
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			FrmSendBlockSlide = true;
			//Tymczasowa blokada chowania/pokazywania okna kontaktow
			FrmMainBlockSlide = true;
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
			//Tymczasowa blokada chowania/pokazywania okna kontaktow
			if((FrmSendSlideChk)&&(!StayOnTopStatus)) FrmSendBlockSlide = false;
			FrmMainBlockSlide = false;
			//Trzymanie okna na wierzchu
			if(((StayOnTopChk)&&(StayOnTopStatus))||((FrmSendSlideChk)&&(FrmSendSlideHideMode!=2)))
				SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			//Tworzenie timera aktywacji pola wpisywania tekstu
			if((!FrmSetStateExist)&&(hRichEdit))
				SetTimer(hTimerFrm,TIMER_FRMSEND_FOCUS_RICHEDIT,500,(TIMERPROC)TimerFrmProc);
		}

		//Otworzenie okna szybkich emotek
		if((ClassName=="TfrmCompletion")&&(Event==WINDOW_EVENT_CREATE))
		{
			//Stan widocznosci okna szybkich emotek
			FrmCompletionExists = true;
			//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
			KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE);
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			FrmSendBlockSlide = true;
			//Trzymanie okna na wierzchu
			if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk))
				SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}
		//Zamkniecie okna szybkich emotek
		if((ClassName=="TfrmCompletion")&&(Event==WINDOW_EVENT_CLOSE))
		{
			//Stan widocznosci okna szybkich emotek
			FrmCompletionExists = false;
			//Trzymanie okna na wierzchu
			if(((StayOnTopChk)&&(StayOnTopStatus))||((FrmSendSlideChk)&&(FrmSendSlideHideMode!=2)))
				SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			//Gdy kursor znajduje sie poza oknem rozmowy
			if((Mouse->CursorPos.y<FrmSendRect.Top)||(FrmSendRect.Bottom<Mouse->CursorPos.y)||(Mouse->CursorPos.x<FrmSendRect.Left)||(FrmSendRect.Right<Mouse->CursorPos.x))
			{
				//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
				SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,2000,(TIMERPROC)TimerFrmProc);
			}
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			else FrmSendBlockSlide = false;
		}

		//Otworzenie okna tworzenia wycinka
		if((ClassName=="TfrmPos")&&(Event==WINDOW_EVENT_CREATE))
		{
			//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
			KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE);
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			FrmSendBlockSlide = true;
			//Informacja o otwarciu okna tworzenia wycinka
			FrmPosExist = true;
			//Trzymanie okna na wierzchu
			if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk)||(FrmMainSlideChk))
			{
				//Normalizacja okna rozmowy/kontaktow
				if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk)) SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
				if((FrmMainSlideChk)&&(FrmMainVisible)) SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			}
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
				//Tymczasowa blokada chowania/pokazywania okna rozmowy
				FrmSendBlockSlide = true;
				//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
				SetTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE,2000,(TIMERPROC)TimerFrmProc);
			}
			//Ustawienie statusu okna dla SideSlide
			else
			{
				//Tymczasowa blokada chowania/pokazywania okna rozmowy
				if((FrmSendSlideChk)&&(!StayOnTopStatus)) FrmSendBlockSlide = false;
			}
		}

		//Otworzenie okna Centrum Akcji
		if((ClassName=="TfrmFindAction")&&(Event==WINDOW_EVENT_CREATE))
		{
			//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
			KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE);
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			FrmSendBlockSlide = true;
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
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			if((FrmSendSlideChk)&&(!StayOnTopStatus)) FrmSendBlockSlide = false;
			//Trzymanie okna na wierzchu
			if(((StayOnTopChk)&&(StayOnTopStatus))||((FrmSendSlideChk)&&(FrmSendSlideHideMode!=2)))
				SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}

		//Otworzenie okna wysylania obrazka metoda Drag&Drop
		if((ClassName=="TfrmDeliveryType")&&(Event==WINDOW_EVENT_CREATE))
		{
			//Zatrzymanie timera wylaczenia tymczasowej blokady chowania/pokazywania okna rozmowy
			KillTimer(hTimerFrm,TIMER_FRMSEND_UNBLOCK_SLIDE);
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			FrmSendBlockSlide = true;
			//Trzymanie okna na wierzchu
			if(((StayOnTopChk)&&(StayOnTopStatus))||(FrmSendSlideChk))
				SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}
		//Zamkniecie okna wysylania obrazka metoda Drag&Drop
		if((ClassName=="TfrmDeliveryType")&&(Event==WINDOW_EVENT_CLOSE))
		{
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			if((FrmSendSlideChk)&&(!StayOnTopStatus)) FrmSendBlockSlide = false;
			//Trzymanie okna na wierzchu
			if(((StayOnTopChk)&&(StayOnTopStatus))||((FrmSendSlideChk)&&(FrmSendSlideHideMode!=2)))
				SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		}

		//Otworzenie okna zmiany opisu
		if((ClassName=="TfrmSetState")&&(Event==WINDOW_EVENT_CREATE))
		{
			//Zatrzymanie timera wylaczanie tymczasowej blokady
			KillTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE);
			//Informacja o otwarciu okna zmiany opisu
			FrmSetStateExist = true;
			//Tymczasowa blokada chowania/pokazywania okna kontaktow
			FrmMainBlockSlide = true;
		}
		//Zamkniecie okna zmiany opisu
		if((ClassName=="TfrmSetState")&&(Event==WINDOW_EVENT_CLOSE))
		{
			//Informacja o zamknieciu okna zmiany opisu
			FrmSetStateExist = false;
			//Wlaczenie timera wylaczenia tymczasowej blokady chowania/pokazywania okna kontaktow
			SetTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE,1500,(TIMERPROC)TimerFrmProc);
			//Wlaczenie timera ustawienia okna na wierzchu
			SetTimer(hTimerFrm,TIMER_FRMMAIN_TOPMOST,10,(TIMERPROC)TimerFrmProc);
		}

		//Otworzenie okna wyszukiwarki kontaktow
		if((ClassName=="TfrmSeekOnList")&&(Event==WINDOW_EVENT_CREATE))
		{
			//Zatrzymanie timera wylaczanie tymczasowej blokady
			KillTimer(hTimerFrm,TIMER_FRMMAIN_UNBLOCK_SLIDE);
			//Tymczasowa blokada chowania/pokazywania okna kontaktow
			FrmMainBlockSlide = true;
			//Zabezpieczenie przed chowaniem okna kontaktow
			if(FrmMainSlideChk)
			{
				//Pobieranie uchwytu do okna wyszukiwarki
				hFrmSeekOnList = (HWND)(int)WindowEvent.Handle;
				//Ustawienie okna kontaktow na wierzchu
				SetWindowPos(hFrmMain,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				//Ustawienie okna wyszukiwarki na wierzchu
				SetWindowPos(hFrmSeekOnList,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
				//Przypisanie nowej procki dla okna wyszukiwarki
				if(!OldFrmSeekOnListProc) OldFrmSeekOnListProc = (WNDPROC)SetWindowLongPtrW(hFrmSeekOnList, GWLP_WNDPROC,(LONG_PTR)FrmSeekOnListProc);
			}
		}
		//Zamkniecie okna wyszukiwarki kontaktow
		if((ClassName=="TfrmSeekOnList")&&(Event==WINDOW_EVENT_CLOSE))
		{
			//Tymczasowa blokada chowania/pokazywania okna kontaktow
			FrmMainBlockSlide = false;
			//Zabezpieczenie przed chowaniem okna kontaktow
			if(FrmMainSlideChk)
			{
				//Przypisanie starej procki do okna wyszukiwarki ma miejsce w WM_CLOSE
				//Zatrzymanie timera
				KillTimer(hTimerFrm,TIMER_FRMMAIN_TOPMOST_EX);
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
			SetTimer(hTimerFrm,TIMER_DISABLE_MODAL,500,(TIMERPROC)TimerFrmProc);
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

		//Otwarcie okna dolaczania do konferencji
		if((ClassName=="TfrmChatJoin")&&(Event==WINDOW_EVENT_CREATE))
		{
			//Informacja o otwarciu okna dolaczania do konferencji
			FrmChatJoinExist = true;
		}
		//Zamkniecie okna dolaczania do konferencji
		if((ClassName=="TfrmChatJoin")&&(Event==WINDOW_EVENT_CLOSE))
		{
			//Informacja o zamknieciu okna dolaczania do konferencji
			FrmChatJoinExist = false;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na odbieranie pakietow XML zawierajace ID
INT_PTR __stdcall OnXMLIDDebug(WPARAM wParam, LPARAM lParam)
{
	//Komunikator nie jest zamykany
	if(!ForceUnloadExecuted)
	{
		//Pobranie danych o pakiecie XML
		TPluginXMLChunk XMLChunk = *(PPluginXMLChunk)lParam;
		UnicodeString ID = (wchar_t*)XMLChunk.ID;
		//Sprawdzenie czy wskazany ID byl przez nas uzywany
		if(XMLIDList->IndexOf(ID)!=-1)
		{
			//Usuniecie ID z listy
			XMLIDList->Delete(XMLIDList->IndexOf(ID));
			//Pobranie pakietu XML
			UnicodeString XML = (wchar_t*)XMLChunk.XML;
			//Pobieranie identyfikatora nadawcy
			UnicodeString From = (wchar_t*)XMLChunk.From;
			//Pobranie indeksu konta
			int UserIdx = XMLChunk.UserIdx;
			//Kodowanie pakietu XML
			XML = UTF8ToUnicodeString(XML.w_str());
			//Wczytanie pakietu XML do parsera
			_di_IXMLDocument XMLDoc = LoadXMLData(XML);
			_di_IXMLNode Nodes = XMLDoc->DocumentElement;
			//Pobranie pierwszego dziecka
			Nodes = Nodes->ChildNodes->GetNode(0);
			//Pobranie ilosci kolejnych dzieci
			int ItemsCount = Nodes->ChildNodes->GetCount();
			//Parsowanie kolejnych elementow
			for(int Count=0;Count<ItemsCount;Count++)
			{
				//Pobieranie zawartosci dziecka
				_di_IXMLNode ChildNodes = Nodes->ChildNodes->GetNode(Count);
				//Sprawdzanie nazwy elementu
				if(ChildNodes->NodeName=="identity")
				{
					//Pobranie nazwy kanalu
					UnicodeString Channel = ChildNodes->Attributes["name"];
					//Normalizacja nazwy kanalu
					Channel = NormalizeChannel(Channel);
					//Otwarcie pliku sesji
					TIniFile *Ini = new TIniFile(SessionFileDir);
					//Pobranie poprzedniej nazwy kanalu
					UnicodeString PrevChannel = DecodeBase64(Ini->ReadString("Channels",From,""));
					//Zapisywanie nazwy kanalu
					Ini->WriteString("Channels",From,EncodeBase64(Channel));
					//Zamkniecie pliku sesji
					delete Ini;
					//Odswiezenie nazwy pokoju
					if(TabsListEx->IndexOf("ischat_"+From+":"+IntToStr(UserIdx))!=-1) OpenNewTab("ischat_"+From+":"+IntToStr(UserIdx));
				}
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Odswiezenie wszystkich zakladek
void RefreshTabs()
{
	//Hook na pobieranie aktywnych zakladek
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs_RefreshTabs);
	//Pobieranie aktywnych zakladek
	PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
	//Usuniecie hooka na pobieranie aktywnych zakladek
	PluginLink.UnhookEvent(OnFetchAllTabs_RefreshTabs);
}
//---------------------------------------------------------------------------

//Pobieranie zdefiniowanego skrotu globalnego do minimalizowania/przywracania okna rozmowy
int GetMinimizeRestoreFrmSendKey()
{
	return MinimizeRestoreKey;
}
//---------------------------------------------------------------------------

//Zapisywanie zasobow
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

//Obliczanie MD5 ciagu znakow
UnicodeString MD5(UnicodeString Text)
{
	TIdHashMessageDigest5* idmd5 = new TIdHashMessageDigest5();
	UnicodeString Result = idmd5->HashStringAsHex(Text.LowerCase()).LowerCase();
	delete idmd5;
	return Result;
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
	else return 0;
}
//---------------------------------------------------------------------------

//Odswiezanie wyjatkow aplikacji pelnoekranowych wylaczonych z wysuwania okna rozmowy/kontaktow
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
	FastAccessClosedTabsChk =	Ini->ReadBool("ClosedTabs","FastAccess",true);
	FrmMainClosedTabsChk =	Ini->ReadBool("ClosedTabs","FrmMain",true);
	FrmSendClosedTabsChk =	Ini->ReadBool("ClosedTabs","FrmSend",true);
	ItemCountUnCloseTabVal = Ini->ReadInteger("ClosedTabs","ItemsCount",5);
	ShowTimeClosedTabsChk = Ini->ReadBool("ClosedTabs","ClosedTime",false);
	DateFormatOnClosedTabs = Ini->ReadString("ClosedTabs", "DateFormat", "dddd, h:nn");
	FastClearClosedTabsChk = Ini->ReadBool("ClosedTabs","FastClear",true);
	UnCloseTabHotKeyChk =	Ini->ReadBool("ClosedTabs","HotKey",false);
	UnCloseTabHotKeyMode = Ini->ReadInteger("ClosedTabs","HotKeyMode",1);
	UnCloseTabHotKeyDef = Ini->ReadInteger("ClosedTabs","HotKeyDef",0);
	UnCloseTabSPMouseChk = Ini->ReadBool("ClosedTabs","SPMouse",true);
	UnCloseTabLPMouseChk = Ini->ReadBool("ClosedTabs","LPMouse",false);
	UnCloseTab2xLPMouseChk = Ini->ReadBool("ClosedTabs","2xLPMouse",false);
	CountUnCloseTabVal = Ini->ReadInteger("ClosedTabs","Count",10);
	RestoreLastMsgChk = Ini->ReadBool("ClosedTabs","RestoreLastMsg",true);
	OnlyConversationTabsChk = Ini->ReadBool("ClosedTabs","OnlyConversationTabs",false);
	//UnsentMsg
	UnsentMsgChk = Ini->ReadBool("UnsentMsg","Enable",true);
	InfoUnsentMsgChk = Ini->ReadBool("UnsentMsg","Info",true);
	CloudUnsentMsgChk = Ini->ReadBool("UnsentMsg","Cloud",true);
	DetailedCloudUnsentMsgChk = Ini->ReadBool("UnsentMsg","DetailedCloud",false);
	TrayUnsentMsgChk =	Ini->ReadBool("UnsentMsg","Tray",true);
	FastAccessUnsentMsgChk =	Ini->ReadBool("UnsentMsg","FastAccess",true);
	FrmMainUnsentMsgChk =	Ini->ReadBool("UnsentMsg","FrmMain",true);
	FrmSendUnsentMsgChk =	Ini->ReadBool("UnsentMsg","FrmSend",true);
	FastClearUnsentMsgChk = Ini->ReadBool("UnsentMsg","FastClear",true);
	//TabsSwitching
	SwitchToNewMsgChk =	Ini->ReadBool("TabsSwitching","SwitchToNewMsg",true);
	SwitchToNewMsgMode = Ini->ReadInteger("TabsSwitching","SwitchToNewMsgMode",1);
	TabsHotKeysChk = Ini->ReadBool("TabsSwitching","TabsHotKeys",true);
	TabsHotKeysMode = Ini->ReadInteger("TabsSwitching","TabsHotKeysMode",2);
	NewMgsHoyKeyChk = Ini->ReadBool("TabsSwitching","NewMgsHoyKey",true);
	//SessionRemember
	RestoreTabsSessionChk = Ini->ReadBool("SessionRemember","RestoreTabs",true);
	ManualRestoreTabsSessionChk = Ini->ReadBool("SessionRemember","ManualRestoreTabs",true);
	RestoreMsgSessionChk = Ini->ReadBool("SessionRemember","RestoreMsg",false);
	//NewMsg
	InactiveFrmNewMsgChk = Ini->ReadBool("NewMsg","InactiveFrm",true);
	KeyboardFlasherChk = Ini->ReadBool("NewMsg","KeyboardFlasher",false);
	KeyboardFlasherModeChk = Ini->ReadInteger("NewMsg","KeyboardFlasherMode",0);
	bool CoreInactiveTabsNewMsgChk = Ini->ReadBool("NewMsg","CoreInactiveTabs",true);
	PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_MSGCOUNTER,CoreInactiveTabsNewMsgChk);
	InactiveTabsNewMsgChk = Ini->ReadBool("NewMsg","InactiveTabs",false);
	if((CoreInactiveTabsNewMsgChk)&&(InactiveTabsNewMsgChk)) PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_MSGCOUNTER,0);
	InactiveNotiferNewMsgChk = Ini->ReadBool("NewMsg","InactiveNotifer",false);
	ChatStateNotiferNewMsgChk = Ini->ReadBool("NewMsg","ChatStateNotifer",true);
	ChatGoneNotiferNewMsgChk = Ini->ReadBool("NewMsg","ChatGoneNotifer",true);
	ChatGoneCloudNotiferNewMsgChk = Ini->ReadBool("NewMsg","ChatGoneCloudNotifer",false);
	ChatGoneSoundNotiferNewMsgChk = Ini->ReadBool("NewMsg","ChatGoneSoundNotifer",false);
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
	ClipTabsChk = Ini->ReadBool("ClipTabs","Enabled",true);
	OpenClipTabsChk = Ini->ReadBool("ClipTabs","OpenClipTabs",true);
	InactiveClipTabsChk = Ini->ReadBool("ClipTabs","InactiveClipTabs",false);
	CounterClipTabsChk = Ini->ReadBool("ClipTabs","Counter",false);
	ExClipTabsFromTabSwitchingChk = Ini->ReadBool("ClipTabs","ExcludeFromTabSwitching",true);
	ExClipTabsFromSwitchToNewMsgChk = !Ini->ReadBool("ClipTabs","ExcludeFromSwitchToNewMsg",false);
	ExClipTabsFromTabsHotKeysChk = Ini->ReadBool("ClipTabs","ExcludeFromTabsHotKeys",true);
	MiniAvatarsClipTabsChk = Ini->ReadBool("ClipTabs","MiniAvatars",true);
	//FavouritesTabs
	FavouritesTabsChk = Ini->ReadBool("FavouritesTabs","Enabled",true);
	FastAccessFavouritesTabsChk = Ini->ReadBool("FavouritesTabs","FastAccess",true);
	FrmMainFastAccessFavouritesTabsChk = Ini->ReadBool("FavouritesTabs","FrmMainFastAccess",false);
	FrmSendFastAccessFavouritesTabsChk = Ini->ReadBool("FavouritesTabs","FrmSendFastAccess",true);
	FavouritesTabsHotKeysChk = Ini->ReadBool("FavouritesTabs","HotKeys",false);
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
	//Wylaczenia opcji uruchamiania komunikatora w postaci zminimalizowanej.
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
	//Okno kontaktow chowane za lewa krawedzia ekranu
	if((FrmMainSlideEdge==1)&&((Shell_TrayWndRect.Height()==Screen->Height)&&(Shell_TrayWndRect.Left==0)))
	{
		FrmMain_Shell_TrayWndLeft = Shell_TrayWndRect.Width();
		FrmMain_Shell_TrayWndRight = 0;
		FrmMain_Shell_TrayWndBottom = 0;
		FrmMain_Shell_TrayWndTop = 0;
	}
	//Okno kontaktow chowane za prawa krawedzia ekranu
	else if((FrmMainSlideEdge==2)&&((Shell_TrayWndRect.Height()==Screen->Height)&&(Shell_TrayWndRect.Right==Screen->Width)))
	{
		FrmMain_Shell_TrayWndRight = Shell_TrayWndRect.Width();
		FrmMain_Shell_TrayWndLeft = 0;
		FrmMain_Shell_TrayWndBottom = 0;
		FrmMain_Shell_TrayWndTop = 0;
	}
	//Okno kontaktow chowane za dolna krawedzia ekranu
	else if((FrmMainSlideEdge==3)&&((Shell_TrayWndRect.Width()==Screen->Width)&&(Shell_TrayWndRect.Bottom==Screen->Height)))
	{
		FrmMain_Shell_TrayWndBottom = Shell_TrayWndRect.Height();
		FrmMain_Shell_TrayWndLeft = 0;
		FrmMain_Shell_TrayWndRight = 0;
		FrmMain_Shell_TrayWndTop = 0;
	}
	//Okno kontaktow chowane za gorna krawedzia ekranu
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
		//Tymczasowa blokada chowania/pokazywania okna kontaktow
		FrmMainBlockSlide = true;
		//Zmiana stanu okna
		SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		//Przywrocenie poczatkowej pozycji okna przy zmianie prostopadlych krawedzi
		if(((pFrmMainSlideEdge==1)||(pFrmMainSlideEdge==2))&&((FrmMainSlideEdge==3)||(FrmMainSlideEdge==4)))
		{
			//Okno kontaktow chowane za lewa krawedzia ekranu
			if(pFrmMainSlideEdge==1)
				SetWindowPos(hFrmMain,HWND_TOP,0+FrmMain_Shell_TrayWndLeft,FrmMainRect.Top,0,0,SWP_NOSIZE);
			//Okno kontaktow chowane za prawa krawedzia ekranu
			else
				SetWindowPos(hFrmMain,HWND_TOP,Screen->Width-FrmMainRect.Width()-FrmMain_Shell_TrayWndRight,FrmMainRect.Top,0,0,SWP_NOSIZE);
			//Pobranie rozmiaru+pozycji okna kontatkow
			GetFrmMainRect();
		}
		else if(((pFrmMainSlideEdge==3)||(pFrmMainSlideEdge==4))&&((FrmMainSlideEdge==1)||(FrmMainSlideEdge==2)))
		{
			//Okno kontaktow chowane za dolna krawedzia ekranu
			if(FrmMainSlideEdge==3)
				SetWindowPos(hFrmMain,HWND_TOP,FrmMainRect.Left,Screen->Height-FrmMainRect.Height()-FrmMain_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
			//Okno kontaktow chowane za gorna krawedzia ekranu
			else
				SetWindowPos(hFrmMain,HWND_TOP,FrmMainRect.Left,FrmMain_Shell_TrayWndTop,0,0,SWP_NOSIZE);
			//Pobranie rozmiaru+pozycji okna kontatkow
			GetFrmMainRect();
		}
		//Ustawienie poprawnej pozycji okna kontaktow
		SetFrmMainPos();
		//Status pre-wysuwania okna kontaktow zza krawedzi ekranu
		PreFrmMainSlideIn = false;
		//Status wysuwania okna kontaktow zza krawedzi ekranu
		FrmMainSlideIn = false;
		//Status pre-chowania okna kontaktow za krawedz ekranu
		PreFrmMainSlideOut = false;
		//Status chowania okna kontaktow za krawedz ekranu
		FrmMainSlideOut = false;
		//Stan widocznosci okna kontaktow
		FrmMainVisible = true;
		//Tymczasowa blokada chowania/pokazywania okna kontaktow
		FrmMainBlockSlide = false;
		//Pobranie rozmiaru+pozycji okna kontatkow
		GetFrmMainRect();
		//Zapisanie pozycji okna kontaktow do ustawiem AQQ
		TSaveSetup SaveSetup;
		SaveSetup.Section = L"Position";
		SaveSetup.Ident = L"MainLeft";
		UnicodeString KeyValue = IntToStr((int)FrmMainRect.Left);
		SaveSetup.Value = KeyValue.w_str();
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		SaveSetup.Section = L"Position";
		SaveSetup.Ident = L"MainTop";
		KeyValue = IntToStr((int)FrmMainRect.Top);
		SaveSetup.Value = KeyValue.w_str();
		PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
		//Odswiezenie ustawien
		PluginLink.CallService(AQQ_FUNCTION_REFRESHSETUP,0,0);
	}
	bool pFrmSendSlideChk = FrmSendSlideChk;
	FrmSendSlideChk = Ini->ReadBool("SideSlide","SlideFrmSend",false);
	int pFrmSendSlideEdge = FrmSendSlideEdge;
	FrmSendSlideEdge = Ini->ReadInteger("SideSlide","FrmSendEdge",1);
	FrmSendSlideHideMode = Ini->ReadInteger("SideSlide","FrmSendHideMode",3);
	FrmSendSlideInDelay = Ini->ReadInteger("SideSlide","FrmSendSlideInDelay",1000);
	FrmSendSlideOutDelay = Ini->ReadInteger("SideSlide","FrmSendSlideOutDelay",1);
	FrmSendSlideInTime = Ini->ReadInteger("SideSlide","FrmSendSlideInTime",300);
	FrmSendSlideOutTime = Ini->ReadInteger("SideSlide","FrmSendSlideOutTime",500);
	FrmSendStepInterval = Ini->ReadInteger("SideSlide","FrmSendStepInterval",30);
	SlideInAtNewMsgChk = Ini->ReadBool("SideSlide","SlideInAtNewMsg",false);
	if(FrmSendStepInterval==30) Ini->WriteInteger("SideSlide","FrmSendStepInterval",30);
	//Wylaczenia opcji uruchamiania okna rozmowy w postaci zminimalizowanej po przyjsciu nowej wiadomosci
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
			SetForegroundWindow(hFrmSend);
		}
	}
	//Ustawianie danych paska menu start dla okna rozmowy
	//Okno rozmowy chowane za lewa krawedzia ekranu
	if((FrmSendSlideEdge==1)&&((Shell_TrayWndRect.Height()==Screen->Height)&&(Shell_TrayWndRect.Left==0)))
	{
		FrmSend_Shell_TrayWndLeft = Shell_TrayWndRect.Width();
		FrmSend_Shell_TrayWndRight = 0;
		FrmSend_Shell_TrayWndBottom = 0;
		FrmSend_Shell_TrayWndTop = 0;
	}
	//Okno rozmowy chowane za prawa krawedzia ekranu
	else if((FrmSendSlideEdge==2)&&((Shell_TrayWndRect.Height()==Screen->Height)&&(Shell_TrayWndRect.Right==Screen->Width)))
	{
		FrmSend_Shell_TrayWndRight = Shell_TrayWndRect.Width();
		FrmSend_Shell_TrayWndLeft = 0;
		FrmSend_Shell_TrayWndBottom = 0;
		FrmSend_Shell_TrayWndTop = 0;
	}
	//Okno rozmowy chowane za dolna krawedzia ekranu
	else if((FrmSendSlideEdge==3)&&((Shell_TrayWndRect.Width()==Screen->Width)&&(Shell_TrayWndRect.Bottom==Screen->Height)))
	{
		FrmSend_Shell_TrayWndBottom = Shell_TrayWndRect.Height();
		FrmSend_Shell_TrayWndLeft = 0;
		FrmSend_Shell_TrayWndRight = 0;
		FrmSend_Shell_TrayWndTop = 0;
	}
	//Okno rozmowy chowane za gorna krawedzia ekranu
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
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			FrmSendBlockSlide = true;
			//Zmiana stanu okna
			SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
			//Przywrocenie poczatkowej pozycji okna przy zmianie prostopadlych krawedzi
			if(((pFrmSendSlideEdge==1)||(pFrmSendSlideEdge==2))&&((FrmSendSlideEdge==3)||(FrmSendSlideEdge==4)))
			{
				//Okno rozmowy chowane za lewa krawedzia ekranu
				if(pFrmSendSlideEdge==1)
					SetWindowPos(hFrmSend,HWND_TOP,0+FrmSend_Shell_TrayWndLeft,FrmSendRect.Top,0,0,SWP_NOSIZE);
				//Okno rozmowy chowane za prawa krawedzia ekranu
				else
					SetWindowPos(hFrmSend,HWND_TOP,Screen->Width-FrmSendRect.Width()-FrmSend_Shell_TrayWndRight,FrmSendRect.Top,0,0,SWP_NOSIZE);
				//Pobranie rozmiaru+pozycji okna rozmowy
				GetFrmSendRect();
			}
			else if(((pFrmSendSlideEdge==3)||(pFrmSendSlideEdge==4))&&((FrmSendSlideEdge==1)||(FrmSendSlideEdge==2)))
			{
				//Okno rozmowy chowane za dolna krawedzia ekranu
				if(pFrmSendSlideEdge==3)
					SetWindowPos(hFrmSend,HWND_TOP,FrmSendRect.Left,Screen->Height-FrmSendRect.Height()-FrmSend_Shell_TrayWndBottom,0,0,SWP_NOSIZE);
				//Okno rozmowy chowane za gorna krawedzia ekranu
				else
					SetWindowPos(hFrmSend,HWND_TOP,FrmSendRect.Left,FrmSend_Shell_TrayWndTop,0,0,SWP_NOSIZE);
				//Pobranie rozmiaru+pozycji okna rozmowy
				GetFrmSendRect();
			}
			//Ustawienie poprawnej pozycji okna rozmowy
			SetFrmSendPos();
			//Status pre-wysuwania okna rozmowy zza krawedzi ekranu
			PreFrmSendSlideIn = false;
			//Status wysuwania okna rozmowy zza krawedzi ekranu
			FrmSendSlideIn = false;
			//Status pre-chowania okna rozmowy za krawedz ekranu
			PreFrmSendSlideOut = false;
			//Status chowania okna rozmowy za krawedz ekranu
			FrmSendSlideOut = false;
			//Stan widocznosci okna rozmowy
			FrmSendVisible = true;
			//Tymczasowa blokada chowania/pokazywania okna rozmowy
			FrmSendBlockSlide = false;
			//Pobranie rozmiaru+pozycji okna rozmowy
			GetFrmSendRect();
			//Zapisanie pozycji okna rozmowy do ustawiem AQQ
			TSaveSetup SaveSetup;
			SaveSetup.Section = L"Position";
			SaveSetup.Ident = L"MsgLeft";
			UnicodeString KeyValue = IntToStr((int)FrmSendRect.Left);
			SaveSetup.Value = KeyValue.w_str();
			PluginLink.CallService(AQQ_FUNCTION_SAVESETUP,1,(LPARAM)(&SaveSetup));
			SaveSetup.Section = L"Position";
			SaveSetup.Ident = L"MsgTop";
			KeyValue = IntToStr((int)FrmSendRect.Top);
			SaveSetup.Value = KeyValue.w_str();
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
	StayOnTopChk = Ini->ReadBool("Other","StayOnTop",true);
	HideToolBarChk = Ini->ReadBool("Other","HideToolBar",false);
	HideTabCloseButtonChk = Ini->ReadBool("Other","HideTabCloseButton",true);
	PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_CLOSEBTN,!HideTabCloseButtonChk);
	HideScrollTabButtonsChk = Ini->ReadBool("Other","HideScrollTabButtons",false);
	CloseBy2xLPMChk = Ini->ReadBool("Other","CloseBy2xLPM",false);
	CloudTimeOut = Ini->ReadInteger("Other","CloudTimeOut",6);
	CloudTickModeChk = Ini->ReadBool("Other","CloudTickMode",true);
	PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_SEARCHONLIST,Ini->ReadBool("Other","SearchOnList",true));
	bool pShortenLinksChk = ShortenLinksChk;
	ShortenLinksChk = Ini->ReadBool("Other","ShortenLinks",true);
	bool pShortenLinksMode = ShortenLinksMode;
	ShortenLinksMode = Ini->ReadInteger("Other","ShortenLinksMode",1);
	//Odswiezenie listy kontaktow
	if(((ShortenLinksChk!=pShortenLinksChk)||(ShortenLinksMode!=pShortenLinksMode))&&((ShortenLinksMode==1)||(ShortenLinksMode==3))&&(!LoadExecuted))
		RefreshList();
	//Wylaczanie funkcji pisaka na pasku tytulu okna rozmowy
	if((InactiveFrmNewMsgChk)||(TweakFrmSendTitlebarChk)||(!TaskbarPenChk))
		PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_TASKBARPEN,0);
	else
		PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_TASKBARPEN,1);

	delete Ini;
}
//---------------------------------------------------------------------------

//Zaladowanie wtyczki
extern "C" INT_PTR __declspec(dllexport) __stdcall Load(PPluginLink Link)
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
	//Tworzenie katalogow lokalizacji
	if(!DirectoryExists(PluginUserDir+"\\\\Languages"))
		CreateDir(PluginUserDir+"\\\\Languages");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\TabKit"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\TabKit");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL");
  //Wypakowanie plikow lokalizacji
	//99E2CDBA63BCC0050D32F6FECEBBC171
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\Const.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\Const.lng").w_str(),L"EN_CONST",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\Const.lng")!="99E2CDBA63BCC0050D32F6FECEBBC171")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\Const.lng").w_str(),L"EN_CONST",L"DATA");
	//13FE1206D7F7A1A3874E9405C8913699
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\TSettingsForm.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\TSettingsForm.lng").w_str(),L"EN_SETTINGSFRM",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\TSettingsForm.lng")!="13FE1206D7F7A1A3874E9405C8913699")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\TSettingsForm.lng").w_str(),L"EN_SETTINGSFRM",L"DATA");
	//324061A51B896B06E99EF1B88D062B2F
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\TSideSlideExceptionsForm.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\TSideSlideExceptionsForm.lng").w_str(),L"EN_SIDESLIDEEXCEPTIONSFRM",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\TSideSlideExceptionsForm.lng")!="324061A51B896B06E99EF1B88D062B2F")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\EN\\\\TSideSlideExceptionsForm.lng").w_str(),L"EN_SIDESLIDEEXCEPTIONSFRM",L"DATA");
	//EBB83209A29FEB1D8A412FA57E6BBA48
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\Const.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\Const.lng").w_str(),L"PL_CONST",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\Const.lng")!="EBB83209A29FEB1D8A412FA57E6BBA48")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\Const.lng").w_str(),L"PL_CONST",L"DATA");
	//39ACDD1BC30A337CEAFC572A15C131F0
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\TSettingsForm.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\TSettingsForm.lng").w_str(),L"PL_SETTINGSFRM",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\TSettingsForm.lng")!="39ACDD1BC30A337CEAFC572A15C131F0")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\TSettingsForm.lng").w_str(),L"PL_SETTINGSFRM",L"DATA");
	//589F6BBC7D5CB448CBF9DDD2BC15D54C
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\TSideSlideExceptionsForm.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\TSideSlideExceptionsForm.lng").w_str(),L"PL_SIDESLIDEEXCEPTIONSFRM",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\TSideSlideExceptionsForm.lng")!="589F6BBC7D5CB448CBF9DDD2BC15D54C")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\TabKit\\\\PL\\\\TSideSlideExceptionsForm.lng").w_str(),L"PL_SIDESLIDEEXCEPTIONSFRM",L"DATA");
	//Ustawienie sciezki lokalizacji wtyczki
	UnicodeString LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETLANGCODE,0,0);
	LangPath = GetPluginUserDirW() + "\\\\Languages\\\\TabKit\\\\" + LangCode + "\\\\";
	if(!DirectoryExists(LangPath))
	{
		LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETDEFLANGCODE,0,0);
		LangPath = GetPluginUserDirW() + "\\\\Languages\\\\TabKit\\\\" + LangCode + "\\\\";
	}
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
		ExtractRes((PluginUserDir + "\\\\Shared\\\\TabKit.dll.png").w_str(),L"SHARED",L"DATA");
	else if(MD5File(PluginUserDir + "\\\\Shared\\\\TabKit.dll.png")!="992F252A9087DBD036BA98B6D5DE2A08")
		ExtractRes((PluginUserDir + "\\\\Shared\\\\TabKit.dll.png").w_str(),L"SHARED",L"DATA");
	//Wypakiwanie ikonki FastAccess.png
	//1DF8A978FA63D5C1EBEDB23BA94A1C3D
	if(!FileExists(PluginUserDir + "\\\\TabKit\\\\FastAccess.png"))
		ExtractRes((PluginUserDir + "\\\\TabKit\\\\FastAccess.png").w_str(),L"FASTACCESS",L"DATA");
	else if(MD5File(PluginUserDir + "\\\\TabKit\\\\FastAccess.png")!="1DF8A978FA63D5C1EBEDB23BA94A1C3D")
		ExtractRes((PluginUserDir + "\\\\TabKit\\\\FastAccess.png").w_str(),L"FASTACCESS",L"DATA");
	//Wypakowanie ikonki Blabler.png
	//BD0334AC086F55413A716DE11C6059C7
	//if(!FileExists(PluginUserDir + "\\\\TabKit\\\\Blabler.png"))
	// ExtractRes((PluginUserDir + "\\\\TabKit\\\\Blabler.png").w_str(),L"BLABLER",L"DATA");
	//else if(MD5File(PluginUserDir + "\\\\TabKit\\\\Blabler.png")!="BD0334AC086F55413A716DE11C6059C7")
	// ExtractRes((PluginUserDir + "\\\\TabKit\\\\Blabler.png").w_str(),L"BLABLER",L"DATA");
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
	//BLABLER
	//if(!FileExists(ThemeDir + "Blabler.png"))
	//	BLABLER = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\TabKit\\\\Blabler.png").w_str());
	//else
	//	BLABLER = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "Blabler.png").w_str());
	//BLABLER sciezek do ikon
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
	//Szybki dostep ulubionych zakladek
	TPluginAction FavouritesTabsPopUp;
	ZeroMemory(&FavouritesTabsPopUp,sizeof(TPluginAction));
	FavouritesTabsPopUp.cbSize = sizeof(TPluginAction);
	FavouritesTabsPopUp.pszName = L"TabKitFavouritesTabsPopUp";
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENU,0,(LPARAM)(&FavouritesTabsPopUp));
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
	PluginLink.CreateServiceFunction(L"sTabKitClipTabCaptionItem",ServiceClipTabCaptionItem);
	//Ulubione zakladki
	PluginLink.CreateServiceFunction(L"sTabKitFavouriteTabItem",ServiceFavouriteTabItem);
	PluginLink.CreateServiceFunction(L"sTabKitFavouritesTabsItem0",ServiceFavouritesTabsItem0);
	PluginLink.CreateServiceFunction(L"sTabKitFavouritesTabsItem1",ServiceFavouritesTabsItem1);
	PluginLink.CreateServiceFunction(L"sTabKitFavouritesTabsItem2",ServiceFavouritesTabsItem2);
	PluginLink.CreateServiceFunction(L"sTabKitFavouritesTabsItem3",ServiceFavouritesTabsItem3);
	PluginLink.CreateServiceFunction(L"sTabKitFavouritesTabsItem4",ServiceFavouritesTabsItem4);
	PluginLink.CreateServiceFunction(L"sTabKitFavouritesTabsItem5",ServiceFavouritesTabsItem5);
	PluginLink.CreateServiceFunction(L"sTabKitFavouritesTabsItem6",ServiceFavouritesTabsItem6);
	PluginLink.CreateServiceFunction(L"sTabKitFavouritesTabsItem7",ServiceFavouritesTabsItem7);
	PluginLink.CreateServiceFunction(L"sTabKitFavouritesTabsItem8",ServiceFavouritesTabsItem8);
	PluginLink.CreateServiceFunction(L"sTabKitFavouritesTabsItem9",ServiceFavouritesTabsItem9);
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
	PluginLink.HookEvent(AQQ_SYSTEM_COLORCHANGEV2,OnColorChange);
	//Hook na zmiane stanu kontaktu
	PluginLink.HookEvent(AQQ_CONTACTS_UPDATE,OnContactsUpdate);
	//Hook na zmiane lokalizacji
	PluginLink.HookEvent(AQQ_SYSTEM_LANGCODE_CHANGED,OnLangCodeChanged);
	//Hook na zakonczenie ladowania listy kontaktow przy starcie AQQ
	PluginLink.HookEvent(AQQ_CONTACTS_LISTREADY,OnListReady);
	//Hook na wpisywany tekst w oknie rozmowy
	PluginLink.HookEvent(AQQ_SYSTEM_MSGCOMPOSING,OnMsgComposing);
	//Hook na zamkniecie menu kontekstowego pola wiadomosci
	PluginLink.HookEvent(AQQ_SYSTEM_MSGCONTEXT_CLOSE,OnMsgContextClose);
	//Hook na otwarcie menu kontekstowego pola wiadomosci
	PluginLink.HookEvent(AQQ_SYSTEM_MSGCONTEXT_POPUP,OnMsgContextPopup);
	//Hook na zamkniecie notyfikacji o nowej wiadomosci w zasobniku systemowym
	PluginLink.HookEvent(AQQ_SYSTEM_NOTIFICATIONCLOSED,OnNotificationClosed);
	//Hook na pobieranie otwieranie adresow URL i przekazywanie plikow do aplikacji
	PluginLink.HookEvent(AQQ_SYSTEM_PERFORM_COPYDATA,OnPerformCopyData);
	//Hook na wysylanie nowej wiadomosci
	PluginLink.HookEvent(AQQ_CONTACTS_PRESENDMSG,OnPreSendMsg);
	//Hook na odbieranie nowej wiadomosci
	PluginLink.HookEvent(AQQ_CONTACTS_RECVMSG,OnRecvMsg);
	//Hook na enumeracje listy kontatkow
	PluginLink.HookEvent(AQQ_CONTACTS_REPLYLIST,OnReplyList);
	//Hook na restart komunikatora
	//PluginLink.HookEvent(AQQ_SYSTEM_RESTART,OnSystemRestart);
	//Hook na zmiane nazwy zasobu przez wtyczke ResourcesChanger
	PluginLink.HookEvent(RESOURCESCHANGER_SYSTEM_RESOURCECHANGED,OnResourceChanged);
	//Hook na zmiane widocznego opisu kontaktu na liscie kontatkow
	PluginLink.HookEvent(AQQ_CONTACTS_SETHTMLSTATUS,OnSetHTMLStatus);
	//Hook na polaczenie sieci przy starcie AQQ
	PluginLink.HookEvent(AQQ_SYSTEM_SETLASTSTATE,OnSetLastState);
	//Hook dla zmiany stanu
	PluginLink.HookEvent(AQQ_SYSTEM_STATECHANGE,OnStateChange);
	//Hook na pokazywanie popupmenu
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
	//Ladowanie ulubionych zakladek
	LoadFavouritesTabs();
	//Tworzenie interfejsu szybkiego dostepu do ustawien wtyczki
	BuildTabKitFastSettings();
	//Tworzenie elementu dodawania/usuwania ulubionej zakladki
	BuildFrmMainFavouriteTab();
	//Pobranie stylu zalacznika
	GetAttachmentStyle();
	//Odczytywanie stanu nieprzeczytanych wiadomosci
	if(FrmMainSlideChk)
	{
		//Odczytywanie danych do pliku sesji
		TIniFile *Ini = new TIniFile(SessionFileDir);
		//Informacja o nieprzeczytanych wiadomosciach istnieje
		if(Ini->SectionExists("UnOpenMsgList"))
		{
			TStringList *UnOpenMsgSection = new TStringList;
			Ini->ReadSection("UnOpenMsgList",UnOpenMsgSection);
			if(UnOpenMsgSection->Count)
			{
				for(int Count=0;Count<UnOpenMsgSection->Count;Count++)
				{
					UnicodeString JID = Ini->ReadString("UnOpenMsgList","Tab"+IntToStr(Count+1),"");
					if(!JID.IsEmpty()) UnOpenMsgList->Add(JID);
				}
			}
			Ini->EraseSection("UnOpenMsgList");
			delete UnOpenMsgSection;
		}
		delete Ini;
	}
	//Wszystkie moduly zostaly zaladowane
	if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0))
	{
		//Pobranie ID dla enumeracji kontaktow
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
		//Stan widocznosci okna kontaktow
		FrmMainVisible = true;
		//Tymczasowa blokada chowania/pokazywania okna kontaktow
		FrmMainBlockSlide = false;
		//Przypisanie nowej procki dla okna kontaktow
		if(!OldFrmMainProc) OldFrmMainProc = (WNDPROC)SetWindowLongPtrW(hFrmMain, GWLP_WNDPROC,(LONG_PTR)FrmMainProc);
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
		//Szybki dostep do ulubionych zakladek
		BuildFavouritesTabs(false);
		//Sprawdzanie niewyslanych wiadomosci
		GetUnsentMsg();
		//Szybki dostep niewyslanych wiadomosci
		BuildFrmUnsentMsg(false);
		//Tworzenie interfesju dla ostatnio zamknietych zakladek
		BuildFrmClosedTabs(false);
		//Tworzenie interfejsu trzymania okna rozmowy na wierzchu
		BuildStayOnTop();
		//Tworzenie elementu do przypinania/odpiniania zakladek oraz pokazywania/ukrywania caption zakladki
		BuildClipTab();
		//Tworzenie elementu dodawania/usuwania ulubionej zakladki
		BuildFrmSendFavouriteTab();
		//Tworzenie interfesju w AQQ dla ostatnio zamknietych zakladek
		BuildAQQClosedTabs();
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
		//Zezwolenie na odswiezenie listy kontaktow
		AllowRefreshList = true;
		//Odswiezenie listy kontaktow - skracanie wyswietlania odnosnikow na liscie kontaktow do wygodniejszej formy
		if((ShortenLinksChk)&&((ShortenLinksMode==1)||(ShortenLinksMode==3)))
			RefreshList();
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
	SetTimer(hTimerFrm,TIMER_MOUSE_POSITION,25,(TIMERPROC)TimerFrmProc);
	//Timer na sprawdzanie aktywnego okna
	SetTimer(hTimerFrm,TIMER_ACTIVE_WINDOW,25,(TIMERPROC)TimerFrmProc);
	//Info o zakonczeniu procedury ladowania
	LoadExecuted = false;

	return 0;
}
//---------------------------------------------------------------------------

//Wyladowanie wtyczki
extern "C" INT_PTR __declspec(dllexport) __stdcall Unload()
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
		SetWindowLongPtrW(hFrmSend, GWLP_WNDPROC,(LONG_PTR)OldFrmSendProc);
		//Skasowanie procki
		OldFrmSendProc = NULL;
	}
	//Przypisanie starej procki do okna kontaktow
	if(OldFrmMainProc)
	{
		//Przywrocenie wczesniej zapisanej procki
		SetWindowLongPtrW(hFrmMain, GWLP_WNDPROC,(LONG_PTR)OldFrmMainProc);
		//Skasowanie procki
		OldFrmMainProc = NULL;
	}
	//Przypisanie starej procki do okna wyszukiwarki
	if(OldFrmSeekOnListProc)
	{
		//Przywrocenie wczesniej zapisanej procki
		SetWindowLongPtrW(hFrmSeekOnList, GWLP_WNDPROC,(LONG_PTR)OldFrmSeekOnListProc);
		//Skasowanie procki
		OldFrmSeekOnListProc = NULL;
	}
	//Wyladowanie timerow
	for(int TimerID=10;TimerID<=380;TimerID=TimerID+10) KillTimer(hTimerFrm,TimerID);
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
		//Usuwanie interfejsu
		DestroyClipTab();
		//Usuwanie serwisu
		PluginLink.DestroyServiceFunction(ServiceClipTabItem);
		PluginLink.DestroyServiceFunction(ServiceClipTabCaptionItem);
		//Ulubione zakladki
		//Usuwanie interfejsu
		DestroyFrmSendFavouriteTab();
		DestroyFrmMainFavouriteTab();
		DestroyFavouritesTabs();
		//Usuwanie serwisow
		PluginLink.DestroyServiceFunction(ServiceFavouriteTabItem);
		PluginLink.DestroyServiceFunction(ServiceFavouritesTabsItem0);
		PluginLink.DestroyServiceFunction(ServiceFavouritesTabsItem1);
		PluginLink.DestroyServiceFunction(ServiceFavouritesTabsItem2);
		PluginLink.DestroyServiceFunction(ServiceFavouritesTabsItem3);
		PluginLink.DestroyServiceFunction(ServiceFavouritesTabsItem4);
		PluginLink.DestroyServiceFunction(ServiceFavouritesTabsItem5);
		PluginLink.DestroyServiceFunction(ServiceFavouritesTabsItem6);
		PluginLink.DestroyServiceFunction(ServiceFavouritesTabsItem7);
		PluginLink.DestroyServiceFunction(ServiceFavouritesTabsItem8);
		PluginLink.DestroyServiceFunction(ServiceFavouritesTabsItem9);
		//Usuwanie PopUpMenu
		TPluginAction FavouritesTabsPopUp;
		ZeroMemory(&FavouritesTabsPopUp,sizeof(TPluginAction));
		FavouritesTabsPopUp.cbSize = sizeof(TPluginAction);
		FavouritesTabsPopUp.pszName = L"TabKitFavouritesTabsPopUp";
		PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU,0,(LPARAM)(&FavouritesTabsPopUp));
		//Trzymanie okna rozmowy na wierzchu
		//Usuwanie interfejsu
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
	PluginLink.UnhookEvent(OnLangCodeChanged);
	PluginLink.UnhookEvent(OnListReady);
	PluginLink.UnhookEvent(OnMsgComposing);
	PluginLink.UnhookEvent(OnMsgContextPopup);
	PluginLink.UnhookEvent(OnNotificationClosed);
	PluginLink.UnhookEvent(OnMsgContextClose);
	PluginLink.UnhookEvent(OnPerformCopyData);
	PluginLink.UnhookEvent(OnPreSendMsg);
	PluginLink.UnhookEvent(OnRecvMsg);
	PluginLink.UnhookEvent(OnReplyList);
	//PluginLink.UnhookEvent(OnSystemRestart);
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
		//PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)BLABLER);
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
		Ini->EraseSection("SessionEx");
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
				int ClosedTimeStr = DateTimeToUnix(ClosedTime, true);
				ClosedTabsTimeList->Insert(0,IntToStr(ClosedTimeStr));
				//Maks pamietanych X elementow
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
		//Sa jakies zakladki do zapamietania
		if(TabsCount>0)
		{
			//Maks X zdefiniowanych elementow
			if(TabsCount>CountUnCloseTabVal) TabsCount = CountUnCloseTabVal;
			//Zapamietywanie zakladek
			for(int Count=0;Count<TabsCount;Count++)
			{
				if(!ClosedTabsList->Strings[Count].IsEmpty())
				{
					//Zapis danych do pliku sesji
					Ini->WriteString("ClosedTabs","Tab"+IntToStr(Count+1),ClosedTabsList->Strings[Count]);
					//Pole z data zamkniecia jest puste
					if(ClosedTabsTimeList->Strings[Count].IsEmpty()) ClosedTabsTimeList->Strings[Count] = 0;
					//Zapis danych do pliku sesji
					Ini->WriteInteger("ClosedTabs","Tab"+IntToStr(Count+1)+"Timestamp",StrToInt(ClosedTabsTimeList->Strings[Count]));
				}
			}
		}
		delete Ini;
	}
	//Zapamietywanie stanu nieprzeczytanych wiadomosci
	if((ForceUnloadExecuted)&&(FrmMainSlideChk)&&(UnOpenMsgList->Count))
	{
		//Zapisywanie danych do pliku sesji
		TIniFile *Ini = new TIniFile(SessionFileDir);
		for(int Count=0;Count<UnOpenMsgList->Count;Count++)
		{
			if(!UnOpenMsgList->Strings[Count].IsEmpty())
				Ini->WriteString("UnOpenMsgList","Tab"+IntToStr(Count+1),UnOpenMsgList->Strings[Count]);
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
	//Miganie diodami LED klawiatury - wylaczanie mrugania
	if((KeyboardFlasherChk)&&(hFlasherThread)&&(hFlasherKeyboardThread))
	{
		SetEvent(hFlasherThread);
		WaitForSingleObject(hFlasherKeyboardThread, 30000);
		CloseHandle(hFlasherThread);
		hFlasherThread = NULL;
		hFlasherKeyboardThread = NULL;
	}
	//Przywracanie przyciskow strzalek do przewijania zakladek
	if(!ForceUnloadExecuted) CheckHideScrollTabButtons();
	//Przywracanie okna rozmowy do "normalnosci"
	if(hFrmSend) SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	if((hFrmSend)&&(FrmSendSlideChk))
	{
		//Tymczasowa blokada chowania/pokazywania okna rozmowy
		FrmSendBlockSlide = true;
		//Przywracanie okna rozmowy
		if(IsIconic(hFrmSend))
		{
			ShowWindow(hFrmSend,SW_RESTORE);
			SetForegroundWindow(hFrmSend);
		}
		//Ustawienie poprawnej pozycji okna rozmowy
		SetFrmSendPos();
	}
	//Pokazywanie wczesniej ukrytego paska narzedzi
	if((HideToolBarChk)&&(hToolBar)&&(!ForceUnloadExecuted)&&(!FrmInstallAddonExist))
		ShowToolBar();
	//Odswiezenie wszystkich zakladek
	if(((ClipTabsList->Count)||(HideTabCloseButtonChk))&&(!ForceUnloadExecuted)&&(!FrmInstallAddonExist)) RefreshTabs();
	//Przywracanie okna kontatkow do "normalnosci"
	if(hFrmMain) SetWindowPos(hFrmMain,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	if((hFrmMain)&&(FrmMainSlideChk))
	{
		//Tymczasowa blokada chowania/pokazywania okna kontaktow
		FrmMainBlockSlide = true;
		//Przywracanie okna kontatow
		if(IsIconic(hFrmMain))
		{
			ShowWindow(hFrmMain,SW_RESTORE);
			SetForegroundWindow(hFrmMain);
		}
		//Ustawienie poprawnej pozycji okna kontaktow
		SetFrmMainPos();
	}
	//Odswiezenie listy kontaktow - przywrocenie nie skroconych odnosnikow na liscie kontaktow
	if((ShortenLinksChk)&&((ShortenLinksMode==1)||(ShortenLinksMode==3))&&(!ForceUnloadExecuted)&&(!FrmInstallAddonExist))
		RefreshList();
	//Info o zakonczeniu procedury wyladowania
	UnloadExecuted = false;

	return 0;
}
//---------------------------------------------------------------------------

//Ustawienia wtyczki
extern "C" INT_PTR __declspec(dllexport)__stdcall Settings()
{
	//Otwieranie okna ustawien wtyczki
	OpenPluginSettings();

	return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" PPluginInfo __declspec(dllexport) __stdcall AQQPluginInfo(DWORD AQQVersion)
{
	PluginInfo.cbSize = sizeof(TPluginInfo);
	PluginInfo.ShortName = L"TabKit";
	PluginInfo.Version = PLUGIN_MAKE_VERSION(1,10,0,2);
	PluginInfo.Description = L"Wtyczka oferuje masê funkcjonalnoœci usprawniaj¹cych korzystanie z komunikatora np. zapamiêtywanie zamkniêtych zak³adek, inteligentne prze³¹czanie, zapamiêtywanie sesji.";
	PluginInfo.Author = L"Krzysztof Grochocki";
	PluginInfo.AuthorMail = L"kontakt@beherit.pl";
	PluginInfo.Copyright = L"Krzysztof Grochocki";
	PluginInfo.Homepage = L"http://beherit.pl";
	PluginInfo.Flag = 0;
	PluginInfo.ReplaceDefaultModule = 0;

	return &PluginInfo;
}
//---------------------------------------------------------------------------

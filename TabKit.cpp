//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#include <Windowsx.h>
#pragma hdrstop
#pragma argsused
#include "SettingsFrm.h"
#include "Aqq.h"
#include <inifiles.hpp>
#include <process.h>
#include <Clipbrd.hpp>
#include <IdHashMessageDigest.hpp>
#include <XMLDoc.hpp>
#define AQQRESTARTER_SYSTEM_RESTARTING L"AQQRestarter/System/Restarting"
#include <boost/regex.hpp>
using namespace boost;
using namespace std;
//---------------------------------------------------------------------------

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

//Uchwyt do formy ustawien
TSettingsForm *hSettingsForm;
//Struktury glowne
TPluginLink PluginLink;
TPluginInfo PluginInfo;
TPluginContact PluginContact;
PPluginContact Contact;
PPluginMessage Message;
PPluginWindowEvent WindowEvent;
TPluginShowInfo PluginShowInfo;
PPluginChatState ChatState;
PPluginPopUp PopUp;
//UnsentMsg
TPluginAction FrmUnsentMsgPopUp;
TPluginAction FrmMainUnsentMsgButton, FrmSendUnsentMsgButton;
TPluginAction BuildFrmUnsentMsgItem[7];
TPluginAction DestroyFrmUnsentMsgItem;
//ClosedTabs
TPluginChatPrep PluginChatPrep;
TPluginAction FrmClosedTabsPopUp;
TPluginAction FrmMainClosedTabsButton, FrmSendClosedTabsButton;
TPluginAction BuildFrmClosedTabsItem[12];
TPluginAction DestroyFrmClosedTabsItem;
UnicodeString JustUnClosedJID;
//ClipTabs
TPluginAction BuildClipTabItem;
UnicodeString JustUnClipTabJID;
//SessionRemember
bool RestoringSession = false;
bool SetLastStateRestore = false;
//NewMsg
int InactiveFrmNewMsgCount = 0;
TCustomIniFile* InactiveTabsNewMsgCount = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
bool ClosingTab = false;
bool BlockInactiveNotiferNewMsg = false;
bool Connecting = false;
//ChatState
UnicodeString ComposingIconSmall;
UnicodeString ComposingIconBig;
UnicodeString PauseIconSmall;
UnicodeString PauseIconBig;
HICON hIconSmall = NULL;
HICON hIconBig = NULL;
//InactiveNotifer
TPluginStateChange PluginStateChange;
PPluginStateChange StateChange;
//StayOnTop
TPluginAction BuildStayOnTopItem;
bool SetStayOnTop;
//EmuTabsW
bool EmuTabsWSupport;
//QuickQuote
TPluginAction BuildQuickQuoteItem;
PPluginTriple PluginTriple;
TPluginItemDescriber PluginItemDescriber;
PPluginAction Action;
//Stany kontaktow
TCustomIniFile* ContactsStateList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Ikonki interfesju
int UNSENTMSG;
int CLOSEDTABS;
int STAYONTOP_OFF;
int STAYONTOP_ON;
int COMPOSING;
int PAUSE;
//Aktywna otwarta zakladka
UnicodeString ActiveTabJID;
//JID kontaktu z menu popTab
UnicodeString ClipTabPopup;
//Sciezka do pliku sesji
UnicodeString SessionFileDir;
//Nazwa aktywnego profilu
UnicodeString ActiveProfileName;
//Uchwyt do okna rozmowy
HWND hFrmSend;
HWND hActiveFrm;
//Stan okna rozmowy
bool FrmSendMaximized = false;
bool FrmSendOpening = false;
bool ToolBarShowing = false;
//Uchwyt do pola RichEdit
HWND hRichEdit;
//Uchwyt do paska narzedzi
HWND hToolBar;
//Uchwyt do paska informacyjnego
HWND hStatusBarPro;
//Uchwyt do glownego okna
HWND hFrmMain;
HWND hFrmMainL;
//Tekst okna
wchar_t OryginalTitlebar[128];
wchar_t TempTitlebarW[128];
UnicodeString TempTitlebar;
//Klasa okna
wchar_t WClassName[128];
//Hook na klawiaturê
HHOOK hKeyboard;
//PID procesu
DWORD PID;
DWORD ProcessPID;
//Stara procka FrmSend
WNDPROC OldFrmSendProc;
//Pozycja/wielkosc okna
TRect WindowRect;
//Lista JID z nowymi wiadomosciami
TStringList *MsgList = new TStringList;
//Lista JID z notyfikacjami wiadomosci
TStringList *PreMsgList = new TStringList;
TCustomIniFile* PreMsgStateList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Lista JID otwartych zakladek
TStringList *TabsList = new TStringList;
//Lista JID zamknietych zakladek
TStringList *ClosedTabsList = new TStringList;
TStringList *ClosedTabsTimeList = new TStringList;
TStringList *AcceptClosedTabsList = new TStringList;
//Lista JID przypietych zakladek
TStringList *ClipTabsList = new TStringList;
TCustomIniFile* ClipTabsIconList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//Lista JID czatowych do przywrocenia
TStringList *ChatSessionList = new TStringList;

//SETTINGS-------------------------------------------------------------------
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
int CountUnCloseTabVal;
bool RestoreLastMsgChk;
bool OnlyConversationTabsChk;
//SessionRemember
bool RestoreTabsSessionChk;
bool ManualRestoreTabsSessionChk;
bool RestoreMsgSessionChk;
//Titlebar
bool TweakFrmSendTitlebarChk;
int TweakFrmSendTitlebarMode;
bool TweakFrmMainTitlebarChk;
int TweakFrmMainTitlebarMode;
UnicodeString TweakFrmMainTitlebarText;
//NewMsg
bool InactiveFrmNewMsgChk;
bool InactiveTabsNewMsgChk;
bool InactiveNotiferNewMsgChk;
bool ChatStateNotiferNewMsgChk;
//ClipTabs
bool OpenClipTabsChk;
bool InactiveClipTabsChk;
bool CounterClipTabsChk;
//Other
bool StayOnTopChk;
bool EmuTabsWChk;
bool QuickQuoteChk;
bool AntiSpimChk;
bool HideStatusBarChk;
bool HideToolBarChk;
bool CollapseImagesChk;
int CollapseImagesModeChk;
int CloudTimeOutChk;
bool CloudTickModeChk;
//---------------------------------------------------------------------------

//Szukanie uchwytu do kontrolki TRichEdit
bool CALLBACK FindRichEdit(HWND hWnd, LPARAM lParam)
{
  GetClassNameW(hWnd, WClassName, sizeof(WClassName));
  if((UnicodeString)WClassName=="TRichEdit")
  {
	hRichEdit = hWnd;
	return false;
  }
  else if((UnicodeString)WClassName=="TsRichEdit")
  {
	hRichEdit = hWnd;
	return false;
  }

  return true;
}
//---------------------------------------------------------------------------

//Szukanie uchwytu do kontrolki TToolBar
bool CALLBACK FindToolBar(HWND hWnd, LPARAM lParam)
{
  GetClassNameW(hWnd, WClassName, sizeof(WClassName));
  if((UnicodeString)WClassName=="TToolBar")
  {
	hToolBar = hWnd;
	return false;
  }

  return true;
}
//---------------------------------------------------------------------------

//Szukanie uchwytu okna glownego AQQ
bool CALLBACK FindFrmMainL(HWND hWnd, LPARAM lParam)
{
  GetClassNameW(hWnd, WClassName, sizeof(WClassName));
  if((UnicodeString)WClassName=="LayeredWndClass")
  {
	GetWindowTextW(hWnd,WClassName,sizeof(WClassName));
	if((UnicodeString)WClassName==(UnicodeString)OryginalTitlebar)
	{
	  GetWindowThreadProcessId(hWnd, &PID);
	  if(PID==ProcessPID)
	  {
		hFrmMainL = hWnd;
		return false;
	  }
	}
  }
  return true;
}
//---------------------------------------------------------------------------
bool CALLBACK FindFrmMain(HWND hWnd, LPARAM lParam)
{
  GetClassNameW(hWnd, WClassName, sizeof(WClassName));
  if((UnicodeString)WClassName=="TfrmMain")
  {
	GetWindowThreadProcessId(hWnd, &PID);
	if(PID==ProcessPID)
	{
	  hFrmMain = hWnd;
	  GetWindowTextW(hFrmMain,OryginalTitlebar,sizeof(OryginalTitlebar));
	  return false;
	}
  }
  return true;
}
//---------------------------------------------------------------------------

UnicodeString StrToIniStr(UnicodeString Str)
{
  char Buffer[512];
  char* B;
  char* S;

  S = Str.t_str();
  B = Buffer;
  while(*S != '\0')
  {
	switch (*S)
	{
	case 13:
	case 10:
	  if((*S == 13) && (S[1] == 10)) S++;
	  else if((*S == 10) && (S[1] == 13)) S++;
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
	}
  }
  *B = '\0';

  return String(Buffer);
}
//---------------------------------------------------------------------------

String IniStrToStr(UnicodeString Str)
{
  char Buffer[512];
  char* B;
  char* S;

  S = Str.t_str();
  B = Buffer;
  while(*S != '\0')
  {
	if((S[0] == '\\') && (S[1] == 'n'))
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

  return String(Buffer);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego uzytkownika
UnicodeString GetPluginUserDir()
{
  UnicodeString Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------
UnicodeString GetPluginUserDirW()
{
  UnicodeString Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
  return Dir;
}
//---------------------------------------------------------------------------
UnicodeString GetContactsUserDir()
{
  UnicodeString Dir = (wchar_t *)(PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,0,0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir + "\\\\Data\\\\Contacts\\\\";
}
//---------------------------------------------------------------------------
UnicodeString GetAvatarsUserDir()
{
  UnicodeString Dir = (wchar_t *)(PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,0,0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir + "\\\\Data\\\\Avatars\\\\";
}
//---------------------------------------------------------------------------

//Pobieranie Nick kontaktu podajac jego JID
UnicodeString GetContactNick(UnicodeString JID)
{
  TPluginContactSimpleInfo PluginContactSimpleInfo;
  PluginContactSimpleInfo.cbSize = sizeof(TPluginContactSimpleInfo);
  PluginContactSimpleInfo.JID = JID.w_str();
  PluginLink.CallService(AQQ_CONTACTS_FILLSIMPLEINFO,0,(LPARAM)(&PluginContactSimpleInfo));
  UnicodeString Nick = (wchar_t*)PluginContactSimpleInfo.Nick;
  Nick = Nick.Trim();
  if(Nick.IsEmpty())
  {
	if(JID.Pos("@")) JID.Delete(JID.Pos("@"),JID.Length());
	return JID;
  }
  else
   return Nick;
}
//---------------------------------------------------------------------------

//Pobieranie stanu kontaktu podajac jego JID
int GetContactState(UnicodeString JID)
{
  int State = ContactsStateList->ReadInteger("State",JID,0);
  //Jezeli stan nie jest zapisany
  if(!State)
  {
	//Przypisanie domyslnej ikonki
	PluginContact.cbSize = sizeof(TPluginContact);
	PluginContact.JID = JID.w_str();
    State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));
  }

  return State;
}
//---------------------------------------------------------------------------

//Usuwanie szybkiego dostepu do niewyslanych wiadomosci
void DestroyFrmUnsentMsg()
{
  TIniFile *Ini = new TIniFile(SessionFileDir);
  TStringList *Messages = new TStringList;
  Ini->ReadSection("Messages",Messages);
  int MsgCount = Messages->Count;
  //Maks 5 elementow w popupmenu
  if(MsgCount>5) MsgCount = 5;
  //Jezeli cos w ogule jest
  if(MsgCount>0)
  {
	//Usuwanie elementow popupmenu
	UnicodeString ItemName;
	//for(int Count=0;Count<MsgCount;Count++)
	for(int Count=0;Count<MsgCount;Count++)
	{
	  ItemName = "FrmUnsentMsgItem"+IntToStr(Count);
	  DestroyFrmUnsentMsgItem.pszName = ItemName.w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyFrmUnsentMsgItem));
	}
	//Usuwanie elementow do usuwania niewyslanych wiadomosci
	if(FastClearUnsentMsgChk)
	{
	  ItemName = "FrmUnsentMsgItem"+IntToStr(MsgCount+1);
	  DestroyFrmUnsentMsgItem.pszName = ItemName.w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyFrmUnsentMsgItem));
	  ItemName = "FrmUnsentMsgItem"+IntToStr(MsgCount+2);
	  DestroyFrmUnsentMsgItem.pszName = ItemName.w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyFrmUnsentMsgItem));
	}
	//Usuwanie buttona na FrmMain
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmMainUnsentMsgButton));
	//Usuwanie buttona na FrmSend
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmSendUnsentMsgButton));
  }
  delete Messages;
  delete Ini;
}
//---------------------------------------------------------------------------

//Tworzenie szybkiego dostepu do niewyslanych wiadomosci
void BuildFrmUnsentMsg()
{
  //Jezeli w ogole mamy tworzyc
  if((UnsentMsgChk)&&(FastAccessUnsentMsgChk))
  {
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *Messages = new TStringList;
	Ini->ReadSection("Messages",Messages);
	delete Ini;
	int MsgCount = Messages->Count;
	//Maks 5 elementow w popupmenu
	if(MsgCount>5) MsgCount = 5;
	//Jezeli w ogole cos jest
	if(MsgCount>0)
	{
	  //Tworzenie buttona na FrmMain
	  if(FrmMainUnsentMsgChk)
	  {
		FrmMainUnsentMsgButton.cbSize = sizeof(TPluginAction);
		FrmMainUnsentMsgButton.pszName = (wchar_t*) L"FrmMainUnsentMsgButton";
		FrmMainUnsentMsgButton.Position = 999;
		FrmMainUnsentMsgButton.IconIndex = UNSENTMSG;
		FrmMainUnsentMsgButton.pszPopupName = L"FrmUnsentMsgPopUp";
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmMainUnsentMsgButton));
	  }
	  //Tworzenie buttona na FrmSend
	  if((hFrmSend)&&(FrmSendUnsentMsgChk))
	  {
		FrmSendUnsentMsgButton.cbSize = sizeof(TPluginAction);
		FrmSendUnsentMsgButton.pszName = (wchar_t*) L"FrmSendUnsentMsgButton";
		FrmSendUnsentMsgButton.Hint = L"Niewys³ane wiadomoœci";
		FrmSendUnsentMsgButton.Position = 0;
		FrmSendUnsentMsgButton.IconIndex = UNSENTMSG;
		FrmSendUnsentMsgButton.pszPopupName = L"FrmUnsentMsgPopUp";
		FrmSendUnsentMsgButton.Handle = (unsigned int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmSendUnsentMsgButton));
	  }
	  //Tworzenie PopUpMenuItems
	  UnicodeString JID;
	  UnicodeString ItemName;
	  UnicodeString ItemJID;
	  UnicodeString ItemService;
	  for(int Count=0;Count<MsgCount;Count++)
	  {
		ItemJID = Messages->Strings[Count];
		if(!ItemJID.IsEmpty())
		{
          if(!ItemJID.Pos("ischat_"))
		  {
			BuildFrmUnsentMsgItem[Count].cbSize = sizeof(TPluginAction);
			BuildFrmUnsentMsgItem[Count].IconIndex = GetContactState(ItemJID);
			ItemName = "FrmUnsentMsgItem"+IntToStr(Count);
			BuildFrmUnsentMsgItem[Count].pszName = ItemName.w_str();
			ItemService = "sFrmUnsentMsgItem"+IntToStr(Count);
			BuildFrmUnsentMsgItem[Count].pszService = ItemService.w_str();
			BuildFrmUnsentMsgItem[Count].pszCaption = GetContactNick(ItemJID).w_str();
			BuildFrmUnsentMsgItem[Count].Position = Count;
			BuildFrmUnsentMsgItem[Count].pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem[Count]));
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
			BuildFrmUnsentMsgItem[Count].cbSize = sizeof(TPluginAction);
			BuildFrmUnsentMsgItem[Count].IconIndex = 79;
			ItemName = "FrmUnsentMsgItem"+IntToStr(Count);
			BuildFrmUnsentMsgItem[Count].pszName = ItemName.w_str();
			ItemService = "sFrmUnsentMsgItem"+IntToStr(Count);
			BuildFrmUnsentMsgItem[Count].pszService = ItemService.w_str();
			BuildFrmUnsentMsgItem[Count].pszCaption = Channel.w_str();
			BuildFrmUnsentMsgItem[Count].Position = Count;
			BuildFrmUnsentMsgItem[Count].pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem[Count]));
          }
        }
      }
	  //Tworzenie elementow do usuwania niewyslanych wiadomosci
	  if(FastClearUnsentMsgChk)
	  {
        //Tworzenie separatora
		BuildFrmUnsentMsgItem[MsgCount+1].cbSize = sizeof(TPluginAction);
		BuildFrmUnsentMsgItem[MsgCount+1].IconIndex = -1;
		ItemName = "FrmUnsentMsgItem"+IntToStr(MsgCount+1);
		BuildFrmUnsentMsgItem[MsgCount+1].pszName = ItemName.w_str();
		BuildFrmUnsentMsgItem[MsgCount+1].pszService = L"";
		BuildFrmUnsentMsgItem[MsgCount+1].pszCaption = L"-";
		BuildFrmUnsentMsgItem[MsgCount+1].Position = MsgCount+1;
		BuildFrmUnsentMsgItem[MsgCount+1].pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem[MsgCount+1]));
		//Tworzenie elementu czyszczenia
		BuildFrmUnsentMsgItem[MsgCount+2].cbSize = sizeof(TPluginAction);
		BuildFrmUnsentMsgItem[MsgCount+2].IconIndex = -1;
		ItemName = "FrmUnsentMsgItem"+IntToStr(MsgCount+2);
		BuildFrmUnsentMsgItem[MsgCount+2].pszName = ItemName.w_str();
		BuildFrmUnsentMsgItem[MsgCount+2].pszService = L"sFrmUnsentMsgItemClear";
		BuildFrmUnsentMsgItem[MsgCount+2].pszCaption = L"Wyczyœæ";
		BuildFrmUnsentMsgItem[MsgCount+2].Position = MsgCount+1;
		BuildFrmUnsentMsgItem[MsgCount+2].pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem[MsgCount+2]));
      }
	}
	delete Messages;
  }
  //Szybki dostep niewyslanych wiadomosci
  else
   DestroyFrmUnsentMsg();
}
//---------------------------------------------------------------------------

//Otwieranie zakladki z niewyslana wiadomoscia
void GetUnsentMsgItem(int Item)
{
  TIniFile *Ini = new TIniFile(SessionFileDir);
  TStringList *Messages = new TStringList;
  Ini->ReadSection("Messages",Messages);
  UnicodeString JID = Messages->Strings[Item];
  delete Messages;
  delete Ini;
  //Otwieranie zakladki z danym kontektem
  if(!JID.Pos("ischat_"))
   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
  //Otwieranie zakladki z czatem
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
	PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
	PluginChatPrep.UserIdx = 0;
	PluginChatPrep.JID = JID.w_str();
	PluginChatPrep.Channel = Channel.w_str();
	PluginChatPrep.CreateNew = false;
	PluginChatPrep.Fast = true;
	PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
  }
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

//Serwisy elementow niewysylanych wiadomosci
int __stdcall ServiceFrmUnsentMsgItemClear (WPARAM, LPARAM)
{
  EraseUnsentMsg();
  return 0;
}
int __stdcall ServiceFrmUnsentMsgItem0 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(0);
  return 0;
}
int __stdcall ServiceFrmUnsentMsgItem1 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(1);
  return 0;
}
int __stdcall ServiceFrmUnsentMsgItem2 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(2);
  return 0;
}
int __stdcall ServiceFrmUnsentMsgItem3 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(3);
  return 0;
}
int __stdcall ServiceFrmUnsentMsgItem4 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(4);
  return 0;
}
//---------------------------------------------------------------------------

//Usuwanie interfejsu dla ostatio zamknietych zakladek
void DestroyFrmClosedTabs()
{
  int TabsCount = ClosedTabsList->Count;
  //Maks X elementow w popupmenu
  if(TabsCount>ItemCountUnCloseTabVal) TabsCount = ItemCountUnCloseTabVal;
  //Jezeli cos w ogule jest
  if(TabsCount>0)
  {
	UnicodeString ItemName;
	//Usuwanie elementow popupmenu
	for(int Count=0;Count<TabsCount;Count++)
	{
	  ItemName = "FrmClosedTabsItem"+IntToStr(Count);
	  DestroyFrmClosedTabsItem.pszName = ItemName.w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyFrmClosedTabsItem));
	}
	//Usuwanie elementow do usuwania ostatnio zamknietych zakladek
	if(FastClearClosedTabsChk)
	{
	  ItemName = "FrmClosedTabsItem"+IntToStr(TabsCount+1);
	  DestroyFrmClosedTabsItem.pszName = ItemName.w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyFrmClosedTabsItem));
	  ItemName = "FrmClosedTabsItem"+IntToStr(TabsCount+2);
	  DestroyFrmClosedTabsItem.pszName = ItemName.w_str();
	  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&DestroyFrmClosedTabsItem));
	}
	//Usuwanie buttona na FrmMain
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmMainClosedTabsButton));
	//Usuwanie buttona na FrmSend
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmSendClosedTabsButton));
  }
}
//---------------------------------------------------------------------------

//Tworzenie interfejsu dla ostatnio zamknietych zakladek
void BuildFrmClosedTabs()
{
  //Jezeli w ogole mamy tworzyc
  if((ClosedTabsChk)&&(FastAccessClosedTabsChk))
  {
	int TabsCount = ClosedTabsList->Count;
	//Maks X elementow w popupmenu
	if(TabsCount>ItemCountUnCloseTabVal) TabsCount = ItemCountUnCloseTabVal;
	//Jezeli w ogole cos jest
	if(TabsCount>0)
	{
	  //Tworzenie buttona na FrmMain
	  if(FrmMainClosedTabsChk)
	  {
		FrmMainClosedTabsButton.cbSize = sizeof(TPluginAction);
		FrmMainClosedTabsButton.pszName = (wchar_t*) L"FrmMainClosedTabsButton";
		FrmMainClosedTabsButton.Position = 999;
		FrmMainClosedTabsButton.IconIndex = CLOSEDTABS;
		FrmMainClosedTabsButton.pszPopupName = L"FrmClosedTabsPopUp";
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmMainClosedTabsButton));
	  }
	  //Tworzenie buttona na FrmSend
	  if((hFrmSend)&&(FrmSendClosedTabsChk))
	  {
		FrmSendClosedTabsButton.cbSize = sizeof(TPluginAction);
		FrmSendClosedTabsButton.pszName = (wchar_t*) L"FrmSendClosedTabsButton";
		FrmSendClosedTabsButton.Hint = L"Ostatnio zamkniête zak³adki";
		FrmSendClosedTabsButton.Position = 0;
		FrmSendClosedTabsButton.IconIndex = CLOSEDTABS;
		FrmSendClosedTabsButton.pszPopupName = L"FrmClosedTabsPopUp";
		FrmSendClosedTabsButton.Handle = (unsigned int)hFrmSend;
		PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&FrmSendClosedTabsButton));
	  }
	  //Tworzenie PopUpMenuItems
	  UnicodeString ItemName;
	  UnicodeString ItemJID;
	  UnicodeString ItemService;
	  for(int Count=0;Count<TabsCount;Count++)
	  {
		ItemJID = ClosedTabsList->Strings[Count];
		if(!ItemJID.IsEmpty())
		{
		  if(!ItemJID.Pos("ischat_"))
		  {
			BuildFrmClosedTabsItem[Count].cbSize = sizeof(TPluginAction);
			BuildFrmClosedTabsItem[Count].IconIndex = GetContactState(ItemJID);
			ItemName = "FrmClosedTabsItem"+IntToStr(Count);
			BuildFrmClosedTabsItem[Count].pszName = ItemName.w_str();
			ItemService = "sFrmClosedTabsItem"+IntToStr(Count);
			BuildFrmClosedTabsItem[Count].pszService = ItemService.w_str();
			if(ShowTimeClosedTabsChk)
			 BuildFrmClosedTabsItem[Count].pszCaption = (GetContactNick(ItemJID)+" ("+ClosedTabsTimeList->Strings[Count]+")").w_str();
			else
			 BuildFrmClosedTabsItem[Count].pszCaption = GetContactNick(ItemJID).w_str();
			BuildFrmClosedTabsItem[Count].Position = Count;
			BuildFrmClosedTabsItem[Count].pszPopupName = (wchar_t*) L"FrmClosedTabsPopUp";
			PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmClosedTabsItem[Count]));
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
			BuildFrmClosedTabsItem[Count].cbSize = sizeof(TPluginAction);
			BuildFrmClosedTabsItem[Count].IconIndex = 79;
			ItemName = "FrmClosedTabsItem"+IntToStr(Count);
			BuildFrmClosedTabsItem[Count].pszName = ItemName.w_str();
			ItemService = "sFrmClosedTabsItem"+IntToStr(Count);
			BuildFrmClosedTabsItem[Count].pszService = ItemService.w_str();
			if(ShowTimeClosedTabsChk)
			 BuildFrmClosedTabsItem[Count].pszCaption = (Channel+" ("+ClosedTabsTimeList->Strings[Count]+")").w_str();
			else
			 BuildFrmClosedTabsItem[Count].pszCaption = Channel.w_str();
			BuildFrmClosedTabsItem[Count].Position = Count;
			BuildFrmClosedTabsItem[Count].pszPopupName = (wchar_t*) L"FrmClosedTabsPopUp";
			PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmClosedTabsItem[Count]));
          }
		}
	  }
	  //Tworzenie elementow do usuwania ostatnio zamknietych zakladek
	  if(FastClearClosedTabsChk)
	  {
		//Tworzenie separatora
		BuildFrmClosedTabsItem[TabsCount+1].cbSize = sizeof(TPluginAction);
		BuildFrmClosedTabsItem[TabsCount+1].IconIndex = -1;
		ItemName = "FrmClosedTabsItem"+IntToStr(TabsCount+1);
		BuildFrmClosedTabsItem[TabsCount+1].pszName = ItemName.w_str();
		BuildFrmClosedTabsItem[TabsCount+1].pszService = L"";
		BuildFrmClosedTabsItem[TabsCount+1].pszCaption = L"-";
		BuildFrmClosedTabsItem[TabsCount+1].Position = TabsCount+1;
		BuildFrmClosedTabsItem[TabsCount+1].pszPopupName = (wchar_t*) L"FrmClosedTabsPopUp";
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmClosedTabsItem[TabsCount+1]));
		//Tworzenie elementu czyszczenia
		BuildFrmClosedTabsItem[TabsCount+2].cbSize = sizeof(TPluginAction);
		BuildFrmClosedTabsItem[TabsCount+2].IconIndex = -1;
		ItemName = "FrmClosedTabsItem"+IntToStr(TabsCount+2);
		BuildFrmClosedTabsItem[TabsCount+2].pszName = ItemName.w_str();
		BuildFrmClosedTabsItem[TabsCount+2].pszService = L"sFrmClosedTabsItemClear";
		BuildFrmClosedTabsItem[TabsCount+2].pszCaption = L"Wyczyœæ";
		BuildFrmClosedTabsItem[TabsCount+2].Position = TabsCount+1;
		BuildFrmClosedTabsItem[TabsCount+2].pszPopupName = (wchar_t*) L"FrmClosedTabsPopUp";
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmClosedTabsItem[TabsCount+2]));
	  }
	}
  }
  //Szybki dostep niewyslanych wiadomosci
  else
   DestroyFrmClosedTabs();
}
//---------------------------------------------------------------------------

//Tworzenie interfejsu w AQQ dla ostatnio zamknietych zakladek
void BuildAQQClosedTabs()
{
   //Jezeli w ogole mamy tworzyc
  if(ClosedTabsChk)
  {
	int TabsCount = ClosedTabsList->Count;
	//Maks 3 zdefiniowanych elementów
	if(TabsCount>3) TabsCount = 3;
	//Jezeli w ogole cos jest
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
  if(ClosedTabsChk)
  {
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *ClosedTabs = new TStringList;
	Ini->ReadSection("ClosedTabs",ClosedTabs);
	int TabsCount = ClosedTabs->Count;
	//Maks X zdefiniowanych elementów
	if(TabsCount>CountUnCloseTabVal) TabsCount = CountUnCloseTabVal;
	if(TabsCount>0)
	{
	  ClosedTabsList->Clear();
	  ClosedTabsTimeList->Clear();
	  for(int Count=0;Count<TabsCount;Count++)
	  {
		UnicodeString JID = Ini->ReadString("ClosedTabs","Tab"+IntToStr(Count+1),"");
		if(!JID.IsEmpty())
		{
		  ClosedTabsList->Add(JID);
		  UnicodeString ClosedTime = Ini->ReadString("ClosedTabs","Tab"+IntToStr(Count+1)+"Time","b/d");
		  ClosedTabsTimeList->Add(ClosedTime);
		}
	  }
	}
	else
	{
	  ClosedTabsList->Clear();
	  ClosedTabsTimeList->Clear();
	}
	delete ClosedTabs;
	delete Ini;
  }
}
//---------------------------------------------------------------------------

//Zapisywanie ostatnio zamknietych zakladek do pliku
void SaveClosedTabs()
{
  if(ClosedTabsChk)
  {
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
	GetClosedTabs();
  }
}
//---------------------------------------------------------------------------

//Otwieranie zakladki z niewyslana wiadomoscia
void GetClosedTabsItem(int Item)
{
  UnicodeString JID = ClosedTabsList->Strings[Item];
  //Zapisywanie JID aktualnie przywracanej zakladki
  JustUnClosedJID = JID;
  //Otwieranie zakladki z danym kontektem
  if(!JID.Pos("ischat_"))
   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
  //Otwieranie zakladki z czatem
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
	PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
	PluginChatPrep.UserIdx = 0;
	PluginChatPrep.JID = JID.w_str();
	PluginChatPrep.Channel = Channel.w_str();
	PluginChatPrep.CreateNew = false;
	PluginChatPrep.Fast = true;
	PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
  }
}
//---------------------------------------------------------------------------

//Usuwanie listy ostatnio zamknietych zakladek
void EraseClosedTabs()
{
  //Usuwanie interfejsu
  DestroyFrmClosedTabs();
  //Usuwanie sesji z pliku
  TIniFile *Ini = new TIniFile(SessionFileDir);
  Ini->EraseSection("ClosedTabs");
  delete Ini;
  //Czyszczenie listy
  ClosedTabsList->Clear();
  ClosedTabsTimeList->Clear();
}
//---------------------------------------------------------------------------

//Serwisy elementow ostatnio zamknietych zakladek
int __stdcall ServiceFrmClosedTabsItemClear (WPARAM, LPARAM)
{
  EraseClosedTabs();
  return 0;
}
int __stdcall ServiceFrmClosedTabsItem0 (WPARAM, LPARAM)
{
  GetClosedTabsItem(0);
  return 0;
}
int __stdcall ServiceFrmClosedTabsItem1 (WPARAM, LPARAM)
{
  GetClosedTabsItem(1);
  return 0;
}
int __stdcall ServiceFrmClosedTabsItem2 (WPARAM, LPARAM)
{
  GetClosedTabsItem(2);
  return 0;
}
int __stdcall ServiceFrmClosedTabsItem3 (WPARAM, LPARAM)
{
  GetClosedTabsItem(3);
  return 0;
}
int __stdcall ServiceFrmClosedTabsItem4 (WPARAM, LPARAM)
{
  GetClosedTabsItem(4);
  return 0;
}
int __stdcall ServiceFrmClosedTabsItem5 (WPARAM, LPARAM)
{
  GetClosedTabsItem(5);
  return 0;
}
int __stdcall ServiceFrmClosedTabsItem6 (WPARAM, LPARAM)
{
  GetClosedTabsItem(6);
  return 0;
}
int __stdcall ServiceFrmClosedTabsItem7 (WPARAM, LPARAM)
{
  GetClosedTabsItem(7);
  return 0;
}
int __stdcall ServiceFrmClosedTabsItem8 (WPARAM, LPARAM)
{
  GetClosedTabsItem(8);
  return 0;
}
int __stdcall ServiceFrmClosedTabsItem9 (WPARAM, LPARAM)
{
  GetClosedTabsItem(9);
  return 0;
}
//---------------------------------------------------------------------------

//Sprawdzanie niewyslanyc wiadomosci
void GetUnsentMsg()
{
  if((UnsentMsgChk)&&(InfoUnsentMsgChk))
  {
	TIniFile *Ini = new TIniFile(SessionFileDir);
	TStringList *Messages = new TStringList;
	Ini->ReadSection("Messages",Messages);
	int MsgCount = Messages->Count;
	if(MsgCount>0)
	{
	  UnicodeString JID;
	  UnicodeString Hint;
	  UnicodeString Body;

	  if(MsgCount==1)
		Hint = "Masz 1 niewys³an¹ wiadomoœæ!";
	  else if(MsgCount>1&&MsgCount<5)
		Hint = "Masz " + IntToStr(MsgCount) + " niewys³ane wiadomoœci!";
	  else
	   Hint = "Masz " + IntToStr(MsgCount) + " niewys³anych wiadomoœci!";

	  //Informacja w chmurce
	  if(CloudUnsentMsgChk)
	  {
		PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
		PluginShowInfo.Event = tmeInfo;
		PluginShowInfo.Text = Hint.w_str();
		PluginShowInfo.ImagePath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,21,0));
		PluginShowInfo.TimeOut = 1000 * CloudTimeOutChk;
		PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));

		//Szczegolowa informacja
		if(DetailedCloudUnsentMsgChk)
		{
		  for(int Count=0;Count<MsgCount;Count++)
		  {
			JID = Messages->Strings[Count];
			Body = UTF8ToUnicodeString(IniStrToStr(Ini->ReadString("Messages", JID, "")).t_str());
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
			PluginShowInfo.ImagePath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0));
			PluginShowInfo.TimeOut = 1000 * CloudTimeOutChk;
			PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
		  }
		}
	  }
	  //Informacja w trayu
	  if(TrayUnsentMsgChk)
	  {
		if(!hSettingsForm)
		{
		  Application->Handle = (HWND)SettingsForm;
		  hSettingsForm = new TSettingsForm(Application);
		}
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
void ShowUnsentMsg()
{
  TIniFile *Ini = new TIniFile(SessionFileDir);
  TStringList *Messages = new TStringList;
  Ini->ReadSection("Messages",Messages);
  int MsgCount = Messages->Count;
  if(MsgCount>0)
  {
	UnicodeString JID;
	for(int Count=0;Count<MsgCount;Count++)
	{
	  JID = Messages->Strings[Count];
	  //Otwieranie zakladki z danym kontektem
	  if(!JID.Pos("ischat_"))
	   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
	  //Otwieranie zakladki z czatem
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
		PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
		PluginChatPrep.UserIdx = 0;
		PluginChatPrep.JID = JID.w_str();
		PluginChatPrep.Channel = Channel.w_str();
		PluginChatPrep.CreateNew = false;
		PluginChatPrep.Fast = true;
		PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
	  }
	}
  }
  delete Messages;
  delete Ini;
}
//---------------------------------------------------------------------------

VOID CALLBACK Timer(HWND hwnd, UINT msg, UINT_PTR idEvent, DWORD dwTime)
{
  switch(msg)
  {
	case WM_TIMER:
	{
	  //StayOnTop
	  if(idEvent==100)
	  {
		//Szukanie okna wysylania wycinka
		hwnd = FindWindow("TfrmPos",NULL);
		if(hwnd)
		{
		  SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
		  KillTimer(hFrmMain,100);
		}
	  }
	  //InactiveNotifer
	  if(idEvent==200)
	  {
		BlockInactiveNotiferNewMsg = false;
		KillTimer(hFrmMain,200);
	  }
	  //Pokazywanie paska narzedzi
	  if(idEvent==300)
	  {
		POINT pCur;
		GetCursorPos(&pCur);
		HWND hCurActiveFrm = WindowFromPoint(pCur);
		//Jezeli okno rozmowy jest aktywne
		if((GetForegroundWindow()==hFrmSend)&&((hCurActiveFrm==hFrmSend)||(IsChild(hFrmSend,hCurActiveFrm))))
		{
          //Wysokosc paska narzedzi
		  GetWindowRect(hToolBar,&WindowRect);
		  if(!WindowRect.Height())
		  {
			//Pobieranie pozycji okna rozmowy
			GetWindowRect(hFrmSend,&WindowRect);
			//Pokazanie paska
			SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),25,SWP_NOMOVE);
			//Oswiezenia okna
			SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
			SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
		  }
		}
		//Zatrzymanie timera
		KillTimer(hFrmMain,300);
	  }
	  //Przywracanie sesji z czatami
	  if(idEvent==400)
	  {
		//Sprawdzenie stanu glownego konta
		PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);
		PluginStateChange.cbSize = sizeof(TPluginStateChange);
		if(PluginStateChange.NewState!=0)
		{
          //Jezeli sa jakies zakladki czatowe do przywrocenia
		  if(ChatSessionList->Count>0)
		  {
			for(int Count=0;Count<ChatSessionList->Count;Count++)
			{
			  UnicodeString JID = ChatSessionList->Strings[Count];
			  TIniFile *Ini = new TIniFile(SessionFileDir);
			  UnicodeString Channel = Ini->ReadString("Channels",JID,"");
			  delete Ini;
			  if(Channel.IsEmpty())
			  {
				Channel = JID;
				Channel = Channel.Delete(Channel.Pos("@"),Channel.Length());
			  }
			  PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
			  PluginChatPrep.UserIdx = 0;
			  PluginChatPrep.JID = JID.w_str();
			  PluginChatPrep.Channel = Channel.w_str();
			  PluginChatPrep.CreateNew = false;
			  PluginChatPrep.Fast = true;
			  PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
			}
			ChatSessionList->Clear();
		  }
		  //Zatrzymanie timera
		  KillTimer(hFrmMain,400);
		}
		//Zatrzymanie timera
		else if(!ChatSessionList->Count)
		 KillTimer(hFrmMain,400);
	  }
	  //Zmiana pozycji nowo otwartej przypietej zakladki
	  if(idEvent==500)
	  {
		if(ClipTabsList->IndexOf(ActiveTabJID)!=-1)
		{
		  //Zmiana miejsca zakladki
		  if(!ActiveTabJID.Pos("ischat_"))
		  {
			int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,0,(LPARAM)ActiveTabJID.w_str());
			if(Index)
			{
			  TPluginTriple PluginTriple;
			  PluginTriple.Handle1 = (unsigned int)hFrmSend;
			  PluginTriple.Param1 = Index;
			  PluginTriple.Param2 = 0;
			  PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
			}
		  }
		  else
		  {
			UnicodeString ClipTabW = ActiveTabJID;
			ClipTabW = ClipTabW.Delete(1,7);
			int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,2,(LPARAM)ClipTabW.w_str());
			if(Index)
			{
			  TPluginTriple PluginTriple;
			  PluginTriple.Handle1 = (unsigned int)hFrmSend;
			  PluginTriple.Param1 = Index;
			  PluginTriple.Param2 = 0;
			  PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
			}
		  }
		}
		//Zatrzymanie timera
		KillTimer(hFrmMain,500);
	  }
	  //Otwieranie przypietych zakladek wraz z oknem rozmowy
	  if(idEvent==600)
	  {
		if(ClipTabsList->Count)
		{
		  UnicodeString ActiveTab = ActiveTabJID;
		  //Otwieranie przypietych zakladek
		  for(int Count=0;Count<ClipTabsList->Count;Count++)
		  {
			UnicodeString JID = ClipTabsList->Strings[Count];
			//Otwieranie zakladki z danym kontektem
			if(!JID.Pos("ischat_"))
			 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
			//Otwieranie zakladki z czatem
			else if(!JID.Pos("@plugin"))
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
			  PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
			  PluginChatPrep.UserIdx = 0;
			  PluginChatPrep.JID = JID.w_str();
			  PluginChatPrep.Channel = Channel.w_str();
			  PluginChatPrep.CreateNew = false;
			  PluginChatPrep.Fast = true;
			  PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
			}
		  }
		  //Zmiana aktywnej zakladki
		  if(!ActiveTab.Pos("ischat_"))
		   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)ActiveTab.w_str());
		  else
		  {
			UnicodeString ActiveTabW = ActiveTab;
			ActiveTabW = ActiveTabW.Delete(1,7);
			PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)ActiveTabW.w_str());
		  }
		  //Zmiana pozycji aktywnej zakladki
		  //Ponieranie ilosci zakladek
		  TPluginTriple PluginTriple;
		  PluginTriple.Handle1 = (unsigned int)hFrmSend;
		  int Count = PluginLink.CallService(AQQ_FUNCTION_TABCOUNT,(WPARAM)&PluginTriple,0);
		  if(!ActiveTab.Pos("ischat_"))
		  {
			int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,0,(LPARAM)ActiveTab.w_str());
			PluginTriple.Param1 = Index;
			PluginTriple.Param2 = Count-1;
			PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
		  }
		  else
		  {
			UnicodeString ActiveTabW = ActiveTab;
			ActiveTabW = ActiveTab.Delete(1,7);
			int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,2,(LPARAM)ActiveTab.w_str());
			PluginTriple.Param1 = Index;
			PluginTriple.Param2 = Count-1;
			PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
		  }
		}
		//Zatrzymanie timera
		KillTimer(hFrmMain,600);
	  }

	  break;
	}
  }
}
//---------------------------------------------------------------------------

LRESULT CALLBACK FrmSendProc(HWND hwnd, UINT mesg, WPARAM wParam, LPARAM lParam)
{
  //Blokowanie zmiany tekstu na belce okna
  if(mesg==WM_SETICON)
  {
	if((InactiveFrmNewMsgChk)&&(InactiveFrmNewMsgCount))
	{
	  //Jezeli okno rozmowy jest nie aktywne
	  if(GetForegroundWindow()!=hFrmSend)
	   SetWindowTextW(hFrmSend,("["+IntToStr(InactiveFrmNewMsgCount)+"] "+TempTitlebar).w_str());
	}
  }
  if(mesg==WM_ACTIVATE)
  {
	//Zmiana tekstu na belce okna
	if(InactiveFrmNewMsgChk)
	{
	  //Okno aktywne
	  if((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE))
	  {
		//Przywracanie poprzedniego stanu titlebara
		if(!TempTitlebar.IsEmpty())
		{
		  SetWindowTextW(hFrmSend,TempTitlebar.w_str());
		  TempTitlebar = "";
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
		if(hIconSmall)
		{
		  SendMessage(hFrmSend, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
		  hIconSmall = NULL;
		  SendMessage(hFrmSend, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIconBig);
		  hIconBig = NULL;
		}
	  }
	}
	if(FrmSendOpening)
	{
	  FrmSendOpening = false;
	  //Maksymalizowanie okna
	  if(FrmSendMaximized)
	  {
		ShowWindow(hFrmSend,SW_MAXIMIZE);
	  }
	  //Ukrywanie paska informacyjnego
	  if(HideStatusBarChk)
	  {
		hStatusBarPro = FindWindowEx(hFrmSend,NULL,"TStatusBarPro",NULL);
		if(hStatusBarPro)
		{
		  SetWindowPos(hStatusBarPro,NULL,0,0,0,0,SWP_NOMOVE);
		  ShowWindow(hStatusBarPro,SW_HIDE);
		}
	  }
	  //Ukrywanie paska narzedzi
	  if(HideToolBarChk)
	  {
		//Pobieranie pozycji okna rozmowy
		GetWindowRect(hFrmSend,&WindowRect);
		//Ukrycie paska
		SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
		//Odswiezenie okna rozmowy
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
	  }
	}
  }
  //Zabezpieczenie przed zamykaniem wielu zakladek jednoczesnie
  if((EmuTabsWSupport)&&(EmuTabsWChk))
  {
	if((mesg==WM_CLOSE)&&(TabsList->Count>=2))
	{
	  if(Application->MessageBox(
	  ("Czy chcesz zamkn¹æ wszystkie otwarte zak³adki?"
	  "\n\nIloœæ otwartych zak³adek: " + IntToStr(TabsList->Count)).w_str(),
	  L"Pytanie o zak³adki",
	  MB_OKCANCEL | MB_ICONASTERISK)==IDOK)
	  {
		return CallWindowProc(OldFrmSendProc, hwnd, mesg, wParam, lParam);
	  }
	  else
	   return 1;
	}
  }
  //"Anty maksymalizacja" okna
  if(mesg==WM_SIZE)
  {
	if((wParam==WM_SIZE)||(wParam==SIZE_MAXIMIZED))
	 FrmSendMaximized = !FrmSendMaximized;
	else if((wParam==WM_SIZE)||(wParam==SIZE_RESTORED))
	 if(!FrmSendOpening) FrmSendMaximized = false;
  }
  //Ukrywanie paska narzedzi
  if(mesg==WM_SETCURSOR)
  {
	if(HideToolBarChk)
	{
      POINT pCur;
	  GetCursorPos(&pCur);
	  HWND hCurActiveFrm = WindowFromPoint(pCur);
	  //Jezeli okno rozmowy jest aktywne
	  if((GetForegroundWindow()==hFrmSend)&&((hCurActiveFrm==hFrmSend)||(IsChild(hFrmSend,hCurActiveFrm))))
	  {
		//Pobieranie pozycji okna rozmowy
		GetWindowRect(hFrmSend,&WindowRect);
		int WindowBottom = WindowRect.Bottom;
		POINT pCur;
		GetCursorPos(&pCur);
		int CursorY = pCur.y;
		//Pozycja kursora w oknie rozmowy
		int CursorPos = WindowBottom - CursorY;
		//Wysokoœæ paska informacyjnego
		if(!hStatusBarPro) hStatusBarPro = FindWindowEx(hFrmSend,NULL,"TStatusBarPro",NULL);
		GetWindowRect(hStatusBarPro,&WindowRect);
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
			  KillTimer(hFrmMain,300);
			  SetTimer(hFrmMain,300,500,(TIMERPROC)Timer);
			}
		  }
		}
		//Ukrywanie paska narzedzi
		else
		{
		  if(ToolBarShowing)
		  {
			//Zatrzymanie timera
			KillTimer(hFrmMain,300);
			ToolBarShowing = false;
		  }
          //Wysokosc paska narzedzi
		  GetWindowRect(hToolBar,&WindowRect);
		  if(WindowRect.Height())
		  {
			//Pobieranie pozycji okna rozmowy
			GetWindowRect(hFrmSend,&WindowRect);
			//Ukrycie paska
			SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
			//Odswiezenie okna
			//GetWindowRect(hFrmSend,&WindowRect);
			SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
			SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
		  }
		}
	  }
	}
  }

  return CallWindowProc(OldFrmSendProc, hwnd, mesg, wParam, lParam);
}
//---------------------------------------------------------------------------

int __stdcall OnContactsUpdate (WPARAM wParam, LPARAM lParam)
{
  Contact = (PPluginContact)wParam;
  if(!Contact->IsChat)
  {
	UnicodeString JID = (wchar_t*)Contact->JID;
    //Pobieranie i zapisywanie stanu kontaktu
	int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(Contact));
	ContactsStateList->WriteInteger("State",JID,State);
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

  return 0;
}
//---------------------------------------------------------------------------

int __stdcall OnActiveTab (WPARAM wParam, LPARAM lParam);
int __stdcall OnTabCaption (WPARAM wParam, LPARAM lParam);
//Pobieranie listy wszystkich otartych zakladek/okien
int __stdcall OnFetchAllTabs (WPARAM wParam, LPARAM lParam)
{
  if((wParam)&&(lParam))
  {
	Contact = (PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	if(Contact->IsChat) JID = "ischat_" + JID;
	//Dodawanie JID do listy otwartych zakladek
	if(TabsList->IndexOf(JID)==-1)
	 TabsList->Add(JID);
	//Pobieranie stanu kontaktu
	if(!Contact->IsChat)
	{
	  int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(Contact));
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
	//Jezeli zakladka jest przypieta
	if(ClipTabsList->IndexOf(JID)!=-1)
	{
	  //"Ustawianie" pustego tekstu na przypietej zakladce
	  PluginLink.UnhookEvent(OnTabCaption);
	  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)L"",(LPARAM)Contact);
	  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
	  //Zmiana ikonki na zakladce
	  if((JID=="blip@blip.pl")||(JID.Pos("202@plugin.gg")>0))
	  {
		PluginContact.cbSize = sizeof(TPluginContact);
		PluginContact.JID = JID.w_str();
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)132,(LPARAM)&PluginContact);
	  }
	  else if(!JID.Pos("ischat_"))
	  {
		int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID,0);
		//Ikona juz w interfejsie AQQ
		if(Icon)
		{
		  PluginContact.cbSize = sizeof(TPluginContact);
		  PluginContact.JID = JID.w_str();
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&PluginContact);
		}
		//Ikona jeszcze niezaladowana do interfejsu AQQ
		else
		{
		  //Jezeli plik PNG jest juz wygenerowany
		  if(FileExists(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
		  {
			Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
			ClipTabsIconList->WriteInteger("ClipTabsIcon",JID,Icon);
			PluginContact.cbSize = sizeof(TPluginContact);
			PluginContact.JID = JID.w_str();
			PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&PluginContact);
		  }
		  //Generowanie pliku PNG 16x16 z awataru kontaktu
		  else
		  {
			TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
			if(!hSettingsForm)
			{
			  Application->Handle = (HWND)SettingsForm;
			  hSettingsForm = new TSettingsForm(Application);
			}
			UnicodeString Avatar = hSettingsForm->IdDecoderMIME->DecodeString(Ini->ReadString("Other","Avatar",""));
			delete Ini;
			if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
			{
			  Avatar = StringReplace(Avatar, "{PROFILEPATH}\\Data\\Avatars\\", "", TReplaceFlags());
			  Avatar = GetAvatarsUserDir() + Avatar;
			  hSettingsForm->ConvertImage(Avatar,GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+JID+".png");
			  if(FileExists(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
			  {
				Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
				ClipTabsIconList->WriteInteger("ClipTabsIcon",JID,Icon);
				PluginContact.cbSize = sizeof(TPluginContact);
				PluginContact.JID = JID.w_str();
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&PluginContact);
			  }
			}
		  }
		}
	  }
	}
	if(!hFrmSend)
	{
	  //Przypisanie uchwytu okna rozmowy
	  hFrmSend = (HWND)wParam;
	  //Szukanie pola wiadomosci
	  EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	  //Szukanie paska narzedzi
	  EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
	  //Przypisanie nowej procki dla okna rozmowy
	  OldFrmSendProc = (WNDPROC)SetWindowLongPtrW(hFrmSend, GWL_WNDPROC,(LONG)FrmSendProc);
	  //Ukrywanie paska informacyjnego
	  if(HideStatusBarChk)
	  {
        //Pobieranie pozycji okna rozmowy
		GetWindowRect(hFrmSend,&WindowRect);
		//Ukrycie paska
		if(!hStatusBarPro) hStatusBarPro = FindWindowEx(hFrmSend,NULL,"TStatusBarPro",NULL);
		SetWindowPos(hStatusBarPro,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
		ShowWindow(hStatusBarPro,SW_HIDE);
	  }
	  //Ukrycie paska narzedzi
	  if(HideToolBarChk)
	  {
		//Pobieranie pozycji okna rozmowy
		GetWindowRect(hFrmSend,&WindowRect);
		//Ukrycie paska narzedzi
		if(!hToolBar) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
		SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
	  }
	  if((HideStatusBarChk)||(HideToolBarChk))
	  {
		//Pobieranie pozycji okna rozmowy
		GetWindowRect(hFrmSend,&WindowRect);
		//Odswiezenie okna rozmowy
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na aktywna zakladke
int __stdcall OnPrimaryTab (WPARAM wParam, LPARAM lParam)
{
  Contact = (PPluginContact)lParam;
  UnicodeString JID = (wchar_t*)(Contact->JID);
  if(Contact->IsChat) JID = "ischat_" + JID;
  ActiveTabJID = JID;
  if(!hFrmSend)
  {
	//Przypisanie uchwytu okna rozmowy
	hFrmSend = (HWND)wParam;
	//Szukanie pola wiadomosci
	EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	//Szukanie paska narzedzi
	EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
	//Przypisanie nowej procki dla okna rozmowy
	OldFrmSendProc = (WNDPROC)SetWindowLongPtrW(hFrmSend, GWL_WNDPROC,(LONG)FrmSendProc);
	//Ukrywanie paska informacyjnego
	if(HideStatusBarChk)
	{
	  //Pobieranie pozycji okna rozmowy
	  GetWindowRect(hFrmSend,&WindowRect);
	  //Ukrycie paska
	  if(!hStatusBarPro) hStatusBarPro = FindWindowEx(hFrmSend,NULL,"TStatusBarPro",NULL);
	  SetWindowPos(hStatusBarPro,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
	  ShowWindow(hStatusBarPro,SW_HIDE);
	}
	//Ukrycie paska narzedzi
	if(HideToolBarChk)
	{
	  //Pobieranie pozycji okna rozmowy
	  GetWindowRect(hFrmSend,&WindowRect);
	  //Ukrycie paska narzedzi
	  if(!hToolBar) EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
	  SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),0,SWP_NOMOVE);
	}
	if((HideStatusBarChk)||(HideToolBarChk))
	{
	  //Pobieranie pozycji okna rozmowy
	  GetWindowRect(hFrmSend,&WindowRect);
	  //Odswiezenie okna rozmowy
	  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
	  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
	}
  }
  //Zmiana caption okna rozmowy
  if(!Contact->IsChat)
  {
	UnicodeString Nick = (wchar_t*)(Contact->Nick);
	UnicodeString Status = (wchar_t*)(Contact->Status);
	Status = StringReplace(Status, "\n", " ", TReplaceFlags() << rfReplaceAll);
	if(TweakFrmSendTitlebarChk)
	{
	  if(TweakFrmSendTitlebarMode==1)
	  {
		if(!Status.IsEmpty())
		 SetWindowTextW(hFrmSend,(Nick + " - " + Status).w_str());
		else
		 SetWindowTextW(hFrmSend,Nick.w_str());
	  }
	  else if(TweakFrmSendTitlebarMode==2)
	   SetWindowTextW(hFrmSend,Nick.w_str());
	  else
	   SetWindowTextW(hFrmSend,(Nick + " - " + JID).w_str());
	}
	else
	{
	  if(!Status.IsEmpty())
	   SetWindowTextW(hFrmSend,(Nick + " - " + JID + " - " + Status).w_str());
	  else
	   SetWindowTextW(hFrmSend,(Nick + " - " + JID).w_str());
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Zmiana titlebar'a FrmSend
void ChangeFrmSendTitlebar()
{
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_PRIMARYTAB,OnPrimaryTab);
  PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
  PluginLink.UnhookEvent(OnPrimaryTab);
}
//---------------------------------------------------------------------------

void ChangeFrmMainTitlebar()
{
  if(TweakFrmMainTitlebarChk)
  {
	if((TweakFrmMainTitlebarMode==1)&&(!ActiveProfileName.IsEmpty()))
	{
	  SetWindowTextW(hFrmMain,("AQQ [" + ActiveProfileName + "]").w_str());
	  SetWindowTextW(hFrmMainL,("AQQ [" + ActiveProfileName + "]").w_str());
	}
	else if(TweakFrmMainTitlebarMode==2)
	{
	  if(!TweakFrmMainTitlebarText.IsEmpty())
	  {
		SetWindowTextW(hFrmMain,("AQQ " + TweakFrmMainTitlebarText).w_str());
		SetWindowTextW(hFrmMainL,("AQQ " + TweakFrmMainTitlebarText).w_str());
	  }
	  else
	  {
		SetWindowTextW(hFrmMain,L"AQQ");
		SetWindowTextW(hFrmMainL,L"AQQ");
	  }
	}
  }
  else
  {
	SetWindowTextW(hFrmMain,OryginalTitlebar);
	SetWindowTextW(hFrmMainL,OryginalTitlebar);
  }
}
//---------------------------------------------------------------------------

int __stdcall ServiceStayOnTopItem (WPARAM wParam, LPARAM lParam)
{
  //Set TOPMOST
  if(!SetStayOnTop)
  {
	//Aktualizacja przycisku
	BuildStayOnTopItem.IconIndex = STAYONTOP_ON;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&BuildStayOnTopItem));
	//Okno rozmowy na wierzchu
	SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	//Stan SetStayOnTop
	SetStayOnTop=true;
  }
  //Set NOTOPMOST
  else
  {
	//Aktualizacja przycisku
	BuildStayOnTopItem.IconIndex = STAYONTOP_OFF;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&BuildStayOnTopItem));
	//Przywrocenie "normalnosci" okna
	SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
    //Stan SetStayOnTop
	SetStayOnTop=false;
  }

  return 0;
}
//---------------------------------------------------------------------------

void DestroyStayOnTop()
{
  PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&BuildStayOnTopItem));
}
//---------------------------------------------------------------------------

void BuildStayOnTop()
{
  if((hFrmSend)&&(StayOnTopChk))
  {
	BuildStayOnTopItem.cbSize = sizeof(TPluginAction);
	BuildStayOnTopItem.pszName = (wchar_t*) L"StayOnTopItemButton";
	BuildStayOnTopItem.pszCaption = (wchar_t*) L"Trzymaj okno na wierzchu";
	BuildStayOnTopItem.Hint = (wchar_t*) L"Trzymaj okno na wierzchu";
	if(!SetStayOnTop)
	 BuildStayOnTopItem.IconIndex = STAYONTOP_OFF;
	else
	{
	  BuildStayOnTopItem.IconIndex = STAYONTOP_ON;
	  SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	}
	BuildStayOnTopItem.Position = 0;
	BuildStayOnTopItem.pszService = (wchar_t*) L"sStayOnTopItem";
	BuildStayOnTopItem.Handle = (unsigned int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_CREATEBUTTON,0,(LPARAM)(&BuildStayOnTopItem));
  }
}
//---------------------------------------------------------------------------

int __stdcall ServiceQuickQuoteItem (WPARAM wParam, LPARAM lParam)
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
	  wchar_t WideBuffer[2048];
	  GetWindowTextW(hRichEdit, WideBuffer, sizeof(WideBuffer));
	  UnicodeString Text = WideBuffer;
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

void DestroyClipTab()
{
  if(hFrmSend)
  {
	BuildClipTabItem.pszName = L"ClipTabItem";
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildClipTabItem));
  }
}
//---------------------------------------------------------------------------

void BuildClipTab()
{
  if(hFrmSend)
  {
	BuildClipTabItem.cbSize = sizeof(TPluginAction);
	BuildClipTabItem.pszName = L"ClipTabItem";
	BuildClipTabItem.pszCaption = L"Przypnij/odepnij zak³adkê";;
	BuildClipTabItem.Position = 1;
	BuildClipTabItem.IconIndex = -1;
	BuildClipTabItem.pszService = L"sClipTabItem";
	BuildClipTabItem.pszPopupName = L"popTab";
	BuildClipTabItem.Handle = (unsigned int)hFrmSend;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildClipTabItem));
  }
}
//---------------------------------------------------------------------------

int __stdcall ServiceClipTabItem (WPARAM wParam, LPARAM lParam)
{
  //Pobieranie nowo zdefinowanej przypietej karty
  UnicodeString ClipTab = ClipTabPopup;;
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
	Ini->WriteString("ClipTabs", "Tab"+IntToStr(Count+1), ClipTab);
	delete Ini;
	//Zmiana miejsca zakladki
	if(!ClipTab.Pos("ischat_"))
	{
	  int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,0,(LPARAM)ClipTab.w_str());
	  if(Index)
	  {
		TPluginTriple PluginTriple;
		PluginTriple.Handle1 = (unsigned int)hFrmSend;
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
		PluginTriple.Handle1 = (unsigned int)hFrmSend;
		PluginTriple.Param1 = Index;
		PluginTriple.Param2 = 0;
		PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
	  }
	}
	//Wymuszenie zmiany caption zakladki
	if(!ClipTab.Pos("ischat_"))
	 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)ClipTab.w_str());
	else
	{
	  UnicodeString ClipTabW = ClipTab;
	  ClipTabW = ClipTabW.Delete(1,7);
	  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)ClipTabW.w_str());
	}
	//Zmiana ikonki na zakladce
	if((ClipTab=="blip@blip.pl")||(ClipTab.Pos("202@plugin.gg")>0))
	{
	  PluginContact.cbSize = sizeof(TPluginContact);
	  PluginContact.JID = ClipTab.w_str();
	  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)132,(LPARAM)&PluginContact);
	}
	else if(!ClipTab.Pos("ischat_"))
	{
	  int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",ClipTab,0);
	  //Ikona juz w interfejsie AQQ
	  if(Icon)
	  {
		PluginContact.cbSize = sizeof(TPluginContact);
		PluginContact.JID = ClipTab.w_str();
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&PluginContact);
	  }
	  //Ikona jeszcze niezaladowana do interfejsu AQQ
	  else
	  {
		//Jezeli plik PNG jest juz wygenerowany
		if(FileExists(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+ClipTab+".png"))
		{
		  Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+ClipTab+".png").w_str());
		 ClipTabsIconList->WriteInteger("ClipTabsIcon",ClipTab,Icon);
		  PluginContact.cbSize = sizeof(TPluginContact);
		  PluginContact.JID = ClipTab.w_str();
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&PluginContact);
		}
		//Generowanie pliku PNG 16x16 z awataru kontaktu
		else
		{
		  TIniFile *Ini = new TIniFile(GetContactsUserDir()+ClipTab+".ini");
		  if(!hSettingsForm)
		  {
			Application->Handle = (HWND)SettingsForm;
			hSettingsForm = new TSettingsForm(Application);
		  }
		  UnicodeString Avatar = hSettingsForm->IdDecoderMIME->DecodeString(Ini->ReadString("Other","Avatar",""));
		  delete Ini;
		  if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
		  {
			Avatar = StringReplace(Avatar, "{PROFILEPATH}\\Data\\Avatars\\", "", TReplaceFlags());
			Avatar = GetAvatarsUserDir() + Avatar;
			hSettingsForm->ConvertImage(Avatar,GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+ClipTab+".png");
			if(FileExists(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+ClipTab+".png"))
			{
			  Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+ClipTab+".png").w_str());
			  ClipTabsIconList->WriteInteger("ClipTabsIcon",ClipTab,Icon);
			  PluginContact.cbSize = sizeof(TPluginContact);
			  PluginContact.JID = ClipTab.w_str();
			  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)Icon,(LPARAM)&PluginContact);
			}
		  }
		}
	  }
	}
  }
  //Jezeli zakladka jest juz przypieta
  else
  {
	//Zapamietanie ostatnio odpietej zakladki
	JustUnClipTabJID = ClipTab;
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
	PluginTriple.Handle1 = (unsigned int)hFrmSend;
	int Count = PluginLink.CallService(AQQ_FUNCTION_TABCOUNT,(WPARAM)&PluginTriple,0);
	//Zmiana miejsca zakladki
	if(Count)
	{
	  if(!ClipTab.Pos("ischat_"))
	  {
		int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,0,(LPARAM)ClipTab.w_str());
		PluginTriple.Param1 = Index;
		PluginTriple.Param2 = Count-1;
		PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
	  }
	  else
	  {
		UnicodeString ClipTabW = ClipTab;
		ClipTabW = ClipTabW.Delete(1,7);
		int Index = PluginLink.CallService(AQQ_FUNCTION_TABINDEX,2,(LPARAM)ClipTabW.w_str());
		PluginTriple.Param1 = Index;
		PluginTriple.Param2 = Count-1;
		PluginLink.CallService(AQQ_FUNCTION_TABMOVE,(WPARAM)&PluginTriple,0);
	  }
	}
	//Wymuszenie zmiany caption zakladki
	if(!ClipTab.Pos("ischat_"))
	 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)ClipTab.w_str());
	else
	{
	  UnicodeString ClipTabW = ClipTab;
	  ClipTabW = ClipTabW.Delete(1,7);
	  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)ClipTabW.w_str());
	}
	//Zmiana ikonki na zakladce
	if(!ClipTab.Pos("ischat_"))
	{
	  PluginContact.cbSize = sizeof(TPluginContact);
	  PluginContact.JID = ClipTab.w_str();
	  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)GetContactState(ClipTab),(LPARAM)&PluginContact);
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

void EraseClipTabs()
{
  //Usuniecie wszystkich zakladek z listy przypietych zakladek
  ClipTabsList->Clear();
  //Usuniecie wszystkich przypietych zakladek z listy przypietych zakladek w pliku sesji
  TIniFile *Ini = new TIniFile(SessionFileDir);
  Ini->EraseSection("ClipTabs");
  delete Ini;
  //Zmiana caption zakladek
  if(TabsList->Count>1)
  {
	//Wylaczenie notyfikacji
	PluginLink.UnhookEvent(OnActiveTab);
	//Odswiezanie zakladek
	UnicodeString TmpActiveJID = ActiveTabJID;
	for(int Count=0;Count<TabsList->Count;Count++)
	{
	  UnicodeString JID = TabsList->Strings[Count];
	  //Otwieranie zakladki z kontektem
	  if(!JID.IsEmpty())
	  {
		if(!JID.Pos("ischat_"))
		 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
		else
		{
		  JID = JID.Delete(1,7);
		  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)JID.w_str());
		}
	  }
	}
	//Przelaczenie na aktywna zakladke
	if(!TmpActiveJID.Pos("ischat_"))
	 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)TmpActiveJID.w_str());
	else
	{
	  TmpActiveJID = TmpActiveJID.Delete(1,7);
	  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)TmpActiveJID.w_str());
	}
	//Wlaczenie notyfikacji
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_ACTIVETAB,OnActiveTab);
  }
  else
  {
	//Wymuszenie zmiany caption zakladki
	if(!ActiveTabJID.Pos("ischat_"))
	 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)ActiveTabJID.w_str());
	else
	{
	  UnicodeString ActiveJIDW = ActiveTabJID;
	  ActiveJIDW = ActiveJIDW.Delete(1,7);
	  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)ActiveJIDW.w_str());
	}
	//Zmiana ikonki na zakladce
	if(!ActiveTabJID.Pos("ischat_"))
	{
	  PluginContact.cbSize = sizeof(TPluginContact);
	  PluginContact.JID = ActiveTabJID.w_str();
	  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)GetContactState(ActiveTabJID),(LPARAM)&PluginContact);
	}
  }
}
//---------------------------------------------------------------------------

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
	//Odswiezenie ikonek na zakladkach
	if(TabsList->Count>1)
	{
	  //Odswiezanie zakladek
	  UnicodeString TmpActiveJID = ActiveTabJID;
	  for(int Count=0;Count<TabsList->Count;Count++)
	  {
		UnicodeString JID = TabsList->Strings[Count];
		//Otwieranie zakladki z kontektem
		if(!JID.IsEmpty())
		{
		  if(!JID.Pos("ischat_"))
		   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
		  else
		  {
			JID = JID.Delete(1,7);
			PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)JID.w_str());
		  }
		}
	  }
	  //Przelaczenie na aktywna zakladke
	  if(!TmpActiveJID.Pos("ischat_"))
	   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)TmpActiveJID.w_str());
	  else
	  {
		TmpActiveJID = TmpActiveJID.Delete(1,7);
		PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)TmpActiveJID.w_str());
	  }
	}
	else
	{
	  //Wymuszenie zmiany zakladki
	  if(!ActiveTabJID.Pos("ischat_"))
	   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)ActiveTabJID.w_str());
	  else
	  {
		UnicodeString ActiveJIDW = ActiveTabJID;
		ActiveJIDW = ActiveJIDW.Delete(1,7);
		PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)ActiveJIDW.w_str());
	  }
	}
  }
  delete ClipIcons;
}
//---------------------------------------------------------------------------

//Wylaczanie/wlaczanie AntiSpim
void CheckAntiSpim()
{
  //Wylaczenie
  if(!AntiSpimChk) PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_ANTISPIM_LEN,0);
  //Wlaczenie
  else PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_ANTISPIM_LEN,1);
}
//---------------------------------------------------------------------------

//Ukrywanie/pokazywanie paska informacyjnego w oknie rozmowy
void CheckHideStatusBar()
{
  if(hFrmSend)
  {
	if(!hStatusBarPro) hStatusBarPro = FindWindowEx(hFrmSend,NULL,"TStatusBarPro",NULL);
	if(hStatusBarPro)
	{
	  GetWindowRect(hStatusBarPro,&WindowRect);
      //Ukrywanie
	  if((HideStatusBarChk)&&(WindowRect.Height()))
	  {
		//Pobieranie pozycji okna rozmowy
		GetWindowRect(hFrmSend,&WindowRect);
		//Ukrywanie paska
		SetWindowPos(hStatusBarPro,NULL,0,0,0,0,SWP_NOMOVE);
		ShowWindow(hStatusBarPro,SW_HIDE);
		//Odswiezanie okna rozmowy
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
	  }
	  //Pokazywanie
	  else if((!HideStatusBarChk)&&(!WindowRect.Height()))
	  {
		//Pobieranie pozycji okna rozmowy
		GetWindowRect(hFrmSend,&WindowRect);
		//Pokazywanie paska
		ShowWindow(hStatusBarPro,SW_SHOW);
		SetWindowPos(hStatusBarPro,NULL,0,0,WindowRect.Width(),18,SWP_NOMOVE);
		//Odswiezanie okna rozmowy
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
		SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
	  }
	}
  }
}
//---------------------------------------------------------------------------

//Pokazywanie paska narzedzi w oknie rozmowy
void ShowToolBar()
{
  if((hFrmSend)&&(hToolBar))
  {
	//Pobieranie wysokosci paska
	GetWindowRect(hToolBar,&WindowRect);
	int ToolBarHeight = WindowRect.Height();
	//Pokazanie paska
	if(!ToolBarHeight)
	{
	  //Pobieranie pozycji okna rozmowy
	  GetWindowRect(hFrmSend,&WindowRect);
	  //Pokazanie paska
	  SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),25,SWP_NOMOVE);
	  //Oswiezenia okna
	  GetWindowRect(hFrmSend,&WindowRect);
	  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
	  SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
	}
  }
}
//---------------------------------------------------------------------------

//Hook na zamkniecie okiena rozmowy
int __stdcall OnWindowEvent (WPARAM wParam, LPARAM lParam)
{
  WindowEvent = (PPluginWindowEvent)lParam;
  int Event = WindowEvent->WindowEvent;
  UnicodeString EventType = (wchar_t*)(WindowEvent->ClassName);

  //Otwarcie glownego okna = zaladowanie w pelni listy kontatkow
  if((EventType=="TfrmMain")&&(Event==1))
  {
	//Odczytywanie sesji
	if(RestoreTabsSessionChk)
	{
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  TStringList *Session = new TStringList;
	  Ini->ReadSection("Session",Session);
	  if(Session->Count>0)
	  {
		RestoringSession = true;
		//Odczyt sesji
		for(int Count=0;Count<Session->Count;Count++)
		 Session->Strings[Count] = Ini->ReadString("Session","Tab"+IntToStr(Count+1),"");
		//Otwieranie kontaktow
		for(int Count=0;Count<Session->Count;Count++)
		{
		  UnicodeString JID = Session->Strings[Count];
		  //Otwieranie zakladki z kontektem
		  if(!JID.IsEmpty())
		  {
			//Otwieranie zakladki z danym kontektem
			if(!JID.Pos("ischat_"))
			 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
			//"Otwieranie" zakladki z czatem
			else
			{
			  JID = JID.Delete(1,7);
			  ChatSessionList->Add(JID);
			}
		  }
		}
		//Usuwanie sesji wiadomosci
		Ini->EraseSection("SessionMsg");
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
	BuildFrmUnsentMsg();
	//Ustawianie tekstu glownego okna AQQ
	EnumWindows((WNDENUMPROC)FindFrmMain,0);
	EnumWindows((WNDENUMPROC)FindFrmMainL,0);
	if(TweakFrmMainTitlebarChk)
	{
	  if((TweakFrmMainTitlebarMode==1)&&(!ActiveProfileName.IsEmpty()))
	  {
		SetWindowTextW(hFrmMain,("AQQ [" + ActiveProfileName + "]").w_str());
		SetWindowTextW(hFrmMainL,("AQQ [" + ActiveProfileName + "]").w_str());
	  }
	  else if(TweakFrmMainTitlebarMode==2)
	  {
		if(!TweakFrmMainTitlebarText.IsEmpty())
		{
		  SetWindowTextW(hFrmMain,("AQQ " + TweakFrmMainTitlebarText).w_str());
		  SetWindowTextW(hFrmMainL,("AQQ " + TweakFrmMainTitlebarText).w_str());
		}
		else
		{
		  SetWindowTextW(hFrmMain,L"AQQ");
		  SetWindowTextW(hFrmMainL,L"AQQ");
		}
	  }
	}
  }

  //Otwarcie okna rozmowy
  if((EventType=="TfrmSend")&&(Event==1))
  {
	FrmSendOpening = true;

	if(!hFrmSend)
	{
	  //Przypisanie uchwytu do okna rozmowy
	  hFrmSend = (HWND)WindowEvent->Handle;
	  //Szukanie pola wiadomosci
	  EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	  //Szukanie paska narzedzi
	  EnumChildWindows(hFrmSend,(WNDENUMPROC)FindToolBar,0);
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
	  //Przypisanie nowej procki dla okna rozmowy
	  OldFrmSendProc = (WNDPROC)SetWindowLongPtrW(hFrmSend, GWL_WNDPROC,(LONG)FrmSendProc);
	  //Usuniêcie uchwytow do ikonek okna rozmowy
	  hIconSmall = NULL;
	  hIconBig = NULL;
	  //Otwieranie przypietych zakladek
	  if((OpenClipTabsChk)&&(!RestoringSession))
	  {
		if(ClipTabsList->Count) SetTimer(hFrmMain,600,500,(TIMERPROC)Timer);
	  }
	}
  }
  //Zamkniecie okna rozmowy
  if((EventType=="TfrmSend")&&(Event==2))
  {
	//Resetowanie zmiennej aktwnie otwartej zakladki
	ActiveTabJID = "";
	//Resetowanie uchwytow
	hFrmSend = NULL;
	hRichEdit = NULL;
	hToolBar = NULL;
	hStatusBarPro = NULL;
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
  }

  if((StayOnTopChk)&&(SetStayOnTop))
  {
	//Otworzenie okna emotek
	if((EventType=="TfrmGraphic")&&(Event==1))
	 SetWindowPos((HWND)WindowEvent->Handle,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	//Otworzenie okna szybkich emotek
	if((EventType=="TfrmCompletion")&&(Event==1))
	 SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	 //Zamkniecie okna szybkich emotek
	if((EventType=="TfrmCompletion")&&(Event==2))
	 SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	//Otworzenie okna wysylania wycinka
	if((EventType=="TfrmPos")&&(Event==1))
	{
	  //Tworzenie timera
	  SetTimer(hFrmMain,100,250,(TIMERPROC)Timer);
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane tekstu na zakladce
int __stdcall OnTabCaption (WPARAM wParam, LPARAM lParam)
{
  //Zmienna caption zakladki
  UnicodeString TabCaption = (wchar_t*)wParam;
  //Pobieranie danych
  Contact = (PPluginContact)lParam;
  UnicodeString JID = (wchar_t*)(Contact->JID);
  if(Contact->IsChat) JID = "ischat_" + JID;
  //Przypiete zakladki
  if((ClipTabsList->IndexOf(JID)!=-1)&&(!CounterClipTabsChk)) TabCaption = "";
  else if((ClipTabsList->IndexOf(JID)!=-1)&&(CounterClipTabsChk)&&(!Contact->IsChat))
  {
	UnicodeString Nick = (wchar_t*)(Contact->Nick);
	TabCaption = StringReplace(TabCaption, Nick + " ", "", TReplaceFlags());
	TabCaption = StringReplace(TabCaption, Nick, "", TReplaceFlags());
	TabCaption = TabCaption.Trim();
  }
  //Licznik nieprzeczytanych wiadomosci
  if((InactiveTabsNewMsgChk)&&(!ClosingTab))
  {
	int Count = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID,0);
	if(Count)
	{
	  if(TabCaption.Pos("[" + IntToStr(Count) + "] ")!=1)
	  {
		TabCaption = "[" + IntToStr(Count) + "] " + TabCaption;
		TabCaption = TabCaption.Trim();
	  }
	}
  }
  //Nieprzypieta zakladka czatowa i normalizacja nazw
  if((ClipTabsList->IndexOf(JID)==-1)&&(Contact->IsChat)&&(!Contact->FromPlugin))
  {
	int Count = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID,0);
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

  return (WPARAM)TabCaption.w_str();
}
//---------------------------------------------------------------------------

//Hook na zmiane ikonki na zakladce
int __stdcall OnTabImage(WPARAM wParam, LPARAM lParam)
{
  //Notyfikcja pisania wiadomosci
  if(PreMsgList->Count>0)
  {
	Contact = (PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	//Jezeli zakladka dotyczy notyfikacji pisania wiadomosci
	if(PreMsgList->IndexOf(JID)!=-1)
	{
	  int TabImage = (int)wParam;
	  if((TabImage!=COMPOSING)||(TabImage!=PAUSE))
	  {
		int ChatState = PreMsgStateList->ReadInteger("PreMsgState",JID,0);
		//Pisanie wiadomosci
		if(ChatState==2)
		 return COMPOSING;
		//Spauzowanie
		else if(ChatState==5)
		 return PAUSE;
		//Domyslna ikona
		else
		{
		  ChatState = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)Contact);
		  return ChatState;
		}
	  }
	}
  }
  //Przypiete zakladki
  if(ClipTabsList->Count>0)
  {
	Contact = (PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	if(ClipTabsList->IndexOf(JID)!=-1)
	{
	  int TabImage = (int)wParam;
	  //Jezeli nie jest zmieniane na ikonke nowej wiadomosci itp
	  if((TabImage!=8)&&(TabImage!=COMPOSING)&&(TabImage!=PAUSE))
	  {
        //Zmiana ikonki na zakladce
		if((JID=="blip@blip.pl")||(JID.Pos("202@plugin.gg")>0))
		{
		  return 132;
		}
		else if(!JID.Pos("ischat_"))
		{
		  int Icon = ClipTabsIconList->ReadInteger("ClipTabsIcon",JID,0);
		  //Ikona juz w interfejsie AQQ
		  if(Icon)
		   return Icon;
		  //Ikona jeszcze niezaladowana do interfejsu AQQ
		  else
		  {
			//Jezeli plik PNG jest juz wygenerowany
			if(FileExists(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
			{
			  Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
			  ClipTabsIconList->WriteInteger("ClipTabsIcon",JID,Icon);
			  return Icon;
			}
			//Generowanie pliku PNG 16x16 z awataru kontaktu
			else
			{
			  TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
			  if(!hSettingsForm)
			  {
				Application->Handle = (HWND)SettingsForm;
				hSettingsForm = new TSettingsForm(Application);
			  }
			  UnicodeString Avatar = hSettingsForm->IdDecoderMIME->DecodeString(Ini->ReadString("Other","Avatar",""));
			  delete Ini;
			  if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
			  {
				Avatar = StringReplace(Avatar, "{PROFILEPATH}\\Data\\Avatars\\", "", TReplaceFlags());
				Avatar = GetAvatarsUserDir() + Avatar;
				hSettingsForm->ConvertImage(Avatar,GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+JID+".png");
				if(FileExists(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+JID+".png"))
				{
				  Icon = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(GetPluginUserDir()+"\\\\TabKit\\\\Avatars\\\\"+JID+".png").w_str());
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

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie okna rozmowy lub zakladki
int __stdcall OnCloseTab(WPARAM wParam, LPARAM lParam)
{
  //Blok zmiany tekstu na zakladce
  ClosingTab = true;
  //Pobieranie danych dt. kontaktu
  Contact = (PPluginContact)lParam;
  UnicodeString JID = (wchar_t*)(Contact->JID);
  if(Contact->IsChat) JID = "ischat_" + JID;
  //Zapisywanie stanu kontaktu
  if(!Contact->IsChat)
  {
	int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(Contact));
	ContactsStateList->WriteInteger("State",JID,State);
  }
  //Usuwanie JID z listy aktywnych zakladek/okien
  if(TabsList->IndexOf(JID)!=-1)
   TabsList->Delete(TabsList->IndexOf(JID));
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
  }
  //Usuwanie JID z kolejki
  if(SwitchToNewMsgChk)
  {
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
	//Sprawdzanie czy kontakt jest czatem z wtyczki
	if((Contact->IsChat)&&(Contact->FromPlugin))
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
	SkipClosedTabsChk:
	//Skip
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na aktwyna zakladke lub okno rozmowy
int __stdcall OnActiveTab (WPARAM wParam, LPARAM lParam)
{
  //Blok zmiany tekstu na zakladce
  ClosingTab = false;
  //Pobieranie danych
  Contact = (PPluginContact)lParam;
  UnicodeString JID = (wchar_t*)(Contact->JID);
  if(Contact->IsChat) JID = "ischat_" + JID;
  //Aktywna zakladka
  ActiveTabJID = JID;
  //Usuwanie JID z kolejki przelaczania sie na nowe wiadomosci
  if(SwitchToNewMsgChk)
  {
	if(MsgList->IndexOf(JID)!=-1)
	 MsgList->Delete(MsgList->IndexOf(JID));
  }
  //Jezeli zakladka z kontaktem nie jest otwarta
  if(TabsList->IndexOf(JID)==-1)
  {
	//Dodawanie JID do tablicy zakladek
	TabsList->Add(JID);
	//Pobieranie stanu kontaktu
	if(!Contact->IsChat)
	{
	  int State = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(Contact));
	  ContactsStateList->WriteInteger("State",JID,State);
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
	  //Wczytywanie ostatnio przeprowadzonej rozmowy
	  if((RestoringSession)&&(!Contact->IsChat))
	   PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)Contact->UserIdx);
	  //Odczytywanie sesji wiadomosci
	  if((RestoreMsgSessionChk)&&(RestoringSession))
	  {
		UnicodeString Body = UTF8ToUnicodeString(IniStrToStr(Ini->ReadString("SessionMsg", JID, "")).t_str());
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
	  SetTimer(hFrmMain,500,300,(TIMERPROC)Timer);
	}
	//Niewyslane wiadomosci
	if((UnsentMsgChk)&&(!RestoringSession))
	{
	  //Odczyt pliku sesji
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  UnicodeString Body = UTF8ToUnicodeString(IniStrToStr(Ini->ReadString("Messages", JID, "")).t_str());
	  //Wczytanie tresci wiadomosci do pola RichEdit
	  if(!Body.IsEmpty())
	  {
		//Pobieranie ostatniej wiadomoœci
		if(!Contact->IsChat) PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)Contact->UserIdx);
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
		BuildFrmUnsentMsg();
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
		  if(!Contact->IsChat) PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)Contact->UserIdx);
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
  //Ustawianie pustego tekstu na przypietej zakladce
  if(ClipTabsList->IndexOf(JID)!=-1)
  {
	PluginLink.UnhookEvent(OnTabCaption);
	PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)L"",(LPARAM)Contact);
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
  }
  //Ustawianie domyslnego tekstu na zakladce nieprzypietej
  else
  {
	int Count = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID,0);
	//Tylko dla zakladki z licznikiem nieprzeczytanych wiadomosci lub ostatnio odpietej zakladki
	if((Count)||(JID==JustUnClipTabJID))
	{
	  JustUnClipTabJID = "";
	  PluginLink.UnhookEvent(OnTabCaption);
	  //Zakladka zwykla
	  if(!Contact->IsChat)
	   PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)Contact->Nick,(LPARAM)Contact);
	  //Zakladka z czatem
	  else
	  {
		//Czat nie ze wtyczki
		if(!Contact->FromPlugin)
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
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)Channel.w_str(),(LPARAM)Contact);
		}
		//Czat z wtyczki
		else
		{
		  UnicodeString Caption = JID;
		  Caption = Caption.Delete(1,7);
		  Caption = Caption.Delete(Caption.Pos("@"),Caption.Length());
		  //Usuwanie licznika
		  wstring input = Caption.c_str();
		  wregex expr(L"[^A-Za-z]");
		  wstring replace = L"";
		  wstring result = regex_replace( input, expr, replace, match_default | format_sed);
		  Caption = String(result.c_str());
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
		  Number = String(result2.c_str());
		  Caption = Caption + " [nr" + Number + "]";
		  //Ustawianie sformatowanego tekstu
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)Caption.w_str(),(LPARAM)Contact);
		}
	  }
	  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
    }
  }
  //Zmiana caption okna rozmowy
  if(TweakFrmSendTitlebarChk)
  {
	if(!Contact->IsChat)
	{
	  UnicodeString Nick = (wchar_t*)(Contact->Nick);
	  UnicodeString Status = (wchar_t*)(Contact->Status);
	  Status = StringReplace(Status, "\n", " ", TReplaceFlags() << rfReplaceAll);

	  if(TweakFrmSendTitlebarMode==1)
	  {
		if(!Status.IsEmpty())
		 SetWindowTextW(hFrmSend,(Nick + " - " + Status).w_str());
		else
		 SetWindowTextW(hFrmSend,Nick.w_str());
	  }
	  else if(TweakFrmSendTitlebarMode==2)
	   SetWindowTextW(hFrmSend,Nick.w_str());
	  else
	   SetWindowTextW(hFrmSend,(Nick + " - " + JID).w_str());
	}
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
	  TempTitlebar = "";
	}
  }
  //Licznik nowych wiadomosci na zakladkach
  if(InactiveTabsNewMsgChk)
  {
	//Resetowanie stanu nowych wiadomosci
	InactiveTabsNewMsgCount->DeleteKey("TabsMsg",JID);
  }
  //Notyfikcja pisania wiadomosci
  if(ChatStateNotiferNewMsgChk)
  {
	//Jezeli JID znajduje sie na liscie notyfikacji wiadomosci
	if((PreMsgList->IndexOf(JID)!=-1)&&(!Contact->IsChat))
	{
	  //Zmiana ikonki na zakladce
	  int ChatState = PreMsgStateList->ReadInteger("PreMsgState",JID,0);
	  //Pisanie wiadomosci
	  if(ChatState==2)
	  {
		//Zmiana ikonki na zakladce
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)COMPOSING,(LPARAM)Contact);
	  }
	  //Spauzowanie
	  else if(ChatState==5)
	  {
		//Zmiana ikonki na zakladce
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)PAUSE,(LPARAM)Contact);
	  }
	  //Inny stan
	  else
	  {
		//Usuwanie JID z listy notyfikacji wiadomosci
		if(PreMsgList->IndexOf(JID)!=-1)
		 PreMsgList->Delete(PreMsgList->IndexOf(JID));
		PreMsgStateList->WriteInteger("PreMsgState",JID,0);
		//Zmiana ikonki na zakladce
		ChatState = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)Contact);
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)ChatState,(LPARAM)Contact);
	  }
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
	Contact = (PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	if(Contact->IsChat) JID = "ischat_" + JID;
	UnicodeString Body = (wchar_t*)wParam;
	Body = Body.Trim();
	if(!Body.IsEmpty())
	{
	  //Szybki dostep niewyslanych wiadomosci
	  DestroyFrmUnsentMsg();
	  //Zapis pliku sesji
	  ShortString BodyShort = UTF8EncodeToShortString(Body);
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  Ini->WriteString("Messages", JID, StrToIniStr(BodyShort.operator AnsiString()));
	  delete Ini;
	  //Szybki dostep niewyslanych wiadomosci
	  BuildFrmUnsentMsg();
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na odbieranie wiadomosci
int __stdcall OnRecvMsg(WPARAM wParam, LPARAM lParam)
{
  //Przelaczanie na zakladke z nowa wiadomoscia
  if(SwitchToNewMsgChk)
  {
	Contact = (PPluginContact)wParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	if(Contact->IsChat) JID = "ischat_" + JID;
	//Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
	if((JID!=ActiveTabJID)&&(TabsList->IndexOf(JID)!=-1))
	{
	  Message = (PPluginMessage)lParam;
	  //Rodzaj wiadomosci
	  if(Message->Kind!=MSGKIND_RTT)
	  {
		UnicodeString Body = (wchar_t*)(Message->Body);
		if(!Body.IsEmpty())
		{
		  //Dodawanie JID do kolejki nowych wiadomosci
		  if(MsgList->IndexOf(JID)==-1)
		   MsgList->Add(JID);
		}
	  }
	}
  }
  //Licznik nieprzeczytanych wiadomosci na oknie rozmowy
  if(InactiveFrmNewMsgChk)
  {
	Contact = (PPluginContact)wParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	if(Contact->IsChat) JID = "ischat_" + JID;
	//Jezeli zakladka jest otwarta
	if((TabsList->IndexOf(JID)!=-1))
	{
	  //Jezeli okno rozmowy jest nieaktywne
	  if(hFrmSend!=GetForegroundWindow())
	  {
		Message = (PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if(Message->Kind!=MSGKIND_RTT)
		{
		  UnicodeString Body = (wchar_t*)(Message->Body);
		  if(!Body.IsEmpty())
		  {
			//Pobranie oryginalnego titlebar'a
			if((TempTitlebar.IsEmpty())&&(!InactiveFrmNewMsgCount))
			{
			  GetWindowTextW(hFrmSend,TempTitlebarW,sizeof(TempTitlebarW));
			  TempTitlebar = (wchar_t*)TempTitlebarW;
			}
			//Dodanie 1 do licznika nieprzeczytachy wiadomosci
			InactiveFrmNewMsgCount++;
			//Ustawianie nowego titlebar'a
			SetWindowTextW(hFrmSend,("[" + IntToStr(InactiveFrmNewMsgCount)+ "] " + TempTitlebar).w_str());
		  }
		}
	  }
	  else
	  {
		//Kasowanie licznika nowych wiadomosci
		InactiveFrmNewMsgCount = 0;
		//Przywracanie poprzedniego stanu titlebara
		if(!TempTitlebar.IsEmpty())
		{
		  SetWindowTextW(hFrmSend,TempTitlebar.w_str());
		  TempTitlebar = "";
		}
	  }
	}
  }
  //Licznik nieprzeczytanych wiadomosci na zakladkach
  if(InactiveTabsNewMsgChk)
  {
	Contact = (PPluginContact)wParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	if(Contact->IsChat) JID = "ischat_" + JID;
	//Jezeli licznik nie ma byc dodawany na przypietej zakladce
	if((ClipTabsList->IndexOf(JID)!=-1)&&(InactiveClipTabsChk))
	 goto SkipInactiveTabsNewMsgChk;
	//Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
	if((JID!=ActiveTabJID)&&(TabsList->IndexOf(JID)!=-1))
	{
	  Message = (PPluginMessage)lParam;
	  //Rodzaj wiadomosci
	  if(Message->Kind!=MSGKIND_RTT)
	  {
		UnicodeString Body = (wchar_t*)(Message->Body);
		if(!Body.IsEmpty())
		{
		  //Pobieranie i ustawianie stanu nowych wiadomosci
		  int InactiveTabsCount = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID,0);
		  InactiveTabsCount++;
		  InactiveTabsNewMsgCount->WriteInteger("TabsMsg",JID,InactiveTabsCount);
		  UnicodeString TabCaption;
		  //Jezeli zakladka jest przypiera
		  if(ClipTabsList->IndexOf(JID)!=-1) TabCaption = "";
		  //JEzeli zakladka nie jest przypieta
		  else
		  {
            //Zakladka zwykla
			if(!Contact->IsChat) TabCaption = (wchar_t*)Contact->Nick;
			//Zakladka z czatem
			else
			{
			  //Czat nie ze wtyczki
			  if(!Contact->FromPlugin)
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
				TabCaption = String(result.c_str());
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
				Number = String(result2.c_str());
				TabCaption = TabCaption + " [nr" + Number + "]";
			  }
			}
		  }
		  TabCaption = "[" + IntToStr(InactiveTabsCount) + "] " + TabCaption;

		  //Ustawianie nowego tytulu zakladki
		  PluginLink.UnhookEvent(OnTabCaption);
		  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)TabCaption.w_str(),(LPARAM)Contact);
		  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
		}
	  }
	}
	SkipInactiveTabsNewMsgChk:
	//Skip
  }
  //Notyfikacja nowej wiadomosci w chmurce
  if((InactiveNotiferNewMsgChk)&&(!BlockInactiveNotiferNewMsg))
  {
	Contact = (PPluginContact)wParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	if(Contact->IsChat) JID = "ischat_" + JID;
	//Jezeli zakladka jest otwarta
	if(TabsList->IndexOf(JID)!=-1)
	{
	  //JID wiadomosci jest inny niz JID aktwnej zakladki
	  if(JID!=ActiveTabJID)
	  {
		Message = (PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if((Message->Kind==MSGKIND_CHAT)||(Message->Kind==MSGKIND_GROUPCHAT))
		{
		  UnicodeString Body = (wchar_t*)(Message->Body);
		  if(!Body.IsEmpty())
		  {
			UnicodeString Nick = (wchar_t*)(Contact->Nick);
			UnicodeString Res = (wchar_t*)(Contact->Resource);
			int UserIdx = Contact->UserIdx;
			//Czy nie jest to obrazek?
			if(Body.Pos("AQQ_CACHE_ITEM")>0)
			 Body = "[" + Nick + " przesy³a obrazek]";
			//Generowanie ID dla chmurek
			DWORD TickID = GetTickCount();
			//Nick
			PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			if(CloudTickModeChk) PluginShowInfo.Event = tmeMsg;
			else PluginShowInfo.Event = tmePseudoMsg;
			PluginShowInfo.Text = Nick.w_str();
			PluginShowInfo.ImagePath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0));
			PluginShowInfo.TimeOut = 1000 * CloudTimeOutChk;
			if(CloudTickModeChk) PluginShowInfo.Tick = TickID;
			PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
			//Body
			PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			if(CloudTickModeChk) PluginShowInfo.Event = tmeMsgCap;
			else PluginShowInfo.Event = tmePseudoMsgCap;//tmeInfo;
			PluginShowInfo.Text = Body.w_str();
			PluginShowInfo.ImagePath = L"";
			PluginShowInfo.TimeOut = 1000 * CloudTimeOutChk;
			if(CloudTickModeChk) PluginShowInfo.Tick = TickID;
			PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
		  }
		}
	  }
	  else if(hFrmSend!=GetForegroundWindow())
	  {
		Message = (PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if((Message->Kind==MSGKIND_CHAT)||(Message->Kind==MSGKIND_GROUPCHAT))
		{
		  UnicodeString Body = (wchar_t*)(Message->Body);
		  if(!Body.IsEmpty())
		  {
			UnicodeString Nick = (wchar_t*)(Contact->Nick);
			UnicodeString Res = (wchar_t*)(Contact->Resource);
			int UserIdx = Contact->UserIdx;

			UnicodeString NotiferJID =  JID + "/" + Res;
			//Czy nie jest to obrazek?
			if(Body.Pos("AQQ_CACHE_ITEM")>0)
			 Body = "[" + Nick + " przesy³a obrazek]";
			//Generowanie ID dla chmurek
			DWORD TickID = GetTickCount();
			//Nick
			PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			if(CloudTickModeChk) PluginShowInfo.Event = tmeMsg;
			else PluginShowInfo.Event = tmePseudoMsg;
			PluginShowInfo.Text = Nick.w_str();
			PluginShowInfo.ImagePath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0));
			PluginShowInfo.TimeOut = 1000 * CloudTimeOutChk;
			if(CloudTickModeChk) PluginShowInfo.Tick = TickID;
			PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
			//Body
			PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			if(CloudTickModeChk) PluginShowInfo.Event = tmeMsgCap;
			else PluginShowInfo.Event = tmePseudoMsgCap;//tmeInfo;
			PluginShowInfo.Text = Body.w_str();
			PluginShowInfo.ImagePath = L"";
			PluginShowInfo.TimeOut = 1000 * CloudTimeOutChk;
			if(CloudTickModeChk) PluginShowInfo.Tick = TickID;
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
	Contact = (PPluginContact)wParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	//Jezeli zakladka jest otwarta & kontakt nie jest czatem
	if((TabsList->IndexOf(JID)!=-1)&&(!Contact->IsChat))
	{
      Message = (PPluginMessage)lParam;
	  int ChatState = Message->ChatState;
	  //Jezeli okno rozmowy jest nieaktywne
	  if(hFrmSend!=GetForegroundWindow())
	  {
		//Pisanie wiadomosci
		if(ChatState==2)
		{
		  if(!hIconSmall)
		  {
			hIconSmall = (HICON)SendMessage(hFrmSend, WM_GETICON, (WPARAM)ICON_SMALL, 0);
			hIconBig = (HICON)SendMessage(hFrmSend, WM_GETICON, (WPARAM)ICON_BIG, 0);
		  }
		  SendMessage(hFrmSend, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)LoadImage(0, ComposingIconSmall.t_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
		  SendMessage(hFrmSend, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)LoadImage(0, ComposingIconBig.t_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE));
		}
		//Spauzowanie
		else if(ChatState==5)
		{
		  if(!hIconSmall)
		  {
			hIconSmall = (HICON)SendMessage(hFrmSend, WM_GETICON, (WPARAM)ICON_SMALL, 0);
			hIconBig = (HICON)SendMessage(hFrmSend, WM_GETICON, (WPARAM)ICON_BIG, 0);
		  }
		  SendMessage(hFrmSend, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)LoadImage(0, PauseIconSmall.t_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
		  SendMessage(hFrmSend, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)LoadImage(0, PauseIconBig.t_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE));
		}
		//Inny stan
		else
		{
		  if(hIconSmall)
		  {
			SendMessage(hFrmSend, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);
			hIconSmall = NULL;
			SendMessage(hFrmSend, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIconBig);
			hIconBig = NULL;
		  }
		}
	  }
	  //Notyfikacja na zakladkach
	  //Pisanie wiadomosci
	  if(ChatState==2)
	  {
		//Dodawanie JID do listy notyfikacji wiadomosci
		if(PreMsgList->IndexOf(JID)==-1)
		 PreMsgList->Add(JID);
		PreMsgStateList->WriteInteger("PreMsgState",JID,2);
		//Zmiana ikonki na zakladce
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)COMPOSING,(LPARAM)Contact);
	  }
	  //Spauzowanie
	  else if(ChatState==5)
	  {
		//Dodawanie JID do listy notyfikacji wiadomosci
		if(PreMsgList->IndexOf(JID)==-1)
		 PreMsgList->Add(JID);
		PreMsgStateList->WriteInteger("PreMsgState",JID,5);
		//Zmiana ikonki na zakladce
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)PAUSE,(LPARAM)Contact);
	  }
	  //Inny stan
	  else
	  {
		//Usuwanie JID do listy notyfikacji wiadomosci
		if(PreMsgList->IndexOf(JID)!=-1)
		 PreMsgList->Delete(PreMsgList->IndexOf(JID));
		//PreMsgStateList->WriteInteger("PreMsgState",JID,0);
		PreMsgStateList->DeleteKey("PreMsgState",JID);
		//Zmiana ikonki na zakladce
		ChatState = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)Contact);
		PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)ChatState,(LPARAM)Contact);
	  }
	}
  }
  //Dodawanie JID do listy kontaktow z ktorymy przeprowadzono rozmowe
  if(ClosedTabsChk)
  {
	Contact = (PPluginContact)wParam;
	UnicodeString JID = (wchar_t*)Contact->JID;
	if(Contact->IsChat) JID = "ischat_" + JID;
	if(AcceptClosedTabsList->IndexOf(JID)==-1)
	{
	  AcceptClosedTabsList->Add(JID);
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

int __stdcall OnPreSendMsg(WPARAM wParam, LPARAM lParam)
{
  //Dodawanie JID do listy kontaktow z ktorymy przeprowadzono rozmowe
  if(ClosedTabsChk)
  {
	Contact = (PPluginContact)wParam;
	UnicodeString JID = (wchar_t*)Contact->JID;
    if(Contact->IsChat) JID = "ischat_" + JID;
	if(AcceptClosedTabsList->IndexOf(JID)==-1)
	{
	  AcceptClosedTabsList->Add(JID);
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

int __stdcall OnMsgComposing(WPARAM wParam, LPARAM lParam)
{
  if((RestoreTabsSessionChk)&&(RestoreMsgSessionChk))
  {
	Contact = (PPluginContact)wParam;
	ChatState = (PPluginChatState)lParam;
	UnicodeString JID = (wchar_t*)Contact->JID;
	if(Contact->IsChat) JID = "ischat_" + JID;
	UnicodeString Body = (wchar_t*)ChatState->Text;
	Body = Body.Trim();
	if(!Body.IsEmpty())
	{
	  ShortString BodyShort = UTF8EncodeToShortString(Body);
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  Ini->WriteString("SessionMsg", JID, StrToIniStr(BodyShort.operator AnsiString()));
	  delete Ini;
	}
	else
	{
	  if(!RestoringSession)
	  {
		TIniFile *Ini = new TIniFile(SessionFileDir);
		Ini->DeleteKey("SessionMsg", JID);
		delete Ini;
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

int __stdcall OnSetLastState (WPARAM wParam, LPARAM lParam)
{
  //Blokowanie notyfikacji nowych wiadomosci
  if(InactiveNotiferNewMsgChk)
  {
	PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);
	int NewState = (int)PluginStateChange.NewState;
	//OnLine - Connected
	if(NewState)
	{
	  //Jezeli uchwyt do glownego okna nie zostal jeszcze zdefiniowany
	  if(!hFrmMain)
	   EnumWindows((WNDENUMPROC)FindFrmMain,0);
	  if(hFrmMain)
	  {
		//Blokowanie notyfikatora nowych wiadomosci
		BlockInactiveNotiferNewMsg = true;
		//Tworzenie timera do odblokowania notyfikatora
		SetTimer(hFrmMain,200,20000,(TIMERPROC)Timer);
	  }
	}
  }
  //Przywracanie sesji z czatami
  if(RestoreTabsSessionChk)
  {
	PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);
	int NewState = (int)PluginStateChange.NewState;
	//OnLine - Connected
	if(NewState)
	{
	  //Jezeli uchwyt do glownego okna nie zostal jeszcze zdefiniowany
	  if(!hFrmMain)
	   EnumWindows((WNDENUMPROC)FindFrmMain,0);
	  if(hFrmMain)
	   SetTimer(hFrmMain,400,10000,(TIMERPROC)Timer);
	  SetLastStateRestore = true;
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany stanu
int __stdcall OnStateChange(WPARAM wParam, LPARAM lParam)
{
  //Blokowanie notyfikacji nowych wiadomosci
  if(InactiveNotiferNewMsgChk)
  {
	StateChange = (PPluginStateChange)lParam;
	int NewState = (int)StateChange->NewState;
	int OldState = (int)StateChange->OldState;
	bool Authorized = (bool)StateChange->Authorized;

	//OnLine - Connecting
	if((!OldState)&&(NewState)&&(!Authorized))
	{
	  Connecting = true;
	}
	//OnLine - Connected
	if((Connecting)&&(Authorized)&&(NewState==OldState))
	{
	  PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

	  int cNewState = (int)PluginStateChange.NewState;
	  int cOldState = (int)PluginStateChange.OldState;

	  if((cNewState==cOldState)&&(cNewState==NewState)&&(cOldState==OldState))
	  {
		//Blokowanie notyfikatora nowych wiadomosci
		BlockInactiveNotiferNewMsg = true;
		//Tworzenie timera
		KillTimer(hFrmMain,200);
		SetTimer(hFrmMain,200,20000,(TIMERPROC)Timer);
		Connecting = false;
	  }
	  else
	   Connecting = false;
	}
	else if((Connecting)&&(Authorized)&&(NewState!=OldState))
	 Connecting = false;
  }
  //Przywracanie sesji z czatami
  if((RestoreTabsSessionChk)||(!SetLastStateRestore))
  {
	if(ChatSessionList->Count>0)
	{
	  StateChange = (PPluginStateChange)lParam;
	  int NewState = (int)StateChange->NewState;
	  int OldState = (int)StateChange->OldState;
	  bool Authorized = (bool)StateChange->Authorized;

	  //OnLine - Connecting
	  if((!OldState)&&(NewState)&&(!Authorized))
	  {
		Connecting = true;
	  }
	  //OnLine - Connected
	  if((Connecting)&&(Authorized)&&(NewState==OldState))
	  {
		PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

		int cNewState = (int)PluginStateChange.NewState;
		int cOldState = (int)PluginStateChange.OldState;

		if((cNewState==cOldState)&&(cNewState==NewState)&&(cOldState==OldState))
		{
          //Niby blokada
		  SetLastStateRestore = true;
		  //Tworzenie timera
		  SetTimer(hFrmMain,400,10000,(TIMERPROC)Timer);
		  Connecting = false;
		}
		else
		Connecting = false;
	  }
	  else if((Connecting)&&(Authorized)&&(NewState!=OldState))
	   Connecting = false;
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

int __stdcall OnMsgContextPopup (WPARAM wParam, LPARAM lParam)
{
  if(QuickQuoteChk)
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
		PluginTriple = (PPluginTriple)lParam;
        //Ustalanie pozycji dla elemenu
		PluginItemDescriber.cbSize = sizeof(TPluginItemDescriber);
		PluginItemDescriber.FormHandle = PluginTriple->Handle1;
		PluginItemDescriber.ParentName = (wchar_t*)L"popRich";
		PluginItemDescriber.Name = (wchar_t*)L"Wklej1";
		Action = (PPluginAction)(PluginLink.CallService(AQQ_CONTROLS_GETPOPUPMENUITEM,0,(LPARAM)(&PluginItemDescriber)));
		//Tworzenie elemtu
		BuildQuickQuoteItem.cbSize = sizeof(TPluginAction);
		BuildQuickQuoteItem.pszName = (wchar_t*)L"QuickQuoteItem";
		BuildQuickQuoteItem.pszCaption = (wchar_t*)L"Wklej jako cytat";
		BuildQuickQuoteItem.IconIndex = -1;
		BuildQuickQuoteItem.pszService = (wchar_t*)L"sQuickQuoteItem";
		BuildQuickQuoteItem.pszPopupName = (wchar_t*)L"popRich";
		BuildQuickQuoteItem.Position = Action->Position + 1;
		BuildQuickQuoteItem.Handle = PluginTriple->Handle1;
		PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildQuickQuoteItem));
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

int __stdcall OnMsgContextClose (WPARAM wParam, LPARAM lParam)
{
  if(QuickQuoteChk)
   PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&BuildQuickQuoteItem));

  return 0;
}
//---------------------------------------------------------------------------

int __stdcall OnXMLIDDebug (WPARAM wParam, LPARAM lParam)
{
  UnicodeString XML = (wchar_t*)wParam;
  //Jezeli jest to pakiet z lista czatow
  if(XML.Pos("<query xmlns='http://jabber.org/protocol/disco#items'>"))
  {
	XML = UTF8ToUnicodeString(XML.t_str());
	_di_IXMLDocument XMLDoc = LoadXMLData(XML);
	_di_IXMLNode Nodes = XMLDoc->DocumentElement;
	Nodes = Nodes->ChildNodes->GetNode(0);
	int ItemsCount = Nodes->ChildNodes->GetCount();
	TIniFile *Ini = new TIniFile(SessionFileDir);
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
	  Channel = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_NORMALIZE,0,(LPARAM)Channel.w_str()));
	  //Zapisywanie nazwy kanalu
	  Ini->WriteString("Channels",JID,Channel);
	}
	delete Ini;
  }

  return 0;
}
//---------------------------------------------------------------------------

int __stdcall OnSystemPopUp (WPARAM wParam, LPARAM lParam)
{
  PopUp = (PPluginPopUp)lParam;
  UnicodeString PopUpName = (wchar_t*)(PopUp->Name);
  if(PopUpName=="popTab")
  {
	Contact = (PPluginContact)wParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	if(Contact->IsChat) JID = "ischat_" + JID;
	ClipTabPopup = JID;
	//Jezeli zakladka nie jest przypieta
	if(ClipTabsList->IndexOf(ClipTabPopup)==-1)
	{
	  TPluginActionEdit PluginActionEdit;
	  PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
	  PluginActionEdit.pszName = L"ClipTabItem";
	  PluginActionEdit.Caption = L"Przypnij zak³adkê";
	  PluginActionEdit.Enabled = true;
	  PluginActionEdit.Visible = true;
	  PluginActionEdit.IconIndex = -1;
	  PluginActionEdit.Checked = false;
	  PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
	}
	else
	{
      TPluginActionEdit PluginActionEdit;
	  PluginActionEdit.cbSize = sizeof(TPluginActionEdit);
	  PluginActionEdit.pszName = L"ClipTabItem";
	  PluginActionEdit.Caption = L"Odepnij zak³adkê";
	  PluginActionEdit.Enabled = true;
	  PluginActionEdit.Visible = true;
	  PluginActionEdit.IconIndex = -1;
	  PluginActionEdit.Checked = false;
	  PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&PluginActionEdit));
	}
  }
  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmianê kompozycji
int __stdcall OnThemeChanged (WPARAM wParam, LPARAM lParam)
{
  //Pobieranie sciezki nowej aktywnej kompozycji
  UnicodeString ThemeDir = (wchar_t*)lParam;//(PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0));
  ThemeDir = StringReplace(ThemeDir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  ThemeDir = ThemeDir + "\\\\TabKit\\\\";
  UnicodeString PluginDir = GetPluginUserDir();
  PluginDir = PluginDir + "\\\\TabKit\\\\";
  //Aktualizacja ikon z interfejsu
  //UNSENTMSG
  if(FileExists(ThemeDir + "UnsentMsg.png"))
   UNSENTMSG = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,UNSENTMSG, (LPARAM)(ThemeDir + "UnsentMsg.png").w_str());
  else
   UNSENTMSG = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,UNSENTMSG, (LPARAM)(PluginDir + "UnsentMsg.png").w_str());
  //CLOSEDTABS
  if(FileExists(ThemeDir + "ClosedTabsButton.png"))
   CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,CLOSEDTABS, (LPARAM)(ThemeDir + "ClosedTabsButton.png").w_str());
  else
   CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,CLOSEDTABS, (LPARAM)(PluginDir + "ClosedTabsButton.png").w_str());
  //STAYONTOP_OFF
  if(FileExists(ThemeDir + "StayOnTopOff.png"))
   STAYONTOP_OFF = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,STAYONTOP_OFF, (LPARAM)(ThemeDir + "StayOnTopOff.png").w_str());
  else
   STAYONTOP_OFF = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,STAYONTOP_OFF, (LPARAM)(PluginDir + "StayOnTopOff.png").w_str());
  //STAYONTOP_ON
  if(FileExists(ThemeDir + "StayOnTopOn.png"))
   STAYONTOP_ON = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,STAYONTOP_ON, (LPARAM)(ThemeDir + "StayOnTopOn.png").w_str());
  else
   STAYONTOP_ON = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,STAYONTOP_ON, (LPARAM)(PluginDir + "StayOnTopOn.png").w_str());
  //COMPOSING
  if(FileExists(ThemeDir + "Composing.png"))
   COMPOSING = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,COMPOSING, (LPARAM)(ThemeDir + "Composing.png").w_str());
  else
   COMPOSING = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,COMPOSING, (LPARAM)(PluginDir + "Composing.png").w_str());
  //PAUSE
  if(FileExists(ThemeDir + "Pause.png"))
   PAUSE = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,PAUSE, (LPARAM)(ThemeDir + "Pause.png").w_str());
  else
   PAUSE = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,PAUSE, (LPARAM)(PluginDir + "Pause.png").w_str());
  //Przypisanie nowych sciezek do ikon
  //Composing_Small
  if(FileExists(ThemeDir + "Composing_Small.ico"))
   ComposingIconSmall = ThemeDir + "Composing_Small.ico";
  else
   ComposingIconSmall = PluginDir + "Composing_Small.ico";
  //Composing_Big
  if(FileExists(ThemeDir + "Composing_Big.ico"))
   ComposingIconBig = ThemeDir + "Composing_Big.ico";
  else
   ComposingIconBig = PluginDir + "Composing_Big.ico";
  //Pause_Small
  if(FileExists(ThemeDir + "Pause_Small.ico"))
   PauseIconSmall = ThemeDir + "Pause_Small.ico";
  else
   PauseIconSmall = PluginDir + "Pause_Small.ico";
  //Pause_Big
  if(FileExists(ThemeDir + "Pause_Big.ico"))
   PauseIconBig = ThemeDir + "Pause_Big.ico";
  else
   PauseIconBig = PluginDir + "Pause_Big.ico";

  return 0;
}
//---------------------------------------------------------------------------

//Hook na pokazywane wiadomosci
int __stdcall OnAddLine (WPARAM wParam, LPARAM lParam)
{
  if(CollapseImagesChk)
  {
	Message = (PPluginMessage)lParam;
	UnicodeString Body = (wchar_t*)Message->Body;
	if(Body.Pos("<IMG CLASS=\"aqqcacheitem\""))
	{
	  //Zwijanie tylko dla wyslanych obrazkow
	  if(CollapseImagesModeChk==2)
	  {
		UnicodeString MessageJID = (wchar_t*)(Message->JID);
		if(MessageJID.Pos("/")>0) MessageJID.Delete(MessageJID.Pos("/"),MessageJID.Length());
		Contact = (PPluginContact)wParam;
		UnicodeString ContactJID = (wchar_t*)(Contact->JID);
		if(MessageJID==ContactJID) return 0;
	  }
	  //Zwijanie tylko dla odebranych obrazkow
	  else if(CollapseImagesModeChk==3)
	  {
		UnicodeString MessageJID = (wchar_t*)(Message->JID);
		if(MessageJID.Pos("/")>0) MessageJID.Delete(MessageJID.Pos("/"),MessageJID.Length());
		Contact = (PPluginContact)wParam;
		UnicodeString ContactJID = (wchar_t*)(Contact->JID);
		if(MessageJID!=ContactJID) return 0;
      }
	  //Styl zalacznoika
	  UnicodeString NewBody = "<span id=\"icon\" style=\"margin-right: 2px;\">CC_ATTACH_ICON</span><div class=\"caption\" style=\"display: inline;\"><span id=\"caption\">CC_ATTACH_CAPTION</span>: <span id=\"title\">CC_ATTACH_SHORT</span></div>";
	  //Generowanie ID sesji
	  UnicodeString Session = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETSTRID,0,0));
	  //Nazwa obrazka
	  UnicodeString PhotoFileName = Body;
	  PhotoFileName = PhotoFileName.Delete(1,PhotoFileName.Pos("ALT=\"")+4);
	  PhotoFileName = PhotoFileName.Delete(PhotoFileName.Pos("\""),PhotoFileName.Length());
	  //Pobieranie sciezki URL do grafiki zalacznika
	  UnicodeString ThemePNGPath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,40,0));
	  //Modyfikacja oryginalnej wiadomosci
	  Body = StringReplace(Body, "\\", "/", TReplaceFlags() << rfReplaceAll);
	  Body = StringReplace(Body, "A HREF=\"", "A HREF=\"image:" + Session + ":file:///", TReplaceFlags());
	  Body = Body.Delete(Body.Pos("<IMG"),Body.Length());
	  Body = Body + PhotoFileName + "</A>";
	  //Generowanie nowej tresci
	  NewBody = StringReplace(NewBody, "CC_ATTACH_ICON", "<IMG border=\"0\" src=\"" + ThemePNGPath + "\">", TReplaceFlags());
	  NewBody = StringReplace(NewBody, "CC_ATTACH_CAPTION", "<SPAN id=\"id_cctext\">Obrazek</SPAN>", TReplaceFlags());
	  NewBody = StringReplace(NewBody, "CC_ATTACH_SHORT", "<SPAN id=\"id_cctext\"><SPAN id=\"" + Session + "\">" + Body + "</SPAN></SPAN>", TReplaceFlags());
	  //Zmienianie tresci wiadomosci w notyfikacji
	  Message->Body = NewBody.w_str();
	  lParam = (LPARAM)Message;
	  return 2;
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na restartowanie AQQ poprzez wtyczke AQQRestarter
int __stdcall OnRestartingAQQ(WPARAM wParam, LPARAM lParam)
{
  if((RestoreTabsSessionChk)&&(!ManualRestoreTabsSessionChk))
   ManualRestoreTabsSessionChk = true;

  return 0;
}
//---------------------------------------------------------------------------

//Hook na klawiature
extern "C" LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  //Blad
  if(nCode<0) return CallNextHookEx(hKeyboard, nCode, wParam, lParam);

  //Inteligentne przelaczenia zakladek
  if(SwitchToNewMsgChk)
  {
	//Wcisniety Ctrl+Tab
	if((GetKeyState(VK_CONTROL)<0)&&((int)wParam==9))
	{
	  if(!(HIWORD(lParam)&KF_UP))
	  {
		//Srawdzanie aktywnego okna
		hActiveFrm = GetActiveWindow();
		GetClassNameW(hActiveFrm, WClassName, sizeof(WClassName));
		GetWindowThreadProcessId(hActiveFrm, &PID);
		//Jezeli aktywne okno to okno rozmowy AQQ
		if(((UnicodeString)WClassName=="TfrmSend")&&(PID==ProcessPID))
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

			if(!JID.IsEmpty())
			{
			  //Przelaczanie na zakladke z danym kontektem
			  if(!JID.Pos("ischat_"))
			   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
		  hActiveFrm = GetActiveWindow();
		  GetClassNameW(hActiveFrm, WClassName, sizeof(WClassName));
		  GetWindowThreadProcessId(hActiveFrm, &PID);
		  //Jezeli aktywne okno to okno rozmowy AQQ
		  if(((UnicodeString)WClassName=="TfrmSend")&&(PID==ProcessPID))
		  {
			//Jezeli nie wcisnieto Ctrl/Alt/Shift
			if((GetKeyState(VK_CONTROL)>=0)&&(GetKeyState(VK_MENU)>=0)&&(GetKeyState(VK_SHIFT)>=0))
			{
			  //Identyfikacja F'a
			  int Key = (int)wParam - 111;
			  //Usuwanie listy zakladek
			  TabsList->Clear();
			  //Hook na pobieranie aktywnych zakladek
			  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs);
			  PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
			  PluginLink.UnhookEvent(OnFetchAllTabs);
              //Sprawdzanie czy wywolujemy zakladke "ducha"
			  if(Key<=TabsList->Count)
			  {
                //Pobieranie JID
				UnicodeString JID = TabsList->Strings[Key-1];
				//Sprawdzanie rodzaju kontaktu
				if(!JID.IsEmpty())
				{
				  //Przelaczanie na zakladke z danym kontektem
				  if(!JID.Pos("ischat_"))
				   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
		if(!(HIWORD(lParam)&KF_UP))//KF_EXTENDED))
		{
		  //Srawdzanie aktywnego okna
		  hActiveFrm = GetActiveWindow();
		  GetClassNameW(hActiveFrm, WClassName, sizeof(WClassName));
		  GetWindowThreadProcessId(hActiveFrm, &PID);
		  //Jezeli aktywne okno to okno rozmowy AQQ
		  if(((UnicodeString)WClassName=="TfrmSend")&&(PID==ProcessPID))
		  {
			//Identyfikacja klawisza
			int Key = (int)wParam - 48;
			//Usuwanie listy zakladek
			TabsList->Clear();
			//Hook na pobieranie aktywnych zakladek
			PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs);
			PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
			PluginLink.UnhookEvent(OnFetchAllTabs);
			//Sprawdzanie czy wywolujemy zakladke "ducha"
			if(Key<=TabsList->Count)
			{
			  //Pobieranie JID
			  UnicodeString JID = TabsList->Strings[Key-1];
			  //Sprawdzanie rodzaju kontaktu
			  if(!JID.IsEmpty())
			  {
				//Przelaczanie na zakladke z danym kontektem
				if(!JID.Pos("ischat_"))
				 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
		   hActiveFrm = GetActiveWindow();
		   GetClassNameW(hActiveFrm, WClassName, sizeof(WClassName));
		   GetWindowThreadProcessId(hActiveFrm, &PID);
		   //Jezeli aktywne okno to okno rozmowy lub glowne okno AQQ
		   if((((UnicodeString)WClassName=="TfrmSend")||((UnicodeString)WClassName=="TfrmMain"))&&(PID==ProcessPID))
		   {
			 if(ClosedTabsList->Count>0)
			 {
			   UnicodeString JID = ClosedTabsList->Strings[0];
			   //Zapisawanie JID aktualnie przywracanej zakladki
			   JustUnClosedJID = JID;
			   //Przelaczanie na ostatnio zamknieta zakladke z danym kontektem
			   if(!JID.Pos("ischat_"))
				PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
			   //Przelaczanie na ostatnio zamknieta zakladke z czatem
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
				 PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
				 PluginChatPrep.UserIdx = 0;
				 PluginChatPrep.JID = JID.w_str();
				 PluginChatPrep.Channel = Channel.w_str();
				 PluginChatPrep.CreateNew = false;
				 PluginChatPrep.Fast = true;
				 PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
			   }
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
		hActiveFrm = GetActiveWindow();
		GetClassNameW(hActiveFrm, WClassName, sizeof(WClassName));
		GetWindowThreadProcessId(hActiveFrm, &PID);
		//Jezeli aktywne okno to okno rozmowy lub glowne okno AQQ
		if((((UnicodeString)WClassName=="TfrmSend")||((UnicodeString)WClassName=="TfrmMain"))&&(PID==ProcessPID))
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
				 UnicodeString JID = ClosedTabsList->Strings[0];
				 //Zapisawanie JID aktualnie przywracanej zakladki
				 JustUnClosedJID = JID;
				 //Przelaczanie na ostatnio zamknieta zakladke z danym kontektem
				 if(!JID.Pos("ischat_"))
				  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
				 //Przelaczanie na ostatnio zamknieta zakladke z czatem
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
				   PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
				   PluginChatPrep.UserIdx = 0;
				   PluginChatPrep.JID = JID.w_str();
				   PluginChatPrep.Channel = Channel.w_str();
				   PluginChatPrep.CreateNew = false;
				   PluginChatPrep.Fast = true;
				   PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
				 }
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
				 UnicodeString JID = ClosedTabsList->Strings[0];
				 //Zapisawanie JID aktualnie przywracanej zakladki
				 JustUnClosedJID = JID;
				 //Przelaczanie na ostatnio zamknieta zakladke z danym kontektem
				 if(!JID.Pos("ischat_"))
				  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
				 //Przelaczanie na ostatnio zamknieta zakladke z czatem
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
				   PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
				   PluginChatPrep.UserIdx = 0;
				   PluginChatPrep.JID = JID.w_str();
				   PluginChatPrep.Channel = Channel.w_str();
				   PluginChatPrep.CreateNew = false;
				   PluginChatPrep.Fast = true;
				   PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
				 }
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
				 UnicodeString JID = ClosedTabsList->Strings[0];
				 //Zapisawanie JID aktualnie przywracanej zakladki
				 JustUnClosedJID = JID;
				 //Przelaczanie na ostatnio zamknieta zakladke z danym kontektem
				 if(!JID.Pos("ischat_"))
				  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
				 //Przelaczanie na ostatnio zamknieta zakladke z czatem
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
				   PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
				   PluginChatPrep.UserIdx = 0;
				   PluginChatPrep.JID = JID.w_str();
				   PluginChatPrep.Channel = Channel.w_str();
					PluginChatPrep.CreateNew = false;
				   PluginChatPrep.Fast = true;
				   PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
				 }
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
				 UnicodeString JID = ClosedTabsList->Strings[0];
				 //Zapisawanie JID aktualnie przywracanej zakladki
				 JustUnClosedJID = JID;
				 //Przelaczanie na ostatnio zamknieta zakladke z danym kontektem
				 if(!JID.Pos("ischat_"))
				  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
				 //Przelaczanie na ostatnio zamknieta zakladke z czatem
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
				   PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
				   PluginChatPrep.UserIdx = 0;
				   PluginChatPrep.JID = JID.w_str();
				   PluginChatPrep.Channel = Channel.w_str();
				   PluginChatPrep.CreateNew = false;
				   PluginChatPrep.Fast = true;
				   PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
				 }
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
				 UnicodeString JID = ClosedTabsList->Strings[0];
				 //Zapisawanie JID aktualnie przywracanej zakladki
				 JustUnClosedJID = JID;
				 //Przelaczanie na ostatnio zamknieta zakladke z danym kontektem
				 if(!JID.Pos("ischat_"))
				  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
				 //Przelaczanie na ostatnio zamknieta zakladke z czatem
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
				   PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
				   PluginChatPrep.UserIdx = 0;
				   PluginChatPrep.JID = JID.w_str();
				   PluginChatPrep.Channel = Channel.w_str();
				   PluginChatPrep.CreateNew = false;
				   PluginChatPrep.Fast = true;
				   PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
				 }
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
				 UnicodeString JID = ClosedTabsList->Strings[0];
				 //Zapisawanie JID aktualnie przywracanej zakladki
				 JustUnClosedJID = JID;
				 //Przelaczanie na ostatnio zamknieta zakladke z danym kontektem
				 if(!JID.Pos("ischat_"))
				  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
				 //Przelaczanie na ostatnio zamknieta zakladke z czatem
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
				   PluginChatPrep.cbSize = sizeof(TPluginChatPrep);
				   PluginChatPrep.UserIdx = 0;
				   PluginChatPrep.JID = JID.w_str();
				   PluginChatPrep.Channel = Channel.w_str();
				   PluginChatPrep.CreateNew = false;
				   PluginChatPrep.Fast = true;
				   PluginLink.CallService(AQQ_SYSTEM_CHAT,0,(LPARAM)&PluginChatPrep);
				 }
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
	  if(!(HIWORD(lParam)&KF_UP))//KF_EXTENDED))
	  {
		//Srawdzanie aktywnego okna
		hActiveFrm = GetActiveWindow();
		GetClassNameW(hActiveFrm, WClassName, sizeof(WClassName));
		GetWindowThreadProcessId(hActiveFrm, &PID);
		//Jezeli aktywne okno to okno rozmowy AQQ
		if(((UnicodeString)WClassName=="TfrmSend")&&(PID==ProcessPID))
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
			  wchar_t WideBuffer[2048];
			  GetWindowTextW(hRichEdit, WideBuffer, sizeof(WideBuffer));
			  UnicodeString Text = WideBuffer;
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

  return CallNextHookEx(hKeyboard, nCode, wParam, lParam);
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
  else return false;
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

//Odczyt ustawien
void LoadSettings()
{
  TIniFile *Ini = new TIniFile(GetPluginUserDir() + "\\\\TabKit\\\\Settings.ini");
  //UnsentMsg
  UnsentMsgChk = Ini->ReadBool("UnsentMsg","Enable",true);
  InfoUnsentMsgChk = Ini->ReadBool("UnsentMsg","Info",true);
  CloudUnsentMsgChk = Ini->ReadBool("UnsentMsg","Cloud",true);
  DetailedCloudUnsentMsgChk = Ini->ReadBool("UnsentMsg","DetailedCloud",false);
  TrayUnsentMsgChk =  Ini->ReadBool("UnsentMsg","Tray",true);
  FastAccessUnsentMsgChk =  Ini->ReadBool("UnsentMsg","FastAccess",true);
  FrmMainUnsentMsgChk =  Ini->ReadBool("UnsentMsg","FrmMain",true);
  FrmSendUnsentMsgChk =  Ini->ReadBool("UnsentMsg","FrmSend",false);
  FastClearUnsentMsgChk = Ini->ReadBool("UnsentMsg","FastClear",false);
  //TabsSwitching
  SwitchToNewMsgChk =  Ini->ReadBool("TabsSwitching","SwitchToNewMsg",true);
  SwitchToNewMsgMode = Ini->ReadInteger("TabsSwitching","SwitchToNewMsgMode",1);
  TabsHotKeysChk = Ini->ReadBool("TabsSwitching","TabsHotKeys",true);
  TabsHotKeysMode = Ini->ReadInteger("TabsSwitching","TabsHotKeysMode",1);
  //ClosedTabs
  ClosedTabsChk = Ini->ReadBool("ClosedTabs","Enable",true);
  FastAccessClosedTabsChk =  Ini->ReadBool("ClosedTabs","FastAccess",true);
  FrmMainClosedTabsChk =  Ini->ReadBool("ClosedTabs","FrmMain",true);
  FrmSendClosedTabsChk =  Ini->ReadBool("ClosedTabs","FrmSend",false);
  ItemCountUnCloseTabVal = Ini->ReadInteger("ClosedTabs","ItemsCount",5);
  ShowTimeClosedTabsChk = Ini->ReadBool("ClosedTabs","ClosedTime",false);
  FastClearClosedTabsChk = Ini->ReadBool("ClosedTabs","FastClear",false);
  UnCloseTabHotKeyChk =  Ini->ReadBool("ClosedTabs","HotKey",false);
  UnCloseTabHotKeyMode = Ini->ReadInteger("ClosedTabs","HotKeyMode",1);
  UnCloseTabHotKeyDef = Ini->ReadInteger("ClosedTabs","HotKeyDef",0);
  CountUnCloseTabVal = Ini->ReadInteger("ClosedTabs","Count",10);
  RestoreLastMsgChk = Ini->ReadBool("ClosedTabs","RestoreLastMsg",false);
  OnlyConversationTabsChk = Ini->ReadBool("ClosedTabs","OnlyConversationTabs",false);
  //SessionRemember
  RestoreTabsSessionChk = Ini->ReadBool("SessionRemember","RestoreTabs",true);
  ManualRestoreTabsSessionChk = Ini->ReadBool("SessionRemember","ManualRestoreTabs",false);
  RestoreMsgSessionChk = Ini->ReadBool("SessionRemember","RestoreMsg",false);
  //Titlebar
  TweakFrmSendTitlebarChk = Ini->ReadBool("Titlebar","TweakSend",false);
  TweakFrmSendTitlebarMode = Ini->ReadInteger("Titlebar","SendMode",1);
  TweakFrmMainTitlebarChk = Ini->ReadBool("Titlebar","TweakMain",false);
  TweakFrmMainTitlebarMode = Ini->ReadInteger("Titlebar","MainMode",1);
  TweakFrmMainTitlebarText = Ini->ReadString("Titlebar","MainText","");
  //NewMsg
  InactiveFrmNewMsgChk = Ini->ReadBool("NewMsg","InactiveFrm",true);
  InactiveTabsNewMsgChk = Ini->ReadBool("NewMsg","InactiveTabs",true);
  InactiveNotiferNewMsgChk = Ini->ReadBool("NewMsg","InactiveNotifer",false);
  ChatStateNotiferNewMsgChk = Ini->ReadBool("NewMsg","ChatStateNotifer",true);
  //ClipTabs
  OpenClipTabsChk = Ini->ReadBool("ClipTabs","OpenClipTabs",true);
  InactiveClipTabsChk = Ini->ReadBool("ClipTabs","InactiveClipTabs",false);
  CounterClipTabsChk = Ini->ReadBool("ClipTabs","Counter",false);
  //Other
  StayOnTopChk = Ini->ReadBool("Other","StayOnTop",false);
  EmuTabsWChk = Ini->ReadBool("Other","EmuTabsW",false);
  if(!EmuTabsWSupport) EmuTabsWChk = false;
  QuickQuoteChk = Ini->ReadBool("Other","QuickQuote",false);
  AntiSpimChk = Ini->ReadBool("Other","AntiSpim",true);
  HideStatusBarChk = Ini->ReadBool("Other","HideStatusBar",false);
  HideToolBarChk = Ini->ReadBool("Other","HideToolBar",false);
  CollapseImagesChk = Ini->ReadBool("Other","CollapseImages",false);
  CollapseImagesModeChk = Ini->ReadInteger("Other","CollapseImagesMode",1);
  if(CollapseImagesModeChk==1) Ini->WriteInteger("Other","CollapseImagesMode",1);
  CloudTimeOutChk = Ini->ReadInteger("Other","CloudTimeOut",6);
  if(CloudTimeOutChk==6) Ini->WriteInteger("Other","CloudTimeOut",6);
  CloudTickModeChk = Ini->ReadBool("Other","CloudTickMode",true);
  if(CloudTickModeChk) Ini->WriteBool("Other","CloudTickMode",true);
  delete Ini;
}
//---------------------------------------------------------------------------

//Zapisywanie zasobów
bool SaveResourceToFile(char *FileName, char *res)
{
  HRSRC hrsrc = FindResource(HInstance, res, RT_RCDATA);
  if(!hrsrc) return false;
  DWORD size = SizeofResource(HInstance, hrsrc);
  HGLOBAL hglob = LoadResource(HInstance, hrsrc);
  LPVOID rdata = LockResource(hglob);
  HANDLE hFile = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  DWORD writ;
  WriteFile(hFile, rdata, size, &writ, NULL);
  CloseHandle(hFile);
  return true;
}
//---------------------------------------------------------------------------

String __fastcall MD5File(const String FileName)
{
  if(FileExists(FileName))
  {
    String Result;
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

//Zaladowanie wtyczki
extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;

  //Sprawdzanie czy wlaczona jest obsluga zakladek
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0)));
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
  }
  //Sprawdzanie czy uzywany OS to maksimum XP
  EmuTabsWSupport = CheckEmuTabsWSupport();
  //Pobieranie PID procesu AQQ
  ProcessPID = getpid();
  //Sciezka katalogu prywatnego uzytkownika
  SessionFileDir = GetPluginUserDir();
  //Folder z ustawieniami wtyczki etc
  if(!DirectoryExists(SessionFileDir + "\\\\TabKit"))
   CreateDir(SessionFileDir + "\\\\TabKit");
  if(!DirectoryExists(SessionFileDir + "\\\\TabKit\\\\Avatars"))
   CreateDir(SessionFileDir + "\\\\TabKit\\\\Avatars");
  //Usuwanie wtyczki TitlebarTweak i przenoszenie ustawien
  if(FileExists(SessionFileDir + "\\\\TitlebarTweak.dll"))
  {
	//Odczyt ustawien TitlebarTweak
	TIniFile *Ini = new TIniFile(SessionFileDir + "\\\\TitlebarTweak\\\\Settings.ini");
	int pFrmSendMode = Ini->ReadInteger("FrmSend", "Mode", 0);
	int pFrmMainMode = Ini->ReadInteger("FrmMain", "Mode", 0);
	UnicodeString pFrmMainText = Ini->ReadString("FrmMain", "Mode2Text", "");
	delete Ini;
	//Przenoszenie ustawien
	TIniFile *Ini2 = new TIniFile(SessionFileDir + "\\\\TabKit\\\\Settings.ini");
	if(pFrmSendMode>0)
	{
	  Ini2->WriteBool("Titlebar","TweakSend",true);
	  Ini2->WriteInteger("Titlebar","SendMode",pFrmSendMode);
	}
	if(pFrmMainMode>0)
	{
	  Ini2->WriteBool("Titlebar","TweakMain",true);
	  Ini2->WriteInteger("Titlebar","MainMode",pFrmSendMode);
	  Ini2->WriteString("Titlebar","MainText",pFrmMainText);
	}
	delete Ini2;
	//Usuwanie folderu ustawien TitlebarTweak
	DeleteFile(SessionFileDir + "\\\\TitlebarTweak\\\\Settings.ini");
	RemoveDir(SessionFileDir + "\\\\TitlebarTweak");
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
	//Wylaczenie wtyczki
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_ACTIVE,0,(LPARAM)(PluginDirW + "\\TitlebarTweak.dll").w_str());
	//Usuwanie wtyczki
	DeleteFile(SessionFileDir + "\\\\TitlebarTweak.dll");
	//Usuwanie wtyczki z wyjatkow
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_EXCLUDE,1,(LPARAM)(PluginDirW + "\\TitlebarTweak.dll").w_str());
  }
  //Usuwanie wtyczki StayOnTop
  if(FileExists(SessionFileDir + "\\\\StayOnTop.dll"))
  {
	//Wlaczanie StayOnTop w TabKit
	TIniFile *Ini = new TIniFile(SessionFileDir + "\\\\TabKit\\\\Settings.ini");
	Ini->WriteBool("Other","StayOnTop",true);
	delete Ini;
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
	//Wylaczenie wtyczki
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_ACTIVE,0,(LPARAM)(PluginDirW + "\\StayOnTop.dll").w_str());
	//Usuwanie wtyczki
	DeleteFile(SessionFileDir + "\\\\StayOnTop.dll");
	//Usuwanie wtyczki z wyjatkow
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_EXCLUDE,1,(LPARAM)(PluginDirW + "\\StayOnTop.dll").w_str());
  }
  //Usuwanie wtyczki MsgNotifier
  if(FileExists(SessionFileDir + "\\\\MsgNotifier.dll"))
  {
	//Wlaczanie InactiveNotifer w TabKit
	TIniFile *Ini = new TIniFile(SessionFileDir + "\\\\TabKit\\\\Settings.ini");
	Ini->WriteBool("NewMsg","InactiveNotifer",true);
	delete Ini;
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
	//Wylaczenie wtyczki
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_ACTIVE,0,(LPARAM)(PluginDirW + "\\MsgNotifier.dll").w_str());
	//Usuwanie wtyczki
	DeleteFile(SessionFileDir + "\\\\MsgNotifier.dll");
	//Usuwanie pliku ustawien
	DeleteFile(SessionFileDir + "\\\\MsgNotifier.ini");
	//Usuwanie wtyczki z wyjatkow
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_EXCLUDE,1,(LPARAM)(PluginDirW + "\\MsgNotifier.dll").w_str());
  }
  //Usuwanie wtyczki EmuTabsW
  if(FileExists(SessionFileDir + "\\\\EmuTabsW.dll"))
  {
	//Wlaczanie EmuTabsW w TabKit
	TIniFile *Ini = new TIniFile(SessionFileDir + "\\\\TabKit\\\\Settings.ini");
	Ini->WriteBool("Other","EmuTabsW",true);
	delete Ini;
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
	//Wylaczenie wtyczki
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_ACTIVE,0,(LPARAM)(PluginDirW + "\\EmuTabsW.dll").w_str());
	//Usuwanie wtyczki
	DeleteFile(SessionFileDir + "\\\\EmuTabsW.dll");
	//Usuwanie wtyczki z wyjatkow
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_EXCLUDE,1,(LPARAM)(PluginDirW + "\\EmuTabsW.dll").w_str());
  }
  //Usuwanie wtyczki QuickQuote
  if(FileExists(SessionFileDir + "\\\\QuickQuote.dll"))
  {
	//Wlaczanie QuickQuote w TabKit
	TIniFile *Ini = new TIniFile(SessionFileDir + "\\\\TabKit\\\\Settings.ini");
	Ini->WriteBool("Other","QuickQuote",true);
	delete Ini;
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
	//Wylaczenie wtyczki
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_ACTIVE,0,(LPARAM)(PluginDirW + "\\QuickQuote.dll").w_str());
	//Usuwanie wtyczki
	DeleteFile(SessionFileDir + "\\\\QuickQuote.dll");
	//Usuwanie wtyczki z wyjatkow
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_EXCLUDE,1,(LPARAM)(PluginDirW + "\\QuickQuote.dll").w_str());
  }
  //Usuwanie wtyczki TabFocus
  if(FileExists(SessionFileDir + "\\\\TabFocus.dll"))
  {
	//Wlaczanie funkcji TabFocus w TabKit
	TIniFile *Ini = new TIniFile(SessionFileDir + "\\\\TabKit\\\\Settings.ini");
	Ini->WriteBool("NewMsg","InactiveFrm",true);
	Ini->WriteBool("NewMsg","InactiveTabs",true);
	Ini->WriteBool("NewMsg","ChatStateNotifer",true);
	delete Ini;
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
	//Wylaczenie wtyczki
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_ACTIVE,0,(LPARAM)(PluginDirW + "\\TabFocus.dll").w_str());
	//Usuwanie wtyczki
	DeleteFile(SessionFileDir + "\\\\TabFocus.dll");
	//Usuwanie wtyczki z wyjatkow
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_EXCLUDE,1,(LPARAM)(PluginDirW + "\\TabFocus.dll").w_str());
  }
  //Usuwanie wtyczki OffAntiSpim
  if(FileExists(SessionFileDir + "\\\\OffAntiSpim.dll"))
  {
	//Wlaczanie funkcji TabKit w TabKit
	TIniFile *Ini = new TIniFile(SessionFileDir + "\\\\TabKit\\\\Settings.ini");
	Ini->WriteBool("Other","AntiSpim",false);
	delete Ini;
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
	//Wylaczenie wtyczki
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_ACTIVE,0,(LPARAM)(PluginDirW + "\\OffAntiSpim.dll").w_str());
	//Usuwanie wtyczki
	DeleteFile(SessionFileDir + "\\\\OffAntiSpim.dll");
	//Usuwanie wtyczki z wyjatkow
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_EXCLUDE,1,(LPARAM)(PluginDirW + "\\OffAntiSpim.dll").w_str());
  }
  //Usuwanie wtyczki CzasoWstrzymywacz
  if(FileExists(SessionFileDir + "\\\CzasoWstrzymywacz.dll"))
  {
	//Wlaczanie funkcji CzasoWstrzymywacz w TabKit
	TIniFile *Ini = new TIniFile(SessionFileDir + "\\\\TabKit\\\\Settings.ini");
	Ini->WriteBool("Other","HideStatusBar",true);
	delete Ini;
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0));
	//Wylaczenie wtyczki
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_ACTIVE,0,(LPARAM)(PluginDirW + "\\CzasoWstrzymywacz.dll").w_str());
	//Usuwanie wtyczki
	DeleteFile(SessionFileDir + "\\\\CzasoWstrzymywacz.dll");
	//Usuwanie wtyczki z wyjatkow
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_EXCLUDE,1,(LPARAM)(PluginDirW + "\\CzasoWstrzymywacz.dll").w_str());
  }
  //Wypakiwanie ikonki UnsentMsg.png
  //8D6126252C378C198BDA6A2F249B2B7C
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png").t_str(),"UNSENTMSG");
  else if(MD5File(SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png")!="8D6126252C378C198BDA6A2F249B2B7C")
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png").t_str(),"UNSENTMSG");
  //Wypakiwanie ikonki ClosedTabsButton.png
  //1298BD768C95081257A09FF8A5590536
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\ClosedTabsButton.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\ClosedTabsButton.png").t_str(),"CLOSEDTABS");
  else if(MD5File(SessionFileDir + "\\\\TabKit\\\\ClosedTabsButton.png")!="1298BD768C95081257A09FF8A5590536")
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\ClosedTabsButton.png").t_str(),"CLOSEDTABS");
  //Wypakowanie ikonki StayOnTopOff.png
  //439D5BC24FDC9CBB95EC4D4D9B7D5B57
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\StayOnTopOff.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\StayOnTopOff.png").t_str(),"STAYONTOP_OFF");
  else if(MD5File(SessionFileDir + "\\\\TabKit\\\\StayOnTopOff.png")!="439D5BC24FDC9CBB95EC4D4D9B7D5B57")
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\StayOnTopOff.png").t_str(),"STAYONTOP_OFF");
  //Wypakiwanie ikonki StayOnTopOn.png
  //2B252A5BFB248BE10277CB97DB0AD437
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\StayOnTopOn.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\StayOnTopOn.png").t_str(),"STAYONTOP_ON");
  else if(MD5File(SessionFileDir + "\\\\TabKit\\\\StayOnTopOn.png")!="2B252A5BFB248BE10277CB97DB0AD437")
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\StayOnTopOn.png").t_str(),"STAYONTOP_ON");
  //Wypakiwanie ikonki Composing_Small.ico
  //986AD6EB91C11CC47833CDC0778523F5
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\Composing_Small.ico"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Composing_Small.ico").t_str(),"COMPOSING_SMALL");
  else if(MD5File(SessionFileDir + "\\\\TabKit\\\\Composing_Small.ico")!="986AD6EB91C11CC47833CDC0778523F5")
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Composing_Small.ico").t_str(),"COMPOSING_SMALL");
  //Wypakiwanie ikonki Composing_Big.ico
  //BEC55EC214FB058B97AFE463D15818D4
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\Composing_Big.ico"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Composing_Big.ico").t_str(),"COMPOSING_BIG");
  else if(MD5File(SessionFileDir + "\\\\TabKit\\\\Composing_Big.ico")!="BEC55EC214FB058B97AFE463D15818D4")
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Composing_Big.ico").t_str(),"COMPOSING_BIG");
  //Wypakiwanie ikonki Pause_Small.ico
  //46D62DD885A071E17F9BF8CCE101649F
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\Pause_Small.ico"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Pause_Small.ico").t_str(),"PAUSE_SMALL");
  else if(MD5File(SessionFileDir + "\\\\TabKit\\\\Pause_Small.ico")!="46D62DD885A071E17F9BF8CCE101649F")
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Pause_Small.ico").t_str(),"PAUSE_SMALL");
  //Wypakiwanie ikonki Pause_Big.ico
  //F1FF8A4FF8DCD54BE7A0A34F4187CF48
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\Pause_Big.ico"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Pause_Big.ico").t_str(),"PAUSE_BIG");
  else if(MD5File(SessionFileDir + "\\\\TabKit\\\\Pause_Big.ico")!="F1FF8A4FF8DCD54BE7A0A34F4187CF48")
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Pause_Big.ico").t_str(),"PAUSE_BIG");
  //Wypakiwanie ikonki Composing.png
  //73BB34FBD1BE53599C470D9A408EF854
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\Composing.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Composing.png").t_str(),"COMPOSING");
  else if(MD5File(SessionFileDir + "\\\\TabKit\\\\Composing.png")!="73BB34FBD1BE53599C470D9A408EF854")
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Composing.png").t_str(),"COMPOSING");
  //Wypakiwanie ikonki Pause_Big.ico
  //756722004F72913D6248D58176EBB788
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\Pause.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Pause.png").t_str(),"PAUSE");
  else if(MD5File(SessionFileDir + "\\\\TabKit\\\\Pause.png")!="756722004F72913D6248D58176EBB788")
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\Pause.png").t_str(),"PAUSE");
  //Przypisanie ikonek do interfejsu AQQ
  UnicodeString ThemeDir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0));
  ThemeDir = StringReplace(ThemeDir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  ThemeDir = ThemeDir + "\\\\TabKit\\\\";
  //UNSENTMSG
  if(!FileExists(ThemeDir + "UnsentMsg.png"))
   UNSENTMSG = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png").w_str());
  else
   UNSENTMSG = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "UnsentMsg.png").w_str());
  //CLOSEDTABS
  if(!FileExists(ThemeDir + "ClosedTabsButton.png"))
   CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\ClosedTabsButton.png").w_str());
  else
   CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "ClosedTabsButton.png").w_str());
  //STAYONTOP_OFF
  if(!FileExists(ThemeDir + "StayOnTopOff.png"))
   STAYONTOP_OFF = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\StayOnTopOff.png").w_str());
  else
   STAYONTOP_OFF = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "StayOnTopOff.png").w_str());
  //STAYONTOP_ON
  if(!FileExists(ThemeDir + "StayOnTopOn.png"))
   STAYONTOP_ON = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\StayOnTopOn.png").w_str());
  else
   STAYONTOP_ON = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "StayOnTopOn.png").w_str());
  //COMPOSING
  if(!FileExists(ThemeDir + "Composing.png"))
   COMPOSING = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\Composing.png").w_str());
  else
   COMPOSING = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "Composing.png").w_str());
  //PAUSE
  if(!FileExists(ThemeDir + "Pause.png"))
   PAUSE = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\Pause.png").w_str());
  else
   PAUSE = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(ThemeDir + "Pause.png").w_str());
  //Przypisanie sciezek do ikon
  //Composing_Small
  if(!FileExists(ThemeDir + "Composing_Small.ico"))
   ComposingIconSmall = SessionFileDir + "\\\\TabKit\\\\Composing_Small.ico";
  else
   ComposingIconSmall = ThemeDir + "Composing_Small.ico";
  //Composing_Big
  if(!FileExists(ThemeDir + "Composing_Big.ico"))
   ComposingIconBig = SessionFileDir + "\\\\TabKit\\\\Composing_Big.ico";
  else
   ComposingIconBig = ThemeDir + "Composing_Big.ico";
  //Pause_Small
  if(!FileExists(ThemeDir + "Pause_Small.ico"))
   PauseIconSmall = SessionFileDir + "\\\\TabKit\\\\Pause_Small.ico";
  else
   PauseIconSmall = ThemeDir + "Pause_Small.ico";
  //Pause_Big
  if(!FileExists(ThemeDir + "Pause_Big.ico"))
   PauseIconBig = SessionFileDir + "\\\\TabKit\\\\Pause_Big.ico";
  else
   PauseIconBig = ThemeDir + "Pause_Big.ico";
  //Dodanie sciezki do pliku sesji
  SessionFileDir = SessionFileDir + "\\\\TabKit\\\\Session.ini";
  //Definiowanie nazwy uzywanego profilu
  ActiveProfileName = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,(WPARAM)0,0);
  if(ActiveProfileName.Pos("\\")>0)
   while(ActiveProfileName.Pos("\\")>0)
	ActiveProfileName.Delete(1,ActiveProfileName.Pos("\\"));
  //Usuwanie PopUpMenu
  //Szybki dostep niewyslanych wiadomosci
  FrmUnsentMsgPopUp.cbSize = sizeof(TPluginAction);
  FrmUnsentMsgPopUp.pszName = L"FrmUnsentMsgPopUp";
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU ,0,(LPARAM)(&FrmUnsentMsgPopUp));
  //Szybki dostep ostatnio zamknietych zakladek
  FrmClosedTabsPopUp.cbSize = sizeof(TPluginAction);
  FrmClosedTabsPopUp.pszName = L"FrmClosedTabsPopUp";
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU,0,(LPARAM)(&FrmClosedTabsPopUp));
  //Tworzenie PopUpMenu
  //Szybki dostep niewyslanych wiadomosci
  FrmUnsentMsgPopUp.cbSize = sizeof(TPluginAction);
  FrmUnsentMsgPopUp.pszName = L"FrmUnsentMsgPopUp";
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENU,0,(LPARAM)(&FrmUnsentMsgPopUp));
  //Szybki dostep ostatnio zamknietych zakladek
  FrmClosedTabsPopUp.cbSize = sizeof(TPluginAction);
  FrmClosedTabsPopUp.pszName = L"FrmClosedTabsPopUp";
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENU,0,(LPARAM)(&FrmClosedTabsPopUp));
  //Tworzenie serwisow
  //Szybki dostep niewyslanych wiadomosci
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem0",ServiceFrmUnsentMsgItem0);
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem1",ServiceFrmUnsentMsgItem1);
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem2",ServiceFrmUnsentMsgItem2);
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem3",ServiceFrmUnsentMsgItem3);
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem4",ServiceFrmUnsentMsgItem4);
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItemClear",ServiceFrmUnsentMsgItemClear);
  //Szybki dostep ostatnio zamknietych zakladek
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem0",ServiceFrmClosedTabsItem0);
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem1",ServiceFrmClosedTabsItem1);
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem2",ServiceFrmClosedTabsItem2);
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem3",ServiceFrmClosedTabsItem3);
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem4",ServiceFrmClosedTabsItem4);
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem5",ServiceFrmClosedTabsItem5);
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem6",ServiceFrmClosedTabsItem6);
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem7",ServiceFrmClosedTabsItem7);
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem8",ServiceFrmClosedTabsItem8);
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem9",ServiceFrmClosedTabsItem9);
  PluginLink.CreateServiceFunction(L"sFrmClosedTabsItemClear",ServiceFrmClosedTabsItemClear);
  //Trzymanie okna na wierzchu
  PluginLink.CreateServiceFunction(L"sStayOnTopItem",ServiceStayOnTopItem);
  //Wklejanie tekstu ze schowka jako cytat
  PluginLink.CreateServiceFunction(L"sQuickQuoteItem",ServiceQuickQuoteItem);
  //Przypinanie zakladek
  PluginLink.CreateServiceFunction(L"sClipTabItem",ServiceClipTabItem);
  //Hook na zamkniecie okna rozmowy lub zakladki wraz z wiadomoscia
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_CLOSETABMESSAGE,OnCloseTabMessage);
  //Hook na zamkniecie okna rozmowy lub zakladki
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_CLOSETAB,OnCloseTab);
  //Hook na aktwyna zakladke lub okno rozmowy
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_ACTIVETAB,OnActiveTab);
  //Hook na zamkniecie okiena rozmowy
  PluginLink.HookEvent(AQQ_SYSTEM_WINDOWEVENT,OnWindowEvent);
  //Hook na odbieranie nowej wiadomosci
  PluginLink.HookEvent(AQQ_CONTACTS_RECVMSG,OnRecvMsg);
  //Hook na wysylanie nowej wiadomosci
  PluginLink.HookEvent(AQQ_CONTACTS_PRESENDMSG,OnPreSendMsg);
  //Hook na wpisywany tekst w oknie rozmowy
  PluginLink.HookEvent(AQQ_SYSTEM_MSGCOMPOSING,OnMsgComposing);
  //Hook na zmiane tekstu na zakladce
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
  //Hook na zmiane ikonki na zakladce
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABIMAGE,OnTabImage);
  //Hook dla zmiany stanu
  PluginLink.HookEvent(AQQ_SYSTEM_STATECHANGE,OnStateChange);
  //Hook na polaczenie sieci przy starcie AQQ
  PluginLink.HookEvent(AQQ_SYSTEM_SETLASTSTATE,OnSetLastState);
  //Hook na zmianê stanu kontaktu
  PluginLink.HookEvent(AQQ_CONTACTS_UPDATE,OnContactsUpdate);
  //Hook na otwarcie menu kontekstowego pola wiadomosci
  PluginLink.HookEvent(AQQ_SYSTEM_MSGCONTEXT_POPUP,OnMsgContextPopup);
  //Hook na zamkniecie menu kontekstowego pola wiadomosci
  PluginLink.HookEvent(AQQ_SYSTEM_MSGCONTEXT_CLOSE,OnMsgContextClose);
  //Hook na odbieranie pakietow XML zawierajace ID
  PluginLink.HookEvent(AQQ_SYSTEM_XMLIDDEBUG,OnXMLIDDebug);
  //Hook na pokazywanie popumenu
  PluginLink.HookEvent(AQQ_SYSTEM_POPUP,OnSystemPopUp);
  //Hook na zmianê kompozycji
  PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED, OnThemeChanged);
  //Hook na pokazywane wiadomosci
  PluginLink.HookEvent(AQQ_CONTACTS_ADDLINE,OnAddLine);
  //Hook na restart AQQ poprzez wtyczkê AQQRestarter
  PluginLink.HookEvent(AQQRESTARTER_SYSTEM_RESTARTING,OnRestartingAQQ);
  //Wylaczanie powiadomienia o pisaniu na pasku przy zminimalizowanym oknie rozmowy
  PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_TASKBARPEN,0);
  //Hook na klawiature
  hKeyboard = SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardProc,HInstance,GetCurrentThreadId());
  //Wczytanie ustawien
  LoadSettings();
  //Wylaczenie/wlaczanie AntiSpim
  CheckAntiSpim();
  //Ladowanie przypietych zakladek
  LoadClipTabs();
  //Sprawdzanie aktywnych zakladek i niewyslanych wiadomosci
  if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0))
  {
	//Pobieranie ostatnio zamknietych zakladek
	 GetClosedTabs();
	//Hook na pobieranie aktywnych zakladek
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs);
	PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_PRIMARYTAB,OnPrimaryTab);
	PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
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
	//Ustawianie tekstu glownego okna AQQ
	EnumWindows((WNDENUMPROC)FindFrmMain,0);
	EnumWindows((WNDENUMPROC)FindFrmMainL,0);
	if(TweakFrmMainTitlebarChk)
	{
	  if((TweakFrmMainTitlebarMode==1)&&(!ActiveProfileName.IsEmpty()))
	  {
		SetWindowTextW(hFrmMain,("AQQ [" + ActiveProfileName + "]").w_str());
		SetWindowTextW(hFrmMainL,("AQQ [" + ActiveProfileName + "]").w_str());
	  }
	  else if(TweakFrmMainTitlebarMode==2)
	  {
		if(!TweakFrmMainTitlebarText.IsEmpty())
		{
		  SetWindowTextW(hFrmMain,("AQQ " + TweakFrmMainTitlebarText).w_str());
		  SetWindowTextW(hFrmMainL,("AQQ " + TweakFrmMainTitlebarText).w_str());
		}
		else
		{
		  SetWindowTextW(hFrmMain,L"AQQ");
		  SetWindowTextW(hFrmMainL,L"AQQ");
        }
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Wyladowanie wtyczki
extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Szybki dostep niewyslanych wiadomosci
  //Usuwanie interwejsu
  DestroyFrmUnsentMsg();
  //Usuwanie serwisow
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItem0);
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItem1);
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItem2);
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItem3);
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItem4);
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItemClear);
  //Usuwanie PopUpMenu
  FrmUnsentMsgPopUp.cbSize = sizeof(TPluginAction);
  FrmUnsentMsgPopUp.pszName = L"FrmUnsentMsgPopUp";
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU ,0,(LPARAM)(&FrmUnsentMsgPopUp));
  //Szybki dostep ostatnio zamknietych zakladek
  //Usuwanie interfejsu
  DestroyFrmClosedTabs();
  //Usuwanie serwisow
  PluginLink.DestroyServiceFunction(ServiceFrmClosedTabsItem0);
  PluginLink.DestroyServiceFunction(ServiceFrmClosedTabsItem1);
  PluginLink.DestroyServiceFunction(ServiceFrmClosedTabsItem2);
  PluginLink.DestroyServiceFunction(ServiceFrmClosedTabsItem3);
  PluginLink.DestroyServiceFunction(ServiceFrmClosedTabsItem4);
  PluginLink.DestroyServiceFunction(ServiceFrmClosedTabsItemClear);
  //Usuwanie PopUpMenu
  FrmClosedTabsPopUp.cbSize = sizeof(TPluginAction);
  FrmClosedTabsPopUp.pszName = L"FrmClosedTabsPopUp";
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU,0,(LPARAM)(&FrmClosedTabsPopUp));
  //Trzymanie okna rozmowy na wierzchu
  //Usuwanie imterfejsu
  DestroyStayOnTop();
  //Usuwanie serwisu
  PluginLink.DestroyServiceFunction(ServiceStayOnTopItem);
  //Przywracanie okna rozmowy do "normalnosci"
  if(hFrmSend) SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
  //Przywracanie paska narzedzi do "normalnosci"
  if((HideToolBarChk)&&(hToolBar))
  {
	//Pobieranie wysokosci paska
	GetWindowRect(hToolBar,&WindowRect);
	int ToolBarHeight = WindowRect.Height();
	//Pokazanie paska
	if(!ToolBarHeight)
	{
      //Pobieranie pozycji okna rozmowy
	  GetWindowRect(hFrmSend,&WindowRect);
	  //Pokazywanie paska
	  SetWindowPos(hToolBar,NULL,0,0,WindowRect.Width(),25,SWP_NOMOVE);
	}
	else
	 HideToolBarChk = false;
  }
  //Przywracanie paska informacyjnego do "normalnosci"
  if((HideStatusBarChk)&&(hStatusBarPro))
  {
	//Pobieranie pozycji okna rozmowy
	GetWindowRect(hFrmSend,&WindowRect);
	//Pokazywanie paska
	SetWindowPos(hStatusBarPro,NULL,0,0,WindowRect.Width(),18,SWP_NOMOVE);
	ShowWindow(hStatusBarPro,SW_SHOW);
  }
  //Odswiezenie okna rozmowy
  if(((HideStatusBarChk)&&(hStatusBarPro))||((HideToolBarChk)&&(hToolBar)))
  {
	GetWindowRect(hFrmSend,&WindowRect);
	SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width()+1,WindowRect.Height(),SWP_NOMOVE);
	SetWindowPos(hFrmSend,NULL,0,0,WindowRect.Width(),WindowRect.Height(),SWP_NOMOVE);
  }
  //Usuwanie serwisu wstawiania tekstu ze schowka jako cytat
  PluginLink.DestroyServiceFunction(ServiceQuickQuoteItem);
  //Przypinanie zakladek
  //Usuwanie imterfejsu
  DestroyClipTab();
  //Usuwanie serwisu
  PluginLink.DestroyServiceFunction(ServiceClipTabItem);
  //Wyladowanie wszystkich hookow
  PluginLink.UnhookEvent(OnCloseTabMessage);
  PluginLink.UnhookEvent(OnCloseTab);
  PluginLink.UnhookEvent(OnActiveTab);
  PluginLink.UnhookEvent(OnWindowEvent);
  PluginLink.UnhookEvent(OnRecvMsg);
  PluginLink.UnhookEvent(OnPreSendMsg);
  PluginLink.UnhookEvent(OnMsgComposing);
  PluginLink.UnhookEvent(OnTabCaption);
  PluginLink.UnhookEvent(OnTabImage);
  PluginLink.UnhookEvent(OnStateChange);
  PluginLink.UnhookEvent(OnSetLastState);
  PluginLink.UnhookEvent(OnContactsUpdate);
  PluginLink.UnhookEvent(OnMsgContextPopup);
  PluginLink.UnhookEvent(OnMsgContextClose);
  PluginLink.UnhookEvent(OnXMLIDDebug);
  PluginLink.UnhookEvent(OnSystemPopUp);
  PluginLink.UnhookEvent(OnThemeChanged);
  PluginLink.UnhookEvent(OnAddLine);
  PluginLink.UnhookEvent(OnRestartingAQQ);
  //Wyladowanie ikonek z intefejsu
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)UNSENTMSG);
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)CLOSEDTABS);
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)STAYONTOP_OFF);
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)STAYONTOP_ON);
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)COMPOSING);
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)PAUSE);
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
  //Wlaczanie powiadomienia o pisaniu na pasku przy zminimalizowanym oknie rozmowy
  PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_TASKBARPEN,1);
  //Wlaczenie AntiSpim
  PluginLink.CallService(AQQ_SYSTEM_FUNCTION_SETENABLED,SYS_FUNCTION_ANTISPIM_LEN,1);
  //Hook systemowy
  if(hKeyboard!=NULL)
   UnhookWindowsHookEx(hKeyboard);
  //Przypisanie starej procki do okna rozmowy
  if(OldFrmSendProc)
  {
	SetWindowLongPtrW(hFrmSend, GWL_WNDPROC,(LONG)OldFrmSendProc);
	OldFrmSendProc = NULL;
  }
  //Sprawdzanie czy wtyczka ma pamietac sesje
  if((RestoreTabsSessionChk)&&(!ManualRestoreTabsSessionChk))
  {
	TIniFile *Ini = new TIniFile(SessionFileDir);
	Ini->EraseSection("Session");
	delete Ini;
  }
  //Dodawanie aktywnych zakladek do ostatnio zamknietych
  if((TabsList->Count)&&(ClosedTabsChk))
  {
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
		//Dodawanie JID do listy ostatnio zamknietych zakladek
		SaveClosedTabs();
	  }
	  SkipClosedTabsChk:
	  //Skip
	}
  }
  //Przywracanie oryginalnego titlebar glownego okna
  SetWindowTextW(hFrmMain,OryginalTitlebar);
  SetWindowTextW(hFrmMainL,OryginalTitlebar);
  //Przywracanie oryginalnego titlebar okna rozmowy
  TweakFrmSendTitlebarChk = false;
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_PRIMARYTAB,OnPrimaryTab);
  PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
  PluginLink.UnhookEvent(OnPrimaryTab);
  //Usuwanie przypietych zakladek
  if((TabsList->Count>1)&&(ClipTabsList->Count))
  {
	//Odswiezanie zakladek
	UnicodeString TmpActiveJID = ActiveTabJID;
	for(int Count=0;Count<TabsList->Count;Count++)
	{
	  UnicodeString JID = TabsList->Strings[Count];
	  //Otwieranie zakladki z kontektem
	  if(!JID.IsEmpty())
	  {
		if(!JID.Pos("ischat_"))
		 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
		else
		{
		  JID = JID.Delete(1,7);
		  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)JID.w_str());
		}
	  }
	}
	//Przelaczenie na aktywna zakladke
	if(!TmpActiveJID.Pos("ischat_"))
	 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)TmpActiveJID.w_str());
	else
	{
	  TmpActiveJID = TmpActiveJID.Delete(1,7);
	  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,2,(LPARAM)TmpActiveJID.w_str());
	}
  }
  else if((TabsList->Count==1)&&(ClipTabsList->Count))
  {
	if(!ActiveTabJID.Pos("ischat_"))
	{
	  PluginContact.cbSize = sizeof(TPluginContact);
	  PluginContact.JID = ActiveTabJID.w_str();
	  //Zmiana caption zakladki
	  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)GetContactNick(ActiveTabJID).w_str(),(LPARAM)&PluginContact);
	  //Zmiana ikonki na zakladce
	  PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABIMAGE,(WPARAM)GetContactState(ActiveTabJID),(LPARAM)&PluginContact);
	}
  }
  //Zmienne
  delete MsgList;
  delete PreMsgList;
  delete PreMsgStateList;
  delete TabsList;
  delete ContactsStateList;
  delete ClosedTabsList;
  delete AcceptClosedTabsList;
  delete ClosedTabsTimeList;
  delete InactiveTabsNewMsgCount;
  delete ClipTabsList;
  delete ClipTabsIconList;
  delete ChatSessionList;

  return 0;
}
//---------------------------------------------------------------------------

//Ustawienia wtyczki
extern "C" int __declspec(dllexport)__stdcall Settings()
{
  if(!hSettingsForm)
  {
	Application->Handle = (HWND)SettingsForm;
	hSettingsForm = new TSettingsForm(Application);
  }
  hSettingsForm->Show();
  hSettingsForm->TweakFrmMainTitlebarMode1RadioButton->Caption = "AQQ [" + ActiveProfileName + "]";
  if(!EmuTabsWSupport) hSettingsForm->EmuTabsWCheckBox->Enabled = false;

  return 0;
}
//---------------------------------------------------------------------------

//Informacja o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  //Sprawdzanie wersji AQQ
  if (PLUGIN_COMPARE_VERSION(AQQVersion,PLUGIN_MAKE_VERSION(2,2,5,30))<0)
  {
	MessageBox(Application->Handle,
	  "Wymagana wesja AQQ przez wtyczkê to minimum 2.2.5.30!\n"
	  "Wtyczka TabKit nie bêdzie dzia³aæ poprawnie!",
	  "Nieprawid³owa wersja AQQ",
	  MB_OK | MB_ICONEXCLAMATION);
  }
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"TabKit";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,1,0,8);
  PluginInfo.Description = L"Ulepszenie obs³ugi zak³adek";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl/";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

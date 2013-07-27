//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "SettingsFrm.h"
#include "Aqq.h"
#include <inifiles.hpp>
#include <process.h>
#include <Clipbrd.hpp>
#define AQQRESTARTER_SYSTEM_RESTARTING L"AQQRestarter/System/Restarting"
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
//UnsentMsg
TPluginAction FrmUnsentMsgPopUp;
TPluginAction FrmMainUnsentMsgButton, FrmSendUnsentMsgButton;
TPluginAction BuildFrmUnsentMsgItem[7];
TPluginAction DestroyFrmUnsentMsgItem;
//ClosedTabs
TPluginAction FrmClosedTabsPopUp;
TPluginAction FrmMainClosedTabsButton, FrmSendClosedTabsButton;
TPluginAction BuildFrmClosedTabsItem[12];
TPluginAction DestroyFrmClosedTabsItem;
//StayOnTop
TPluginAction BuildStayOnTopItem;
//InactiveNotifer
TPluginStateChange PluginStateChange;
PPluginStateChange StateChange;
//QuickQuote
TPluginAction BuildQuickQuoteItem;
PPluginTriple PluginTriple;
TPluginItemDescriber PluginItemDescriber;
PPluginAction Action;
//Ikonki interfesju
int UNSENTMSG;
int CLOSEDTABS;
int STAYONTOP_OFF;
int STAYONTOP_ON;

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
UnicodeString JustUnClosedJID;
//Stany kontaktow
TCustomIniFile* ContactsStateList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
//SessionRemember
bool RestoreTabsSessionChk;
bool ManualRestoreTabsSessionChk;
bool RestoreMsgSessionChk;
bool RestoringSession = false;
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
int InactiveFrmNewMsgCount = 0;
TCustomIniFile* InactiveTabsNewMsgCount = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
TStringList *InactiveTabsNewMsgList = new TStringList;
bool ClosingTab = false;
bool BlockInactiveNotiferNewMsg = false;
bool Connecting = false;
//Other
bool StayOnTopChk;
bool EmuTabsWChk;
bool SetStayOnTop;
bool EmuTabsWSupport;
bool QuickQuoteChk = true;

//Aktywna otwarta zakladka
UnicodeString ActiveJID;
//Sciezka do pliku sesji
UnicodeString SessionFileDir;
//Nazwa aktywnego profilu
UnicodeString ActiveProfileName;
//Uchwyt do okna rozmowy
HWND hFrmSend;
HWND hActiveFrm;
//Uchwyt do pola RichEdit
HWND hRichEdit;
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
WNDPROC g_OldWndProc;
//Zla wersja AQQ
bool AQQVersionChk = true;

//Lista JID z nowymi wiadomosciami
TStringList *MsgList = new TStringList;
//Lista JID otwartych zakladek
TStringList *TabsList = new TStringList;
//Lista JID zamknietych zakladek
TStringList *ClosedTabsList = new TStringList;
TStringList *ClosedTabsTimeList = new TStringList;

//Szukanie uchwytu do kontrolki TRichEdit
bool CALLBACK FindRichEdit(HWND hWnd, LPARAM lParam)
{
  GetClassNameW(hWnd, WClassName, sizeof(WClassName));

  if((UnicodeString)WClassName=="TRichEdit")
  {
	hRichEdit=hWnd;
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

//Pobieranie Nick kontaktu podajac jego JID
UnicodeString GetContactNick(UnicodeString JID)
{
  TPluginContactSimpleInfo PluginContactSimpleInfo;
  PluginContactSimpleInfo.cbSize = sizeof(TPluginContactSimpleInfo);
  PluginContactSimpleInfo.JID = JID.w_str();
  PluginLink.CallService(AQQ_CONTACTS_FILLSIMPLEINFO,0,(LPARAM)(&PluginContactSimpleInfo));
  UnicodeString Nick = (wchar_t*)PluginContactSimpleInfo.Nick;
  Nick = Nick.Trim();
  UnicodeString NickW = Nick;
  NickW.t_str();
  if((NickW.IsEmpty())||(NickW=="?"))
  {
	if(AnsiPos("@plugin.gg.",JID))
	 JID.Delete(AnsiPos("@plugin.gg.",JID),JID.Length());
	return JID;
  }
  else
   return Nick;
}
//---------------------------------------------------------------------------

//Pobieranie statusu kontaktu podajac jego JID
int GetContactState(UnicodeString JID)
{
  int State = ContactsStateList->ReadInteger("State",JID,0);
  //Jezeli stan nie jest zapisany
  if(State==0)
  {
	//Przypisanie domyslnego
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
    //Usuwanie buttona na FrmMain
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmMainUnsentMsgButton));
	//Usuwanie buttona na FrmSend
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmSendUnsentMsgButton));

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
	  if((hFrmSend!=NULL)&&(FrmSendUnsentMsgChk))
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
  //Otwieranie zakladki z kontektem z
  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
	  if((hFrmSend!=NULL)&&(FrmSendClosedTabsChk))
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
  //Otwieranie zakladki z kontektem z
  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
		PluginShowInfo.TimeOut = 1000 * 6;
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
			UnicodeString tempJID = GetContactNick(JID);
			if(!tempJID.IsEmpty())
			 PluginShowInfo.Text = (tempJID + "\r\n" + Body).w_str();
			else
			 PluginShowInfo.Text = (JID + "\r\n" + Body).w_str();
			PluginShowInfo.ImagePath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0));
			PluginShowInfo.TimeOut = 1000 * 6;
			PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
		  }
		}
	  }
	  //Informacja w trayu
	  if(TrayUnsentMsgChk)
	  {
		if(hSettingsForm==NULL)
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
	  //Otwieranie zakladki z kontektem
	  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
	}
  }
  delete Messages;
  delete Ini;
}
//---------------------------------------------------------------------------

LRESULT CALLBACK FrmSendProc(HWND hwnd, UINT mesg, WPARAM wParam, LPARAM lParam)
{
  //Zmiana tekstu na belce okna
  if(InactiveFrmNewMsgChk)
  {
	if(mesg==WM_ACTIVATE)
	{
	  //Okno aktywne
	  if((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE))
	  {
		//Przywracanie poprzedniego stanu titlebara
		if(!TempTitlebar.IsEmpty())
		{
		  SetWindowTextW(hwnd,TempTitlebarW);
		  TempTitlebar = "";
		}
		//Kasowanie licznika nowych wiadomosci
		InactiveFrmNewMsgCount = 0;
	  }
	}
  }

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
		return CallWindowProc(g_OldWndProc, hwnd, mesg, wParam, lParam);
	  }
	  else
	   return 1;
	}
  }

  return CallWindowProc(g_OldWndProc, hwnd, mesg, wParam, lParam);
}
//---------------------------------------------------------------------------

int __stdcall OnContactsUpdate (WPARAM wParam, LPARAM lParam)
{
  Contact = (PPluginContact)wParam;
  if(!Contact->IsChat)
  {
	UnicodeString JID = (wchar_t*)Contact->JID;
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

//Pobieranie listy wszystkich otartych zakladek/okien
int __stdcall OnFetchAllTabs (WPARAM wParam, LPARAM lParam)
{
  if((wParam!=0)&&(lParam!=0))
  {
	Contact = (PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);
	//Dodawanie info na poczatku JID jezeli kontakt jest czatem
	bool IsChat = (bool)(Contact->IsChat);
	if(IsChat)
	 JID = "TabKit_IsChat_" + JID;

    //Dodawanie JID do listy otwartych zakladek
	if(TabsList->IndexOf(JID)==-1)
	 TabsList->Add(JID);

	//Usuwanie JID z listy ostatnio zamknietych zakladek
	if(ClosedTabsChk)
	{
	  if((ClosedTabsList->IndexOf(JID)!=-1)&&(!IsChat))
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

	if(hFrmSend==NULL)
	{
	  //Przypisanie uchwytu okna rozmowy
	  hFrmSend = (HWND)wParam;
	  //Szukanie pola wiadomosci
	  EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	  //Przypisanie nowej procki dla okna rozmowy
	  if(g_OldWndProc==NULL)
	   g_OldWndProc = (WNDPROC)SetWindowLongW(hFrmSend, GWL_WNDPROC,(LONG)FrmSendProc);
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
  //Dodawanie info na poczatku JID jezeli kontakt jest czatem
  bool IsChat = (bool)(Contact->IsChat);
  if(IsChat)
   JID = "TabKit_IsChat_" + JID;
  ActiveJID = JID;

  if(!IsChat)
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
	if((TweakFrmMainTitlebarMode==1)&&(ActiveProfileName!=""))
	{
	  SetWindowTextW(hFrmMain,("AQQ [" + ActiveProfileName + "]").w_str());
	  SetWindowTextW(hFrmMainL,("AQQ [" + ActiveProfileName + "]").w_str());
	}
	else if((TweakFrmMainTitlebarMode==2)&&(!TweakFrmMainTitlebarText.IsEmpty()))
	{
	  SetWindowTextW(hFrmMain,("AQQ " + TweakFrmMainTitlebarText).w_str());
	  SetWindowTextW(hFrmMainL,("AQQ " + TweakFrmMainTitlebarText).w_str());
	}
  }
  else
  {
	SetWindowTextW(hFrmMain,OryginalTitlebar);
	SetWindowTextW(hFrmMainL,OryginalTitlebar);
  }
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

	  break;
	}
  }
}
//---------------------------------------------------------------------------

int __stdcall ServiceStayOnTopItem (WPARAM wParam, LPARAM lParam)
{
  if(!SetStayOnTop)//Set TOPMOST
  {
	//Aktualizacja przycisku
	BuildStayOnTopItem.IconIndex = STAYONTOP_ON;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&BuildStayOnTopItem));

	//Okno rozmowy na wierzchu
	SetWindowPos(hFrmSend,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);

	SetStayOnTop=true;
  }
  else//Set NOTOPMOST
  {
	//Aktualizacja przycisku
	BuildStayOnTopItem.IconIndex = STAYONTOP_OFF;
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_UPDATEBUTTON,0,(LPARAM)(&BuildStayOnTopItem));

	//Przywrocenie "normalnosci" okna
	SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);

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
	  //Szukanie RichEdit
	  //EnumChildWindows(hActiveFrm,(WNDENUMPROC)FindRichEdit,0);
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
		   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
	//Sprawdzanie niewyslanych wiadomosci
	GetUnsentMsg();
	//Szybki dostep niewyslanych wiadomosci
	BuildFrmUnsentMsg();

	//Ustawianie tekstu glownego okna AQQ
	EnumWindows((WNDENUMPROC)FindFrmMain,0);
	EnumWindows((WNDENUMPROC)FindFrmMainL,0);
	if(TweakFrmMainTitlebarChk)
	{
	  if((TweakFrmMainTitlebarMode==1)&&(ActiveProfileName!=""))
	  {
		SetWindowTextW(hFrmMain,("AQQ [" + ActiveProfileName + "]").w_str());
		SetWindowTextW(hFrmMainL,("AQQ [" + ActiveProfileName + "]").w_str());
	  }
	  else if((TweakFrmMainTitlebarMode==2)&&(!TweakFrmMainTitlebarText.IsEmpty()))
	  {
		SetWindowTextW(hFrmMain,("AQQ " + TweakFrmMainTitlebarText).w_str());
		SetWindowTextW(hFrmMainL,("AQQ " + TweakFrmMainTitlebarText).w_str());
	  }
	}
  }

  //Otwarcie okna rozmowy
  if((EventType=="TfrmSend")&&(Event==1))
  {
	if(hFrmSend==NULL)
	{
	  //Przypisanie uchwytu do okna rozmowy
	  hFrmSend = (HWND)WindowEvent->Handle;
	  //Szukanie pola wiadomosci
	  EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
	  //Tworzenie interfejsu tworzenia okna rozmowy na wierzchu
	  BuildStayOnTop();
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
	  g_OldWndProc = (WNDPROC)SetWindowLongW(hFrmSend, GWL_WNDPROC,(LONG)FrmSendProc);
	}
  }
  //Zamkniecie okna rozmowy
  if((EventType=="TfrmSend")&&(Event==2))
  {
    //Przypisanie starej procki dla okna rozmowy
	SetWindowLongW(hFrmSend, GWL_WNDPROC,(LONG)g_OldWndProc);
	g_OldWndProc = NULL;
	//Resetowanie zmiennej aktwnie otwartej zakladki
	ActiveJID = "";
	//Resetowanie uchwytu do okna rozmowy
	hFrmSend = NULL;
	//Szybki dostep niewyslanych wiadomosci
	DestroyFrmUnsentMsg();
	BuildFrmUnsentMsg();
	//Szybki dostep do ostatnio zamknietych zakladek
	DestroyFrmClosedTabs();
	BuildFrmClosedTabs();
	//Usuwanie interfejsu tworzenia okna rozmowy na wierzchu
	DestroyStayOnTop();
  }

  if((StayOnTopChk)&&(SetStayOnTop))
  {
	//Otworzenie okna emotek
	if((EventType=="TfrmGraphic")&&(Event==1))
	{
	  //hWindowHandle = (HWND)WindowEvent->Handle;
	  SetWindowPos((HWND)WindowEvent->Handle,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	}
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

int __stdcall OnTabCaption (WPARAM wParam, LPARAM lParam)
{
  if((InactiveTabsNewMsgChk)&&(!ClosingTab))
  {
	Contact = (PPluginContact)lParam;
	UnicodeString JID = (wchar_t*)(Contact->JID);

	if(InactiveTabsNewMsgList->IndexOf(JID)!=-1)
	{
	  UnicodeString TabCaption = (wchar_t*)wParam;
	  int Count = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID,0);
	  if(Count)
	  {
		if(AnsiPos("[" + IntToStr(Count) + "] ",TabCaption)!=1)
		 TabCaption = "[" + IntToStr(Count) + "] " + TabCaption;
		return (WPARAM)TabCaption.w_str();
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na aktwyna zakladke lub okno rozmowy
int __stdcall OnActiveTab (WPARAM wParam, LPARAM lParam)
{
  //Blok na zmiane tekstu na zakladce
  ClosingTab = false;

  Contact = (PPluginContact)lParam;
  UnicodeString JID = (wchar_t*)(Contact->JID);
  //Dodawanie info na poczatku JID jezeli kontakt jest czatem
  bool IsChat = (bool)(Contact->IsChat);
  if(IsChat)
   JID = "TabKit_IsChat_" + JID;
  ActiveJID = JID;

  //Usuwanie JID z kolejki
  if((SwitchToNewMsgChk)&&(!IsChat))
  {
	if(MsgList->IndexOf(JID)!=-1)
	 MsgList->Delete(MsgList->IndexOf(JID));
  }

  //Jezeli JID nie ma w tablicy zakladek
  if(TabsList->IndexOf(JID)==-1)
  {
	//Dodawanie JID do tablicy zakladek
	TabsList->Add(JID);

	//Zapisywanie sesji
	if(RestoreTabsSessionChk)
	{
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  Ini->EraseSection("Session");
	  for(int Count=0;Count<TabsList->Count;Count++)
	  {
		if(AnsiPos("TabKit_IsChat_",TabsList->Strings[Count])==0)
		 Ini->WriteString("Session","Tab"+IntToStr(Count+1),TabsList->Strings[Count]);
	  }
      //Odczytywanie sesji wiadomosci
	  if((RestoreMsgSessionChk)&&(RestoringSession))
	  {
		UnicodeString Body = UTF8ToUnicodeString(IniStrToStr(Ini->ReadString("SessionMsg", JID, "")).t_str());
		//Wczytanie tresci wiadomosci do pola RichEdit
		if(!Body.IsEmpty())
		{
		  int UserIdx = Contact->UserIdx;
		  //Pobieranie ostatniej wiadomoœci
		  PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)UserIdx);
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

	//Niewyslane wiadomosci
	if((UnsentMsgChk)&&(!IsChat)&&(!RestoringSession))
	{
	  //Odczyt pliku sesji
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  UnicodeString Body = UTF8ToUnicodeString(IniStrToStr(Ini->ReadString("Messages", JID, "")).t_str());

	  //Wczytanie tresci wiadomosci do pola RichEdit
	  if(!Body.IsEmpty())
	  {
		int UserIdx = Contact->UserIdx;
		//Pobieranie ostatniej wiadomoœci
		PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)UserIdx);
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
	if((ClosedTabsChk)&&(!IsChat)&&(!RestoringSession))
	{
	  //Usuwanie JID z listy ostatnio zamknietych zakladek
	  if((ClosedTabsList->IndexOf(JID)!=-1)&&(!IsChat))
	  {
		//Odczytywanie JID ostatnio przywrcanej zakladki
		if((RestoreLastMsgChk)&&(JustUnClosedJID==JID))
		{
		  int UserIdx = Contact->UserIdx;
		  //Pobieranie ostatniej wiadomoœci
		  PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)UserIdx);
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

  if(TweakFrmSendTitlebarChk)
  {
	if(!IsChat)
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

  if((InactiveTabsNewMsgChk)&&(!IsChat))
  {
	//Zabezpieczenie zmiany tekstu na zakladce
	if(InactiveTabsNewMsgList->IndexOf(JID)!=-1)
	 InactiveTabsNewMsgList->Delete(InactiveTabsNewMsgList->IndexOf(JID));
	//Przywracanie domyslnego tekstu na zakladce
	PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)Contact->Nick,(LPARAM)Contact);
	//Resetowanie stanu nowych wiadomosci
	InactiveTabsNewMsgCount->DeleteKey("TabsMsg",JID);
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie okna rozmowy lub zakladki
int __stdcall OnCloseTab(WPARAM wParam, LPARAM lParam)
{
  //Blok na zmiane tekstu na zakladce
  ClosingTab = false;

  Contact = (PPluginContact)lParam;
  UnicodeString JID = (wchar_t*)(Contact->JID);
  //Dodawanie info na poczatku JID jezeli kontakt jest czatem
  bool IsChat = (bool)(Contact->IsChat);
  if(IsChat)
   JID = "TabKit_IsChat_" + JID;

  //Zapisywanie stanu kontaktu
  if(!IsChat)
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
	  for(int Count=0;Count<TabsList->Count;Count++)
	  {
		if(AnsiPos("TabKit_IsChat_",TabsList->Strings[Count])==0)
		 Ini->WriteString("Session","Tab"+IntToStr(Count+1),TabsList->Strings[Count]);
	  }
	}
  }

  //Usuwanie JID z kolejki
  if((SwitchToNewMsgChk)&&(!IsChat))
  {
	if(MsgList->IndexOf(JID)!=-1)
	 MsgList->Delete(MsgList->IndexOf(JID));
  }

  //Dodawanie JID do listy ostatnio zamknietych zakladek
  if(ClosedTabsChk)
  {
	if((ClosedTabsList->IndexOf(JID)==-1)&&(!IsChat))
	{
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
	//Jezeli kontakt nie jest czatem
	if(!Contact->IsChat)
	{
	  UnicodeString JID = (wchar_t*)(Contact->JID);
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
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na odbieranie wiadomosci
int __stdcall OnRecvMsg(WPARAM wParam, LPARAM lParam)
{
  if(SwitchToNewMsgChk)
  {
	Contact = (PPluginContact)wParam;
	//Jezeli kontakt nie jest czatem
	if(!Contact->IsChat)
	{
	  UnicodeString JID = (wchar_t*)(Contact->JID);
	  //Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
	  if((JID!=ActiveJID)&&(TabsList->IndexOf(JID)!=-1))
	  {
		Message = (PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if(Message->Kind==MSGKIND_CHAT)
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
  }

  if(InactiveFrmNewMsgChk)
  {
	Contact = (PPluginContact)wParam;

	UnicodeString JID = (wchar_t*)(Contact->JID);

	if(Contact->IsChat)
	 JID = "TabKit_IsChat_" + JID;

	//Jezeli zakladka jest otwarta
	if((TabsList->IndexOf(JID)!=-1))
	{
	  //Jezeli okno rozmowy jest nieaktywne
	  if(GetForegroundWindow()!=hFrmSend)
	  {
		Message = (PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if((Message->Kind==MSGKIND_CHAT)||(Message->Kind==MSGKIND_GROUPCHAT))
		{
		  UnicodeString Body = (wchar_t*)(Message->Body);
		  if(!Body.IsEmpty())
		  {
			//Dodanie 1 do licznika nieprzeczytachy wiadomosci
			InactiveFrmNewMsgCount++;
			//Pobranie oryginalnego titlebar'a
			if(TempTitlebar.IsEmpty())
			{
			  GetWindowTextW(hFrmSend,TempTitlebarW,sizeof(TempTitlebarW));
			  TempTitlebar = (wchar_t*)TempTitlebarW;
			}
			//Pobranie aktualnego titlebar'a
			wchar_t WindowTitlebarW[128];
			GetWindowTextW(hFrmSend,WindowTitlebarW,sizeof(WindowTitlebarW));
			UnicodeString WindowTitlebar = (wchar_t*)WindowTitlebarW;
			if(AnsiPos("["+IntToStr(InactiveFrmNewMsgCount-1)+"] ",WindowTitlebar)==1)
			 WindowTitlebar = StringReplace(WindowTitlebar, "["+IntToStr(InactiveFrmNewMsgCount-1)+"]", "["+IntToStr(InactiveFrmNewMsgCount)+"]", TReplaceFlags() << rfReplaceAll);
			else
			 WindowTitlebar = "["+IntToStr(InactiveFrmNewMsgCount)+"] "+WindowTitlebar;
			//Ustawianie nowego titlebar'a
			SetWindowTextW(hFrmSend,WindowTitlebar.w_str());
		  }
		}
	  }
	}
  }

  if(InactiveTabsNewMsgChk)
  {
	Contact = (PPluginContact)wParam;

	if(!Contact->IsChat)
	{
	  UnicodeString JID = (wchar_t*)(Contact->JID);
	  //Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
	  if((JID!=ActiveJID)&&(TabsList->IndexOf(JID)!=-1))
	  {
		Message = (PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if(Message->Kind==MSGKIND_CHAT)
		{
		  UnicodeString Body = (wchar_t*)(Message->Body);
		  if(!Body.IsEmpty())
		  {
			//Pobieranie i ustawianie stanu nowych wiadomosci
			int InactiveTabsCount = InactiveTabsNewMsgCount->ReadInteger("TabsMsg",JID,0);
			InactiveTabsCount++;
			InactiveTabsNewMsgCount->WriteInteger("TabsMsg",JID,InactiveTabsCount);
			//Tworzenie nowego tytulu zakladki
			UnicodeString Nick = (wchar_t*)(Contact->Nick);
			Nick = "[" + IntToStr(InactiveTabsCount) + "] " + Nick;
			//Zabezpieczenie zmiany tekstu na zakladce
			if(InactiveTabsNewMsgList->IndexOf(JID)!=-1)
			 InactiveTabsNewMsgList->Delete(InactiveTabsNewMsgList->IndexOf(JID));
			//Ustawianie nowego tytulu zakladki
			PluginLink.CallService(AQQ_CONTACTS_BUDDY_TABCAPTION,(WPARAM)Nick.w_str(),(LPARAM)Contact);
			//Zabezpieczenie zmiany tekstu na zakladce
			InactiveTabsNewMsgList->Add(JID);
		  }
		}
	  }
	}
  }

  if((InactiveNotiferNewMsgChk)&&(!BlockInactiveNotiferNewMsg))
  {
	Contact = (PPluginContact)wParam;

	UnicodeString JID = (wchar_t*)(Contact->JID);
	if(Contact->IsChat)
	 JID = "TabKit_IsChat_" + JID;

	//Jezeli zakladka jest otwarta
	if(TabsList->IndexOf(JID)!=-1)
	{
      //JID wiadomosci jest inny niz JID aktwnej zakladki
	  if(JID!=ActiveJID)
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
			PluginShowInfo.Event = tmeMsg;
			PluginShowInfo.Text = Nick.w_str();
			PluginShowInfo.ImagePath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0));
			PluginShowInfo.TimeOut = 1000 * 6;
			PluginShowInfo.Tick = TickID;
			PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
			//Body
			PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			PluginShowInfo.Event = tmeMsgCap;
			PluginShowInfo.Text = Body.w_str();
			PluginShowInfo.ImagePath = L"";
			PluginShowInfo.TimeOut = 1000 * 6;
			PluginShowInfo.Tick = TickID;
			PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
		  }
		}
	  }
	  else if(GetForegroundWindow()!=hFrmSend)
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
			PluginShowInfo.Event = tmeMsg;
			PluginShowInfo.Text = Nick.w_str();
			PluginShowInfo.ImagePath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPNG_FILEPATH,8,0));
			PluginShowInfo.TimeOut = 1000 * 6;
			PluginShowInfo.Tick = TickID;
			PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
			//Body
			PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			PluginShowInfo.Event = tmeMsgCap;
			PluginShowInfo.Text = Body.w_str();
			PluginShowInfo.ImagePath = L"";
			PluginShowInfo.TimeOut = 1000 * 6;
			PluginShowInfo.Tick = TickID;
			PluginShowInfo.ActionID = ("EXEC_MSG:" + IntToStr(UserIdx) + ";" + JID + "/" + Res).w_str();
			PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));
		  }
		}
	  }
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
  if(InactiveNotiferNewMsgChk)
  {
	PluginLink.CallService(AQQ_FUNCTION_GETNETWORKSTATE,(WPARAM)(&PluginStateChange),0);

	int NewState = (int)PluginStateChange.NewState;

	//OnLine - Connected
	if(NewState!=0)
	{
	  //Jezeli uchwyt do glownego okna nie zostal jeszcze zdefiniowany
	  if(!hFrmMain)
	   EnumWindows((WNDENUMPROC)FindFrmMain,0);
	  if(hFrmMain)
	  {
		//Blokowanie notyfikatora nowych wiadomosci
		BlockInactiveNotiferNewMsg = true;
		//Tworzenie timera
		SetTimer(hFrmMain,200,20000,(TIMERPROC)Timer);
	  }
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Notyfikacja zmiany stanu
int __stdcall OnStateChange(WPARAM wParam, LPARAM lParam)
{
  if(InactiveNotiferNewMsgChk)
  {
	StateChange = (PPluginStateChange)lParam;
	int NewState = (int)StateChange->NewState;
	int OldState = (int)StateChange->OldState;
	bool Authorized = (bool)StateChange->Authorized;

	//OnLine - Connecting
	if((OldState==0)&&(NewState!=0)&&(Authorized==0))
	{
	  Connecting = true;
	}
	//OnLine - Connected
	if((Connecting==true)&&(Authorized==1)&&(NewState==OldState))
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
	else if((Connecting==true)&&(Authorized==1)&&(NewState!=OldState))
	 Connecting = false;
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
		//Przypisanie uchwytu do pola wiadomosci
		hRichEdit = (HWND)PluginTriple->Handle2;
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
			  //Przelaczanie na zakladki z kontaktem z kolejki
			  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
				if((!JID.IsEmpty())&&(AnsiPos("TabKit_IsChat_",JID)==0))
				{
				  //Przelaczanie na zakladke z danym kontektem
				  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
			  if((!JID.IsEmpty())&&(AnsiPos("TabKit_IsChat_",JID)==0))
			  {
				//Przelaczanie na zakladke z danym kontektem
				PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
		   //Jezeli aktywne okno to okno rozmowy AQQ
		   if(((UnicodeString)WClassName=="TfrmSend")&&(PID==ProcessPID))
		   {
			 if(ClosedTabsList->Count>0)
			 {
			   UnicodeString JID = ClosedTabsList->Strings[0];
			   //Zapisawanie JID aktualnie przywracanej zakladki
			   JustUnClosedJID = JID;
			   //Przelaczanie na ostatnio zamknieta zakladke
			   PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
		//Jezeli aktywne okno to okno rozmowy AQQ
		if(((UnicodeString)WClassName=="TfrmSend")&&(PID==ProcessPID))
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
				 //Przelaczanie na ostatnio zamknieta zakladke
				 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
				 //Przelaczanie na ostatnio zamknieta zakladke
				 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
				 //Przelaczanie na ostatnio zamknieta zakladke
				 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
				 //Przelaczanie na ostatnio zamknieta zakladke
				 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
				 //Przelaczanie na ostatnio zamknieta zakladke
				 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
				 //Przelaczanie na ostatnio zamknieta zakladke
				 PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)JID.w_str());
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
			  //Szukanie RichEdit
			  EnumChildWindows(hActiveFrm,(WNDENUMPROC)FindRichEdit,0);
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

//Pobieranie sciezki katalogu prywatnego uzytkownika
UnicodeString GetPluginUserDir()
{
  UnicodeString Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------

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
  if(!AQQVersionChk) InactiveTabsNewMsgChk = false;
  InactiveNotiferNewMsgChk = Ini->ReadBool("NewMsg","InactiveNotifer",false);
  //Other
  StayOnTopChk = Ini->ReadBool("Other","StayOnTop",false);
  EmuTabsWChk = Ini->ReadBool("Other","EmuTabsW",false);
  if(!EmuTabsWSupport) EmuTabsWChk = false;
  QuickQuoteChk = Ini->ReadBool("Other","QuickQuote",false);
  delete Ini;
}
//---------------------------------------------------------------------------

//Zapisywanie zasobów
bool SaveResourceToFile(char *FileName, char *res)
{
  HRSRC hrsrc = FindResource(HInstance, res, RT_RCDATA);
  if(hrsrc == NULL) return false;
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
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
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
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
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
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
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
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
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
	UnicodeString PluginDirW = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
	//Wylaczenie wtyczki
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_ACTIVE,0,(LPARAM)(PluginDirW + "\\QuickQuote.dll").w_str());
	//Usuwanie wtyczki
	DeleteFile(SessionFileDir + "\\\\QuickQuote.dll");
	//Usuwanie wtyczki z wyjatkow
	PluginLink.CallService(AQQ_SYSTEM_PLUGIN_EXCLUDE,1,(LPARAM)(PluginDirW + "\\QuickQuote.dll").w_str());
  }
  //Folder z ustawieniami wtyczki etc
  if(!DirectoryExists(SessionFileDir + "\\\\TabKit"))
   CreateDir(SessionFileDir + "\\\\TabKit");
  //Wypakiwanie ikonki UnsentMsg.png
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png").t_str(),"UNSENTMSG");
  //Usuwanie starej ikonki ClosedTabs.png
  if(FileExists(SessionFileDir + "\\\\TabKit\\\\ClosedTabs.png"))
   DeleteFile(SessionFileDir + "\\\\TabKit\\\\ClosedTabs.png");
  //Wypakiwanie ikonki ClosedTabsButton.png
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\ClosedTabsButton.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\ClosedTabsButton.png").t_str(),"CLOSEDTABS");
  //Wypakowanie ikonki StayOnTopOff.png
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\StayOnTopOff.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\StayOnTopOff.png").t_str(),"STAYONTOP_OFF");
  //Wypakiwanie ikonki StayOnTopOn.png
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\StayOnTopOn.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\StayOnTopOn.png").t_str(),"STAYONTOP_ON");
  //Przypisanie ikonek do interfejsu AQQ
  UNSENTMSG = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png").w_str());
  CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\ClosedTabsButton.png").w_str());
  STAYONTOP_OFF = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\StayOnTopOff.png").w_str());
  STAYONTOP_ON = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\StayOnTopOn.png").w_str());
  //Dodanie sciezki do pliku sesji
  SessionFileDir = SessionFileDir + "\\\\TabKit\\\\Session.ini";
  //Definiowanie nazwy uzywanego profilu
  ActiveProfileName = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,(WPARAM)HInstance,0);
  if(AnsiPos("\\",ActiveProfileName)>0)
   while(AnsiPos("\\",ActiveProfileName)>0)
	ActiveProfileName.Delete(1,AnsiPos("\\",ActiveProfileName));
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
  //Hook na wpisywany tekst w oknie rozmowy
  PluginLink.HookEvent(AQQ_SYSTEM_MSGCOMPOSING,OnMsgComposing);
  //Hook na zmiane tekstu na zakladce
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_TABCAPTION,OnTabCaption);
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
  //Hook na restart AQQ poprzez wtyczkê AQQRestarter
  PluginLink.HookEvent(AQQRESTARTER_SYSTEM_RESTARTING,OnRestartingAQQ);
  //Hook na klawiature
  hKeyboard = SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardProc,HInstance,GetCurrentThreadId());
  //Wczytanie ustawien
  LoadSettings();
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
	//Sprawdzanie niewyslanych wiadomosci
	GetUnsentMsg();
	//Szybki dostep niewyslanych wiadomosci
	BuildFrmUnsentMsg();
	//Ustawianie tekstu glownego okna AQQ
	EnumWindows((WNDENUMPROC)FindFrmMain,0);
	EnumWindows((WNDENUMPROC)FindFrmMainL,0);
	if(TweakFrmMainTitlebarChk)
	{
	  if((TweakFrmMainTitlebarMode==1)&&(ActiveProfileName!=""))
	  {
		SetWindowTextW(hFrmMain,("AQQ [" + ActiveProfileName + "]").w_str());
		SetWindowTextW(hFrmMainL,("AQQ [" + ActiveProfileName + "]").w_str());
	  }
	  else if((TweakFrmMainTitlebarMode==2)&&(!TweakFrmMainTitlebarText.IsEmpty()))
	  {
		SetWindowTextW(hFrmMain,("AQQ " + TweakFrmMainTitlebarText).w_str());
		SetWindowTextW(hFrmMainL,("AQQ " + TweakFrmMainTitlebarText).w_str());
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
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU,0,(LPARAM)(&FrmClosedTabsPopUp));
  //Trzymanie okna rozmowy na wierzchu
  //Usuwanie imterfejsu
  DestroyStayOnTop();
  //Usuwanie serwisu
  PluginLink.DestroyServiceFunction(ServiceStayOnTopItem);
  //Przywracanie okna rozmowy do "normalnosci"
  if(hFrmSend) SetWindowPos(hFrmSend,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
  //Usuwanie serwisu wstawiania tekstu ze schowka jako cytat
  PluginLink.DestroyServiceFunction(ServiceQuickQuoteItem);
  //Wyladowanie wszystkich hookow
  PluginLink.UnhookEvent(OnCloseTabMessage);
  PluginLink.UnhookEvent(OnCloseTab);
  PluginLink.UnhookEvent(OnActiveTab);
  PluginLink.UnhookEvent(OnWindowEvent);
  PluginLink.UnhookEvent(OnRecvMsg);
  PluginLink.UnhookEvent(OnMsgComposing);
  PluginLink.UnhookEvent(OnTabCaption);
  PluginLink.UnhookEvent(OnStateChange);
  PluginLink.UnhookEvent(OnSetLastState);
  PluginLink.UnhookEvent(OnContactsUpdate);
  PluginLink.UnhookEvent(OnMsgContextPopup);
  PluginLink.UnhookEvent(OnMsgContextClose);
  PluginLink.UnhookEvent(OnRestartingAQQ);
  //Hook systemowy
  if(hKeyboard!=NULL)
   UnhookWindowsHookEx(hKeyboard);
  //Przypisanie starej procki do okna rozmowy
  if(g_OldWndProc!=NULL)
  {
	SetWindowLongW(hFrmSend, GWL_WNDPROC,(LONG)g_OldWndProc);
	g_OldWndProc = NULL;
  }
  //Sprawdzanie czy wtyczka ma pamietac sesje
  if((RestoreTabsSessionChk)&&(!ManualRestoreTabsSessionChk))
  {
	TIniFile *Ini = new TIniFile(SessionFileDir);
	Ini->EraseSection("Session");
    delete Ini;
  }
  //Zmienne
  delete MsgList;
  delete TabsList;
  delete ClosedTabsList;
  delete ClosedTabsTimeList;
  delete InactiveTabsNewMsgCount;
  delete InactiveTabsNewMsgList;
  //Przywracanie oryginalnego titlebar glownego okna
  SetWindowTextW(hFrmMain,OryginalTitlebar);
  SetWindowTextW(hFrmMainL,OryginalTitlebar);
  //Przywracanie oryginalnego titlebar okna rozmowy
  TweakFrmSendTitlebarChk = false;
  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_PRIMARYTAB,OnPrimaryTab);
  PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
  PluginLink.UnhookEvent(OnPrimaryTab);
  return 0;
}
//---------------------------------------------------------------------------

//Ustawienia wtyczki
extern "C" int __declspec(dllexport)__stdcall Settings()
{
  if(hSettingsForm==NULL)
  {
	Application->Handle = (HWND)SettingsForm;
	hSettingsForm = new TSettingsForm(Application);
  }
  hSettingsForm->Show();
  hSettingsForm->TweakFrmMainTitlebarMode1RadioButton->Caption = "AQQ [" + ActiveProfileName + "]";
  if(!AQQVersionChk) hSettingsForm->InactiveTabsNewMsgCheckBox->Enabled = false;
  if(!EmuTabsWSupport) hSettingsForm->EmuTabsWCheckBox->Enabled = false;

  return 0;
}
//---------------------------------------------------------------------------

//Informacja o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  //Sprawdzanie wersji AQQ
  if (PLUGIN_COMPARE_VERSION(AQQVersion,PLUGIN_MAKE_VERSION(2,2,4,23))<0)
  {
	AQQVersionChk = false;
	//AQQVersion=false;
	/*MessageBox(Application->Handle,
	  "Wymagana wesja AQQ przez wtyczkê to minimum 2.2.4.21!\n"
	  "Wtyczka TabKit nie bêdzie dzia³aæ poprawnie!",
	  "Nieprawid³owa wersja AQQ",
	  MB_OK | MB_ICONEXCLAMATION);*/
  }
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"TabKit";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,3,6);
  PluginInfo.Description = (wchar_t*)L"";
  PluginInfo.Author = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t*)L"email@beherit.pl";
  PluginInfo.Copyright = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t*)L"http://beherit.pl/";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

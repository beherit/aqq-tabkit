//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "SettingsFrm.h"
#include "Aqq.h"
#include <inifiles.hpp>
#include <process.h>
#define AQQRESTARTER_SYSTEM_RESTARTING L"AQQRestarter/System/Restarting"
//---------------------------------------------------------------------------

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

TSettingsForm *hSettingsForm;

//Struktury
TPluginLink PluginLink;
TPluginInfo PluginInfo;
TPluginContact PluginContact;
PPluginContact Contact;
PPluginMessage Message;
PPluginWindowEvent WindowEvent;
TPluginShowInfo PluginShowInfo;
TPluginShowInfo PluginShowInfo2;
PPluginChatState ChatState;
//UnsentMsg
TPluginAction FrmUnsentMsgPopUp;
TPluginAction FrmMainUnsentMsgButton, FrmSendUnsentMsgButton;
TPluginAction BuildFrmUnsentMsgItem1, BuildFrmUnsentMsgItem2, BuildFrmUnsentMsgItem3, BuildFrmUnsentMsgItem4, BuildFrmUnsentMsgItem5;
//ClosedTabs
TPluginAction FrmClosedTabsPopUp;
TPluginAction FrmMainClosedTabsButton, FrmSendClosedTabsButton;
TPluginAction BuildFrmClosedTabsItem[10];
TPluginAction DestroyFrmClosedTabsItem;
//Ikonki interfesju
int UNSENTMSG;
int CLOSEDTABS;

//UnsentMsg
bool UnsentMsgChk;
bool InfoUnsentMsgChk;
bool CloudUnsentMsgChk;
bool DetailedCloudUnsentMsgChk;
bool TrayUnsentMsgChk;
bool FastAccessUnsentMsgChk;
bool FrmMainUnsentMsgChk;
bool FrmSendUnsentMsgChk;
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
bool UnCloseTabHotKeyChk;
int UnCloseTabHotKeyMode;
int UnCloseTabHotKeyDef;
int CountUnCloseTabVal;
int ItemCountUnCloseTabVal;
UnicodeString JustUnClosedJID;
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
int InactiveFrmNewMsgCount = 0;

//Zmienne Itemow popupmenu0
UnicodeString ItemJID;
UnicodeString ItemName;
UnicodeString ItemService;
//JID kontaktu
UnicodeString JID;
UnicodeString ActiveJID;
//Inne dane kontaktu
UnicodeString Nick;
UnicodeString Status;
//Rodzaj kontaktu
bool IsChat;
//ID konta
int UserIdx;
//Tresc wiadomosci
UnicodeString Body;
ShortString BodyShort;
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
wchar_t OryginalTitlebar[1024];
wchar_t TempTitlebarW[1024];
UnicodeString TempTitlebar;
wchar_t WindowTitlebarW[1024];
UnicodeString WindowTitlebar;
//Klasa okna
wchar_t WClassName[1024];
//Pozycja kursora w RichEdit
CHARRANGE SelPos;
//Do WindowEvent
int Event;
UnicodeString Typ;
//Hook na klawiaturê
HHOOK hKeyboard;
//PID procesu okna
DWORD PID;
//Stara procka FrmSend
WNDPROC g_OldWndProc;

//Lista JID z nowymi wiadomosciami
TStringList *MsgList = new TStringList;
//Lista JID otwartych zakladek
TStringList *TabsList = new TStringList;
//Lista JID zamknietych zakladek
TStringList *ClosedTabsList = new TStringList;

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

	  if(getpid()==PID)
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

	if(getpid()==PID)
	{
	  hFrmMain = hWnd;
	  GetWindowTextW(hFrmMain,OryginalTitlebar,sizeof(OryginalTitlebar));
	  return false;
	}
  }
  return true;
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

//Otwieranie zakladki z niewyslana wiadomoscia
void GetUnsentMsgItem(int Item)
{
  TIniFile *Ini = new TIniFile(SessionFileDir);
  TStringList *Messages = new TStringList;
  Ini->ReadSection("Messages",Messages);
  UnicodeString mJID = Messages->Strings[Item];
  delete Messages;
  delete Ini;
  //Otwieranie zakladki z kontektem z
  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)mJID.w_str());
}
//---------------------------------------------------------------------------

//Serwisy elementow niewysylanych wiadomosci
int __stdcall ServiceFrmUnsentMsgItem1 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(0);
  return 0;
}
int __stdcall ServiceFrmUnsentMsgItem2 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(1);
  return 0;
}
int __stdcall ServiceFrmUnsentMsgItem3 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(2);
  return 0;
}
int __stdcall ServiceFrmUnsentMsgItem4 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(3);
  return 0;
}
int __stdcall ServiceFrmUnsentMsgItem5 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(4);
  return 0;
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
	for(int Count=0;Count<MsgCount;Count++)
	{
	  switch(Count)
	  {
		case 0:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmUnsentMsgItem1));
		 break;
		case 1:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmUnsentMsgItem2));
		 break;
		case 2:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmUnsentMsgItem3));
		 break;
		case 3:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmUnsentMsgItem4));
		 break;
		case 4:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmUnsentMsgItem5));
		 break;
	  }
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

	  UnicodeString mJID;
	  //Tworzenie PopUpMenuItems
	  for(int Count=0;Count<MsgCount;Count++)
	  {
		mJID = Messages->Strings[Count];
		if(!mJID.IsEmpty())
		{
		  switch(Count)
		  {
			case 0:
			 BuildFrmUnsentMsgItem1.cbSize = sizeof(TPluginAction);
			 BuildFrmUnsentMsgItem1.pszName = L"FrmUnsentMsgItem1";
			 BuildFrmUnsentMsgItem1.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmUnsentMsgItem1.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmUnsentMsgItem1.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));
			 BuildFrmUnsentMsgItem1.pszService = L"sFrmUnsentMsgItem1";
			 BuildFrmUnsentMsgItem1.pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem1));
			 break;
			case 1:
			 BuildFrmUnsentMsgItem2.cbSize = sizeof(TPluginAction);
			 BuildFrmUnsentMsgItem2.pszName = L"FrmUnsentMsgItem2";
			 BuildFrmUnsentMsgItem2.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmUnsentMsgItem2.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmUnsentMsgItem2.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));
			 BuildFrmUnsentMsgItem2.pszService = L"sFrmUnsentMsgItem2";
			 BuildFrmUnsentMsgItem2.pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem2));
			 break;
			case 2:
			 BuildFrmUnsentMsgItem3.cbSize = sizeof(TPluginAction);
			 BuildFrmUnsentMsgItem3.pszName = L"FrmUnsentMsgItem3";
			 BuildFrmUnsentMsgItem3.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmUnsentMsgItem3.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmUnsentMsgItem3.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));
			 BuildFrmUnsentMsgItem3.pszService = L"sFrmUnsentMsgItem3";
			 BuildFrmUnsentMsgItem3.pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem3));
			 break;
			case 3:
			 BuildFrmUnsentMsgItem4.cbSize = sizeof(TPluginAction);
			 BuildFrmUnsentMsgItem4.pszName = L"FrmUnsentMsgItem4";
			 BuildFrmUnsentMsgItem4.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmUnsentMsgItem4.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmUnsentMsgItem4.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));
			 BuildFrmUnsentMsgItem4.pszService = L"sFrmUnsentMsgItem4";
			 BuildFrmUnsentMsgItem4.pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem4));
			 break;
			case 4:
			 BuildFrmUnsentMsgItem5.cbSize = sizeof(TPluginAction);
			 BuildFrmUnsentMsgItem5.pszName = L"FrmUnsentMsgItem5";
			 BuildFrmUnsentMsgItem5.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmUnsentMsgItem5.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmUnsentMsgItem5.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));
			 BuildFrmUnsentMsgItem5.pszService = L"sFrmUnsentMsgItem5";
			 BuildFrmUnsentMsgItem5.pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem5));
			 break;
		  }
		}
	  }
	}
	delete Messages;
  }
  //Szybki dostep niewyslanych wiadomosci
  else
   DestroyFrmUnsentMsg();
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
	  for(int Count=0;Count<TabsCount;Count++)
	  {
		JID = Ini->ReadString("ClosedTabs","Tab"+IntToStr(Count+1),"");
		if(!JID.IsEmpty())
		 ClosedTabsList->Add(JID);
	  }
	}
	else
	 ClosedTabsList->Clear();
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
		 Ini->WriteString("ClosedTabs","Tab"+IntToStr(Count+1),ClosedTabsList->Strings[Count]);
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
  UnicodeString mJID = ClosedTabsList->Strings[Item];
  //Zapisywanie JID aktualnie przywracanej zakladki
  JustUnClosedJID = mJID;
  //Otwieranie zakladki z kontektem z
  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)mJID.w_str());
}
//---------------------------------------------------------------------------

//Serwisy elementow ostatnio zamknietych zakladek
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
	  for(int Count=0;Count<TabsCount;Count++)
	  {
		ItemJID = ClosedTabsList->Strings[Count];
		if(!ItemJID.IsEmpty())
		{
		  BuildFrmClosedTabsItem[Count].cbSize = sizeof(TPluginAction);
		  ItemName = "FrmClosedTabsItem"+IntToStr(Count);
		  BuildFrmClosedTabsItem[Count].pszName = ItemName.w_str();
		  ItemService = "sFrmClosedTabsItem"+IntToStr(Count);
		  BuildFrmClosedTabsItem[Count].pszService = ItemService.w_str();
		  BuildFrmClosedTabsItem[Count].pszCaption = GetContactNick(ItemJID).w_str();
		  BuildFrmClosedTabsItem[Count].Position = Count;
		  PluginContact.cbSize = sizeof(TPluginContact);
		  PluginContact.JID = ItemJID.w_str();
		  BuildFrmClosedTabsItem[Count].IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));
       	  BuildFrmClosedTabsItem[Count].pszPopupName = (wchar_t*) L"FrmClosedTabsPopUp";
		  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmClosedTabsItem[Count]));
		}
	  }
	}
  }
  //Szybki dostep niewyslanych wiadomosci
  else
   DestroyFrmClosedTabs();
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
	  UnicodeString mJID;
	  UnicodeString Hint;
	  UnicodeString mBody;

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
			mJID = Messages->Strings[Count];
			mBody = UTF8ToUnicodeString(Ini->ReadString("Messages", mJID, "").t_str());
			if(mBody.Length()>25)
			 mBody = mBody.SetLength(25) + "...";

			PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
			PluginShowInfo.Event = tmeInfo;
			UnicodeString TmJID = GetContactNick(mJID);
			if(!TmJID.IsEmpty())
			 PluginShowInfo.Text = (TmJID + "\r\n" + mBody).w_str();
			else
			 PluginShowInfo.Text = (mJID + "\r\n" + mBody).w_str();
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
	UnicodeString mJID;
	for(int Count=0;Count<MsgCount;Count++)
	{
	  mJID = Messages->Strings[Count];
	  //Otwieranie zakladki z kontektem
	  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)mJID.w_str());
	}
  }
  delete Messages;
  delete Ini;
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

LRESULT CALLBACK FrmSendProc(HWND hwnd, UINT mesg, WPARAM wParam, LPARAM lParam)
{
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

  return CallWindowProc(g_OldWndProc, hwnd, mesg, wParam, lParam);
}
//---------------------------------------------------------------------------

//Pobieranie listy wszystkich otartych zakladek/okien
int __stdcall OnFetchAllTabs (WPARAM wParam, LPARAM lParam)
{
  if((wParam!=0)&&(lParam!=0))
  {
	Contact = (PPluginContact)lParam;
	JID = (wchar_t*)(Contact->JID);
	//Dodawanie info na poczatku JID jezeli kontakt jest czatem
	IsChat = (bool)(Contact->IsChat);
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
		ClosedTabsList->Delete(ClosedTabsList->IndexOf(JID));
		//Maks pamietanych X elementów
		if(ClosedTabsList->Count>CountUnCloseTabVal)
		{
		  while(ClosedTabsList->Count>CountUnCloseTabVal)
		   ClosedTabsList->Delete(CountUnCloseTabVal);
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
  IsChat = (bool)(Contact->IsChat);

  if(!IsChat)
  {
    JID = (wchar_t*)(Contact->JID);
	Nick = (wchar_t*)(Contact->Nick);
	Status = (wchar_t*)(Contact->Status);
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

//Hook na zamkniecie okiena rozmowy
int __stdcall OnWindowEvent (WPARAM wParam, LPARAM lParam)
{
  WindowEvent = (PPluginWindowEvent)lParam;
  Event = WindowEvent->WindowEvent;
  Typ = (wchar_t*)(WindowEvent->ClassName);

  //Otwarcie glownego okna = zaladowanie w pelni listy kontatkow
  if((Typ=="TfrmMain")&&(Event==1))
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
		  JID = Session->Strings[Count];
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
  if((Typ=="TfrmSend")&&(Event==1))
  {
	if(hFrmSend==NULL)
	{
	  //Przypisanie uchwytu do okna rozmowy
	  hFrmSend = (HWND)WindowEvent->Handle;
	  //Szukanie pola wiadomosci
	  EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
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
  if((Typ=="TfrmSend")&&(Event==2))
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
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na aktwyna zakladke lub okno rozmowy
int __stdcall OnActiveTab (WPARAM wParam, LPARAM lParam)
{
  Contact = (PPluginContact)lParam;
  JID = (wchar_t*)(Contact->JID);
  //Dodawanie info na poczatku JID jezeli kontakt jest czatem
  IsChat = (bool)(Contact->IsChat);
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
		Body = UTF8ToUnicodeString(Ini->ReadString("SessionMsg", JID, "").t_str());
		//Wczytanie tresci wiadomosci do pola RichEdit
		if(!Body.IsEmpty())
		{
		  UserIdx = Contact->UserIdx;
		  //Pobieranie ostatniej wiadomoœci
		  PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)UserIdx);
		  //Ustawianie tekstu
		  SetWindowTextW(hRichEdit, Body.w_str());
		  //Ustawianie pozycji kursora
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
	  Body = UTF8ToUnicodeString(Ini->ReadString("Messages", JID, "").t_str());

	  //Wczytanie tresci wiadomosci do pola RichEdit
	  if(!Body.IsEmpty())
	  {
		UserIdx = Contact->UserIdx;
		//Pobieranie ostatniej wiadomoœci
		PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)UserIdx);
		//Ustawianie tekstu
		SetWindowTextW(hRichEdit, Body.w_str());
		//Ustawianie pozycji kursora
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
		if(JustUnClosedJID==JID)
		{
          UserIdx = Contact->UserIdx;
		  //Pobieranie ostatniej wiadomoœci
		  PluginLink.CallService(AQQ_FUNCTION_LOADLASTCONV,(WPARAM)JID.w_str(),(LPARAM)UserIdx);
          JustUnClosedJID = "";
        }
		//Usuwanie interfejsu
		DestroyFrmClosedTabs();
		//Usuwanie JID z tablicy
		ClosedTabsList->Delete(ClosedTabsList->IndexOf(JID));
		//Maks pamietanych X elementów
		if(ClosedTabsList->Count>CountUnCloseTabVal)
		{
		  while(ClosedTabsList->Count>CountUnCloseTabVal)
		   ClosedTabsList->Delete(CountUnCloseTabVal);
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
	  Nick = (wchar_t*)(Contact->Nick);
	  Status = (wchar_t*)(Contact->Status);
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

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie okna rozmowy lub zakladki
int __stdcall OnCloseTab(WPARAM wParam, LPARAM lParam)
{
  Contact = (PPluginContact)lParam;
  JID = (wchar_t*)(Contact->JID);
  //Dodawanie info na poczatku JID jezeli kontakt jest czatem
  IsChat = (bool)(Contact->IsChat);
  if(IsChat)
   JID = "TabKit_IsChat_" + JID;

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
	  //Maks pamietanych X elementów
	  if(ClosedTabsList->Count>CountUnCloseTabVal)
	  {
		while(ClosedTabsList->Count>CountUnCloseTabVal)
		 ClosedTabsList->Delete(CountUnCloseTabVal);
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
	  JID = (wchar_t*)(Contact->JID);
	  Body = (wchar_t*)wParam;
	  Body = Body.Trim();
	  if(!Body.IsEmpty())
	  {
		//Szybki dostep niewyslanych wiadomosci
		DestroyFrmUnsentMsg();
		//Zapis pliku sesji
		BodyShort = UTF8EncodeToShortString(Body);
		TIniFile *Ini = new TIniFile(SessionFileDir);
		Ini->WriteString("Messages", JID, String(BodyShort.operator AnsiString()));
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
	  JID = (wchar_t*)(Contact->JID);
	  //Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
	  if((JID!=ActiveJID)&&(TabsList->IndexOf(JID)!=-1))
	  {
		Message = (PPluginMessage)lParam;
		//Rodzaj wiadomosci
		if(Message->Kind==MSGKIND_CHAT)
		{
		  Body = (wchar_t*)(Message->Body);
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
    //Jezeli kontakt nie jest czatem
	if(!Contact->IsChat)
	{
      JID = (wchar_t*)(Contact->JID);
	  //Jezeli JID jest rozny od JID z aktywnej zakladki i zakladka jest otwarta
	  //Jezeli zakladka jest otwarta
	  if((TabsList->IndexOf(JID)!=-1))//((JID!=ActiveJID)&&(TabsList->IndexOf(JID)!=-1))
	  {
		//Jezeli okno rozmowy jest nieaktywne
		if(GetForegroundWindow()!=hFrmSend)
		{
		  Message = (PPluginMessage)lParam;
		  //Rodzaj wiadomosci
		  if(Message->Kind==MSGKIND_CHAT)
		  {
			Body = (wchar_t*)(Message->Body);
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
			  GetWindowTextW(hFrmSend,WindowTitlebarW,sizeof(WindowTitlebarW));
			  WindowTitlebar = (wchar_t*)WindowTitlebarW;
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

	JID = (wchar_t*)Contact->JID;
	Body = (wchar_t*)ChatState->Text;
	Body = Body.Trim();
	if(!Body.IsEmpty())
	{
	  BodyShort = UTF8EncodeToShortString(Body);
	  TIniFile *Ini = new TIniFile(SessionFileDir);
	  Ini->WriteString("SessionMsg", JID, String(BodyShort.operator AnsiString()));
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
		if(((UnicodeString)WClassName=="TfrmSend")&&(getpid()==PID))
		{
		  //Jezeli tablica cos zawiera
		  if(MsgList->Count>0)
		  {
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
		  if(((UnicodeString)WClassName=="TfrmSend")&&(getpid()==PID))
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
				JID = TabsList->Strings[Key-1];
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
		  if(((UnicodeString)WClassName=="TfrmSend")&&(getpid()==PID))
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
			  JID = TabsList->Strings[Key-1];
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
		   if(((UnicodeString)WClassName=="TfrmSend")&&(getpid()==PID))
		   {
			 if(ClosedTabsList->Count>0)
			 {
			   JID = ClosedTabsList->Strings[0];
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
		if(((UnicodeString)WClassName=="TfrmSend")&&(getpid()==PID))
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
				 JID = ClosedTabsList->Strings[0];
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
				 JID = ClosedTabsList->Strings[0];
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
				 JID = ClosedTabsList->Strings[0];
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
				 JID = ClosedTabsList->Strings[0];
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
				 JID = ClosedTabsList->Strings[0];
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
				 JID = ClosedTabsList->Strings[0];
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
  UnCloseTabHotKeyChk =  Ini->ReadBool("ClosedTabs","HotKey",false);
  UnCloseTabHotKeyMode = Ini->ReadInteger("ClosedTabs","HotKeyMode",1);
  UnCloseTabHotKeyDef = Ini->ReadInteger("ClosedTabs","HotKeyDef",0);
  CountUnCloseTabVal = Ini->ReadInteger("ClosedTabs","Count",10);
  ItemCountUnCloseTabVal = Ini->ReadInteger("ClosedTabs","ItemsCount",5);
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
  //Przypisanie ikonki do interfejsu AQQ
  UNSENTMSG = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png").w_str());
  CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\ClosedTabsButton.png").w_str());
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
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem1",ServiceFrmUnsentMsgItem1);
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem2",ServiceFrmUnsentMsgItem2);
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem3",ServiceFrmUnsentMsgItem3);
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem4",ServiceFrmUnsentMsgItem4);
  PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem5",ServiceFrmUnsentMsgItem5);
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
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItem1);
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItem2);
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItem3);
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItem4);
  PluginLink.DestroyServiceFunction(ServiceFrmUnsentMsgItem5);
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
  //Usuwanie PopUpMenu
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU,0,(LPARAM)(&FrmClosedTabsPopUp));
  //Wyladowanie wszystkich hookow
  PluginLink.UnhookEvent(OnCloseTabMessage);
  PluginLink.UnhookEvent(OnCloseTab);
  PluginLink.UnhookEvent(OnActiveTab);
  PluginLink.UnhookEvent(OnWindowEvent);
  PluginLink.UnhookEvent(OnRecvMsg);
  PluginLink.UnhookEvent(OnMsgComposing);
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

  return 0;
}
//---------------------------------------------------------------------------

//Informacja o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"TabKit";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,2,0);
  PluginInfo.Description = (wchar_t*)L"";
  PluginInfo.Author = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t*)L"email@beherit.pl";
  PluginInfo.Copyright = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t*)L"http://beherit.pl/";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

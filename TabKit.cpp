//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "SettingsFrm.h"
#include "Aqq.h"
#include <inifiles.hpp>
#include <process.h>
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
//UnsentMsg
TPluginAction FrmUnsentMsgPopUp;
TPluginAction FrmMainUnsentMsgButton, FrmSendUnsentMsgButton;
TPluginAction BuildFrmUnsentMsgItem0, BuildFrmUnsentMsgItem1, BuildFrmUnsentMsgItem2, BuildFrmUnsentMsgItem3, BuildFrmUnsentMsgItem4;
//ClosedTabs
TPluginAction FrmClosedTabsPopUp;
TPluginAction FrmMainClosedTabsButton, FrmSendClosedTabsButton;
TPluginAction BuildFrmClosedTabsItem0, BuildFrmClosedTabsItem1, BuildFrmClosedTabsItem2, BuildFrmClosedTabsItem3, BuildFrmClosedTabsItem4;
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

//JID kontaktu
UnicodeString JID;
UnicodeString ActiveJID;
//Rodzaj kontaktu
bool IsChat;
//ID konta
int UserIdx;
//Tresc wiadomosci
UnicodeString Body;
ShortString BodyShort;
//Sciezka do pliku sesji
UnicodeString SessionFileDir;
//Uchwyt do okna rozmowy
HWND hFrmSend;
HWND hActiveFrm;
//Uchwyt do pola RichEdit
HWND hRichEdit;
//Klasa okna
wchar_t WClassName[2048];
//Pozycja kursora w RichEdit
CHARRANGE SelPos;
//Do WindowEvent
int Event;
UnicodeString Typ;
//Hook na klawiaturê
HHOOK hKeyboard;
//PID procesu okna
DWORD PID;

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

//Pobieranie Nick kontaktu podajac jego JID
UnicodeString GetContactNick(UnicodeString JID)
{
  TPluginContactSimpleInfo PluginContactSimpleInfo;
  PluginContactSimpleInfo.cbSize = sizeof(TPluginContactSimpleInfo);
  PluginContactSimpleInfo.JID = JID.w_str();
  PluginLink.CallService(AQQ_CONTACTS_FILLSIMPLEINFO,0,(LPARAM)(&PluginContactSimpleInfo));
  UnicodeString Nick = (wchar_t*)(PluginContactSimpleInfo.Nick);
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
  //Otwieranie zakladki z kontektem z
  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)mJID.w_str());
  delete Messages;
  delete Ini;
}
//---------------------------------------------------------------------------

//Serwisy elementow niewysylanych wiadomosci
int __stdcall FrmUnsentMsgItem0 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(0);
  return 0;
}
int __stdcall FrmUnsentMsgItem1 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(1);
  return 0;
}
int __stdcall FrmUnsentMsgItem2 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(2);
  return 0;
}
int __stdcall FrmUnsentMsgItem3 (WPARAM, LPARAM)
{
  GetUnsentMsgItem(3);
  return 0;
}
int __stdcall FrmUnsentMsgItem4 (WPARAM, LPARAM)
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
	//Usuwanie elementow popupmenu
	for(int Count=0;Count<MsgCount;Count++)
	{
	  switch(Count)
	  {
		case 0:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmUnsentMsgItem0));
		 PluginLink.DestroyServiceFunction(FrmUnsentMsgItem0);
		 break;
		case 1:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmUnsentMsgItem1));
		 PluginLink.DestroyServiceFunction(FrmUnsentMsgItem1);
		 break;
		case 2:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmUnsentMsgItem2));
		 PluginLink.DestroyServiceFunction(FrmUnsentMsgItem2);
		 break;
		case 3:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmUnsentMsgItem3));
		 PluginLink.DestroyServiceFunction(FrmUnsentMsgItem3);
		 break;
		case 4:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmUnsentMsgItem4));
		 PluginLink.DestroyServiceFunction(FrmUnsentMsgItem4);
		 break;
	  }
	}
	//Usuwanie buttona na FrmMain
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmMainUnsentMsgButton));
	//Usuwanie buttona na FrmSend
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmSendUnsentMsgButton));
	//Usuwanie PopUpMenu
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU ,0,(LPARAM)(&FrmUnsentMsgPopUp));
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
	int MsgCount = Messages->Count;
	//Maks 5 elementow w popupmenu
	if(MsgCount>5) MsgCount = 5;
	//Jezeli w ogole cos jest
	if(MsgCount>0)
	{
	  //Tworzenie PopUpMenu
	  FrmUnsentMsgPopUp.cbSize = sizeof(TPluginAction);
	  FrmUnsentMsgPopUp.pszName = L"FrmUnsentMsgPopUp";
	  FrmUnsentMsgPopUp.pszPopupName = (wchar_t*)L"FrmUnsentMsgPopUp";
	  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENU,0,(LPARAM)(&FrmUnsentMsgPopUp));
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
			 BuildFrmUnsentMsgItem0.cbSize = sizeof(TPluginAction);
			 BuildFrmUnsentMsgItem0.pszName = L"FrmUnsentMsgItem0";
			 BuildFrmUnsentMsgItem0.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmUnsentMsgItem0.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmUnsentMsgItem0.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));//-1;
			 //BuildFrmUnsentMsgItem1.IconIndex = -1;
			 BuildFrmUnsentMsgItem0.pszService = L"sFrmUnsentMsgItem0";
			 BuildFrmUnsentMsgItem0.pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem0));
			 PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem0" ,FrmUnsentMsgItem0);
			 break;
			case 1:
			 BuildFrmUnsentMsgItem1.cbSize = sizeof(TPluginAction);
			 BuildFrmUnsentMsgItem1.pszName = L"FrmUnsentMsgItem1";
			 BuildFrmUnsentMsgItem1.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmUnsentMsgItem1.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmUnsentMsgItem1.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));//-1;
			 //BuildFrmUnsentMsgItem1.IconIndex = -1;
			 BuildFrmUnsentMsgItem1.pszService = L"sFrmUnsentMsgItem1";
			 BuildFrmUnsentMsgItem1.pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem1));
			 PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem1" ,FrmUnsentMsgItem1);
			 break;
			case 2:
			 BuildFrmUnsentMsgItem2.cbSize = sizeof(TPluginAction);
			 BuildFrmUnsentMsgItem2.pszName = L"FrmUnsentMsgItem2";
			 BuildFrmUnsentMsgItem2.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmUnsentMsgItem2.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmUnsentMsgItem2.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));//-1;
			 //BuildFrmUnsentMsgItem2.IconIndex = -1;
			 BuildFrmUnsentMsgItem2.pszService = L"sFrmUnsentMsgItem2";
			 BuildFrmUnsentMsgItem2.pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem2));
			 PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem2" ,FrmUnsentMsgItem2);
			 break;
			case 3:
			 BuildFrmUnsentMsgItem3.cbSize = sizeof(TPluginAction);
			 BuildFrmUnsentMsgItem3.pszName = L"FrmUnsentMsgItem3";
			 BuildFrmUnsentMsgItem3.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmUnsentMsgItem3.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmUnsentMsgItem3.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));//-1;
			 //BuildFrmUnsentMsgItem3.IconIndex = -1;
			 BuildFrmUnsentMsgItem3.pszService = L"sFrmUnsentMsgItem3";
			 BuildFrmUnsentMsgItem3.pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem3));
			 PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem3" ,FrmUnsentMsgItem3);
			 break;
			case 4:
			 BuildFrmUnsentMsgItem4.cbSize = sizeof(TPluginAction);
			 BuildFrmUnsentMsgItem4.pszName = L"FrmUnsentMsgItem4";
			 BuildFrmUnsentMsgItem4.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmUnsentMsgItem4.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmUnsentMsgItem4.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));//-1;
			 //BuildFrmUnsentMsgItem4.IconIndex = -1;
			 BuildFrmUnsentMsgItem4.pszService = L"sFrmUnsentMsgItem4";
			 BuildFrmUnsentMsgItem4.pszPopupName = (wchar_t*) L"FrmUnsentMsgPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmUnsentMsgItem4));
			 PluginLink.CreateServiceFunction(L"sFrmUnsentMsgItem4" ,FrmUnsentMsgItem4);
			 break;
          }
		}
	  }
	}
	delete Ini;
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
  //Otwieranie zakladki z kontektem z
  PluginLink.CallService(AQQ_FUNCTION_EXECUTEMSG,0,(LPARAM)mJID.w_str());
}
//---------------------------------------------------------------------------

//Serwisy elementow ostatnio zamknietych zakladek
int __stdcall FrmClosedTabsItem0 (WPARAM, LPARAM)
{
  GetClosedTabsItem(0);
  return 0;
}
int __stdcall FrmClosedTabsItem1 (WPARAM, LPARAM)
{
  GetClosedTabsItem(1);
  return 0;
}
int __stdcall FrmClosedTabsItem2 (WPARAM, LPARAM)
{
  GetClosedTabsItem(2);
  return 0;
}
int __stdcall FrmClosedTabsItem3 (WPARAM, LPARAM)
{
  GetClosedTabsItem(3);
  return 0;
}
int __stdcall FrmClosedTabsItem4 (WPARAM, LPARAM)
{
  GetClosedTabsItem(4);
  return 0;
}
//---------------------------------------------------------------------------

//Usuwanie interfejsu dla ostatio zamknietych zakladek
void DestroyFrmClosedTabs()
{
  int TabsCount = ClosedTabsList->Count;
  //Maks 5 elementow w popupmenu
  if(TabsCount>5) TabsCount = 5;
  //Jezeli cos w ogule jest
  if(TabsCount>0)
  {
	//Usuwanie elementow popupmenu
	for(int Count=0;Count<TabsCount;Count++)
	{
	  switch(Count)
	  {
		case 0:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmClosedTabsItem0));
		 PluginLink.DestroyServiceFunction(FrmClosedTabsItem0);
		 break;
		case 1:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmClosedTabsItem1));
		 PluginLink.DestroyServiceFunction(FrmClosedTabsItem1);
		 break;
		case 2:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmClosedTabsItem2));
		 PluginLink.DestroyServiceFunction(FrmClosedTabsItem2);
		 break;
		case 3:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmClosedTabsItem3));
		 PluginLink.DestroyServiceFunction(FrmClosedTabsItem3);
		 break;
		case 4:
		 PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM ,0,(LPARAM)(&BuildFrmClosedTabsItem4));
		 PluginLink.DestroyServiceFunction(FrmClosedTabsItem4);
		 break;
	  }
	}
	//Usuwanie buttona na FrmMain
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "ToolDown" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmMainClosedTabsButton));
	//Usuwanie buttona na FrmSend
	PluginLink.CallService(AQQ_CONTROLS_TOOLBAR "tbMain" AQQ_CONTROLS_DESTROYBUTTON ,0,(LPARAM)(&FrmSendClosedTabsButton));
	//Usuwanie PopUpMenu
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENU ,0,(LPARAM)(&FrmClosedTabsPopUp));
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
	//Maks 5 elementow w popupmenu
	if(TabsCount>5) TabsCount = 5;
	//Jezeli w ogole cos jest
	if(TabsCount>0)
	{
	  //Tworzenie PopUpMenu
	  FrmClosedTabsPopUp.cbSize = sizeof(TPluginAction);
	  FrmClosedTabsPopUp.pszName = L"FrmClosedTabsPopUp";
	  FrmClosedTabsPopUp.pszPopupName = (wchar_t*)L"FrmClosedTabsPopUp";
	  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENU,0,(LPARAM)(&FrmClosedTabsPopUp));
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

	  UnicodeString mJID;
	  //Tworzenie PopUpMenuItems
	  for(int Count=0;Count<TabsCount;Count++)
	  {
		mJID = ClosedTabsList->Strings[Count];
		if(!mJID.IsEmpty())
		{
		  switch(Count)
		  {
			case 0:
			 BuildFrmClosedTabsItem0.cbSize = sizeof(TPluginAction);
			 BuildFrmClosedTabsItem0.pszName = L"FrmClosedTabsItem0";
			 BuildFrmClosedTabsItem0.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmClosedTabsItem0.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmClosedTabsItem0.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));//-1;
			 //BuildFrmClosedTabsItem0.IconIndex = -1;
			 BuildFrmClosedTabsItem0.pszService = L"sFrmClosedTabsItem0";
			 BuildFrmClosedTabsItem0.pszPopupName = (wchar_t*) L"FrmClosedTabsPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmClosedTabsItem0));
			 PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem0" ,FrmClosedTabsItem0);
			 break;
			case 1:
			 BuildFrmClosedTabsItem1.cbSize = sizeof(TPluginAction);
			 BuildFrmClosedTabsItem1.pszName = L"FrmClosedTabsItem1";
			 BuildFrmClosedTabsItem1.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmClosedTabsItem1.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmClosedTabsItem1.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));//-1;
			 //BuildFrmClosedTabsItem1.IconIndex = -1;
			 BuildFrmClosedTabsItem1.pszService = L"sFrmClosedTabsItem1";
			 BuildFrmClosedTabsItem1.pszPopupName = (wchar_t*) L"FrmClosedTabsPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmClosedTabsItem1));
			 PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem1" ,FrmClosedTabsItem1);
			 break;
			case 2:
			 BuildFrmClosedTabsItem2.cbSize = sizeof(TPluginAction);
			 BuildFrmClosedTabsItem2.pszName = L"FrmClosedTabsItem2";
			 BuildFrmClosedTabsItem2.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmClosedTabsItem2.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmClosedTabsItem2.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));//-1;
			 //BuildFrmClosedTabsItem2.IconIndex = -1;
			 BuildFrmClosedTabsItem2.pszService = L"sFrmClosedTabsItem2";
			 BuildFrmClosedTabsItem2.pszPopupName = (wchar_t*) L"FrmClosedTabsPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmClosedTabsItem2));
			 PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem2" ,FrmClosedTabsItem2);
			 break;
			case 3:
			 BuildFrmClosedTabsItem3.cbSize = sizeof(TPluginAction);
			 BuildFrmClosedTabsItem3.pszName = L"FrmClosedTabsItem3";
			 BuildFrmClosedTabsItem3.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmClosedTabsItem3.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmClosedTabsItem3.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));//-1;
			 //BuildFrmClosedTabsItem3.IconIndex = -1;
			 BuildFrmClosedTabsItem3.pszService = L"sFrmClosedTabsItem3";
			 BuildFrmClosedTabsItem3.pszPopupName = (wchar_t*) L"FrmClosedTabsPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmClosedTabsItem3));
			 PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem3" ,FrmClosedTabsItem3);
			 break;
			case 4:
			 BuildFrmClosedTabsItem4.cbSize = sizeof(TPluginAction);
			 BuildFrmClosedTabsItem4.pszName = L"FrmClosedTabsItem4";
			 BuildFrmClosedTabsItem4.pszCaption = GetContactNick(mJID).w_str();
			 BuildFrmClosedTabsItem4.Position = Count;
			 PluginContact.cbSize = sizeof(TPluginContact);
			 PluginContact.JID = mJID.w_str();
			 BuildFrmClosedTabsItem4.IconIndex = PluginLink.CallService(AQQ_FUNCTION_GETSTATEPNG_INDEX,0,(LPARAM)(&PluginContact));//-1;
			 //BuildFrmClosedTabsItem4.IconIndex = -1;
			 BuildFrmClosedTabsItem4.pszService = L"sFrmClosedTabsItem4";
			 BuildFrmClosedTabsItem4.pszPopupName = (wchar_t*) L"FrmClosedTabsPopUp";
			 PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&BuildFrmClosedTabsItem4));
			 PluginLink.CreateServiceFunction(L"sFrmClosedTabsItem4" ,FrmClosedTabsItem4);
			 break;
          }
		}
	  }
	}
  }
  //Szybki dostep niewyslanych wiadomosci
  else
   DestroyFrmClosedTabs();
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
		//Maks X zdefiniowanych elementów
		if(ClosedTabsList->Count>CountUnCloseTabVal)
		{
		  while(ClosedTabsList->Count>CountUnCloseTabVal)
		   ClosedTabsList->Delete(CountUnCloseTabVal);
		}
		//Zapisywanie ostatnio zamknietych zakladek do pliku
		SaveClosedTabs();
	  }
	}

	//Przypisanie uchwytu okna rozmowy
	if(hFrmSend==NULL)
	 hFrmSend = (HWND)wParam;

  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie okiena rozmowy
int __stdcall OnWindowEvent (WPARAM wParam, LPARAM lParam)
{
  WindowEvent = (PPluginWindowEvent)lParam;
  Event = WindowEvent->WindowEvent;
  Typ = (wchar_t*)(WindowEvent->ClassName);

  //Otwarcie okna rozmowy
  if((Typ=="TfrmSend")&&(Event==1))
  {
	//Przypisanie uchwytu do okna rozmowy
	hFrmSend = (HWND)WindowEvent->Handle;
	//Szybki dostep niewyslanych wiadomosci
	DestroyFrmUnsentMsg();
	BuildFrmUnsentMsg();
	//Szybki dostep do ostatnio zamknietych zakladek
	DestroyFrmClosedTabs();
	BuildFrmClosedTabs();
  }
  //Zamkniecie okna rozmowy
  if((Typ=="TfrmSend")&&(Event==2))
  {
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

	//Niewyslane wiadomosci
	if((UnsentMsgChk)&&(!IsChat))
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
		//Pobranie uchwytu do zakladki lub okna rozmowy
		hFrmSend = (HWND)wParam;
		//Szukanie RichEdit
		EnumChildWindows(hFrmSend,(WNDENUMPROC)FindRichEdit,0);
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
	if(ClosedTabsChk)
	{
	  //Usuwanie JID z listy ostatnio zamknietych zakladek
	  if((ClosedTabsList->IndexOf(JID)!=-1)&&(!IsChat))
	  {
		//Usuwanie interfejsu
		DestroyFrmClosedTabs();
		//Usuwanie JID z tablicy
		ClosedTabsList->Delete(ClosedTabsList->IndexOf(JID));
		//Maks X zdefiniowanych elementów
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
	  //Maks X zdefiniowanych elementów
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
			  //Otwieranie zakladki z kontektem z kolejki
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
			  //Sprawdzanie czy wywolujemy zakladke "ducha"
			  if(Key<=TabsList->Count)
			  {
				//Usuwanie listy zakladek
				TabsList->Clear();
				//Hook na pobieranie aktywnych zakladek
				PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs);
				PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
				PluginLink.UnhookEvent(OnFetchAllTabs);
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
			//Sprawdzanie czy wywolujemy zakladke "ducha"
			if(Key<=TabsList->Count)
			{
			  //Usuwanie listy zakladek
			  TabsList->Clear();
			  //Hook na pobieranie aktywnych zakladek
			  PluginLink.HookEvent(AQQ_CONTACTS_BUDDY_FETCHALLTABS,OnFetchAllTabs);
			  PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
			  PluginLink.UnhookEvent(OnFetchAllTabs);
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

//Hook na zaladowanie wszystkich modulow
int __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
  //Pobieranie ostatnio zamknietych zakladek
  GetClosedTabs();
  //Tworzenie interfesju dla ostatnio zamknietych zakladek
  BuildFrmClosedTabs();
  //Sprawdzanie niewyslanych wiadomosci
  GetUnsentMsg();
  //Szybki dostep niewyslanych wiadomosci
  BuildFrmUnsentMsg();

  return 0;
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
  //1.0.0.0 -> 1.0.1.0
  if(Ini->ValueExists("ClosedTabs","UnCloseTabHotKey"))
  {
	Ini->WriteInteger("ClosedTabs","HotKey",Ini->ReadBool("ClosedTabs","UnCloseTabHotKey",false));
	Ini->DeleteKey("ClosedTabs","UnCloseTabHotKey");
  }
  if(Ini->ValueExists("ClosedTabs","UnCloseTabHotKeyMode"))
  {
	Ini->WriteInteger("ClosedTabs","HotKeyMode",Ini->ReadBool("ClosedTabs","UnCloseTabHotKeyMode",false));
	Ini->DeleteKey("ClosedTabs","UnCloseTabHotKeyMode");
  }
  if(Ini->ValueExists("ClosedTabs","UnCloseTabHotKeyDef"))
  {
	Ini->WriteInteger("ClosedTabs","HotKeyDef",Ini->ReadBool("ClosedTabs","UnCloseTabHotKeyDef",false));
	Ini->DeleteKey("ClosedTabs","UnCloseTabHotKeyDef");
  }
  //END
  UnCloseTabHotKeyChk =  Ini->ReadBool("ClosedTabs","HotKey",false);  
  UnCloseTabHotKeyMode = Ini->ReadInteger("ClosedTabs","HotKeyMode",1);
  UnCloseTabHotKeyDef = Ini->ReadInteger("ClosedTabs","HotKeyDef",0);
  CountUnCloseTabVal = Ini->ReadInteger("ClosedTabs","Count",5);
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
  //Folder z ustawieniami wtyczki etc
  if(!DirectoryExists(SessionFileDir + "\\\\TabKit"))
   CreateDir(SessionFileDir + "\\\\TabKit");
  //Wypakiwanie ikonki UnsentMsg.png
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png").t_str(),"UNSENTMSG");
  //Wypakiwanie ikonki ClosedTabs.png
  if(!FileExists(SessionFileDir + "\\\\TabKit\\\\ClosedTabs.png"))
   SaveResourceToFile((SessionFileDir + "\\\\TabKit\\\\ClosedTabs.png").t_str(),"CLOSEDTABS");
  //Przypisanie ikonki do interfejsu AQQ
  UNSENTMSG = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\UnsentMsg.png").w_str());
  CLOSEDTABS = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(SessionFileDir + "\\\\TabKit\\\\ClosedTabs.png").w_str());
  //Dodanie sciezki do pliku sesji
  SessionFileDir = SessionFileDir + "\\\\TabKit\\\\Session.ini";

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
  //Hook na zaladowanie wszystkich modulow
  PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED,OnModulesLoaded);
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
	PluginLink.CallService(AQQ_CONTACTS_BUDDY_FETCHALLTABS,0,0);
	PluginLink.UnhookEvent(OnFetchAllTabs);
	//Tworzenie interfesju dla ostatnio zamknietych zakladek
	BuildFrmClosedTabs();
	//Sprawdzanie niewyslanych wiadomosci
	GetUnsentMsg();
	//Szybki dostep niewyslanych wiadomosci
    BuildFrmUnsentMsg();
  }

  return 0;
}
//---------------------------------------------------------------------------

//Wyladowanie wtyczki
extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Szybki dostep niewyslanych wiadomosci
  DestroyFrmUnsentMsg();
  //Szybki dostep ostatnio zamknietych zakladek
  DestroyFrmClosedTabs();
  //Wyladowanie wszystkich hookow
  PluginLink.UnhookEvent(OnCloseTabMessage);
  PluginLink.UnhookEvent(OnCloseTab);
  PluginLink.UnhookEvent(OnActiveTab);
  PluginLink.UnhookEvent(OnWindowEvent);
  PluginLink.UnhookEvent(OnModulesLoaded);
  PluginLink.UnhookEvent(OnRecvMsg);
  //Hook systemowy
  if(hKeyboard!=NULL)
   UnhookWindowsHookEx(hKeyboard);
  //Zmienne
  delete MsgList;
  delete TabsList;

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

  return 0;
}
//---------------------------------------------------------------------------

//Informacja o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"TabKit";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,1,0);
  PluginInfo.Description = (wchar_t*)L"";
  PluginInfo.Author = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t*)L"email@beherit.pl";
  PluginInfo.Copyright = (wchar_t*)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t*)L"http://beherit.pl/";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

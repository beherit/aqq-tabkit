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
#ifndef SettingsFrmH
#define SettingsFrmH
#define WM_ALPHAWINDOWS (WM_USER + 666)
//---------------------------------------------------------------------------
#include "acAlphaImageList.hpp"
#include "acPNG.hpp"
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sCheckBox.hpp"
#include "sComboBox.hpp"
#include "sEdit.hpp"
#include "sGroupBox.hpp"
#include "sLabel.hpp"
#include "sListView.hpp"
#include "sMemo.hpp"
#include "sPageControl.hpp"
#include "sRadioButton.hpp"
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include "sSpeedButton.hpp"
#include "sSpinEdit.hpp"
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <IdThreadComponent.hpp>
#include <System.Actions.hpp>
#include <System.Classes.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.FileCtrl.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.StdCtrls.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
class TSettingsForm : public TForm
{
__published:	// IDE-managed Components
	TsBevel *Bevel;
	TsButton *SaveButton;
	TsButton *CancelButton;
	TsCheckBox *FrmSendUnsentMsgCheckBox;
	TsCheckBox *FrmMainUnsentMsgCheckBox;
	TsCheckBox *CloudUnsentMsgCheckBox;
	TsCheckBox *TrayUnsentMsgCheckBox;
	TsRadioButton *SwitchToNewMsgMode1RadioButton;
	TsRadioButton *SwitchToNewMsgMode2RadioButton;
	TTrayIcon *UnsentMsgTrayIcon;
	TsCheckBox *DetailedCloudUnsentMsgCheckBox;
	TActionList *ActionList;
	TAction *aExit;
	TAction *aLoadSettings;
	TAction *aSaveSettings;
	TAction *aUnsentMsgChk;
	TAction *aTabsSwitchingChk;
	TsRadioButton *TabsHotKeysMode1RadioButton;
	TsRadioButton *TabsHotKeysMode2RadioButton;
	TsButton *OkButton;
	TAction *aSaveSettingsW;
	TsCheckBox *FrmMainClosedTabsCheckBox;
	TsCheckBox *FrmSendClosedTabsCheckBox;
	TsRadioButton *UnCloseTabHotKeyMode1RadioButton;
	TsRadioButton *UnCloseTabHotKeyMode2RadioButton;
	THotKey *UnCloseTabHotKeyInput;
	TAction *aClosedTabsChk;
	TsSpinEdit *CountClosedTabsSpinEdit;
	TAction *aSessionRememberChk;
	TsCheckBox *RestoreMsgSessionCheckBox;
	TsLabel *RestoreSessionLabel;
	TsRadioButton *TweakFrmSendTitlebarMode1RadioButton;
	TsRadioButton *TweakFrmSendTitlebarMode2RadioButton;
	TsRadioButton *TweakFrmSendTitlebarMode3RadioButton;
	TsRadioButton *TweakFrmMainTitlebarMode1RadioButton;
	TsRadioButton *TweakFrmMainTitlebarMode2RadioButton;
	TsEdit *TweakFrmMainTitlebarMode2Edit;
	TsSpinEdit *ItemsCountClosedTabsSpinEdit;
	TAction *aTitlebarTweakChk;
	TsCheckBox *InactiveFrmNewMsgCheckBox;
	TsButton *UnsentMsgEraseButton;
	TsButton *ClosedTabsEraseButton;
	TsButton *SessionRememberEraseButton;
	TAction *aNewMsgChk;
	TsCheckBox *InactiveTabsNewMsgCheckBox;
	TsCheckBox *ManualRestoreTabsSessionCheckBox;
	TsCheckBox *StayOnTopCheckBox;
	TsGroupBox *ClearCacheGroupBox;
	TAction *aOtherChk;
	TsCheckBox *InactiveNotiferNewMsgCheckBox;
	TsCheckBox *RestoreLastMsgClosedTabsCheckBox;
	TsCheckBox *ShowTimeClosedTabsCheckBox;
	TsCheckBox *FastClearClosedTabsCheckBox;
	TsCheckBox *FastClearUnsentMsgCheckBox;
	TsCheckBox *QuickQuoteCheckBox;
	TsCheckBox *ChatStateNotiferNewMsgCheckBox;
	TsCheckBox *AntiSpimCheckBox;
	TsCheckBox *OnlyConversationTabsCheckBox;
	TsCheckBox *HideToolBarCheckBox;
	TsCheckBox *OpenClipTabsCheckBox;
	TsButton *ClipTabsEraseButton;
	TsCheckBox *InactiveClipTabsCheckBox;
	TAction *aClipTabsChk;
	TsButton *MiniAvatarsEraseButton;
	TFileListBox *FileListBox;
	TsCheckBox *CollapseImagesCheckBox;
	TsCheckBox *CounterClipTabsCheckBox;
	TsSkinManager *sSkinManager;
	TsCheckBox *MinimizeRestoreCheckBox;
	THotKey *MinimizeRestoreHotKey;
	TsCheckBox *UnCloseTabSPMouseCheckBox;
	TsCheckBox *UnCloseTabLPMouseCheckBox;
	TsLabel *UnCloseMouseLabel;
	TAction *aSideSlideChk;
	TsCheckBox *HideTabCloseButtonCheckBox;
	TsCheckBox *CloseBy2xLPMCheckBox;
	TsCheckBox *OffCoreInactiveTabsNewMsgCheckBox;
	TsComboBox *TweakFrmMainTitlebarModeExComboBox;
	TsGroupBox *FrmSendTimeGroupBox;
	TsSpinEdit *FrmSendSlideInTimeSpinEdit;
	TsSpinEdit *FrmSendSlideOutTimeSpinEdit;
	TsGroupBox *FrmMainTimeGroupBox;
	TsSpinEdit *FrmMainSlideInTimeSpinEdit;
	TsSpinEdit *FrmMainSlideOutTimeSpinEdit;
	TsLabel *TweakFrmMainTitlebarModeExLabel;
	TsGroupBox *SlideFrmSendGroupBox;
	TsGroupBox *SlideFrmMainGroupBox;
	TsSpinEdit *FrmSendSlideInDelaySpinEdit;
	TsSpinEdit *FrmMainSlideInDelaySpinEdit;
	TsSpinEdit *FrmSendSlideOutDelaySpinEdit;
	TsSpinEdit *FrmMainSlideOutDelaySpinEdit;
	TsPageControl *sPageControl;
	TsTabSheet *ClosedTabsTabSheet;
	TsTabSheet *UnsentMsgTabSheet;
	TsTabSheet *TabsSwitchingTabSheet;
	TsTabSheet *SessionRememberTabSheet;
	TsTabSheet *NewMsgTabSheet;
	TsTabSheet *TitlebarTabSheet;
	TsTabSheet *ClipTabsTabSheet;
	TsTabSheet *SideSlideTabSheet;
	TsTabSheet *OtherTabSheet;
	TsGroupBox *RememberClosedTabsGroupBox;
	TsGroupBox *FastAccessClosedTabsGroupBox;
	TsGroupBox *UnCloseTabHotKeyGroupBox;
	TsGroupBox *OtherClosedTabGroupBox;
	TsGroupBox *RememberUnsentMsgGroupBox;
	TsGroupBox *InfoUnsentMsgGroupBox;
	TsGroupBox *FastAccessUnsentMsgGroupBox;
	TsGroupBox *SwitchToNewMsgGroupBox;
	TsGroupBox *TabsHotKeysGroupBox;
	TsGroupBox *RestoreTabsSessionGroupBox;
	TsGroupBox *TweakFrmSendTitlebarGroupBox;
	TsGroupBox *TweakFrmMainTitlebarGroupBox;
	TsCheckBox *SideSlideFullScreenModeCheckBox;
	TsCheckBox *SideSlideCtrlAndMousBlockCheckBox;
	TsComboBox *CollapseImagesModeComboBox;
	TsSpinEdit *CloudTimeOutSpinEdit;
	TsGroupBox *CloudSettingsGroupBox;
	TsComboBox *CloudTickModeComboBox;
	TsCheckBox *HideScrollTabButtonsCheckBox;
	TsCheckBox *NewMgsHoyKeyCheckBox;
	TsCheckBox *TaskbarPenCheckBox;
	TsCheckBox *SearchOnListCheckBox;
	TsCheckBox *ExcludeClipTabsFromTabSwitchingCheckBox;
	TsCheckBox *ExcludeClipTabsFromSwitchToNewMsgCheckBox;
	TsCheckBox *ExcludeClipTabsFromTabsHotKeysCheckBox;
	TsCheckBox *ShortenLinksCheckBox;
	TsRadioButton *TweakFrmSendTitlebarMode4RadioButton;
	TsCheckBox *ChangeTabAfterSlideInCheckBox;
	TsCheckBox *NoMiniAvatarsClipTabsCheckBox;
	TsCheckBox *ChatGoneNotiferNewMsgCheckBox;
	TsCheckBox *TurnOffModalCheckBox;
	TsSpeedButton *SideSlideFullScreenModeExceptionsButton;
	TsSkinProvider *sSkinProvider;
	TsAlphaImageList *sAlphaImageList;
	TIdHTTP *IdHTTP;
	TIdThreadComponent *GetYouTubeTitleThread;
	TTimer *RefreshTimer;
	TsComboBox *ShortenLinksModeComboBox;
	TsCheckBox *KeyboardFlasherCheckBox;
	TsComboBox *KeyboardFlasherModeComboBox;
	TsMemo *FileMemo;
	TsCheckBox *UnCloseTab2xLPMouseCheckBox;
	TsCheckBox *ChatGoneCloudNotiferNewMsgCheckBox;
	TsCheckBox *ChatGoneSoundNotiferNewMsgCheckBox;
	TsTabSheet *FavouritesTabsTabSheet;
	TsCheckBox *FavouritesTabsHotKeysCheckBox;
	TAction *aFavouritesTabsChk;
	TsGroupBox *FastAccessFavouritesTabsGroupBox;
	TsCheckBox *FrmMainFastAccessFavouritesTabsCheckBox;
	TsCheckBox *FrmSendFastAccessFavouritesTabsCheckBox;
	TsListView *FavouritesTabsListView;
	TAction *aReloadFavouritesTabs;
	TsSpeedButton *AddChatsFavouriteTabSpeedButton;
	TsSpeedButton *MoveUpFavouriteTabSpeedButton;
	TsSpeedButton *MoveDownFavouriteTabSpeedButton;
	TsSpeedButton *RemoveFavouriteTabSpeedButton;
	TsAlphaImageList *FavouritesTabsAlphaImageList;
	TsGroupBox *ClipTabsGroupBox;
	TsGroupBox *FavouritesTabsGroupBox;
	TsComboBox *FrmMainEdgeComboBox;
	TsLabel *FrmMainEdgeLabel;
	TsLabel *FrmSendEdgeLabel;
	TsComboBox *FrmSendEdgeComboBox;
	TsLabel *CloudTickModeLabel;
	TsEdit *DateFormatEdit;
	TsCheckBox *ChatGoneFrmSendNotiferNewMsgCheckBox;
	TsCheckBox *ChatGoneSaveInArchiveCheckBox;
	TsCheckBox *SaveClosedInfoInArchiveCheckBox;
	void __fastcall UnsentMsgTrayIconClick(TObject *Sender);
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall aLoadSettingsExecute(TObject *Sender);
	void __fastcall aUnsentMsgChkExecute(TObject *Sender);
	void __fastcall aSaveSettingsExecute(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall aTabsSwitchingChkExecute(TObject *Sender);
	void __fastcall aSaveSettingsWExecute(TObject *Sender);
	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall aClosedTabsChkExecute(TObject *Sender);
	void __fastcall aSessionRememberChkExecute(TObject *Sender);
	void __fastcall aTitlebarTweakChkExecute(TObject *Sender);
	void __fastcall UnsentMsgEraseButtonClick(TObject *Sender);
	void __fastcall ClosedTabsEraseButtonClick(TObject *Sender);
	void __fastcall SessionRememberEraseButtonClick(TObject *Sender);
	void __fastcall aNewMsgChkExecute(TObject *Sender);
	void __fastcall aOtherChkExecute(TObject *Sender);
	void __fastcall ClipTabsEraseButtonClick(TObject *Sender);
	void __fastcall aClipTabsChkExecute(TObject *Sender);
	void __fastcall ConvertImage(UnicodeString Old, UnicodeString New);
	void __fastcall MiniAvatarsEraseButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall aSideSlideChkExecute(TObject *Sender);
	void __fastcall OtherTabSheetShow(TObject *Sender);
	void __fastcall PayPalImageClick(TObject *Sender);
	void __fastcall NewMsgTabSheetShow(TObject *Sender);
	void __fastcall SideSlideFullScreenModeExceptionsButtonClick(TObject *Sender);
	void __fastcall GetYouTubeTitleThreadRun(TIdThreadComponent *Sender);
	void __fastcall RefreshTimerTimer(TObject *Sender);
	void __fastcall sSkinManagerSysDlgInit(TacSysDlgData DlgData, bool &AllowSkinning);
	void __fastcall aFavouritesTabsChkExecute(TObject *Sender);
	void __fastcall aReloadFavouritesTabsExecute(TObject *Sender);
	void __fastcall FavouritesTabsListViewKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FavouritesTabsListViewSelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall FavouritesTabsListViewExit(TObject *Sender);
	void __fastcall RemoveFavouriteTabSpeedButtonClick(TObject *Sender);
	void __fastcall MoveUpFavouriteTabSpeedButtonClick(TObject *Sender);
	void __fastcall MoveDownFavouriteTabSpeedButtonClick(TObject *Sender);
	void __fastcall AddChatsFavouriteTabSpeedButtonClick(TObject *Sender);
	void __fastcall DateFormatEditChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	UnicodeString XML;
	__fastcall TSettingsForm(TComponent* Owner);
	void __fastcall WMTransparency(TMessage &Message);
	void __fastcall WMHotKey(TMessage &Message);
	UnicodeString __fastcall IdHTTPGet(UnicodeString URL);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_ALPHAWINDOWS,TMessage,WMTransparency);
	MESSAGE_HANDLER(WM_HOTKEY,TMessage,WMHotKey);
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif

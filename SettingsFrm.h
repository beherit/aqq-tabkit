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

//---------------------------------------------------------------------------
#ifndef SettingsFrmH
#define SettingsFrmH
#define WM_ALPHAWINDOWS (WM_USER + 666)
//---------------------------------------------------------------------------
#include "acPNG.hpp"
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sCheckBox.hpp"
#include "sComboBox.hpp"
#include "sEdit.hpp"
#include "sGroupBox.hpp"
#include "sLabel.hpp"
#include "sPageControl.hpp"
#include "sRadioButton.hpp"
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include "sSpeedButton.hpp"
#include "sSpinEdit.hpp"
#include <IdBaseComponent.hpp>
#include <IdCoder.hpp>
#include <IdCoder3to4.hpp>
#include <IdCoderMIME.hpp>
#include <IdThreadComponent.hpp>
#include <System.Actions.hpp>
#include <System.Classes.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.FileCtrl.hpp>
#include <Vcl.StdCtrls.hpp>
#include "acAlphaImageList.hpp"
#include <Vcl.ImgList.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <IdAntiFreezeBase.hpp>
#include <Vcl.IdAntiFreeze.hpp>
//---------------------------------------------------------------------------
class TSettingsForm : public TForm
{
__published:	// IDE-managed Components
	TsBevel *Bevel;
	TsButton *SaveButton;
	TsButton *CancelButton;
	TsCheckBox *RememberUnsentMsgCheckBox;
	TsCheckBox *InfoUnsentMsgCheckBox;
	TsCheckBox *FastAccessUnsentMsgCheckBox;
	TsCheckBox *FrmSendUnsentMsgCheckBox;
	TsCheckBox *FrmMainUnsentMsgCheckBox;
	TsCheckBox *CloudUnsentMsgCheckBox;
	TsCheckBox *TrayUnsentMsgCheckBox;
	TsCheckBox *SwitchToNewMsgCheckBox;
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
	TsCheckBox *RememberClosedTabsCheckBox;
	TsCheckBox *TabsHotKeysCheckBox;
	TsRadioButton *TabsHotKeysMode1RadioButton;
	TsRadioButton *TabsHotKeysMode2RadioButton;
	TsButton *OkButton;
	TAction *aSaveSettingsW;
	TsCheckBox *FastAccessClosedTabsCheckBox;
	TsCheckBox *FrmMainClosedTabsCheckBox;
	TsCheckBox *FrmSendClosedTabsCheckBox;
	TsCheckBox *UnCloseTabHotKeyCheckBox;
	TsRadioButton *UnCloseTabHotKeyMode1RadioButton;
	TsRadioButton *UnCloseTabHotKeyMode2RadioButton;
	THotKey *UnCloseTabHotKeyInput;
	TAction *aClosedTabsChk;
	TsSpinEdit *CountClosedTabsSpinEdit;
	TsCheckBox *RestoreTabsSessionCheckBox;
	TAction *aSessionRememberChk;
	TsCheckBox *RestoreMsgSessionCheckBox;
	TsLabel *RestoreSessionLabel;
	TsCheckBox *TweakFrmSendTitlebarCheckBox;
	TsRadioButton *TweakFrmSendTitlebarMode1RadioButton;
	TsRadioButton *TweakFrmSendTitlebarMode2RadioButton;
	TsRadioButton *TweakFrmSendTitlebarMode3RadioButton;
	TsCheckBox *TweakFrmMainTitlebarCheckBox;
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
	TsCheckBox *EmuTabsWCheckBox;
	TsCheckBox *ShowTimeClosedTabsCheckBox;
	TsCheckBox *FastClearClosedTabsCheckBox;
	TsCheckBox *FastClearUnsentMsgCheckBox;
	TsCheckBox *QuickQuoteCheckBox;
	TsCheckBox *ChatStateNotiferNewMsgCheckBox;
	TsCheckBox *AntiSpimCheckBox;
	TsCheckBox *OnlyConversationTabsCheckBox;
	TsCheckBox *HideToolBarCheckBox;
	TIdDecoderMIME *IdDecoderMIME;
	TsCheckBox *OpenClipTabsCheckBox;
	TsButton *ClipTabsEraseButton;
	TsCheckBox *InactiveClipTabsCheckBox;
	TAction *aClipTabsChk;
	TsButton *MiniAvatarsEraseButton;
	TFileListBox *FileListBox;
	TsCheckBox *CollapseImagesCheckBox;
	TsCheckBox *CounterClipTabsCheckBox;
	TsSkinManager *sSkinManager;
	TsLabel *ItemsCountClosedTabsLabel;
	TsLabel *CountClosedTabsLabel;
	TsCheckBox *MinimizeRestoreCheckBox;
	THotKey *MinimizeRestoreHotKey;
	TsCheckBox *UnCloseTabSPMouseCheckBox;
	TsCheckBox *UnCloseTabLPMouseCheckBox;
	TsLabel *UnCloseMouseLabel;
	TsCheckBox *SlideFrmSendCheckBox;
	TsCheckBox *SlideFrmMainCheckBox;
	TsGroupBox *FrmSendEdgeGroupBox;
	TsRadioButton *FrmSendEdgeLeftRadioButton;
	TsRadioButton *FrmSendEdgeRightRadioButton;
	TsGroupBox *FrmMainEdgeGroupBox;
	TsRadioButton *FrmMainEdgeLeftRadioButton;
	TsRadioButton *FrmMainEdgeRightRadioButton;
	TsGroupBox *FrmSendHideGroupBox;
	TsRadioButton *FrmSendHideFocusRadioButton;
	TsRadioButton *FrmSendHideAppFocusRadioButton;
	TsRadioButton *FrmSendHideCursorRadioButton;
	TsGroupBox *FrmMainHideGroupBox;
	TsRadioButton *FrmMainHideFocusRadioButton;
	TsRadioButton *FrmMainHideAppFocusRadioButton;
	TsRadioButton *FrmMainHideCursorRadioButton;
	TAction *aSideSlideChk;
	TsCheckBox *HideTabCloseButtonCheckBox;
	TsCheckBox *CloseBy2xLPMCheckBox;
	TsCheckBox *OffCoreInactiveTabsNewMsgCheckBox;
	TsComboBox *TweakFrmMainTitlebarModeExComboBox;
	TsGroupBox *FrmSendTimeGroupBox;
	TsSpinEdit *FrmSendSlideInTimeSpinEdit;
	TsSpinEdit *FrmSendSlideOutTimeSpinEdit;
	TsCheckBox *SlideInAtNewMsgCheckBox;
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
	TsTabSheet *DefaultTabSheet;
	TsGroupBox *ClosedTabGroupBox;
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
	TImage *PayPalImage;
	TsLabelFX *VersionLabel;
	TImage *AvatarImage;
	TsLabel *AuthorLabel;
	TsLabel *EmailLabel;
	TsLabel *JabberLabel;
	TsLabel *URLLabel;
	TsLabel *AuthorLabel2;
	TsWebLabel *EmailWebLabel;
	TsWebLabel *JabberWebLabel;
	TsWebLabel *URLWebLabel;
	TImage *BugImage;
	TImage *ForumImage;
	TsWebLabel *ForumWebLabel;
	TsWebLabel *BugWebLabel;
	TsLabelFX *DonateLabelFX;
	TsLabel *DonateLabel;
	TsCheckBox *SideSlideCtrlAndMousBlockCheckBox;
	TsComboBox *CollapseImagesModeComboBox;
	TsSpinEdit *CloudTimeOutSpinEdit;
	TsGroupBox *CloudSettingsGroupBox;
	TsComboBox *CloudTickModeComboBox;
	TsWebLabel *OtherPaymentsWebLabel;
	TsRadioButton *FrmMainEdgeBottomRadioButton;
	TsRadioButton *FrmMainEdgeTopRadioButton;
	TsRadioButton *FrmSendEdgeTopRadioButton;
	TsRadioButton *FrmSendEdgeBottomRadioButton;
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
	TImage *StarImage;
	TsWebLabel *StarWebLabel;
	TsCheckBox *NoMiniAvatarsClipTabsCheckBox;
	TIdThreadComponent *PrepareXMLThread;
	TsCheckBox *ChatGoneNotiferNewMsgCheckBox;
	TsCheckBox *TurnOffModalCheckBox;
	TsSpeedButton *SideSlideFullScreenModeExceptionsButton;
	TsSkinProvider *sSkinProvider;
	TsAlphaImageList *sAlphaImageList;
	TIdHTTP *IdHTTP;
	TIdThreadComponent *GetYouTubeTitleThread;
	TTimer *RefreshTimer;
	void __fastcall UnsentMsgTrayIconClick(TObject *Sender);
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
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
	void __fastcall PrepareXMLThreadRun(TIdThreadComponent *Sender);
	void __fastcall SideSlideFullScreenModeExceptionsButtonClick(TObject *Sender);
	void __fastcall GetYouTubeTitleThreadRun(TIdThreadComponent *Sender);
	void __fastcall RefreshTimerTimer(TObject *Sender);
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

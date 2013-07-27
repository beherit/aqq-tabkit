//---------------------------------------------------------------------------
#ifndef SettingsFrmH
#define SettingsFrmH
//---------------------------------------------------------------------------
#include "IdBaseComponent.hpp"
#include "IdCoder.hpp"
#include "IdCoder3to4.hpp"
#include "IdCoderMIME.hpp"
#include "LMDPNGImage.hpp"
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
#include "sSpinEdit.hpp"
#include <ActnList.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <FileCtrl.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
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
	TCategoryPanelGroup *CategoryPanelGroup;
	TCategoryPanel *ClosedTabsCategoryPanel;
	TCategoryPanel *UnsentMsgCategoryPanel;
	TCategoryPanel *TabsSwitchingCategoryPanel;
	TCategoryPanel *SessionRememberCategoryPanel;
	TCategoryPanel *TitlebarCategoryPanel;
	TCategoryPanel *NewMsgCategoryPanel;
	TCategoryPanel *OtherCategoryPanel;
	TsCheckBox *EmuTabsWCheckBox;
	TsCheckBox *ShowTimeClosedTabsCheckBox;
	TsCheckBox *FastClearClosedTabsCheckBox;
	TsCheckBox *FastClearUnsentMsgCheckBox;
	TsCheckBox *QuickQuoteCheckBox;
	TsCheckBox *ChatStateNotiferNewMsgCheckBox;
	TsCheckBox *AntiSpimCheckBox;
	TsCheckBox *OnlyConversationTabsCheckBox;
	TsCheckBox *HideStatusBarCheckBox;
	TsCheckBox *HideToolBarCheckBox;
	TIdDecoderMIME *IdDecoderMIME;
	TCategoryPanel *ClipTabsCategoryPanel;
	TsCheckBox *OpenClipTabsCheckBox;
	TsButton *ClipTabsEraseButton;
	TsCheckBox *InactiveClipTabsCheckBox;
	TAction *aClipTabsChk;
	TsButton *MiniAvatarsEraseButton;
	TFileListBox *FileListBox;
	TsCheckBox *CollapseImagesCheckBox;
	TsCheckBox *CounterClipTabsCheckBox;
	TsSkinManager *sSkinManager;
	TsSkinProvider *sSkinProvider;
	TsLabel *ItemsCountClosedTabsLabel;
	TsLabel *CountClosedTabsLabel;
	TsCheckBox *MinimizeRestoreCheckBox;
	THotKey *MinimizeRestoreHotKey;
	TsCheckBox *UnCloseTabSPMouseCheckBox;
	TsCheckBox *UnCloseTabLPMouseCheckBox;
	TsLabel *UnCloseMouseLabel;
	TCategoryPanel *SideSlideCategoryPanel;
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
	TAction *aRefreshPanels;
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
	TTimer *RefreshTimer;
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
	TsCheckBox *HideTabListButtonCheckBox;
	TsCheckBox *HideScrollTabButtonsCheckBox;
	TsCheckBox *NewMgsHoyKeyCheckBox;
	TsCheckBox *TaskbarPenCheckBox;
	TsCheckBox *SearchOnListCheckBox;
	TsCheckBox *ExcludeClipTabsFromTabSwitchingCheckBox;
	TsCheckBox *ExcludeClipTabsFromSwitchToNewMsgCheckBox;
	TsCheckBox *ExcludeClipTabsFromTabsHotKeysCheckBox;
	TsCheckBox *ShortenLinksCheckBox;
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
	void __fastcall OtherCategoryPanelExpand(TObject *Sender);
	void __fastcall ClipTabsEraseButtonClick(TObject *Sender);
	void __fastcall aClipTabsChkExecute(TObject *Sender);
	void __fastcall ConvertImage(UnicodeString Old, UnicodeString New);
	void __fastcall MiniAvatarsEraseButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall aSideSlideChkExecute(TObject *Sender);
	void __fastcall ClipTabsCategoryPanelCollapse(TObject *Sender);
	void __fastcall ClipTabsCategoryPanelExpand(TObject *Sender);
	void __fastcall ClosedTabsCategoryPanelCollapse(TObject *Sender);
	void __fastcall ClosedTabsCategoryPanelExpand(TObject *Sender);
	void __fastcall NewMsgCategoryPanelCollapse(TObject *Sender);
	void __fastcall NewMsgCategoryPanelExpand(TObject *Sender);
	void __fastcall OtherCategoryPanelCollapse(TObject *Sender);
	void __fastcall SessionRememberCategoryPanelCollapse(TObject *Sender);
	void __fastcall SessionRememberCategoryPanelExpand(TObject *Sender);
	void __fastcall SideSlideCategoryPanelCollapse(TObject *Sender);
	void __fastcall SideSlideCategoryPanelExpand(TObject *Sender);
	void __fastcall TabsSwitchingCategoryPanelCollapse(TObject *Sender);
	void __fastcall TabsSwitchingCategoryPanelExpand(TObject *Sender);
	void __fastcall TitlebarCategoryPanelCollapse(TObject *Sender);
	void __fastcall TitlebarCategoryPanelExpand(TObject *Sender);
	void __fastcall UnsentMsgCategoryPanelCollapse(TObject *Sender);
	void __fastcall UnsentMsgCategoryPanelExpand(TObject *Sender);
	void __fastcall aRefreshPanelsExecute(TObject *Sender);
	void __fastcall RefreshTimerTimer(TObject *Sender);
	void __fastcall OtherTabSheetShow(TObject *Sender);
	void __fastcall PayPalImageClick(TObject *Sender);
	void __fastcall NewMsgTabSheetShow(TObject *Sender);

private:	// User declarations
	//Konieczne do inicjalizacji GDIPlus
	//Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	//ULONG_PTR gdiplusToken;
public:		// User declarations
	__fastcall TSettingsForm(TComponent* Owner);
	void WMHotKey(TMessage& Message);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_HOTKEY,TMessage,WMHotKey);
	END_MESSAGE_MAP(TComponent)
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif

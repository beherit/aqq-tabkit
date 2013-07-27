//---------------------------------------------------------------------------
#ifndef SettingsFrmH
#define SettingsFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ActnList.hpp>
#include "IdBaseComponent.hpp"
#include "IdCoder.hpp"
#include "IdCoder3to4.hpp"
#include "IdCoderMIME.hpp"
#include <FileCtrl.hpp>
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sLabel.hpp"
#include "sGroupBox.hpp"
#include "sPanel.hpp"
#include "sCheckBox.hpp"
#include "sSpinEdit.hpp"
#include "sEdit.hpp"
#include "sRadioButton.hpp"
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
	TRadioButton *SwitchToNewMsgMode1RadioButton;
	TRadioButton *SwitchToNewMsgMode2RadioButton;
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
	TRadioButton *TabsHotKeysMode1RadioButton;
	TRadioButton *TabsHotKeysMode2RadioButton;
	TPanel *SwitchToNewMsgModePanel;
	TPanel *TabsHotKeysModePanel;
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
	TRadioButton *TweakFrmSendTitlebarMode1RadioButton;
	TRadioButton *TweakFrmSendTitlebarMode2RadioButton;
	TRadioButton *TweakFrmSendTitlebarMode3RadioButton;
	TsCheckBox *TweakFrmMainTitlebarCheckBox;
	TRadioButton *TweakFrmMainTitlebarMode1RadioButton;
	TRadioButton *TweakFrmMainTitlebarMode2RadioButton;
	TEdit *TweakFrmMainTitlebarMode2Edit;
	TPanel *TweakFrmSendTitlebarPanel;
	TPanel *TweakFrmMainTitlebarPanel;
	TsSpinEdit *ItemsCountClosedTabsSpinEdit;
	TAction *aTitlebarTweakChk;
	TsCheckBox *InactiveFrmNewMsgCheckBox;
	TButton *UnsentMsgEraseButton;
	TButton *ClosedTabsEraseButton;
	TButton *SessionRememberEraseButton;
	TAction *aNewMsgChk;
	TsCheckBox *InactiveTabsNewMsgCheckBox;
	TsCheckBox *ManualRestoreTabsSessionCheckBox;
	TsCheckBox *StayOnTopCheckBox;
	TGroupBox *ClearCacheGroupBox;
	TAction *aClosedTabs;
	TAction *aOtherChk;
	TsCheckBox *InactiveNotiferNewMsgCheckBox;
	TsCheckBox *RestoreLastMsgClosedTabsCheckBox;
	TCategoryPanelGroup *CategoryPanelGroup;
	TCategoryPanel *ClosedCategoryPanel;
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
	TButton *ClipTabsEraseButton;
	TsCheckBox *InactiveClipTabsCheckBox;
	TAction *aClipTabsChk;
	TImage *Image;
	TButton *MiniAvatarsEraseButton;
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
	TsCheckBox *EnableBlockFrmSendResizeCheckBox;
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
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall ClipTabsEraseButtonClick(TObject *Sender);
	void __fastcall aClipTabsChkExecute(TObject *Sender);
	void __fastcall ConvertImage(UnicodeString Old, UnicodeString New);
	void __fastcall MiniAvatarsEraseButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
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

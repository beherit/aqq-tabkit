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
#include "cspin.h"
#include "IdBaseComponent.hpp"
#include "IdCoder.hpp"
#include "IdCoder3to4.hpp"
#include "IdCoderMIME.hpp"
#include <FileCtrl.hpp>
//---------------------------------------------------------------------------
class TSettingsForm : public TForm
{
__published:	// IDE-managed Components
	TBevel *Bevel;
	TButton *SaveButton;
	TButton *CancelButton;
	TCheckBox *RememberUnsentMsgCheckBox;
	TCheckBox *InfoUnsentMsgCheckBox;
	TCheckBox *FastAccessUnsentMsgCheckBox;
	TCheckBox *FrmSendUnsentMsgCheckBox;
	TCheckBox *FrmMainUnsentMsgCheckBox;
	TCheckBox *CloudUnsentMsgCheckBox;
	TCheckBox *TrayUnsentMsgCheckBox;
	TCheckBox *SwitchToNewMsgCheckBox;
	TRadioButton *SwitchToNewMsgMode1RadioButton;
	TRadioButton *SwitchToNewMsgMode2RadioButton;
	TTrayIcon *UnsentMsgTrayIcon;
	TCheckBox *DetailedCloudUnsentMsgCheckBox;
	TActionList *ActionList;
	TAction *aExit;
	TAction *aLoadSettings;
	TAction *aSaveSettings;
	TAction *aUnsentMsgChk;
	TAction *aTabsSwitchingChk;
	TCheckBox *RememberClosedTabsCheckBox;
	TCheckBox *TabsHotKeysCheckBox;
	TRadioButton *TabsHotKeysMode1RadioButton;
	TRadioButton *TabsHotKeysMode2RadioButton;
	TPanel *SwitchToNewMsgModePanel;
	TPanel *TabsHotKeysModePanel;
	TButton *OkButton;
	TAction *aSaveSettingsW;
	TCheckBox *FastAccessClosedTabsCheckBox;
	TCheckBox *FrmMainClosedTabsCheckBox;
	TCheckBox *FrmSendClosedTabsCheckBox;
	TCheckBox *UnCloseTabHotKeyCheckBox;
	TRadioButton *UnCloseTabHotKeyMode1RadioButton;
	TRadioButton *UnCloseTabHotKeyMode2RadioButton;
	THotKey *UnCloseTabHotKeyInput;
	TAction *aClosedTabsChk;
	TLabel *CountClosedTabsLabel;
	TCSpinEdit *CountClosedTabsCSpinEdit;
	TCheckBox *RestoreTabsSessionCheckBox;
	TAction *aSessionRememberChk;
	TCheckBox *RestoreMsgSessionCheckBox;
	TLabel *RestoreSessionLabel;
	TCheckBox *TweakFrmSendTitlebarCheckBox;
	TRadioButton *TweakFrmSendTitlebarMode1RadioButton;
	TRadioButton *TweakFrmSendTitlebarMode2RadioButton;
	TRadioButton *TweakFrmSendTitlebarMode3RadioButton;
	TCheckBox *TweakFrmMainTitlebarCheckBox;
	TRadioButton *TweakFrmMainTitlebarMode1RadioButton;
	TRadioButton *TweakFrmMainTitlebarMode2RadioButton;
	TEdit *TweakFrmMainTitlebarMode2Edit;
	TPanel *TweakFrmSendTitlebarPanel;
	TPanel *TweakFrmMainTitlebarPanel;
	TLabel *ItemsCountClosedTabsLabel;
	TCSpinEdit *ItemsCountClosedTabsCSpinEdit;
	TAction *aTitlebarTweakChk;
	TCheckBox *InactiveFrmNewMsgCheckBox;
	TButton *UnsentMsgEraseButton;
	TButton *ClosedTabsEraseButton;
	TButton *SessionRememberEraseButton;
	TAction *aNewMsgChk;
	TCheckBox *InactiveTabsNewMsgCheckBox;
	TCheckBox *ManualRestoreTabsSessionCheckBox;
	TCheckBox *StayOnTopCheckBox;
	TGroupBox *ClearCacheGroupBox;
	TAction *aClosedTabs;
	TAction *aOtherChk;
	TCheckBox *InactiveNotiferNewMsgCheckBox;
	TCheckBox *RestoreLastMsgClosedTabsCheckBox;
	TLabel *TabsSwitchingLabel;
	TCategoryPanelGroup *CategoryPanelGroup;
	TCategoryPanel *ClosedCategoryPanel;
	TCategoryPanel *UnsentMsgCategoryPanel;
	TCategoryPanel *TabsSwitchingCategoryPanel;
	TCategoryPanel *SessionRememberCategoryPanel;
	TCategoryPanel *TitlebarCategoryPanel;
	TCategoryPanel *NewMsgCategoryPanel;
	TCategoryPanel *OtherCategoryPanel;
	TPanel *ClosedPanel;
	TPanel *UnsentMsgPanel;
	TPanel *TabsSwitchingPanel;
	TPanel *SessionRememberPanel;
	TPanel *TitlebarPanel;
	TPanel *NewMsgPanel;
	TPanel *OtherPanel;
	TCheckBox *EmuTabsWCheckBox;
	TCheckBox *ShowTimeClosedTabsCheckBox;
	TCheckBox *FastClearClosedTabsCheckBox;
	TCheckBox *FastClearUnsentMsgCheckBox;
	TCheckBox *QuickQuoteCheckBox;
	TCheckBox *ChatStateNotiferNewMsgCheckBox;
	TCheckBox *AntiSpimCheckBox;
	TCheckBox *OnlyConversationTabsCheckBox;
	TCheckBox *HideStatusBarCheckBox;
	TCheckBox *HideToolBarCheckBox;
	TIdDecoderMIME *IdDecoderMIME;
	TCategoryPanel *ClipTabsCategoryPanel;
	TPanel *ClipTabsPanel;
	TCheckBox *OpenClipTabsCheckBox;
	TButton *ClipTabsEraseButton;
	TCheckBox *InactiveClipTabsCheckBox;
	TAction *aClipTabsChk;
	TImage *Image;
	TButton *MiniAvatarsEraseButton;
	TFileListBox *FileListBox;
	TCheckBox *CollapseImagesCheckBox;
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
private:	// User declarations
	//Konieczne do inicjalizacji GDIPlus
	//Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	//ULONG_PTR gdiplusToken;
public:		// User declarations
	__fastcall TSettingsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif

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
//---------------------------------------------------------------------------
class TSettingsForm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	TBevel *Bevel;
	TButton *SaveButton;
	TButton *CancelButton;
	TTabSheet *UnsentMsgTabSheet;
	TTabSheet *TabsSwitchingTabSheet;
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
	TTabSheet *ClosedTabsSheet;
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
	TTabSheet *SessionRememberTabSheet;
	TCheckBox *RestoreTabsSessionCheckBox;
	TAction *aSessionRememberChk;
	TCheckBox *RestoreMsgSessionCheckBox;
	TLabel *RestoreSessionLabel;
	TTabSheet *TitlebarTabSheet;
	TCheckBox *TweakFrmSendTitlebarCheckBox;
	TRadioButton *TweakFrmSendTitlebarMode1RadioButton;
	TRadioButton *TweakFrmSendTitlebarMode2RadioButton;
	TRadioButton *TweakFrmSendTitlebarMode3RadioButton;
	TCheckBox *TweakFrmMainTitlebarCheckBox;
	TRadioButton *TweakFrmMainTitlebarMode1RadioButton;
	TRadioButton *TweakFrmMainTitlebarMode2RadioButton;
	TEdit *TweakFrmMainTitlebarMode2Edit;
	TLabel *TitlebarTweakLabel;
	TPanel *TweakFrmSendTitlebarPanel;
	TPanel *TweakFrmMainTitlebarPanel;
	TLabel *ItemsCountClosedTabsLabel;
	TCSpinEdit *ItemsCountClosedTabsCSpinEdit;
	TAction *aTitlebarTweakChk;
	TTabSheet *NewMsgTabSheet;
	TTabSheet *OtherTabSheet;
	TCheckBox *InactiveFrmNewMsgCheckBox;
	TButton *UnsentMsgEraseButton;
	TLabel *Label1;
	TButton *ClosedTabsEraseButton;
	TButton *SessionRememberEraseButton;
	TAction *aNewMsgChk;
	TCheckBox *InactiveTabsNewMsgCheckBox;
	TCheckBox *ManualRestoreTabsSessionCheckBox;
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
	void __fastcall OtherTabSheetShow(TObject *Sender);
	void __fastcall UnsentMsgEraseButtonClick(TObject *Sender);
	void __fastcall ClosedTabsEraseButtonClick(TObject *Sender);
	void __fastcall SessionRememberEraseButtonClick(TObject *Sender);
	void __fastcall aNewMsgChkExecute(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TSettingsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif

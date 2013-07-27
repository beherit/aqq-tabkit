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
	void __fastcall UnsentMsgTrayIconClick(TObject *Sender);
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall aLoadSettingsExecute(TObject *Sender);
	void __fastcall aUnsentMsgChkExecute(TObject *Sender);
	void __fastcall RememberUnsentMsgCheckBoxClick(TObject *Sender);
	void __fastcall InfoUnsentMsgCheckBoxClick(TObject *Sender);
	void __fastcall CloudUnsentMsgCheckBoxClick(TObject *Sender);
	void __fastcall DetailedCloudUnsentMsgCheckBoxClick(TObject *Sender);
	void __fastcall TrayUnsentMsgCheckBoxClick(TObject *Sender);
	void __fastcall FastAccessUnsentMsgCheckBoxClick(TObject *Sender);
	void __fastcall FrmSendUnsentMsgCheckBoxClick(TObject *Sender);
	void __fastcall FrmMainUnsentMsgCheckBoxClick(TObject *Sender);
	void __fastcall aSaveSettingsExecute(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall aTabsSwitchingChkExecute(TObject *Sender);
	void __fastcall SwitchToNewMsgCheckBoxClick(TObject *Sender);
	void __fastcall SwitchToNewMsgMode1RadioButtonClick(TObject *Sender);
	void __fastcall SwitchToNewMsgMode2RadioButtonClick(TObject *Sender);
	void __fastcall TabsHotKeysCheckBoxClick(TObject *Sender);
	void __fastcall TabsHotKeysMode1RadioButtonClick(TObject *Sender);
	void __fastcall TabsHotKeysMode2RadioButtonClick(TObject *Sender);
	void __fastcall aSaveSettingsWExecute(TObject *Sender);
	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall aClosedTabsChkExecute(TObject *Sender);
	void __fastcall RememberClosedTabsCheckBoxClick(TObject *Sender);
	void __fastcall FastAccessClosedTabsCheckBoxClick(TObject *Sender);
	void __fastcall FrmMainClosedTabsCheckBoxClick(TObject *Sender);
	void __fastcall FrmSendClosedTabsCheckBoxClick(TObject *Sender);
	void __fastcall UnCloseTabHotKeyCheckBoxClick(TObject *Sender);
	void __fastcall UnCloseTabHotKeyMode1RadioButtonClick(TObject *Sender);
	void __fastcall UnCloseTabHotKeyMode2RadioButtonClick(TObject *Sender);
	void __fastcall UnCloseTabHotKeyInputChange(TObject *Sender);
	void __fastcall CountClosedTabsCSpinEditChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TSettingsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------

#ifndef SideSlideExceptionsFrmH
#define SideSlideExceptionsFrmH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sLabel.hpp"
#include "sListBox.hpp"
#include "sTabControl.hpp"
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TSideSlideExceptionsForm : public TForm
{
__published:	// IDE-managed Components
	TsBevel *Bevel;
	TsButton *SaveButton;
	TsTabControl *TabControl;
	TsLabel *InfoLabel;
	TsListBox *ProcessListBox;
	TsButton *AddButton;
	TsButton *DeleteButton;
	TsButton *CancelButton;
	TActionList *ActionList;
	TAction *aExit;
	TAction *aSaveSettings;
	TAction *aLoadSettings;
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall AddButtonClick(TObject *Sender);
	void __fastcall ProcessListBoxClick(TObject *Sender);
	void __fastcall DeleteButtonClick(TObject *Sender);
	void __fastcall aLoadSettingsExecute(TObject *Sender);
	void __fastcall aSaveSettingsExecute(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TSideSlideExceptionsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSideSlideExceptionsForm *SideSlideExceptionsForm;
//---------------------------------------------------------------------------
#endif

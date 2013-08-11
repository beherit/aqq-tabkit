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
#ifndef SideSlideExceptionsFrmH
#define SideSlideExceptionsFrmH
#define WM_ALPHAWINDOWS (WM_USER + 666)
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
#include "sSkinProvider.hpp"
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
	TsSkinProvider *sSkinProvider;
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
	bool SkinManagerEnabled;
	void __fastcall WMTransparency(TMessage &Message);
	BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_ALPHAWINDOWS,TMessage,WMTransparency);
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TSideSlideExceptionsForm *SideSlideExceptionsForm;
//---------------------------------------------------------------------------
#endif

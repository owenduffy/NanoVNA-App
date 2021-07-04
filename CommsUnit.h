
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#ifndef CommsUnitH
#define CommsUnitH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.WinXCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.WinXPanels.hpp>

#include <vector>
#include "types.h"

class TCommsForm : public TForm
{
__published:
	TMemo *Memo1;
	TButton *CloseButton;
	TButton *SaveButton;
	TSaveDialog *SaveDialog1;
	TButton *ClearButton;
	TEdit *CommandEdit;
	TLabel *Label1;
	TToggleSwitch *PauseCommsToggleSwitch;
	TToggleSwitch *ScrollToggleSwitch;
	TPopupMenu *CommandsButtonPopupMenu;
	TMenuItem *Clearcommand1;
	TMenuItem *Editcommand1;
	TMenuItem *Cancel1;
	TMenuItem *N1;
	TMenuItem *Rename1;
	TMenuItem *Copy1;
	TMenuItem *Paste1;
	TGridPanel *GridPanel1;
	TButton *CommandsButton1;
	TButton *CommandsButton2;
	TButton *CommandsButton3;
	TButton *CommandsButton4;
	TLabel *Label2;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall CloseButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall ClearButtonClick(TObject *Sender);
	void __fastcall CommandEditKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
			 TShiftState Shift);
	void __fastcall CommandEditChange(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall PauseCommsToggleSwitchClick(TObject *Sender);
	void __fastcall CommandEditExit(TObject *Sender);
	void __fastcall CommandEditEnter(TObject *Sender);
	void __fastcall Clearcommand1Click(TObject *Sender);
	void __fastcall Editcommand1Click(TObject *Sender);
	void __fastcall Copy1Click(TObject *Sender);
	void __fastcall Paste1Click(TObject *Sender);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall CommandsButtonClick(TObject *Sender);
	void __fastcall CommandsButtonDropDownClick(TObject *Sender);
	void __fastcall Cancel1Click(TObject *Sender);
	void __fastcall CommandsButtonPopupMenuPopup(TObject *Sender);
	void __fastcall FormMouseEnter(TObject *Sender);

private:
	bool m_initialised;

	int m_command_history_index;
	std::vector <String> m_command_history;

	std::vector <String> m_user_commands[VNA_MAX_USER_COMMANDS];

	std::vector <String> m_memo_contents;
	int m_commands_edit_index;

	TButton *m_popup_menu_button;

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);

	void __fastcall loadCommandHistory();
	void __fastcall saveCommandHistory();

	void __fastcall loadUserCommands();
	void __fastcall saveUserCommands();
	void __fastcall editUserCommand(const int index);

	TButton * __fastcall getPopupMenuButton();

protected:
	#pragma option push -vi-
	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);
	END_MESSAGE_MAP(TForm)
	#pragma option pop

public:
	__fastcall TCommsForm(TComponent* Owner);

	void __fastcall show();
	void __fastcall clear();
	void __fastcall addText(AnsiString prefix, AnsiString s);
	void __fastcall update();
};

extern PACKAGE TCommsForm *CommsForm;

#endif


#ifndef VNAUsartUnitH
#define VNAUsartUnitH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.WinXCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>

#include <vector>

#include "types.h"

class TVNAUsartCommsForm : public TForm
{
__published:	// IDE-managed Components
	TButton *CloseButton;
	TButton *SaveButton;
	TButton *ClearButton;
	TMemo *Memo1;
	TLabel *Label1;
	TEdit *CommandEdit;
	TSaveDialog *SaveDialog1;
	TComboBox *VNAUsartBaudrateComboBox;
	TToggleSwitch *ScrollToggleSwitch;
	TGridPanel *GridPanel1;
	TButton *CommandsButton1;
	TButton *CommandsButton2;
	TButton *CommandsButton3;
	TButton *CommandsButton4;
	TLabel *Label2;
	TPopupMenu *CommandsButtonPopupMenu;
	TMenuItem *Clearcommand1;
	TMenuItem *Editcommand1;
	TMenuItem *Copy1;
	TMenuItem *Paste1;
	TMenuItem *Rename1;
	TMenuItem *N1;
	TMenuItem *Cancel1;
	TComboBox *CommandEndsComboBox;
	TEdit *TimeoutEdit;
	TLabel *Label3;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall CloseButtonClick(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall ClearButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall CommandEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall CommandEditEnter(TObject *Sender);
	void __fastcall CommandEditExit(TObject *Sender);
	void __fastcall CommandEditChange(TObject *Sender);
	void __fastcall VNAUsartBaudrateComboBoxChange(TObject *Sender);
	void __fastcall CommandsButtonPopupMenuPopup(TObject *Sender);
	void __fastcall Clearcommand1Click(TObject *Sender);
	void __fastcall Editcommand1Click(TObject *Sender);
	void __fastcall Copy1Click(TObject *Sender);
	void __fastcall Paste1Click(TObject *Sender);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall Cancel1Click(TObject *Sender);
	void __fastcall CommandsButtonClick(TObject *Sender);
	void __fastcall CommandsButtonDropDownClick(TObject *Sender);
	void __fastcall CommandEndsComboBoxClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall TimeoutEditChange(TObject *Sender);
	void __fastcall FormMouseEnter(TObject *Sender);


private:	// User declarations
	bool m_initialised;

	int m_command_timeout_ms;

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

public:		// User declarations
	__fastcall TVNAUsartCommsForm(TComponent* Owner);

	void __fastcall show();
	void __fastcall clear();
	void __fastcall addText(AnsiString prefix, AnsiString s);
	void __fastcall setBaudrate(int baudrate);
	void __fastcall update();
};

extern PACKAGE TVNAUsartCommsForm *VNAUsartCommsForm;

#endif

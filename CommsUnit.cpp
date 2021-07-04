
// (c) OneOfEleven 2020
//
// This code can be used on terms of WTFPL Version 2 (http://www.wtfpl.net)

#include <vcl.h>
#include <Clipbrd.hpp>
#include <stdio.h>

#pragma hdrstop

#include "CommsUnit.h"
#include "Unit1.h"
#include "DataUnit.h"
#include "common.h"
#include "Settings.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TCommsForm *CommsForm = NULL;

__fastcall TCommsForm::TCommsForm(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TCommsForm::FormCreate(TObject *Sender)
{
	AnsiString s;

	Caption = Application->Title + " " + this->Caption;

	// stop flicker
	Memo1->ControlStyle = Memo1->ControlStyle << csOpaque;

	Memo1->Clear();

	SaveDialog1->InitialDir = ExtractFilePath(Application->ExeName);

	SaveButton->Enabled = false;

	CommandEdit->Text = "";

	m_command_history_index = 0;

	loadCommandHistory();
	loadUserCommands();

	m_commands_edit_index = -1;

	m_popup_menu_button = NULL;

	// move to the saved position
	this->Top    = settings.commsWindowPos.top;
	this->Left   = settings.commsWindowPos.left;
	this->Width  = settings.commsWindowPos.width;
	this->Height = settings.commsWindowPos.height;
}

void __fastcall TCommsForm::WMWindowPosChanging(TWMWindowPosChanging &msg)
{
	const int thresh = WINDOW_SNAP;

	if (msg.WindowPos->flags & SWP_STATECHANGED)
	{
		if (msg.WindowPos->flags & SWP_FRAMECHANGED)
		{
			if (msg.WindowPos->x < 0 && msg.WindowPos->y < 0)
			{	// Window state is about to change to MAXIMIZED
				if ((msg.WindowPos->flags & (SWP_SHOWWINDOW | SWP_NOACTIVATE)) == (SWP_SHOWWINDOW | SWP_NOACTIVATE))
				{	// about to minimize
					return;
				}
				else
				{	// about to maximize
					return;
				}
			}
		}
	}

	if (msg.WindowPos->hwnd != this->Handle || Screen == NULL)
		return;

	const int dtLeft   = Screen->DesktopRect.left;
	//const int dtRight  = Screen->DesktopRect.right;
	const int dtTop    = Screen->DesktopRect.top;
	const int dtBottom = Screen->DesktopRect.bottom;
	const int dtWidth  = Screen->DesktopRect.Width();
	const int dtHeight = Screen->DesktopRect.Height();

	//const int waLeft   = Screen->WorkAreaRect.left;
	//const int waRight  = Screen->WorkAreaRect.right;
	//const int waTop    = Screen->WorkAreaRect.top;
	//const int waBottom = Screen->WorkAreaRect.bottom;
	//const int waWidth  = Screen->WorkAreaRect.Width();
	//const int waHeight = Screen->WorkAreaRect.Height();

	int x = msg.WindowPos->x;
	int y = msg.WindowPos->y;
	int w = msg.WindowPos->cx;
	int h = msg.WindowPos->cy;

	for (int i = 0; i < Screen->MonitorCount; i++)
	{	// sticky screen edges
		const int mLeft   = Screen->Monitors[i]->WorkareaRect.left;
		const int mRight  = Screen->Monitors[i]->WorkareaRect.right;
		const int mTop    = Screen->Monitors[i]->WorkareaRect.top;
		const int mBottom = Screen->Monitors[i]->WorkareaRect.bottom;
		const int mWidth  = Screen->Monitors[i]->WorkareaRect.Width();
		const int mHeight = Screen->Monitors[i]->WorkareaRect.Height();

		if (ABS(x - mLeft) < thresh)
				  x = mLeft;			// stick left to left side
		else
		if (ABS((x + w) - mRight) < thresh)
					x = mRight - w;	// stick right to right side

		if (ABS(y - mTop) < thresh)
				  y = mTop;				// stick top to top side
		else
		if (ABS((y + h) - mBottom) < thresh)
					y = mBottom - h;	// stick bottom to bottm side

		// stick the right side to the right side of the screen if the left side is stuck to the left side of the screen
		if (x == mLeft)
			if ((w >= (mWidth - thresh)) && (w <= (mWidth + thresh)))
				w = mWidth;

		// stick the bottom to the bottom of the screen if the top is stuck to the top of the screen
		if (y == mTop)
			if ((h >= (mHeight - thresh)) && (h <= (mHeight + thresh)))
				h = mHeight;
	}
/*
	{	// sticky screen edges
		if (ABS(x - waLeft) < thresh)
			x = waLeft;			// stick left to left side
		else
		if (ABS((x + w) - waRight) < thresh)
			x = waRight - w;	// stick right to right side

		if (ABS(y - waTop) < thresh)
			y = waTop;			// stick top to top side
		else
		if (ABS((y + h) - waBottom) < thresh)
			y = waBottom - h;	// stick bottom to bottm side

		// stick the right side to the right side of the screen if the left side is stuck to the left side of the screen
		if (x == waLeft)
			if ((w >= (waWidth - thresh)) && (w <= (waWidth + thresh)))
				w = waWidth;

		// stick the bottom to the bottom of the screen if the top is stuck to the top of the screen
		if (y == waTop)
			if ((h >= (waHeight - thresh)) && (h <= (waHeight + thresh)))
				h = waHeight;
	}
*/
	// limit minimum size
	if (w < Constraints->MinWidth)
		 w = Constraints->MinWidth;
	if (h < Constraints->MinHeight)
		 h = Constraints->MinHeight;

	// limit maximum size
	if (w > Constraints->MaxWidth && Constraints->MaxWidth > Constraints->MinWidth)
		 w = Constraints->MaxWidth;
	if (h > Constraints->MaxHeight && Constraints->MaxHeight > Constraints->MinHeight)
		 h = Constraints->MaxHeight;

	// limit maximum size
	if (w > dtWidth)
		 w = dtWidth;
	if (h > dtHeight)
		 h = dtHeight;
/*
	if (Application->MainForm && this != Application->MainForm)
	{	// stick to our main form sides
		const TRect rect = Application->MainForm->BoundsRect;

		if (ABS(x - rect.left) < thresh)
			x = rect.left;			// stick to left to left side
		else
		if (ABS((x + w) - rect.left) < thresh)
			x = rect.left - w;	// stick right to left side
		else
		if (ABS(x - rect.right) < thresh)
			x = rect.right;		// stick to left to right side
		else
		if (ABS((x + w) - rect.right) < thresh)
			x = rect.right - w;	// stick to right to right side

		if (ABS(y - rect.top) < thresh)
			y = rect.top;			// stick top to top side
		else
		if (ABS((y + h) - rect.top) < thresh)
			y = rect.top - h;		// stick bottom to top side
		else
		if (ABS(y - rect.bottom) < thresh)
			y = rect.bottom;		// stick top to bottom side
		else
		if (ABS((y + h) - rect.bottom) < thresh)
			y = rect.bottom - h;	// stick bottom to bottom side
	}
*/
	// stop it completely leaving the desktop area
	if (x < (dtLeft - Width + (dtWidth / 15)))
		 x =  dtLeft - Width + (dtWidth / 15);
	if (x > (dtWidth - (Screen->Width / 15)))
		 x =  dtWidth - (Screen->Width / 15);
	if (y < dtTop)
		 y = dtTop;
	if (y > (dtBottom - (dtHeight / 10)))
		 y =  dtBottom - (dtHeight / 10);

	msg.WindowPos->x  = x;
	msg.WindowPos->y  = y;
	msg.WindowPos->cx = w;
	msg.WindowPos->cy = h;
}

void __fastcall TCommsForm::CloseButtonClick(TObject *Sender)
{
	if (!Memo1->ReadOnly)
	{  // being used to edit user commands
		m_commands_edit_index = -1;

		Memo1->Lines->BeginUpdate();
		Memo1->Clear();
		for (unsigned int i = 0; i < m_memo_contents.size(); i++)
			Memo1->Lines->Add(m_memo_contents[i]);
		m_memo_contents.resize(0);
		Memo1->ReadOnly = true;
		Memo1->Lines->EndUpdate();
		if (ScrollToggleSwitch->State == tssOff)
			Memo1->Lines->Add("");	// cause a scroll to end

		return;
	}

	this->Hide();
}

void __fastcall TCommsForm::FormShow(TObject *Sender)
{
	if (!m_initialised)
	{
		// move to the saved position
		this->Top    = settings.commsWindowPos.top;
		this->Left   = settings.commsWindowPos.left;
		this->Width  = settings.commsWindowPos.width;
		this->Height = settings.commsWindowPos.height;

		m_initialised = true;
	}

	if (Form1)
	{
		const String s = CommandEdit->Text.Trim();
		Form1->pauseComms(!s.IsEmpty() || PauseCommsToggleSwitch->State == tssOn);
	}

	update();
}

void __fastcall TCommsForm::show()
{
	update();

	this->Show();
	this->BringToFront();
//	this->Update();
}

void __fastcall TCommsForm::clear()
{
	Memo1->Clear();
	SaveButton->Enabled = !Memo1->ReadOnly || !Memo1->Text.IsEmpty();
}

void __fastcall TCommsForm::addText(AnsiString prefix, AnsiString s)
{
	AnsiString s2;

	if (!Memo1->ReadOnly)
		return;	// the memo is currently in use

	const bool scroll_to_end = ScrollToggleSwitch->State == tssOff;

	std::vector <AnsiString> lines;
	{
		for (int i = 1; i <= s.Length(); i++)
			if (s[i] == '\r')
				s = s.Delete(i, 1);

		while (!s.IsEmpty() && s[s.Length()] == '\n')
			s = s.Delete(s.Length(), 1);

		while (true)
		{
			const int pos = s.Pos('\n');
			if (pos <= 0)
			{
				lines.push_back(s);
				break;
			}
			s2 = s.SubString(1, pos - 1);
			s  = s.SubString(pos + 1, s.Length() - pos);
			lines.push_back(s2);
		}
	}
	if (lines.empty())
		return;

	if (!scroll_to_end)
		Memo1->Lines->BeginUpdate();

	// limit memory use
	if (Memo1->MaxLength > 0)
	{
		if (Memo1->Text.Length() >= Memo1->MaxLength)
		{
			if (scroll_to_end)
				Memo1->Lines->BeginUpdate();
			Memo1->Clear();
			if (scroll_to_end)
				Memo1->Lines->EndUpdate();
		}
	}
	else
	{
		if (Memo1->Lines->Count >= 2500)
		{
			if (scroll_to_end)
				Memo1->Lines->BeginUpdate();
			while (Memo1->Lines->Count > 1900)
				Memo1->Lines->Delete(0);
			if (scroll_to_end)
				Memo1->Lines->EndUpdate();
		}
	}

	for (unsigned int i = 0; i < lines.size(); i++)
		Memo1->Lines->Add(prefix + lines[i]);

	if (!scroll_to_end)
		Memo1->Lines->EndUpdate();
//	else
//	if (this->Showing)
//		Memo1->Update();

	SaveButton->Enabled = !Memo1->Text.IsEmpty();
}

void __fastcall TCommsForm::update()
{
	SaveButton->Enabled = !Memo1->Text.IsEmpty();
}

void __fastcall TCommsForm::SaveButtonClick(TObject *Sender)
{
	if (!Memo1->ReadOnly)
	{  // being used to edit user commands

		TButton *but = NULL;
		switch (m_commands_edit_index)
		{
			case 0: but = CommandsButton1; break;
			case 1: but = CommandsButton2; break;
			case 2: but = CommandsButton3; break;
			case 3: but = CommandsButton4; break;
			default: break;
		}

		if (but)
		{
			String hint;
			std::vector <String> cmds;

			for (int i = 0; i < Memo1->Lines->Count; i++)
			{
				const String s = Memo1->Lines->Strings[i].Trim();
				cmds.push_back(s);
				if (i < (Memo1->Lines->Count - 1))
					hint += s + "\r\n";
				else
					hint += s;
			}

			but->Hint = hint;

			cmds.insert(cmds.begin() + 0, but->Caption.Trim());	// command sequence name

			if (m_commands_edit_index >= 0 && m_commands_edit_index < VNA_MAX_USER_COMMANDS)
			{
				m_user_commands[m_commands_edit_index] = cmds;
				saveUserCommands();
			}
		}

		m_commands_edit_index = -1;

		Memo1->Lines->BeginUpdate();
		Memo1->Clear();
		for (unsigned int i = 0; i < m_memo_contents.size(); i++)
			Memo1->Lines->Add(m_memo_contents[i]);
		m_memo_contents.resize(0);
		Memo1->ReadOnly = true;
		Memo1->Lines->EndUpdate();
		if (ScrollToggleSwitch->State == tssOff)
			Memo1->Lines->Add("");	// cause a scroll to end

		return;
	}

	if (Memo1->Text.IsEmpty())
		return;

	AnsiString filename = data_unit.m_vna_data.name.LowerCase() + "_commands_" + FormatDateTime("yyyy-mm-dd_hh-nn-ss", Now()) + ".txt";

	filename = common.cleanFilename(filename, false, true);

	SaveDialog1->FileName = filename;

	Application->NormalizeTopMosts();
	const bool ok = SaveDialog1->Execute();
	Application->RestoreTopMosts();

	if (!ok)
		return;

	#if (__BORLANDC__ < 0x0600)
		FILE *fout = fopen(filename.c_str(), "wt");
	#else
		FILE *fout = NULL;
		int res = fopen_s(&fout, filename.c_str(), "wt");
		if (res != 0 || !fout)
		{
			if (fout)
				fclose(fout);
			fout = NULL;
		}
	#endif

	if (fout == NULL)
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"File not saved", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
		return;
	}

	for (int i = 0; i < Memo1->Lines->Count; i++)
	{
		AnsiString s = Memo1->Lines->Strings[i];
		fprintf(fout, "%s\n", s.c_str());
	}

	fclose(fout);
}

void __fastcall TCommsForm::ClearButtonClick(TObject *Sender)
{
	clear();
}

void __fastcall TCommsForm::CommandEditKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	switch (Key)
	{
		case VK_RETURN:
			Key = 0;
			{
				String s = CommandEdit->Text.Trim();
				CommandEdit->Text = "";

				if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
				{	// JANVNA-V2
				}
				else
				if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
				{	// V2

					std::vector <String> params;
					std::vector <uint8_t> data;

					{	// remove any spaces and control chars
						int i = 1;
						while (i <= s.Length())
						{
							if (s[i] <= 32)
								s = s.Delete(i, 1).Trim();
							else
								i++;
						}
					}

					if (s.LowerCase().Pos("0x") > 0)
					{	// the user is using "0x" with each byte
						common.parseString(s.LowerCase(), "0x", params);
						for (unsigned int i = 0; i < params.size(); i++)
						{
							String s2 = params[i];
							int value;
							if (!TryStrToInt("0x" + s2, value))
							{
								Memo1->Lines->Add("error: only hex characters allowed '" + s + "'");
								data.resize(0);
								break;
							}
							data.push_back((uint8_t)value);
						}
					}
					else
					{	// no they're not
						for (int i = 1; i <= s.Length(); i += 2)
						{
							String s2 = s.SubString(i, 2);	// get two (or one) hex char(s)
							int value;
							if (!TryStrToInt("0x" + s2, value))
							{	// error
								Memo1->Lines->Add("error: only hex characters allowed '" + s + "'");
								data.resize(0);
								break;
							}
							data.push_back((uint8_t)value);
                  }
					}

					if (!data.empty())
					{	// send the data
						if (Form1)
							Form1->addSerialTxCommand(data);

						int i = 0;
						while (i < (int)m_command_history.size())
						{
							const String s2 = m_command_history[i].LowerCase();
							if (s2 == s.LowerCase())
								break;
							i++;
						}

						if (i < (int)m_command_history.size())
							m_command_history.erase(m_command_history.begin() + i);   // delete the command from the history

						m_command_history.push_back(s);
						m_command_history_index = m_command_history.size();
					}
				}
				else
				{	// V1
					if (Form1)
						Form1->addSerialTxCommand(s);

					int i = 0;
					while (i < (int)m_command_history.size())
					{
						const String s2 = m_command_history[i].LowerCase();
						if (s2 == s.LowerCase())
							break;
						i++;
					}

					if (i < (int)m_command_history.size())
						m_command_history.erase(m_command_history.begin() + i);   // delete the command from the history

					m_command_history.push_back(s);
					m_command_history_index = m_command_history.size();
				}
			}
			break;

		case VK_UP:		// up arrow
			Key = 0;
			if (!m_command_history.empty())
			{
				if (m_command_history_index > 0)
					CommandEdit->Text = m_command_history[--m_command_history_index];
			}
			break;

		case VK_DOWN:	// down arrow
			Key = 0;
			if (!m_command_history.empty())
			{
				if (m_command_history_index < ((int)m_command_history.size() - 1))
					CommandEdit->Text = m_command_history[++m_command_history_index];
				else
				{
					if (m_command_history_index < (int)m_command_history.size())
						m_command_history_index++;
					CommandEdit->Text = "";
				}
			}
			break;
	}
}

void __fastcall TCommsForm::FormKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	switch (Key)
	{
		case VK_ESCAPE:
			Key = 0;
			if (Form1)
				Form1->pauseComms(false);
			this->Hide();
			break;
	}
}

void __fastcall TCommsForm::CommandEditChange(TObject *Sender)
{
	if (Form1)
	{
		const String s = CommandEdit->Text.Trim();
		Form1->pauseComms(!s.IsEmpty() || PauseCommsToggleSwitch->State == tssOn);
	}
}

void __fastcall TCommsForm::FormHide(TObject *Sender)
{
	if (!Memo1->ReadOnly) 				// being used to edit user commands ?
		SaveButtonClick(SaveButton);	// yes

	saveCommandHistory();
	saveUserCommands();

	if (Form1)
		Form1->pauseComms(false);
}

void __fastcall TCommsForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	saveCommandHistory();
	saveUserCommands();

	if (Form1)
		Form1->pauseComms(false);
}

void __fastcall TCommsForm::PauseCommsToggleSwitchClick(TObject *Sender)
{
	if (Form1)
	{
		const String s = CommandEdit->Text.Trim();
		Form1->pauseComms(!s.IsEmpty() || (PauseCommsToggleSwitch->State == tssOn));
	}
}

void __fastcall TCommsForm::CommandEditExit(TObject *Sender)
{
	if (Form1)
	{
		const String s = CommandEdit->Text.Trim();
		Form1->pauseComms(PauseCommsToggleSwitch->State == tssOn);
	}
}

void __fastcall TCommsForm::CommandEditEnter(TObject *Sender)
{
	if (Form1)
	{
		const String s = CommandEdit->Text.Trim();
		Form1->pauseComms(!s.IsEmpty() || (PauseCommsToggleSwitch->State == tssOn));
	}
}

void __fastcall TCommsForm::loadCommandHistory()
{
	const String filename = ExtractFilePath(Application->ExeName) + VNA_COMMS_COMMAND_HISTORY_FILENAME;

	std::vector <String> cmds;

	if (common.loadFile(filename, cmds) > 0)
	{
		m_command_history = cmds;
		m_command_history_index = m_command_history.size();
	}
}

void __fastcall TCommsForm::saveCommandHistory()
{
	const String filename = ExtractFilePath(Application->ExeName) + VNA_COMMS_COMMAND_HISTORY_FILENAME;
	common.saveFile(filename, m_command_history);
}

void __fastcall TCommsForm::loadUserCommands()
{
	const String path = ExtractFilePath(Application->ExeName);

	for (unsigned int i = 0; i < VNA_MAX_USER_COMMANDS; i++)
	{
		String filename;
		filename.printf(L"user_command_%u.txt", i);

		std::vector <String> cmds;

		if (common.loadFile(path + filename, cmds) > 0)
		{
			TButton *but = NULL;
			switch (i)
			{
				case 0: but = CommandsButton1; break;
				case 1: but = CommandsButton2; break;
				case 2: but = CommandsButton3; break;
				case 3: but = CommandsButton4; break;
				default: break;
			}
			if (but == NULL)
				continue;

			String s;

			if (cmds.empty())
			{
				m_user_commands[i].resize(0);
			}
			else
			{
				s = cmds[0].Trim();  // 1st line is the name of the command sequence
				m_user_commands[i] = cmds;
			}

			if (s.IsEmpty())
				s.printf(L"Commands %d", 1 + i);

			but->Caption = s;

			String hint;
			for (unsigned int i = 1; i < cmds.size(); i++)
			{
				s = cmds[i].Trim();
				if (i < (cmds.size() - 1))
					hint += s + "\r\n";
				else
					hint += s;
			}
			but->Hint = hint;
		}
	}
}

void __fastcall TCommsForm::saveUserCommands()
{
	const String path = ExtractFilePath(Application->ExeName);

	for (unsigned int i = 0; i < VNA_MAX_USER_COMMANDS; i++)
	{
		std::vector <String> cmds = m_user_commands[i];

		String name;
		switch (i)
		{
			case 0: name = CommandsButton1->Caption; break;
			case 1: name = CommandsButton2->Caption; break;
			case 2: name = CommandsButton3->Caption; break;
			case 3: name = CommandsButton4->Caption; break;
			default: break;
		}

		name = name.Trim();
		if (name.IsEmpty())
			name.printf(L"Commands %d", 1 + i);

		// the 1st line is the name of the command sequence
		if (cmds.empty())
			cmds.push_back(name.Trim());
		else
			cmds[0] = name.Trim();	// the 1st line is the name of the command sequence

		String filename;
		filename.printf(L"user_command_%u.txt", i);

		common.saveFile(path + filename, cmds);
	}
}

void __fastcall TCommsForm::editUserCommand(const int index)
{
	if (index < 0 || index >= VNA_MAX_USER_COMMANDS)
		return;

	// use the memo for the command editing

	Memo1->Lines->BeginUpdate();
	Memo1->ReadOnly = false;

	m_memo_contents.resize(0);
	for (int i = 0; i < Memo1->Lines->Count; i++)
		m_memo_contents.push_back(Memo1->Lines->Strings[i]);

	Memo1->Clear();
	Memo1->SetFocus();

	std::vector <String> cmds = m_user_commands[index];
	for (unsigned int i = 1; i < cmds.size(); i++)
		Memo1->Lines->Add(cmds[i].Trim());

	Memo1->Lines->EndUpdate();

	SaveButton->Enabled = true;

	m_commands_edit_index = index;
}

TButton * __fastcall TCommsForm::getPopupMenuButton()
{
	TButton *but = NULL;
	if (m_popup_menu_button != NULL)
	{
		but = m_popup_menu_button;
		m_popup_menu_button = NULL;
	}
	else
	{
		TComponent *comp = CommandsButtonPopupMenu->PopupComponent;
		if (comp)
			if (comp->ClassNameIs("TButton"))
				but = dynamic_cast<TButton *>(comp);
	}
	return but;
}

void __fastcall TCommsForm::Clearcommand1Click(TObject *Sender)
{
	TButton *but = getPopupMenuButton();
	if (but == CommandsButton1)
	{
		m_user_commands[0].resize(0);
		but->Hint = "";
		saveUserCommands();
	}
	else
	if (but == CommandsButton2)
	{
		m_user_commands[1].resize(0);
		but->Hint = "";
		saveUserCommands();
	}
	else
	if (but == CommandsButton3)
	{
		m_user_commands[2].resize(0);
		but->Hint = "";
		saveUserCommands();
	}
	else
	if (but == CommandsButton4)
	{
		m_user_commands[3].resize(0);
		but->Hint = "";
		saveUserCommands();
	}
}

void __fastcall TCommsForm::Editcommand1Click(TObject *Sender)
{
	TButton *but = getPopupMenuButton();
	if (but == CommandsButton1)
	{
		editUserCommand(0);
	}
	else
	if (but == CommandsButton2)
	{
		editUserCommand(1);
	}
	else
	if (but == CommandsButton3)
	{
		editUserCommand(2);
	}
	else
	if (but == CommandsButton4)
	{
		editUserCommand(3);
	}
}

void __fastcall TCommsForm::Copy1Click(TObject *Sender)
{
	TButton *but = getPopupMenuButton();
	int index = -1;
	if (but == CommandsButton1) index = 0;
	else
	if (but == CommandsButton2) index = 1;
	else
	if (but == CommandsButton3) index = 2;
	else
	if (but == CommandsButton4) index = 3;
	if (index < 0)
		return;

	std::vector <String> cmds = m_user_commands[index];

	String s;
	for (unsigned int i = 1; i < cmds.size(); i++)
	{
		if (i < (cmds.size() - 1))
			s += cmds[i] + "\r\n";
		else
			s += cmds[i];
	}

	try
	{
		Clipboard()->SetTextBuf(s.w_str());
	}
	catch (...)
	{
		Application->NormalizeTopMosts();
		Application->MessageBox(L"Failed to copy to clipboard", L"Error", MB_ICONERROR | MB_OK);
		Application->RestoreTopMosts();
	}
}

void __fastcall TCommsForm::Paste1Click(TObject *Sender)
{
	TButton *but = getPopupMenuButton();
	if (but == NULL)
		return;

	if (!Clipboard()->HasFormat(CF_TEXT))
		return;

	String s = Clipboard()->AsText;
//	Clipboard()->Clear();

	{  // remove all CR's
		int pos;
		while ((pos = s.Pos('\r')) > 0)
			s = s.Delete(pos, 1).Trim();
	}

	{	// replace all tabs with spaces
		int pos;
		while ((pos = s.Pos('\t')) > 0)
			s[pos] = ' ';
	}

	{	// remove all control chars apart from line feeds
		int i = 1;
		while (i <= s.Length())
		{
			const char c = s[i];
			if (c < 32 && c != '\n')
				s = s.Delete(i, 1).Trim();
			else
				i++;
		}
	}

	std::vector <String> cmds;
	{
		int pos;
		while ((pos = s.Pos('\n')) > 0)
		{
			String s2 = s.SubString(1, pos - 1).Trim();
			s = s.SubString(pos + 1, s.Length()).Trim();
			if (!s2.IsEmpty())
				cmds.push_back(s2);
		}
		if (!s.IsEmpty())
			cmds.push_back(s);
	}

	String hint;
	for (unsigned int i = 0; i < cmds.size(); i++)
	{
		if (i < (cmds.size() - 1))
			hint += cmds[i] + "\r\n";
		else
			hint += cmds[i];
	}

	cmds.insert(cmds.begin() + 0, but->Caption.Trim());	// command sequence name
	int index = -1;
	if (but == CommandsButton1) index = 0;
	else
	if (but == CommandsButton2) index = 1;
	else
	if (but == CommandsButton3) index = 2;
	else
	if (but == CommandsButton4) index = 3;
	if (index >= 0)
	{
		m_user_commands[index] = cmds;
		but->Hint = hint;
		saveUserCommands();
	}
}

void __fastcall TCommsForm::Rename1Click(TObject *Sender)
{
	TButton *but = getPopupMenuButton();
	if (but == NULL)
		return;

	String name = but->Caption.Trim();

	if (!InputQuery("Enter a name for the command sequence", "Name", name))
		return;

	name = name.Trim();

	if (name.IsEmpty())
	{
		if (but == CommandsButton1)
			name = "Commands 1";
		else
		if (but == CommandsButton2)
			name = "Commands 2";
		else
		if (but == CommandsButton3)
			name = "Commands 3";
		else
		if (but == CommandsButton4)
			name = "Commands 4";
	}
	but->Caption = name;

	saveUserCommands();
}

void __fastcall TCommsForm::CommandsButtonClick(TObject *Sender)
{
	TButton *but = dynamic_cast<TButton *>(Sender);
	if (but == NULL)
		return;

	if (data_unit.m_vna_data.type == UNIT_TYPE_JANVNA_V2)
		return;
	if (data_unit.m_vna_data.type == UNIT_TYPE_NANOVNA_V2)
		return;

	int index = -1;
	if (but == CommandsButton1) index = 0;
	else
	if (but == CommandsButton2) index = 1;
	else
	if (but == CommandsButton3) index = 2;
	else
	if (but == CommandsButton4) index = 3;
	if (index < 0)
		return;

	if (!Memo1->ReadOnly) 				// being used to edit user commands ?
		SaveButtonClick(SaveButton);	// yes

	if (Form1)
	{	// send the commands

		// fetch the commands
		std::vector <String> cmds = m_user_commands[index];
		if (!cmds.empty())
		{	// remove all comment/empty lines
			unsigned int i = 0;

			cmds.erase(cmds.begin() + i);	// the first line is the name of the list of commands

			while (i < cmds.size())
			{
				// fetch a text line (a single command)
				String s = cmds[i].Trim();

				// remove any control characters
				int k = 1;
				while (k <= s.Length())
				{
					if (s[k] == '\t')
						s = ' ';							// replace tab with space
					if (s[k] < 32)
						s = s.Delete(k, 1).Trim();	// remove control character from the line
					else
						k++;
				}

				if (s.IsEmpty())
				{	// the line is empty
					cmds.erase(cmds.begin() + i);
					continue;
				}

				// check the first character, if it's a comment indicator then remove the line from the list of commands
				if (s[1] == ';' || s[1] == '#' || s[1] == ':' || s[1] == '/')
				{
					cmds.erase(cmds.begin() + i);	// a comment line - remove it
					continue;
				}

				// line seems OK, move on to next line
				i++;
			}
		}

		// send the commands to the unit
		for (unsigned int i = 0; i < cmds.size(); i++)
			Form1->addSerialTxCommand(cmds[i]);
	}
}

void __fastcall TCommsForm::CommandsButtonDropDownClick(TObject *Sender)
{
	if (!Memo1->ReadOnly) 				// being used to edit user commands ?
		SaveButtonClick(SaveButton);	// yes

	m_popup_menu_button = dynamic_cast<TButton *>(Sender);
}

void __fastcall TCommsForm::Cancel1Click(TObject *Sender)
{
	m_popup_menu_button = NULL;
}

void __fastcall TCommsForm::CommandsButtonPopupMenuPopup(TObject *Sender)
{
	Paste1->Enabled = Clipboard()->HasFormat(CF_TEXT);
}

void __fastcall TCommsForm::FormMouseEnter(TObject *Sender)
{
//	this->SetFocus();
}


object CommsForm: TCommsForm
  Left = 1004
  Top = 146
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = 'VNA Comms'
  ClientHeight = 397
  ClientWidth = 469
  Color = clBtnFace
  Constraints.MinHeight = 300
  Constraints.MinWidth = 485
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Lucida Sans Unicode'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnHide = FormHide
  OnKeyDown = FormKeyDown
  OnMouseEnter = FormMouseEnter
  OnShow = FormShow
  DesignSize = (
    469
    397)
  PixelsPerInch = 96
  TextHeight = 15
  object Label1: TLabel
    Top = 344
    Left = 8
    Width = 55
    Height = 15
    Alignment = taRightJustify
    Anchors = [akLeft, akBottom]
    Caption = 'User cmd '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 8
    Top = 374
    Width = 61
    Height = 15
    Alignment = taRightJustify
    Anchors = [akLeft, akBottom]
    Caption = 'User cmds '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Memo1: TMemo
    Left = 6
    Top = 36
    Width = 458
    Height = 298
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    Lines.Strings = (
      'Memo1')
    MaxLength = 250000
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 5
    WordWrap = False
  end
  object CloseButton: TButton
    Left = 389
    Top = 5
    Width = 75
    Height = 25
    Cursor = crHandPoint
    Anchors = [akTop, akRight]
    Caption = 'Close'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 4
    OnClick = CloseButtonClick
  end
  object SaveButton: TButton
    Left = 308
    Top = 5
    Width = 75
    Height = 25
    Cursor = crHandPoint
    Anchors = [akTop, akRight]
    Caption = 'Save'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    OnClick = SaveButtonClick
  end
  object ClearButton: TButton
    Left = 228
    Top = 5
    Width = 75
    Height = 25
    Cursor = crHandPoint
    Anchors = [akTop, akRight]
    Caption = 'Clear'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    OnClick = ClearButtonClick
  end
  object CommandEdit: TEdit
    Left = 95
    Top = 340
    Width = 369
    Height = 23
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 6
    OnChange = CommandEditChange
    OnEnter = CommandEditEnter
    OnExit = CommandEditExit
    OnKeyDown = CommandEditKeyDown
  end
  object PauseCommsToggleSwitch: TToggleSwitch
    Left = 112
    Top = 8
    Width = 106
    Height = 20
    FrameColor = clBtnFace
    StateCaptions.CaptionOn = 'Paused'
    StateCaptions.CaptionOff = 'Un-paused'
    SwitchWidth = 40
    TabOrder = 1
    ThumbColor = clBlue
    ThumbWidth = 20
    OnClick = PauseCommsToggleSwitchClick
  end
  object ScrollToggleSwitch: TToggleSwitch
    Left = 16
    Top = 8
    Width = 76
    Height = 20
    FrameColor = clBtnFace
    StateCaptions.CaptionOn = 'Hold'
    StateCaptions.CaptionOff = 'Scroll'
    SwitchWidth = 40
    TabOrder = 0
    ThumbColor = clBlue
    ThumbWidth = 20
  end
  object GridPanel1: TGridPanel
    Left = 95
    Top = 368
    Width = 369
    Height = 25
    Anchors = [akLeft, akRight, akBottom]
    BevelOuter = bvNone
    Caption = 'V'
    ColumnCollection = <
      item
        Value = 25.000000000000000000
      end
      item
        Value = 25.000000000000000000
      end
      item
        Value = 25.000000000000000000
      end
      item
        Value = 25.000000000000000000
      end>
    ControlCollection = <
      item
        Column = 0
        Control = CommandsButton1
        Row = 0
      end
      item
        Column = 1
        Control = CommandsButton2
        Row = 0
      end
      item
        Column = 2
        Control = CommandsButton3
        Row = 0
      end
      item
        Column = 3
        Control = CommandsButton4
        Row = 0
      end>
    RowCollection = <
      item
        Value = 100.000000000000000000
      end>
    TabOrder = 7
    object CommandsButton1: TButton
      Left = 0
      Top = 0
      Width = 92
      Height = 25
      Cursor = crHandPoint
      Align = alClient
      Caption = 'Commands 1'
      DoubleBuffered = True
      DropDownMenu = CommandsButtonPopupMenu
      ParentDoubleBuffered = False
      ParentShowHint = False
      ShowHint = False
      Style = bsSplitButton
      TabOrder = 0
      OnClick = CommandsButtonClick
      OnDropDownClick = CommandsButtonDropDownClick
    end
    object CommandsButton2: TButton
      Left = 92
      Top = 0
      Width = 92
      Height = 25
      Cursor = crHandPoint
      Align = alClient
      Caption = 'Commands 2'
      DoubleBuffered = True
      DropDownMenu = CommandsButtonPopupMenu
      ParentDoubleBuffered = False
      ParentShowHint = False
      ShowHint = False
      Style = bsSplitButton
      TabOrder = 1
      OnClick = CommandsButtonClick
      OnDropDownClick = CommandsButtonDropDownClick
    end
    object CommandsButton3: TButton
      Left = 184
      Top = 0
      Width = 93
      Height = 25
      Cursor = crHandPoint
      Align = alClient
      Caption = 'Commands 3'
      DoubleBuffered = True
      DropDownMenu = CommandsButtonPopupMenu
      ParentDoubleBuffered = False
      ParentShowHint = False
      ShowHint = False
      Style = bsSplitButton
      TabOrder = 2
      OnClick = CommandsButtonClick
      OnDropDownClick = CommandsButtonDropDownClick
    end
    object CommandsButton4: TButton
      Left = 277
      Top = 0
      Width = 92
      Height = 25
      Cursor = crHandPoint
      Align = alClient
      Caption = 'Commands 4'
      DoubleBuffered = True
      DropDownMenu = CommandsButtonPopupMenu
      ParentDoubleBuffered = False
      ParentShowHint = False
      ShowHint = False
      Style = bsSplitButton
      TabOrder = 3
      OnClick = CommandsButtonClick
      OnDropDownClick = CommandsButtonDropDownClick
    end
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'txt'
    Filter = '*.txt|*.txt|*.*|*.*'
    Options = [ofPathMustExist, ofEnableSizing]
    Left = 44
    Top = 56
  end
  object CommandsButtonPopupMenu: TPopupMenu
    OnPopup = CommandsButtonPopupMenuPopup
    Left = 104
    Top = 64
    object Clearcommand1: TMenuItem
      Caption = 'C&lear'
      OnClick = Clearcommand1Click
    end
    object Editcommand1: TMenuItem
      Caption = '&Edit'
      OnClick = Editcommand1Click
    end
    object Copy1: TMenuItem
      Caption = '&Copy'
      OnClick = Copy1Click
    end
    object Paste1: TMenuItem
      Caption = '&Paste'
      OnClick = Paste1Click
    end
    object Rename1: TMenuItem
      Caption = '&Rename'
      OnClick = Rename1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Cancel1: TMenuItem
      Caption = 'Ca&ncel'
      OnClick = Cancel1Click
    end
  end
end

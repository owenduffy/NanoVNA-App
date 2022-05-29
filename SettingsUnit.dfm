object SettingsForm: TSettingsForm
  Left = 192
  Top = 112
  AlphaBlendValue = 160
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = 'Settings'
  ClientHeight = 531
  ClientWidth = 824
  Color = clBtnFace
  Constraints.MinHeight = 570
  Constraints.MinWidth = 840
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Lucida Sans Unicode'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  SnapBuffer = 6
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnHide = FormHide
  OnKeyDown = FormKeyDown
  OnMouseEnter = FormMouseEnter
  OnShow = FormShow
  DesignSize = (
    824
    531)
  PixelsPerInch = 96
  TextHeight = 15
  object Label7: TLabel
    Left = 763
    Top = 26
    Width = 58
    Height = 15
    Anchors = [akTop, akRight]
    Caption = 'Firmware'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clMaroon
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label6: TLabel
    Left = 6
    Top = 14
    Width = 68
    Height = 15
    Alignment = taRightJustify
    Caption = 'Record path '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object ColourBevel: TBevel
    Left = 4
    Top = 42
    Width = 401
    Height = 380
    Anchors = [akLeft, akTop, akBottom]
  end
  object Label9: TLabel
    Left = 15
    Top = 54
    Width = 50
    Height = 15
    Alignment = taRightJustify
    Caption = 'GUI Style '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object GraphColourPaintBox: TPaintBox
    Left = 12
    Top = 77
    Width = 273
    Height = 337
    Cursor = crHandPoint
    Anchors = [akLeft, akTop, akBottom]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -9
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
    OnMouseDown = GraphColourPaintBoxMouseDown
    OnMouseLeave = GraphColourPaintBoxMouseLeave
    OnMouseMove = GraphColourPaintBoxMouseMove
    OnPaint = GraphColourPaintBoxPaint
    ExplicitHeight = 364
  end
  object Label14: TLabel
    Left = 297
    Top = 221
    Width = 54
    Height = 15
    Caption = 'Line alpha'
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label5: TLabel
    Left = 297
    Top = 259
    Width = 57
    Height = 15
    Caption = 'Line width '
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label15: TLabel
    Left = 297
    Top = 300
    Width = 53
    Height = 15
    Caption = 'Marker fill'
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label16: TLabel
    Left = 297
    Top = 183
    Width = 71
    Height = 15
    Caption = 'Border width '
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label17: TLabel
    Left = 297
    Top = 338
    Width = 99
    Height = 15
    Caption = 'Auto scale pk hold'
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label18: TLabel
    Left = 297
    Top = 376
    Width = 95
    Height = 15
    Caption = 'Smith both scales'
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Memo1: TMemo
    Left = 416
    Top = 39
    Width = 398
    Height = 119
    Anchors = [akLeft, akTop, akRight]
    DoubleBuffered = True
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    Lines.Strings = (
      'Memo1')
    ParentDoubleBuffered = False
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 1
    WordWrap = False
  end
  object FreqBandGroupBox: TGroupBox
    Left = 416
    Top = 164
    Width = 399
    Height = 362
    Anchors = [akLeft, akTop, akRight, akBottom]
    Caption = ' Frequency Bands '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clMaroon
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    DesignSize = (
      399
      362)
    object Label10: TLabel
      Left = 14
      Top = 48
      Width = 50
      Height = 15
      Alignment = taRightJustify
      Caption = 'Low MHz '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
    end
    object Label11: TLabel
      Left = 170
      Top = 48
      Width = 54
      Height = 15
      Alignment = taRightJustify
      Caption = 'High MHz '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
    end
    object Label12: TLabel
      Left = 30
      Top = 24
      Width = 34
      Height = 15
      Alignment = taRightJustify
      Caption = 'Name '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
    end
    object Label8: TLabel
      Left = 287
      Top = 76
      Width = 22
      Height = 15
      Alignment = taRightJustify
      Caption = 'Pad '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
    end
    object FreqBandLowMHzEdit: TEdit
      Left = 68
      Top = 44
      Width = 77
      Height = 23
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
    end
    object FreqBandHighMHzEdit: TEdit
      Left = 228
      Top = 44
      Width = 77
      Height = 23
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
    end
    object FreqBandListBox: TListBox
      Left = 12
      Top = 100
      Width = 375
      Height = 252
      Cursor = crHandPoint
      Anchors = [akLeft, akTop, akRight, akBottom]
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ItemHeight = 15
      ParentFont = False
      TabOrder = 7
      OnClick = FreqBandListBoxClick
      OnDblClick = FreqBandListBoxDblClick
      OnKeyDown = FreqBandListBoxKeyDown
    end
    object FreqBandDeleteButton: TButton
      Left = 12
      Top = 72
      Width = 75
      Height = 21
      Cursor = crHandPoint
      Caption = 'Delete'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
      OnClick = FreqBandDeleteButtonClick
    end
    object FreqBandAddButton: TButton
      Left = 92
      Top = 72
      Width = 75
      Height = 21
      Cursor = crHandPoint
      Caption = 'Add'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 4
      OnClick = FreqBandAddButtonClick
    end
    object FreqBandNameEdit: TEdit
      Left = 68
      Top = 20
      Width = 77
      Height = 23
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      MaxLength = 12
      ParentFont = False
      TabOrder = 0
    end
    object SetScanButton: TButton
      Left = 200
      Top = 72
      Width = 75
      Height = 21
      Cursor = crHandPoint
      Caption = 'Set Sweep'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 5
      OnClick = SetScanButtonClick
    end
    object FreqBandPadComboBox: TComboBox
      Left = 312
      Top = 72
      Width = 77
      Height = 23
      Cursor = crHandPoint
      Style = csDropDownList
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
      TabOrder = 6
      OnChange = FreqBandPadComboBoxChange
      Items.Strings = (
        '')
    end
    object FreqBandDefaultButton: TButton
      Left = 312
      Top = 13
      Width = 77
      Height = 25
      Cursor = crHandPoint
      Hint = 'Set default frequency bands'
      Caption = 'Default'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 8
      OnClick = FreqBandDefaultButtonClick
    end
    object FreqBandEnableToggleSwitch: TToggleSwitch
      Left = 194
      Top = 18
      Width = 69
      Height = 20
      Cursor = crHandPoint
      Alignment = taLeftJustify
      Font.Charset = ANSI_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      FrameColor = clBtnFace
      ParentFont = False
      State = tssOn
      StateCaptions.CaptionOn = 'Show'
      StateCaptions.CaptionOff = 'Hide'
      SwitchWidth = 35
      TabOrder = 9
      ThumbColor = clNavy
      ThumbWidth = 18
      OnClick = FreqBandEnableToggleSwitchClick
    end
  end
  object VNAGroupBox: TGroupBox
    Left = 8
    Top = 432
    Width = 377
    Height = 94
    Anchors = [akLeft, akBottom]
    Caption = ' VNA Settings '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clMaroon
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    DesignSize = (
      377
      94)
    object Label1: TLabel
      Left = 18
      Top = 20
      Width = 58
      Height = 15
      Alignment = taRightJustify
      Caption = 'Threshold '
      Font.Charset = ANSI_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
    end
    object Label2: TLabel
      Left = -400
      Top = 20
      Width = 26
      Height = 15
      Anchors = [akTop, akRight]
      Caption = 'MHz'
    end
    object Label3: TLabel
      Left = 9
      Top = 44
      Width = 67
      Height = 15
      Alignment = taRightJustify
      Caption = 'VBAT Offset '
      Font.Charset = ANSI_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
    end
    object Label4: TLabel
      Left = 154
      Top = 44
      Width = 18
      Height = 15
      Caption = 'mV'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
    end
    object Label13: TLabel
      Left = 154
      Top = 20
      Width = 23
      Height = 15
      Caption = 'MHz'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentFont = False
    end
    object ThresholdEdit: TEdit
      Left = 79
      Top = 16
      Width = 69
      Height = 23
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clMaroon
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      Text = 'ThresholdEdit'
      OnKeyDown = ThresholdEditKeyDown
    end
    object ClearConfigButton: TButton
      Left = 260
      Top = 40
      Width = 105
      Height = 21
      Cursor = crHandPoint
      Hint = 'Clear the VNA'#39's configuration settings'
      Caption = 'Clear Config'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      OnClick = ClearConfigButtonClick
    end
    object SetTimeButton: TButton
      Left = 260
      Top = 16
      Width = 105
      Height = 21
      Cursor = crHandPoint
      Hint = 'Sets the VNA'#39's date and time to your PC'#39's current date and time'
      Caption = 'Set Date/Time'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      OnClick = SetTimeButtonClick
    end
    object SaveConfigButton: TButton
      Left = 260
      Top = 64
      Width = 105
      Height = 21
      Cursor = crHandPoint
      Hint = 'Save the VNA'#39's configuration settings'
      Caption = 'Save Config'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
      OnClick = SaveConfigButtonClick
    end
    object VBatOffsetSpinEdit: TSpinEdit
      Left = 79
      Top = 40
      Width = 69
      Height = 24
      MaxValue = 1000
      MinValue = 0
      TabOrder = 1
      Value = 0
      OnChange = VBatOffsetSpinEditChange
    end
  end
  object RecordPathEdit: TEdit
    Left = 78
    Top = 10
    Width = 588
    Height = 23
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 3
    Text = 'RecordPathEdit'
    OnChange = RecordPathEditChange
  end
  object SelectRecordPathBitBtn: TBitBtn
    Left = 672
    Top = 10
    Width = 33
    Height = 21
    Cursor = crHandPoint
    Hint = 'Select recording folder'
    Anchors = [akTop, akRight]
    BiDiMode = bdLeftToRight
    Glyph.Data = {
      36060000424D3606000000000000360400002800000020000000100000000100
      08000000000000020000220B0000220B00000001000000010000000000003131
      31007B7B7B00BDBDBD00E7E7E700FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010105050505050505050505010101010100000000
      0000000000000101010101010202020202020202020201050101010000040304
      0304030403040001010101020201050101010101010102010501010005000403
      0403040304030400010101020502010501010101010101020105010004050004
      0304030403040304000101020501020105050505050505050205010005040500
      0000000000000000000101020501010202020202020202020201010004050405
      0405040504000101010101020501010101010101010205010101010005040504
      0504050405000101010101020501010101010105050201010101010004050405
      0405000000010101010101020105010101010202020101010101010100040504
      0500010101010101010101010201050505020101010101010101010102000000
      0002010101010101010101010102020202010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101010101010101
      0101010101010101010101010101010101010101010101010101}
    NumGlyphs = 2
    ParentBiDiMode = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnClick = SelectRecordPathBitBtnClick
  end
  object CloseBitBtn: TBitBtn
    Left = 780
    Top = 8
    Width = 36
    Height = 24
    Cursor = crHandPoint
    Anchors = [akTop, akRight]
    Glyph.Data = {
      36050000424D3605000000000000360400002800000010000000100000000100
      08000000000000010000130B0000130B00000001000000010000FFFFFF006360
      F8007774FF007774FF00625FF8005D5BF7005956F500726FFD00615EF8005451
      F3004F4DF2006C69FB00605DF7004A47F0004542EE007673FF007471FE006361
      F9005F5CF7004946F0004441EE00716EFD006E6BFC004845EF004341EE006967
      FB006663F9004744EF004240EE00615EF8005D5AF6004643EF00413FED005C59
      F6003C39EB005B58F6003230E8005A57F5004341EE003E3CEC002A27E5005956
      F5004240EE003E3BEC003937EB003532E9002220E3005451F300413FED003D3A
      EC00302DE7002C2AE6001F1DE2004A47F000403EED003C39EB002725E5002422
      E4003F3DED003B38EB00211FE3001E1CE2007A77FF007976FE006461F8006A68
      F900817EFF00817EFE007471FD00625FF8006F6DFB007E7CFF007D7AFE008A87
      FF007C79FD006C69FB00615EF8006E6CFA007D7AFF00615FF7007774FD008682
      FF007673FC006462F800605DF7006D6AFA007B79FF00706DFB00807EFF007E7B
      FF007C79FF007977FF005E5CF7007D79FF005E5BFF005B58FF007674FF005D5B
      F7007976FF005956FF005754FF007270FF004846F0005C5AF6006764FA007472
      FF007370FF00706EFF006E6CFF005755F7003F3DEE005B59F6006663FA007471
      FF005A58F600504DF4006867FF00504EF5003634EB005B58F6006562FA007170
      FF005956F6004745F2006362FF004A48F4002F2DE9005856F5006361FA005855
      F600413FF1004C4AF600312FEA004F4CF200FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000003A3B0000000000003C3D000000000035853637
      00000000383984340000002F7F8081303100003233828384340000297778797A
      2A2B2C2D7B7C7D7E2E000000256F707172262773747576280000000000236768
      696A6B6C6D6E2400000000000000216162636465662200000000000000001D1E
      5D5E5F601F2000000000000000191A5758595A5B5C1B1C000000000015165051
      52535455565417180000000F1048494A4B11124C4D4E4F13140000023E424344
      0B00000C454647450D0E0000023E3F0700000000084041090A00000000020300
      0000000000040506000000000000000000000000000001000000}
    TabOrder = 5
    Visible = False
    OnClick = CloseBitBtnClick
  end
  object GUIStyleComboBox: TComboBox
    Left = 69
    Top = 50
    Width = 216
    Height = 23
    Cursor = crHandPoint
    Hint = 'Set the GUI style'
    AutoDropDown = True
    Style = csDropDownList
    DropDownCount = 40
    Sorted = True
    TabOrder = 6
    OnChange = GUIStyleComboBoxChange
  end
  object DefaultLightColoursButton: TButton
    Left = 291
    Top = 50
    Width = 108
    Height = 21
    Cursor = crHandPoint
    Hint = 'Set default light colours'
    Caption = 'Default Lt'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 7
    OnClick = DefaultDarkColoursButtonClick
  end
  object DefaultDarkColoursButton: TButton
    Left = 291
    Top = 77
    Width = 108
    Height = 21
    Cursor = crHandPoint
    Hint = 'Set default dark colours'
    Caption = 'Default Dk'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 8
    OnClick = DefaultDarkColoursButtonClick
  end
  object FontButton: TButton
    Left = 291
    Top = 131
    Width = 108
    Height = 21
    Cursor = crHandPoint
    Hint = 'Select graph font'
    Caption = 'Graph Font'
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 9
    Visible = False
    OnClick = FontButtonClick
  end
  object LineAlphaTrackBar: TTrackBar
    Left = 291
    Top = 235
    Width = 83
    Height = 21
    Cursor = crHandPoint
    Ctl3D = True
    Max = 255
    Min = 16
    ParentCtl3D = False
    ParentShowHint = False
    PageSize = 16
    Position = 180
    ShowHint = True
    TabOrder = 10
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = LineAlphaTrackBarChange
  end
  object LineWidthTrackBar: TTrackBar
    Left = 291
    Top = 274
    Width = 83
    Height = 21
    Cursor = crHandPoint
    Ctl3D = True
    Max = 3
    Min = 1
    ParentCtl3D = False
    ParentShowHint = False
    PageSize = 1
    Position = 1
    ShowHint = True
    TabOrder = 11
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = LineWidthTrackBarChange
  end
  object MarkerFillToggleSwitch: TToggleSwitch
    Left = 301
    Top = 318
    Width = 65
    Height = 17
    Cursor = crHandPoint
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    FrameColor = clBtnFace
    ParentFont = False
    State = tssOn
    StateCaptions.CaptionOn = 'Yes'
    StateCaptions.CaptionOff = 'no'
    SwitchHeight = 15
    SwitchWidth = 40
    TabOrder = 12
    ThumbColor = clNavy
    ThumbWidth = 20
    OnClick = MarkerFillToggleSwitchClick
  end
  object BorderWidthTrackBar: TTrackBar
    Left = 291
    Top = 198
    Width = 83
    Height = 21
    Cursor = crHandPoint
    Ctl3D = True
    ParentCtl3D = False
    ParentShowHint = False
    PageSize = 1
    Position = 1
    ShowHint = True
    TabOrder = 13
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = BorderWidthTrackBarChange
  end
  object Button1: TButton
    Left = 291
    Top = 104
    Width = 108
    Height = 21
    Cursor = crHandPoint
    Hint = 'Set the graph background to the window background'
    Caption = 'Bg to Win'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 14
    OnClick = Button1Click
  end
  object DefaultGraphScalesButton: TButton
    Left = 291
    Top = 158
    Width = 108
    Height = 21
    Cursor = crHandPoint
    Hint = 'Set all graph scales to defaults'
    Caption = 'Default Scales'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 15
    OnClick = DefaultGraphScalesButtonClick
  end
  object AutoScalePeakHoldToggleSwitch: TToggleSwitch
    Left = 301
    Top = 356
    Width = 65
    Height = 17
    Cursor = crHandPoint
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    FrameColor = clBtnFace
    ParentFont = False
    State = tssOn
    StateCaptions.CaptionOn = 'Yes'
    StateCaptions.CaptionOff = 'no'
    SwitchHeight = 15
    SwitchWidth = 40
    TabOrder = 16
    ThumbColor = clNavy
    ThumbWidth = 20
    OnClick = AutoScalePeakHoldToggleSwitchClick
  end
  object SmithBothScalesToggleSwitch: TToggleSwitch
    Left = 301
    Top = 394
    Width = 65
    Height = 17
    Cursor = crHandPoint
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    FrameColor = clBtnFace
    ParentFont = False
    State = tssOn
    StateCaptions.CaptionOn = 'Yes'
    StateCaptions.CaptionOff = 'no'
    SwitchHeight = 15
    SwitchWidth = 40
    TabOrder = 17
    ThumbColor = clNavy
    ThumbWidth = 20
    OnClick = SmithBothScalesToggleSwitchClick
  end
  object ColorDialog1: TColorDialog
    Options = [cdFullOpen, cdSolidColor]
    Left = 92
    Top = 158
  end
  object FontDialog1: TFontDialog
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    Options = [fdFixedPitchOnly, fdForceFontExist, fdNoVectorFonts, fdWysiwyg, fdApplyButton]
    OnApply = FontDialog1Apply
    Left = 192
    Top = 178
  end
end

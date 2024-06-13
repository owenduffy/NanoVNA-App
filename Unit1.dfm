object Form1: TForm1
  Left = 188
  Top = 132
  HorzScrollBar.Tracking = True
  HorzScrollBar.Visible = False
  VertScrollBar.Tracking = True
  VertScrollBar.Visible = False
  AlphaBlendValue = 128
  Caption = 'Form1'
  ClientHeight = 661
  ClientWidth = 884
  Color = clBtnFace
  Constraints.MinHeight = 700
  Constraints.MinWidth = 900
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Lucida Sans Unicode'
  Font.Style = []
  KeyPreview = True
  Touch.InteractiveGestures = [igPressAndTap]
  OnClose = FormClose
  OnCreate = FormCreate
  OnDblClick = FormDblClick
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnMouseEnter = FormMouseEnter
  OnMouseWheel = FormMouseWheel
  OnShow = FormShow
  DesignSize = (
    884
    661)
  TextHeight = 15
  object Label12: TLabel
    Left = 9
    Top = 307
    Width = 86
    Height = 15
    Alignment = taRightJustify
    Caption = 'Time averaging '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object MemorySpeedButton1: TSpeedButton
    Left = 64
    Top = 449
    Width = 36
    Height = 25
    Cursor = crHandPoint
    Hint = 'Right click for options'
    AllowAllUp = True
    GroupIndex = 1
    Caption = 'M1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    PopupMenu = MemoryPopupMenu
    ShowHint = True
    OnClick = MemorySpeedButtonClick
    OnMouseDown = MemorySpeedButtonMouseDown
    OnMouseUp = MemorySpeedButtonMouseUp
  end
  object MemorySpeedButton2: TSpeedButton
    Left = 104
    Top = 449
    Width = 36
    Height = 25
    Cursor = crHandPoint
    Hint = 'Right click for options'
    HelpType = htKeyword
    AllowAllUp = True
    GroupIndex = 2
    Caption = 'M2'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    PopupMenu = MemoryPopupMenu
    ShowHint = True
    OnClick = MemorySpeedButtonClick
    OnMouseDown = MemorySpeedButtonMouseDown
    OnMouseUp = MemorySpeedButtonMouseUp
  end
  object MemorySpeedButton3: TSpeedButton
    Left = 144
    Top = 449
    Width = 36
    Height = 25
    Cursor = crHandPoint
    Hint = 'Right click for options'
    AllowAllUp = True
    GroupIndex = 3
    Caption = 'M3'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    PopupMenu = MemoryPopupMenu
    ShowHint = True
    OnClick = MemorySpeedButtonClick
    OnMouseDown = MemorySpeedButtonMouseDown
    OnMouseUp = MemorySpeedButtonMouseUp
  end
  object TDRWindowLabel2: TLabel
    Left = 101
    Top = 376
    Width = 16
    Height = 15
    Caption = 'XX'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object TDRWindowLabel1: TLabel
    Left = 27
    Top = 377
    Width = 68
    Height = 15
    Alignment = taRightJustify
    Caption = 'TDR window '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 79
    Top = 92
    Width = 62
    Height = 15
    Alignment = taRightJustify
    Caption = 'Start (MHz) '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label1: TLabel
    Left = 68
    Top = 140
    Width = 73
    Height = 15
    Alignment = taRightJustify
    Caption = 'Center (MHz) '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 78
    Top = 164
    Width = 63
    Height = 15
    Alignment = taRightJustify
    Caption = 'Span (MHz) '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 80
    Top = 116
    Width = 61
    Height = 15
    Alignment = taRightJustify
    Caption = 'Stop (MHz) '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label13: TLabel
    Left = 87
    Top = 188
    Width = 54
    Height = 15
    Alignment = taRightJustify
    Caption = 'CW (MHz) '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object PointBandwidthLabel: TLabel
    Left = 61
    Top = 236
    Width = 80
    Height = 15
    Alignment = taRightJustify
    Caption = 'Point RBW (Hz) '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object PointsPerSegmentLabel: TLabel
    Left = 43
    Top = 211
    Width = 98
    Height = 15
    Alignment = taRightJustify
    Caption = 'Number of points '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label33: TLabel
    Left = 444
    Top = 35
    Width = 63
    Height = 15
    Alignment = taRightJustify
    Caption = 'Freq bands '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object SweepNameFontLabel: TLabel
    Left = 13
    Top = 246
    Width = 180
    Height = 18
    Caption = 'SweepNameFontLabel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -15
    Font.Name = 'Lucida Sans Unicode'
    Font.Pitch = fpFixed
    Font.Style = [fsBold]
    Font.Quality = fqAntialiased
    ParentFont = False
    Visible = False
  end
  object LCMatchingLabel: TLabel
    Left = 555
    Top = 35
    Width = 71
    Height = 15
    Alignment = taRightJustify
    Caption = 'LC matching '
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
    Visible = False
  end
  object ScanOnceSpeedButton: TSpeedButton
    Left = 62
    Top = 57
    Width = 51
    Height = 27
    Cursor = crHandPoint
    Hint = 'Single scan'
    AllowAllUp = True
    GroupIndex = 4
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000000B0000000B00001000000010000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      000000000000000000000000F00000000000000F1F0000000000000F11F00000
      0000000F111F00000000000F1111F0000000000F11111F000000000F111111F0
      0000000F11111F000000000F1111F0000000000F111F00000000000F11F00000
      0000000F1F00000000000000F000000000000000000000000000}
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    OnClick = ScanOnceSpeedButtonClick
  end
  object Label40: TLabel
    Left = 0
    Top = 331
    Width = 95
    Height = 15
    Alignment = taRightJustify
    Caption = 'Trace smoothing '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object CurveSmoothingLabel: TLabel
    Left = 101
    Top = 330
    Width = 24
    Height = 15
    Caption = 'XXX'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object OutputPowerLabel2: TLabel
    Left = 101
    Top = 399
    Width = 28
    Height = 15
    Caption = 'XXXX'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object OutputPowerLabel1: TLabel
    Left = 18
    Top = 400
    Width = 77
    Height = 15
    Alignment = taRightJustify
    Caption = 'Output power '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object ConnectDisconnectSpeedButton: TSpeedButton
    Left = 8
    Top = 31
    Width = 101
    Height = 21
    Cursor = crHandPoint
    Hint = 'Connect/Disconnect to/from device'
    AllowAllUp = True
    GroupIndex = 5
    Caption = 'Disconnected'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    OnClick = ConnectDisconnectSpeedButtonClick
  end
  object MemorySpeedButton0: TSpeedButton
    Left = 5
    Top = 449
    Width = 55
    Height = 25
    Cursor = crHandPoint
    Hint = 'Right click for options'
    AllowAllUp = True
    GroupIndex = 6
    Down = True
    Caption = 'Live'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    PopupMenu = MemoryPopupMenu
    ShowHint = True
    OnClick = MemorySpeedButtonClick
    OnMouseDown = MemorySpeedButtonMouseDown
  end
  object RecordSpeedButton: TSpeedButton
    Left = 169
    Top = 57
    Width = 51
    Height = 27
    Cursor = crHandPoint
    Hint = 'Records all scans to '#39'.s2p'#39' files in the the recording directory'
    AllowAllUp = True
    GroupIndex = 7
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000000B0000000B00001000000010000000008080000000
      C000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      00000000000000000000000000FFFF0000000000FF1111FF0000000F11111111
      F000000F11111111F00000F1111111111F0000F1111111111F0000F111111111
      1F0000F1111111111F00000F11111111F000000FF1111111F0000000FF1111FF
      0000000000FFFF00000000000000000000000000000000000000}
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    OnClick = RecordSpeedButtonClick
    OnMouseDown = RecordSpeedButtonMouseDown
  end
  object ScanSpeedButton: TSpeedButton
    Left = 116
    Top = 57
    Width = 51
    Height = 27
    Cursor = crHandPoint
    Hint = 'Continuous scan'
    AllowAllUp = True
    GroupIndex = 8
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000940E0000940E0000100000001000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FAFAFA00FFFFFF00111111111111
      11111111111111111111EEEEEEEEEEEEEEEEE00000000000000EE00000000000
      000EE0EEEEEEEEEEEE0EE0E1111111111E0EE0E111EEE1111E0EE0E111E00E11
      1E0EE0EEEEE000EEEE0EE00000000000000EE00000000000000EEEEEEEE000EE
      EEEE111111E00E111111111111EEE11111111111111111111111}
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    OnClick = ScanSpeedButtonClick
  end
  object InfoPanelLabel2: TLabel
    Left = 810
    Top = 35
    Width = 24
    Height = 15
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    Caption = 'Info '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object EDelayLabel: TLabel
    Left = 74
    Top = 259
    Width = 67
    Height = 15
    Alignment = taRightJustify
    Caption = 'e-delay (ps) '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object S21OffsetLabel: TLabel
    Left = 60
    Top = 283
    Width = 81
    Height = 15
    Alignment = taRightJustify
    Caption = 'S21 offset (dB) '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object Label5: TLabel
    Left = 26
    Top = 354
    Width = 69
    Height = 15
    Alignment = taRightJustify
    Caption = 'Median filter '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object MedianFilterLabel: TLabel
    Left = 101
    Top = 353
    Width = 16
    Height = 15
    Caption = 'XX'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object MemorySpeedButton4: TSpeedButton
    Left = 184
    Top = 449
    Width = 36
    Height = 25
    Cursor = crHandPoint
    Hint = 'Right click for options'
    AllowAllUp = True
    GroupIndex = 10
    Caption = 'M4'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    PopupMenu = MemoryPopupMenu
    ShowHint = True
    OnClick = MemorySpeedButtonClick
    OnMouseDown = MemorySpeedButtonMouseDown
    OnMouseUp = MemorySpeedButtonMouseUp
  end
  object EnableNormaliseSpeedButton: TSpeedButton
    Left = 114
    Top = 421
    Width = 106
    Height = 25
    Cursor = crHandPoint
    AllowAllUp = True
    GroupIndex = 11
    Caption = 'Enable Norm'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    OnClick = EnableNormaliseSpeedButtonClick
  end
  object MovingAverageFilterLabel: TLabel
    Left = 101
    Top = 307
    Width = 16
    Height = 15
    Caption = 'XX'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label18: TLabel
    Left = 339
    Top = 35
    Width = 20
    Height = 15
    Alignment = taRightJustify
    Caption = 'Cal '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object CWModeSpeedButton: TSpeedButton
    Left = 39
    Top = 184
    Width = 42
    Height = 21
    Cursor = crHandPoint
    Hint = 'CW mode'
    AllowAllUp = True
    GroupIndex = 9
    Caption = 'CW'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    OnClick = CWModeSpeedButtonClick
  end
  object TimeAverageLevelTrackBar: TTrackBar
    Left = 136
    Top = 304
    Width = 90
    Height = 21
    Cursor = crHandPoint
    Ctl3D = True
    Max = 6
    ParentCtl3D = False
    ParentShowHint = False
    PageSize = 1
    ShowHint = True
    TabOrder = 29
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = TimeAverageLevelTrackBarChange
  end
  object CloseBitBtn: TBitBtn
    Left = 840
    Top = 4
    Width = 36
    Height = 21
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
    TabOrder = 11
    Visible = False
    OnClick = CloseBitBtnClick
  end
  object SerialPortBaudrateComboBox: TComboBox
    Left = 220
    Top = 4
    Width = 80
    Height = 23
    Cursor = crHandPoint
    Hint = 'Serial port baudrate - ignored in USB VCP mode'
    AutoDropDown = True
    AutoCloseUp = True
    Style = csDropDownList
    DropDownCount = 40
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    OnChange = SerialPortBaudrateComboBoxChange
  end
  object DeviceComboBox: TComboBox
    Left = 8
    Top = 4
    Width = 208
    Height = 23
    Cursor = crHandPoint
    Hint = 'Device to connect too'
    AutoDropDown = True
    AutoCloseUp = True
    Style = csDropDownList
    DropDownCount = 40
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnChange = DeviceComboBoxChange
    OnClick = DeviceComboBoxClick
    OnDropDown = DeviceComboBoxDropDown
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 642
    Width = 884
    Height = 19
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBtnText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    Panels = <
      item
        Alignment = taCenter
        Width = 180
      end
      item
        Alignment = taCenter
        Width = 150
      end
      item
        Alignment = taCenter
        Width = 130
      end
      item
        Alignment = taCenter
        Width = 130
      end
      item
        Alignment = taCenter
        Width = 130
      end
      item
        Alignment = taCenter
        Width = 160
      end
      item
        Alignment = taCenter
        Width = 220
      end
      item
        Alignment = taCenter
        Width = 50
      end>
    ParentShowHint = False
    ShowHint = False
    UseSystemFont = False
    StyleElements = [seClient, seBorder]
    OnMouseDown = StatusBarMouseDown
  end
  object VelocityFactorComboBox: TComboBox
    Left = 684
    Top = 4
    Width = 131
    Height = 23
    Cursor = crHandPoint
    Hint = ' TDR \n Velocity factor '
    AutoDropDown = True
    AutoCloseUp = True
    Style = csDropDownList
    Anchors = [akLeft, akTop, akRight]
    DropDownCount = 40
    ParentShowHint = False
    ShowHint = True
    TabOrder = 14
    OnChange = VelocityFactorComboBoxChange
  end
  object VelocityFactorEdit: TEdit
    Left = 821
    Top = 4
    Width = 55
    Height = 23
    Alignment = taCenter
    Anchors = [akTop, akRight]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 15
    Text = '0.66'
    TextHint = 'Enter velocity factor'
    OnChange = VelocityFactorEditChange
  end
  object TDRWindowTrackBar: TTrackBar
    Left = 136
    Top = 373
    Width = 90
    Height = 21
    Cursor = crHandPoint
    Hint = 'TDR FFT window amount'
    Max = 20
    ParentShowHint = False
    PageSize = 1
    Position = 10
    ShowHint = True
    TabOrder = 31
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = TDRWindowTrackBarChange
  end
  object CWMHzEdit: TEdit
    Left = 144
    Top = 184
    Width = 76
    Height = 23
    Alignment = taCenter
    TabOrder = 24
    Text = '150'
    TextHint = 'Enter MHz'
    OnChange = FreqEditChange
    OnKeyDown = CWMHzEditKeyDown
  end
  object StopMHzEdit: TEdit
    Left = 144
    Top = 112
    Width = 76
    Height = 23
    Alignment = taCenter
    Ctl3D = True
    ParentCtl3D = False
    TabOrder = 21
    Text = '900'
    TextHint = 'Enter MHz'
    OnChange = FreqEditChange
    OnKeyDown = StopMHzEditKeyDown
  end
  object SpanMHzEdit: TEdit
    Left = 144
    Top = 160
    Width = 76
    Height = 23
    Alignment = taCenter
    TabOrder = 23
    Text = '10'
    TextHint = 'Enter MHz'
    OnChange = FreqEditChange
    OnKeyDown = SpanMHzEditKeyDown
  end
  object CenterMHzEdit: TEdit
    Left = 144
    Top = 136
    Width = 76
    Height = 23
    Alignment = taCenter
    TabOrder = 22
    Text = '145'
    TextHint = 'Enter MHz'
    OnChange = FreqEditChange
    OnKeyDown = CenterMHzEditKeyDown
  end
  object StartMHzEdit: TEdit
    Left = 144
    Top = 88
    Width = 76
    Height = 23
    Alignment = taCenter
    ParentShowHint = False
    ShowHint = True
    TabOrder = 20
    Text = '50k'
    TextHint = 'Enter MHz'
    OnChange = FreqEditChange
    OnKeyDown = StartMHzEditKeyDown
  end
  object Panel1: TPanel
    Left = 228
    Top = 57
    Width = 653
    Height = 563
    Anchors = [akLeft, akTop, akRight, akBottom]
    BevelOuter = bvNone
    DoubleBuffered = True
    FullRepaint = False
    ParentColor = True
    ParentDoubleBuffered = False
    ShowCaption = False
    TabOrder = 39
    object GraphPaintBox: TPaintBox
      Left = 0
      Top = 0
      Width = 358
      Height = 536
      Cursor = crArrow
      Hint = 'No hint'
      Align = alClient
      Color = clGray
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
      ParentShowHint = False
      PopupMenu = GraphPopupMenu
      ShowHint = False
      Touch.InteractiveGestures = [igZoom, igPan, igPressAndTap]
      OnDblClick = GraphPaintBoxDblClick
      OnGesture = GraphPaintBoxGesture
      OnMouseDown = GraphPaintBoxMouseDown
      OnMouseEnter = GraphPaintBoxMouseEnter
      OnMouseLeave = GraphPaintBoxMouseLeave
      OnMouseMove = GraphPaintBoxMouseMove
      OnMouseUp = GraphPaintBoxMouseUp
      OnPaint = GraphPaintBoxPaint
      ExplicitLeft = 188
      ExplicitWidth = 170
      ExplicitHeight = 543
    end
    object InfoPanel: TPanel
      Left = 358
      Top = 0
      Width = 295
      Height = 536
      Align = alRight
      BevelOuter = bvLowered
      Caption = 'InfoPanel'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      ParentColor = True
      ParentFont = False
      TabOrder = 0
      VerticalAlignment = taAlignTop
      object Label14: TLabel
        Left = 70
        Top = 25
        Width = 59
        Height = 15
        Alignment = taRightJustify
        Caption = 'Frequency '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label16: TLabel
        Left = 51
        Top = 87
        Width = 78
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Real Imag '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label23: TLabel
        Left = 61
        Top = 104
        Width = 68
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Series R '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label19: TLabel
        Left = 61
        Top = 121
        Width = 68
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Series X '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label22: TLabel
        Left = 62
        Top = 138
        Width = 67
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Series L '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label25: TLabel
        Left = 60
        Top = 155
        Width = 69
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Series C '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label27: TLabel
        Left = 55
        Top = 172
        Width = 74
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Parallel R '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label29: TLabel
        Left = 55
        Top = 189
        Width = 74
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Parallel X '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label35: TLabel
        Left = 56
        Top = 206
        Width = 73
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Parallel L '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label31: TLabel
        Left = 54
        Top = 223
        Width = 75
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Parallel C '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label20: TLabel
        Left = 51
        Top = 420
        Width = 78
        Height = 15
        Alignment = taRightJustify
        Caption = 'S21 Real Imag '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label53: TLabel
        Left = 47
        Top = 437
        Width = 82
        Height = 15
        Alignment = taRightJustify
        Caption = 'S21 Magnitude '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label51: TLabel
        Left = 79
        Top = 454
        Width = 50
        Height = 15
        Alignment = taRightJustify
        Caption = 'S21 Gain '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS21GainLabel: TLabel
        Left = 136
        Top = 454
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS21Label: TLabel
        Left = 136
        Top = 437
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS21RealImagLabel: TLabel
        Left = 136
        Top = 420
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11ParallelCLabel: TLabel
        Left = 136
        Top = 223
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11ParallelLLabel: TLabel
        Left = 136
        Top = 206
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11ParallelXLabel: TLabel
        Left = 136
        Top = 189
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11ParallelRLabel: TLabel
        Left = 136
        Top = 172
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11SeriesCLabel: TLabel
        Left = 136
        Top = 155
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11SeriesLLabel: TLabel
        Left = 136
        Top = 138
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11SeriesXLabel: TLabel
        Left = 136
        Top = 121
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11SeriesRLabel: TLabel
        Left = 136
        Top = 104
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11RealImagLabel: TLabel
        Left = 136
        Top = 87
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerFrequencyLabel: TLabel
        Left = 136
        Top = 25
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label37: TLabel
        Left = 39
        Top = 240
        Width = 90
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Return Loss '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label39: TLabel
        Left = 47
        Top = 257
        Width = 82
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Magnitude '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label47: TLabel
        Left = 88
        Top = 291
        Width = 41
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 |Z| '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label41: TLabel
        Left = 28
        Top = 274
        Width = 101
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Quality Factor '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label43: TLabel
        Left = 72
        Top = 308
        Width = 57
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Phase '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label46: TLabel
        Left = 76
        Top = 325
        Width = 53
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Polar '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label49: TLabel
        Left = 37
        Top = 342
        Width = 92
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Group Delay '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label34: TLabel
        Left = 74
        Top = 359
        Width = 55
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 VSWR '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label15: TLabel
        Left = 44
        Top = 376
        Width = 85
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Impedance '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11AdmittanceLabel1: TLabel
        Left = 41
        Top = 393
        Width = 88
        Height = 15
        Alignment = taRightJustify
        Caption = 'S11 Admittance '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label59: TLabel
        Left = 72
        Top = 471
        Width = 57
        Height = 15
        Alignment = taRightJustify
        Caption = 'S21 Phase '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label55: TLabel
        Left = 76
        Top = 488
        Width = 53
        Height = 15
        Alignment = taRightJustify
        Caption = 'S21 Polar '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label58: TLabel
        Left = 37
        Top = 505
        Width = 92
        Height = 15
        Alignment = taRightJustify
        Caption = 'S21 Group Delay '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS21GroupDelayLabel: TLabel
        Left = 136
        Top = 505
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS21PolarLabel: TLabel
        Left = 136
        Top = 488
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS21PhaseLabel: TLabel
        Left = 136
        Top = 471
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11ImpedanceLabel: TLabel
        Left = 136
        Top = 376
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11AdmittanceLabel2: TLabel
        Left = 136
        Top = 393
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11GroupDelayLabel: TLabel
        Left = 136
        Top = 342
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11PolarLabel: TLabel
        Left = 136
        Top = 325
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11PhaseLabel: TLabel
        Left = 136
        Top = 308
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11ZLabel: TLabel
        Left = 136
        Top = 291
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11QualityFactorLabel: TLabel
        Left = 136
        Top = 274
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11Label: TLabel
        Left = 136
        Top = 257
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11ReturnLossLabel: TLabel
        Left = 136
        Top = 240
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerS11VSWRLabel: TLabel
        Left = 136
        Top = 359
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Bevel1: TBevel
        Left = 24
        Top = 79
        Width = 245
        Height = 5
        Shape = bsTopLine
        Visible = False
      end
      object Bevel2: TBevel
        Left = 28
        Top = 413
        Width = 245
        Height = 5
        Shape = bsTopLine
        Visible = False
      end
      object Bevel3: TBevel
        Left = 28
        Top = 526
        Width = 245
        Height = 5
        Shape = bsTopLine
        Visible = False
      end
      object Label6: TLabel
        Left = 65
        Top = 42
        Width = 64
        Height = 15
        Alignment = taRightJustify
        Caption = 'Wavelength '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerWavelengthLabel1: TLabel
        Left = 136
        Top = 42
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object Label8: TLabel
        Left = 42
        Top = 59
        Width = 87
        Height = 15
        Alignment = taRightJustify
        Caption = '1/4 Wavelength '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
      object MarkerWavelengthLabel2: TLabel
        Left = 136
        Top = 59
        Width = 70
        Height = 15
        Caption = 'xxxxxxxxxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        ParentFont = False
      end
    end
    object Panel2: TPanel
      Left = 0
      Top = 536
      Width = 653
      Height = 27
      Margins.Left = 0
      Margins.Right = 0
      Align = alBottom
      BevelOuter = bvNone
      Caption = 'Panel2'
      ParentColor = True
      ShowCaption = False
      TabOrder = 1
      object HistoryTrackBar: TTrackBar
        Left = 46
        Top = 0
        Width = 607
        Height = 27
        Cursor = crHandPoint
        Hint = 'History position'
        Align = alRight
        Anchors = [akLeft, akTop, akRight, akBottom]
        Max = 0
        Min = -1023
        ParentShowHint = False
        PageSize = 64
        Frequency = 32
        SelStart = -500
        ShowHint = True
        TabOrder = 0
        ThumbLength = 23
        TickMarks = tmBoth
        TickStyle = tsNone
        StyleElements = [seFont, seBorder]
        OnChange = HistoryTrackBarChange
      end
      object BitBtn1: TBitBtn
        Left = 0
        Top = 1
        Width = 31
        Height = 25
        Cursor = crHandPoint
        Hint = 'Clear the history buffer'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Lucida Sans Unicode'
        Font.Style = []
        Glyph.Data = {
          F6000000424DF600000000000000760000002800000010000000100000000100
          04000000000080000000940E0000940E0000100000001000000000000000FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FAFAFA00FFFFFF0011EEEEEEEEEE
          EE111E000000000000E11E0EE0EEEE0EE0E11E0EE0EEEE0EE0E11E0EE0EEEE0E
          E0E11E0EE0EEEE0EE0E11E0EE0EEEE0EE0E11E0EE0EEEE0EE0E11E0EE0EEEE0E
          E0E11E0EE0EEEE0EE0E11E0EE0EEEE0EE0E1E00000000000000EEEEEEEEEEEEE
          EEEEE00000000000000E1E000000000000E111EEEE0000EEEE11}
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnClick = BitBtn1Click
      end
    end
    object GLPanel: TPanel
      Left = 0
      Top = 0
      Width = 358
      Height = 536
      Align = alClient
      BevelOuter = bvLowered
      Caption = 'GLPanel'
      Color = clNone
      FullRepaint = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ParentBackground = False
      ParentFont = False
      PopupMenu = GraphPopupMenu
      ShowCaption = False
      TabOrder = 2
      Visible = False
      StyleElements = []
      OnDblClick = GLPanelDblClick
      OnEndDrag = GLPanelEndDrag
      OnGesture = GLPanelGesture
      OnMouseDown = GLPanelMouseDown
      OnMouseEnter = GLPanelMouseEnter
      OnMouseLeave = GLPanelMouseLeave
      OnMouseMove = GLPanelMouseMove
      OnMouseUp = GLPanelMouseUp
      OnResize = GLPanelResize
    end
  end
  object FreqBandEnableToggleSwitch: TToggleSwitch
    Left = 507
    Top = 31
    Width = 40
    Height = 20
    Cursor = crHandPoint
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    FrameColor = clBtnFace
    ParentFont = False
    ShowStateCaption = False
    State = tssOn
    SwitchWidth = 40
    TabOrder = 9
    ThumbColor = clNavy
    ThumbWidth = 20
    OnClick = FreqBandEnableToggleSwitchClick
  end
  object SweepNameEdit: TEdit
    Left = 684
    Top = 31
    Width = 117
    Height = 23
    Hint = 'Sweep name - added to top of saved image'
    Anchors = [akLeft, akTop, akRight]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 16
    TextHint = 'Graph title'
    OnChange = SweepNameEditChange
  end
  object NumberOfPointsComboBox: TComboBox
    Left = 144
    Top = 208
    Width = 76
    Height = 23
    Cursor = crHandPoint
    AutoDropDown = True
    AutoCloseUp = True
    Style = csDropDownList
    Ctl3D = True
    DropDownCount = 30
    ParentCtl3D = False
    TabOrder = 26
    OnChange = NumberOfPointsComboBoxChange
  end
  object PointBandwidthHzComboBox: TComboBox
    Left = 144
    Top = 232
    Width = 76
    Height = 23
    Cursor = crHandPoint
    AutoDropDown = True
    AutoCloseUp = True
    Ctl3D = True
    DropDownCount = 30
    ParentCtl3D = False
    TabOrder = 25
    TextHint = 'Enter Hz'
    OnChange = PointBandwidthHzComboBoxChange
    OnKeyDown = PointBandwidthHzComboBoxKeyDown
  end
  object LCMatchingToggleSwitch: TToggleSwitch
    Left = 630
    Top = 31
    Width = 40
    Height = 20
    Cursor = crHandPoint
    Hint = 
      'Uses selected marker or mouse position to calculate approximate ' +
      'LC matching values'
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    FrameColor = clBtnFace
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    ShowStateCaption = False
    SwitchWidth = 40
    TabOrder = 10
    ThumbColor = clNavy
    ThumbWidth = 20
    Visible = False
    OnClick = LCMatchingToggleSwitchClick
  end
  object CurveSmoothingTrackBar: TTrackBar
    Left = 136
    Top = 327
    Width = 90
    Height = 21
    Cursor = crHandPoint
    Max = 11
    ParentShowHint = False
    PageSize = 1
    ShowHint = True
    TabOrder = 30
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = CurveSmoothingTrackBarChange
  end
  object MarkersGroupBox: TGroupBox
    Left = 7
    Top = 537
    Width = 213
    Height = 84
    Anchors = [akLeft, akTop, akBottom]
    Caption = ' Frequency markers '
    Color = clBtnFace
    Ctl3D = False
    ParentColor = False
    ParentCtl3D = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 38
    DesignSize = (
      213
      84)
    object MarkerListView: TListView
      Left = 8
      Top = 36
      Width = 198
      Height = 39
      Cursor = crHandPoint
      Anchors = [akLeft, akTop, akRight, akBottom]
      Columns = <
        item
          MaxWidth = 1
          MinWidth = 1
          Width = 1
        end
        item
          Alignment = taCenter
          Caption = 'No.'
          MaxWidth = 40
          MinWidth = 40
          Width = 40
        end
        item
          Caption = 'Frequency'
          MaxWidth = 100
          MinWidth = 100
          Width = 100
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      GridLines = True
      OwnerData = True
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      ShowColumnHeaders = False
      TabOrder = 3
      ViewStyle = vsReport
      Visible = False
      OnChange = MarkerListViewChange
      OnData = MarkerListViewData
      OnKeyDown = MarkerListViewKeyDown
    end
    object MarkerListBox: TListBox
      Left = 8
      Top = 42
      Width = 198
      Height = 34
      Cursor = crHandPoint
      Anchors = [akLeft, akTop, akRight, akBottom]
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      ItemHeight = 15
      ParentFont = False
      PopupMenu = MarkersListBoxPopupMenu
      TabOrder = 2
      OnClick = MarkerListBoxClick
      OnDblClick = MarkerListBoxDblClick
      OnKeyDown = MarkerListBoxKeyDown
      OnMouseDown = MarkerListBoxMouseDown
    end
    object ShowMarkersOnGraphToggleSwitch: TToggleSwitch
      Left = 8
      Top = 16
      Width = 40
      Height = 20
      Cursor = crHandPoint
      Hint = 'Marker display mode'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      FrameColor = clBtnFace
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      ShowStateCaption = False
      SwitchWidth = 40
      TabOrder = 0
      ThumbColor = clNavy
      ThumbWidth = 20
      OnClick = ShowMarkersOnGraphToggleSwitchClick
    end
    object ShowMarkerTextToggleSwitch: TToggleSwitch
      Left = 54
      Top = 16
      Width = 40
      Height = 20
      Cursor = crHandPoint
      Hint = 'Show marker text'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = []
      FrameColor = clBtnFace
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      ShowStateCaption = False
      State = tssOn
      SwitchWidth = 40
      TabOrder = 1
      ThumbColor = clNavy
      ThumbWidth = 20
      OnClick = ShowMarkerTextToggleSwitchClick
    end
    object DeleteFrequencyMarkersBitBtn: TBitBtn
      Left = 175
      Top = 12
      Width = 31
      Height = 24
      Cursor = crHandPoint
      Hint = 'Delete all the markers'
      Anchors = [akTop, akRight]
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        04000000000080000000940E0000940E0000100000001000000000000000FFFF
        FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
        FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FAFAFA00FFFFFF0011EEEEEEEEEE
        EE111E000000000000E11E0EE0EEEE0EE0E11E0EE0EEEE0EE0E11E0EE0EEEE0E
        E0E11E0EE0EEEE0EE0E11E0EE0EEEE0EE0E11E0EE0EEEE0EE0E11E0EE0EEEE0E
        E0E11E0EE0EEEE0EE0E11E0EE0EEEE0EE0E1E00000000000000EEEEEEEEEEEEE
        EEEEE00000000000000E1E000000000000E111EEEE0000EEEE11}
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
      OnClick = DeleteFrequencyMarkersBitBtnClick
    end
  end
  object OutputPowerTrackBar: TTrackBar
    Left = 136
    Top = 396
    Width = 90
    Height = 21
    Cursor = crHandPoint
    Max = 3
    Min = -1
    ParentShowHint = False
    PageSize = 1
    Position = -1
    ShowHint = True
    TabOrder = 32
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = OutputPowerTrackBarChange
  end
  object SetScanRangeToVNAScanRangeBitBtn: TBitBtn
    Left = 8
    Top = 88
    Width = 25
    Height = 142
    Cursor = crHandPoint
    Hint = 'Set the scan range to match the VNA'#39's scan range'
    Glyph.Data = {
      6E020000424D6E020000000000007600000028000000100000003F0000000100
      040000000000F8010000F00A0000F00A00001000000010000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000100000000000000011000000
      0000000011100000000000001111000000000000111110000000000011111100
      0000000011111000000000001111000000000000111000000000000011000000
      0000000010000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000100000000000000011000000
      0000000011100000000000001111000000000000111110000000000011111100
      0000000011111000000000001111000000000000111000000000000011000000
      0000000010000000000000000000000000000000000000000000000000000000
      0000000000000000000000001000000000000000110000000000000011100000
      0000000011110000000000001111100000000000111111000000000011111000
      0000000011110000000000001110000000000000110000000000000010000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000001000000000000000110000000000000011100000
      0000000011110000000000001111100000000000111111000000000011111000
      0000000011110000000000001110000000000000110000000000000010000000
      000000000000000000000000000000000000}
    ParentShowHint = False
    ShowHint = True
    TabOrder = 19
    OnClick = SetScanRangeToVNAScanRangeBitBtnClick
  end
  object NewGraphBitBtn: TBitBtn
    Left = 631
    Top = 4
    Width = 44
    Height = 21
    Cursor = crHandPoint
    Hint = 'New graph'
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000220B0000220B00001000000010000000000000000000
      800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00555555555555
      5555555555555555555555000000000005555555555555555555550FFFFFFFFF
      FF55555FFFFFFFFFFF55550F555555555555555F777777777775550F55555555
      5555555F755555555555550F555555555555555F755555555555550F55555555
      5555555F755555555555550F555555555555555F755555555555550F55555555
      5555555F755555555555550F0F5555550F55555F7F7555555F55550F50F555F0
      F555555F75F755F5F755550F550F0F0F5555555F755F7F7F7555550F5550F550
      5555555F7555F7575555550F555505555555555F755575555555555F55555555
      5555555F55555555555555555555555555555555555555555555}
    NumGlyphs = 2
    ParentShowHint = False
    ShowHint = True
    TabOrder = 17
    OnClick = NewGraphBitBtnClick
  end
  object UploadFirmwareBitBtn: TBitBtn
    Left = 355
    Top = 4
    Width = 44
    Height = 21
    Cursor = crHandPoint
    Hint = 'Upload VNA firmware'
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000200B0000200B00001000000010000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000FFFFFFFFFFF000000FFFFFFFFFFF000F1111111111
      1F0000F11111111111F000F111111111F000000F111111111F00000F1111111F
      00000000F1111111F0000000F11111F0000000000F11111F000000000F111F00
      0000000000F111F00000000000F1F00000000000000F1F0000000000000F0000
      000000000000F000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000}
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = UploadFirmwareBitBtnClick
  end
  object SettingsBitBtn: TBitBtn
    Left = 309
    Top = 4
    Width = 44
    Height = 21
    Cursor = crHandPoint
    Hint = 'Settings'
    Glyph.Data = {
      36050000424D3605000000000000360400002800000010000000100000000100
      08000000000000010000D40E0000D40E00000001000000010000000000000101
      0100020202000303030004040400050505000606060007070700080808000909
      09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
      1100121212001313130014141400151515001616160017171700181818001919
      19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
      2100222222002323230024242400252525002626260027272700282828002929
      29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
      3100323232003333330034343400353535003636360037373700383838003939
      39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
      4100424242004343430044444400454545004646460047474700484848004949
      49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
      5100525252005353530054545400555555005656560057575700585858005959
      59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
      6100626262006363630064646400656565006666660067676700686868006969
      69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
      7100727272007373730074747400757575007676760077777700787878007979
      79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
      8100828282008383830084848400858585008686860087878700888888008989
      89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
      9100929292009393930094949400959595009696960097979700989898009999
      99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
      A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
      A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
      B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
      B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
      C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
      C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
      D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
      D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
      E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
      E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
      F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
      F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
      F60000F6FFFFFFFFFFFFFFFFF6F6F6FFF60000F6FFF6F6F6FFFFFFF6F600F6F6
      F60000F6F6F600F6F6FFFFF6000000F618000000F6000000F6FFFFF6F6000000
      0000000000001BF6F6FFFFFFF6F60006F6F6F6F60600F6F6FFFFF6F6F60000F6
      F60000F6F60000F6F6F60000000000F600000000F600000000000000000000F6
      00000000F60000000000F6F6F60000F6F60000F6F60000F6F6F6FFFFF6F60006
      F6F6F6F60000F6F6FFFFFFF6F600000000000000000000F6F6FFFFF6000015F6
      12000012F6000000F6FFFFF6F600F6F6F60000F6F6F600F6F6FFFFFFF6F6F6FF
      F60000F6FFF6F6F6FFFFFFFFFFFFFFFFF60000F6FFFFFFFFFFFF}
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnClick = SettingsBitBtnClick
  end
  object BatteryVoltageBitBtn: TBitBtn
    Left = 401
    Top = 4
    Width = 44
    Height = 21
    Cursor = crHandPoint
    Hint = 'VNA battery voltage'
    Glyph.Data = {
      36050000424D3605000000000000360400002800000010000000100000000100
      08000000000000010000D40E0000D40E00000001000000010000000000000101
      0100020202000303030004040400050505000606060007070700080808000909
      09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
      1100121212001313130014141400151515001616160017171700181818001919
      19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
      2100222222002323230024242400252525002626260027272700282828002929
      29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
      3100323232003333330034343400353535003636360037373700383838003939
      39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
      4100424242004343430044444400454545004646460047474700484848004949
      49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
      5100525252005353530054545400555555005656560057575700585858005959
      59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
      6100626262006363630064646400656565006666660067676700686868006969
      69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
      7100727272007373730074747400757575007676760077777700787878007979
      79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
      8100828282008383830084848400858585008686860087878700888888008989
      89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
      9100929292009393930094949400959595009696960097979700989898009999
      99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
      A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
      A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
      B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
      B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
      C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
      C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
      D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
      D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
      E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
      E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
      F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
      F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFF5F5F5F5F5F5F5F5F5F5F5F5F5F5FFFFF54800000000
      00000000000048F5FFFFF500F5F5F5F5F5F5F5F5F5F500F5F5F5F500F5000000
      000000F5F5F5000048F5F500F5000000000000F5F5F500F500F5F500F5000000
      000000F5F5F500F500F5F500F5000000000000F5F5F5000042F5F500F5F5F5F5
      F5F5F5F5F5F500F5F5F5F5420000000000000000000042F5FFFFF5F5F5F5F5F5
      F5F5F5F5F5F5F5F5FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnClick = BatteryVoltageBitBtnClick
  end
  object CaptureVNAScreenBitBtn: TBitBtn
    Left = 447
    Top = 4
    Width = 44
    Height = 21
    Cursor = crHandPoint
    Hint = 'Capture VNA screen image'
    Glyph.Data = {
      36050000424D3605000000000000360400002800000010000000100000000100
      08000000000000010000100B0000100B00000001000000010000000000000101
      0100020202000303030004040400050505000606060007070700080808000909
      09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
      1100121212001313130014141400151515001616160017171700181818001919
      19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
      2100222222002323230024242400252525002626260027272700282828002929
      29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
      3100323232003333330034343400353535003636360037373700383838003939
      39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
      4100424242004343430044444400454545004646460047474700484848004949
      49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
      5100525252005353530054545400555555005656560057575700585858005959
      59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
      6100626262006363630064646400656565006666660067676700686868006969
      69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
      7100727272007373730074747400757575007676760077777700787878007979
      79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
      8100828282008383830084848400858585008686860087878700888888008989
      89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
      9100929292009393930094949400959595009696960097979700989898009999
      99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
      A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
      A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
      B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
      B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
      C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
      C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
      D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
      D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
      E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
      E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
      F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
      F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEEEEEEEEEE
      EEEEEEEEEEEEEEEEEEFFEEEE000000000000000000000000EEEEEE0000000000
      000000000000000000EEEE0000EEEEEEEEEEEEEEEEEEEE0000EEEE0000EEEEEE
      EE0000EEEEEEEE0000EEEE0000EEEEEE00EEEE00EEEEEE0000EEEE0000EEEE1F
      EEFFFFEE00EEEE0000EEEE0000EEEE00EEFFFFEE00EEEE0000EEEE0000EEEEEE
      00EEEE00EEEEEE0000EEEE0000EEEEEEEE0000EEEEEEEE0000EEEEEE00000000
      EEEEEEEE00000000EEEEFFEEEEEE0000000000000000EEEEEEFFFFFFFFFFEEEE
      00000000EEEEFFFFFFFFFFFFFFFFFFEEEEEEEEEEEEFFFFFFFFFF}
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
    OnClick = CaptureVNAScreenBitBtnClick
  end
  object VNACommsBitBtn: TBitBtn
    Left = 493
    Top = 4
    Width = 44
    Height = 21
    Cursor = crHandPoint
    Hint = 'VNA comms'
    Glyph.Data = {
      36050000424D3605000000000000360400002800000010000000100000000100
      08000000000000010000000B0000000B00000001000000010000000000000101
      0100020202000303030004040400050505000606060007070700080808000909
      09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
      1100121212001313130014141400151515001616160017171700181818001919
      19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
      2100222222002323230024242400252525002626260027272700282828002929
      29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
      3100323232003333330034343400353535003636360037373700383838003939
      39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
      4100424242004343430044444400454545004646460047474700484848004949
      49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
      5100525252005353530054545400555555005656560057575700585858005959
      59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
      6100626262006363630064646400656565006666660067676700686868006969
      69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
      7100727272007373730074747400757575007676760077777700787878007979
      79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
      8100828282008383830084848400858585008686860087878700888888008989
      89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
      9100929292009393930094949400959595009696960097979700989898009999
      99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
      A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
      A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
      B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
      B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
      C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
      C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
      D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
      D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
      E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
      E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
      F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
      F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFEEEEEEEEFFFFFFFFFFFFFFFFFFFFFFEEEE0000EEFFFFFFFFFFFFFFFF
      FFFFEEEE000000EEFFFFFFFFFFFFFFFFFFEEEE00000000EEFFFFEEEEEEEEEEFF
      EEEE0000EEEEEEEEEEFFEE000000EEEEEE0000EEEEEE0000EEEEEE0000000000
      000000000000000000EEEE0000000000000000000000000000EEEE000000EEEE
      0000EEEEEEEE0000EEEEEEEEEEEEEEEEEE0000EE00EEEEEEEEFFFFFFFFFFFFFF
      EEEE00000000EEFFFFFFFFFFFFFFFFFFFFEEEEEE00EEEEFFFFFFFFFFFFFFFFFF
      FFFFFFEEEEEEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
    ParentShowHint = False
    ShowHint = True
    TabOrder = 6
    OnClick = VNACommsBitBtnClick
  end
  object VNAUsartCommsBitBtn: TBitBtn
    Left = 539
    Top = 4
    Width = 44
    Height = 21
    Cursor = crHandPoint
    Hint = 'VNA usart comms'
    Glyph.Data = {
      36050000424D3605000000000000360400002800000010000000100000000100
      08000000000000010000000B0000000B00000001000000010000000000000101
      0100020202000303030004040400050505000606060007070700080808000909
      09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
      1100121212001313130014141400151515001616160017171700181818001919
      19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
      2100222222002323230024242400252525002626260027272700282828002929
      29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
      3100323232003333330034343400353535003636360037373700383838003939
      39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
      4100424242004343430044444400454545004646460047474700484848004949
      49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
      5100525252005353530054545400555555005656560057575700585858005959
      59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
      6100626262006363630064646400656565006666660067676700686868006969
      69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
      7100727272007373730074747400757575007676760077777700787878007979
      79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
      8100828282008383830084848400858585008686860087878700888888008989
      89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
      9100929292009393930094949400959595009696960097979700989898009999
      99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
      A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
      A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
      B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
      B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
      C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
      C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
      D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
      D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
      E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
      E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
      F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
      F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFEEEEEEEEFFFFFFFFFFFFFFFFFFFFFFEEEE0000EEFFFFFFFFFFFFFFFF
      FFFFEEEE000000EEFFFFFFFFFFFFFFFFFFEEEE00000000EEFFFFEEEEEEEEEEFF
      EEEE0000EEEEEEEEEEFFEE000000EEEEEE0000EEEEEE0000EEEEEE0000000000
      000000000000000000EEEE0000000000000000000000000000EEEE000000EEEE
      0000EEEEEEEE0000EEEEEEEEEEEEEEEEEE0000EE00EEEEEEEEFFFFFFFFFFFFFF
      EEEE00000000EEFFFFFFFFFFFFFFFFFFFFEEEEEE00EEEEFFFFFFFFFFFFFFFFFF
      FFFFFFEEEEEEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
    ParentShowHint = False
    ShowHint = True
    TabOrder = 7
    OnClick = VNAUsartCommsBitBtnClick
  end
  object CalibrationBitBtn: TBitBtn
    Left = 585
    Top = 4
    Width = 44
    Height = 21
    Cursor = crHandPoint
    Hint = 'Calibration'
    Glyph.Data = {
      36050000424D3605000000000000360400002800000010000000100000000100
      08000000000000010000E40E0000E40E00000001000000010000000000000101
      0100020202000303030004040400050505000606060007070700080808000909
      09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
      1100121212001313130014141400151515001616160017171700181818001919
      19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
      2100222222002323230024242400252525002626260027272700282828002929
      29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
      3100323232003333330034343400353535003636360037373700383838003939
      39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
      4100424242004343430044444400454545004646460047474700484848004949
      49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
      5100525252005353530054545400555555005656560057575700585858005959
      59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
      6100626262006363630064646400656565006666660067676700686868006969
      69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
      7100727272007373730074747400757575007676760077777700787878007979
      79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
      8100828282008383830084848400858585008686860087878700888888008989
      89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
      9100929292009393930094949400959595009696960097979700989898009999
      99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
      A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
      A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
      B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
      B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
      C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
      C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
      D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
      D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
      E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
      E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
      F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
      F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFEEEE
      00000000EEEEFFFFFFFFFFFFFFEE0000000000000000EEFFFFFFFFFFEE000000
      00000000000000EEFFFFFFEE0000EEEEEE0000EEEEEE0000EEFFEE0000EEEEEE
      EE0000EEEEEEEE0000EEEE0000EEEE000000000000EEEE0000EE000000EE0000
      EE0000EE0000EE00000000000000000000000000000000000000000000000000
      00000000000000000000000000EE0000EE0000EE0000EE000000EE0000EEEE00
      0000000000EEEE0000EEEE0000EEEEEEEE0000EEEEEEEE0000EEFFEE0000EEEE
      EE0000EEEEEE0000EEFFFFFFEE00000000000000000000EEFFFFFFFFFFEE0000
      000000000000EEFFFFFFFFFFFFFFEEEE00000000EEEEFFFFFFFF}
    ParentShowHint = False
    ShowHint = True
    TabOrder = 8
    OnClick = CalibrationBitBtnClick
  end
  object InfoPanelToggleSwitch: TToggleSwitch
    Left = 836
    Top = 31
    Width = 40
    Height = 20
    Cursor = crHandPoint
    Anchors = [akTop, akRight]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    FrameColor = clBtnFace
    ParentFont = False
    ShowStateCaption = False
    State = tssOn
    SwitchWidth = 40
    TabOrder = 18
    ThumbColor = clNavy
    ThumbWidth = 20
    OnClick = InfoPanelToggleSwitchClick
  end
  object SetNormaliseButton: TButton
    Left = 5
    Top = 421
    Width = 106
    Height = 25
    Cursor = crHandPoint
    Caption = 'Set Norm'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 33
    OnClick = SetNormaliseButtonClick
  end
  object EDelayEdit: TEdit
    Left = 144
    Top = 256
    Width = 76
    Height = 23
    Alignment = taCenter
    TabOrder = 27
    Text = '0'
    OnChange = EDelayEditChange
    OnKeyDown = EDelayEditKeyDown
  end
  object S21OffsetEdit: TEdit
    Left = 144
    Top = 280
    Width = 76
    Height = 23
    Alignment = taCenter
    TabOrder = 28
    Text = '0'
    OnChange = S21OffsetEditChange
    OnKeyDown = S21OffsetEditKeyDown
  end
  object TCPIPAddressEdit: TEdit
    Left = 112
    Top = 31
    Width = 153
    Height = 23
    Hint = 'TCP IP address'
    Alignment = taCenter
    Ctl3D = True
    ParentCtl3D = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 12
    OnChange = TCPIPAddressPortEditChange
  end
  object TCPIPPortEdit: TEdit
    Left = 268
    Top = 31
    Width = 61
    Height = 23
    Hint = 'TCP IP port'
    Alignment = taCenter
    Ctl3D = True
    MaxLength = 5
    NumbersOnly = True
    ParentCtl3D = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 13
    OnChange = TCPIPAddressPortEditChange
  end
  object MedianFilterTrackBar: TTrackBar
    Left = 136
    Top = 350
    Width = 90
    Height = 21
    Cursor = crHandPoint
    Hint = 'Median trace filter'
    Max = 6
    ParentShowHint = False
    PageSize = 1
    ShowHint = True
    TabOrder = 41
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = MedianFilterTrackBarChange
  end
  object TraceLPFTrackBar: TTrackBar
    Left = 1
    Top = 280
    Width = 58
    Height = 21
    Cursor = crHandPoint
    Hint = 'Fast smooth test'
    Max = 20
    ParentShowHint = False
    PageSize = 1
    ShowHint = True
    TabOrder = 42
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    Visible = False
    OnChange = TraceLPFTrackBarChange
  end
  object SaveGraphImageBitBtn: TBitBtn
    Left = 78
    Top = 477
    Width = 69
    Height = 25
    Cursor = crHandPoint
    Hint = 'Save graph image'
    Caption = 'Graph'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000230B0000230B00001000000010000000AD633100B96B
      3100C2784400CE8C6300CF987300DE9C7300DEA57300E7A57B0063C68C00D2B1
      9500EBBFA000F6DDCC00FFECE100FFF7EF00FFFBF700FFFFFF00F11111111111
      111F1BBEEEEEEEEEE4411B7E88888888E3411B7EB9B9B9B9E3411B7E88888888
      E3911B7EEEEEEEEEE4711B777777777774911B777777777774911B7777777777
      75911B733333333334A11B7DDDDDDDDDD4A11B7DD3DDDDDDD5A11B7DD3DDDDDD
      D6B11B7DD3DDDDDDDBA11BBDDDDDDDDDDA31F11111111111111F}
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 34
    OnClick = SaveGraphImageBitBtnClick
  end
  object SaveS1PFileBitBtn: TBitBtn
    Left = 5
    Top = 505
    Width = 69
    Height = 25
    Cursor = crHandPoint
    Hint = 'Save s1p file'
    Caption = 'S1P'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000230B0000230B00001000000010000000AD633100B96B
      3100C2784400CE8C6300CF987300DE9C7300DEA57300E7A57B0063C68C00D2B1
      9500EBBFA000F6DDCC00FFECE100FFF7EF00FFFBF700FFFFFF00F11111111111
      111F1BBEEEEEEEEEE4411B7E88888888E3411B7EB9B9B9B9E3411B7E88888888
      E3911B7EEEEEEEEEE4711B777777777774911B777777777774911B7777777777
      75911B733333333334A11B7DDDDDDDDDD4A11B7DD3DDDDDDD5A11B7DD3DDDDDD
      D6B11B7DD3DDDDDDDBA11BBDDDDDDDDDDA31F11111111111111F}
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 36
    OnClick = SaveS1PFileBitBtnClick
  end
  object Save2PFileBitBtn: TBitBtn
    Left = 78
    Top = 505
    Width = 69
    Height = 25
    Cursor = crHandPoint
    Hint = 'Save s2p file'
    Caption = 'S2P'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000230B0000230B00001000000010000000AD633100B96B
      3100C2784400CE8C6300CF987300DE9C7300DEA57300E7A57B0063C68C00D2B1
      9500EBBFA000F6DDCC00FFECE100FFF7EF00FFFBF700FFFFFF00F11111111111
      111F1BBEEEEEEEEEE4411B7E88888888E3411B7EB9B9B9B9E3411B7E88888888
      E3911B7EEEEEEEEEE4711B777777777774911B777777777774911B7777777777
      75911B733333333334A11B7DDDDDDDDDD4A11B7DD3DDDDDDD5A11B7DD3DDDDDD
      D6B11B7DD3DDDDDDDBA11BBDDDDDDDDDDA31F11111111111111F}
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 37
    OnClick = Save2PFileBitBtnClick
  end
  object SaveCSVFileBitBtn: TBitBtn
    Left = 151
    Top = 505
    Width = 69
    Height = 25
    Cursor = crHandPoint
    Hint = 'Save csv file'
    Caption = 'CSV'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000230B0000230B00001000000010000000AD633100B96B
      3100C2784400CE8C6300CF987300DE9C7300DEA57300E7A57B0063C68C00D2B1
      9500EBBFA000F6DDCC00FFECE100FFF7EF00FFFBF700FFFFFF00F11111111111
      111F1BBEEEEEEEEEE4411B7E88888888E3411B7EB9B9B9B9E3411B7E88888888
      E3911B7EEEEEEEEEE4711B777777777774911B777777777774911B7777777777
      75911B733333333334A11B7DDDDDDDDDD4A11B7DD3DDDDDDD5A11B7DD3DDDDDD
      D6B11B7DD3DDDDDDDBA11BBDDDDDDDDDDA31F11111111111111F}
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 35
    OnClick = SaveCSVFileBitBtnClick
  end
  object CopyGraphImageBitBtn: TBitBtn
    Left = 151
    Top = 477
    Width = 69
    Height = 25
    Cursor = crHandPoint
    Hint = 'Copy graph image'
    Caption = 'Graph'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      36050000424D3605000000000000360400002800000010000000100000000100
      08000000000000010000130B0000130B00000001000000010000FFFFFF000000
      0000FAFAFA000000C900FCFCFC00D2D2D200A9A9A900F6F6F6004B4B4B00E5E5
      E500E9E9E900999999006F6F6F00F1F1F1006C6C6C00ADADAD00D6D6D6002424
      24008181810018181800C5C6ED0011111100DEDEDE0067676700C2C2C2007474
      7400CACACA0038383800505050008A8A8A00A1A1A1009B9AE300918FE0004646
      D100D0D0F1002D2D2D005E5E5E009B9B9B0033333300DFDFF4005E5ED500B4B4
      B40085858500282828001414CB00F2F1FA002020200044444400696AD700A6A6
      E600AFB1E8007B7CDA002426CD003836CE008785DC000A07C8003F41CE007071
      D9005151D200D9DAF2003031D0002725CD009FA1E200BDBFEC00000000000000
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
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000090909090909000000000000000002
      0101010101010909000000000000000116020202020201090000000000000001
      0900000000000109000000000000090109000000000001090000000000010901
      0900000000000109000000000001090109000000000001090000000000010901
      0900000000000109000000000001090109000000000001090000000000010901
      0909090909090109000000000001092B01010101010101000000000000010909
      0909090909090000000000000001010101010101010000000000000000000000
      0000000000000000000000000000000000000000000000000000}
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 43
    OnClick = CopyGraphImageBitBtnClick
  end
  object StopScanBitBtn: TBitBtn
    Left = 8
    Top = 57
    Width = 51
    Height = 27
    Cursor = crHandPoint
    Hint = 'Stop scan'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000000B0000000B00001000000010000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000011111111
      0000000011111111000000001111111100000000111111110000000011111111
      0000000011111111000000001111111100000000111111110000000000000000
      0000000000000000000000000000000000000000000000000000}
    ParentFont = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 44
    OnClick = StopScanBitBtnClick
  end
  object SpareBitBtn: TBitBtn
    Left = 5
    Top = 477
    Width = 69
    Height = 25
    Cursor = crHandPoint
    Hint = 'Save form image'
    Caption = 'Form'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000230B0000230B00001000000010000000AD633100B96B
      3100C2784400CE8C6300CF987300DE9C7300DEA57300E7A57B0063C68C00D2B1
      9500EBBFA000F6DDCC00FFECE100FFF7EF00FFFBF700FFFFFF00F11111111111
      111F1BBEEEEEEEEEE4411B7E88888888E3411B7EB9B9B9B9E3411B7E88888888
      E3911B7EEEEEEEEEE4711B777777777774911B777777777774911B7777777777
      75911B733333333334A11B7DDDDDDDDDD4A11B7DD3DDDDDDD5A11B7DD3DDDDDD
      D6B11B7DD3DDDDDDDBA11BBDDDDDDDDDDA31F11111111111111F}
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 45
    OnClick = SpareBitBtnClick
  end
  object StatusBar2: TStatusBar
    Left = 0
    Top = 623
    Width = 884
    Height = 19
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBtnText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    Panels = <
      item
        Alignment = taCenter
        Bevel = pbRaised
        Width = 180
      end
      item
        Alignment = taCenter
        Width = 150
      end
      item
        Alignment = taCenter
        Width = 130
      end
      item
        Alignment = taCenter
        Width = 130
      end
      item
        Alignment = taCenter
        Width = 130
      end
      item
        Alignment = taCenter
        Width = 220
      end
      item
        Alignment = taCenter
        Width = 50
      end>
    ParentShowHint = False
    ShowHint = False
    SizeGrip = False
    UseSystemFont = False
    StyleElements = [seClient, seBorder]
    OnMouseDown = StatusBarMouseDown
  end
  object CalibrationSelectComboBox: TComboBox
    Left = 363
    Top = 31
    Width = 70
    Height = 23
    Cursor = crHandPoint
    Hint = 
      'Select the desired calibration you want - None, the VNA'#39's own or' +
      ' this App'#39's'
    AutoDropDown = True
    AutoCloseUp = True
    Style = csDropDownList
    Ctl3D = True
    DropDownCount = 30
    ParentCtl3D = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 47
    OnChange = CalibrationSelectComboBoxChange
  end
  object GraphPopupMenu: TPopupMenu
    AutoHotkeys = maManual
    OnPopup = GraphPopupMenuPopup
    Left = 292
    Top = 136
    object Enablespline1: TMenuItem
      Caption = 'Enable spline'
      OnClick = Enablespline1Click
    end
    object Showpoints1: TMenuItem
      Caption = 'Show points'
      OnClick = Showpoints1Click
    end
    object Cliptraces1: TMenuItem
      Caption = 'Clip traces'
      OnClick = Cliptraces1Click
    end
    object Snaptonearestpoint1: TMenuItem
      Caption = 'Snap to nearest point'
      OnClick = Snaptonearestpoint1Click
    end
    object N8: TMenuItem
      Caption = '-'
    end
    object Grapharrange1: TMenuItem
      Caption = 'Graph tiling'
      object GraphArrange1a: TMenuItem
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          9E050000424D9E05000000000000360400002800000012000000120000000100
          08000000000068010000230B0000230B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000000000000000000000FF
          0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFE
          FEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF
          0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF0000000000
          0000000000000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange2a: TMenuItem
        Tag = 1
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          9E050000424D9E05000000000000360400002800000012000000120000000100
          08000000000068010000230B0000230B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000FFFF00000000000000FF
          0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFE
          FE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF0000000000
          0000FFFF00000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000}
        Break = mbBreak
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange2b: TMenuItem
        Tag = 2
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          9E050000424D9E05000000000000360400002800000012000000120000000100
          08000000000068010000230B0000230B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000000000000000000000FF
          0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00000000000000000000000000000000FF0000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FF00000000000000000000000000000000FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF0000000000
          0000000000000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange3e: TMenuItem
        Tag = 7
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          1E060000424D1E060000000000003600000028000000120000001B0000000100
          180000000000E8050000530B0000530B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFF0000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000FFFFFF0000FFFFFF000000FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE000000FFFFFF
          0000FFFFFF000000FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE000000FFFFFF0000FFFFFF000000
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFE000000FFFFFF0000FFFFFF000000FEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFE000000FFFFFF0000FFFFFF000000FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE000000FFFFFF
          0000FFFFFF000000000000000000000000000000000000000000000000000000
          000000000000000000000000000000000000000000FFFFFF0000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFF0000FFFFFF00000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000FFFFFF
          0000FFFFFF000000FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE000000FFFFFF0000FFFFFF000000
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFE000000FFFFFF0000FFFFFF000000FEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFE000000FFFFFF0000FFFFFF000000FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE000000FFFFFF
          0000FFFFFF000000FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE000000FFFFFF0000FFFFFF000000
          0000000000000000000000000000000000000000000000000000000000000000
          00000000000000000000000000FFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FFFFFF000000000000000000000000000000000000000000000000000000
          000000000000000000000000000000000000000000FFFFFF0000FFFFFF000000
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFE000000FFFFFF0000FFFFFF000000FEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFE000000FFFFFF0000FFFFFF000000FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE000000FFFFFF
          0000FFFFFF000000FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE000000FFFFFF0000FFFFFF000000
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFE000000FFFFFF0000FFFFFF0000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000FFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000}
        Break = mbBreak
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange3f: TMenuItem
        Tag = 8
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          2E060000424D2E0600000000000036040000280000001B000000120000000100
          080000000000F8010000330B0000330B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FF00000000000000FFFF
          00000000000000FFFF00000000000000FF00FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00FEFEFEFE
          FE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF0000000000
          0000FFFF00000000000000FFFF00000000000000FF00FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange3a: TMenuItem
        Tag = 3
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          9E050000424D9E05000000000000360400002800000012000000120000000100
          08000000000068010000230B0000230B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000FFFF00000000000000FF
          0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00000000000000FFFF00000000000000FF0000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FF00000000000000000000000000000000FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF0000000000
          0000000000000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange3b: TMenuItem
        Tag = 4
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          9E050000424D9E05000000000000360400002800000012000000120000000100
          08000000000068010000230B0000230B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000FFFF00000000000000FF
          0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00000000000000FFFF00FEFEFEFEFE00FF0000FFFFFFFFFFFF
          FFFFFFFF00FEFEFEFEFE00FF0000FFFFFFFFFFFFFFFFFFFF00FEFEFEFEFE00FF
          0000FF00000000000000FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF0000000000
          0000FFFF00000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange3c: TMenuItem
        Tag = 5
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          9E050000424D9E05000000000000360400002800000012000000120000000100
          08000000000068010000230B0000230B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000000000000000000000FF
          0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00000000000000000000000000000000FF0000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FF00000000000000FFFF00000000000000FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF0000000000
          0000FFFF00000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange3d: TMenuItem
        Tag = 6
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          9E050000424D9E05000000000000360400002800000012000000120000000100
          08000000000068010000330B0000330B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000FFFF00000000000000FF
          0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00000000000000FF0000FF00FEFEFEFE
          FE00FFFFFFFFFFFFFFFFFFFF0000FF00FEFEFEFEFE00FFFFFFFFFFFFFFFFFFFF
          0000FF00FEFEFEFEFE00FFFF00000000000000FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF0000000000
          0000FFFF00000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4a: TMenuItem
        Tag = 9
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          9E050000424D9E05000000000000360400002800000012000000120000000100
          08000000000068010000230B0000230B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000FFFF00000000000000FF
          0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00000000000000FFFF00000000000000FF0000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FF00000000000000FFFF00000000000000FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF0000000000
          0000FFFF00000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000}
        Break = mbBreak
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4b: TMenuItem
        Tag = 10
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          BE060000424DBE06000000000000360400002800000024000000120000000100
          08000000000088020000430B0000430B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          000000000000FFFF00000000000000FFFF00000000000000FFFF000000000000
          00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FE
          FEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFE
          FE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE
          00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FE
          FEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF0000000000
          0000FFFF00000000000000FFFF00000000000000FFFF00000000000000FFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFF}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4c: TMenuItem
        Tag = 11
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          06070000424D0607000000000000360400002800000012000000240000000100
          080000000000D0020000430B0000430B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000000000000000000000FF
          0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00000000000000000000000000000000FF0000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FF00000000000000000000000000000000FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF0000000000
          0000000000000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FF000000000000000000
          00000000000000FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFE
          FEFEFEFEFEFEFEFEFEFE00FF0000FF00000000000000000000000000000000FF
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFF0000FF00000000000000000000000000000000FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFE
          FEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF
          0000FF00000000000000000000000000000000FF0000FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFF0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4d: TMenuItem
        Tag = 12
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          2E060000424D2E0600000000000036040000280000001B000000120000000100
          080000000000F8010000330B0000330B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FF000000000000000000
          00000000000000000000000000000000FF00FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFEFEFEFEFE00FF00FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFE00FF00FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFE00FF00FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE00
          FF00FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF00FF00
          000000000000000000000000000000000000000000000000FF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FF00000000000000FFFF00000000
          000000FFFF00000000000000FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF0000000000
          0000FFFF00000000000000FFFF00000000000000FF00FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4e: TMenuItem
        Tag = 13
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          52060000424D52060000000000003604000028000000120000001B0000000100
          0800000000001C020000330B0000330B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000FFFF00000000000000FF
          0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00000000000000FFFF00FEFEFEFEFE00FF0000FFFFFFFFFFFF
          FFFFFFFF00FEFEFEFEFE00FF0000FFFFFFFFFFFFFFFFFFFF00FEFEFEFEFE00FF
          0000FF00000000000000FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF0000000000
          0000FFFF00FEFEFEFEFE00FF0000FFFFFFFFFFFFFFFFFFFF00FEFEFEFEFE00FF
          0000FFFFFFFFFFFFFFFFFFFF00FEFEFEFEFE00FF0000FF00000000000000FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFE
          FE00FFFF00FEFEFEFEFE00FF0000FF00000000000000FFFF00000000000000FF
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4f: TMenuItem
        Tag = 14
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          2E060000424D2E0600000000000036040000280000001B000000120000000100
          080000000000F8010000430B0000430B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FF00000000000000FFFF
          00000000000000FFFF00000000000000FF00FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00
          000000000000FFFF00000000000000FFFF00000000000000FF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FF00000000000000000000000000
          000000000000000000000000FF00FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFEFEFEFEFE00FF00FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE
          FEFEFE00FF00FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE00
          FF00FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF00FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF00FF0000000000
          0000000000000000000000000000000000000000FF00FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4g: TMenuItem
        Tag = 15
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          52060000424D52060000000000003604000028000000120000001B0000000100
          0800000000001C020000430B0000430B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000FFFF00000000000000FF
          0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00000000000000FF0000FF00FEFEFEFE
          FE00FFFFFFFFFFFFFFFFFFFF0000FF00FEFEFEFEFE00FFFFFFFFFFFFFFFFFFFF
          0000FF00FEFEFEFEFE00FFFF00000000000000FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFE
          FE00FFFF00000000000000FF0000FF00FEFEFEFEFE00FFFFFFFFFFFFFFFFFFFF
          0000FF00FEFEFEFEFE00FFFFFFFFFFFFFFFFFFFF0000FF00FEFEFEFEFE00FFFF
          00000000000000FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFE
          FE00FFFF00FEFEFEFEFE00FF0000FF00000000000000FFFF00000000000000FF
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4h: TMenuItem
        Tag = 16
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          52060000424D52060000000000003604000028000000120000001B0000000100
          0800000000001C020000430B0000430B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000FFFF00000000000000FF
          0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00000000000000FFFF00000000000000FF0000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FF00000000000000000000000000000000FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF0000000000
          0000000000000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FF000000000000000000
          00000000000000FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFE
          FEFEFEFEFEFEFEFEFEFE00FF0000FF00000000000000000000000000000000FF
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4i: TMenuItem
        Tag = 17
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          52060000424D52060000000000003604000028000000120000001B0000000100
          0800000000001C020000430B0000430B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000000000000000000000FF
          0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00000000000000000000000000000000FF0000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FF00000000000000FFFF00000000000000FF0000FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF0000000000
          0000FFFF00000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FF000000000000000000
          00000000000000FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFE
          FEFEFEFEFEFEFEFEFEFE00FF0000FF00000000000000000000000000000000FF
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4j: TMenuItem
        Tag = 18
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          52060000424D52060000000000003604000028000000120000001B0000000100
          0800000000001C020000530B0000530B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000000000000000000000FF
          0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00000000000000000000000000000000FF0000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FF00000000000000000000000000000000FF0000FF00FEFEFEFEFEFEFEFE
          FEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00
          FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFE
          FEFE00FF0000FF00FEFEFEFEFEFEFEFEFEFEFEFEFEFE00FF0000FF0000000000
          0000000000000000000000FF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FF00000000000000FFFF
          00000000000000FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FF0000FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF0000FF00FEFEFEFE
          FE00FFFF00FEFEFEFEFE00FF0000FF00000000000000FFFF00000000000000FF
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4k: TMenuItem
        Tag = 19
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          2E060000424D2E0600000000000036040000280000001B000000120000000100
          080000000000F8010000530B0000530B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FF00000000000000FFFF
          00000000000000FFFF00000000000000FF00FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00000000000000FF00FF00FEFEFEFE
          FE00FFFF00FEFEFEFEFE00FFFFFFFFFFFFFFFFFFFF00FF00FEFEFEFEFE00FFFF
          00FEFEFEFEFE00FFFFFFFFFFFFFFFFFFFF00FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FFFF00000000000000FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF0000000000
          0000FFFF00000000000000FFFF00000000000000FF00FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4l: TMenuItem
        Tag = 20
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          2E060000424D2E0600000000000036040000280000001B000000120000000100
          080000000000F8010000530B0000530B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FF00000000000000FFFF
          00000000000000FFFF00000000000000FF00FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00
          FEFEFEFEFE00FFFF00000000000000FFFF00FEFEFEFEFE00FF00FF00FEFEFEFE
          FE00FFFFFFFFFFFFFFFFFFFFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF
          FFFFFFFFFFFFFFFFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00000000
          000000FFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF0000000000
          0000FFFF00000000000000FFFF00000000000000FF00FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
      object GraphArrange4m: TMenuItem
        Tag = 21
        AutoHotkeys = maManual
        AutoLineReduction = maManual
        Bitmap.Data = {
          2E060000424D2E0600000000000036040000280000001B000000120000000100
          080000000000F8010000630B0000630B00000001000000010000000000000101
          0100020202000303030004040400050505000606060007070700080808000909
          09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
          1100121212001313130014141400151515001616160017171700181818001919
          19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
          2100222222002323230024242400252525002626260027272700282828002929
          29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
          3100323232003333330034343400353535003636360037373700383838003939
          39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
          4100424242004343430044444400454545004646460047474700484848004949
          49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
          5100525252005353530054545400555555005656560057575700585858005959
          59005A5A5A005B5B5B005C5C5C005D5D5D005E5E5E005F5F5F00606060006161
          6100626262006363630064646400656565006666660067676700686868006969
          69006A6A6A006B6B6B006C6C6C006D6D6D006E6E6E006F6F6F00707070007171
          7100727272007373730074747400757575007676760077777700787878007979
          79007A7A7A007B7B7B007C7C7C007D7D7D007E7E7E007F7F7F00808080008181
          8100828282008383830084848400858585008686860087878700888888008989
          89008A8A8A008B8B8B008C8C8C008D8D8D008E8E8E008F8F8F00909090009191
          9100929292009393930094949400959595009696960097979700989898009999
          99009A9A9A009B9B9B009C9C9C009D9D9D009E9E9E009F9F9F00A0A0A000A1A1
          A100A2A2A200A3A3A300A4A4A400A5A5A500A6A6A600A7A7A700A8A8A800A9A9
          A900AAAAAA00ABABAB00ACACAC00ADADAD00AEAEAE00AFAFAF00B0B0B000B1B1
          B100B2B2B200B3B3B300B4B4B400B5B5B500B6B6B600B7B7B700B8B8B800B9B9
          B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
          C100C2C2C200C3C3C300C4C4C400C5C5C500C6C6C600C7C7C700C8C8C800C9C9
          C900CACACA00CBCBCB00CCCCCC00CDCDCD00CECECE00CFCFCF00D0D0D000D1D1
          D100D2D2D200D3D3D300D4D4D400D5D5D500D6D6D600D7D7D700D8D8D800D9D9
          D900DADADA00DBDBDB00DCDCDC00DDDDDD00DEDEDE00DFDFDF00E0E0E000E1E1
          E100E2E2E200E3E3E300E4E4E400E5E5E500E6E6E600E7E7E700E8E8E800E9E9
          E900EAEAEA00EBEBEB00ECECEC00EDEDED00EEEEEE00EFEFEF00F0F0F000F1F1
          F100F2F2F200F3F3F300F4F4F400F5F5F500F6F6F600F7F7F700F8F8F800F9F9
          F900FAFAFA00FBFBFB00FCFCFC00FDFDFD00FEFEFE00FFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FF00000000000000FFFF
          00000000000000FFFF00000000000000FF00FF00FEFEFEFEFE00FFFF00FEFEFE
          FEFE00FFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00
          000000000000FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FFFFFFFFFFFF
          FFFFFFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FFFFFFFFFFFFFFFFFFFF
          00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00000000000000FFFF00FEFEFE
          FEFE00FFFF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF
          FF00FEFEFEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFE
          FEFEFE00FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00
          FF00FF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF00
          FEFEFEFEFE00FFFF00FEFEFEFEFE00FFFF00FEFEFEFEFE00FF00FF0000000000
          0000FFFF00000000000000FFFF00000000000000FF00FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00}
        RadioItem = True
        OnClick = GraphArrangeClick
        OnMeasureItem = OnGraphArrangeMeasureItem
      end
    end
    object Graphtype1: TMenuItem
      Caption = 'Graph type'
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object Addmarker1: TMenuItem
      Caption = 'Add marker'
      OnClick = Addmarker1Click
    end
    object Deletemarker1: TMenuItem
      Caption = 'Delete marker'
      OnClick = Deletemarker1Click
    end
    object Deselectmarker1: TMenuItem
      Caption = 'Deselect marker'
      OnClick = Deselectmarker1Click
    end
    object Deltamarker2: TMenuItem
      Caption = 'Delta marker'
      OnClick = Deltamarker2Click
    end
    object Markergraph1: TMenuItem
      Caption = 'Marker graph'
      object Allgraphs1: TMenuItem
        Caption = 'All graphs'
        OnClick = Allgraphs1Click
      end
      object Graph11: TMenuItem
        Caption = 'Graph 1'
        OnClick = Graph11Click
      end
      object Graph21: TMenuItem
        Caption = 'Graph 2'
        OnClick = Graph21Click
      end
      object Graph31: TMenuItem
        Caption = 'Graph 3'
        OnClick = Graph31Click
      end
      object Graph41: TMenuItem
        Caption = 'Graph 4'
        OnClick = Graph41Click
      end
    end
    object Markertrace1: TMenuItem
      Caption = 'Marker trace'
      object Alltraces1: TMenuItem
        Caption = 'All traces'
        OnClick = Alltraces1Click
      end
      object Trace11: TMenuItem
        Caption = 'Trace 1'
        OnClick = Trace11Click
      end
      object Trace21: TMenuItem
        Caption = 'Trace 2'
        OnClick = Trace21Click
      end
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object Showmarkers1: TMenuItem
      Caption = 'Show markers'
      OnClick = Showmarkers1Click
    end
    object Showminmaxtext1: TMenuItem
      Caption = 'Show min/max text'
      OnClick = Showminmaxtext1Click
    end
    object Showmaxmarker1: TMenuItem
      Caption = 'Show max marker'
      OnClick = Showmaxmarker1Click
    end
    object Showminmarker1: TMenuItem
      Caption = 'Show min marker'
      OnClick = Showminmarker1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Setasstartfrequency1: TMenuItem
      Caption = 'Set as start frequency'
      OnClick = Setasstartfrequency1Click
    end
    object Setascenterfrequency1: TMenuItem
      Caption = 'Set as center frequency'
      OnClick = Setascenterfrequency1Click
    end
    object Setasstopfrequency1: TMenuItem
      Caption = 'Set as stop frequency'
      OnClick = Setasstopfrequency1Click
    end
    object N6: TMenuItem
      Caption = '-'
    end
    object Automaxscale1: TMenuItem
      Caption = 'Auto max scale'
      OnClick = Automaxscale1Click
    end
    object Autominscale1: TMenuItem
      Caption = 'Auto min scale'
      OnClick = Autominscale1Click
    end
    object Autoscalepeakhold1: TMenuItem
      Caption = 'Auto scale peak hold'
      OnClick = Autoscalepeakhold1Click
    end
    object Scaletominmaxvalues1: TMenuItem
      Caption = 'Scale to min/max values'
      OnClick = Scaletominmaxvalues1Click
    end
    object Scaletomaxvalue1: TMenuItem
      Caption = 'Scale to max value'
      OnClick = Scaletomaxvalue1Click
    end
    object Scaletominvalue1: TMenuItem
      Caption = 'Scale to min value'
      OnClick = Scaletominvalue1Click
    end
    object Setmaxscale1: TMenuItem
      Caption = 'Set max scale'
      OnClick = Setmaxscale1Click
    end
    object Setminscale1: TMenuItem
      Caption = 'Set min scale'
      OnClick = Setminscale1Click
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object Copyimage1: TMenuItem
      Caption = 'Copy image'
      OnClick = Copyimage1Click
    end
    object Saveimage1: TMenuItem
      Caption = '&Save image'
      OnClick = Saveimage1Click
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object Cancel1: TMenuItem
      Caption = 'Cancel'
    end
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 50
    OnTimer = Timer1Timer
    Left = 292
    Top = 204
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = '*.png'
    Filter = 
      'PNG {*.png}|*.png|BMP {*.bmp}|*.bmp|JPG {*.jpg}|*.jpg|All files ' +
      '(*.*}|*.*'
    Options = [ofPathMustExist, ofNoReadOnlyReturn, ofEnableSizing, ofDontAddToRecent]
    Left = 298
    Top = 84
  end
  object MarkersListBoxPopupMenu: TPopupMenu
    OnPopup = MarkersListBoxPopupMenuPopup
    Left = 488
    Top = 204
    object Addmarker2: TMenuItem
      Caption = 'Add marker'
      OnClick = Addmarker2Click
    end
    object Deletemarker2: TMenuItem
      Caption = 'Delete marker'
      OnClick = Deletemarker2Click
    end
    object Deleteallmarkers1: TMenuItem
      Caption = 'Delete all markers'
      OnClick = Deleteallmarkers1Click
    end
    object N9: TMenuItem
      Caption = '-'
    end
    object Setmarkerfrequency1: TMenuItem
      Caption = 'Marker frequency'
      OnClick = Setmarkerfrequency1Click
    end
    object Deltamarker1: TMenuItem
      Caption = 'Delta marker'
      OnClick = Deltamarker1Click
    end
    object N7: TMenuItem
      Caption = '-'
    end
    object Cancel2: TMenuItem
      Caption = 'Cancel'
    end
  end
  object GraphTypePopupMenu: TPopupMenu
    AutoHotkeys = maManual
    AutoPopup = False
    OnPopup = GraphTypePopupMenuPopup
    Left = 388
    Top = 141
  end
  object MemoryPopupMenu: TPopupMenu
    OnPopup = MemoryPopupMenuPopup
    Left = 488
    Top = 139
    object ClearMemory1: TMenuItem
      Caption = 'Clear memory'
      OnClick = ClearMemory1Click
    end
    object LoadMemoryFromFile1: TMenuItem
      Caption = 'Load memory from file'
      OnClick = LoadMemoryFromFile1Click
    end
    object SetMemory1: TMenuItem
      Caption = 'Set memory'
      OnClick = SetMemory1Click
    end
    object N12: TMenuItem
      Caption = '-'
    end
    object Clearmemoryname1: TMenuItem
      Caption = 'Clear memory name'
      OnClick = Clearmemoryname1Click
    end
    object Memoryname1: TMenuItem
      Caption = 'Memory name'
      OnClick = Memoryname1Click
    end
    object N11: TMenuItem
      Caption = '-'
    end
    object SetScanRangeFromMemory1: TMenuItem
      Caption = 'Set scan range to this memory'
      OnClick = SetScanRangeFromMemory1Click
    end
    object N10: TMenuItem
      Caption = '-'
    end
    object Cancel3: TMenuItem
      Caption = 'Cancel'
    end
  end
end

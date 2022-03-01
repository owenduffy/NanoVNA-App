object CalibrationForm: TCalibrationForm
  Left = 94
  Top = 140
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = 'Calibration'
  ClientHeight = 360
  ClientWidth = 559
  Color = clBtnFace
  Constraints.MinHeight = 350
  Constraints.MinWidth = 575
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
  OnDestroy = FormDestroy
  OnHide = FormHide
  OnKeyDown = FormKeyDown
  OnMouseEnter = FormMouseEnter
  OnShow = FormShow
  DesignSize = (
    559
    360)
  PixelsPerInch = 96
  TextHeight = 13
  object Label6: TLabel
    Left = 8
    Top = 155
    Width = 92
    Height = 13
    Alignment = taRightJustify
    Caption = 'Calibration file path '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object CheckImage1: TImage
    Left = 204
    Top = 8
    Width = 16
    Height = 16
    AutoSize = True
    Center = True
    Picture.Data = {
      07544269746D617036030000424D360300000000000036000000280000001000
      000010000000010018000000000000030000000B0000000B0000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000012
      00003D0002580008670009660002540000300000050000000000000000000000
      00000000000000000000000044000173000888000F8D001388001F7E00327200
      266D00076200001D0000000000000000000000000000000000590002830404A0
      08009D0000A30D009A000097000094001E7E00396B000F6900001C0000000000
      0000000000003D000681070BA81701A10D38BF54DCF3E97ADD9B009D03009900
      0098001285003A6C00056400000500000000000000006C0024A82F08A91B2BB4
      3EF6F0F5FCF6FAFFFFFF7DDE9C009E07009B02009900217D00266E0000310000
      0000000000027F0039BF4F20AA2AECE6EAF5EEF4FFF7FFFFFBFFFFFFFF7DDD9C
      009F09009B020294003671000057000000000000000F90143BBE50AAC9A7FAEA
      F9D1E1D11EAF33DAEFDDFFFFFFFFFFFF7BDD9B009E07009A00297C0008690000
      00000000001193155DD27C17A3258EBF8811AB241BB9400BAD27DBEFDFFFFFFF
      FFFFFF7BDB99009E021B8600086B0000000000000000870077DC952CC55A1FBB
      4529C45626C04F22BB450BAC26DAEEDDFFFCFFFFFDFF78DB96138C00015E0000
      000000000000820053C66C61D8892ECA622FC9612CC55927C04D20B94209AA22
      DAEDDDFFF9FFCAECD70E8E080040000000000000000094000E981483E5A74CD3
      792ECB632FC96029C35422BB461CB53A0BA81F7ACC8511AD2401730000120000
      0000000000000000009D0022A52B81E3A65ED88831CA6327C35321BC471DB63B
      16B02E0FA920028805004A00000000000000000000000000000000009D000F99
      1456CA7377E09C59D37E43C86636C1541EAA2F058405005B0000000000000000
      0000000000000000000000000000009400008400028A021397190F9316028301
      007000003B000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000}
    Transparent = True
    Visible = False
  end
  object CheckImage2: TImage
    Left = 182
    Top = 8
    Width = 16
    Height = 16
    AutoSize = True
    Center = True
    Picture.Data = {
      07544269746D617036030000424D360300000000000036000000280000001000
      000010000000010018000000000000030000130B0000130B0000000000000000
      0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FF317A362D7532FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFF39854037833D317B372E7633FFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF429249408E
      4754A35C4F9F57327C382E7734FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFF4B9E53499A515BAC6477CA8274C87E51A059337D392F7835
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF53A95C51A65A63B56D7ECE
      897BCC8776CA8176C98152A25A347E3A307935FFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFF5AB46559B0636BBD7684D2907AC98560B26A63B46D78C98378CB8253A35C
      347F3A317A36FFFFFFFFFFFFFFFFFFFFFFFF5EB9695BB56679C98680CE8D51A6
      5A4DA156499C515CAD677CCC8679CB8554A45D35803B317B37FFFFFFFFFFFFFF
      FFFFFFFFFF5FBA6A5CB6666DC07955AC5FFFFFFFFFFFFF4A9D525EAE687DCD89
      7CCD8756A55F36813C327C38FFFFFFFFFFFFFFFFFFFFFFFF5FBB6A5CB767FFFF
      FFFFFFFFFFFFFFFFFFFF4B9E535FAF697FCE8A7ECE8957A66037823D337D39FF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF4B9F54
      60B06A81CF8D7FCF8B58A761398540347E3AFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF4CA05562B26C82D18F7AC88557A66038
      843FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFF4DA15663B36D5FAF69419149FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF4EA2574A9D52FFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFF}
    Transparent = True
    Visible = False
  end
  object Label2: TLabel
    Left = 14
    Top = 123
    Width = 43
    Height = 13
    Alignment = taRightJustify
    Caption = 'Average '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object AverageLabel: TLabel
    Left = 159
    Top = 123
    Width = 21
    Height = 13
    Caption = 'XXX'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object SmoothingLabel: TLabel
    Left = 350
    Top = 123
    Width = 21
    Height = 13
    Caption = 'XXX'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label40: TLabel
    Left = 195
    Top = 123
    Width = 53
    Height = 13
    Alignment = taRightJustify
    Caption = 'Smoothing '
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object MedianLabel: TLabel
    Left = 528
    Top = 123
    Width = 21
    Height = 13
    Caption = 'XXX'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label4: TLabel
    Left = 389
    Top = 123
    Width = 37
    Height = 13
    Alignment = taRightJustify
    Caption = 'Median '
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    ParentFont = False
  end
  object CalibrationNameLabel: TLabel
    Left = 6
    Top = 40
    Width = 547
    Height = 21
    Alignment = taCenter
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'CalibrationNameLabel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object SaveAsFileBitBtn: TBitBtn
    Left = 49
    Top = 6
    Width = 38
    Height = 25
    Cursor = crHandPoint
    Hint = 'Save as'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000330B0000330B00001000000010000000AD633100B96B
      3100C2784400CE8C6300CF987300DE9C7300DEA57300E7A57B0063C68C00D2B1
      9500EBBFA000F6DDCC00FFECE100FFF7EF00FFFBF700FFFFFF00F11111111111
      111FF989898B889898BC1BBEEEEEEEEEE4419FEEEEEEEEEEEBB81B7E88888888
      E3419F8EBB8BBBBBEBB91B7EB9B9B9B9E3419FBEDBEBBBE8E8B81B7E88888888
      E3919FBEBB8CB8CBDBB91B7EEEEEEEEEE4719F8EDDEDEEDDDBB81B7777777777
      74919FDBBBBBBBBBB8B91B777777777774919FB8C8B9B8B8CBBB1B7777777777
      75919FBBBBBBBBBBB8B81B733333333334A19F8C8C8B8B8B8BB91B7DDDDDDDDD
      D4A19FBEEEEEEEEEEBB81B7DD3DDDDDDD5A19FBEEBEEEEEEEDB91B7DD3DDDDDD
      D6B19F8FE8EEEEEEE8D81B7DD3DDDDDDDBA19FBFEBEEFEFEFEB91BBDDDDDDDDD
      DA319FFFEFFFFEFFEBD8F11111111111111FF999B999B999B99F}
    NumGlyphs = 2
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    OnClick = SaveAsFileBitBtnClick
  end
  object LoadFileBitBtn: TBitBtn
    Left = 6
    Top = 6
    Width = 37
    Height = 25
    Cursor = crHandPoint
    Hint = 'Load a calibration file'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000120B0000120B00001000000000000000000000000000
      800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00555555555555
      5555555555555555555555555555555555555555555555555555555555555555
      555555555555555555555555555555555555555FFFFFFFFFF555550000000000
      55555577777777775F55500B8B8B8B8B05555775F555555575F550F0B8B8B8B8
      B05557F75F555555575F50BF0B8B8B8B8B0557F575FFFFFFFF7F50FBF0000000
      000557F557777777777550BFBFBFBFB0555557F555555557F55550FBFBFBFBF0
      555557F555555FF7555550BFBFBF00055555575F555577755555550BFBF05555
      55555575FFF75555555555700007555555555557777555555555555555555555
      5555555555555555555555555555555555555555555555555555}
    NumGlyphs = 2
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnClick = LoadFileBitBtnClick
  end
  object GridPanel1: TGridPanel
    Left = 6
    Top = 70
    Width = 547
    Height = 37
    Anchors = [akLeft, akTop, akRight]
    BevelOuter = bvNone
    Caption = 'GridPanel1'
    ColumnCollection = <
      item
        Value = 20.000000000000000000
      end
      item
        Value = 20.000000000000000000
      end
      item
        Value = 20.000000000000000000
      end
      item
        Value = 20.000000000000000000
      end
      item
        Value = 20.000000000000000000
      end>
    ControlCollection = <
      item
        Column = 0
        Control = ClearCalibrationButton
        Row = 0
      end
      item
        Column = 3
        Control = LoadBitBtn
        Row = 0
      end
      item
        Column = 1
        Control = ShortBitBtn
        Row = 0
      end
      item
        Column = 2
        Control = OpenBitBtn
        Row = 0
      end
      item
        Column = 4
        Control = ThroughBitBtn
        Row = 0
      end>
    RowCollection = <
      item
        Value = 100.000000000000000000
      end>
    ShowCaption = False
    TabOrder = 4
    object ClearCalibrationButton: TButton
      Left = 0
      Top = 0
      Width = 109
      Height = 37
      Cursor = crHandPoint
      Hint = 'Clear the calibration'
      Align = alClient
      Caption = 'Reset SOLT'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      WordWrap = True
      OnClick = ClearCalibrationButtonClick
    end
    object LoadBitBtn: TBitBtn
      Left = 327
      Top = 0
      Width = 109
      Height = 37
      Cursor = crHandPoint
      Hint = 'Attach 50R LOAD cal-standards onto both S11 and S21 ports'
      Align = alClient
      Caption = 'Load/Isoln'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      Layout = blGlyphRight
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Spacing = 8
      TabOrder = 1
      WordWrap = True
      OnClick = CalTypeBitBtnClick
      OnMouseDown = CalButtonBitBtnMouseDown
    end
    object ShortBitBtn: TBitBtn
      Left = 109
      Top = 0
      Width = 109
      Height = 37
      Cursor = crHandPoint
      Hint = 'Attach SHORT cal-standard onto S11 port'
      Align = alClient
      Caption = 'Short'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      Layout = blGlyphRight
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Spacing = 8
      TabOrder = 2
      WordWrap = True
      OnClick = CalTypeBitBtnClick
      OnMouseDown = CalButtonBitBtnMouseDown
    end
    object OpenBitBtn: TBitBtn
      Left = 218
      Top = 0
      Width = 109
      Height = 37
      Cursor = crHandPoint
      Hint = 'Attach OPEN cal-standard onto S11 port'
      Align = alClient
      Caption = 'Open'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      Layout = blGlyphRight
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Spacing = 8
      TabOrder = 3
      WordWrap = True
      OnClick = CalTypeBitBtnClick
      OnMouseDown = CalButtonBitBtnMouseDown
    end
    object ThroughBitBtn: TBitBtn
      Left = 436
      Top = 0
      Width = 111
      Height = 37
      Cursor = crHandPoint
      Hint = 
        'Attach 50R THROUGH-CABLE cal-standard between S11 port and S21 p' +
        'ort'
      Align = alClient
      Caption = 'Through'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Lucida Sans Unicode'
      Font.Style = [fsBold]
      Layout = blGlyphRight
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Spacing = 8
      TabOrder = 4
      WordWrap = True
      OnClick = CalTypeBitBtnClick
      OnMouseDown = CalButtonBitBtnMouseDown
    end
  end
  object CalibrationPathEdit: TEdit
    Left = 116
    Top = 152
    Width = 394
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    ReadOnly = True
    TabOrder = 5
    Text = 'CalibrationPathEdit'
    OnChange = CalibrationPathEditChange
  end
  object SelectFilePathBitBtn: TBitBtn
    Left = 520
    Top = 152
    Width = 33
    Height = 21
    Cursor = crHandPoint
    Hint = 'Select calibration file path'
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
    TabOrder = 6
    OnClick = SelectFilePathBitBtnClick
  end
  object ExportFilesBitBtn: TBitBtn
    Left = 137
    Top = 6
    Width = 39
    Height = 25
    Cursor = crHandPoint
    Hint = 'Export SOLT calibration to individual s1p files'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000330B0000330B00001000000010000000AD633100B96B
      3100C2784400CE8C6300CF987300DE9C7300DEA57300E7A57B0063C68C00D2B1
      9500EBBFA000F6DDCC00FFECE100FFF7EF00FFFBF700FFFFFF00F11111111111
      111FF989898B889898BC1BBEEEEEEEEEE4419FEEEEEEEEEEEBB81B7E88888888
      E3419F8EBB8BBBBBEBB91B7EB9B9B9B9E3419FBEDBEBBBE8E8B81B7E88888888
      E3919FBEBB8CB8CBDBB91B7EEEEEEEEEE4719F8EDDEDEEDDDBB81B7777777777
      74919FDBBBBBBBBBB8B91B777777777774919FB8C8B9B8B8CBBB1B7777777777
      75919FBBBBBBBBBBB8B81B733333333334A19F8C8C8B8B8B8BB91B7DDDDDDDDD
      D4A19FBEEEEEEEEEEBB81B7DD3DDDDDDD5A19FBEEBEEEEEEEDB91B7DD3DDDDDD
      D6B19F8FE8EEEEEEE8D81B7DD3DDDDDDDBA19FBFEBEEFEFEFEB91BBDDDDDDDDD
      DA319FFFEFFFFEFFEBD8F11111111111111FF999B999B999B99F}
    NumGlyphs = 2
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = ExportFilesBitBtnClick
  end
  object SaveFileBitBtn: TBitBtn
    Left = 93
    Top = 6
    Width = 38
    Height = 25
    Cursor = crHandPoint
    Hint = 'Save'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000330B0000330B00001000000010000000AD633100B96B
      3100C2784400CE8C6300CF987300DE9C7300DEA57300E7A57B0063C68C00D2B1
      9500EBBFA000F6DDCC00FFECE100FFF7EF00FFFBF700FFFFFF00F11111111111
      111FF989898B889898BC1BBEEEEEEEEEE4419FEEEEEEEEEEEBB81B7E88888888
      E3419F8EBB8BBBBBEBB91B7EB9B9B9B9E3419FBEDBEBBBE8E8B81B7E88888888
      E3919FBEBB8CB8CBDBB91B7EEEEEEEEEE4719F8EDDEDEEDDDBB81B7777777777
      74919FDBBBBBBBBBB8B91B777777777774919FB8C8B9B8B8CBBB1B7777777777
      75919FBBBBBBBBBBB8B81B733333333334A19F8C8C8B8B8B8BB91B7DDDDDDDDD
      D4A19FBEEEEEEEEEEBB81B7DD3DDDDDDD5A19FBEEBEEEEEEEDB91B7DD3DDDDDD
      D6B19F8FE8EEEEEEE8D81B7DD3DDDDDDDBA19FBFEBEEFEFEFEB91BBDDDDDDDDD
      DA319FFFEFFFFEFFEBD8F11111111111111FF999B999B999B99F}
    NumGlyphs = 2
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnClick = SaveFileBitBtnClick
  end
  object AverageTrackBar: TTrackBar
    Left = 63
    Top = 119
    Width = 90
    Height = 21
    Cursor = crHandPoint
    Hint = 
      'Scans per calibration - averaging reduces the calibrated noise l' +
      'evel'
    Ctl3D = True
    Max = 8
    ParentCtl3D = False
    ParentShowHint = False
    PageSize = 1
    Position = 3
    ShowHint = True
    TabOrder = 7
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = AverageTrackBarChange
  end
  object SmoothingTrackBar: TTrackBar
    Left = 254
    Top = 119
    Width = 90
    Height = 21
    Cursor = crHandPoint
    Hint = 'Recommended setting: OFF'
    Max = 6
    ParentShowHint = False
    PageSize = 1
    ShowHint = True
    TabOrder = 8
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = SmoothingTrackBarChange
  end
  object MedianTrackBar: TTrackBar
    Left = 432
    Top = 119
    Width = 90
    Height = 21
    Cursor = crHandPoint
    Hint = 'Recommended setting: OFF'
    Max = 6
    ParentShowHint = False
    PageSize = 1
    ShowHint = True
    TabOrder = 9
    ThumbLength = 19
    TickMarks = tmBoth
    TickStyle = tsNone
    OnChange = MedianTrackBarChange
  end
  object SetMainSweepButton: TButton
    Left = 448
    Top = 6
    Width = 105
    Height = 25
    Cursor = crHandPoint
    Hint = 'Set the main sweep settings to match the calibration'
    Anchors = [akTop, akRight]
    Caption = 'Set Sweep'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = [fsBold]
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 10
    OnClick = SetMainSweepButtonClick
  end
  object CalibrationFilesListView: TListView
    Left = 0
    Top = 181
    Width = 559
    Height = 179
    Cursor = crArrow
    Align = alBottom
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    Columns = <
      item
        MaxWidth = 1
        MinWidth = 1
        Width = 1
      end
      item
        Alignment = taRightJustify
        Caption = 'No.'
        MaxWidth = 50
        MinWidth = 50
      end
      item
        AutoSize = True
        Caption = 'Calibration Name'
        MinWidth = 50
      end
      item
        Alignment = taCenter
        Caption = 'Start'
        MinWidth = 50
        Width = 100
      end
      item
        Alignment = taCenter
        Caption = 'Stop'
        MinWidth = 50
        Width = 100
      end
      item
        Alignment = taCenter
        Caption = 'Points'
        MinWidth = 50
        Width = 70
      end>
    ColumnClick = False
    HotTrackStyles = [htHandPoint]
    HoverTime = 20
    OwnerData = True
    ReadOnly = True
    RowSelect = True
    ParentColor = True
    ParentShowHint = False
    ShowHint = False
    TabOrder = 11
    ViewStyle = vsReport
    OnChange = CalibrationFilesListViewChange
    OnData = CalibrationFilesListViewData
    OnDblClick = CalibrationFilesListViewDblClick
    OnKeyDown = CalibrationFilesListViewKeyDown
    OnMouseDown = CalibrationFilesListViewMouseDown
    OnMouseLeave = CalibrationFilesListViewMouseLeave
    OnSelectItem = CalibrationFilesListViewSelectItem
  end
  object ListPopupMenu: TPopupMenu
    OnPopup = ListPopupMenuPopup
    Left = 300
    Top = 220
    object LoadCalibrationFile1: TMenuItem
      Caption = 'Load'
      OnClick = LoadCalibrationFile1Click
    end
    object DeleteCalibrationFile1: TMenuItem
      Caption = 'Delete'
      OnClick = DeleteCalibrationFile1Click
    end
    object RenameCalibrationFile1: TMenuItem
      Caption = 'Rename'
      OnClick = RenameCalibrationFile1Click
    end
    object CopyCalibrationName1: TMenuItem
      Caption = 'Copy name'
      OnClick = CopyCalibrationName1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Cancel1: TMenuItem
      Caption = 'Cancel'
    end
  end
  object CalButtonPopupMenu: TPopupMenu
    OnPopup = CalButtonPopupMenuPopup
    Left = 288
    Top = 72
    object ClearCal1: TMenuItem
      Caption = 'Clear'
      OnClick = ClearCal1Click
    end
    object Stop1: TMenuItem
      Caption = 'Stop'
      Enabled = False
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object Cancel2: TMenuItem
      Caption = 'Cancel'
    end
  end
end

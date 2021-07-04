object BatteryVoltageForm: TBatteryVoltageForm
  Left = 147
  Top = 215
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = 'Battery Voltage'
  ClientHeight = 295
  ClientWidth = 334
  Color = clBtnFace
  Constraints.MinHeight = 260
  Constraints.MinWidth = 350
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnMouseEnter = FormMouseEnter
  OnShow = FormShow
  DesignSize = (
    334
    295)
  PixelsPerInch = 96
  TextHeight = 13
  object PaintBox1: TPaintBox
    Left = 4
    Top = 32
    Width = 325
    Height = 243
    Anchors = [akLeft, akTop, akRight, akBottom]
    Color = clWhite
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Consolas'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    OnMouseLeave = PaintBox1MouseLeave
    OnMouseMove = PaintBox1MouseMove
    OnPaint = PaintBox1Paint
    ExplicitWidth = 613
    ExplicitHeight = 320
  end
  object Label1: TLabel
    Left = 141
    Top = 9
    Width = 56
    Height = 13
    Alignment = taRightJustify
    Caption = 'vbat_offset '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object VBATOffsetLabel: TLabel
    Left = 197
    Top = 9
    Width = 33
    Height = 13
    Caption = 'XXXX'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 276
    Width = 334
    Height = 19
    Panels = <
      item
        Alignment = taCenter
        Width = 150
      end
      item
        Alignment = taCenter
        Width = 100
      end
      item
        Alignment = taCenter
        Width = 100
      end>
  end
  object CloseBitBtn: TBitBtn
    Left = 293
    Top = 4
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
    TabOrder = 1
    Visible = False
    OnClick = CloseBitBtnClick
  end
  object CopyBitBtn: TBitBtn
    Left = 88
    Top = 4
    Width = 36
    Height = 24
    Cursor = crHandPoint
    Hint = 'Copy the image to the clipboard'
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
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnClick = CopyBitBtnClick
  end
  object SaveBitBtn: TBitBtn
    Left = 46
    Top = 4
    Width = 36
    Height = 24
    Cursor = crHandPoint
    Hint = 'Save the image to a file'
    Glyph.Data = {
      F6000000424DF600000000000000760000002800000010000000100000000100
      04000000000080000000230B0000230B00001000000010000000AD633100B96B
      3100C2784400CE8C6300CF987300DE9C7300DEA57300E7A57B0063C68C00D2B1
      9500EBBFA000F6DDCC00FFECE100FFF7EF00FFFBF700FFFFFF00F11111111111
      111F1BBEEEEEEEEEE4411B7E88888888E3411B7EB9B9B9B9E3411B7E88888888
      E3911B7EEEEEEEEEE4711B777777777774911B777777777774911B7777777777
      75911B733333333334A11B7DDDDDDDDDD4A11B7DD3DDDDDDD5A11B7DD3DDDDDD
      D6B11B7DD3DDDDDDDBA11BBDDDDDDDDDDA31F11111111111111F}
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = SaveBitBtnClick
  end
  object ClearBitBtn: TBitBtn
    Left = 4
    Top = 4
    Width = 36
    Height = 24
    Cursor = crHandPoint
    Hint = 'Clear the graph'
    Glyph.Data = {
      36050000424D3605000000000000360400002800000010000000100000000100
      08000000000000010000130B0000130B00000001000000010000FFFFFF006D6D
      E2006B6AE100686ADD006364DC006164DA005D63D9005B63D6005862D4006D6C
      E3006A6ADF00535ED100505DCE006A69E0004C5BCC004858CA006C6CE1006A69
      E0004557C7004156C5006A69E0003E54C200676ADE003A53C0006469DB00364F
      BE005F63DA003250BC005C62D7002E4BBA005860D4002A4AB800545FD2005361
      CF002A4AB800294BB5004D5ACD004959CB002B4AB700294BB5004658C8004255
      C6002B4AB7003F54C3003A50C2003853BE003551BD00304BBC002E4EB8002B4C
      B7002748B5008E93ED00BEC3F800CCD3F900C4CBF900AAB4F4006670E200AAAD
      F200D8DCFD00AEBAFA0091A3FA008B9DFA009CA9FB00BAC7FC00707BE900A3A7
      F300D4DBFD00879AFA007F91F0007A8EF1007F94F8007E92F900768CF800A8B6
      F800636EE3007D83EA00CDD4FC007E93F700758AEE006C84F6006379F300A4AF
      F8003E4FD000A5AFF5009DABFA00778CF000545FEC006377F200818EF400787F
      E900AFB9F9007F93FA007085F000FFFFFF004D59E9005E6AEE00969DF100A1AB
      F7007086F8006882F6005C66EA00969CF100818CEE007E91F7005D73F3004F5B
      E9007B83F000757BE2004B56DB00A2ABF6005664F0005266EE004C58E600525A
      E6009FA3F5003450C400616BE300A1ACF500505CEA004E59E6004C56E6005056
      E6009EA2F4005460D6005C65E000A1A6F5007E86EF005B63E900595DE7007D84
      EE009EA0F400515DD7003C52CC00757AE8008F92EE007178E400334DC1000000
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
      00000000000000000000000000002B2C2D2E2F30313200000000000000282984
      85868687882A27000000000024257C7D7E7F8081828326270000002021747556
      765E7778797A7B222300001E6C6D6E6F5E5E5E5E707172731F00001C6667685E
      5E5E5E5E5E696A6B1D00001A6162635D5D5D5D5D5D5E64651B0000185A5B5C5D
      5D5D5D5D5D5E5F6019000016535455565656565656575859170000144B4C3D4D
      4E4F4F4F4F50515215000010114142434445464748494A1213000000090D393A
      3B3C3D3E3F400E0F0000000000090A3334353637380B0C000000000000000102
      0304050607080000000000000000000000000000000000000000}
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnClick = ClearBitBtnClick
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'bmp'
    Filter = 
      'PNG {*.png}|*.png|BMP {*.bmp}|*.bmp|JPG {*.jpg}|*.jpg|All files ' +
      '(*.*}|*.*'
    Options = [ofPathMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Left = 44
    Top = 40
  end
end

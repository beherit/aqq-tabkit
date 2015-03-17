object SideSlideExceptionsForm: TSideSlideExceptionsForm
  Tag = 1
  Left = 0
  Top = 0
  BorderStyle = bsToolWindow
  Caption = 'TabKit - wyj'#261'tki'
  ClientHeight = 211
  ClientWidth = 232
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel: TsBevel
    Left = 0
    Top = 173
    Width = 232
    Height = 38
    Align = alBottom
    Shape = bsTopLine
    ExplicitTop = 177
  end
  object SaveButton: TsButton
    Tag = 2
    Left = 153
    Top = 181
    Width = 75
    Height = 25
    Align = alCustom
    Anchors = [akRight, akBottom]
    Caption = 'Zapisz'
    Enabled = False
    TabOrder = 1
    TabStop = False
    OnClick = SaveButtonClick
    SkinData.SkinSection = 'BUTTON'
  end
  object TabControl: TsTabControl
    AlignWithMargins = True
    Left = 6
    Top = 6
    Width = 220
    Height = 161
    Margins.Left = 6
    Margins.Top = 6
    Margins.Right = 6
    Margins.Bottom = 6
    Align = alClient
    TabOrder = 0
    SkinData.SkinSection = 'PAGECONTROL'
    object ProcessListBox: TsListBox
      Tag = 4
      Left = 8
      Top = 27
      Width = 202
      Height = 96
      Align = alCustom
      Anchors = [akLeft, akTop, akRight, akBottom]
      ItemHeight = 13
      TabOrder = 0
      OnClick = ProcessListBoxClick
      BoundLabel.Active = True
      BoundLabel.Caption = 'Ignorowane aplikacje pe'#322'noekranowe:'
      BoundLabel.Indent = 6
      BoundLabel.Font.Charset = DEFAULT_CHARSET
      BoundLabel.Font.Color = clWindowText
      BoundLabel.Font.Height = -11
      BoundLabel.Font.Name = 'Tahoma'
      BoundLabel.Font.Style = []
      BoundLabel.Layout = sclTopCenter
      BoundLabel.MaxWidth = 0
      BoundLabel.UseSkinColor = True
      SkinData.SkinSection = 'EDIT'
    end
    object AddButton: TsButton
      Tag = 5
      Left = 8
      Top = 129
      Width = 75
      Height = 25
      Align = alCustom
      Anchors = [akLeft, akBottom]
      Caption = 'Dodaj'
      TabOrder = 1
      OnClick = AddButtonClick
      SkinData.SkinSection = 'BUTTON'
    end
    object DeleteButton: TsButton
      Tag = 6
      Left = 135
      Top = 129
      Width = 75
      Height = 25
      Align = alCustom
      Anchors = [akRight, akBottom]
      Caption = 'Usu'#324
      Enabled = False
      TabOrder = 2
      OnClick = DeleteButtonClick
      SkinData.SkinSection = 'BUTTON'
    end
  end
  object CancelButton: TsButton
    Tag = 3
    Left = 72
    Top = 181
    Width = 75
    Height = 25
    Align = alCustom
    Anchors = [akRight, akBottom]
    Caption = 'Anuluj'
    TabOrder = 2
    OnClick = aExitExecute
    SkinData.SkinSection = 'BUTTON'
  end
  object ActionList: TActionList
    Top = 176
    object aExit: TAction
      Caption = 'aExit'
      ShortCut = 27
      OnExecute = aExitExecute
    end
    object aSaveSettings: TAction
      Caption = 'aSaveSettings'
      OnExecute = aSaveSettingsExecute
    end
    object aLoadSettings: TAction
      Caption = 'aLoadSettings'
      OnExecute = aLoadSettingsExecute
    end
  end
  object sSkinProvider: TsSkinProvider
    AddedTitle.Font.Charset = DEFAULT_CHARSET
    AddedTitle.Font.Color = clNone
    AddedTitle.Font.Height = -11
    AddedTitle.Font.Name = 'Tahoma'
    AddedTitle.Font.Style = []
    SkinData.SkinSection = 'FORM'
    TitleButtons = <>
    Left = 32
    Top = 176
  end
end

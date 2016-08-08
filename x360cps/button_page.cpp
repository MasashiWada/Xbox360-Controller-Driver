
//	Property Sheet for Microsoft Xbox 360 Controller

#include	"button_page.h"
#include	"resource.h"

//----------------------------------------------------------------------------------------------
//	CButtonPage
//----------------------------------------------------------------------------------------------
CButtonPage::CButtonPage( CSetting * Setting )
{
	//	�ݒ�
	this->Setting		= Setting;

	//	���݂̃R���g���[���[�̐ݒ�
	CurrentSetting		= &Setting->CurrentSetting;
}

//----------------------------------------------------------------------------------------------
//	wmInitDialog
//----------------------------------------------------------------------------------------------
VOID CButtonPage::wmInitDialog( HWND Wnd )
{
	//	�E�B���h�E �n���h��
	this->Wnd	= Wnd;

	//	�{�^���̈ꗗ
	WORD	ButtonList[]	= {	 IDS_A_BUTTON
								,IDS_B_BUTTON
								,IDS_X_BUTTON
								,IDS_Y_BUTTON
								,IDS_LB_BUTTON
								,IDS_RB_BUTTON
								,IDS_START_BUTTON
								,IDS_BACK_BUTTON
								,IDS_LEFT_STICK_BUTTON
								,IDS_RIGHT_STICK_BUTTON
								,IDS_XBOX_GUIDE_BUTTON };
	WORD	ButtonListCount	= sizeof( ButtonList ) / sizeof( WORD ) ;

	//	�{�^���̃t�@���N�V����
	WORD	ButtonFunction[]	= {	 IDS_NONE
									,IDS_X_AXIS_PLUS
									,IDS_X_AXIS_MINUS
									,IDS_Y_AXIS_PLUS
									,IDS_Y_AXIS_MINUS
									,IDS_X_ROTATION_PLUS
									,IDS_X_ROTATION_MINUS
									,IDS_Y_ROTATION_PLUS
									,IDS_Y_ROTATION_MINUS
									,IDS_Z_AXIS_PLUS
									,IDS_Z_AXIS_MINUS
									,IDS_Z_ROTATION_PLUS
									,IDS_Z_ROTATION_MINUS
									,IDS_SLIDER1_PLUS
									,IDS_SLIDER1_MINUS
									,IDS_SLIDER2_PLUS
									,IDS_SLIDER2_MINUS
									,IDS_BUTTON1
									,IDS_BUTTON2
									,IDS_BUTTON3
									,IDS_BUTTON4
									,IDS_BUTTON5
									,IDS_BUTTON6
									,IDS_BUTTON7
									,IDS_BUTTON8
									,IDS_BUTTON9
									,IDS_BUTTON10
									,IDS_BUTTON11
									,IDS_BUTTON12
									,IDS_BUTTON13
									,IDS_BUTTON14
									,IDS_BUTTON15
									,IDS_BUTTON16
									,IDS_BUTTON17
									,IDS_BUTTON18
									,IDS_BUTTON19
									,IDS_BUTTON20
									,IDS_BUTTON21
									,IDS_BUTTON22
									,IDS_BUTTON23
									,IDS_BUTTON24
									,IDS_BUTTON25
									,IDS_BUTTON26
									,IDS_BUTTON27
									,IDS_BUTTON28
									,IDS_BUTTON29
									,IDS_BUTTON30
									,IDS_BUTTON31
									,IDS_BUTTON32 };
	WORD	ButtonFunctionCount	= sizeof( ButtonFunction ) / sizeof( WORD ) ;

	//	�����O ���C�g�̓_�Ńp�^�[��
	WORD	RingLightPattern[]	= {	 IDS_RING_LIGHT_0
									,IDS_RING_LIGHT_1
									,IDS_RING_LIGHT_2
									,IDS_RING_LIGHT_3
									,IDS_RING_LIGHT_4
									,IDS_RING_LIGHT_5
									,IDS_RING_LIGHT_6
									,IDS_RING_LIGHT_7
									,IDS_RING_LIGHT_8
									,IDS_RING_LIGHT_9
									,IDS_RING_LIGHT_10
									,IDS_RING_LIGHT_11
									,IDS_RING_LIGHT_12
									,IDS_RING_LIGHT_13
									,IDS_RING_LIGHT_14
									,IDS_RING_LIGHT_15 };
	WORD	RingLightPatternCount	= sizeof( RingLightPattern ) / sizeof( WORD ) ;

	//	�u�{�^���̈ꗗ�v
	InitListBox( IDC_BUTTON_LIST, ButtonList, ButtonListCount );
	SetListBoxIndex( IDC_BUTTON_LIST, 0 );
	//	�u�{�^���̋@�\�v
	InitComboBox( IDC_BUTTON_USAGE, ButtonFunction, ButtonFunctionCount );
	//	�u�{�^���̘A�ˑ��x�v
	InitTrackBar( IDC_BUTTON_RAPID_FIRE_SPEED, 0, 100, 20, 10 );

	//	�u�_�Ńp�^�[���̈ꗗ�v
	InitComboBox( IDC_RING_LIGHT_PATTERN, RingLightPattern, RingLightPatternCount );
}

//----------------------------------------------------------------------------------------------
//	wmCommand
//----------------------------------------------------------------------------------------------
VOID CButtonPage::wmCommand(
	 HWND	Control
	,WORD	ControlID
	,WORD	NotifyMessage )
{
	//	�R���g���[���ɂ���ď�����U�蕪����
	switch( ControlID )
	{
		case IDC_BUTTON_LIST:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case LBN_SELCHANGE:
					ApplySetting();
					break;
			}
			break;

		case IDC_BUTTON_USAGE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->ButtonUsage[GetListBoxIndex( IDC_BUTTON_LIST )]
						= GetComboBoxIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_BUTTON_RAPID_FIRE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->ButtonRapidFire[GetListBoxIndex( IDC_BUTTON_LIST )]
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_RING_LIGHT_PATTERN:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->RingLightPattern	= GetComboBoxIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	wmHScroll
//----------------------------------------------------------------------------------------------
VOID CButtonPage::wmHScroll(
	 HWND	Control
	,WORD	Request )
{
	//	�u�{�^���̘A�ˑ��x�v����̃��b�Z�[�W
	if( Control == GetDlgItem( Wnd, IDC_BUTTON_RAPID_FIRE_SPEED ) )
	{
		//	���݂̐ݒ�ɔ��f����
		CurrentSetting->ButtonRapidFireSpeed[GetListBoxIndex( IDC_BUTTON_LIST )]
			= GetTrackBarPos( IDC_BUTTON_RAPID_FIRE_SPEED );
		//	�ݒ�̕ύX��ʒm����
		SettingChanged();
	}
}

//----------------------------------------------------------------------------------------------
//	ApplySetting
//----------------------------------------------------------------------------------------------
VOID CButtonPage::ApplySetting()
{
	//	�u�{�^���̋@�\�v
	LONG	ListBoxIndex	= GetListBoxIndex( IDC_BUTTON_LIST );
	//	�u�{�^���̋@�\�v
	SetComboBoxIndex( IDC_BUTTON_USAGE, CurrentSetting->ButtonUsage[ListBoxIndex] );
	//	�u�{�^���̘A�˂��s���v
	SetCheckBoxState( IDC_BUTTON_RAPID_FIRE, CurrentSetting->ButtonRapidFire[ListBoxIndex] );
	//	�u�{�^���̘A�ˑ��x�v
	SetTrackBarPos(
		 IDC_BUTTON_RAPID_FIRE_SPEED
		,CurrentSetting->ButtonRapidFireSpeed[ListBoxIndex] );
	if( CurrentSetting->ButtonRapidFire[ListBoxIndex] == FALSE )
	{
		EnableControl( IDC_BUTTON_RAPID_FIRE_SPEED, FALSE );
	} else {
		EnableControl( IDC_BUTTON_RAPID_FIRE_SPEED, TRUE );
	}

	//	�u�_�Ńp�^�[���̈ꗗ�v
	SetComboBoxIndex( IDC_RING_LIGHT_PATTERN, CurrentSetting->RingLightPattern );
}

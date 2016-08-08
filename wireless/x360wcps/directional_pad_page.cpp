
//	Property Sheet for Microsoft Xbox 360 Wireless Controller

#include	"directional_pad_page.h"
#include	"resource.h"

//----------------------------------------------------------------------------------------------
//	CDirectionalPadPage
//----------------------------------------------------------------------------------------------
CDirectionalPadPage::CDirectionalPadPage( CSetting * Setting )
{
	//	�ݒ�
	this->Setting		= Setting;

	//	���݂̃R���g���[���[�̐ݒ�
	CurrentSetting		= &Setting->CurrentSetting;
}

//----------------------------------------------------------------------------------------------
//	wmInitDialog
//----------------------------------------------------------------------------------------------
VOID CDirectionalPadPage::wmInitDialog( HWND Wnd )
{
	//	�E�B���h�E �n���h��
	this->Wnd	= Wnd;

	//	�����p�b�h�̃t�@���N�V����
	WORD	DirectionalPadFunction[]	= {	 IDS_NONE
											,IDS_X_AXIS_Y_AXIS
											,IDS_X_AXIS_NONE
											,IDS_NONE_X_AXIS
											,IDS_Y_AXIS_X_AXIS
											,IDS_Y_AXIS_NONE
											,IDS_NONE_Y_AXIS
											,IDS_X_ROTATION_Y_ROTATION
											,IDS_X_ROTATION_NONE
											,IDS_NONE_X_ROTATION
											,IDS_Y_ROTATION_X_ROTATION
											,IDS_Y_ROTATION_NONE
											,IDS_NONE_Y_ROTATION
											,IDS_Z_AXIS_ROTATION
											,IDS_Z_AXIS_SLIDER1
											,IDS_Z_AXIS_SLIDER2
											,IDS_Z_AXIS_NONE
											,IDS_NONE_Z_AXIS
											,IDS_Z_ROTATION_AXIS
											,IDS_Z_ROTATION_SLIDER1
											,IDS_Z_ROTATION_SLIDER2
											,IDS_Z_ROTATION_NONE
											,IDS_NONE_Z_ROTATION
											,IDS_SLIDER1_Z_AXIS
											,IDS_SLIDER1_Z_ROTATION
											,IDS_SLIDER1_SLIDER2
											,IDS_SLIDER1_NONE
											,IDS_NONE_SLIDER1
											,IDS_SLIDER2_Z_AXIS
											,IDS_SLIDER2_Z_ROTATION
											,IDS_SLIDER2_SLIDER1
											,IDS_SLIDER2_NONE
											,IDS_NONE_SLIDER2
											,IDS_HAT_SWITCH1
											,IDS_HAT_SWITCH2
											,IDS_HAT_SWITCH3
											,IDS_HAT_SWITCH4
											,IDS_4WAY_BUTTON };
	WORD	DirectionalPadFunctionCount	= sizeof( DirectionalPadFunction ) / sizeof( WORD ) ;

	//	�����ʂ̃t�@���N�V����
	WORD	WayButtonFunction[]			= {	 IDS_NONE
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
	WORD	WayButtonFunctionCount	= sizeof( WayButtonFunction ) / sizeof( WORD ) ;

	//	�u�����p�b�h�̋@�\�v
	InitComboBox(
		 IDC_DIRECTIONAL_PAD_USAGE
		,DirectionalPadFunction
		,DirectionalPadFunctionCount );
	//	�u�����p�b�h��̋@�\�v
	InitComboBox( IDC_DIRECTIONAL_PAD_UP_USAGE, WayButtonFunction, WayButtonFunctionCount );
	//	�u�����p�b�h���̋@�\�v
	InitComboBox( IDC_DIRECTIONAL_PAD_DOWN_USAGE, WayButtonFunction, WayButtonFunctionCount );
	//	�u�����p�b�h���̋@�\�v
	InitComboBox( IDC_DIRECTIONAL_PAD_LEFT_USAGE, WayButtonFunction, WayButtonFunctionCount );
	//	�u�����p�b�h�E�̋@�\�v
	InitComboBox( IDC_DIRECTIONAL_PAD_RIGHT_USAGE, WayButtonFunction, WayButtonFunctionCount );
}

//----------------------------------------------------------------------------------------------
//	wmCommand
//----------------------------------------------------------------------------------------------
VOID CDirectionalPadPage::wmCommand(
	 HWND	Control
	,WORD	ControlID
	,WORD	NotifyMessage )
{
	//	�R���g���[���ɂ���ď�����U�蕪����
	switch( ControlID )
	{
		case IDC_DIRECTIONAL_PAD_USAGE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DirectionalPadUsage	= GetComboBoxIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_DIRECTIONAL_PAD_UP_USAGE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_UP]
						= GetWayUsageByControlIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_DIRECTIONAL_PAD_DOWN_USAGE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_DOWN]
						= GetWayUsageByControlIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_DIRECTIONAL_PAD_LEFT_USAGE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_LEFT]
						= GetWayUsageByControlIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_DIRECTIONAL_PAD_RIGHT_USAGE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_RIGHT]
						= GetWayUsageByControlIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_DIRECTIONAL_PAD_REVERSE_Y:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DirectionalPadReverseY	= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_DIRECTIONAL_PAD_UP_RAPID_FIRE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DirectionalPadWayRapidFire[INDEX_WAY_UP]
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_DIRECTIONAL_PAD_DOWN_RAPID_FIRE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DirectionalPadWayRapidFire[INDEX_WAY_DOWN]
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_DIRECTIONAL_PAD_LEFT_RAPID_FIRE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DirectionalPadWayRapidFire[INDEX_WAY_LEFT]
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_DIRECTIONAL_PAD_RIGHT_RAPID_FIRE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DirectionalPadWayRapidFire[INDEX_WAY_RIGHT]
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	ApplySetting
//----------------------------------------------------------------------------------------------
VOID CDirectionalPadPage::ApplySetting()
{
	//	�u�����p�b�h�̋@�\�v
	SetComboBoxIndex( IDC_DIRECTIONAL_PAD_USAGE, CurrentSetting->DirectionalPadUsage );
	//	�u�㉺�𔽓]����v
	SetCheckBoxState( IDC_DIRECTIONAL_PAD_REVERSE_Y, CurrentSetting->DirectionalPadReverseY );
	//	�u�����p�b�h�̋@�\�v���u�Ȃ��v�̏ꍇ�́A�u�㉺�𔽓]����v�𖳌��ɂ���
	if( CurrentSetting->DirectionalPadUsage == USAGE_NONE )
	{
		EnableControl( IDC_DIRECTIONAL_PAD_REVERSE_Y, FALSE );
	} else {
		EnableControl( IDC_DIRECTIONAL_PAD_REVERSE_Y, TRUE );
	}

	//	�u�����p�b�h��̋@�\�v
	SetComboBoxIndex(
		 IDC_DIRECTIONAL_PAD_UP_USAGE
		,GetControlIndexByWayUsage( CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_UP] ) );
	//	�u�����p�b�h��̘A�˂��s���v
	SetCheckBoxState(
		 IDC_DIRECTIONAL_PAD_UP_RAPID_FIRE
		,CurrentSetting->DirectionalPadWayRapidFire[INDEX_WAY_UP] );

	//	�u�����p�b�h���̋@�\�v
	SetComboBoxIndex(
		 IDC_DIRECTIONAL_PAD_DOWN_USAGE
		,GetControlIndexByWayUsage( CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_DOWN] ) );
	//	�u�����p�b�h���̘A�˂��s���v
	SetCheckBoxState(
		 IDC_DIRECTIONAL_PAD_DOWN_RAPID_FIRE
		,CurrentSetting->DirectionalPadWayRapidFire[INDEX_WAY_DOWN] );

	//	�u�����p�b�h���̋@�\�v
	SetComboBoxIndex(
		 IDC_DIRECTIONAL_PAD_LEFT_USAGE
		,GetControlIndexByWayUsage( CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_LEFT] ) );
	//	�u�����p�b�h���̘A�˂��s���v
	SetCheckBoxState(
		 IDC_DIRECTIONAL_PAD_LEFT_RAPID_FIRE
		,CurrentSetting->DirectionalPadWayRapidFire[INDEX_WAY_LEFT] );

	//	�u�����p�b�h�E�̋@�\�v
	SetComboBoxIndex(
		 IDC_DIRECTIONAL_PAD_RIGHT_USAGE
		,GetControlIndexByWayUsage( CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_RIGHT] ) );
	//	�u�����p�b�h�E�̘A�˂��s���v
	SetCheckBoxState(
		 IDC_DIRECTIONAL_PAD_RIGHT_RAPID_FIRE
		,CurrentSetting->DirectionalPadWayRapidFire[INDEX_WAY_RIGHT] );

	//	�u�����p�b�h�̋@�\�v���u�����ʂɊ��蓖�āv�̈ȊO�̏ꍇ�́A�����𖳌��ɂ���
	if( CurrentSetting->DirectionalPadUsage != USAGE_4WAY_BUTTON )
	{
		EnableControls( IDC_STATIC_301, IDC_DIRECTIONAL_PAD_RIGHT_RAPID_FIRE, FALSE );
	} else {
		EnableControls( IDC_STATIC_301, IDC_DIRECTIONAL_PAD_RIGHT_RAPID_FIRE, TRUE );
		//	�u�����p�b�h��̋@�\�v���u�Ȃ��v�̏ꍇ�́A�u�����p�b�h��̘A�˂��s���v�𖳌��ɂ���
		if( CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_UP] == USAGE_NONE )
		{
			EnableControl( IDC_DIRECTIONAL_PAD_UP_RAPID_FIRE, FALSE );
		} else {
			EnableControl( IDC_DIRECTIONAL_PAD_UP_RAPID_FIRE, TRUE );
		}
		//	�u�����p�b�h���̋@�\�v���u�Ȃ��v�̏ꍇ�́A�u�����p�b�h���̘A�˂��s���v�𖳌��ɂ���
		if( CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_DOWN] == USAGE_NONE )
		{
			EnableControl( IDC_DIRECTIONAL_PAD_DOWN_RAPID_FIRE, FALSE );
		} else {
			EnableControl( IDC_DIRECTIONAL_PAD_DOWN_RAPID_FIRE, TRUE );
		}
		//	�u�����p�b�h���̋@�\�v���u�Ȃ��v�̏ꍇ�́A�u�����p�b�h���̘A�˂��s���v�𖳌��ɂ���
		if( CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_LEFT] == USAGE_NONE )
		{
			EnableControl( IDC_DIRECTIONAL_PAD_LEFT_RAPID_FIRE, FALSE );
		} else {
			EnableControl( IDC_DIRECTIONAL_PAD_LEFT_RAPID_FIRE, TRUE );
		}
		//	�u�����p�b�h�E�̋@�\�v���u�Ȃ��v�̏ꍇ�́A�u�����p�b�h�E�̘A�˂��s���v�𖳌��ɂ���
		if( CurrentSetting->DirectionalPadWayUsage[INDEX_WAY_RIGHT] == USAGE_NONE )
		{
			EnableControl( IDC_DIRECTIONAL_PAD_RIGHT_RAPID_FIRE, FALSE );
		} else {
			EnableControl( IDC_DIRECTIONAL_PAD_RIGHT_RAPID_FIRE, TRUE );
		}
	}
}

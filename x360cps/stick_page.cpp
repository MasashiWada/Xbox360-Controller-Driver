
//	Property Sheet for Microsoft Xbox 360 Controller

#include	"stick_page.h"
#include	"resource.h"

//----------------------------------------------------------------------------------------------
//	CStickPage
//----------------------------------------------------------------------------------------------
CStickPage::CStickPage( CSetting * Setting )
{
	//	�ݒ�
	this->Setting		= Setting;

	//	���݂̃R���g���[���[�̐ݒ�
	CurrentSetting		= &Setting->CurrentSetting;

	//	�g���b�N �o�[�̑I�����
	TrackBarSelecting	= FALSE;
}

//----------------------------------------------------------------------------------------------
//	wmInitDialog
//----------------------------------------------------------------------------------------------
VOID CStickPage::wmInitDialog( HWND Wnd )
{
	//	�E�B���h�E �n���h��
	this->Wnd	= Wnd;

	//	�X�e�B�b�N�̃t�@���N�V����
	WORD	StickFunction[]	= {	 IDS_NONE
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
								,IDS_HAT_SWITCH4 };
	WORD	StickFunctionCount	= sizeof( StickFunction ) / sizeof( WORD ) ;

	//	�u�X�e�B�b�N�̋@�\�i���j�v
	InitComboBox( IDC_LEFT_STICK_USAGE, StickFunction, StickFunctionCount );
	//	�u�X�e�B�b�N�̋@�\�i�E�j�v
	InitComboBox( IDC_RIGHT_STICK_USAGE, StickFunction, StickFunctionCount );
	//	�u�L���͈́v
	InitTrackBar( IDC_STICK_THRESHOLD, 0, 100, 10, 10 );
	//	�u�X�e�B�b�N�̓���臒l�v
	InitTrackBar( IDC_STICK_HAT_SWITCH_THRESHOLD, 0, 100, 10, 10 );
}

//----------------------------------------------------------------------------------------------
//	wmCommand
//----------------------------------------------------------------------------------------------
VOID CStickPage::wmCommand(
	 HWND	Control
	,WORD	ControlID
	,WORD	NotifyMessage )
{
	//	�R���g���[���ɂ���ď�����U�蕪����
	switch( ControlID )
	{
		case IDC_LEFT_STICK_USAGE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->StickUsage[INDEX_LEFT_STICK]
						= GetComboBoxIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_RIGHT_STICK_USAGE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->StickUsage[INDEX_RIGHT_STICK]
						= GetComboBoxIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_LEFT_STICK_REVERSE_Y:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->StickReverseY[INDEX_LEFT_STICK]
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_RIGHT_STICK_REVERSE_Y:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->StickReverseY[INDEX_RIGHT_STICK]
						= GetCheckBoxState( ControlID );
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
VOID CStickPage::wmHScroll(
	 HWND	Control
	,WORD	Request )
{
	//	�u�L���͈́v����̃��b�Z�[�W
	if( Control == GetDlgItem( Wnd, IDC_STICK_THRESHOLD ) )
	{
		//	���݂̑I����Ԃ�ޔ�����
		BOOL	PrevSelecting	= TrackBarSelecting;

		//	�g���b�N �o�[�͈̔͑I�����s��
		SelectTrackBar( Control, Request, &TrackBarSelecting );
		if( PrevSelecting != TrackBarSelecting )
		{
			//	���݂̐ݒ�ɔ��f����
			GetTrackBarRange(
				 IDC_STICK_THRESHOLD
				,&CurrentSetting->StickMinThreshold[INDEX_LEFT_STICK]
				,&CurrentSetting->StickMaxThreshold[INDEX_LEFT_STICK] );
			CurrentSetting->StickMinThreshold[INDEX_RIGHT_STICK]
				= CurrentSetting->StickMinThreshold[INDEX_LEFT_STICK];
			CurrentSetting->StickMaxThreshold[INDEX_RIGHT_STICK]
				= CurrentSetting->StickMaxThreshold[INDEX_LEFT_STICK];
			//	�ݒ�̕ύX��ʒm����
			SettingChanged();
		}
	}

	//	�u�X�e�B�b�N�̓���臒l�v����̃��b�Z�[�W
	if( Control == GetDlgItem( Wnd, IDC_STICK_HAT_SWITCH_THRESHOLD ) )
	{
		//	���݂̐ݒ�ɔ��f����
		CurrentSetting->StickHatSwitchThreshold[INDEX_LEFT_STICK]
			= GetTrackBarPos( IDC_STICK_HAT_SWITCH_THRESHOLD );
		CurrentSetting->StickHatSwitchThreshold[INDEX_RIGHT_STICK]
			= CurrentSetting->StickHatSwitchThreshold[INDEX_LEFT_STICK];
		//	�ݒ�̕ύX��ʒm����
		SettingChanged();
	}
}

//----------------------------------------------------------------------------------------------
//	ApplySetting
//----------------------------------------------------------------------------------------------
VOID CStickPage::ApplySetting()
{
	//	�u�X�e�B�b�N�̋@�\�i���j�v
	SetComboBoxIndex( IDC_LEFT_STICK_USAGE, CurrentSetting->StickUsage[INDEX_LEFT_STICK] );
	//	�u�㉺�𔽓]����i���j�v
	SetCheckBoxState(
		 IDC_LEFT_STICK_REVERSE_Y
		,CurrentSetting->StickReverseY[INDEX_LEFT_STICK] );
	//	�u�X�e�B�b�N�̋@�\�i���j�v���u�Ȃ��v�̏ꍇ�́A�u�㉺�𔽓]����i���j�v�𖳌��ɂ���
	if( CurrentSetting->StickUsage[INDEX_LEFT_STICK] == USAGE_NONE )
	{
		EnableControl( IDC_LEFT_STICK_REVERSE_Y, FALSE );
	} else {
		EnableControl( IDC_LEFT_STICK_REVERSE_Y, TRUE );
	}

	//	�u�X�e�B�b�N�̋@�\�i�E�j�v
	SetComboBoxIndex( IDC_RIGHT_STICK_USAGE, CurrentSetting->StickUsage[INDEX_RIGHT_STICK] );
	//	�u�㉺�𔽓]����i�E�j�v
	SetCheckBoxState(
		 IDC_RIGHT_STICK_REVERSE_Y
		,CurrentSetting->StickReverseY[INDEX_RIGHT_STICK] );
	//	�u�X�e�B�b�N�̋@�\�i�E�j�v���u�Ȃ��v�̏ꍇ�́A�u�㉺�𔽓]����i�E�j�v�𖳌��ɂ���
	if( CurrentSetting->StickUsage[INDEX_RIGHT_STICK] == USAGE_NONE )
	{
		EnableControl( IDC_RIGHT_STICK_REVERSE_Y, FALSE );
	} else {
		EnableControl( IDC_RIGHT_STICK_REVERSE_Y, TRUE );
	}

	//	�u�L���͈́v
	SetTrackBarRange(
		 IDC_STICK_THRESHOLD
		,CurrentSetting->StickMinThreshold[INDEX_LEFT_STICK]
		,CurrentSetting->StickMaxThreshold[INDEX_LEFT_STICK] );
	SetTrackBarPos( IDC_STICK_THRESHOLD, CurrentSetting->StickMinThreshold[INDEX_LEFT_STICK] );
	//	�u�X�e�B�b�N�̓���臒l�v
	SetTrackBarPos(
		 IDC_STICK_HAT_SWITCH_THRESHOLD
		,CurrentSetting->StickHatSwitchThreshold[INDEX_LEFT_STICK] );
}

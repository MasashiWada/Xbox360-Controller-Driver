
//	Property Sheet for Microsoft Xbox 360 Controller

#include	"trigger_page.h"
#include	"resource.h"

//----------------------------------------------------------------------------------------------
//	CTriggerPage
//----------------------------------------------------------------------------------------------
CTriggerPage::CTriggerPage( CSetting * Setting )
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
VOID CTriggerPage::wmInitDialog( HWND Wnd )
{
	//	�E�B���h�E �n���h��
	this->Wnd	= Wnd;

	//	�g���K�[�̃t�@���N�V����
	WORD	TriggerFunction[]	= {	 IDS_NONE
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
	WORD	TriggerFunctionCount	= sizeof( TriggerFunction ) / sizeof( WORD ) ;

	//	�u�g���K�[�̋@�\�i���j�v
	InitComboBox( IDC_LEFT_TRIGGER_USAGE, TriggerFunction, TriggerFunctionCount );
	//	�u�g���K�[�̋@�\�i�E�j�v
	InitComboBox( IDC_RIGHT_TRIGGER_USAGE, TriggerFunction, TriggerFunctionCount );
	//	�u�ڍאݒ�v
	InitTrackBar( IDC_TRIGGER_THRESHOLD, 0, 100, 10, 10 );
	//	�u�g���K�[�̓���臒l�v
	InitTrackBar( IDC_TRIGGER_BUTTON_THRESHOLD, 0, 100, 10, 10 );
}

//----------------------------------------------------------------------------------------------
//	wmCommand
//----------------------------------------------------------------------------------------------
VOID CTriggerPage::wmCommand(
	 HWND	Control
	,WORD	ControlID
	,WORD	NotifyMessage )
{
	//	�R���g���[���ɂ���ď�����U�蕪����
	switch( ControlID )
	{
		case IDC_LEFT_TRIGGER_USAGE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->TriggerUsage[INDEX_LEFT_TRIGGER]
						= GetWayUsageByControlIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_RIGHT_TRIGGER_USAGE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->TriggerUsage[INDEX_RIGHT_TRIGGER]
						= GetWayUsageByControlIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_LEFT_TRIGGER_RAPID_FIRE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->TriggerRapidFire[INDEX_LEFT_TRIGGER]
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_RIGHT_TRIGGER_RAPID_FIRE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->TriggerRapidFire[INDEX_RIGHT_TRIGGER]
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
VOID CTriggerPage::wmHScroll(
	 HWND	Control
	,WORD	Request )
{
	//	�u�L���͈́v����̃��b�Z�[�W
	if( Control == GetDlgItem( Wnd, IDC_TRIGGER_THRESHOLD ) )
	{
		//	���݂̑I����Ԃ�ޔ�����
		BOOL	PrevSelecting	= TrackBarSelecting;

		//	�g���b�N �o�[�͈̔͑I�����s��
		SelectTrackBar( Control, Request, &TrackBarSelecting );
		if( PrevSelecting != TrackBarSelecting )
		{
			//	���݂̐ݒ�ɔ��f����
			GetTrackBarRange(
				 IDC_TRIGGER_THRESHOLD
				,&CurrentSetting->TriggerMinThreshold[INDEX_LEFT_TRIGGER]
				,&CurrentSetting->TriggerMaxThreshold[INDEX_LEFT_TRIGGER] );
			CurrentSetting->TriggerMinThreshold[INDEX_RIGHT_TRIGGER]
				= CurrentSetting->TriggerMinThreshold[INDEX_LEFT_TRIGGER];
			CurrentSetting->TriggerMaxThreshold[INDEX_RIGHT_TRIGGER]
				= CurrentSetting->TriggerMaxThreshold[INDEX_LEFT_TRIGGER];
			//	�ݒ�̕ύX��ʒm����
			SettingChanged();
		}
	}

	//	�u�g���K�[�̓���臒l�v����̃��b�Z�[�W
	if( Control == GetDlgItem( Wnd, IDC_TRIGGER_BUTTON_THRESHOLD ) )
	{
		//	���݂̐ݒ�ɔ��f����
		CurrentSetting->TriggerButtonThreshold[INDEX_LEFT_TRIGGER]
			= GetTrackBarPos( IDC_TRIGGER_BUTTON_THRESHOLD );
		CurrentSetting->TriggerButtonThreshold[INDEX_RIGHT_TRIGGER]
			= CurrentSetting->TriggerButtonThreshold[INDEX_LEFT_TRIGGER];
		//	�ݒ�̕ύX��ʒm����
		SettingChanged();
	}
}

//----------------------------------------------------------------------------------------------
//	ApplySetting
//----------------------------------------------------------------------------------------------
VOID CTriggerPage::ApplySetting()
{
	//	�u�g���K�[�̋@�\�i���j�v
	SetComboBoxIndex(
		 IDC_LEFT_TRIGGER_USAGE
		,GetControlIndexByWayUsage( CurrentSetting->TriggerUsage[INDEX_LEFT_TRIGGER] ) );
	//	�u�g���K�[�̘A�˂��s���i���j�v
	SetCheckBoxState(
		 IDC_LEFT_TRIGGER_RAPID_FIRE
		,CurrentSetting->TriggerRapidFire[INDEX_LEFT_TRIGGER] );
	//	�u�g���K�[�̋@�\�i���j�v���u�Ȃ��v�̏ꍇ�́A�u�g���K�[�̘A�˂��s���i���j�v�𖳌��ɂ���
	if( CurrentSetting->TriggerUsage[INDEX_LEFT_TRIGGER] == USAGE_NONE )
	{
		EnableControl( IDC_LEFT_TRIGGER_RAPID_FIRE, FALSE );
	} else {
		EnableControl( IDC_LEFT_TRIGGER_RAPID_FIRE, TRUE );
	}

	//	�u�g���K�[�̋@�\�i�E�j�v
	SetComboBoxIndex(
		 IDC_RIGHT_TRIGGER_USAGE
		,GetControlIndexByWayUsage( CurrentSetting->TriggerUsage[INDEX_RIGHT_TRIGGER] ) );
	//	�u�g���K�[�̘A�˂��s���i�E�j�v
	SetCheckBoxState(
		 IDC_RIGHT_TRIGGER_RAPID_FIRE
		,CurrentSetting->TriggerRapidFire[INDEX_RIGHT_TRIGGER] );
	//	�u�g���K�[�̋@�\�i�E�j�v���u�Ȃ��v�̏ꍇ�́A�u�g���K�[�̘A�˂��s���i�E�j�v�𖳌��ɂ���
	if( CurrentSetting->TriggerUsage[INDEX_RIGHT_TRIGGER] == USAGE_NONE )
	{
		EnableControl( IDC_RIGHT_TRIGGER_RAPID_FIRE, FALSE );
	} else {
		EnableControl( IDC_RIGHT_TRIGGER_RAPID_FIRE, TRUE );
	}

	//	�u�L���͈́v
	SetTrackBarRange(
		 IDC_TRIGGER_THRESHOLD
		,CurrentSetting->TriggerMinThreshold[INDEX_LEFT_TRIGGER]
		,CurrentSetting->TriggerMaxThreshold[INDEX_LEFT_TRIGGER] );
	SetTrackBarPos(
		 IDC_TRIGGER_THRESHOLD
		,CurrentSetting->TriggerMinThreshold[INDEX_LEFT_TRIGGER] );
	//	�u�g���K�[�̓���臒l�v
	SetTrackBarPos(
		 IDC_TRIGGER_BUTTON_THRESHOLD
		,CurrentSetting->TriggerButtonThreshold[INDEX_LEFT_TRIGGER] );
}

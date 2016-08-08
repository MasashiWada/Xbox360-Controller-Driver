
//	Property Sheet for Microsoft Xbox 360 Controller

#include	"actuator_page.h"
#include	"resource.h"

//----------------------------------------------------------------------------------------------
//	CActuatorPage
//----------------------------------------------------------------------------------------------
CActuatorPage::CActuatorPage( CSetting * Setting )
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
VOID CActuatorPage::wmInitDialog( HWND Wnd )
{
	//	�E�B���h�E �n���h��
	this->Wnd	= Wnd;

	//	�u���A�N�`���G�[�^�v
	InitTrackBar( IDC_LEFT_ACTUATOR_LEVEL, 0, 100, 10, 10 );
	//	�u�E�A�N�`���G�[�^�v
	InitTrackBar( IDC_RIGHT_ACTUATOR_LEVEL, 0, 100, 10, 10 );
	//	�u�U���̗L���͈́v
	InitTrackBar( IDC_ACTUATOR_THRESHOLD, 0, 100, 10, 10 );
}

//----------------------------------------------------------------------------------------------
//	wmCommand
//----------------------------------------------------------------------------------------------
VOID CActuatorPage::wmCommand(
	 HWND	Control
	,WORD	ControlID
	,WORD	NotifyMessage )
{
	//	�R���g���[���ɂ���ď�����U�蕪����
	switch( ControlID )
	{
		case IDC_ACTUATOR:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->Actuator	= GetCheckBoxState( ControlID );
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
VOID CActuatorPage::wmHScroll(
	 HWND	Control
	,WORD	Request )
{
	//	�u���A�N�`���G�[�^�v����̃��b�Z�[�W
	if( Control == GetDlgItem( Wnd, IDC_LEFT_ACTUATOR_LEVEL ) )
	{
		//	���݂̐ݒ�ɔ��f����
		CurrentSetting->ActuatorLevel[INDEX_LEFT_ACTUATOR]
			= GetTrackBarPos( IDC_LEFT_ACTUATOR_LEVEL );
		//	�ݒ�̕ύX��ʒm����
		SettingChanged();
	}

	//	�u�E�A�N�`���G�[�^�v����̃��b�Z�[�W
	if( Control == GetDlgItem( Wnd, IDC_RIGHT_ACTUATOR_LEVEL ) )
	{
		//	���݂̐ݒ�ɔ��f����
		CurrentSetting->ActuatorLevel[INDEX_RIGHT_ACTUATOR]
			= GetTrackBarPos( IDC_RIGHT_ACTUATOR_LEVEL );
		//	�ݒ�̕ύX��ʒm����
		SettingChanged();
	}

	//	�u�U���̗L���͈́v����̃��b�Z�[�W
	if( Control == GetDlgItem( Wnd, IDC_ACTUATOR_THRESHOLD ) )
	{
		//	���݂̑I����Ԃ�ޔ�����
		BOOL	PrevSelecting	= TrackBarSelecting;

		//	�g���b�N �o�[�͈̔͑I�����s��
		SelectTrackBar( Control, Request, &TrackBarSelecting );
		if( PrevSelecting != TrackBarSelecting )
		{
			//	���݂̐ݒ�ɔ��f����
			GetTrackBarRange(
				 IDC_ACTUATOR_THRESHOLD
				,&CurrentSetting->ActuatorMinThreshold
				,&CurrentSetting->ActuatorMaxThreshold );
			//	�ݒ�̕ύX��ʒm����
			SettingChanged();
		}
	}
}

//----------------------------------------------------------------------------------------------
//	ApplySetting
//----------------------------------------------------------------------------------------------
VOID CActuatorPage::ApplySetting()
{
	//	�u�R���g���[���̐U���v
	SetCheckBoxState( IDC_ACTUATOR, CurrentSetting->Actuator );
	if( CurrentSetting->Actuator == TRUE )
	{
		EnableControls( IDC_STATIC_601, IDC_STATIC_613, TRUE );
	} else {
		EnableControls( IDC_STATIC_601, IDC_STATIC_613, FALSE );
	}

	//	�u���A�N�`���G�[�^�v
	SetTrackBarPos(
		 IDC_LEFT_ACTUATOR_LEVEL
		,CurrentSetting->ActuatorLevel[INDEX_LEFT_ACTUATOR] );
	//	�u�E�A�N�`���G�[�^�v
	SetTrackBarPos(
		 IDC_RIGHT_ACTUATOR_LEVEL
		,CurrentSetting->ActuatorLevel[INDEX_RIGHT_ACTUATOR] );

	//	�u�U���̗L���͈́v
	SetTrackBarRange(
		 IDC_ACTUATOR_THRESHOLD
		,CurrentSetting->ActuatorMinThreshold
		,CurrentSetting->ActuatorMaxThreshold );
	SetTrackBarPos( IDC_ACTUATOR_THRESHOLD, CurrentSetting->ActuatorMinThreshold );
}

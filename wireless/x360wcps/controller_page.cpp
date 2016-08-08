
//	Property Sheet for Microsoft Xbox 360 Wireless Controller

#include	"controller_page.h"
#include	"extension.h"
#include	"save_dialog.h"
#include	"resource.h"

//----------------------------------------------------------------------------------------------
//	CControllerPage
//----------------------------------------------------------------------------------------------
CControllerPage::CControllerPage( CSetting * Setting )
{
	//	�ݒ�
	this->Setting	= Setting;

	//	���݂̃R���g���[���[�̐ݒ�
	CurrentSetting	= &Setting->CurrentSetting;

	//	�f�o�C�X�ɓK�p����Ă���Z�b�e�B���O��ޔ�����
	CopyMemory( &DeviceSetting, CurrentSetting, sizeof( SETTING ) );

	//	�e�C���X�^���X���쐬����
	Device			= new CDevice();
	Report			= new CReport();
}

//----------------------------------------------------------------------------------------------
//	wmInitDialog
//----------------------------------------------------------------------------------------------
VOID CControllerPage::wmInitDialog( HWND Wnd )
{
	//	�E�B���h�E �n���h��
	this->Wnd	= Wnd;

	//	�X�e�[�^�X �G���A �A�C�R���Ƃ̔r��������s��
	Mutex	= CreateMutex( NULL, TRUE, X360WCPS_MUTEX );

	//	�u�U���v
	InitTrackBar( IDC_FORCE_FEEDBACK, 0, 100, 10, 10 );

	//	�|�[�����O�p�^�C�}�[���J�n����
	SetTimer( Wnd, POLLING_TIMER_ID, POLLING_INTERVAL, NULL );
}

//----------------------------------------------------------------------------------------------
//	wmCommand
//----------------------------------------------------------------------------------------------
VOID CControllerPage::wmCommand(
	 HWND	Control
	,WORD	ControlID
	,WORD	NotifyMessage )
{
	//	�R���g���[���ɂ���ď�����U�蕪����
	switch( ControlID )
	{
		case IDC_SETTING:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	�ݒ��ύX����
					ChangeSetting();
					break;
			}
			break;

		case IDC_SETTING_SAVE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	�ݒ��ۑ�����
					SaveSetting();
					break;
			}
			break;

		case IDC_SETTING_DELETE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	�ݒ���폜����
					DeleteSetting();
					break;
			}
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	wmNotify
//----------------------------------------------------------------------------------------------
VOID CControllerPage::wmNotify( NMHDR * Notify )
{
	//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
	switch( Notify->code )
	{
		case PSN_APPLY:
			//	�݊����̐ݒ肪�ς���Ă���΃_�C�A���O��\������
			if( ( DeviceSetting.ControllerCount	!= CurrentSetting->ControllerCount )
			||	( DeviceSetting.LimitationMode	!= CurrentSetting->LimitationMode ) )
			{
				ShowMessageBox(
					 IDS_RESTART_TITLE
					,IDS_RESTART_MESSAGE
					,NULL
					,MB_OK | MB_ICONINFORMATION );
			}

			//	�R���g���[���[�̐ݒ��ύX����
			Setting->SaveRegistry();
			Device->Apply();

			//	�X�e�[�^�X �G���A �A�C�R���ɕύX��ʒm����
			SendMessage(
				 FindWindow( X360WCSAI_CLASS_NAME, NULL )
				,WM_STATUS_AREA_ICON_NOTIFY
				,UPDATE_SETTING
				,NULL );

			//	�v���p�e�B �V�[�g�����
			if( ((PSHNOTIFY *)Notify)->lParam == TRUE )
			{
				ClosePage();
			}
			break;

		case PSN_RESET:
			//	�v���p�e�B �V�[�g�����
			ClosePage();
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	wmTimer
//----------------------------------------------------------------------------------------------
VOID CControllerPage::wmTimer( WPARAM TimerId )
{
	switch( TimerId )
	{
		case POLLING_TIMER_ID:
			//	�|�[�����O�p�^�C�}�[���~����
			KillTimer( Wnd, TimerId );

			//	�R���g���[���[�̏�Ԃ�\������
			ShowControllerState();

			//	�U�����e�X�g����
			if( CurrentSetting->Actuator == TRUE )
			{
				//	�e�X�g�l���擾����
				LONG TestLevel	= (LONG)SendMessage(
									 GetDlgItem( Wnd, IDC_FORCE_FEEDBACK )
									,TBM_GETPOS
									,0
									,0 ) * 255 / 100;
				//	�U�����e�X�g����
				TestForceFeedback( TestLevel );
			} else {
				//	�U�����e�X�g����
				TestForceFeedback( 0 );
			}

			//	�|�[�����O�p�^�C�}�[���J�n����
			SetTimer( Wnd, POLLING_TIMER_ID, POLLING_INTERVAL, NULL );
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	wmPageChanged
//----------------------------------------------------------------------------------------------
VOID CControllerPage::wmPageChanged()
{
	//	�ύX���ꂽ�ݒ�𔽉f����
	ApplyChangedSetting();
}

//----------------------------------------------------------------------------------------------
//	ApplySetting
//----------------------------------------------------------------------------------------------
VOID CControllerPage::ApplySetting()
{
	//	�ϐ��錾
	BOOL	Different	= FALSE;

	//	�R���{ �{�b�N�X�̓��e���قȂ邩�`�F�b�N����
	HWND	ComboBox		= GetDlgItem( Wnd, IDC_SETTING );
	LONG	ItemCount		= (LONG)SendMessage( ComboBox, CB_GETCOUNT, 0, 0 );
	if( Setting->ModifiedFlag == TRUE )
	{
		ItemCount --;
	}
	if( ItemCount == Setting->SettingCount )
	{
		//	�A�C�e���̕�������`�F�b�N����
		for( LONG Index = 0; Index < Setting->SettingCount; Index ++ )
		{
			//	�R���{ �{�b�N�X�̕�������擾����
			WCHAR	Text[MAX_PATH]	= { '\n' };
			if( Setting->ModifiedFlag == TRUE )
			{
				SendMessage( ComboBox, CB_GETLBTEXT, Index + 1, (LPARAM)Text );
			} else {
				SendMessage( ComboBox, CB_GETLBTEXT, Index, (LPARAM)Text );
			}
			if( wcscmp( Setting->SettingName[Index], Text ) != 0 )
			{
				Different	= TRUE;
				break;
			}
		}
	} else {
		Different	= TRUE;
	}

	//	�R���{ �{�b�N�X�̓��e��ݒ肷��
	if( Different == TRUE )
	{
		//	�R���{ �{�b�N�X�̓��e�����Z�b�g����
		SendMessage( ComboBox, CB_RESETCONTENT, 0, 0 );

		//	�ݒ薼�̂��R���{ �{�b�N�X�ɑ}������
		for( LONG Index = 0; Index < Setting->SettingCount; Index ++ )
		{
			SendMessage(
				 ComboBox
				,CB_INSERTSTRING
				,Index
				,(LPARAM)Setting->SettingName[Index] );
		}

		//	�ݒ肪�ύX�ς݂Ȃ��ԏ�Ɂu�` (�ύX�ς�)�v��}������
		if( Setting->ModifiedFlag == TRUE )
		{
			//	�ݒ薼�̂��擾����
			WCHAR	SettingName[MAX_PATH * 2];
			wcscpy_s( SettingName, Setting->SettingName[Setting->CurrentSettingIndex] );

			//	���\�[�X����u(�ύX�ς�)�v���擾���A�ݒ薼�̂ɕt������
			WCHAR	Modified[MAX_PATH];
			LoadString( Instance, IDS_MODIFIED, Modified, MAX_PATH );
			wcscat_s( SettingName, Modified );

			//	�ύX�ς݂̐ݒ薼�̂��R���{ �{�b�N�X�̈�ԏ�ɑ}������
			SendMessage( ComboBox, CB_INSERTSTRING, 0, (LPARAM)SettingName );
		}
	}

	//	�R���{ �{�b�N�X�̏��Ԃ�ݒ肷��
	if( Setting->ModifiedFlag == FALSE )
	{
		SetComboBoxIndex( IDC_SETTING, (UCHAR)Setting->CurrentSettingIndex );
	} else {
		SetComboBoxIndex( IDC_SETTING, 0 );
	}
	//	�폜�{�^���̗L���A������ݒ肷��
	if( Setting->ModifiedFlag == FALSE && Setting->CurrentSettingIndex > 0 )
	{
		EnableControl( IDC_SETTING_DELETE, TRUE );
	} else {
		EnableControl( IDC_SETTING_DELETE, FALSE );
	}
}

//----------------------------------------------------------------------------------------------
//	ChangeSetting
//----------------------------------------------------------------------------------------------
VOID CControllerPage::ChangeSetting()
{
	//	�ϐ��錾
	LONG	MsgResult;

	//	�ύX�ς݂��ǂ����ŏ�����U�蕪����
	if( Setting->ModifiedFlag == TRUE )
	{
		//	�������Ԃ�I�������Ȃ牽�����Ȃ�
		if( GetComboBoxIndex( IDC_SETTING ) == 0 )
		{
			return;
		}

		//	�_�C�A���O��\�����A������U�蕪����
		MsgResult	= ShowMessageBox(
						 IDS_SAVE_TITLE
						,IDS_SAVE_MESSAGE
						,NULL
						,MB_YESNOCANCEL | MB_ICONQUESTION );
		switch( MsgResult )
		{
			case IDYES:
				//	�R���g���[���[�̐ݒ��ۑ�����
				MsgResult	= (LONG)DialogBoxParam(
								 Instance
								,MAKEINTRESOURCE( IDD_SAVE_DIALOG )
								,Wnd
								,(DLGPROC)SaveDialogProc
								,(LPARAM)Setting );
				if( MsgResult == IDOK )
				{
					//	�ύX���ꂽ�ݒ�𔽉f����
					ApplyChangedSetting();
				}
				if( MsgResult == IDCANCEL )
				{
					//	�L�����Z���Ȃ�R���g���[���[�̐ݒ�� 0 �ɖ߂�
					SetComboBoxIndex( IDC_SETTING, 0 );
				}
				break;

			case IDNO:
				//	�R���g���[���[�̐ݒ��ύX����
				Setting->ModifiedFlag	= FALSE;
				Setting->Change( GetComboBoxIndex( IDC_SETTING ) - 1, TRUE );
				//	�ύX���ꂽ�ݒ�𔽉f����
				ApplyChangedSetting();
				break;

			case IDCANCEL:
				//	�L�����Z���Ȃ�R���g���[���[�̐ݒ�� 0 �ɖ߂�
				SetComboBoxIndex( IDC_SETTING, 0 );
				break;
		}

	} else {
		//	�������Ԃ�I�������Ȃ牽�����Ȃ�
		if( GetComboBoxIndex( IDC_SETTING ) == Setting->CurrentSettingIndex )
		{
			return;
		}

		//	�I�����ꂽ�ݒ�ɕύX����
		Setting->Change( GetComboBoxIndex( IDC_SETTING ), TRUE );
		//	�ύX���ꂽ�ݒ�𔽉f����
		ApplyChangedSetting();
	}
}

//----------------------------------------------------------------------------------------------
//	SaveSetting
//----------------------------------------------------------------------------------------------
VOID CControllerPage::SaveSetting()
{
	//	�_�C�A���O �{�b�N�X��\�����A�ݒ��ۑ�����
	LONG	MsgResult	= (LONG)DialogBoxParam(
							 Instance
							,MAKEINTRESOURCE( IDD_SAVE_DIALOG )
							,Wnd
							,(DLGPROC)SaveDialogProc
							,(LPARAM)Setting );
	if( MsgResult == IDOK )
	{
		//	�ύX���ꂽ�ݒ�𔽉f����
		ApplyChangedSetting();
	}
}

//----------------------------------------------------------------------------------------------
//	DeleteSetting
//----------------------------------------------------------------------------------------------
VOID CControllerPage::DeleteSetting()
{
	//	���b�Z�[�W��\�����A�ݒ�̍폜���m�F����
	LONG	Result	= ShowMessageBox(
						 IDS_DELETE_TITLE
						,IDS_DELETE_MESSAGE
						,Setting->SettingName[Setting->CurrentSettingIndex]
						,MB_YESNO | MB_ICONQUESTION );
	if( Result == IDYES )
	{
		//	�I������Ă���ݒ���폜����
		Setting->Delete();
		//	�ύX���ꂽ�ݒ�𔽉f����
		ApplyChangedSetting();
	}
}

//----------------------------------------------------------------------------------------------
//	ApplyChangedSetting
//----------------------------------------------------------------------------------------------
VOID CControllerPage::ApplyChangedSetting()
{
	//	�ϐ��錾
	HWND	Dialog	= GetParent( Wnd );

	//	�ݒ�𔽉f����
	ApplySetting();

	//	�e�y�[�W�ɐݒ�𔽉f����
	for( LONG Index = 1; Index < 7; Index ++ )
	{
		HWND	Page	= PropSheet_IndexToHwnd( Dialog, Index );
		if( Page != NULL )
		{
			SendMessage(
				 Page
				,WM_SETTING_LIST_CHANGED
				,NULL
				,NULL );
		}
	}

	//	�v���p�e�B �V�[�g�ɕύX��ʒm����
	PropSheet_Changed( Dialog, Wnd );
}

//----------------------------------------------------------------------------------------------
//	ClosePage
//----------------------------------------------------------------------------------------------
VOID CControllerPage::ClosePage()
{
	//	�|�[�����O�p�^�C�}�[���~����
	KillTimer( Wnd, POLLING_TIMER_ID );

	//	�U�����~����
	TestForceFeedback( 0 );

	//	�e�C���X�^���X���J������
	delete	Device;
	delete	Report;

	//	�~���[�e�b�N�X���J������
	CloseHandle( Mutex );
}

//----------------------------------------------------------------------------------------------
//	ShowControllerState
//----------------------------------------------------------------------------------------------
VOID CControllerPage::ShowControllerState()
{
	//	�R���g���[���[�̌��݂̏�Ԃ��擾����
	FEATURE_REPORT_ID	FeatureReport;
	ZeroMemory( &FeatureReport, sizeof( FEATURE_REPORT_ID ) );
	Device->GetState( &FeatureReport );

	//	���̓��|�[�g���쐬����i�A���C�������g�̊֌W���|�C���^�n������肭�����Ȃ��j
	INPUT_REPORT	InputReport;
	InputReport.DigitalButton	= FeatureReport.DigitalButton;
	InputReport.LeftTrigger		= FeatureReport.LeftTrigger;
	InputReport.RightTrigger	= FeatureReport.RightTrigger;
	InputReport.LeftStickX		= FeatureReport.LeftStickX;
	InputReport.LeftStickY		= FeatureReport.LeftStickY;
	InputReport.RightStickX		= FeatureReport.RightStickX;
	InputReport.RightStickY		= FeatureReport.RightStickY;

	//	HID �Ǎ��p���|�[�g�𐶐�����
	HID_READ_REPORT	ReadReport;
	ZeroMemory( &ReadReport, sizeof( HID_READ_REPORT ) );
	Report->GenerateReadReport(
		 (INPUT_REPORT *)&InputReport
		,CurrentSetting
		,&ReadReport
		,&LastState );

	//	�������ꂽ�R���g���[���[�̏�Ԃ�\������
	SetIntegerText( IDC_X_AXIS, ReadReport.XAxis );
	SetIntegerText( IDC_Y_AXIS, ReadReport.YAxis );
	SetIntegerText( IDC_X_ROTATION, ReadReport.XRotation );
	SetIntegerText( IDC_Y_ROTATION, ReadReport.YRotation );
	SetIntegerText( IDC_Z_AXIS, ReadReport.ZAxis );
	SetIntegerText( IDC_Z_ROTATION, ReadReport.ZRotation );
	SetIntegerText( IDC_SLIDER1, ReadReport.Slider1 );
	SetIntegerText( IDC_SLIDER2, ReadReport.Slider2 );
	SetIntegerText(
		 IDC_HAT_SWITCH1
		,ReadReport.HatSwitch1 != -1 ? ReadReport.HatSwitch1 * 45 : -1 );
	SetIntegerText(
		 IDC_HAT_SWITCH2
		,ReadReport.HatSwitch2 != -1 ? ReadReport.HatSwitch2 * 45 : -1 );
	SetIntegerText(
		 IDC_HAT_SWITCH3
		,ReadReport.HatSwitch3 != -1 ? ReadReport.HatSwitch3 * 45 : -1 );
	SetIntegerText(
		 IDC_HAT_SWITCH4
		,ReadReport.HatSwitch4 != -1 ? ReadReport.HatSwitch4 * 45 : -1 );
	SetButtonNumberText( IDC_BUTTON, ReadReport.Buttons );
}

//----------------------------------------------------------------------------------------------
//	TestForceFeedback
//----------------------------------------------------------------------------------------------
VOID CControllerPage::TestForceFeedback( LONG TestLevel )
{
	//	���|�[�g���쐬����
	HID_WRITE_REPORT_ID	WriteReport;
	ZeroMemory( &WriteReport, sizeof( HID_WRITE_REPORT_ID ) );
	WriteReport.Id		= 0;
	WriteReport.Type	= REPORT_TYPE_DIRECT;
	Report->SetActuator(
		 TestLevel
		,CurrentSetting->ActuatorLevel[INDEX_LEFT_ACTUATOR]
		,CurrentSetting->ActuatorMinThreshold
		,CurrentSetting->ActuatorMaxThreshold
		,&WriteReport.Left );
	Report->SetActuator(
		 TestLevel
		,CurrentSetting->ActuatorLevel[INDEX_RIGHT_ACTUATOR]
		,CurrentSetting->ActuatorMinThreshold
		,CurrentSetting->ActuatorMaxThreshold
		,&WriteReport.Right );

	//	�f�o�C�X�ɐݒ肷��
	Device->SetState( &WriteReport );
}

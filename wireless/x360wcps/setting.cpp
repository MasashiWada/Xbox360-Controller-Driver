
//	Property Sheet for Microsoft Xbox 360 Wireless Controller

#include	"setting.h"
#include	"resource.h"

//----------------------------------------------------------------------------------------------
//	CSetting
//----------------------------------------------------------------------------------------------
CSetting::CSetting(
	 HINSTANCE	Instance
	,HWND		Wnd )
{
	//	�C���X�^���X
	this->Instance	= Instance;
	//	�E�B���h�E �n���h��
	this->Wnd		= Wnd;

	//	�R���g���[���[�̐ݒ�
	Setting			= NULL;
	//	�R���g���[���[�̐ݒ�̖���
	SettingName		= NULL;

	//	���W�X�g������ݒ��ǂݍ���
	LoadRegistry();
}

//----------------------------------------------------------------------------------------------
//	~CSetting
//----------------------------------------------------------------------------------------------
CSetting::~CSetting()
{
	//	�R���g���[���[�̐ݒ�A�ݒ�̖��̗̂̈���J������
	if( Setting != NULL )
	{
		LocalFree( Setting );
	}
	if( SettingName != NULL )
	{
		for( LONG Index = 0; Index < SettingCount; Index ++ )
		{
			LocalFree( SettingName[Index] );
		}
		LocalFree( SettingName );
	}
}

//----------------------------------------------------------------------------------------------
//	LoadRegistry
//----------------------------------------------------------------------------------------------
VOID CSetting::LoadRegistry()
{
	//	�R���g���[���[�̐ݒ�A�ݒ�̖��̗̂̈���J������
	if( Setting != NULL )
	{
		LocalFree( Setting );
	}
	if( SettingName != NULL )
	{
		for( LONG Index = 0; Index < SettingCount; Index ++ )
		{
			LocalFree( SettingName[Index] );
		}
		LocalFree( SettingName );
	}

	//	���݂̃R���g���[���[�̐ݒ���擾����
	GetDefaultSetting( &CurrentSetting );
	GetRegistry( &CurrentSetting, REG_BINARY, sizeof( SETTING ), CURRENT_SETTING );

	//	���݂̃R���g���[���[�̐ݒ�̏��Ԃ��擾����
	CurrentSettingIndex	= 0;
	GetRegistry( &CurrentSettingIndex, REG_DWORD, sizeof( LONG ), CURRENT_SETTING_INDEX );

	//	�R���g���[���[�̐ݒ萔���擾����
	SettingCount	= 1;
	GetRegistry( &SettingCount, REG_DWORD, sizeof( LONG ), SETTING_COUNT );

	//	�R���g���[���[�̐ݒ�A�ݒ�̖��̂�ۑ����邽�߂̗̈���m�ۂ���
	Setting		= (SETTING *)LocalAlloc( LPTR, SettingCount * sizeof( SETTING ) );
	SettingName	= (WCHAR * *)LocalAlloc( LPTR, SettingCount * sizeof( WCHAR * ) );
	for( LONG Index = 0; Index < SettingCount; Index ++ )
	{
		SettingName[Index]	= (WCHAR *)LocalAlloc( LPTR, sizeof( WCHAR ) * MAX_PATH );
	}

	//	�W���̐ݒ���擾����
	GetDefaultSetting( &Setting[0] );
	LoadString( Instance, IDS_DEFAULT_SETTING, SettingName[0], sizeof( WCHAR ) * MAX_PATH );

	//	�R���g���[���[�̐ݒ�A�ݒ�̖��̂��擾����
	for( LONG Index = 1; Index < SettingCount; Index ++ )
	{
		GetSetting( Index, &Setting[Index] );
		GetSettingName( Index, SettingName[Index], sizeof( WCHAR ) * MAX_PATH );
	}

	//	�ύX�ς݃t���O���擾����
	ModifiedFlag	= FALSE;
	GetRegistry( &ModifiedFlag, REG_BINARY, sizeof( BOOLEAN ), MODIFIED_FLAG );

	//	�ݒ�̎����؊������擾����
	AutoSettingChange	= FALSE;
	GetRegistry( &AutoSettingChange, REG_BINARY, sizeof( BOOLEAN ), AUTO_SETTING_CHANGE );
}

//----------------------------------------------------------------------------------------------
//	SaveRegistry
//----------------------------------------------------------------------------------------------
VOID CSetting::SaveRegistry()
{
	//	���݂̃R���g���[���[�̐ݒ���X�V����
	SetRegistry( &CurrentSetting, REG_BINARY, sizeof( SETTING ), CURRENT_SETTING );

	//	���݂̃R���g���[���[�̐ݒ�̏��Ԃ��X�V����
	SetRegistry( &CurrentSettingIndex, REG_DWORD, sizeof( LONG ), CURRENT_SETTING_INDEX );

	//	�R���g���[���[�̐ݒ萔���擾���Ă���
	LONG	OldSettingCount	= 0;
	GetRegistry( &OldSettingCount, REG_DWORD, sizeof( LONG ), SETTING_COUNT );

	//	�R���g���[���[�̐ݒ萔���X�V����
	SetRegistry( &SettingCount, REG_DWORD, sizeof( LONG ), SETTING_COUNT );

	//	�R���g���[���[�̐ݒ�A�ݒ�̖��̂��X�V����
	for( LONG Index = 1; Index < SettingCount; Index ++ )
	{
		SetSetting( Index, &Setting[Index] );
		SetSettingName( Index, SettingName[Index] );
	}

	//	�]���ȃR���g���[���[�̐ݒ�A�ݒ�̖��̂��폜����
	if( OldSettingCount > SettingCount )
	{
		for( LONG Index = SettingCount; Index < OldSettingCount; Index ++ )
		{
			DelSetting( Index );
			DelSettingName( Index );
		}
	}

	//	�ύX�ς݃t���O���X�V����
	SetRegistry( &ModifiedFlag, REG_BINARY, sizeof( BOOLEAN ), MODIFIED_FLAG );

	//	�ݒ�̎����؊������X�V����
	SetRegistry( &AutoSettingChange, REG_BINARY, sizeof( BOOLEAN ), AUTO_SETTING_CHANGE );
}

//----------------------------------------------------------------------------------------------
//	SaveAs
//----------------------------------------------------------------------------------------------
VOID CSetting::SaveAs( WCHAR * SaveName )
{
	//	���݂̃R���g���[���[�̐ݒ�̏��Ԃ��X�V����
	CurrentSettingIndex	= SettingCount;

	//	�R���g���[���[�̐ݒ萔���X�V����
	SettingCount ++;

	//	�R���g���[���[�̐ݒ��ǉ�����
	Setting	= (SETTING *)LocalReAlloc(
				 Setting
				,SettingCount * sizeof( SETTING )
				,LMEM_MOVEABLE );
	CopyMemory( &Setting[CurrentSettingIndex], &CurrentSetting, sizeof( SETTING ) );
	//	�R���g���[���[�̐ݒ�̖��̂�ǉ�����
	SettingName	= (WCHAR * *)LocalReAlloc(
					 SettingName
					,SettingCount * sizeof( WCHAR * )
					,LMEM_MOVEABLE );
	SettingName[CurrentSettingIndex]	= (WCHAR *)LocalAlloc(
											  LPTR
											 ,sizeof( WCHAR ) * MAX_PATH );
	wcscpy_s( SettingName[CurrentSettingIndex], MAX_PATH, SaveName );

	//	�ύX�ς݃t���O���X�V����
	ModifiedFlag	= FALSE;
}

//----------------------------------------------------------------------------------------------
//	Save
//----------------------------------------------------------------------------------------------
VOID CSetting::Save( LONG Index )
{
	//	���݂̃R���g���[���[�̐ݒ�̏��Ԃ��X�V����
	CurrentSettingIndex	= Index;

	//	�R���g���[���[�̐ݒ���X�V����
	CopyMemory( &Setting[CurrentSettingIndex], &CurrentSetting, sizeof( SETTING ) );

	//	�ύX�ς݃t���O���X�V����
	ModifiedFlag	= FALSE;
}

//----------------------------------------------------------------------------------------------
//	Delete
//----------------------------------------------------------------------------------------------
VOID CSetting::Delete()
{
	//	�R���g���[���[�̐ݒ萔���X�V����
	SettingCount --;
	if( CurrentSettingIndex < SettingCount )
	{
		//	�R���g���[���[�̐ݒ�A�ݒ�̖��̂��P���X���C�h����
		MoveMemory(
			 &Setting[CurrentSettingIndex]
			,&Setting[CurrentSettingIndex + 1]
			,sizeof( SETTING ) * ( SettingCount - CurrentSettingIndex ) );
		LocalFree( SettingName[CurrentSettingIndex] );
		MoveMemory(
			 &SettingName[CurrentSettingIndex]
			,&SettingName[CurrentSettingIndex + 1]
			,sizeof( WCHAR * ) * ( SettingCount - CurrentSettingIndex ) );
	} else {
		//	���݂̃R���g���[���[�̐ݒ�̏��Ԃ��X�V����
		CurrentSettingIndex --;
	}
	//	���݂̃R���g���[���[�̐ݒ���X�V����
	CopyMemory( &CurrentSetting, &Setting[CurrentSettingIndex], sizeof( SETTING ) );
	//	�s�v�ȃ��������J������
	Setting	= (SETTING *)LocalReAlloc(
				 Setting
				,SettingCount * sizeof( SETTING )
				,LMEM_MOVEABLE );
	SettingName	= (WCHAR * *)LocalReAlloc(
					 SettingName
					,SettingCount * sizeof( WCHAR * )
					,LMEM_MOVEABLE );

	//	�ύX�ς݃t���O���X�V����
	ModifiedFlag	= FALSE;
}

//----------------------------------------------------------------------------------------------
//	Change
//----------------------------------------------------------------------------------------------
VOID CSetting::Change(
	 LONG	Index
	,BOOL	Auto )
{
	//	�ύX�ς݂Ȃ�m�F����
	LONG	MsgResult	= IDYES;
	if( ModifiedFlag == TRUE )
	{
		if( Auto == TRUE )
		{
			MsgResult	= IDNO;
		} else {
			MsgResult	= ShowMessageBox(
							 Wnd
							,IDS_WARNING_TITLE
							,IDS_WARNING_MESSAGE
							,MB_YESNO | MB_ICONQUESTION );
		}
	}

	//	�R���g���[���[�̐ݒ��ύX����
	if( MsgResult == IDYES )
	{
		//	�݊����̐ݒ肪�ς���Ă���΃_�C�A���O��\������
		if( Auto == FALSE )
		{
			if(	CurrentSetting.ControllerCount	!= Setting[Index].ControllerCount
			||	CurrentSetting.LimitationMode	!= Setting[Index].LimitationMode )
			{
				ShowMessageBox(
					 Wnd
					,IDS_RESTART_TITLE
					,IDS_RESTART_MESSAGE
					,MB_OK | MB_ICONINFORMATION );
			}
		}

		//	���݂̃R���g���[���[�̐ݒ���X�V����
		CopyMemory( &CurrentSetting, &Setting[Index], sizeof( SETTING ) );
		//	���݂̃R���g���[���[�̐ݒ�̏��Ԃ��X�V����
		CurrentSettingIndex	= Index;

		//	�ύX�ς݃t���O���X�V����
		ModifiedFlag	= FALSE;
	}
}

//----------------------------------------------------------------------------------------------
//	ShowMessageBox
//----------------------------------------------------------------------------------------------
LONG CSetting::ShowMessageBox(
	 HWND	Wnd
	,ULONG	TitleID
	,ULONG	MessageID
	,ULONG	Type )
{
	//	���b�Z�[�W��ǂݍ���
	WCHAR	Title[MAX_PATH];
	LoadString( Instance, TitleID, Title, MAX_PATH );
	WCHAR	Message[MAX_PATH];
	LoadString( Instance, MessageID, Message, MAX_PATH );

	//	���b�Z�[�W �{�b�N�X��\������
	return( MessageBox( Wnd, Message, Title, Type ) );
}

//----------------------------------------------------------------------------------------------
//	GetSetting
//----------------------------------------------------------------------------------------------
VOID CSetting::GetSetting(
	 LONG		Index
	,SETTING *	Setting )
{
	//	�R���g���[���[�̐ݒ���擾����
	WCHAR	KeyName[MAX_PATH];
	swprintf_s( KeyName, SETTING_DATA, Index );
	GetRegistry( Setting, REG_BINARY, sizeof( SETTING ), KeyName );
}

//----------------------------------------------------------------------------------------------
//	SetSetting
//----------------------------------------------------------------------------------------------
VOID CSetting::SetSetting(
	 LONG		Index
	,SETTING *	Setting )
{
	//	�R���g���[���[�̐ݒ��ݒ肷��
	WCHAR	KeyName[MAX_PATH];
	swprintf_s( KeyName, SETTING_DATA, Index );
	SetRegistry( Setting, REG_BINARY, sizeof( SETTING ), KeyName );
}

//----------------------------------------------------------------------------------------------
//	DelSetting
//----------------------------------------------------------------------------------------------
VOID CSetting::DelSetting( LONG Index )
{
	//	�R���g���[���[�̐ݒ���폜����
	WCHAR	KeyName[MAX_PATH];
	swprintf_s( KeyName, SETTING_DATA, Index );
	DelRegistry( KeyName );
}

//----------------------------------------------------------------------------------------------
//	GetSettingName
//----------------------------------------------------------------------------------------------
VOID CSetting::GetSettingName(
	 LONG		Index
	,WCHAR *	SettingName
	,LONG		Size )
{
	//	�R���g���[���[�̐ݒ���擾����
	WCHAR	KeyName[MAX_PATH];
	swprintf_s( KeyName, SETTING_NAME, Index );
	GetRegistry( SettingName, REG_SZ, Size, KeyName );
}

//----------------------------------------------------------------------------------------------
//	SetSettingName
//----------------------------------------------------------------------------------------------
VOID CSetting::SetSettingName(
	 LONG		Index
	,WCHAR *	SettingName )
{
	//	�R���g���[���[�̐ݒ��ݒ肷��
	WCHAR	KeyName[MAX_PATH];
	swprintf_s( KeyName, SETTING_NAME, Index );
	SetRegistry( SettingName, REG_SZ, sizeof( WCHAR ) * wcslen( SettingName ) + 1, KeyName );
}

//----------------------------------------------------------------------------------------------
//	DelSettingName
//----------------------------------------------------------------------------------------------
VOID CSetting::DelSettingName( LONG Index )
{
	//	�R���g���[���[�̐ݒ���폜����
	WCHAR	KeyName[MAX_PATH];
	swprintf_s( KeyName, SETTING_NAME, Index );
	DelRegistry( KeyName );
}

//----------------------------------------------------------------------------------------------
//	GetRegistry
//----------------------------------------------------------------------------------------------
VOID CSetting::GetRegistry(
	 VOID *		Value
	,ULONG		ValueType
	,ULONG		ValueSize
	,WCHAR *	ValueName )
{
	//	�ϐ��錾
	HKEY	Key;
	DWORD	Disposition;
	LONG	Result;

	//	���W�X�g�����J��
	Result	= RegCreateKeyEx(
				 HKEY_LOCAL_MACHINE
				,REGISTRY_KEY
				,0
				,NULL
				,REG_OPTION_NON_VOLATILE
				,KEY_ALL_ACCESS
				,NULL
				,&Key
				,&Disposition  );
	if( Result == ERROR_SUCCESS )
	{
		//	���W�X�g������ǂݍ���
		RegQueryValueEx( Key, ValueName, NULL, &ValueType, (LPBYTE)Value, &ValueSize );
		//	���W�X�g�������
		RegCloseKey( Key );
	}
}

//----------------------------------------------------------------------------------------------
//	SetRegistry
//----------------------------------------------------------------------------------------------
VOID CSetting::SetRegistry(
	 VOID *		Value
	,ULONG		ValueType
	,ULONG		ValueSize
	,WCHAR *	ValueName )
{
	//	�ϐ��錾
	HKEY	Key;
	DWORD	Disposition;
	LONG	Result;

	//	���W�X�g�����J��
	Result	= RegCreateKeyEx(
				 HKEY_LOCAL_MACHINE
				,REGISTRY_KEY
				,0
				,NULL
				,REG_OPTION_NON_VOLATILE
				,KEY_ALL_ACCESS
				,NULL
				,&Key
				,&Disposition  );
	if( Result == ERROR_SUCCESS )
	{
		//	���W�X�g���ɏ�������
		RegSetValueEx( Key, ValueName, 0, ValueType, (LPBYTE)Value, ValueSize );
		//	���W�X�g�������
		RegCloseKey( Key );
	}
}

//----------------------------------------------------------------------------------------------
//	DelRegistry
//----------------------------------------------------------------------------------------------
VOID CSetting::DelRegistry( WCHAR * ValueName )
{
	//	�ϐ��錾
	HKEY	Key;
	DWORD	Disposition;
	LONG	Result;

	//	���W�X�g�����J��
	Result	= RegCreateKeyEx(
				 HKEY_LOCAL_MACHINE
				,REGISTRY_KEY
				,0
				,NULL
				,REG_OPTION_NON_VOLATILE
				,KEY_ALL_ACCESS
				,NULL
				,&Key
				,&Disposition  );
	if( Result == ERROR_SUCCESS )
	{
		//	���W�X�g������폜����
		RegDeleteValue( Key, ValueName );
		//	���W�X�g�������
		RegCloseKey( Key );
	}
}

//----------------------------------------------------------------------------------------------
//	GetDefaultSetting
//----------------------------------------------------------------------------------------------
VOID CSetting::GetDefaultSetting( SETTING * Setting )
{
	//	�ϐ��錾
	UCHAR	Index;
	UCHAR	WayIndex;

	//	�W���̐ݒ���쐬����
	ZeroMemory( Setting, sizeof( SETTING ) );

	//	�X�e�B�b�N
	Setting->StickUsage[INDEX_LEFT_STICK]	= USAGE_X_AXIS_Y_AXIS;
	Setting->StickUsage[INDEX_RIGHT_STICK]	= USAGE_X_ROTATION_Y_ROTATION;
	for( Index = INDEX_LEFT_STICK; Index <= INDEX_RIGHT_STICK; Index ++ )
	{
		Setting->StickReverseX[Index]			= FALSE;
		Setting->StickReverseY[Index]			= FALSE;
		Setting->StickAngle[Index]				= 0;
		Setting->StickDeadZoneType[Index]		= DEAD_ZONE_TYPE_SQUARE;
		Setting->StickTransformType[Index]		= TRANSFORM_TYPE_NONE;
		Setting->StickMinThreshold[Index]		= 15;
		Setting->StickMaxThreshold[Index]		= 95;
		Setting->StickHatSwitchThreshold[Index]	= 50;
		for( WayIndex = INDEX_WAY_UP; WayIndex <= INDEX_WAY_RIGHT; WayIndex ++ )
		{
			Setting->StickWayUsage[Index][WayIndex]				= USAGE_BUTTON_13 + WayIndex;
			Setting->StickWayRapidFire[Index][WayIndex]			= FALSE;
			Setting->StickWayRapidFireSpeed[Index][WayIndex]	= 80;
		}
	}

	//	�����p�b�h
	Setting->DirectionalPadUsage			= USAGE_HAT_SWITCH1;
	Setting->DirectionalPadReverseX			= FALSE;
	Setting->DirectionalPadReverseY			= FALSE;
	Setting->DirectionalPadAngle			= 0;
	Setting->DirectionalPadTransformType	= TRANSFORM_TYPE_NONE;
	for( WayIndex = INDEX_WAY_UP; WayIndex <= INDEX_WAY_RIGHT; WayIndex ++ )
	{
		Setting->DirectionalPadWayUsage[WayIndex]			= USAGE_BUTTON_13 + WayIndex;
		Setting->DirectionalPadWayRapidFire[WayIndex]		= FALSE;
		Setting->DirectionalPadWayRapidFireSpeed[WayIndex]	= 80;
	}

	//	�g���K�[
	for( Index = INDEX_LEFT_TRIGGER; Index <= INDEX_RIGHT_TRIGGER; Index ++ )
	{
		Setting->TriggerUsage[Index]			= USAGE_BUTTON_11 + Index;
		Setting->TriggerRapidFire[Index]		= FALSE;
		Setting->TriggerRapidFireSpeed[Index]	= 80;
		Setting->TriggerMinThreshold[Index]		= 15;
		Setting->TriggerMaxThreshold[Index]		= 95;
		Setting->TriggerButtonThreshold[Index]	= 85;
	}

	//	�{�^��
	for( Index = INDEX_A_BUTTON; Index <= INDEX_RIGHT_STICK_BUTTON; Index ++ )
	{
		Setting->ButtonUsage[Index]				= USAGE_BUTTON_1 + Index;
		Setting->ButtonRapidFire[Index]			= FALSE;
		Setting->ButtonRapidFireSpeed[Index]	= 80;
	}

	//	Xbox �K�C�h �{�^��
	Setting->ButtonUsage[INDEX_XBOX_GUIDE_BUTTON]			= USAGE_BUTTON_13;
	Setting->ButtonRapidFire[INDEX_XBOX_GUIDE_BUTTON]		= FALSE;
	Setting->ButtonRapidFireSpeed[INDEX_XBOX_GUIDE_BUTTON]	= 80;

	//	�����O ���C�g
	Setting->RingLightPattern	= 16;

	//	�U��
	Setting->Actuator								= TRUE;
	Setting->ActuatorLevel[INDEX_LEFT_ACTUATOR]		= 80;
	Setting->ActuatorLevel[INDEX_RIGHT_ACTUATOR]	= 80;
	Setting->ActuatorMinThreshold					= 20;
	Setting->ActuatorMaxThreshold					= 100;

	//	�ڍאݒ�
	for( Index = Z_AXIS; Index <= SLIDER2; Index ++ )
	{
		Setting->SlideSliderValue[Index]			= FALSE;
		Setting->SlideSliderCenterValue[Index]		= FALSE;
		Setting->ReverseSliderValue[Index]			= FALSE;
	}
	Setting->ControllerCount	= 1;
	Setting->LimitationMode		= LIMITATION_NONE;
}

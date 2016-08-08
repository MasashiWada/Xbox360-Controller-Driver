
//	Property Sheet for Microsoft Xbox 360 Controller

#include	"device.h"

//----------------------------------------------------------------------------------------------
//	CDevice
//----------------------------------------------------------------------------------------------
CDevice::CDevice()
{
	//	�T�|�[�g���Ă���f�o�C�X�̐����擾����
	SupportedDeviceCount	= 0;
	GetRegistry(
		 &SupportedDeviceCount
		,REG_DWORD
		,sizeof( LONG )
		,SUPPORTED_DEVICE_COUNT );
	SupportedDeviceCount	= max( 0, min( SupportedDeviceCount, 256 ) );

	//	�T�|�[�g���Ă���f�o�C�X�̑������擾����
	if( SupportedDeviceCount > 0 )
	{
		SupportedDeviceAttribute	= (ULONG *)LocalAlloc(
										  LPTR
										 ,SupportedDeviceCount * sizeof( ULONG ) );
		for( LONG Index = 0; Index < SupportedDeviceCount; Index ++ )
		{
			//	���W�X�g������f�o�C�X�̑������擾����
			WCHAR	Attribute[32];
			swprintf_s( Attribute, SUPPORTED_DEVICE_ATTRIBUTE, Index );
			GetRegistry(
				 &SupportedDeviceAttribute[Index]
				,REG_DWORD
				,sizeof( ULONG )
				,Attribute );
		}
	} else {
		SupportedDeviceAttribute	= NULL;
	}
}

//----------------------------------------------------------------------------------------------
//	~CDevice
//----------------------------------------------------------------------------------------------
CDevice::~CDevice()
{
	//	�T�|�[�g���Ă���f�o�C�X�̑������J������
	if( SupportedDeviceAttribute != NULL )
	{
		LocalFree( SupportedDeviceAttribute );
	}
}

//----------------------------------------------------------------------------------------------
//	GetRegistry
//----------------------------------------------------------------------------------------------
VOID CDevice::GetRegistry(
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
//	IsConnected
//----------------------------------------------------------------------------------------------
BOOL CDevice::IsConnected()
{
	//	�f�o�C�X���ڑ�����Ă��邩�ǂ�����Ԃ�
	return( IoDevice( IO_MODE_IS_CONNECTED, NULL, NULL ) );
}

//----------------------------------------------------------------------------------------------
//	GetState
//----------------------------------------------------------------------------------------------
BOOL CDevice::GetState( FEATURE_REPORT_ID * FeatureReport )
{
	//	�f�o�C�X�̏�Ԃ��擾����
	return( IoDevice( IO_MODE_GET_STATE, NULL, FeatureReport ) );
}

//----------------------------------------------------------------------------------------------
//	SetState
//----------------------------------------------------------------------------------------------
BOOL CDevice::SetState( HID_WRITE_REPORT_ID * WriteReport )
{
	//	�f�o�C�X�̏�Ԃ�ݒ肷��
	return( IoDevice( IO_MODE_SET_STATE, WriteReport, NULL ) );
}

//----------------------------------------------------------------------------------------------
//	Apply
//----------------------------------------------------------------------------------------------
BOOL CDevice::Apply()
{
	//	�f�o�C�X�ɕύX��ʒm����
	return( IoDevice( IO_MODE_APPLY, NULL, NULL ) );
}

//----------------------------------------------------------------------------------------------
//	IoDevice
//----------------------------------------------------------------------------------------------
BOOL CDevice::IoDevice(
	 LONG					IoMode
	,HID_WRITE_REPORT_ID *	WriteReport
	,FEATURE_REPORT_ID *	FeatureReport )
{
	//	�ϐ��錾
	GUID				GUID_HID;
	LONG				InterfaceIndex	= 0;
	BOOL				DeviceFound		= FALSE;
	FEATURE_REPORT_ID	Report;

	//	�f�o�C�X �N���X�̃n���h�����擾����
	HidD_GetHidGuid( &GUID_HID );
	HDEVINFO	DevInfo	= SetupDiGetClassDevs(
							 &GUID_HID
							,NULL
							,NULL
							,DIGCF_PRESENT | DIGCF_INTERFACEDEVICE );
	if( DevInfo == INVALID_HANDLE_VALUE )
	{
		return( FALSE );
	}

	//	�Ώۂ̃C���^�[�t�F�C�X��T��
	for(;;)
	{
		//	�C���^�[�t�F�C�X���擾����
		SP_INTERFACE_DEVICE_DATA	Interface;
		Interface.cbSize	= sizeof( SP_INTERFACE_DEVICE_DATA );
		BOOL	Result	= SetupDiEnumDeviceInterfaces(
							 DevInfo
							,NULL
							,&GUID_HID
							,InterfaceIndex
							,&Interface );
		if( Result == FALSE )
		{
			SetupDiDestroyDeviceInfoList( DevInfo );
			return( DeviceFound );
		}

		//	�C���^�[�t�F�C�X�̏ڍ׏��̃T�C�Y���擾����
		DWORD	DetailSize;
		SetupDiGetDeviceInterfaceDetail( DevInfo, &Interface, NULL, 0, &DetailSize, NULL );

		//	�C���^�[�t�F�C�X�̏ڍ׏����擾����
		PSP_INTERFACE_DEVICE_DETAIL_DATA	Detail;
		Detail	= (PSP_INTERFACE_DEVICE_DETAIL_DATA)LocalAlloc( LPTR, DetailSize );
		if( Detail == NULL )
		{
			SetupDiDestroyDeviceInfoList( DevInfo );
			return( DeviceFound );
		}
		Detail->cbSize	= sizeof( SP_INTERFACE_DEVICE_DETAIL_DATA );
		Result	= SetupDiGetDeviceInterfaceDetail(
					 DevInfo
					,&Interface
					,Detail
					,DetailSize
					,NULL
					,NULL );
		if( Result == FALSE )
		{
			SetupDiDestroyDeviceInfoList( DevInfo );
			LocalFree( Detail );
			return( DeviceFound );
		}

		//	�f�o�C�X �C���^�[�t�F�C�X���J��
		HANDLE	Device	= CreateFile(
							 Detail->DevicePath
							,GENERIC_READ | GENERIC_WRITE
							,FILE_SHARE_READ | FILE_SHARE_WRITE
							,NULL
							,OPEN_EXISTING
							,FILE_ATTRIBUTE_NORMAL
							,NULL );
		if( Device == INVALID_HANDLE_VALUE )
		{
			LocalFree( Detail );
			InterfaceIndex	++;
			continue;
		}

		//	�C���^�[�t�F�C�X�̏ڍׂɃx���_ ID�A�v���_�N�g ID ���܂܂�邩�`�F�b�N����
		HIDD_ATTRIBUTES	HidAttribute;
		HidD_GetAttributes( Device, &HidAttribute );
		BOOL	TargetDeviceFound	= FALSE;
		for( LONG DeviceIndex = 0; DeviceIndex < SupportedDeviceCount; DeviceIndex ++ )
		{
			if(	HidAttribute.VendorID	== HIWORD( SupportedDeviceAttribute[DeviceIndex] )
			&&	HidAttribute.ProductID	== LOWORD( SupportedDeviceAttribute[DeviceIndex] ) )
			{
				TargetDeviceFound	= TRUE;
				break;
			}
		}

		//	�f�o�C�X�𔭌������ꍇ
		if( TargetDeviceFound == TRUE )
		{
			switch( IoMode )
			{
				case IO_MODE_IS_CONNECTED:
					;
					break;

				case IO_MODE_GET_STATE:
					//	�f�o�C�X�̏�Ԃ��擾����
					ZeroMemory( &Report, sizeof( FEATURE_REPORT_ID ) );
					Result	= HidD_GetFeature( Device, &Report, sizeof( FEATURE_REPORT_ID ) );
					if( Result == TRUE )
					{
						//	�����R���g���[���ɑΏ����邽�߁A�f�[�^�����Z����
						//	�f�W�^�� �{�^��
						FeatureReport->DigitalButton	|= Report.DigitalButton;
						//	���g���K�[
						FeatureReport->LeftTrigger		= ADD_UCHAR(
															 FeatureReport->LeftTrigger
															,Report.LeftTrigger );
						//	�E�g���K�[
						FeatureReport->RightTrigger		= ADD_UCHAR(
															 FeatureReport->RightTrigger
															,Report.RightTrigger );
						//	���X�e�B�b�N X
						FeatureReport->LeftStickX		= ADD_SHORT(
															 FeatureReport->LeftStickX
															,Report.LeftStickX );
						//	���X�e�B�b�N Y
						FeatureReport->LeftStickY		= ADD_SHORT(
															 FeatureReport->LeftStickY
															,Report.LeftStickY );
						//	���X�e�B�b�N X
						FeatureReport->RightStickX		= ADD_SHORT(
															 FeatureReport->RightStickX
															,Report.RightStickX );
						//	���X�e�B�b�N Y
						FeatureReport->RightStickY		= ADD_SHORT(
															 FeatureReport->RightStickY
															,Report.RightStickY );
					}
					break;

				case IO_MODE_SET_STATE:
					//	�f�o�C�X�̏�Ԃ�ݒ肷��
					DWORD	ReqLen;
					WriteFile(
						 Device
						,WriteReport
						,sizeof( HID_WRITE_REPORT_ID )
						,&ReqLen
						,NULL );
					break;

				case IO_MODE_APPLY:
					//	�f�o�C�X�ɕύX��ʒm����
					ZeroMemory( &Report, sizeof( FEATURE_REPORT_ID ) );
					HidD_SetFeature( Device, &Report, sizeof( FEATURE_REPORT_ID ) );
					break;
			}
			DeviceFound	= TRUE;
		}

		//	�f�o�C�X �C���^�[�t�F�C�X�����
		CloseHandle( Device );

		//	���̃C���^�[�t�F�C�X��
		LocalFree( Detail );
		InterfaceIndex	++;
	}

	//	�f�o�C�X �N���X�̃n���h�����J������
	SetupDiDestroyDeviceInfoList( DevInfo );

	return( DeviceFound );
}

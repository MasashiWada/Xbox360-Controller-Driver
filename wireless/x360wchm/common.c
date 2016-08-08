
//	HID Minidriver for Microsoft Xbox 360 Wireless Controller

#include	"driver.h"

//----------------------------------------------------------------------------------------------
//	�֐�
//----------------------------------------------------------------------------------------------

IO_COMPLETION_ROUTINE IoCompletionRoutine;

//----------------------------------------------------------------------------------------------
//	CompleteRequest
//----------------------------------------------------------------------------------------------
VOID CompleteRequest(
	 IN PIRP		Irp
	,IN NTSTATUS	Status
	,IN ULONG		Information )
{
	//	I/O �v��������������
	Irp->IoStatus.Status		= Status;
	Irp->IoStatus.Information	= Information;
	IoCompleteRequest( Irp, IO_NO_INCREMENT );
}

//----------------------------------------------------------------------------------------------
//	SendRequest
//----------------------------------------------------------------------------------------------
NTSTATUS SendRequest(
	 IN PDEVICE_OBJECT	DeviceObject
	,IN PIRP			Irp )
{
	//	���ʃh���C�o�� I/O �v���𔭍s����
	IoSkipCurrentIrpStackLocation( Irp );
	return( IoCallDriver( GET_NEXT_DEVICE_OBJECT( DeviceObject ), Irp ) );
}

//----------------------------------------------------------------------------------------------
//	SendAndWaitRequest
//----------------------------------------------------------------------------------------------
NTSTATUS SendAndWaitRequest(
	 IN PDEVICE_OBJECT	DeviceObject
	,IN PIRP			Irp )
{
	//	�ϐ��錾
	NTSTATUS	Status;
	KEVENT		WaitEvent;

	//	���ʃh���C�o�� I/O �v���𔭍s����
	KeInitializeEvent( &WaitEvent, NotificationEvent, FALSE );
	IoCopyCurrentIrpStackLocationToNext( Irp );
	IoSetCompletionRoutine(
		 Irp
		,(PIO_COMPLETION_ROUTINE)IoCompletionRoutine
		,(PVOID)&WaitEvent
		,TRUE
		,TRUE
		,TRUE );
	Status	= IoCallDriver( GET_NEXT_DEVICE_OBJECT( DeviceObject ), Irp );

	//	���ʃh���C�o�̏�������������܂őҋ@����
	if( Status == STATUS_PENDING )
	{
		KeWaitForSingleObject( &WaitEvent, Executive, KernelMode, FALSE, NULL );
		Status	= Irp->IoStatus.Status;
	}

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	SendPowerRequest
//----------------------------------------------------------------------------------------------
NTSTATUS SendPowerRequest(
	 IN PDEVICE_OBJECT	DeviceObject
	,IN PIRP			Irp )
{
	//	���ʃh���C�o�� I/O �v���𔭍s����
	PoStartNextPowerIrp( Irp );
	IoSkipCurrentIrpStackLocation( Irp );
#if (NTDDI_VERSION < NTDDI_LONGHORN)
	return( PoCallDriver( GET_NEXT_DEVICE_OBJECT( DeviceObject ), Irp ) );
#else
	return( IoCallDriver( GET_NEXT_DEVICE_OBJECT( DeviceObject ), Irp ) );
#endif
}

//----------------------------------------------------------------------------------------------
//	SendAndWaitPowerRequest
//----------------------------------------------------------------------------------------------
NTSTATUS SendAndWaitPowerRequest(
	 IN PDEVICE_OBJECT	DeviceObject
	,IN PIRP			Irp )
{
	//	�ϐ��錾
	NTSTATUS	Status;
	KEVENT		WaitEvent;

	//	���ʃh���C�o�� I/O �v���𔭍s����
	KeInitializeEvent( &WaitEvent, NotificationEvent, FALSE );
	PoStartNextPowerIrp( Irp );
	IoCopyCurrentIrpStackLocationToNext( Irp );
	IoSetCompletionRoutine(
		 Irp
		,(PIO_COMPLETION_ROUTINE)IoCompletionRoutine
		,(PVOID)&WaitEvent
		,TRUE
		,TRUE
		,TRUE );
#if (NTDDI_VERSION < NTDDI_LONGHORN)
	Status	= PoCallDriver( GET_NEXT_DEVICE_OBJECT( DeviceObject ), Irp );
#else
	Status	= IoCallDriver( GET_NEXT_DEVICE_OBJECT( DeviceObject ), Irp );
#endif

	//	���ʃh���C�o�̏�������������܂őҋ@����
	if( Status == STATUS_PENDING )
	{
		KeWaitForSingleObject( &WaitEvent, Executive, KernelMode, FALSE, NULL );
		Status	= Irp->IoStatus.Status;
	}

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	IoCompletionRoutine
//----------------------------------------------------------------------------------------------
NTSTATUS IoCompletionRoutine(
	 IN PDEVICE_OBJECT	DeviceObject
	,IN PIRP			Irp
	,IN PKEVENT			Event )
{
	//	I/O �v���̊�����ʒm����
	KeSetEvent( Event, 0, FALSE );

	return( STATUS_MORE_PROCESSING_REQUIRED );
}

//----------------------------------------------------------------------------------------------
//	IncrementReference
//----------------------------------------------------------------------------------------------
NTSTATUS IncrementReference( IN PDEVICE_EXTENSION DeviceExtension )
{
	//	�ϐ��錾
	NTSTATUS	Status	= STATUS_SUCCESS;

	//	�Q�ƃJ�E���^�����Z����
	InterlockedIncrement( &DeviceExtension->ReferenceCount );

	//	�h���C�o�̏�Ԃ͗L���E�����H
	if( DeviceExtension->DriverState == Disable )
	{
		if( InterlockedDecrement( &DeviceExtension->ReferenceCount ) == 0 )
		{
			//	�Q�ƏI���C�x���g�𑗂�
			KeSetEvent( &DeviceExtension->ReferenceEnd, 0, FALSE );
			Status	= STATUS_DELETE_PENDING;
		}
	}

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	DecrementReference
//----------------------------------------------------------------------------------------------
VOID DecrementReference( IN PDEVICE_EXTENSION DeviceExtension )
{
	//	�Q�ƃJ�E���^�����Z����
	if( InterlockedDecrement( &DeviceExtension->ReferenceCount ) == 0 )
	{
		//	�Q�ƏI���C�x���g�𑗂�
		KeSetEvent( &DeviceExtension->ReferenceEnd, 0, FALSE );
	}
}

//----------------------------------------------------------------------------------------------
//	GetRegistryBinary
//----------------------------------------------------------------------------------------------
VOID GetRegistryBinary(
	 IN PWSTR	Path
	,IN PWSTR	Value
	,IN CHAR *	Buf
	,IN LONG	BufSize )
{
	//	�ϐ��錾
	NTSTATUS						Status;
	UNICODE_STRING					RegistryPath;
	UNICODE_STRING					RegistryValue;
	OBJECT_ATTRIBUTES				ObjAttribute;
	HANDLE							Key;
	ULONG							ValueSize	= 0;
	PKEY_VALUE_PARTIAL_INFORMATION	ValueInfo;

	//	���W�X�g�� �p�X���쐬����
	RtlInitUnicodeString( &RegistryPath, Path );
	//	�I�u�W�F�N�g�������쐬����
	InitializeObjectAttributes(
		 &ObjAttribute
		,&RegistryPath
		,OBJ_CASE_INSENSITIVE
		,NULL
		,NULL );

	//	���W�X�g�����J��
	Status	= ZwOpenKey( &Key, KEY_QUERY_VALUE, &ObjAttribute );
	if( NT_SUCCESS( Status ) )
	{
		//	���W�X�g���l�����쐬����
		RtlInitUnicodeString( &RegistryValue, Value );

		//	���W�X�g���l�̃T�C�Y���擾����
		Status	= ZwQueryValueKey(
					 Key
					,&RegistryValue
					,KeyValuePartialInformation
					,NULL
					,0
					,&ValueSize );
		if( ( Status == STATUS_OBJECT_NAME_NOT_FOUND ) || ( ValueSize == 0 ) )
		{
			ZwClose( Key );
			return;
		}

		//	���W�X�g���l���擾����o�b�t�@���쐬����
		ValueInfo	= (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag(
						 NonPagedPool
						,ValueSize
						,'100T' );
		if( ValueInfo == NULL )
		{
			ZwClose( Key );
			return;
		}

		//	���W�X�g���l���擾����
		Status	= ZwQueryValueKey(
					 Key
					,&RegistryValue
					,KeyValuePartialInformation
					,ValueInfo
					,ValueSize
					,&ValueSize );
		if( !NT_SUCCESS( Status ) )
		{
			ExFreePool( ValueInfo );
			ZwClose( Key );
			return;
		}

		//	�擾�������e��ޔ�����
		RtlCopyMemory( Buf, ValueInfo->Data, BufSize );

		//	�o�b�t�@���J������
		ExFreePool( ValueInfo );

		//	���W�X�g�������
		ZwClose( Key );
	}
}


//	HID Minidriver for Microsoft Xbox 360 Controller

#include	"driver.h"

//----------------------------------------------------------------------------------------------
//	�萔
//----------------------------------------------------------------------------------------------

#define	INTERVAL					-1
#define	INTERVAL_WAIT				-4 * 1000 * 10
#define	INTERVAL_STALL				-16 * 1000 * 10

//----------------------------------------------------------------------------------------------
//	�֐�
//----------------------------------------------------------------------------------------------

NTSTATUS StartDevice( IN PDEVICE_OBJECT, IN PDEVICE_EXTENSION, IN PIRP );
NTSTATUS QueryCapabilities( IN PDEVICE_OBJECT, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS SurpriseRemoval( IN PDEVICE_OBJECT, IN PDEVICE_EXTENSION, IN PIRP );
NTSTATUS RemoveDevice( IN PDEVICE_OBJECT, IN PDEVICE_EXTENSION, IN PIRP );
KSTART_ROUTINE WorkerThreadIn;
KSTART_ROUTINE WorkerThreadOut;

//----------------------------------------------------------------------------------------------
//	PnP
//----------------------------------------------------------------------------------------------
NTSTATUS PnP(
	 IN PDEVICE_OBJECT	DeviceObject
	,IN PIRP			Irp )
{
	//	�ϐ��錾
	NTSTATUS			Status;
	PDEVICE_EXTENSION	DeviceExtension	= GET_MINIDRIVER_DEVICE_EXTENSION( DeviceObject );
	PIO_STACK_LOCATION	CurrentIrpStack;

	//	�Q�ƃJ�E���^�����Z����
	Status	= IncrementReference( DeviceExtension );
	if( !NT_SUCCESS( Status ) )
	{
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	�}�C�i�[ �t�@���N�V�����ɂ���ď�����U�蕪����
	CurrentIrpStack	= IoGetCurrentIrpStackLocation( Irp );
	switch( CurrentIrpStack->MinorFunction )
	{
		case IRP_MN_START_DEVICE:
			Status	= StartDevice( DeviceObject, DeviceExtension, Irp );
			break;

		case IRP_MN_QUERY_CAPABILITIES:
			Status	= QueryCapabilities( DeviceObject, Irp, CurrentIrpStack );
			break;

		case IRP_MN_SURPRISE_REMOVAL:
			Status	= SurpriseRemoval( DeviceObject, DeviceExtension, Irp );
			break;

		case IRP_MN_REMOVE_DEVICE:
			Status	= RemoveDevice( DeviceObject, DeviceExtension, Irp );
			break;

		default:
			//	IRP_MJ_PNP �v�������ʃh���C�o�ɑ���
			Status	= SendRequest( DeviceObject, Irp );
			break;
	}

	//	�Q�ƃJ�E���^�����Z����
	DecrementReference( DeviceExtension );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	StartDevice
//----------------------------------------------------------------------------------------------
NTSTATUS StartDevice(
	 IN PDEVICE_OBJECT		DeviceObject
	,IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp )
{
	//	�ϐ��錾
	NTSTATUS	Status;

	//	��� IRP_MN_START_DEVICE �v�������ʃh���C�o�ɑ���
	Status	= SendAndWaitRequest( DeviceObject, Irp );
	if( !NT_SUCCESS( Status ) )
	{
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	�f�o�C�X��L���ɂ���
	Status	= EnableDevice( DeviceExtension );

	//	IRP_MN_START_DEVICE �v��������������
	CompleteRequest( Irp, Status, 0 );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	QueryCapabilities
//----------------------------------------------------------------------------------------------
NTSTATUS QueryCapabilities(
	 IN PDEVICE_OBJECT		DeviceObject
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	�ϐ��錾
	NTSTATUS				Status;
	PDEVICE_CAPABILITIES	DeviceCapabilities;

	//	��� IRP_MN_QUERY_CAPABILITIES �v�������ʃh���C�o�ɑ���
	Status	= SendAndWaitRequest( DeviceObject, Irp );
	if( !NT_SUCCESS( Status ) )
	{
		return( Status );
	}

	//	���ʃh���C�o�Őݒ肳�ꂽ�f�o�C�X�̋@�\��ύX����
	DeviceCapabilities	= CurrentIrpStack->Parameters.DeviceCapabilities.Capabilities;
	DeviceCapabilities->SurpriseRemovalOK	= TRUE;

	//	IRP_MN_QUERY_CAPABILITIES �v��������������
	CompleteRequest( Irp, Status, 0 );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	SurpriseRemoval
//----------------------------------------------------------------------------------------------
NTSTATUS SurpriseRemoval(
	 IN PDEVICE_OBJECT		DeviceObject
	,IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp )
{
	//	�f�o�C�X�𖳌��ɂ���
	DisableDevice( DeviceExtension );

	//	IRP_MN_SURPRISE_REMOVAL �v�������ʃh���C�o�ɑ���
	return( SendRequest( DeviceObject, Irp ) );
}

//----------------------------------------------------------------------------------------------
//	RemoveDevice
//----------------------------------------------------------------------------------------------
NTSTATUS RemoveDevice(
	 IN PDEVICE_OBJECT		DeviceObject
	,IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp )
{
	//	�f�o�C�X�𖳌��ɂ���
	DisableDevice( DeviceExtension );

	//	IRP_MN_REMOVE_DEVICE �v�������ʃh���C�o�ɑ���
	return( SendRequest( DeviceObject, Irp ) );
}

//----------------------------------------------------------------------------------------------
//	EnableDevice
//----------------------------------------------------------------------------------------------
NTSTATUS EnableDevice( IN PDEVICE_EXTENSION DeviceExtension )
{
	//	�ϐ��錾
	NTSTATUS	Status;
	HANDLE		ThreadHandle;

	//	�f�o�C�X��L���ɂ���
	if( DeviceExtension->DeviceState == Disable )
	{
		//	USB �|�[�g������������
		Status	= SetupPort( DeviceExtension->NextDeviceObject );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	�f�o�C�X�L�q�q���擾����
		Status	= GetDeviceDescriptor(
					 DeviceExtension->NextDeviceObject
					,&DeviceExtension->DeviceDescriptor );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	�R���t�B�M�����[�V�����L�q�q���擾����
		Status	= GetConfigurationDescriptor(
					 DeviceExtension->NextDeviceObject
					,&DeviceExtension->ConfigurationDescriptor );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	�C���^�[�t�F�C�X�L�q�q���擾����
		Status	= GetInterfaceDescriptor(
					 DeviceExtension->ConfigurationDescriptor
					,&DeviceExtension->InterfaceDescriptor );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	�R���t�B�M�����[�V������I������
		Status	= SelectConfiguration(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->ConfigurationDescriptor
					,DeviceExtension->InterfaceDescriptor
					,&DeviceExtension->InputPipeHandle
					,&DeviceExtension->OutputPipeHandle );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	�ݒ��ǂݍ��܂���
		DeviceExtension->ResetDevice	= TRUE;

		//	�f�o�C�X�̏�Ԃ�L���ɂ���
		DeviceExtension->DeviceState	= Enable;
	}

	//	���[�J �X���b�h���J�n����
	if( DeviceExtension->ThreadState == Disable )
	{
		//	���͗p���[�J �X���b�h���J�n����
		DeviceExtension->ThreadState	= Enable;
		PsCreateSystemThread(
			 &ThreadHandle
			,THREAD_ALL_ACCESS
			,NULL
			,NULL
			,NULL
			,WorkerThreadIn
			,DeviceExtension );
		ObReferenceObjectByHandle(
			 ThreadHandle
			,THREAD_ALL_ACCESS
			,NULL
			,KernelMode
			,&DeviceExtension->ThreadObjectIn
			,NULL );
		ZwClose( ThreadHandle );

		//	�o�͗p���[�J �X���b�h���J�n����
		if( DeviceExtension->OutputPipeHandle != NULL )
		{
			PsCreateSystemThread(
				 &ThreadHandle
				,THREAD_ALL_ACCESS
				,NULL
				,NULL
				,NULL
				,WorkerThreadOut
				,DeviceExtension );
			ObReferenceObjectByHandle(
				 ThreadHandle
				,THREAD_ALL_ACCESS
				,NULL
				,KernelMode
				,&DeviceExtension->ThreadObjectOut
				,NULL );
			ZwClose( ThreadHandle );
		}
	}

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	WorkerThreadIn
//----------------------------------------------------------------------------------------------
VOID WorkerThreadIn( IN PVOID Context )
{
	//	�ϐ��錾
	NTSTATUS			Status;
	PDEVICE_EXTENSION	DeviceExtension	= Context;
	SETTING				Setting;
	KIRQL				Irql;
	INPUT_REPORT		ControllerReport;
	LARGE_INTEGER		TimeOut;

	//	�X���b�h�̗D�揇�ʂ�������
	KeSetPriorityThread( KeGetCurrentThread(), LOW_REALTIME_PRIORITY );

	for(;;)
	{
		//	�X���b�h�̏I�����`�F�b�N����
		if( DeviceExtension->ThreadState == Disable )
		{
			break;
		}

		//	�R���g���[���[�̐ݒ��ǂݍ���
		if(	DeviceExtension->ResetDevice		== TRUE
		&&	DeviceExtension->OutputPipeHandle	== NULL )
		{
			//	�ݒ��ǂݍ���
			LoadSetting( &Setting );

			//	���̓��|�[�g����̊��荞�݂��֎~����
			KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

			//	�ݒ�𔽉f����
			if( DeviceExtension->ResetDevice == TRUE )
			{
				RtlCopyMemory( &DeviceExtension->Setting, &Setting, sizeof( SETTING ) );
				DeviceExtension->ResetDevice	= FALSE;
			}

			//	���̓��|�[�g����̊��荞�݂�������
			KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );
		}

		//	�R���g���[���[ ���|�[�g�̓��͂��s��
		RtlZeroMemory( &ControllerReport, sizeof( INPUT_REPORT ) );
		ControllerReport.Length	= sizeof( INPUT_REPORT );
		Status	= InputReport(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->InputPipeHandle
					,&ControllerReport );
		if( NT_SUCCESS( Status ) )
		{
			//	���̓��|�[�g����̊��荞�݂��֎~����
			KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

			//	�R���g���[���[ ���|�[�g�𔽉f����
			if( ControllerReport.Length == sizeof( INPUT_REPORT ) )
			{
				RtlCopyMemory(
					 &DeviceExtension->ControllerReport
					,&ControllerReport
					,sizeof( INPUT_REPORT ) );
			}
			TimeOut.QuadPart	= INTERVAL;

			//	���̓��|�[�g����̊��荞�݂�������
			KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );
		} else {
			//	�G���[������������p�C�v�����Z�b�g����
			if( Status != STATUS_CANCELLED && Status != STATUS_DEVICE_NOT_CONNECTED )
			{
				ResetPipe(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->InputPipeHandle );
			}
			TimeOut.QuadPart	= INTERVAL_STALL;
		}
		//	�ҋ@����
		KeDelayExecutionThread( KernelMode, FALSE, &TimeOut );
	}

	//	�X���b�h���J������
	ObDereferenceObject( DeviceExtension->ThreadObjectIn );
	DeviceExtension->ThreadObjectIn	= NULL;

	//	�X���b�h���I���������Ƃ�ʒm����
	KeSetEvent( &DeviceExtension->ThreadExitingIn, 0 ,FALSE );

	//	�X���b�h���I������
	PsTerminateSystemThread( STATUS_SUCCESS );
}

//----------------------------------------------------------------------------------------------
//	WorkerThreadOut
//----------------------------------------------------------------------------------------------
VOID WorkerThreadOut( IN PVOID Context )
{
	//	�ϐ��錾
	NTSTATUS			Status;
	PDEVICE_EXTENSION	DeviceExtension		= Context;
	SETTING				Setting;
	KIRQL				Irql;
	POUTPUT_REPORT		RingLightReport		= &DeviceExtension->RingLightReport;
	POUTPUT_REPORT		ActuatorsReport		= &DeviceExtension->ActuatorsReport;
	OUTPUT_REPORT		PrevActuatorsReport	= *ActuatorsReport;
	LONG				ReportCount			= 0;
	LARGE_INTEGER		TimeOut;

	//	�X���b�h�̗D�揇�ʂ�������
	KeSetPriorityThread( KeGetCurrentThread(), LOW_REALTIME_PRIORITY );

	for(;;)
	{
		//	�X���b�h�̏I�����`�F�b�N����
		if( DeviceExtension->ThreadState == Disable )
		{
			break;
		}

		//	�R���g���[���[�̐ݒ��ǂݍ���
		if( DeviceExtension->ResetDevice == TRUE )
		{
			//	�ݒ��ǂݍ���
			LoadSetting( &Setting );

			//	���̓��|�[�g����̊��荞�݂��֎~����
			KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

			//	�ݒ�𔽉f����
			if( DeviceExtension->ResetDevice == TRUE )
			{
				RtlCopyMemory( &DeviceExtension->Setting, &Setting, sizeof( SETTING ) );
				DeviceExtension->ActuatorsReported	= FALSE;
				DeviceExtension->RingLightReported	= FALSE;
				DeviceExtension->ResetDevice		= FALSE;
			}

			//	���̓��|�[�g����̊��荞�݂�������
			KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );
		}

		//	�����O ���C�g ���|�[�g�̏o�͂��s��
		if( DeviceExtension->RingLightReported == FALSE )
		{
			//	�����O ���C�g ���|�[�g���쐬����
			RingLightReport->Type		= 1;
			RingLightReport->Length		= 3;
			RingLightReport->Pattern	= (UCHAR)DeviceExtension->Setting.RingLightPattern;
			//	�����O ���C�g ���|�[�g���o�͂���
			Status	= OutputReport(
						 DeviceExtension->NextDeviceObject
						,DeviceExtension->OutputPipeHandle
						,&DeviceExtension->RingLightReport );
			if( NT_SUCCESS( Status ) )
			{
				DeviceExtension->RingLightReported	= TRUE;
			}
		}

		//	�A�N�`���G�[�^ ���|�[�g���쐬����
		ActuatorsReport->Type	= 0;
		ActuatorsReport->Length	= sizeof( OUTPUT_REPORT );
		if( DeviceExtension->ActuatorsReported == FALSE )
		{
			ActuatorsReport->Left	= 0;
			ActuatorsReport->Right	= 0;
		}

		//	����̒l�Ń��|�[�g�����񐔂�����������
		if(	ActuatorsReport->Left				!= PrevActuatorsReport.Left
		||	ActuatorsReport->Right				!= PrevActuatorsReport.Right
		||	DeviceExtension->ActuatorsReported	== FALSE )
		{
			ReportCount	= 0;
		}

		//	�A�N�`���G�[�^ ���|�[�g���o�͂���
		if( ReportCount < 250 )
		{
			Status	= OutputReport(
						 DeviceExtension->NextDeviceObject
						,DeviceExtension->OutputPipeHandle
						,&DeviceExtension->ActuatorsReport );
			if( NT_SUCCESS( Status ) )
			{
				//	���|�[�g�̓��e��ޔ�����
				PrevActuatorsReport					= *ActuatorsReport;
				ReportCount ++;
				DeviceExtension->ActuatorsReported	= TRUE;
				TimeOut.QuadPart					= INTERVAL;
			} else {
				//	�G���[������������p�C�v�����Z�b�g����
				if( Status != STATUS_CANCELLED && Status != STATUS_DEVICE_NOT_CONNECTED )
				{
					ResetPipe(
						 DeviceExtension->NextDeviceObject
						,DeviceExtension->OutputPipeHandle );
				}
				TimeOut.QuadPart	= INTERVAL_STALL;
			}
		} else {
			TimeOut.QuadPart	= INTERVAL_WAIT;
		}
		//	�ҋ@����
		KeDelayExecutionThread( KernelMode, FALSE, &TimeOut );
	}

	//	�X���b�h���J������
	ObDereferenceObject( DeviceExtension->ThreadObjectOut );
	DeviceExtension->ThreadObjectOut	= NULL;

	//	�X���b�h���I���������Ƃ�ʒm����
	KeSetEvent( &DeviceExtension->ThreadExitingOut, 0 ,FALSE );

	//	�X���b�h���I������
	PsTerminateSystemThread( STATUS_SUCCESS );
}

//----------------------------------------------------------------------------------------------
//	DisableDevice
//----------------------------------------------------------------------------------------------
NTSTATUS DisableDevice( IN PDEVICE_EXTENSION DeviceExtension )
{
	//	���[�J �X���b�h���I������̂�҂�
	if( DeviceExtension->ThreadState == Enable )
	{
		//	���[�J �X���b�h���I������
		DeviceExtension->ThreadState	= Disable;

		//	���͗p�p�C�v���A�{�[�g����
		AbortPipe( DeviceExtension->NextDeviceObject, DeviceExtension->InputPipeHandle );
		//	���͗p���[�J �X���b�h����������̂�҂�
		KeWaitForSingleObject(
			 &DeviceExtension->ThreadExitingIn
			,Executive
			,KernelMode
			,FALSE
			,NULL );

		if( DeviceExtension->OutputPipeHandle != NULL )
		{
			//	�o�͗p�p�C�v���A�{�[�g����
			AbortPipe( DeviceExtension->NextDeviceObject, DeviceExtension->OutputPipeHandle );
			//	�o�͗p���[�J �X���b�h����������̂�҂�
			KeWaitForSingleObject(
				 &DeviceExtension->ThreadExitingOut
				,Executive
				,KernelMode
				,FALSE
				,NULL );
		}
	}

	//	�h���C�o�̏�Ԃ𖳌��ɂ���
	DeviceExtension->DriverState	= Disable;

	//	�Q�ƏI���C�x���g������������
	KeResetEvent( &DeviceExtension->ReferenceEnd );

	//	�Q�ƃJ�E���^�����Z����
	DecrementReference( DeviceExtension );
	DecrementReference( DeviceExtension );

	//	�S�Ă� I/O �v�����I�����A�Q�ƏI�����ʒm�����܂őҋ@����
	KeWaitForSingleObject( &DeviceExtension->ReferenceEnd, Executive, KernelMode, FALSE, NULL);

	//	�f�o�C�X�𖳌��ɂ���
	if( DeviceExtension->DeviceState == Enable )
	{
		//	�R���t�B�M�����[�V�����̑I������������
		DeselectConfiguration( DeviceExtension->NextDeviceObject );

		//	�R���t�B�M�����[�V�����L�q�q�p�o�b�t�@���m�ۂ���Ă���ΊJ������
		if( DeviceExtension->ConfigurationDescriptor != NULL )
		{
			ExFreePool( DeviceExtension->ConfigurationDescriptor );
			DeviceExtension->ConfigurationDescriptor	= NULL;
		}

		//	�C���^�[�t�F�C�X�L�q�q�p�o�b�t�@���m�ۂ���Ă���ΊJ������
		if( DeviceExtension->InterfaceDescriptor != NULL )
		{
			ExFreePool( DeviceExtension->InterfaceDescriptor );
			DeviceExtension->InterfaceDescriptor	= NULL;
		}

		//	�f�o�C�X�̏�Ԃ𖳌��ɂ���
		DeviceExtension->DeviceState	= Disable;
	}

	//	�h���C�o�̏�Ԃ�L���ɂ���
	DeviceExtension->DriverState	= Enable;

	//	�Q�ƃJ�E���^�̐������킹��
	IncrementReference( DeviceExtension );
	IncrementReference( DeviceExtension );

	return( STATUS_SUCCESS );
}

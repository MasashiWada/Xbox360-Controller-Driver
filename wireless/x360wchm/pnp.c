
//	HID Minidriver for Microsoft Xbox 360 Wireless Controller

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
KSTART_ROUTINE WorkerThreadIdc;

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
	UCHAR		ThreadId;

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
					,DeviceExtension->InputPipeHandle
					,DeviceExtension->OutputPipeHandle );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	�ݒ��ǂݍ��܂���
		for( ThreadId = 0; ThreadId < ControllerCount; ThreadId ++ )
		{
			DeviceExtension->ResetDevice[ThreadId]	= TRUE;
		}

		//	�f�o�C�X�̏�Ԃ�L���ɂ���
		DeviceExtension->DeviceState	= Enable;
	}

	//	���[�J �X���b�h���J�n����
	if( DeviceExtension->ThreadStateIo == Disable )
	{
		DeviceExtension->ThreadStateIo	= Enable;
		for( ThreadId = 0; ThreadId < ControllerCount; ThreadId ++ )
		{
			//	�X���b�h �p�����[�^��ݒ肷��
			DeviceExtension->ThreadParam[ThreadId].ThreadId			= ThreadId;
			DeviceExtension->ThreadParam[ThreadId].DeviceExtension	= DeviceExtension;
			//	���͗p���[�J �X���b�h���J�n����
			if( DeviceExtension->InputPipeHandle[ThreadId] != NULL )
			{
				//	���͗p���[�J �X���b�h���J�n����
				PsCreateSystemThread(
					 &ThreadHandle
					,THREAD_ALL_ACCESS
					,NULL
					,NULL
					,NULL
					,WorkerThreadIn
					,&DeviceExtension->ThreadParam[ThreadId] );
				ObReferenceObjectByHandle(
					 ThreadHandle
					,THREAD_ALL_ACCESS
					,NULL
					,KernelMode
					,&DeviceExtension->ThreadObjectIn[ThreadId]
					,NULL );
				ZwClose( ThreadHandle );
			}
			//	�o�͗p���[�J �X���b�h���J�n����
			if( DeviceExtension->OutputPipeHandle[ThreadId] != NULL )
			{
				PsCreateSystemThread(
					 &ThreadHandle
					,THREAD_ALL_ACCESS
					,NULL
					,NULL
					,NULL
					,WorkerThreadOut
					,&DeviceExtension->ThreadParam[ThreadId] );
				ObReferenceObjectByHandle(
					 ThreadHandle
					,THREAD_ALL_ACCESS
					,NULL
					,KernelMode
					,&DeviceExtension->ThreadObjectOut[ThreadId]
					,NULL );
				ZwClose( ThreadHandle );
			}
		}
	}

	//	IRP �����p���[�J �X���b�h���J�n����
	if( DeviceExtension->ThreadStateIdc == Disable )
	{
		DeviceExtension->ThreadStateIdc	= Enable;
		PsCreateSystemThread(
			 &ThreadHandle
			,THREAD_ALL_ACCESS
			,NULL
			,NULL
			,NULL
			,WorkerThreadIdc
			,DeviceExtension );
		ObReferenceObjectByHandle(
			 ThreadHandle
			,THREAD_ALL_ACCESS
			,NULL
			,KernelMode
			,&DeviceExtension->ThreadObjectIdc
			,NULL );
		ZwClose( ThreadHandle );
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
	PTHREAD_PARAM		ThreadParam		= Context;
	PDEVICE_EXTENSION	DeviceExtension	= ThreadParam->DeviceExtension;
	UCHAR				ThreadId		= ThreadParam->ThreadId;
	SETTING				Setting;
	KIRQL				Irql;
	INPUT_REPORT		ControllerReport;
	LARGE_INTEGER		TimeOut;

	//	�X���b�h�̗D�揇�ʂ�������
	KeSetPriorityThread( KeGetCurrentThread(), LOW_REALTIME_PRIORITY );

	for(;;)
	{
		//	�X���b�h�̏I�����`�F�b�N����
		if( DeviceExtension->ThreadStateIo == Disable )
		{
			break;
		}

		//	�R���g���[���[�̐ݒ��ǂݍ���
		if(	DeviceExtension->ResetDevice[ThreadId]		== TRUE
		&&	DeviceExtension->OutputPipeHandle[ThreadId]	== NULL )
		{
			//	�ݒ��ǂݍ���
			LoadSetting( &Setting );

			//	���̓��|�[�g����̊��荞�݂��֎~����
			KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

			//	�ݒ�𔽉f����
			if( DeviceExtension->ResetDevice[ThreadId] == TRUE )
			{
				RtlCopyMemory( &DeviceExtension->Setting, &Setting, sizeof( SETTING ) );
				DeviceExtension->ResetDevice[ThreadId]	= FALSE;
			}

			//	���̓��|�[�g����̊��荞�݂�������
			KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );
		}

		//	�R���g���[���[ ���|�[�g�̓��͂��s��
		Status	= InputReport(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->InputPipeHandle[ThreadId]
					,&ControllerReport
					,sizeof( INPUT_REPORT ) );
		if( NT_SUCCESS( Status ) )
		{
			//	�R���g���[���[���ڑ����ꂽ�ꍇ
			if(	ControllerReport.Type		== 0x08
			&&	ControllerReport.ParamType	== 0x80 )
			{
				DeviceExtension->Connected[ThreadId]			= TRUE;
				DeviceExtension->ConnectionReported[ThreadId]	= TRUE;
				DeviceExtension->ResetDevice[ThreadId]			= TRUE;
			}
			//	�R���g���[���[���ؒf���ꂽ�ꍇ
			if(	ControllerReport.Type		== 0x08
			&&	ControllerReport.ParamType	== 0x00 )
			{
				DeviceExtension->Connected[ThreadId]			= FALSE;
				DeviceExtension->ConnectionReported[ThreadId]	= TRUE;
			}
			//	���͗p���|�[�g���擾�����ꍇ
			if(	ControllerReport.ParamType				== 0x01
			&&	ControllerReport.Param1					== 0xF0
			&&	DeviceExtension->Connected[ThreadId]	== TRUE )
			{
				//	���̓��|�[�g����̊��荞�݂��֎~����
				KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

				//	�R���g���[���[ ���|�[�g�𔽉f����
				RtlCopyMemory(
					 &DeviceExtension->ControllerReport[ThreadId]
					,&ControllerReport
					,sizeof( INPUT_REPORT ) );

				//	���̓��|�[�g����̊��荞�݂�������
				KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );

				//	���|�[�g���ꂽ������ޔ�����
				KeQuerySystemTime( &DeviceExtension->ControllerReportTime[ThreadId] );
			}
			TimeOut.QuadPart	= INTERVAL;
		} else {
			//	�G���[������������p�C�v�����Z�b�g����
			if( Status != STATUS_CANCELLED && Status != STATUS_DEVICE_NOT_CONNECTED )
			{
				ResetPipe(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->InputPipeHandle[ThreadId] );
			}
			TimeOut.QuadPart	= INTERVAL_STALL;
		}
		//	�ҋ@����
		KeDelayExecutionThread( KernelMode, FALSE, &TimeOut );
	}

	//	�X���b�h���J������
	ObDereferenceObject( DeviceExtension->ThreadObjectIn[ThreadId] );
	DeviceExtension->ThreadObjectIn[ThreadId]	= NULL;

	//	�X���b�h���I���������Ƃ�ʒm����
	KeSetEvent( &DeviceExtension->ThreadExitingIn[ThreadId], 0 ,FALSE );

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
	PTHREAD_PARAM		ThreadParam			= Context;
	PDEVICE_EXTENSION	DeviceExtension		= ThreadParam->DeviceExtension;
	UCHAR				ThreadId			= ThreadParam->ThreadId;
	SETTING				Setting;
	KIRQL				Irql;
	POUTPUT_REPORT		ConnectionReport	= &DeviceExtension->ConnectionReport[ThreadId];
	POUTPUT_REPORT		RingLightReport		= &DeviceExtension->RingLightReport[ThreadId];
	POUTPUT_REPORT		ActuatorsReport		= &DeviceExtension->ActuatorsReport[ThreadId];
	OUTPUT_REPORT		PrevActuatorsReport	= *ActuatorsReport;
	LONG				ReportCount			= 0;
	LARGE_INTEGER		TimeOut;

	//	�X���b�h�̗D�揇�ʂ�������
	KeSetPriorityThread( KeGetCurrentThread(), LOW_REALTIME_PRIORITY );

	for(;;)
	{
		//	�X���b�h�̏I�����`�F�b�N����
		if( DeviceExtension->ThreadStateIo == Disable )
		{
			break;
		}

		//	�R���g���[���[�̐ݒ��ǂݍ���
		if( DeviceExtension->ResetDevice[ThreadId] == TRUE )
		{
			//	�ݒ��ǂݍ���
			LoadSetting( &Setting );

			//	���̓��|�[�g����̊��荞�݂��֎~����
			KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

			//	�ݒ�𔽉f����
			if( DeviceExtension->ResetDevice[ThreadId] == TRUE )
			{
				RtlCopyMemory( &DeviceExtension->Setting, &Setting, sizeof( SETTING ) );
				DeviceExtension->ActuatorsReported[ThreadId]	= FALSE;
				DeviceExtension->RingLightReported[ThreadId]	= FALSE;
				DeviceExtension->ResetDevice[ThreadId]			= FALSE;
			}

			//	���̓��|�[�g����̊��荞�݂�������
			KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );
		}

		//	�R���g���[���[�̐ڑ��m�F���|�[�g�̏o�͂��s��
		if( DeviceExtension->ConnectionReported[ThreadId] == FALSE )
		{
			//	�R���g���[���[�̐ڑ��m�F���|�[�g���쐬����
			ConnectionReport->Type		= 0x08;
			ConnectionReport->ParamType	= 0x00;
			ConnectionReport->Param1	= 0x0F;
			ConnectionReport->Param2	= 0xC0;
			//	�R���g���[���[�̐ڑ��m�F���|�[�g���o�͂���
			Status	= OutputReport(
						 DeviceExtension->NextDeviceObject
						,DeviceExtension->OutputPipeHandle[ThreadId]
						,&DeviceExtension->ConnectionReport[ThreadId]
						,sizeof( OUTPUT_REPORT ) );
			if( NT_SUCCESS( Status ) )
			{
				;
			}
		}

		//	�����O ���C�g ���|�[�g�̏o�͂��s��
		if(	DeviceExtension->RingLightReported[ThreadId]	== FALSE
		&&	DeviceExtension->Connected[ThreadId]			== TRUE )
		{
			//	�����O ���C�g ���|�[�g���쐬����
			RingLightReport->Type		= 0x00;
			RingLightReport->ParamType	= 0x00;
			RingLightReport->Param1		= 0x08;
			if( DeviceExtension->Setting.RingLightPattern == 16 )
			{
				RingLightReport->Param2	= 0x42 + ThreadId;
			} else {
				RingLightReport->Param2	= 0x40
											+ (UCHAR)DeviceExtension->Setting.RingLightPattern;
			}
			//	�����O ���C�g ���|�[�g���o�͂���
			Status	= OutputReport(
						 DeviceExtension->NextDeviceObject
						,DeviceExtension->OutputPipeHandle[ThreadId]
						,&DeviceExtension->RingLightReport[ThreadId]
						,sizeof( OUTPUT_REPORT ) );
			if( NT_SUCCESS( Status ) )
			{
				DeviceExtension->RingLightReported[ThreadId]	= TRUE;
			}
		}

		//	�A�N�`���G�[�^ ���|�[�g���쐬����
		if( DeviceExtension->Connected[ThreadId] == TRUE )
		{
			ActuatorsReport->Type		= 0x00;
			ActuatorsReport->ParamType	= 0x01;
			ActuatorsReport->Param1		= 0x0F;
			ActuatorsReport->Param2		= 0xC0;
			if( DeviceExtension->ActuatorsReported[ThreadId] == FALSE )
			{
				ActuatorsReport->LeftActuator	= 0;
				ActuatorsReport->RightActuator	= 0;
			}

			//	����̒l�Ń��|�[�g�����񐔂�����������
			if(	ActuatorsReport->LeftActuator		!= PrevActuatorsReport.LeftActuator
			||	ActuatorsReport->RightActuator		!= PrevActuatorsReport.RightActuator
			||	DeviceExtension->ActuatorsReported[ThreadId]	== FALSE )
			{
				ReportCount	= 0;
			}

			//	�A�N�`���G�[�^ ���|�[�g���o�͂���
			if( ReportCount < 250 )
			{
				Status	= OutputReport(
							 DeviceExtension->NextDeviceObject
							,DeviceExtension->OutputPipeHandle[ThreadId]
							,&DeviceExtension->ActuatorsReport[ThreadId]
							,sizeof( OUTPUT_REPORT ) );
				if( NT_SUCCESS( Status ) )
				{
					//	���|�[�g�̓��e��ޔ�����
					PrevActuatorsReport								= *ActuatorsReport;
					ReportCount ++;
					DeviceExtension->ActuatorsReported[ThreadId]	= TRUE;
					TimeOut.QuadPart								= INTERVAL;
				} else {
					//	�G���[������������p�C�v�����Z�b�g����
					if( Status != STATUS_CANCELLED && Status != STATUS_DEVICE_NOT_CONNECTED )
					{
						ResetPipe(
							 DeviceExtension->NextDeviceObject
							,DeviceExtension->OutputPipeHandle[ThreadId] );
					}
					TimeOut.QuadPart	= INTERVAL_STALL;
				}
			} else {
				TimeOut.QuadPart	= INTERVAL_WAIT;
			}
		} else {
			TimeOut.QuadPart	= INTERVAL_STALL;
		}
		//	�ҋ@����
		KeDelayExecutionThread( KernelMode, FALSE, &TimeOut );
	}

	//	�X���b�h���J������
	ObDereferenceObject( DeviceExtension->ThreadObjectOut[ThreadId] );
	DeviceExtension->ThreadObjectOut[ThreadId]	= NULL;

	//	�X���b�h���I���������Ƃ�ʒm����
	KeSetEvent( &DeviceExtension->ThreadExitingOut[ThreadId], 0 ,FALSE );

	//	�X���b�h���I������
	PsTerminateSystemThread( STATUS_SUCCESS );
}

//----------------------------------------------------------------------------------------------
//	WorkerThreadIdc
//----------------------------------------------------------------------------------------------
VOID WorkerThreadIdc( IN PVOID Context )
{
	//	�ϐ��錾
	NTSTATUS			Status;
	PDEVICE_EXTENSION	DeviceExtension	= Context;
	KIRQL				Irql;
	PIRP				Irp;
	PIRP *				Irps;
	LARGE_INTEGER		TimeOut;

	//	�X���b�h�̗D�揇�ʂ�������
	KeSetPriorityThread( KeGetCurrentThread(), LOW_REALTIME_PRIORITY );

	for(;;)
	{
		//	�X���b�h�̏I�����`�F�b�N����i�L���[�� IRP ���c���Ă���ꍇ�͑S�ď�������j
		if( DeviceExtension->ThreadStateIdc == Disable && DeviceExtension->IrpCount == 0 )
		{
			break;
		}

		//	�r��������s��
		KeAcquireSpinLock( &DeviceExtension->LockIrp, &Irql );

		//	�L���[���� IRP ���擾����
		if( DeviceExtension->IrpCount > 0 )
		{
			//	�L���[���� IRP ���擾����
			Irp	= DeviceExtension->Irps[0];
			//	�擾���� IRP ���L���[�����菜��
			if( DeviceExtension->IrpCount > 1 )
			{
				Irps	= (PIRP *)ExAllocatePoolWithTag(
							 NonPagedPool
							,(DeviceExtension->IrpCount - 1) * sizeof( PIRP )
							,'300T' );
				RtlCopyMemory(
					 Irps
					,&DeviceExtension->Irps[1]
					,(DeviceExtension->IrpCount - 1) * sizeof( PIRP ) );
			}
			if( DeviceExtension->Irps != NULL )
			{
				ExFreePool( DeviceExtension->Irps );
				DeviceExtension->Irps = NULL;
			}
			//	�V�����L���[��ݒ肷��
			DeviceExtension->Irps	= Irps;
			DeviceExtension->IrpCount --;
		} else {
			Irp	= NULL;
		}

		//	�r���������������
		KeReleaseSpinLock( &DeviceExtension->LockIrp, Irql );

		//	IRP ����������
		if( Irp != NULL )
		{
			ReadReport( Irp, DeviceExtension );
		} else {
			//	�ҋ@����
			TimeOut.QuadPart	= INTERVAL;
			KeDelayExecutionThread( KernelMode, FALSE, &TimeOut );
		}
	}

	//	�X���b�h���J������
	ObDereferenceObject( DeviceExtension->ThreadObjectIdc );
	DeviceExtension->ThreadObjectIdc	= NULL;

	//	�X���b�h���I���������Ƃ�ʒm����
	KeSetEvent( &DeviceExtension->ThreadExitingIdc, 0 ,FALSE );

	//	�X���b�h���I������
	PsTerminateSystemThread( STATUS_SUCCESS );
}

//----------------------------------------------------------------------------------------------
//	DisableDevice
//----------------------------------------------------------------------------------------------
NTSTATUS DisableDevice( IN PDEVICE_EXTENSION DeviceExtension )
{
	//	�ϐ��錾
	UCHAR	ThreadId;

	//	���[�J �X���b�h���I������̂�҂�
	if( DeviceExtension->ThreadStateIo == Enable )
	{
		//	���[�J �X���b�h���I������
		DeviceExtension->ThreadStateIo	= Disable;
		for( ThreadId = 0; ThreadId < ControllerCount; ThreadId ++ )
		{
			if( DeviceExtension->InputPipeHandle[ThreadId] != NULL )
			{
				//	���͗p�p�C�v���A�{�[�g����
				AbortPipe(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->InputPipeHandle[ThreadId] );
				//	���͗p���[�J �X���b�h����������̂�҂�
				KeWaitForSingleObject(
					 &DeviceExtension->ThreadExitingIn[ThreadId]
					,Executive
					,KernelMode
					,FALSE
					,NULL );
			}
			if( DeviceExtension->OutputPipeHandle[ThreadId] != NULL )
			{
				//	�o�͗p�p�C�v���A�{�[�g����
				AbortPipe(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->OutputPipeHandle[ThreadId] );
				//	�o�͗p���[�J �X���b�h����������̂�҂�
				KeWaitForSingleObject(
					 &DeviceExtension->ThreadExitingOut[ThreadId]
					,Executive
					,KernelMode
					,FALSE
					,NULL );
			}
		}
	}

	//	�h���C�o�̏�Ԃ𖳌��ɂ���
	DeviceExtension->DriverState	= Disable;

	//	IRP �����p���[�J �X���b�h����������̂�҂�
	if( DeviceExtension->ThreadStateIdc == Enable )
	{
		//	IRP �����p���[�J �X���b�h���I������
		DeviceExtension->ThreadStateIdc	= Disable;

		//	IRP �����p���[�J �X���b�h����������̂�҂�
		KeWaitForSingleObject(
			 &DeviceExtension->ThreadExitingIdc
			,Executive
			,KernelMode
			,FALSE
			,NULL );
	}

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


//	HID Minidriver for Microsoft Xbox 360 Wireless Controller

#include	"driver.h"

//----------------------------------------------------------------------------------------------
//	�֐�
//----------------------------------------------------------------------------------------------

NTSTATUS GetPortStatus( IN PDEVICE_OBJECT );
NTSTATUS ResetPort( IN PDEVICE_OBJECT );
NTSTATUS PipeRequest( IN PDEVICE_OBJECT, IN USHORT, IN USBD_PIPE_HANDLE );
NTSTATUS CallUSBDI( IN PDEVICE_OBJECT, IN ULONG, IN OUT PVOID );

//----------------------------------------------------------------------------------------------
//	SetupPort
//----------------------------------------------------------------------------------------------
NTSTATUS SetupPort( IN PDEVICE_OBJECT NextDeviceObject )
{
	//	�ϐ��錾
	NTSTATUS	Status;

	//	USB �|�[�g�̏�Ԃ��擾����
	Status	= GetPortStatus( NextDeviceObject );
	if( !NT_SUCCESS( Status ) )
	{
		//	USB �|�[�g������������
		Status	= ResetPort( NextDeviceObject );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	�ēx USB �|�[�g�̏�Ԃ��擾����
		Status	= GetPortStatus( NextDeviceObject );
	}

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	GetPortStatus
//----------------------------------------------------------------------------------------------
NTSTATUS GetPortStatus( IN PDEVICE_OBJECT NextDeviceObject )
{
	//	�ϐ��錾
	NTSTATUS	Status;
	ULONG		PortStatus;

	//	USB �|�[�g�̏�Ԃ��擾����
	Status	= CallUSBDI( NextDeviceObject, IOCTL_INTERNAL_USB_GET_PORT_STATUS, &PortStatus );
	if( !NT_SUCCESS( Status ) )
	{
		return( Status );
	}

	//	�f�o�C�X�� USB �|�[�g�ɐڑ�����Ă��邩���ׂ�
	if( ( PortStatus & USBD_PORT_CONNECTED ) != USBD_PORT_CONNECTED )
	{
		return( STATUS_NO_SUCH_DEVICE );
	}

	//	USB �|�[�g���L�������ׂ�
	if( ( PortStatus & USBD_PORT_ENABLED ) != USBD_PORT_ENABLED )
	{
		return( STATUS_DEVICE_NOT_READY );
	}

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	ResetPort
//----------------------------------------------------------------------------------------------
NTSTATUS ResetPort( IN PDEVICE_OBJECT NextDeviceObject )
{
	//	USB �|�[�g�����Z�b�g����
	return( CallUSBDI( NextDeviceObject, IOCTL_INTERNAL_USB_RESET_PORT, NULL ) );
}

//----------------------------------------------------------------------------------------------
//	GetDeviceDescriptor
//----------------------------------------------------------------------------------------------
NTSTATUS GetDeviceDescriptor(
	 IN PDEVICE_OBJECT			NextDeviceObject
	,OUT PUSB_DEVICE_DESCRIPTOR	DeviceDescriptor )
{
	//	�ϐ��錾
	NTSTATUS	Status;
	USHORT		UrbSize	= sizeof( struct _URB_CONTROL_DESCRIPTOR_REQUEST );
	PURB		Urb;

	//	URB �p�o�b�t�@���쐬����
	Urb	= (PURB)ExAllocatePoolWithTag( NonPagedPool, UrbSize, '400T' );
	if( Urb == NULL )
	{
		return( STATUS_INSUFFICIENT_RESOURCES );
	}

	//	�f�o�C�X�L�q�q���擾����ׂ� URB ���쐬����
	UsbBuildGetDescriptorRequest(
		 Urb
		,UrbSize
		,USB_DEVICE_DESCRIPTOR_TYPE
		,0
		,0
		,DeviceDescriptor
		,NULL
		,sizeof( USB_DEVICE_DESCRIPTOR )
		,NULL );

	//	�f�o�C�X�L�q�q���擾����ׂ� URB �𔭍s����
	Status	= CallUSBDI( NextDeviceObject, IOCTL_INTERNAL_USB_SUBMIT_URB, Urb );
	if( !NT_SUCCESS( Status ) || !USBD_SUCCESS( Urb->UrbHeader.Status ) )
	{
		Status	= STATUS_UNSUCCESSFUL;
	}

	//	�쐬���� URB ���������
	ExFreePool( Urb );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	GetConfigurationDescriptor
//----------------------------------------------------------------------------------------------
NTSTATUS GetConfigurationDescriptor(
	 IN PDEVICE_OBJECT						NextDeviceObject
	,OUT PUSB_CONFIGURATION_DESCRIPTOR *	ConfigurationDescriptor )
{
	//	�ϐ��錾
	NTSTATUS	Status;
	USHORT		UrbSize	= sizeof( struct _URB_CONTROL_DESCRIPTOR_REQUEST );
	PURB		Urb;
	LONG		ConfigurationDescriptorSize
						= sizeof( USB_CONFIGURATION_DESCRIPTOR );

	//	URB �p�o�b�t�@���쐬����
	Urb	= (PURB)ExAllocatePoolWithTag( NonPagedPool, UrbSize, '500T' );
	if( Urb == NULL )
	{
		return( STATUS_INSUFFICIENT_RESOURCES );
	}

	//	�R���t�B�M�����[�V�����L�q�q�̓T�C�Y�����m��Ȃ̂Œ�������K�v������
	for(;;)
	{
		//	�R���t�B�M�����[�V�����L�q�q�p�o�b�t�@���쐬����
		*ConfigurationDescriptor	= ExAllocatePoolWithTag(
										 NonPagedPool
										,ConfigurationDescriptorSize
										,'600T' );
		if( *ConfigurationDescriptor == NULL )
		{
			ExFreePool( Urb );
			return( STATUS_INSUFFICIENT_RESOURCES );
		}

		//	�R���t�B�M�����[�V�����L�q�q���擾����ׂ� URB ���쐬����
		UsbBuildGetDescriptorRequest(
			 Urb
			,UrbSize
			,USB_CONFIGURATION_DESCRIPTOR_TYPE
			,0
			,0
			,*ConfigurationDescriptor
			,NULL
			,ConfigurationDescriptorSize
			,NULL );

		//	�R���t�B�M�����[�V�����L�q�q���擾����ׂ� URB �𔭍s����
		Status	= CallUSBDI( NextDeviceObject, IOCTL_INTERNAL_USB_SUBMIT_URB, Urb );
		if( !NT_SUCCESS( Status ) || !USBD_SUCCESS( Urb->UrbHeader.Status ) )
		{
			Status	= STATUS_UNSUCCESSFUL;
			break;
		}

		//	�쐬�����R���t�B�M�����[�V�����L�q�q�̃T�C�Y���������ꍇ�A�č쐬����
		if( ConfigurationDescriptorSize < (*ConfigurationDescriptor)->wTotalLength )
		{
			ConfigurationDescriptorSize	= (*ConfigurationDescriptor)->wTotalLength;
			ExFreePool( *ConfigurationDescriptor );
			*ConfigurationDescriptor	= NULL;
		} else {
			break;
		}
	}

	//	�쐬���� URB ���������
	ExFreePool( Urb );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	GetInterfaceDescriptor
//----------------------------------------------------------------------------------------------
NTSTATUS GetInterfaceDescriptor(
	 IN PUSB_CONFIGURATION_DESCRIPTOR	ConfigurationDescriptor
	,OUT PUSB_INTERFACE_DESCRIPTOR * *	InterfaceDescriptor )
{
	//	�ϐ��錾
	NTSTATUS	Status	= STATUS_SUCCESS;
	LONG		Index;

	//	�C���^�[�t�F�C�X�L�q�q�p�o�b�t�@���쐬����
	*InterfaceDescriptor	= ExAllocatePoolWithTag(
								 NonPagedPool
								,ConfigurationDescriptor->bNumInterfaces
									* sizeof( PUSB_INTERFACE_DESCRIPTOR )
								,'700T' );
	if( *InterfaceDescriptor == NULL )
	{
		return( STATUS_INSUFFICIENT_RESOURCES );
	}

	//	�S�ẴC���^�[�t�F�C�X�L�q�q���擾����
	(*InterfaceDescriptor)[0]	= (PUSB_INTERFACE_DESCRIPTOR)ConfigurationDescriptor;
	for( Index = 0; Index < ConfigurationDescriptor->bNumInterfaces; Index ++ )
	{
		//	�C���^�[�t�F�C�X�L�q�q���擾����
		(*InterfaceDescriptor)[Index]	= USBD_ParseConfigurationDescriptorEx(
											 ConfigurationDescriptor
											,(*InterfaceDescriptor)[max( 0, Index - 1 )]
											,Index
											,0
											,-1
											,-1
											,-1 );
		if( (*InterfaceDescriptor)[Index] == NULL )
		{
			Status	= STATUS_UNSUCCESSFUL;
			break;
		}
	}

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	SelectConfiguration
//----------------------------------------------------------------------------------------------
NTSTATUS SelectConfiguration(
	 IN PDEVICE_OBJECT					NextDeviceObject
	,IN PUSB_CONFIGURATION_DESCRIPTOR	ConfigurationDescriptor
	,IN PUSB_INTERFACE_DESCRIPTOR *		InterfaceDescriptor
	,OUT USBD_PIPE_HANDLE *				InputPipeHandle
	,OUT USBD_PIPE_HANDLE *				OutputPipeHandle )
{
	//	�ϐ��錾
	NTSTATUS					Status;
	LONG						InterfaceListEntrySize
									= ( ConfigurationDescriptor->bNumInterfaces + 1 )
										* sizeof( USBD_INTERFACE_LIST_ENTRY );
	PUSBD_INTERFACE_LIST_ENTRY	InterfaceListEntry;
	LONG						Index;
	PURB						Urb;
	PUSBD_INTERFACE_INFORMATION	InterfaceInformation;

	//	�C���^�[�t�F�C�X ���X�g�p�o�b�t�@���쐬����
	InterfaceListEntry	= ExAllocatePoolWithTag(
							  NonPagedPool
							 ,InterfaceListEntrySize
							 ,'800T' );
	if( InterfaceListEntry == NULL )
	{
		return( STATUS_INSUFFICIENT_RESOURCES );
	}

	//	�C���^�[�t�F�C�X ���X�g��ݒ肷��
	RtlZeroMemory( InterfaceListEntry, InterfaceListEntrySize );
	for( Index = 0; Index < ConfigurationDescriptor->bNumInterfaces; Index ++ )
	{
		InterfaceListEntry[Index].InterfaceDescriptor	= InterfaceDescriptor[Index];
	}

	//	�R���t�B�M�����[�V������I�����邽�߂� URB ���쐬����
	Urb	= USBD_CreateConfigurationRequestEx(
			 ConfigurationDescriptor
			,InterfaceListEntry );
	if( Urb == NULL )
	{
		ExFreePool( InterfaceListEntry );
		return( STATUS_INSUFFICIENT_RESOURCES );
	}

	//	�R���t�B�M�����[�V������I������ׂ� URB �𔭍s����
	Status	= CallUSBDI( NextDeviceObject, IOCTL_INTERNAL_USB_SUBMIT_URB, Urb );
	if( NT_SUCCESS( Status ) )
	{
		for( Index = 0; Index < ConfigurationDescriptor->bNumInterfaces; Index += 2 )
		{
			//	�R���g���[���[����葽���ꍇ�͖�������
			if( ControllerCount < Index / 2)
			{
				break;
			}

			//	�C���^�[�t�F�C�X �C���t�H���[�V�������擾����
			InterfaceInformation	= InterfaceListEntry[Index].Interface;
			//	�p�C�v���擾����
			switch( InterfaceInformation->NumberOfPipes )
			{
				case 0:
					Status	= STATUS_UNSUCCESSFUL;
					break;

				case 1:
					InputPipeHandle[Index / 2]	= InterfaceInformation->Pipes[0].PipeHandle;
					break;

				default:
					InputPipeHandle[Index / 2]	= InterfaceInformation->Pipes[0].PipeHandle;
					OutputPipeHandle[Index / 2]	= InterfaceInformation->Pipes[1].PipeHandle;
					break;
			}
		}
	}

	//	�C���^�[�t�F�C�X ���X�g�p�o�b�t�@���������
	ExFreePool( InterfaceListEntry );

	//	�쐬���� URB ���������
	ExFreePool( Urb );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	DeselectConfiguration
//----------------------------------------------------------------------------------------------
NTSTATUS DeselectConfiguration( IN PDEVICE_OBJECT NextDeviceObject )
{
	//	�ϐ��錾
	NTSTATUS	Status;
	USHORT		UrbSize	= sizeof( struct _URB_CONTROL_DESCRIPTOR_REQUEST );
	PURB		Urb;

	//	URB �p�o�b�t�@���쐬����
	Urb	= (PURB)ExAllocatePoolWithTag( NonPagedPool, UrbSize, '900T' );
	if( Urb == NULL )
	{
		return( STATUS_INSUFFICIENT_RESOURCES );
	}

	//	�R���t�B�M�����[�V�����I������������ׂ� URB ���쐬����
	UsbBuildSelectConfigurationRequest( Urb, UrbSize, NULL );

	//	�R���t�B�M�����[�V�����I������������ׂ� URB �𔭍s����
	Status	= CallUSBDI( NextDeviceObject, IOCTL_INTERNAL_USB_SUBMIT_URB, Urb );
	if( !NT_SUCCESS( Status ) || !USBD_SUCCESS( Urb->UrbHeader.Status ) )
	{
		ExFreePool( Urb );
		return( STATUS_UNSUCCESSFUL );
	}

	//	�쐬���� URB ���������
	ExFreePool( Urb );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	ResetPipe
//----------------------------------------------------------------------------------------------
NTSTATUS ResetPipe(
	 IN PDEVICE_OBJECT		NextDeviceObject
	,IN USBD_PIPE_HANDLE	PipeHandle )
{
	//	�p�C�v�����Z�b�g����
	return( PipeRequest( NextDeviceObject, URB_FUNCTION_RESET_PIPE, PipeHandle ) );
}

//----------------------------------------------------------------------------------------------
//	AbortPipe
//----------------------------------------------------------------------------------------------
NTSTATUS AbortPipe(
	 IN PDEVICE_OBJECT		NextDeviceObject
	,IN USBD_PIPE_HANDLE	PipeHandle )
{
	//	�p�C�v���A�{�[�g����
	return( PipeRequest( NextDeviceObject, URB_FUNCTION_ABORT_PIPE, PipeHandle ) );
}

//----------------------------------------------------------------------------------------------
//	PipeRequest
//----------------------------------------------------------------------------------------------
NTSTATUS PipeRequest(
	 IN PDEVICE_OBJECT		NextDeviceObject
	,IN USHORT				Function
	,IN USBD_PIPE_HANDLE	PipeHandle )
{
	//	�ϐ��錾
	NTSTATUS	Status;
	USHORT		UrbSize	= sizeof( struct _URB_PIPE_REQUEST );
	PURB		Urb;

	//	URB �p�o�b�t�@���쐬����
	Urb	= (PURB)ExAllocatePoolWithTag( NonPagedPool, UrbSize, '010T' );
	if( Urb == NULL )
	{
		return( STATUS_INSUFFICIENT_RESOURCES );
	}

	//	�p�C�v�ւ� URB ���쐬����
	Urb->UrbHeader.Length			= (USHORT)sizeof( struct _URB_PIPE_REQUEST );
	Urb->UrbHeader.Function			= Function;
	Urb->UrbPipeRequest.PipeHandle	= PipeHandle;

	//	�p�C�v�ւ� URB �𔭍s����
	Status	= CallUSBDI( NextDeviceObject, IOCTL_INTERNAL_USB_SUBMIT_URB, Urb );
	if( !NT_SUCCESS( Status ) || !USBD_SUCCESS( Urb->UrbHeader.Status ) )
	{
		Status	= STATUS_UNSUCCESSFUL;
	}

	//	�쐬���� URB ���������
	ExFreePool( Urb );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	InputReport
//----------------------------------------------------------------------------------------------
NTSTATUS InputReport(
	 IN PDEVICE_OBJECT		NextDeviceObject
	,IN USBD_PIPE_HANDLE	PipeHandle
	,OUT INPUT_REPORT *		InputReport
	,IN LONG				InputReportSize )
{
	//	�ϐ��錾
	NTSTATUS	Status;
	USHORT		UrbSize	= sizeof( struct _URB_BULK_OR_INTERRUPT_TRANSFER );
	PURB		Urb;

	//	URB �p�o�b�t�@���쐬����
	Urb	= (PURB)ExAllocatePoolWithTag( NonPagedPool, UrbSize, '110T' );
	if( Urb == NULL )
	{
		return( STATUS_INSUFFICIENT_RESOURCES );
	}

	//	���̓��|�[�g���擾����ׂ� URB ���쐬����
	UsbBuildInterruptOrBulkTransferRequest(
		 Urb
		,UrbSize
		,PipeHandle
		,InputReport
		,NULL
		,InputReportSize
		,USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK
		,NULL );

	//	���̓��|�[�g���擾����ׂ� URB �𔭍s����
	Status	= CallUSBDI( NextDeviceObject, IOCTL_INTERNAL_USB_SUBMIT_URB, Urb );
	if( !NT_SUCCESS( Status ) || !USBD_SUCCESS( Urb->UrbHeader.Status ) )
	{
		Status	= STATUS_UNSUCCESSFUL;
	}

	//	�쐬���� URB ���������
	ExFreePool( Urb );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	OutputReport
//----------------------------------------------------------------------------------------------
NTSTATUS OutputReport(
	 IN PDEVICE_OBJECT		NextDeviceObject
	,IN USBD_PIPE_HANDLE	PipeHandle
	,OUT OUTPUT_REPORT *	OutputReport
	,IN LONG				OutputReportSize )
{
	//	�ϐ��錾
	NTSTATUS	Status;
	USHORT		UrbSize	= sizeof( struct _URB_BULK_OR_INTERRUPT_TRANSFER );
	PURB		Urb;

	//	URB �p�o�b�t�@���쐬����
	Urb	= (PURB)ExAllocatePoolWithTag( NonPagedPool, UrbSize, '210T' );
	if( Urb == NULL )
	{
		return( STATUS_INSUFFICIENT_RESOURCES );
	}

	//	�o�̓��|�[�g��ݒ肷��ׂ� URB ���쐬����
	UsbBuildInterruptOrBulkTransferRequest(
		 Urb
		,UrbSize
		,PipeHandle
		,OutputReport
		,NULL
		,OutputReportSize
		,USBD_SHORT_TRANSFER_OK
		,NULL );
	//	�o�̓��|�[�g��ݒ肷��ׂ� URB �𔭍s����
	Status	= CallUSBDI( NextDeviceObject, IOCTL_INTERNAL_USB_SUBMIT_URB, Urb );
	if( !NT_SUCCESS( Status ) || !USBD_SUCCESS( Urb->UrbHeader.Status ) )
	{
		Status	= STATUS_UNSUCCESSFUL;
	}

	//	�쐬���� URB ���������
	ExFreePool( Urb );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	CallUSBDI
//----------------------------------------------------------------------------------------------
NTSTATUS CallUSBDI(
	 IN PDEVICE_OBJECT	NextDeviceObject
	,IN ULONG			IOCTLCode
	,IN OUT PVOID		Arg1 )
{
	//	�ϐ��錾
	NTSTATUS			Status;
	PIRP				Irp;
	KEVENT				WaitEvent;
	IO_STATUS_BLOCK		IoStatus;
	PIO_STACK_LOCATION	NextIrpStack;

	//	I/O �v�����쐬����
	KeInitializeEvent( &WaitEvent, NotificationEvent, FALSE );
	Irp	= IoBuildDeviceIoControlRequest(
			 IOCTLCode
			,NextDeviceObject
			,NULL
			,0
			,NULL
			,0
			,TRUE
			,&WaitEvent
			,&IoStatus );
	NextIrpStack	= IoGetNextIrpStackLocation( Irp );
	NextIrpStack->Parameters.Others.Argument1	= Arg1;

	//	���ʃh���C�o�� I/O �v���𔭍s����
	Status	= IoCallDriver( NextDeviceObject, Irp );
	if( Status == STATUS_PENDING )
	{
		//	���ʃh���C�o�̏�������������܂őҋ@����
		KeWaitForSingleObject(
			 &WaitEvent
			,Executive
			,KernelMode
			,FALSE
			,NULL );
		Status	= IoStatus.Status;
	}

	return( Status );
}


//	HID Minidriver for Microsoft Xbox 360 Wireless Controller

#include	"driver.h"

//----------------------------------------------------------------------------------------------
//	AddDevice
//----------------------------------------------------------------------------------------------
NTSTATUS AddDevice(
	 IN PDRIVER_OBJECT	DriverObject
	,IN PDEVICE_OBJECT	DeviceObject )
{
	//	�ϐ��錾
	PDEVICE_EXTENSION	DeviceExtension	= GET_MINIDRIVER_DEVICE_EXTENSION( DeviceObject );
	UCHAR				ThreadId;

	//	�f�o�C�X �G�N�X�e���V�����̓��e������������
	RtlZeroMemory( DeviceExtension, sizeof( DEVICE_EXTENSION ) );

	//	I/O �v������
	DeviceExtension->DriverState	= Enable;
	DeviceExtension->DeviceState	= Disable;
	DeviceExtension->ReferenceCount	= 1;
	KeInitializeEvent( &DeviceExtension->ReferenceEnd, NotificationEvent, FALSE );

	//	���[�J �X���b�h
	DeviceExtension->ThreadStateIo	= Disable;
	for( ThreadId = 0; ThreadId < ControllerCount; ThreadId ++ )
	{
		KeInitializeEvent(
			 &DeviceExtension->ThreadExitingIn[ThreadId]
			,SynchronizationEvent
			,FALSE );
		KeInitializeEvent(
			 &DeviceExtension->ThreadExitingOut[ThreadId]
			,SynchronizationEvent
			,FALSE );
	}

	//	IRP
	DeviceExtension->ThreadStateIdc	= Disable;
	KeInitializeEvent( &DeviceExtension->ThreadExitingIdc, SynchronizationEvent, FALSE );
	KeInitializeSpinLock( &DeviceExtension->LockIrp );

	//	USB
	DeviceExtension->NextDeviceObject	= GET_NEXT_DEVICE_OBJECT( DeviceObject );

	//	���|�[�g
	KeInitializeSpinLock( &DeviceExtension->LockReport );

	//	�f�o�C�X �G�N�X�e���V�����̏�����������ʒm����
	DeviceObject->Flags	&= ~DO_DEVICE_INITIALIZING;

	return( STATUS_SUCCESS );
}

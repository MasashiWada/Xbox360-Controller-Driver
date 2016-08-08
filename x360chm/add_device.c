
//	HID Minidriver for Microsoft Xbox 360 Controller

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

	//	�f�o�C�X �G�N�X�e���V�����̓��e������������
	RtlZeroMemory( DeviceExtension, sizeof( DEVICE_EXTENSION ) );

	//	I/O �v������
	DeviceExtension->DriverState	= Enable;
	DeviceExtension->DeviceState	= Disable;
	DeviceExtension->ReferenceCount	= 1;
	KeInitializeEvent( &DeviceExtension->ReferenceEnd, NotificationEvent, FALSE );

	//	���[�J �X���b�h
	DeviceExtension->ThreadState	= Disable;
	KeInitializeEvent( &DeviceExtension->ThreadExitingIn, SynchronizationEvent, FALSE );
	KeInitializeEvent( &DeviceExtension->ThreadExitingOut, SynchronizationEvent, FALSE );

	//	USB
	DeviceExtension->NextDeviceObject	= GET_NEXT_DEVICE_OBJECT( DeviceObject );

	//	���|�[�g
	KeInitializeSpinLock( &DeviceExtension->LockReport );

	//	�f�o�C�X �G�N�X�e���V�����̏�����������ʒm����
	DeviceObject->Flags	&= ~DO_DEVICE_INITIALIZING;

	return( STATUS_SUCCESS );
}

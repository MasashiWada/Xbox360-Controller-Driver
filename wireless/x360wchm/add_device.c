
//	HID Minidriver for Microsoft Xbox 360 Wireless Controller

#include	"driver.h"

//----------------------------------------------------------------------------------------------
//	AddDevice
//----------------------------------------------------------------------------------------------
NTSTATUS AddDevice(
	 IN PDRIVER_OBJECT	DriverObject
	,IN PDEVICE_OBJECT	DeviceObject )
{
	//	変数宣言
	PDEVICE_EXTENSION	DeviceExtension	= GET_MINIDRIVER_DEVICE_EXTENSION( DeviceObject );
	UCHAR				ThreadId;

	//	デバイス エクステンションの内容を初期化する
	RtlZeroMemory( DeviceExtension, sizeof( DEVICE_EXTENSION ) );

	//	I/O 要求制御
	DeviceExtension->DriverState	= Enable;
	DeviceExtension->DeviceState	= Disable;
	DeviceExtension->ReferenceCount	= 1;
	KeInitializeEvent( &DeviceExtension->ReferenceEnd, NotificationEvent, FALSE );

	//	ワーカ スレッド
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

	//	レポート
	KeInitializeSpinLock( &DeviceExtension->LockReport );

	//	デバイス エクステンションの初期化完了を通知する
	DeviceObject->Flags	&= ~DO_DEVICE_INITIALIZING;

	return( STATUS_SUCCESS );
}

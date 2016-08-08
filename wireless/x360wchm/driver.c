
//	HID Minidriver for Microsoft Xbox 360 Wireless Controller

#include	"driver.h"

//----------------------------------------------------------------------------------------------
//	グローバル変数
//----------------------------------------------------------------------------------------------

UCHAR	ControllerCount;
UCHAR	LimitationMode;

//----------------------------------------------------------------------------------------------
//	DriverEntry
//----------------------------------------------------------------------------------------------
DRIVER_INITIALIZE DriverEntry;
NTSTATUS DriverEntry(
	 IN PDRIVER_OBJECT	DriverObject
	,IN PUNICODE_STRING	RegistryPath )
{
	//	変数宣言
	NTSTATUS					Status;
	SETTING						Setting;
	HID_MINIDRIVER_REGISTRATION	HIDMiniDriverRegistration;

	//	ドライバ エントリ ポイントを設定する
	DriverObject->DriverExtension->AddDevice					= AddDevice;
	DriverObject->DriverUnload									= DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL]	= InternalDeviceControl;
	DriverObject->MajorFunction[IRP_MJ_PNP]						= PnP;
	DriverObject->MajorFunction[IRP_MJ_POWER]					= Power;

	//	設定を取得する
	LoadSetting( &Setting );
	ControllerCount	= Setting.ControllerCount;
	LimitationMode	= Setting.LimitationMode;

	//	HID Minidriver を登録する
	RtlZeroMemory( &HIDMiniDriverRegistration, sizeof( HIDMiniDriverRegistration ) );
	HIDMiniDriverRegistration.Revision				= HID_REVISION;
	HIDMiniDriverRegistration.DriverObject			= DriverObject;
	HIDMiniDriverRegistration.RegistryPath			= RegistryPath;
	HIDMiniDriverRegistration.DeviceExtensionSize	= sizeof( DEVICE_EXTENSION );
	HIDMiniDriverRegistration.DevicesArePolled		= FALSE;
	Status	= HidRegisterMinidriver( &HIDMiniDriverRegistration );

	return( Status );
}

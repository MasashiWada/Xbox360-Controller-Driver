
//	HID Minidriver for Microsoft Xbox 360 Wireless Controller

#include	"driver.h"

//----------------------------------------------------------------------------------------------
//	構造体
//----------------------------------------------------------------------------------------------

typedef struct _READ_TIMER
{
	KDPC				ReadTimerDpc;
	KTIMER				ReadTimer;
	PIRP				Irp;
	PDEVICE_EXTENSION	DeviceExtension;
} READ_TIMER, *PREAD_TIMER;

//----------------------------------------------------------------------------------------------
//	グローバル変数
//----------------------------------------------------------------------------------------------

CHAR	ReportDescriptorHeader[]	=
{
	0x05, 0x01,			//	USAGE_PAGE (Generic Desktop)
	0x09, 0x05,			//	USAGE (Game Pad)
	0xa1, 0x01,			//	COLLECTION (Application)

	0x85, 0x00			//		REPORT_ID (0)
};

CHAR	ReportDescriptorBodyNormal[]	=
{
	0x05, 0x01,			//		USAGE_PAGE (Generic Desktop)

	0x09, 0x30,			//		USAGE (X)
	0x09, 0x31,			//		USAGE (Y)
	0x09, 0x33,			//		USAGE (Rx)
	0x09, 0x34,			//		USAGE (Ry)
	0x16, 0x00, 0x80,	//		LOGICAL_MINIMUM (-32768)
	0x26, 0xff, 0x7f,	//		LOGICAL_MAXIMUM (32767)
	0x75, 0x10,			//		REPORT_SIZE (16)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x09, 0x32,			//		USAGE (Z)
	0x09, 0x35,			//		USAGE (Rz)
	0x09, 0x36,			//		USAGE (Slider)
	0x09, 0x36,			//		USAGE (Slider)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x05, 0x09,			//		USAGE_PAGE (Button)

	0x19, 0x01,			//		USAGE_MINIMUM (Button 1)
	0x29, 0x20,			//		USAGE_MAXIMUM (Button 32)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x01,			//		LOGICAL_MAXIMUM (1)
	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x20,			//		REPORT_COUNT (32)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x06, 0x00, 0xff,	//		USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x03,			//		REPORT_COUNT (3)
	0x91, 0x02,			//		OUTPUT (Data,Var,Abs)

	0x06, 0x00, 0xff,	//		USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x1D,			//		REPORT_COUNT (29)
	0xb1, 0x00			//		FEATURE (Data,Ary,Abs)
};

CHAR	ReportDescriptorBodyH1B16[]	=
{
	0x05, 0x01,			//		USAGE_PAGE (Generic Desktop)

	0x09, 0x30,			//		USAGE (X)
	0x09, 0x31,			//		USAGE (Y)
	0x09, 0x33,			//		USAGE (Rx)
	0x09, 0x34,			//		USAGE (Ry)
	0x16, 0x00, 0x80,	//		LOGICAL_MINIMUM (-32768)
	0x26, 0xff, 0x7f,	//		LOGICAL_MAXIMUM (32767)
	0x75, 0x10,			//		REPORT_SIZE (16)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x09, 0x32,			//		USAGE (Z)
	0x09, 0x35,			//		USAGE (Rz)
	0x09, 0x36,			//		USAGE (Slider)
	0x09, 0x36,			//		USAGE (Slider)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x1c,			//		REPORT_COUNT (28)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x05, 0x09,			//		USAGE_PAGE (Button)

	0x19, 0x01,			//		USAGE_MINIMUM (Button 1)
	0x29, 0x10,			//		USAGE_MAXIMUM (Button 16)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x01,			//		LOGICAL_MAXIMUM (1)
	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x10,			//		REPORT_COUNT (16)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x10,			//		REPORT_COUNT (16)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x06, 0x00, 0xff,	//		USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x03,			//		REPORT_COUNT (3)
	0x91, 0x02,			//		OUTPUT (Data,Var,Abs)

	0x06, 0x00, 0xff,	//		USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x1D,			//		REPORT_COUNT (29)
	0xb1, 0x00			//		FEATURE (Data,Ary,Abs)
};

CHAR	ReportDescriptorBodyNoRxRy[]	=
{
	0x05, 0x01,			//		USAGE_PAGE (Generic Desktop)

	0x09, 0x30,			//		USAGE (X)
	0x09, 0x31,			//		USAGE (Y)
	0x16, 0x00, 0x80,	//		LOGICAL_MINIMUM (-32768)
	0x26, 0xff, 0x7f,	//		LOGICAL_MAXIMUM (32767)
	0x75, 0x10,			//		REPORT_SIZE (16)
	0x95, 0x02,			//		REPORT_COUNT (2)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x75, 0x10,			//		REPORT_SIZE (16)
	0x95, 0x02,			//		REPORT_COUNT (2)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x32,			//		USAGE (Z)
	0x09, 0x35,			//		USAGE (Rz)
	0x09, 0x36,			//		USAGE (Slider)
	0x09, 0x36,			//		USAGE (Slider)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x05, 0x09,			//		USAGE_PAGE (Button)

	0x19, 0x01,			//		USAGE_MINIMUM (Button 1)
	0x29, 0x20,			//		USAGE_MAXIMUM (Button 32)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x01,			//		LOGICAL_MAXIMUM (1)
	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x20,			//		REPORT_COUNT (32)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x06, 0x00, 0xff,	//		USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x03,			//		REPORT_COUNT (3)
	0x91, 0x02,			//		OUTPUT (Data,Var,Abs)

	0x06, 0x00, 0xff,	//		USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x1D,			//		REPORT_COUNT (29)
	0xb1, 0x00			//		FEATURE (Data,Ary,Abs)
};

CHAR	ReportDescriptorBodyNoSlider[]	=
{
	0x05, 0x01,			//		USAGE_PAGE (Generic Desktop)

	0x09, 0x30,			//		USAGE (X)
	0x09, 0x31,			//		USAGE (Y)
	0x09, 0x33,			//		USAGE (Rx)
	0x09, 0x34,			//		USAGE (Ry)
	0x16, 0x00, 0x80,	//		LOGICAL_MINIMUM (-32768)
	0x26, 0xff, 0x7f,	//		LOGICAL_MAXIMUM (32767)
	0x75, 0x10,			//		REPORT_SIZE (16)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x09, 0x32,			//		USAGE (Z)
	0x09, 0x35,			//		USAGE (Rz)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x02,			//		REPORT_COUNT (2)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x02,			//		REPORT_COUNT (2)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x09, 0x39,			//		USAGE (Hat switch)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x07,			//		LOGICAL_MAXIMUM (7)
	0x75, 0x04,			//		REPORT_SIZE (4)
	0x95, 0x01,			//		REPORT_COUNT (1)
	0x81, 0x42,			//		INPUT (Data,Var,Abs,Null)

	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x04,			//		REPORT_COUNT (4)
	0x81, 0x03,			//		INPUT (Cnst,Var,Abs)

	0x05, 0x09,			//		USAGE_PAGE (Button)

	0x19, 0x01,			//		USAGE_MINIMUM (Button 1)
	0x29, 0x20,			//		USAGE_MAXIMUM (Button 32)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x25, 0x01,			//		LOGICAL_MAXIMUM (1)
	0x75, 0x01,			//		REPORT_SIZE (1)
	0x95, 0x20,			//		REPORT_COUNT (32)
	0x81, 0x02,			//		INPUT (Data,Var,Abs)

	0x06, 0x00, 0xff,	//		USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x03,			//		REPORT_COUNT (3)
	0x91, 0x02,			//		OUTPUT (Data,Var,Abs)

	0x06, 0x00, 0xff,	//		USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,			//		USAGE (Vendor Usage 1)
	0x15, 0x00,			//		LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,	//		LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//		REPORT_SIZE (8)
	0x95, 0x1D,			//		REPORT_COUNT (29)
	0xb1, 0x00			//		FEATURE (Data,Ary,Abs)
};

CHAR	ReportDescriptorFooter[]	=
{
	0xc0				//	END_COLLECTION
};

CHAR *	ReportDescriptorBody[]	=
{
	 ReportDescriptorBodyNormal
	,ReportDescriptorBodyH1B16
	,ReportDescriptorBodyNoRxRy
	,ReportDescriptorBodyNoSlider
};

USHORT	ReportDescriptorBodySize[]	=
{
	 sizeof( ReportDescriptorBodyNormal )
	,sizeof( ReportDescriptorBodyH1B16 )
	,sizeof( ReportDescriptorBodyNoRxRy )
	,sizeof( ReportDescriptorBodyNoSlider )
};

//----------------------------------------------------------------------------------------------
//	関数
//----------------------------------------------------------------------------------------------

NTSTATUS HidGetDeviceAttributes( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS HidGetDeviceDescriptor( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS HidGetReportDescriptor( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS HidReadReport( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS HidWriteReport( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS HidGetFeature( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS HidSetFeature( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );

//----------------------------------------------------------------------------------------------
//	InternalDeviceControl
//----------------------------------------------------------------------------------------------
NTSTATUS InternalDeviceControl(
	 IN PDEVICE_OBJECT	DeviceObject
	,IN PIRP			Irp )
{
	//	変数宣言
	NTSTATUS			Status;
	PDEVICE_EXTENSION	DeviceExtension	= GET_MINIDRIVER_DEVICE_EXTENSION( DeviceObject );
	PIO_STACK_LOCATION	CurrentIrpStack;

	//	参照カウンタを加算する
	Status	= IncrementReference( DeviceExtension );
	if( !NT_SUCCESS( Status ) )
	{
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	マイナー ファンクションによって処理を振り分ける
	CurrentIrpStack	= IoGetCurrentIrpStackLocation( Irp );
	switch( CurrentIrpStack->Parameters.DeviceIoControl.IoControlCode )
	{
		case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
			Status	= HidGetDeviceAttributes( DeviceExtension, Irp, CurrentIrpStack );
			break;

		case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
			Status	= HidGetDeviceDescriptor( DeviceExtension, Irp, CurrentIrpStack );
			break;

		case IOCTL_HID_GET_REPORT_DESCRIPTOR:
			Status	= HidGetReportDescriptor( DeviceExtension, Irp, CurrentIrpStack );
			break;

		case IOCTL_HID_READ_REPORT:
			Status	= HidReadReport( DeviceExtension, Irp, CurrentIrpStack );
			break;

		case IOCTL_HID_WRITE_REPORT:
			Status	= HidWriteReport( DeviceExtension, Irp, CurrentIrpStack );
			break;

		case IOCTL_HID_GET_FEATURE:
			Status	= HidGetFeature( DeviceExtension, Irp, CurrentIrpStack );
			break;

		case IOCTL_HID_SET_FEATURE:
			Status	= HidSetFeature( DeviceExtension, Irp, CurrentIrpStack );
			break;

		default:
			Status	= STATUS_NOT_SUPPORTED;
			CompleteRequest( Irp, Status, 0 );
			break;
	}

	//	参照カウンタを減算する
	DecrementReference( DeviceExtension );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	HidGetDeviceAttributes
//----------------------------------------------------------------------------------------------
NTSTATUS HidGetDeviceAttributes(
	 IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	変数宣言
	NTSTATUS				Status			= STATUS_SUCCESS;
	UCHAR					BufferLength	= sizeof( HID_DEVICE_ATTRIBUTES );
	PHID_DEVICE_ATTRIBUTES	HidDeviceAttributes;

	//	デバイス属性用バッファのサイズをチェックする
	if( CurrentIrpStack->Parameters.DeviceIoControl.OutputBufferLength < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	デバイス属性を設定する
	HidDeviceAttributes					= (PHID_DEVICE_ATTRIBUTES)Irp->UserBuffer;
	RtlZeroMemory( HidDeviceAttributes, BufferLength );
	HidDeviceAttributes->Size			= BufferLength;
	HidDeviceAttributes->VendorID		= DeviceExtension->DeviceDescriptor.idVendor;
	HidDeviceAttributes->ProductID		= DeviceExtension->DeviceDescriptor.idProduct;
	HidDeviceAttributes->VersionNumber	= DeviceExtension->DeviceDescriptor.bcdDevice;

	//	IOCTL_HID_GET_DEVICE_ATTRIBUTES 要求を完了させる
	CompleteRequest( Irp, Status, BufferLength );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	HidGetDeviceDescriptor
//----------------------------------------------------------------------------------------------
NTSTATUS HidGetDeviceDescriptor(
	 IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	変数宣言
	NTSTATUS		Status			= STATUS_SUCCESS;
	UCHAR			BufferLength	= sizeof( HID_DESCRIPTOR );
	PHID_DESCRIPTOR	HidDescriptor;
	USHORT			ReportLength;

	//	HID 記述子用バッファのサイズをチェックする
	if( CurrentIrpStack->Parameters.DeviceIoControl.OutputBufferLength < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	レポート記述子のサイズを計算する
	ReportLength	= sizeof( ReportDescriptorHeader )
						+ ReportDescriptorBodySize[LimitationMode]
						+ sizeof( ReportDescriptorFooter );
	ReportLength	*= ControllerCount;


	//	HID 記述子を設定する
	HidDescriptor									= (PHID_DESCRIPTOR)Irp->UserBuffer;
	RtlZeroMemory( HidDescriptor, BufferLength );
	HidDescriptor->bLength							= BufferLength;
	HidDescriptor->bDescriptorType					= HID_HID_DESCRIPTOR_TYPE;
	HidDescriptor->bcdHID							= HID_REVISION;
	HidDescriptor->bCountry							= 0;
	HidDescriptor->bNumDescriptors					= 1;
	HidDescriptor->DescriptorList[0].bReportType	= HID_REPORT_DESCRIPTOR_TYPE;
	HidDescriptor->DescriptorList[0].wReportLength	= ReportLength;
	//	IOCTL_HID_GET_DEVICE_DESCRIPTOR 要求を完了させる
	CompleteRequest( Irp, Status, BufferLength );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	HidGetReportDescriptor
//----------------------------------------------------------------------------------------------
NTSTATUS HidGetReportDescriptor(
	 IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	変数宣言
	NTSTATUS	Status			= STATUS_SUCCESS;
	USHORT		BufferLength	= sizeof( ReportDescriptorHeader )
									+ ReportDescriptorBodySize[LimitationMode]
									+ sizeof( ReportDescriptorFooter );
	CHAR		Index;
	LONG		BufferIndex		= 0;

	//	レポート記述子のサイズを計算する
	BufferLength	= sizeof( ReportDescriptorHeader )
						+ ReportDescriptorBodySize[LimitationMode]
						+ sizeof( ReportDescriptorFooter );
	BufferLength	*= ControllerCount;

	//	レポート記述子用バッファのサイズをチェックする
	if( CurrentIrpStack->Parameters.DeviceIoControl.OutputBufferLength < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	レポート記述子を設定する
	for( Index = 0; Index < ControllerCount; Index ++ )
	{
		//	ヘッダ
		RtlCopyMemory(
			 (char *)Irp->UserBuffer + BufferIndex
			,ReportDescriptorHeader
			,sizeof( ReportDescriptorHeader ) );
		BufferIndex	+= sizeof( ReportDescriptorHeader );
		((char *)Irp->UserBuffer)[BufferIndex - 1]	= Index + 1;
		//	ボディ
		RtlCopyMemory(
			 (char *)Irp->UserBuffer + BufferIndex
			,ReportDescriptorBody[LimitationMode]
			,ReportDescriptorBodySize[LimitationMode] );
		BufferIndex	+= ReportDescriptorBodySize[LimitationMode];
		//	フッタ
		RtlCopyMemory(
			 (char *)Irp->UserBuffer + BufferIndex
			,ReportDescriptorFooter
			,sizeof( ReportDescriptorFooter ) );
		BufferIndex	+= sizeof( ReportDescriptorFooter );
	}

	//	IOCTL_HID_GET_REPORT_DESCRIPTOR 要求を完了させる
	CompleteRequest( Irp, Status, BufferLength );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	HidReadReport
//----------------------------------------------------------------------------------------------
NTSTATUS HidReadReport(
	 IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	変数宣言
	KIRQL	Irql;
	PIRP *	Irps;

	//	排他制御を行う
	KeAcquireSpinLock( &DeviceExtension->LockIrp, &Irql );

	//	新しい IRP のキューを確保する
	Irps	= (PIRP *)ExAllocatePoolWithTag(
				 NonPagedPool
				,(DeviceExtension->IrpCount + 1) * sizeof( PIRP )
				,'200T' );

	//	既に IRP のキューがあればコピーする
	if( DeviceExtension->IrpCount > 0 )
	{
		RtlCopyMemory(
			 Irps
			,DeviceExtension->Irps
			,(DeviceExtension->IrpCount) * sizeof( PIRP ) );
		if( DeviceExtension->Irps != NULL )
		{
			ExFreePool( DeviceExtension->Irps );
			DeviceExtension->Irps = NULL;
		}
	}

	//	新しいキューに切り替える
	DeviceExtension->Irps	= Irps;

	//	IRP をキューに乗せる
	DeviceExtension->Irps[DeviceExtension->IrpCount]	= Irp;
	DeviceExtension->IrpCount ++;

	//	排他制御を解除する
	KeReleaseSpinLock( &DeviceExtension->LockIrp, Irql );

	return( STATUS_SUCCESS );
}

//----------------------------------------------------------------------------------------------
//	ReadReport
//----------------------------------------------------------------------------------------------
VOID ReadReport(
	 IN PIRP				Irp
	,IN PDEVICE_EXTENSION	DeviceExtension )
{
	//	変数宣言
	NTSTATUS			Status			= STATUS_SUCCESS;
	LARGE_INTEGER		ReadReportInterval;
	UCHAR				ControllerIndex;
	LARGE_INTEGER		TimeOut;
	KIRQL				Irql;
	UCHAR				BufferLength	= sizeof( HID_READ_REPORT );
	PIO_STACK_LOCATION	CurrentIrpStack;

	//	レポート ID を進める
	DeviceExtension->ReadReportId ++;
	if( DeviceExtension->ReadReportId > ControllerCount )
	{
		//	レポートした時刻を退避する
		DeviceExtension->LastReadReportTime	= DeviceExtension->CurrentReadReportTime;
		KeQuerySystemTime( &DeviceExtension->CurrentReadReportTime );
		ReadReportInterval.QuadPart	= DeviceExtension->CurrentReadReportTime.QuadPart
									- DeviceExtension->LastReadReportTime.QuadPart;

		//	前回の入力用レポートを退避する
		for( ControllerIndex = 0; ControllerIndex < ControllerCount; ControllerIndex ++ )
		{
			RtlCopyMemory(
				 &DeviceExtension->LastControllerReport[ControllerIndex]
				,&DeviceExtension->CurrentControllerReport[ControllerIndex]
				,sizeof( INPUT_REPORT ) );
			DeviceExtension->LastControllerReportTime[ControllerIndex]
				= DeviceExtension->CurrentControllerReportTime[ControllerIndex];
		}

		// タイマー精度が高く、レポートの間隔が 1.5ms を下回れば待機する
		if( ReadReportInterval.QuadPart <= 1500 * 10 )
		{
			TimeOut.QuadPart	= -1500 * 10;
			KeDelayExecutionThread( KernelMode, FALSE, &TimeOut );
		}

		//	現在の入力用レポートを退避する
		for( ControllerIndex = 0; ControllerIndex < ControllerCount; ControllerIndex ++ )
		{
			RtlCopyMemory(
				 &DeviceExtension->CurrentControllerReport[ControllerIndex]
				,&DeviceExtension->ControllerReport[ControllerIndex]
				,sizeof( INPUT_REPORT ) );
			DeviceExtension->CurrentControllerReportTime[ControllerIndex]
				= DeviceExtension->ControllerReportTime[ControllerIndex];
		}

		//	レポート ID を最初に戻す
		DeviceExtension->ReadReportId	= 1;
	}

	//	入力レポート用バッファのサイズをチェックする
	CurrentIrpStack	= IoGetCurrentIrpStackLocation( Irp );
	if( CurrentIrpStack->Parameters.DeviceIoControl.OutputBufferLength < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
	} else {

		//	ワーカー スレッドからの割り込みを禁止する
		KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

		//	入力レポートを設定する
		RtlZeroMemory( Irp->UserBuffer, BufferLength );
		GenerateReadReport(
			 &DeviceExtension->ControllerReport[DeviceExtension->ReadReportId - 1]
			,&DeviceExtension->Setting
			,(HID_READ_REPORT *)Irp->UserBuffer
			,&DeviceExtension->LastState[DeviceExtension->ReadReportId - 1]
			,DeviceExtension->CurrentReadReportTime );

		//	レポート ID を設定する
		((UCHAR *)Irp->UserBuffer)[0]	= DeviceExtension->ReadReportId;

		//	ワーカー スレッドからの割り込みを許可する
		KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );

		//	IOCTL_HID_READ_REPORT 要求を完了させる
		CompleteRequest( Irp, Status, BufferLength );
	}
}

//----------------------------------------------------------------------------------------------
//	HidWriteReport
//----------------------------------------------------------------------------------------------
NTSTATUS HidWriteReport(
	 IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	変数宣言
	NTSTATUS			Status			= STATUS_SUCCESS;
	UCHAR				BufferLength	= sizeof( HID_WRITE_REPORT );
	PHID_XFER_PACKET	Report			= (PHID_XFER_PACKET)Irp->UserBuffer;
	UCHAR				ReportId;

	//	出力レポート用バッファのサイズをチェックする
	if( Report->reportBufferLen < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	レポート ID を調整する
	ReportId	= Report->reportId;
	ReportId	= min( max( 1, Report->reportId ), MAX_CONTROLLER_COUNT );

	//	出力レポートを設定する
	GenerateWriteReport(
		 (HID_WRITE_REPORT *)Report->reportBuffer
		,&DeviceExtension->Setting
		,&DeviceExtension->ActuatorsReport[ReportId - 1] );

	//	IOCTL_HID_WRITE_REPORT 要求を完了させる
	CompleteRequest( Irp, Status, BufferLength );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	HidGetFeature
//----------------------------------------------------------------------------------------------
NTSTATUS HidGetFeature(
	 IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	変数宣言
	NTSTATUS			Status			= STATUS_SUCCESS;
	UCHAR				BufferLength	= sizeof( INPUT_REPORT );
	PHID_XFER_PACKET	Feature			= (PHID_XFER_PACKET)Irp->UserBuffer;
	UCHAR				ReportId;

	//	フューチャー用バッファのサイズをチェックする
	if( Feature->reportBufferLen < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	レポート ID を調整する
	ReportId	= Feature->reportId;
	ReportId	= min( max( 1, Feature->reportId ), MAX_CONTROLLER_COUNT );

	//	コントローラーの状態を返す
	RtlCopyMemory(
		 Feature->reportBuffer + 1
		,&DeviceExtension->ControllerReport[ReportId - 1]
		,BufferLength );

	//	IOCTL_HID_GET_FEATURE 要求を完了させる
	CompleteRequest( Irp, Status, BufferLength );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	HidSetFeature
//----------------------------------------------------------------------------------------------
NTSTATUS HidSetFeature(
	 IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	変数宣言
	NTSTATUS	Status	= STATUS_SUCCESS;
	UCHAR		ThreadId;

	//	コントローラーの設定を読み直す
	for( ThreadId = 0; ThreadId < ControllerCount; ThreadId ++ )
	{
		DeviceExtension->ResetDevice[ThreadId]	= TRUE;
	}

	//	IOCTL_HID_SET_FEATURE 要求を完了させる
	CompleteRequest( Irp, Status, 0 );

	return( Status );
}

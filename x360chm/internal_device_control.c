
//	HID Minidriver for Microsoft Xbox 360 Controller

#include	"driver.h"

//----------------------------------------------------------------------------------------------
//	�\����
//----------------------------------------------------------------------------------------------

typedef struct _READ_TIMER
{
	KDPC				ReadTimerDpc;
	KTIMER				ReadTimer;
	PIRP				Irp;
	PDEVICE_EXTENSION	DeviceExtension;
} READ_TIMER, *PREAD_TIMER;

//----------------------------------------------------------------------------------------------
//	�O���[�o���ϐ�
//----------------------------------------------------------------------------------------------

CHAR	ReportDescriptorNormal[]	=
{
	0x05, 0x01,			//	USAGE_PAGE (Generic Desktop)
	0x09, 0x05,			//	USAGE (Game Pad)
	0xa1, 0x01,			//	COLLECTION (Application)

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
	0x95, 0x14,			//		REPORT_COUNT (20)
	0xb1, 0x00,			//		FEATURE (Data,Ary,Abs)

	0xc0				//	END_COLLECTION
};

CHAR	ReportDescriptorH1B16[]	=
{
	0x05, 0x01,			//	USAGE_PAGE (Generic Desktop)
	0x09, 0x05,			//	USAGE (Game Pad)
	0xa1, 0x01,			//	COLLECTION (Application)

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
	0x95, 0x14,			//		REPORT_COUNT (20)
	0xb1, 0x00,			//		FEATURE (Data,Ary,Abs)

	0xc0				//	END_COLLECTION
};

CHAR	ReportDescriptorNoRxRy[]	=
{
	0x05, 0x01,			//	USAGE_PAGE (Generic Desktop)
	0x09, 0x05,			//	USAGE (Game Pad)
	0xa1, 0x01,			//	COLLECTION (Application)

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
	0x95, 0x14,			//		REPORT_COUNT (20)
	0xb1, 0x00,			//		FEATURE (Data,Ary,Abs)

	0xc0				//	END_COLLECTION
};

CHAR	ReportDescriptorNoSlider[]	=
{
	0x05, 0x01,			//	USAGE_PAGE (Generic Desktop)
	0x09, 0x05,			//	USAGE (Game Pad)
	0xa1, 0x01,			//	COLLECTION (Application)

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
	0x95, 0x14,			//		REPORT_COUNT (20)
	0xb1, 0x00,			//		FEATURE (Data,Ary,Abs)

	0xc0				//	END_COLLECTION
};

CHAR *	ReportDescriptor[]	= {
	 ReportDescriptorNormal
	,ReportDescriptorH1B16
	,ReportDescriptorNoRxRy
	,ReportDescriptorNoSlider
};

UCHAR	ReportDescriptorSize[]	= {
	 sizeof( ReportDescriptorNormal )
	,sizeof( ReportDescriptorH1B16 )
	,sizeof( ReportDescriptorNoRxRy )
	,sizeof( ReportDescriptorNoSlider )
};

//----------------------------------------------------------------------------------------------
//	�֐�
//----------------------------------------------------------------------------------------------

NTSTATUS HidGetDeviceAttributes( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS HidGetDeviceDescriptor( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS HidGetReportDescriptor( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS HidReadReport( IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );
KDEFERRED_ROUTINE ReadReportDpcRoutine;
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

	//	�Q�ƃJ�E���^�����Z����
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
	//	�ϐ��錾
	NTSTATUS				Status			= STATUS_SUCCESS;
	UCHAR					BufferLength	= sizeof( HID_DEVICE_ATTRIBUTES );
	PHID_DEVICE_ATTRIBUTES	HidDeviceAttributes;

	//	�f�o�C�X�����p�o�b�t�@�̃T�C�Y���`�F�b�N����
	if( CurrentIrpStack->Parameters.DeviceIoControl.OutputBufferLength < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	�f�o�C�X������ݒ肷��
	HidDeviceAttributes					= (PHID_DEVICE_ATTRIBUTES)Irp->UserBuffer;
	RtlZeroMemory( HidDeviceAttributes, BufferLength );
	HidDeviceAttributes->Size			= BufferLength;
	HidDeviceAttributes->VendorID		= DeviceExtension->DeviceDescriptor.idVendor;
	HidDeviceAttributes->ProductID		= DeviceExtension->DeviceDescriptor.idProduct;
	HidDeviceAttributes->VersionNumber	= DeviceExtension->DeviceDescriptor.bcdDevice;

	//	IOCTL_HID_GET_DEVICE_ATTRIBUTES �v��������������
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
	//	�ϐ��錾
	NTSTATUS		Status			= STATUS_SUCCESS;
	UCHAR			BufferLength	= sizeof( HID_DESCRIPTOR );
	PHID_DESCRIPTOR	HidDescriptor;

	//	HID �L�q�q�p�o�b�t�@�̃T�C�Y���`�F�b�N����
	if( CurrentIrpStack->Parameters.DeviceIoControl.OutputBufferLength < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	HID �L�q�q��ݒ肷��
	HidDescriptor									= (PHID_DESCRIPTOR)Irp->UserBuffer;
	RtlZeroMemory( HidDescriptor, BufferLength );
	HidDescriptor->bLength							= BufferLength;
	HidDescriptor->bDescriptorType					= HID_HID_DESCRIPTOR_TYPE;
	HidDescriptor->bcdHID							= HID_REVISION;
	HidDescriptor->bCountry							= 0;
	HidDescriptor->bNumDescriptors					= 1;
	HidDescriptor->DescriptorList[0].bReportType	= HID_REPORT_DESCRIPTOR_TYPE;
	HidDescriptor->DescriptorList[0].wReportLength	= ReportDescriptorSize[LimitationMode];

	//	IOCTL_HID_GET_DEVICE_DESCRIPTOR �v��������������
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
	//	�ϐ��錾
	NTSTATUS	Status			= STATUS_SUCCESS;
	UCHAR		BufferLength	= ReportDescriptorSize[LimitationMode];

	//	���|�[�g�L�q�q�p�o�b�t�@�̃T�C�Y���`�F�b�N����
	if( CurrentIrpStack->Parameters.DeviceIoControl.OutputBufferLength < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	���|�[�g�L�q�q��ݒ肷��
	RtlCopyMemory( Irp->UserBuffer, ReportDescriptor[LimitationMode], BufferLength );

	//	IOCTL_HID_GET_REPORT_DESCRIPTOR �v��������������
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
	//	�ϐ��錾
	NTSTATUS		Status			= STATUS_SUCCESS;
	UCHAR			BufferLength	= sizeof( HID_READ_REPORT );
	KIRQL			Irql;
	LARGE_INTEGER	TimeOut;
	PREAD_TIMER		ReadTimer;

	if( DevicesArePolled == FALSE )
	{
		//	�^�C�}�[�p�o�b�t�@���쐬����
		ReadTimer	= (PREAD_TIMER)ExAllocatePoolWithTag(
						 NonPagedPool
						,sizeof( READ_TIMER )
						,'200T' );
		RtlZeroMemory( ReadTimer, sizeof( READ_TIMER ) );
		ReadTimer->Irp				= Irp;
		ReadTimer->DeviceExtension	= DeviceExtension;
		//	�^�C�}�[������������
		KeInitializeDpc(
			 &ReadTimer->ReadTimerDpc
			,ReadReportDpcRoutine
			,(PVOID)ReadTimer );
		KeInitializeTimer( &ReadTimer->ReadTimer );
		//	�^�C�}�[�𔭍s����
		TimeOut.QuadPart	= -1;
		KeSetTimer(
			 &ReadTimer->ReadTimer
			,TimeOut
			,&ReadTimer->ReadTimerDpc );
	} else {
		//	���̓��|�[�g�p�o�b�t�@�̃T�C�Y���`�F�b�N����
		if( CurrentIrpStack->Parameters.DeviceIoControl.OutputBufferLength < BufferLength )
		{
			Status	= STATUS_BUFFER_TOO_SMALL;
			CompleteRequest( Irp, Status, 0 );
			return( Status );
		}

		//	���[�J�[ �X���b�h����̊��荞�݂��֎~����
		KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

		//	���̓��|�[�g��ݒ肷��
		RtlZeroMemory( Irp->UserBuffer, BufferLength );
		GenerateReadReport(
			 &DeviceExtension->ControllerReport
			,&DeviceExtension->Setting
			,(HID_READ_REPORT *)Irp->UserBuffer
			,&DeviceExtension->LastState );

		//	���[�J�[ �X���b�h����̊��荞�݂�������
		KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );

		//	IOCTL_HID_READ_REPORT �v��������������
		CompleteRequest( Irp, Status, BufferLength );
	}

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	ReadReportDpcRoutine
//----------------------------------------------------------------------------------------------
VOID ReadReportDpcRoutine(
	 IN PKDPC	Dpc
	,IN PVOID	DeferredContext
	,IN PVOID	SystemArgument1
	,IN PVOID	SystemArgument2 )
{
	//	�ϐ��錾
	NTSTATUS			Status			= STATUS_SUCCESS;
	PREAD_TIMER			ReadTimer		= (PREAD_TIMER)DeferredContext;
	PIRP				Irp				= ReadTimer->Irp;
	PDEVICE_EXTENSION	DeviceExtension	= ReadTimer->DeviceExtension;
	PIO_STACK_LOCATION	CurrentIrpStack;
	KIRQL				Irql;
	UCHAR				BufferLength	= sizeof( HID_READ_REPORT );

	//	���̓��|�[�g�p�o�b�t�@�̃T�C�Y���`�F�b�N����
	CurrentIrpStack	= IoGetCurrentIrpStackLocation( Irp );
	if( CurrentIrpStack->Parameters.DeviceIoControl.OutputBufferLength < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
	} else {

		//	���[�J�[ �X���b�h����̊��荞�݂��֎~����
		KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

		//	���̓��|�[�g��ݒ肷��
		RtlZeroMemory( Irp->UserBuffer, BufferLength );
		GenerateReadReport(
			 &DeviceExtension->ControllerReport
			,&DeviceExtension->Setting
			,(HID_READ_REPORT *)Irp->UserBuffer
			,&DeviceExtension->LastState );

		//	���[�J�[ �X���b�h����̊��荞�݂�������
		KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );

		//	IOCTL_HID_READ_REPORT �v��������������
		CompleteRequest( Irp, Status, BufferLength );
	}

	//	�^�C�}�[�p�o�b�t�@���J������
	ExFreePool( ReadTimer );
}

//----------------------------------------------------------------------------------------------
//	HidWriteReport
//----------------------------------------------------------------------------------------------
NTSTATUS HidWriteReport(
	 IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	�ϐ��錾
	NTSTATUS			Status			= STATUS_SUCCESS;
	UCHAR				BufferLength	= sizeof( HID_WRITE_REPORT );
	PHID_XFER_PACKET	Report			= (PHID_XFER_PACKET)Irp->UserBuffer;

	//	�o�̓��|�[�g�p�o�b�t�@�̃T�C�Y���`�F�b�N����
	if( Report->reportBufferLen < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	�o�̓��|�[�g��ݒ肷��
	GenerateWriteReport(
		 (HID_WRITE_REPORT *)Report->reportBuffer
		,&DeviceExtension->Setting
		,&DeviceExtension->ActuatorsReport );

	//	IOCTL_HID_WRITE_REPORT �v��������������
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
	//	�ϐ��錾
	NTSTATUS			Status			= STATUS_SUCCESS;
	UCHAR				BufferLength	= sizeof( INPUT_REPORT );
	PHID_XFER_PACKET	Feature			= (PHID_XFER_PACKET)Irp->UserBuffer;

	//	�t���[�`���[�p�o�b�t�@�̃T�C�Y���`�F�b�N����
	if( Feature->reportBufferLen < BufferLength )
	{
		Status	= STATUS_BUFFER_TOO_SMALL;
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	�R���g���[���[�̏�Ԃ�Ԃ�
	RtlCopyMemory(
		 Feature->reportBuffer + 1
		,&DeviceExtension->ControllerReport
		,BufferLength );

	//	IOCTL_HID_GET_FEATURE �v��������������
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
	//	�ϐ��錾
	NTSTATUS	Status	= STATUS_SUCCESS;

	//	�R���g���[���[�̐ݒ��ǂݒ���
	DeviceExtension->ResetDevice	= TRUE;

	//	IOCTL_HID_SET_FEATURE �v��������������
	CompleteRequest( Irp, Status, 0 );

	return( Status );
}

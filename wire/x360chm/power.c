
//	HID Minidriver for Microsoft Xbox 360 Controller

#include	"driver.h"

//----------------------------------------------------------------------------------------------
//	�֐�
//----------------------------------------------------------------------------------------------

NTSTATUS SetPower( IN PDEVICE_OBJECT, IN PDEVICE_EXTENSION, IN PIRP, IN PIO_STACK_LOCATION );

//----------------------------------------------------------------------------------------------
//	Power
//----------------------------------------------------------------------------------------------
NTSTATUS Power(
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
		case IRP_MN_SET_POWER:
			Status = SetPower( DeviceObject, DeviceExtension, Irp, CurrentIrpStack );
			break;

		default:
			//	IRP_MJ_POWER �v�������ʃh���C�o�ɑ���
			Status	= SendPowerRequest( DeviceObject, Irp );
			break;
	}

	//	�Q�ƃJ�E���^�����Z����
	DecrementReference( DeviceExtension );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	SetPower
//----------------------------------------------------------------------------------------------
NTSTATUS SetPower(
	 IN PDEVICE_OBJECT		DeviceObject
	,IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	�ϐ��錾
	NTSTATUS	Status;

	//	�f�o�C�X�̓d����Ԃ��ύX���ꂽ�ꍇ�Ƀf�o�C�X�̏�Ԃ�ύX����
#if (NTDDI_VERSION < NTDDI_LONGHORN)
	switch( CurrentIrpStack->Parameters.Power.Type )
	{
		case SystemPowerState:
			if( CurrentIrpStack->Parameters.Power.State.SystemState == PowerSystemWorking )
			{
				//	�f�o�C�X�����Z�b�g����
				DeviceExtension->ResetDevice	= TRUE;
			}
		break;
	}
	//	IRP_MJ_POWER �v�������ʃh���C�o�ɑ���
	Status	= SendPowerRequest( DeviceObject, Irp );
#else
	if( CurrentIrpStack->Parameters.Power.Type == DevicePowerState )
	{
		switch( CurrentIrpStack->Parameters.Power.State.DeviceState )
		{
			case PowerDeviceD0:
				//	��� IRP_MN_SET_POWER �v�������ʃh���C�o�ɑ���
				Status = SendAndWaitPowerRequest( DeviceObject, Irp );
				if( !NT_SUCCESS( Status ) )
				{
					CompleteRequest( Irp, Status, 0 );
					return( Status );
				}

				//	�f�o�C�X��L���ɂ���
				Status	= EnableDevice( DeviceExtension );

				//	IRP_MN_SET_POWER �v��������������
				CompleteRequest( Irp, Status, 0 );
				break;

			case PowerDeviceD1:
			case PowerDeviceD2:
			case PowerDeviceD3:
				//	�f�o�C�X�𖳌��ɂ���
				DisableDevice( DeviceExtension );

				//	IRP_MJ_POWER �v�������ʃh���C�o�ɑ���
				Status	= SendPowerRequest( DeviceObject, Irp );
				break;

			default:
				//	IRP_MJ_POWER �v�������ʃh���C�o�ɑ���
				Status	= SendPowerRequest( DeviceObject, Irp );
				break;
		}
	} else {
		//	IRP_MJ_POWER �v�������ʃh���C�o�ɑ���
		Status	= SendPowerRequest( DeviceObject, Irp );
	}
#endif

	return( Status );
}

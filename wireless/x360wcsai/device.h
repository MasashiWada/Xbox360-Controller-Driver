
//	Status Area Icon for Microsoft Xbox 360 Wireless Controller

#pragma	once

#include	<windows.h>
#include	<stdio.h>
#include	<setupapi.h>

extern "C"
{
#include	<hidsdi.h>
}

//----------------------------------------------------------------------------------------------
//	�}�N����`
//----------------------------------------------------------------------------------------------

#define	ADD_UCHAR( D0, D1 )( (CHAR)max( 0, min( 255, (LONG)( D0 ) + (LONG)( D1 ) ) ) )
#define	ADD_SHORT( D0, D1 )( (SHORT)max( -32768, min( 32767, (LONG)( D0 ) + (LONG)( D1 ) ) ) )

//----------------------------------------------------------------------------------------------
//	�萔
//----------------------------------------------------------------------------------------------

#define	REGISTRY_KEY				L"SYSTEM\\CurrentControlSet\\Services\\Xbox360WirelessController\\Parameters"

#define	SUPPORTED_DEVICE_COUNT		L"Supported Device Count"
#define	SUPPORTED_DEVICE_ATTRIBUTE	L"Supported Device Attribute %d"

#define	MAX_CONTROLLER_COUNT		4

#define	REPORT_TYPE_NORMAL			0
#define	REPORT_TYPE_DIRECT			1

//----------------------------------------------------------------------------------------------
//	�\����
//----------------------------------------------------------------------------------------------

#pragma pack(1)

//	HID �����p���|�[�g
typedef struct _HID_WRITE_REPORT_ID
{
	UCHAR	Id;
	UCHAR	Type;
	UCHAR	Left;
	UCHAR	Right;
} HID_WRITE_REPORT_ID, *PHID_WRITE_REPORT_ID;

//	�t���[�`���[�p���|�[�g
typedef struct _FEATURE_REPORT_ID
{
	UCHAR	Id;
	UCHAR	Type;
	UCHAR	ParamType;
	UCHAR	Zero1;
	UCHAR	Zero2;
	UCHAR	Zero3;
	UCHAR	Param1;
	USHORT	DigitalButton;
	UCHAR	LeftTrigger;
	UCHAR	RightTrigger;
	SHORT	LeftStickX;
	SHORT	LeftStickY;
	SHORT	RightStickX;
	SHORT	RightStickY;
	UCHAR	Zero4;
	UCHAR	Zero5;
	UCHAR	Zero6;
	UCHAR	Zero7;
	UCHAR	Zero8;
	UCHAR	Zero9;
	UCHAR	Zero10;
	UCHAR	Zero11;
	UCHAR	Zero12;
	UCHAR	Zero13;
	UCHAR	Zero14;
} FEATURE_REPORT_ID, *PFEATURE_REPORT_ID;

#pragma pack()

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CDevice
{

public:

	//	�R���X�g���N�^
	CDevice();
	//	�f�X�g���N�^
	~CDevice();

	//	�f�o�C�X���ڑ�����Ă��邩�ǂ�����Ԃ�
	BOOL IsConnected();
	//	�f�o�C�X�̏�Ԃ��擾����
	BOOL GetState( FEATURE_REPORT_ID * );
	//	�f�o�C�X�̏�Ԃ�ݒ肷��
	BOOL SetState( HID_WRITE_REPORT_ID * );
	//	�f�o�C�X�ɕύX��ʒm����
	BOOL Apply();

private:

	//	���W�X�g������l���擾����
	VOID GetRegistry( VOID *, ULONG, ULONG, WCHAR * );
	//	�f�o�C�X�ɃA�N�Z�X����
	BOOL IoDevice( LONG, HID_WRITE_REPORT_ID *, FEATURE_REPORT_ID * );
	//	�R���g���[���� ID ���擾����
	CHAR GetControllerId( WCHAR * );

	//	�f�o�C�X�ւ̃A�N�Z�X���@
	static const LONG IO_MODE_IS_CONNECTED	= 1;
	static const LONG IO_MODE_GET_STATE		= 2;
	static const LONG IO_MODE_SET_STATE		= 3;
	static const LONG IO_MODE_APPLY			= 4;

	//	�T�|�[�g���Ă���f�o�C�X�̐�
	LONG	SupportedDeviceCount;
	//	�T�|�[�g���Ă���f�o�C�X�̑���
	ULONG *	SupportedDeviceAttribute;

protected:

};

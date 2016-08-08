
//	Force Feedback Driver for Microsoft Xbox 360 Wireless Controller

#pragma	once

#ifndef	DIRECTINPUT_VERSION
#define	DIRECTINPUT_VERSION	0x0800
#endif

#include	<windows.h>
#include	<dinput.h>
#include	<math.h>

//----------------------------------------------------------------------------------------------
//	�萔
//----------------------------------------------------------------------------------------------

#define	CONSTANT_FORCE	0x00
#define	RAMP_FORCE		0x01
#define	SQUARE			0x02
#define	SINE			0x03
#define	TRIANGLE		0x04
#define	SAWTOOTH_UP		0x05
#define	SAWTOOTH_DOWN	0x06
#define	SPRING			0x07
#define	DAMPER			0x08
#define	INERTIA			0x09
#define	FRICTION		0x0A
#define	CUSTOM_FORCE	0x0B

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CEffect
{

public:

	//	�R���X�g���N�^
	CEffect();

	//	�G�t�F�N�g���v�Z����
	VOID Calc( LONG *, LONG * );

	//	���
	DWORD			Type;
	//	DIEFFECT �\����
	DIEFFECT		DiEffect;
	//	DIENVELOPE �\����
	DIENVELOPE		DiEnvelope;
	//	DICONSTANTFORCE �\����
	DICONSTANTFORCE	DiConstantForce;
	//	DIPERIODIC �\����
	DIPERIODIC		DiPeriodic;
	//	DIRAMPFORCE �\����
	DIRAMPFORCE		DiRampforce;

	//	�n���h��
	DWORD			Handle;

	//	���
	DWORD			Status;
	//	�Đ���
	DWORD			PlayCount;
	//	�Đ�����
	DWORD			StartTime;

private:

	//	�G���x���[�v���v�Z����
	VOID CalcEnvelope( ULONG, ULONG, LONG *, LONG *, LONG * );
	//	�t�H�[�X���v�Z����
	VOID CalcForce( ULONG, ULONG, LONG, LONG, LONG, LONG * );

protected:

};

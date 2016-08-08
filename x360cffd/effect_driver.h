
//	Force Feedback Driver for Microsoft Xbox 360 Controller

#pragma	once

#ifndef	DIRECTINPUT_VERSION
#define	DIRECTINPUT_VERSION	0x0800
#endif

#include	<windows.h>
#include	<objbase.h>
#include	<initguid.h>
#include	<dinput.h>
#include	<dinputd.h>

#include	"extension.h"
#include	"effect.h"

//----------------------------------------------------------------------------------------------
//	�萔
//----------------------------------------------------------------------------------------------

#define	REPORT_TYPE_NORMAL	0
#define	REPORT_TYPE_DIRECT	1

//----------------------------------------------------------------------------------------------
//	�\����
//----------------------------------------------------------------------------------------------

typedef struct _HID_WRITE_REPORT_ID
{
	UCHAR	Id;
	UCHAR	Type;
	UCHAR	Left;
	UCHAR	Right;
} HID_WRITE_REPORT_ID, *PHID_WRITE_REPORT_ID;

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CEffectDriver : public IDirectInputEffectDriver
{

public:

	//	�R���X�g���N�^
	CEffectDriver( VOID );

	//	IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, PVOID * );
	ULONG STDMETHODCALLTYPE AddRef( VOID );
	ULONG STDMETHODCALLTYPE Release( VOID );

	//	IDirectInputEffectDriver
	HRESULT STDMETHODCALLTYPE DeviceID( DWORD, DWORD, DWORD, DWORD, LPVOID );
	HRESULT STDMETHODCALLTYPE GetVersions( LPDIDRIVERVERSIONS );
	HRESULT STDMETHODCALLTYPE Escape( DWORD, DWORD, LPDIEFFESCAPE );
	HRESULT STDMETHODCALLTYPE SetGain( DWORD, DWORD );
	HRESULT STDMETHODCALLTYPE SendForceFeedbackCommand( DWORD, DWORD );
	HRESULT STDMETHODCALLTYPE GetForceFeedbackState( DWORD, LPDIDEVICESTATE );
	HRESULT STDMETHODCALLTYPE DownloadEffect( DWORD, DWORD, LPDWORD, LPCDIEFFECT, DWORD );
	HRESULT STDMETHODCALLTYPE DestroyEffect( DWORD, DWORD );
	HRESULT STDMETHODCALLTYPE StartEffect( DWORD, DWORD, DWORD, DWORD );
	HRESULT STDMETHODCALLTYPE StopEffect( DWORD, DWORD );
	HRESULT STDMETHODCALLTYPE GetEffectStatus( DWORD, DWORD, LPDWORD );

	//	�f�o�C�X �C���^�[�t�F�C�X
	HANDLE		Device;

	//	�G�t�F�N�g��
	LONG		EffectCount;
	//	�G�t�F�N�g ���X�g
	CEffect * *	EffectList;

	//	�Q�C��
	DWORD		Gain;
	//	�A�N�`���G�[�^
	BOOL		Actuator;

	//	�I���V�O�i��
	BOOL		Quit;

private:

	//	�Q�ƃJ�E���^
	LONG	ReferenceCount;

	//	���[�J �X���b�h
	HANDLE	WorkerThread;

	//	�G�t�F�N�g�ԍ�
	LONG	EffectIndex;

	//	��~
	BOOL	Stopped;
	//	�ꎞ��~
	BOOL	Paused;
	//	�ꎞ��~��������
	LONG	PausedTime;

protected:

};

//----------------------------------------------------------------------------------------------
//	�֐�
//----------------------------------------------------------------------------------------------

STDAPI_(DWORD) WINAPI EffectProc( LPVOID );



//	Property Sheet for Microsoft Xbox 360 Wireless Controller

#pragma	once

#include	"page.h"
#include	"device.h"
#include	"report.h"

//----------------------------------------------------------------------------------------------
//	�萔
//----------------------------------------------------------------------------------------------

#define	X360WCPS_MUTEX				L"x360wcps mutex"

#define	X360WCSAI_CLASS_NAME		L"x360wcsai class"
#define	X360WCSAI_WINDOW_NAME		L"x360wcsai window"

#define	WM_STATUS_AREA_ICON_NOTIFY	WM_USER + 1

#define	STATUS_AREA_ICON_ID			0
#define	UPDATE_SETTING				1

#define	POLLING_TIMER_ID			1
#define POLLING_INTERVAL			16

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CControllerPage : public CPage
{

public:

	//	�R���X�g���N�^
	CControllerPage( CSetting * );

	//	�_�C�A���O������������
	VOID wmInitDialog( HWND );
	//	�R�}���h����������
	VOID wmCommand( HWND, WORD, WORD );
	//	�ʒm����������
	VOID wmNotify( NMHDR * );
	//	�^�C�}�[����������
	VOID wmTimer( WPARAM );
	//	�y�[�W�ύX�̒ʒm����������
	VOID wmPageChanged();

	//	�ݒ�𔽉f����
	VOID ApplySetting();

	//	�ݒ�̏��Ԃ�ύX����
	VOID ChangeSetting();
	//	���݂̐ݒ��ۑ�����
	VOID SaveSetting();
	//	���݂̐ݒ���폜����
	VOID DeleteSetting();

	//	�ݒ�̕ύX�𔽉f����
	VOID ApplyChangedSetting();

	//	�y�[�W�����
	VOID ClosePage();

	//	�R���g���[���[�̏�Ԃ�\������
	VOID ShowControllerState();
	//	�U�����e�X�g����
	VOID TestForceFeedback( LONG );

private:

	//	�~���[�e�b�N�X
	HANDLE		Mutex;

	//	�f�o�C�X�ɓK�p����Ă���Z�b�e�B���O
	SETTING		DeviceSetting;

	//	�f�o�C�X
	CDevice *	Device;
	//	���|�[�g
	CReport *	Report;

	//	�R���g���[���[�̍Ō�̏��
	LAST_STATE	LastState;

protected:

};

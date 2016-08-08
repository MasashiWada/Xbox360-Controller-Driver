
//	Status Area Icon for Microsoft Xbox 360 Controller

#pragma	once

#include	<windows.h>
#include	<shellapi.h>

#include	"setting.h"
#include	"property_sheet.h"
#include	"device.h"

//----------------------------------------------------------------------------------------------
//	�萔
//----------------------------------------------------------------------------------------------

#define	WM_STATUS_AREA_ICON_NOTIFY	WM_USER + 1

#define	STATUS_AREA_ICON_ID			0
#define	UPDATE_SETTING				1

#define	LEFT_CLICK_TIMER			1
#define	AUTO_SETTING_CHANGE_TIMER	2

#define CHECK_INTERVAL				1000

#define	TH32CS_SNAPPROCESS			0x00000002

//----------------------------------------------------------------------------------------------
//	�\����
//----------------------------------------------------------------------------------------------

typedef struct tagPROCESSENTRY32
{
	DWORD		dwSize;
	DWORD		cntUsage;
	DWORD		th32ProcessID;
	ULONG_PTR	th32DefaultHeapID;
	DWORD		th32ModuleID;
	DWORD		cntThreads;
	DWORD		th32ParentProcessID;
	LONG		pcPriClassBase;
	DWORD		dwFlags;
	WCHAR		szExeFile[MAX_PATH];
} PROCESSENTRY32, *PPROCESSENTRY32;

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CStatusAreaIcon
{

public:

	//	�R���X�g���N�^
	CStatusAreaIcon( HINSTANCE, HWND );
	//	�f�X�g���N�^
	~CStatusAreaIcon();

	//	�X�e�[�^�X �G���A �A�C�R���̃C�x���g����������
	VOID wmNotify( WPARAM, LPARAM );
	//	�R�}���h����������
	VOID wmCommand( WORD );
	//	�^�C�}�[����������
	VOID wmTimer( WPARAM );
	//	�f�o�C�X�̕ύX����������
	VOID wmDeviceChange();
	//	�^�X�N �o�[�̍č쐬����������
	VOID wmTaskbarCtreated();

	//	�C���X�^���X
	HINSTANCE	Instance;
	//	�E�B���h�E �n���h��
	HWND		Wnd;

	//	�v���Z�X ID
	DWORD		ProcessID;

private:

	//	�X�e�[�^�X �G���A�ɃA�C�R����\������
	VOID ShowIcon();
	//	�X�e�[�^�X �G���A����A�C�R�����폜����
	VOID HideIcon();

	//	�|�b�v�A�b�v ���j���[�i���j��\������
	VOID ShowLeftPopupMenu();
	//	�|�b�v�A�b�v ���j���[�i�E�j��\������
	VOID ShowRightPopupMenu();

	//	�ݒ�̎����؊������s��
	VOID AutomaticChange( DWORD );

	//	�ݒ�
	CSetting *			Setting;
	//	�v���p�e�B �V�[�g
	CPropertySheet *	PropertySheet;
	//	�f�o�C�X
	CDevice *			Device;

protected:

};

//----------------------------------------------------------------------------------------------
//	�֐�
//----------------------------------------------------------------------------------------------

extern "C"
{
HANDLE WINAPI CreateToolhelp32Snapshot( DWORD, DWORD );
BOOL WINAPI Process32FirstW( HANDLE, PPROCESSENTRY32 );
BOOL WINAPI Process32NextW( HANDLE, PPROCESSENTRY32 );
}


//	Property Sheet for Microsoft Xbox 360 Controller

#pragma	once

#include	<windows.h>
#include	<commctrl.h>

#include	"setting.h"

//----------------------------------------------------------------------------------------------
//	�萔
//----------------------------------------------------------------------------------------------

#define	WM_PAGE_CHANGED				WM_USER + 1
#define	WM_SETTING_LIST_CHANGED		WM_USER + 2

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CPage
{

public:

	//	�R���X�g���N�^
	CPage();

	//	�_�C�A���O������������
	virtual VOID wmInitDialog( HWND );
	//	�R�}���h����������
	virtual VOID wmCommand( HWND, WORD, WORD );
	//	�ʒm����������
	virtual VOID wmNotify( NMHDR * );
	//	�^�C�}�[����������
	virtual VOID wmTimer( WPARAM );
	//	�����X�N���[������������
	virtual VOID wmHScroll( HWND, WORD );
	//	�y�[�W�ύX�̒ʒm����������
	virtual VOID wmPageChanged();
	//	�ݒ�ύX�̒ʒm����������
	virtual VOID wmSettingListChanged();

	//	�ݒ�𔽉f����
	virtual VOID ApplySetting();

private:

protected:

	//	�ݒ�̕ύX��ʒm����
	VOID SettingChanged();

	//	�R���{ �{�b�N�X������������
	VOID InitComboBox( WORD, WORD *, WORD );
	//	�R���{ �{�b�N�X�����Ԃ�ݒ肷��
	VOID SetComboBoxIndex( WORD, UCHAR );
	//	�R���{ �{�b�N�X�����Ԃ��擾����
	UCHAR GetComboBoxIndex( WORD );

	//	���X�g �{�b�N�X������������
	VOID InitListBox( WORD, WORD *, WORD );
	//	���X�g �{�b�N�X�̏��Ԃ�ݒ肷��
	VOID SetListBoxIndex( WORD, LONG );
	//	���X�g �{�b�N�X�̏��Ԃ��擾����
	LONG GetListBoxIndex( WORD );

	//	�`�F�b�N �{�b�N�X�̏�Ԃ�ݒ肷��
	VOID SetCheckBoxState( WORD, BOOLEAN );
	//	�`�F�b�N �{�b�N�X�̏�Ԃ��擾����
	BOOLEAN GetCheckBoxState( WORD );

	//	�g���b�N �o�[������������
	VOID InitTrackBar( WORD, UCHAR, UCHAR, UCHAR, UCHAR );
	//	�g���b�N �o�[�̈ʒu��ݒ肷��
	VOID SetTrackBarPos( WORD, UCHAR );
	//	�g���b�N �o�[�̈ʒu���擾����
	UCHAR GetTrackBarPos( WORD );
	//	�g���b�N �o�[�͈̔͂�ݒ肷��
	VOID SetTrackBarRange( WORD, UCHAR, UCHAR );
	//	�g���b�N �o�[�͈̔͂��擾����
	VOID GetTrackBarRange( WORD, UCHAR *, UCHAR * );
	//	�g���b�N �o�[�̑I�����s��
	VOID SelectTrackBar( HWND, WORD, BOOL * );

	//	���b�Z�[�W �{�b�N�X��\������
	LONG ShowMessageBox( ULONG, ULONG, WCHAR *, ULONG );

	//	�E�B���h�E������ɐ��l��ݒ肷��
	VOID SetIntegerText( WORD, LONG );
	//	�E�B���h�E������Ƀ{�^���ԍ���ݒ肷��
	VOID SetButtonNumberText( WORD, ULONG );

	//	�R���g���[���̏��Ԃ�������ʂ̋@�\���擾����
	UCHAR GetWayUsageByControlIndex( WORD );
	//	�����ʂ̋@�\����R���g���[���̏��Ԃ��擾����
	UCHAR GetControlIndexByWayUsage( UCHAR );

	//	�R���g���[���̗L���A������؂�ւ���
	VOID EnableControl( WORD, BOOL );
	//	�͈͓��̃R���g���[���̗L���A������؂�ւ���
	VOID EnableControls( WORD, WORD, BOOL );

	//	�E�B���h�E �n���h��
	HWND		Wnd;
	//	�ݒ�
	CSetting *	Setting;

	//	���݂̃R���g���[���[�̐ݒ�
	SETTING *	CurrentSetting;

};

//----------------------------------------------------------------------------------------------
//	�֐�
//----------------------------------------------------------------------------------------------

INT_PTR CALLBACK PageProc( HWND, UINT, WPARAM, LPARAM );


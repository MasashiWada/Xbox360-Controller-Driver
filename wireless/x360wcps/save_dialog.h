
//	Property Sheet for Microsoft Xbox 360 Wireless Controller

#pragma	once

#include	<windows.h>

#include	"setting.h"

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CSaveDialog
{

public:

	//	�R���X�g���N�^
	CSaveDialog( HWND, CSetting * );

	//	�R�}���h����������
	VOID wmCommand( HWND, HWND, WORD, WORD );

private:

	//	�ݒ��ۑ�����
	BOOL Save();

	//	���b�Z�[�W �{�b�N�X��\������
	LONG ShowMessageBox( HWND, ULONG, ULONG, WCHAR *, ULONG );

	//	�E�B���h�E �n���h��
	HWND		Wnd;
	//	�ݒ�
	CSetting *	Setting;

protected:

};

//----------------------------------------------------------------------------------------------
//	�֐�
//----------------------------------------------------------------------------------------------

INT_PTR CALLBACK SaveDialogProc( HWND, UINT, WPARAM, LPARAM );


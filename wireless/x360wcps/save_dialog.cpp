
//	Property Sheet for Microsoft Xbox 360 Wireless Controller

#include	"save_dialog.h"
#include	"extension.h"
#include	"resource.h"

//----------------------------------------------------------------------------------------------
//	CSaveDialog
//----------------------------------------------------------------------------------------------
CSaveDialog::CSaveDialog(
	  HWND			Wnd
	 ,CSetting *	Setting )
{
	//	�E�B���h�E �n���h��
	this->Wnd		= Wnd;
	//	�ݒ�
	this->Setting	= Setting;

	//	�����l�Ƃ��Č��݂̐ݒ薼�̂�ݒ肷��
	SetWindowText(
		 GetDlgItem( Wnd, IDC_SETTING_NAME )
		,Setting->SettingName[Setting->CurrentSettingIndex] );
}

//----------------------------------------------------------------------------------------------
//	wmCommand
//----------------------------------------------------------------------------------------------
VOID CSaveDialog::wmCommand(
	 HWND	Wnd
	,HWND	Control
	,WORD	ControlID
	,WORD	NotifyMessage )
{
	//	�R���g���[���ɂ���ď�����U�蕪����
	switch( ControlID )
	{
		case IDC_SETTING_NAME:
			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case EN_CHANGE:
					//	�ݒ�̖��̂���̏ꍇ�� OK �{�^���𖳌��ɂ���
					WCHAR	SettingName[MAX_PATH];
					GetWindowText( Control, SettingName, MAX_PATH );
					if( wcslen( SettingName ) > 0 )
					{
						EnableWindow( GetDlgItem( Wnd, IDOK ), TRUE );
					} else {
						EnableWindow( GetDlgItem( Wnd, IDOK ), FALSE );
					}
					break;
			}
			break;

		case IDOK:
			//	�ݒ��ۑ�����
			if( Save() == TRUE )
			{
				EndDialog( Wnd, IDOK );
				delete this;
			}
			break;

		case IDCANCEL:
			//	�_�C�A���O�����
			EndDialog( Wnd, IDCANCEL );
			delete this;
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	Save
//----------------------------------------------------------------------------------------------
BOOL CSaveDialog::Save()
{
	//	���͂��ꂽ�ݒ薼�̂��擾����
	WCHAR	SettingName[MAX_PATH];
	GetWindowText( GetDlgItem( Wnd, IDC_SETTING_NAME ), SettingName, MAX_PATH );

	//	�u�W���̐ݒ�v�Əd�����邩�`�F�b�N����
	if( wcscmp( SettingName, Setting->SettingName[0] ) == 0 )
	{
		ShowMessageBox(
			 Wnd
			,IDS_WARNING_TITLE
			,IDS_WARNING_MESSAGE
			,SettingName
			,MB_OK | MB_ICONINFORMATION );
		return( FALSE );
	}

	//	���̐ݒ薼�̂Əd�����邩�`�F�b�N����
	LONG	DuplicateSettingIndex	= 0;
	for( LONG Index = 1; Index < Setting->SettingCount; Index ++ )
	{
		if( wcscmp( SettingName, Setting->SettingName[Index] ) == 0 )
		{
			DuplicateSettingIndex	= Index;
			break;
		}
	}
	if( DuplicateSettingIndex == 0 )
	{
		//	�d�����Ȃ���΁A���͂��ꂽ�ݒ薼�̂ŕۑ�����
		Setting->SaveAs( SettingName );
	} else {
		//	�d�����Ă���΁A�_�C�A���O��\�����ď㏑�����m�F����
		LONG	Result	= ShowMessageBox(
							 Wnd
							,IDS_REPLACE_TITLE
							,IDS_REPLACE_MESSAGE
							,SettingName
							,MB_YESNO | MB_ICONQUESTION );
		if( Result == IDYES )
		{
			Setting->Save( DuplicateSettingIndex );
		} else {
			return( FALSE );
		}
	}

	return( TRUE );
}

//----------------------------------------------------------------------------------------------
//	ShowMessageBox
//----------------------------------------------------------------------------------------------
LONG CSaveDialog::ShowMessageBox(
	 HWND		Wnd
	,ULONG		TitleID
	,ULONG		MessageID
	,WCHAR *	Message
	,ULONG		Type )
{
	//	���b�Z�[�W��ǂݍ���
	WCHAR	Title[MAX_PATH];
	LoadString( Instance, TitleID, Title, MAX_PATH );
	WCHAR	ResourceMessage[MAX_PATH];
	LoadString( Instance, MessageID, ResourceMessage, MAX_PATH );
	WCHAR	FormattedMessage[MAX_PATH];
	swprintf_s( FormattedMessage, ResourceMessage, Message );

	//	���b�Z�[�W �{�b�N�X��\������
	return( MessageBox( Wnd, FormattedMessage, Title, Type ) );
}

//----------------------------------------------------------------------------------------------
//	SaveDialogProc
//----------------------------------------------------------------------------------------------
INT_PTR CALLBACK SaveDialogProc(
	 HWND	Wnd
	,UINT	Message
	,WPARAM	wParam
	,LPARAM	lParam )
{
	//	�ϐ��錾
			INT_PTR			Result	= FALSE;
	static	CSaveDialog *	SaveDialog;

	//	���b�Z�[�W�ɂ���ď�����U�蕪����
	switch( Message )
	{
		case WM_INITDIALOG:
			//	�_�C�A���O���쐬����
			SaveDialog	= new CSaveDialog( Wnd, (CSetting *)lParam );
			break;

		case WM_COMMAND:
			//	�R�}���h����������
			SaveDialog->wmCommand( Wnd, (HWND)lParam, LOWORD( wParam ), HIWORD( wParam ) );
			break;
	}

	return( Result );
}

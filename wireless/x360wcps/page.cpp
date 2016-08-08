
//	Property Sheet for Microsoft Xbox 360 Wireless Controller

#include	"page.h"
#include	"extension.h"

//----------------------------------------------------------------------------------------------
//	CPage
//----------------------------------------------------------------------------------------------
CPage::CPage()
{
	//	�E�B���h�E �n���h��
	Wnd				= NULL;

	//	�ݒ�
	Setting			= NULL;

	//	���݂̃R���g���[���[�̐ݒ�
	CurrentSetting	= NULL;
}

//----------------------------------------------------------------------------------------------
//	wmInitDialog
//----------------------------------------------------------------------------------------------
VOID CPage::wmInitDialog( HWND )
{
	;
}

//----------------------------------------------------------------------------------------------
//	wmCommand
//----------------------------------------------------------------------------------------------
VOID CPage::wmCommand( HWND, WORD, WORD )
{
	;
}

//----------------------------------------------------------------------------------------------
//	wmNotify
//----------------------------------------------------------------------------------------------
VOID CPage::wmNotify( NMHDR * )
{
	;
}

//----------------------------------------------------------------------------------------------
//	wmTimer
//----------------------------------------------------------------------------------------------
VOID CPage::wmTimer( WPARAM )
{
	;
}

//----------------------------------------------------------------------------------------------
//	wmHScroll
//----------------------------------------------------------------------------------------------
VOID CPage::wmHScroll( HWND, WORD )
{
	;
}

//----------------------------------------------------------------------------------------------
//	wmPageChanged
//----------------------------------------------------------------------------------------------
VOID CPage::wmPageChanged()
{
	;
}

//----------------------------------------------------------------------------------------------
//	wmSettingListChanged
//----------------------------------------------------------------------------------------------
VOID CPage::wmSettingListChanged()
{
	//	�ݒ�𔽉f����
	ApplySetting();
}

//----------------------------------------------------------------------------------------------
//	ApplySetting
//----------------------------------------------------------------------------------------------
VOID CPage::ApplySetting()
{
	;
}

//----------------------------------------------------------------------------------------------
//	SettingChanged
//----------------------------------------------------------------------------------------------
VOID CPage::SettingChanged()
{
	//	�ϐ��錾
	HWND	Dialog	= GetParent( Wnd );
	HWND	Page	= PropSheet_IndexToHwnd( Dialog, 0 );

	//	�ݒ胊�X�g�̓��e���X�V���A�ύX�ς݂̐ݒ��I������
	Setting->ModifiedFlag	= TRUE;
	SendMessage(
		 Page
		,WM_PAGE_CHANGED
		,NULL
		,NULL );
}

//----------------------------------------------------------------------------------------------
//	InitComboBox
//----------------------------------------------------------------------------------------------
VOID CPage::InitComboBox(
	 WORD	ComboBoxId
	,WORD *	ResourceId
	,WORD	ResourceCount )
{
	//	�R���{ �{�b�N�X������������
	HWND	ComboBox	= GetDlgItem( Wnd, ComboBoxId );
	for( WORD Index = 0; Index < ResourceCount; Index ++ )
	{
		WCHAR	FunctionName[MAX_PATH];
		LoadString( Instance, ResourceId[Index], FunctionName, MAX_PATH );
		SendMessage( ComboBox, CB_INSERTSTRING, Index, (LPARAM)FunctionName );
	}
}

//----------------------------------------------------------------------------------------------
//	SetComboBoxIndex
//----------------------------------------------------------------------------------------------
VOID CPage::SetComboBoxIndex(
	 WORD	ComboBoxId
	,UCHAR	Index )
{
	//	�R���{ �{�b�N�X�����Ԃ�ݒ肷��
	HWND	ComboBox	= GetDlgItem( Wnd, ComboBoxId );
	if( SendMessage( ComboBox, CB_GETCURSEL, 0, 0 ) != Index )
	{
		SendMessage( ComboBox, CB_SETCURSEL, Index, 0 );
	}
}

//----------------------------------------------------------------------------------------------
//	GetComboBoxIndex
//----------------------------------------------------------------------------------------------
UCHAR CPage::GetComboBoxIndex( WORD ComboBoxId )
{
	//	�R���{ �{�b�N�X�����Ԃ��擾����
	return( (UCHAR)SendMessage( GetDlgItem( Wnd, ComboBoxId ), CB_GETCURSEL, 0, 0 ) );
}

//----------------------------------------------------------------------------------------------
//	InitListBox
//----------------------------------------------------------------------------------------------
VOID CPage::InitListBox(
	 WORD	ListBoxId
	,WORD *	ResourceId
	,WORD	ResourceCount )
{
	//	���X�g �{�b�N�X������������
	HWND	ListBox	= GetDlgItem( Wnd, ListBoxId );
	for( WORD Index = 0; Index < ResourceCount; Index ++ )
	{
		WCHAR	FunctionName[MAX_PATH];
		LoadString( Instance, ResourceId[Index], FunctionName, MAX_PATH );
		SendMessage( ListBox, LB_ADDSTRING, Index, (LPARAM)FunctionName );
	}
}

//----------------------------------------------------------------------------------------------
//	SetListBoxIndex
//----------------------------------------------------------------------------------------------
VOID CPage::SetListBoxIndex(
	 WORD	ListBoxId
	,LONG	Index )
{
	//	���X�g �{�b�N�X�̏��Ԃ�ݒ肷��
	HWND	ListBox	= GetDlgItem( Wnd, ListBoxId );
	if( SendMessage( ListBox, LB_GETCURSEL, 0, 0 ) != Index )
	{
		SendMessage( ListBox, LB_SETCURSEL, Index, 0 );
	}
}

//----------------------------------------------------------------------------------------------
//	GetListBoxIndex
//----------------------------------------------------------------------------------------------
LONG CPage::GetListBoxIndex( WORD ListBoxId )
{
	//	���X�g �{�b�N�X�̏��Ԃ��擾����
	return( (LONG)SendMessage( GetDlgItem( Wnd, ListBoxId ), LB_GETCURSEL, 0, 0 ) );
}

//----------------------------------------------------------------------------------------------
//	SetCheckBoxState
//----------------------------------------------------------------------------------------------
VOID CPage::SetCheckBoxState(
	 WORD		CheckBoxId
	,BOOLEAN	State )
{
	//	�`�F�b�N �{�b�N�X�̏�Ԃ�ݒ肷��
	HWND	CheckBox	= GetDlgItem( Wnd, CheckBoxId );
	if( SendMessage( CheckBox, BM_GETCHECK, 0, 0 ) != State )
	{
		SendMessage( CheckBox, BM_SETCHECK, State, 0 );
	}
}

//----------------------------------------------------------------------------------------------
//	GetCheckBoxState
//----------------------------------------------------------------------------------------------
BOOLEAN CPage::GetCheckBoxState( WORD CheckBoxId )
{
	//	�`�F�b�N �{�b�N�X�̏�Ԃ��擾����
	return( (BOOLEAN)SendMessage( GetDlgItem( Wnd, CheckBoxId ), BM_GETCHECK, 0, 0 ) );
}

//----------------------------------------------------------------------------------------------
//	InitTrackBar
//----------------------------------------------------------------------------------------------
VOID CPage::InitTrackBar(
	 WORD	TrackBarId
	,UCHAR	Min
	,UCHAR	Max
	,UCHAR	TicFreq
	,UCHAR	PageSize )
{
	//	�g���b�N �o�[������������
	HWND	TrackBar	= GetDlgItem( Wnd, TrackBarId );
	SendMessage( TrackBar, TBM_SETRANGE, FALSE, MAKELONG( Min, Max ) );
	SendMessage( TrackBar, TBM_SETTICFREQ, TicFreq, 0 );
	SendMessage( TrackBar, TBM_SETPAGESIZE, 0, PageSize );
}

//----------------------------------------------------------------------------------------------
//	SetTrackBarPos
//----------------------------------------------------------------------------------------------
VOID CPage::SetTrackBarPos(
	 WORD	TrackBarId
	,UCHAR	Pos )
{
	//	�g���b�N �o�[�̈ʒu��ݒ肷��
	HWND	TrackBar	= GetDlgItem( Wnd, TrackBarId );
	if( SendMessage( TrackBar, TBM_GETPOS, 0, 0 ) != Pos )
	{
		SendMessage( TrackBar, TBM_SETPOS, TRUE, Pos );
	}
}

//----------------------------------------------------------------------------------------------
//	GetTrackBarPos
//----------------------------------------------------------------------------------------------
UCHAR CPage::GetTrackBarPos( WORD TrackBarId )
{
	//	�g���b�N �o�[�̈ʒu���擾����
	return( (UCHAR)SendMessage( GetDlgItem( Wnd, TrackBarId ), TBM_GETPOS, 0, 0 ) );
}

//----------------------------------------------------------------------------------------------
//	SetTrackBarRange
//----------------------------------------------------------------------------------------------
VOID CPage::SetTrackBarRange(
	 WORD	TrackBarId
	,UCHAR	StartPos
	,UCHAR	EndPos )
{
	//	�g���b�N �o�[�͈̔͂�ݒ肷��
	HWND	TrackBar	= GetDlgItem( Wnd, TrackBarId );
	if( SendMessage( TrackBar, TBM_GETSELSTART, 0, 0 ) != StartPos
	||	SendMessage( TrackBar, TBM_GETSELEND, 0, 0 ) != EndPos )
	{
		SendMessage( TrackBar, TBM_SETSELSTART, TRUE, StartPos );
		SendMessage( TrackBar, TBM_SETSELEND, TRUE, EndPos );
	}
}

//----------------------------------------------------------------------------------------------
//	GetTrackBarRange
//----------------------------------------------------------------------------------------------
VOID CPage::GetTrackBarRange(
	 WORD	TrackBarId
	,UCHAR *	StartPos
	,UCHAR *	EndPos )
{
	//	�g���b�N �o�[�͈̔͂��擾����
	HWND	TrackBar	= GetDlgItem( Wnd, TrackBarId );
	*StartPos	= (UCHAR)SendMessage( TrackBar, TBM_GETSELSTART, 0, 0 );
	*EndPos		= (UCHAR)SendMessage( TrackBar, TBM_GETSELEND, 0, 0 );
}

//----------------------------------------------------------------------------------------------
//	SelectTrackBar
//----------------------------------------------------------------------------------------------
VOID CPage::SelectTrackBar(
	 HWND	TrackBar
	,WORD	Request
	,BOOL *	Selecting )
{
	//	�ϐ��錾
	LONG	CurrentPos;

	//	�g���b�N �o�[�̌��݈ʒu���擾����
	CurrentPos	= (LONG)SendMessage( TrackBar, TBM_GETPOS, 0, 0 );

	//	���N�G�X�g�ɂ���ď�����U�蕪����
	switch( Request )
	{
		case TB_ENDTRACK:
			if( *Selecting == TRUE )
			{
				//	�͈͂̑I�����I������
				SendMessage( TrackBar, TBM_SETSELEND, TRUE, (LPARAM)CurrentPos );
				*Selecting	= FALSE;
			}
			break;

		default:
			if( GetAsyncKeyState(VK_SHIFT) & 0x80000000 )
			{
				if( *Selecting == FALSE )
				{
					//	�͈͂̑I�����J�n����
					SendMessage( TrackBar, TBM_CLEARSEL, TRUE, 0 );
					SendMessage( TrackBar, TBM_SETSELSTART, TRUE, (LPARAM)CurrentPos );
					*Selecting	= TRUE;
				} else {
					//	�͈͂�I��
					SendMessage( TrackBar, TBM_SETSELEND, TRUE, (LPARAM)CurrentPos );
				}
			} else {
				if( *Selecting == TRUE )
				{
					//	�͈͂�I�𒆂� Shift �L�[�𗣂���
					SendMessage( TrackBar, TBM_SETSELEND, TRUE, (LPARAM)CurrentPos );
					*Selecting	= FALSE;
				}
			}
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	ShowMessageBox
//----------------------------------------------------------------------------------------------
LONG CPage::ShowMessageBox(
	 ULONG		TitleId
	,ULONG		MessageId
	,WCHAR *	Message
	,ULONG		Type )
{
	//	���b�Z�[�W��ǂݍ���
	WCHAR	Title[MAX_PATH];
	LoadString( Instance, TitleId, Title, MAX_PATH );
	WCHAR	ResourceMessage[MAX_PATH];
	LoadString( Instance, MessageId, ResourceMessage, MAX_PATH );
	WCHAR	FormattedMessage[MAX_PATH];
	swprintf_s( FormattedMessage, ResourceMessage, Message );

	//	���b�Z�[�W �{�b�N�X��\������
	return( MessageBox( Wnd, FormattedMessage, Title, Type ) );
}

//----------------------------------------------------------------------------------------------
//	SetIntegerText
//----------------------------------------------------------------------------------------------
VOID CPage::SetIntegerText(
	 WORD	TextId
	,LONG	Value )
{
	//	�ϐ��錾
	HWND	Text	= GetDlgItem( Wnd, TextId );
	WCHAR	OldText[MAX_PATH];
	WCHAR	NewText[MAX_PATH];

	//	�E�B���h�E�̕�����𐶐�����
	swprintf_s( NewText, L"%d", Value );

	//	�E�B���h�E�̕����񂪈قȂ�Βu��������
	GetWindowText( Text, OldText, MAX_PATH );
	if( wcscmp( OldText, NewText ) != 0 )
	{
		SetWindowText( Text, NewText );
	}
}

//----------------------------------------------------------------------------------------------
//	SetButtonNumberText
//----------------------------------------------------------------------------------------------
VOID CPage::SetButtonNumberText(
	 WORD	TextId
	,ULONG	Value )
{
	//	�ϐ��錾
	HWND	Text	= GetDlgItem( Wnd, TextId );
	WCHAR	OldText[MAX_PATH];
	WCHAR	NewText[MAX_PATH];
	LONG	Index;

	//	�E�B���h�E�̕�����𐶐�����
	ZeroMemory( NewText, sizeof( NewText ) );
	for( Index = 0; Index < 32; Index ++ )
	{
		if( Value & ( 1 << Index ) )
		{
			WCHAR	WorkText[8];
			swprintf_s( WorkText, L" %02d", Index + 1 );
			wcscat_s( NewText, WorkText );
		}
	}

	//	�E�B���h�E�̕����񂪈قȂ�Βu��������
	GetWindowText( Text, OldText, MAX_PATH );
	if( wcscmp( OldText, NewText ) != 0 )
	{
		SetWindowText( Text, NewText );
	}
}

//----------------------------------------------------------------------------------------------
//	GetWayUsageByControlIndex
//----------------------------------------------------------------------------------------------
UCHAR CPage::GetWayUsageByControlIndex( WORD ComboBoxId )
{
	//	�����ʂ̗p�r
	UCHAR	WayUsage[]	= {	 USAGE_NONE
							,USAGE_X_AXIS_PLUS
							,USAGE_X_AXIS_MINUS
							,USAGE_Y_AXIS_PLUS
							,USAGE_Y_AXIS_MINUS
							,USAGE_X_ROTATION_PLUS
							,USAGE_X_ROTATION_MINUS
							,USAGE_Y_ROTATION_PLUS
							,USAGE_Y_ROTATION_MINUS
							,USAGE_Z_AXIS_PLUS
							,USAGE_Z_AXIS_MINUS
							,USAGE_Z_ROTATION_PLUS
							,USAGE_Z_ROTATION_MINUS
							,USAGE_SLIDER1_PLUS
							,USAGE_SLIDER1_MINUS
							,USAGE_SLIDER2_PLUS
							,USAGE_SLIDER2_MINUS
							,USAGE_BUTTON_1
							,USAGE_BUTTON_2
							,USAGE_BUTTON_3
							,USAGE_BUTTON_4
							,USAGE_BUTTON_5
							,USAGE_BUTTON_6
							,USAGE_BUTTON_7
							,USAGE_BUTTON_8
							,USAGE_BUTTON_9
							,USAGE_BUTTON_10
							,USAGE_BUTTON_11
							,USAGE_BUTTON_12
							,USAGE_BUTTON_13
							,USAGE_BUTTON_14
							,USAGE_BUTTON_15
							,USAGE_BUTTON_16
							,USAGE_BUTTON_17
							,USAGE_BUTTON_18
							,USAGE_BUTTON_19
							,USAGE_BUTTON_20
							,USAGE_BUTTON_21
							,USAGE_BUTTON_22
							,USAGE_BUTTON_23
							,USAGE_BUTTON_24
							,USAGE_BUTTON_25
							,USAGE_BUTTON_26
							,USAGE_BUTTON_27
							,USAGE_BUTTON_28
							,USAGE_BUTTON_29
							,USAGE_BUTTON_30
							,USAGE_BUTTON_31
							,USAGE_BUTTON_32 };

	//	�����ʂ̋@�\��Ԃ�
	return WayUsage[GetComboBoxIndex( ComboBoxId )];
}

//----------------------------------------------------------------------------------------------
//	GetControlIndexByWayUsage
//----------------------------------------------------------------------------------------------
UCHAR CPage::GetControlIndexByWayUsage( UCHAR WayUsage )
{
	//	�R���g���[���̏���
	UCHAR	ControlIndex[]	= {	 USAGE_NONE
								,USAGE_X_AXIS_PLUS
								,USAGE_X_AXIS_MINUS
								,USAGE_Y_AXIS_PLUS
								,USAGE_Y_AXIS_MINUS
								,USAGE_X_ROTATION_PLUS
								,USAGE_X_ROTATION_MINUS
								,USAGE_Y_ROTATION_PLUS
								,USAGE_Y_ROTATION_MINUS
								,USAGE_Z_AXIS_PLUS
								,USAGE_Z_AXIS_MINUS
								,USAGE_Z_ROTATION_PLUS
								,USAGE_Z_ROTATION_MINUS
								,USAGE_SLIDER1_PLUS
								,USAGE_SLIDER1_MINUS
								,USAGE_SLIDER2_PLUS
								,USAGE_SLIDER2_MINUS
								,USAGE_BUTTON_1
								,USAGE_BUTTON_2
								,USAGE_BUTTON_3
								,USAGE_BUTTON_4
								,USAGE_BUTTON_5
								,USAGE_BUTTON_6
								,USAGE_BUTTON_7
								,USAGE_BUTTON_8
								,USAGE_BUTTON_9
								,USAGE_BUTTON_10
								,USAGE_BUTTON_11
								,USAGE_BUTTON_12
								,USAGE_BUTTON_13
								,USAGE_BUTTON_14
								,USAGE_BUTTON_15
								,USAGE_BUTTON_16
								,USAGE_BUTTON_17
								,USAGE_BUTTON_18
								,USAGE_BUTTON_19
								,USAGE_BUTTON_20
								,USAGE_BUTTON_21
								,USAGE_BUTTON_22
								,USAGE_BUTTON_23
								,USAGE_BUTTON_24
								,USAGE_BUTTON_25
								,USAGE_BUTTON_26
								,USAGE_BUTTON_27
								,USAGE_BUTTON_28
								,USAGE_BUTTON_29
								,USAGE_BUTTON_30
								,USAGE_BUTTON_31
								,USAGE_BUTTON_32 };

	//	�R���g���[���̏��Ԃ�Ԃ�
	for( UCHAR Index = 0; Index < sizeof( ControlIndex ); Index ++ )
	{
		if( ControlIndex[Index] == WayUsage )
		{
			return Index;
		}
	}

	return USAGE_NONE;
}

//----------------------------------------------------------------------------------------------
//	EnableControl
//----------------------------------------------------------------------------------------------
VOID CPage::EnableControl(
	 WORD	ControlId
	,BOOL	State )
{
	//	�R���g���[���̗L���A������؂�ւ���
	HWND	Control	= GetDlgItem( Wnd, ControlId );
	if( IsWindowEnabled( Control ) == 0 )
	{
		if( State == TRUE )
		{
			EnableWindow( Control, TRUE );
		}
	} else {
		if( State == FALSE )
		{
			EnableWindow( Control, FALSE );
		}
	}
}

//----------------------------------------------------------------------------------------------
//	EnableControls
//----------------------------------------------------------------------------------------------
VOID CPage::EnableControls(
	 WORD	StartId
	,WORD	EndId
	,BOOL	State )
{
	//	�͈͓��̃R���g���[���̗L���A������؂�ւ���
	for( WORD Index = StartId; Index <= EndId; Index ++ )
	{
		EnableControl( Index, State );
	}
}

//----------------------------------------------------------------------------------------------
//	PageProc
//----------------------------------------------------------------------------------------------
INT_PTR CALLBACK PageProc(
	 HWND	Wnd
	,UINT	Message
	,WPARAM	wParam
	,LPARAM	lParam )
{
	//	�ϐ��錾
			INT_PTR		Result	= FALSE;
	static	LONG		PageCount;
	static	CPage * *	Page;
	static	HWND *		PageWnd;

	//	���b�Z�[�W�ɂ���ď�����U�蕪����
	switch( Message )
	{
		case WM_INITDIALOG:
			//	�y�[�W �N���X���擾����
			PageCount	++;
			if( Page == NULL )
			{
				Page	= (CPage * *)LocalAlloc( LPTR, PageCount * sizeof( CPage * ) );
				PageWnd	= (HWND *)LocalAlloc( LPTR, PageCount * sizeof( HWND ) );
			} else {
				Page	= (CPage * *)LocalReAlloc(
							 Page
							,PageCount * sizeof( CPage * )
							,LMEM_MOVEABLE );
				PageWnd	= (HWND *)LocalReAlloc(
							 PageWnd
							,PageCount * sizeof( HWND )
							,LMEM_MOVEABLE );
			}
			Page[PageCount - 1]		= (CPage *)((PROPSHEETPAGE *)lParam)->lParam;
			PageWnd[PageCount - 1]	= Wnd;
			//	�_�C�A���O������������
			Page[PageCount - 1]->wmInitDialog( Wnd );
			//	�ݒ�𔽉f����
			Page[PageCount - 1]->ApplySetting();
			break;

		case WM_COMMAND:
			//	�R�}���h����������
			for( LONG Index = 0; Index < PageCount; Index ++ )
			{
				if( Wnd == PageWnd[Index] )
				{
					Page[Index]->wmCommand( (HWND)lParam, LOWORD( wParam ), HIWORD( wParam ) );
				}
			}
			break;

		case WM_HSCROLL:
			//	�����X�N���[������������
			for( LONG Index = 0; Index < PageCount; Index ++ )
			{
				if( Wnd == PageWnd[Index] )
				{
					Page[Index]->wmHScroll( (HWND)lParam, LOWORD( wParam ) );
				}
			}
			break;

		case WM_NOTIFY:
			//	�ʒm����������
			for( LONG Index = 0; Index < PageCount; Index ++ )
			{
				if( Wnd == PageWnd[Index] )
				{
					Page[Index]->wmNotify( (NMHDR *)lParam );
				}
			}
			break;

		case WM_TIMER:
			//	�^�C�}�[����������
			for( LONG Index = 0; Index < PageCount; Index ++ )
			{
				if( Wnd == PageWnd[Index] )
				{
					Page[Index]->wmTimer( wParam );
				}
			}
			break;

		case WM_PAGE_CHANGED:
			//	�y�[�W�ύX�̒ʒm����������
			for( LONG Index = 0; Index < PageCount; Index ++ )
			{
				if( Wnd == PageWnd[Index] )
				{
					Page[Index]->wmPageChanged();
				}
			}
			break;

		case WM_SETTING_LIST_CHANGED:
			//	�ݒ�ύX�̒ʒm����������
			for( LONG Index = 0; Index < PageCount; Index ++ )
			{
				if( Wnd == PageWnd[Index] )
				{
					Page[Index]->wmSettingListChanged();
				}
			}
			break;
	}

	return( Result );
}

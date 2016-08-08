
//	Status Area Icon for Microsoft Xbox 360 Controller

#include	"status_area_icon.h"
#include	"resource.h"

//----------------------------------------------------------------------------------------------
//	CStatusAreaIcon
//----------------------------------------------------------------------------------------------
CStatusAreaIcon::CStatusAreaIcon(
	 HINSTANCE	Instance
	,HWND		Wnd )
{
	//	�C���X�^���X
	this->Instance	= Instance;
	//	�E�B���h�E �n���h��
	this->Wnd		= Wnd;

	//	�v���Z�X ID
	ProcessID		= NULL;

	//	�e�C���X�^���X���쐬����
	Setting			= new CSetting( Instance, Wnd );
	PropertySheet	= new CPropertySheet( Instance, Wnd );
	Device			= new CDevice();

	//	�f�o�C�X�����݂���΃X�e�[�^�X �G���A�ɃA�C�R����ǉ�����
	if( Device->IsConnected() == TRUE )
	{
		ShowIcon();
	}

	//	�ݒ�̎����؊����p�^�C�}�[���J�n����
	if( Setting->AutoSettingChange == TRUE )
	{
		SetTimer( Wnd, AUTO_SETTING_CHANGE_TIMER, 0, NULL );
	}
}

//----------------------------------------------------------------------------------------------
//	~CStatusAreaIcon
//----------------------------------------------------------------------------------------------
CStatusAreaIcon::~CStatusAreaIcon()
{
	//	�X�e�[�^�X �G���A����A�C�R�����폜����
	HideIcon();

	//	�e�C���X�^���X���J������
	delete Device;
	delete PropertySheet;
	delete Setting;
}

//----------------------------------------------------------------------------------------------
//	wmNotify
//----------------------------------------------------------------------------------------------
VOID CStatusAreaIcon::wmNotify(
	 WPARAM	wParam
	,LPARAM	lParam )
{
	switch( wParam )
	{
		case STATUS_AREA_ICON_ID:
			switch( lParam )
			{
				case WM_LBUTTONDOWN:
					//	�^�C�}�[�Ń|�b�v�A�b�v ���j���[�i���j��\������
					SetTimer( Wnd, LEFT_CLICK_TIMER, GetDoubleClickTime(), NULL );
					break;

				case WM_LBUTTONDBLCLK:
					//	���N���b�N�p�^�C�}�[���~����
					KillTimer( Wnd, LEFT_CLICK_TIMER );
					//	�v���p�e�B �V�[�g���J��
					if( PropertySheet->IsOpened() == FALSE )
					{
						PropertySheet->Open();
					}
					break;

				case WM_RBUTTONUP:
					//	�|�b�v�A�b�v ���j���[�i�E�j��\������
					ShowRightPopupMenu();
					break;
			}
			break;

		case UPDATE_SETTING:
			//	���W�X�g������ݒ��ǂݍ���
			Setting->LoadRegistry();
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	wmCommand
//----------------------------------------------------------------------------------------------
VOID CStatusAreaIcon::wmCommand( WORD CommandId )
{
	switch( CommandId )
	{
		case IDM_PROPERTY:
			//	�v���p�e�B(&P)
			PropertySheet->Open();
			break;

		case IDM_AUTO_SETTING_CHANGE:
			//	�ݒ�̎����؊���(A)
			if( Setting->AutoSettingChange == TRUE )
			{
				//	�ݒ�̎����؊������~����
				KillTimer( Wnd, AUTO_SETTING_CHANGE_TIMER );
				Setting->AutoSettingChange	= FALSE;
			} else {
				//	�ݒ�̎����؊������J�n����
				SetTimer( Wnd, AUTO_SETTING_CHANGE_TIMER, 0, NULL );
				Setting->AutoSettingChange	= TRUE;
			}
			//	���W�X�g���ɐݒ����������
			Setting->SaveRegistry();
			break;

		case IDM_EXIT:
			//	�I��(X)
			if( PropertySheet->IsOpened() == FALSE )
			{
				PostMessage( Wnd, WM_CLOSE, 0, 0 );
			}
			break;

		default:
			//	�R���g���[���[�̐ݒ��ύX����
			LONG	SettingIndex	= CommandId - IDM_SETTING;
			if(	PropertySheet->IsOpened() == FALSE
			&&	0 <= SettingIndex && SettingIndex < Setting->SettingCount
			&&	SettingIndex != Setting->CurrentSettingIndex )
			{
				//	�R���g���[���[�̐ݒ��ύX����
				Setting->Change( SettingIndex, FALSE );
				Setting->SaveRegistry();
				Device->Apply();
			}
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	wmTimer
//----------------------------------------------------------------------------------------------
VOID CStatusAreaIcon::wmTimer( WPARAM TimerId )
{
	switch( TimerId )
	{
		case LEFT_CLICK_TIMER:
			//	���N���b�N�p�^�C�}�[���~����
			KillTimer( Wnd, TimerId );
			//	�|�b�v�A�b�v ���j���[�i���j��\������
			ShowLeftPopupMenu();
			break;

		case AUTO_SETTING_CHANGE_TIMER:
			//	�ݒ�̎����؊����p�^�C�}�[���~����
			KillTimer( Wnd, TimerId );

			//	�t�H�A �O���E���h�̃A�v���P�[�V�����̃v���Z�X ID ���擾����
			DWORD	CurrentProcessID;
			GetWindowThreadProcessId( GetForegroundWindow(), &CurrentProcessID );
			if( ProcessID != CurrentProcessID )
			{
				//	�ݒ�̎����؊������s��
				if(	PropertySheet->IsOpened() == FALSE
				&&	Setting->ModifiedFlag == FALSE )
				{
					AutomaticChange( CurrentProcessID );
					//	�v���Z�X ID ��ޔ�����
					ProcessID	= CurrentProcessID;
				}
			}

			//	�ݒ�̎����؊����p�^�C�}�[���J�n����
			SetTimer( Wnd, AUTO_SETTING_CHANGE_TIMER, CHECK_INTERVAL, NULL );
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	wmDeviceChange
//----------------------------------------------------------------------------------------------
VOID CStatusAreaIcon::wmDeviceChange()
{
	//	�f�o�C�X�̗L���ɂ���ăA�C�R���̕\���A��\����؊�����
	if( Device->IsConnected() == TRUE )
	{
		//	�X�e�[�^�X �G���A�ɃA�C�R����ǉ�����
		ShowIcon();
	} else {
		//	�X�e�[�^�X �G���A����A�C�R�����폜����
		HideIcon();
	}
}

//----------------------------------------------------------------------------------------------
//	wmTaskbarCtreated
//----------------------------------------------------------------------------------------------
VOID CStatusAreaIcon::wmTaskbarCtreated()
{
	//	�f�o�C�X���L��΃X�e�[�^�X �G���A�ɃA�C�R����ǉ�����
	if( Device->IsConnected() == TRUE )
	{
		ShowIcon();
	}
}

//----------------------------------------------------------------------------------------------
//	ShowIcon
//----------------------------------------------------------------------------------------------
VOID CStatusAreaIcon::ShowIcon()
{
	//	�X�e�[�^�X �G���A�ɃA�C�R����ǉ�����
	NOTIFYICONDATA	IconData;
	ZeroMemory( &IconData, sizeof( NOTIFYICONDATA ) );
	IconData.cbSize				= sizeof( NOTIFYICONDATA );
	IconData.hWnd				= Wnd;
	IconData.uID				= STATUS_AREA_ICON_ID;
	IconData.uFlags				= NIF_ICON | NIF_MESSAGE | NIF_TIP;
	IconData.uCallbackMessage	= WM_STATUS_AREA_ICON_NOTIFY;
	IconData.hIcon				= (HICON)LoadImage(
									 Instance
									,MAKEINTRESOURCE( IDI_ICON )
									,IMAGE_ICON
									,0
									,0
									,LR_DEFAULTSIZE );
	LoadString(
		 Instance
		,IDS_ICON_TIP
		,IconData.szTip
		,sizeof( IconData.szTip ) / sizeof( TCHAR ) );
	Shell_NotifyIcon( NIM_ADD, &IconData );
	//	�A�C�R�����폜����
	if( IconData.hIcon != NULL )
	{
		DestroyIcon( IconData.hIcon );
	}
	SetForegroundWindow( Wnd );
}

//----------------------------------------------------------------------------------------------
//	HideIcon
//----------------------------------------------------------------------------------------------
VOID CStatusAreaIcon::HideIcon()
{
	//	�X�e�[�^�X �G���A����A�C�R�����폜����
	NOTIFYICONDATA	IconData;
	ZeroMemory( &IconData, sizeof( NOTIFYICONDATA ) );
	IconData.cbSize	= sizeof( NOTIFYICONDATA );
	IconData.hWnd	= Wnd;
	IconData.uID	= STATUS_AREA_ICON_ID;
	Shell_NotifyIcon( NIM_DELETE, &IconData );
}

//----------------------------------------------------------------------------------------------
//	ShowLeftPopupMenu
//----------------------------------------------------------------------------------------------
VOID CStatusAreaIcon::ShowLeftPopupMenu()
{
	//	�v���p�e�B �V�[�g�̏�Ԃ��擾����
	BOOL	PropertySheetOpened	= PropertySheet->IsOpened();

	//	�|�b�v�A�b�v ���j���[���쐬����
	HMENU	PopupMenu	= CreatePopupMenu();

	//	�R���g���[���[�̐ݒ��\������
	for( LONG Index = 0; Index < Setting->SettingCount; Index ++ )
	{
		MENUITEMINFO	ItemInfo;
		ZeroMemory( &ItemInfo, sizeof( ItemInfo ) );
		ItemInfo.cbSize	= sizeof( ItemInfo );
		ItemInfo.fMask	= MIIM_TYPE | MIIM_STATE | MIIM_ID;
		ItemInfo.fType	= MFT_STRING;
		ItemInfo.fState	= MFS_ENABLED;
		if( PropertySheetOpened == TRUE )
		{
			ItemInfo.fState	|= MFS_DISABLED;
		}
		ItemInfo.wID	= IDM_SETTING + Index;
		if( Setting->CurrentSettingIndex == Index )
		{
			ItemInfo.fState		|= MFS_CHECKED;
		}
		WCHAR	SettingName[MAX_PATH * 2];
		wcscpy_s( SettingName, Setting->SettingName[Index] );
		//	�ύX�ς݃t���O���オ���Ă���΁u�i�ύX�ς݁j�v��ݒ肷��
		if( Setting->ModifiedFlag == TRUE && Setting->CurrentSettingIndex == Index )
		{
			WCHAR	Modified[MAX_PATH];
			LoadString( Instance, IDS_MODIFIED, Modified, MAX_PATH );
			wcscat_s( SettingName, Modified );
		}
		ItemInfo.dwTypeData	= SettingName;
		InsertMenuItem( PopupMenu, Index, TRUE, &ItemInfo );
	}

	//	�|�b�v�A�b�v ���j���[��\������
	SetForegroundWindow( Wnd );
	POINT	CursorPos;
	GetCursorPos( &CursorPos );
	TrackPopupMenuEx(
		 PopupMenu
		,TPM_LEFTALIGN | TPM_RIGHTBUTTON
		,CursorPos.x
		,CursorPos.y
		,Wnd
		,NULL );
	DestroyMenu( PopupMenu );
	PostMessage( Wnd, WM_NULL, 0, 0 );
}

//----------------------------------------------------------------------------------------------
//	ShowRightPopupMenu
//----------------------------------------------------------------------------------------------
VOID CStatusAreaIcon::ShowRightPopupMenu()
{
	//	�v���p�e�B �V�[�g�̏�Ԃ��擾����
	BOOL	PropertySheetOpened	= PropertySheet->IsOpened();

	//	�|�b�v�A�b�v ���j���[���쐬����
	HMENU	Menu		= LoadMenu( Instance, MAKEINTRESOURCE( IDR_MENU ) );
	HMENU	PopupMenu	= GetSubMenu( Menu, 0 );

	//	�u�v���p�e�B�v���f�t�H���g�ɂ���
	MENUITEMINFO	ItemInfo;
	ZeroMemory( &ItemInfo, sizeof( ItemInfo ) );
	ItemInfo.cbSize	= sizeof( ItemInfo );
	GetMenuItemInfo( PopupMenu, IDM_PROPERTY, FALSE, &ItemInfo );
	ItemInfo.fMask	= MIIM_STATE;
	ItemInfo.fState	= MFS_ENABLED | MFS_DEFAULT;
	if( PropertySheetOpened == TRUE )
	{
		ItemInfo.fState	|= MFS_DISABLED;
	}
	SetMenuItemInfo( PopupMenu, IDM_PROPERTY, FALSE, &ItemInfo );

	//	�u�ݒ�̎����؊����v�̃`�F�b�N��ݒ肷��
	GetMenuItemInfo( PopupMenu, IDM_AUTO_SETTING_CHANGE, FALSE, &ItemInfo );
	ItemInfo.fMask	= MIIM_STATE;
	ItemInfo.fState	= MFS_ENABLED;
	if( Setting->AutoSettingChange == TRUE )
	{
		ItemInfo.fState	|= MFS_CHECKED;
	}
	if( PropertySheetOpened == TRUE )
	{
		ItemInfo.fState	|= MFS_DISABLED;
	}
	SetMenuItemInfo( PopupMenu, IDM_AUTO_SETTING_CHANGE, FALSE, &ItemInfo );

	//	�u�I���v�̗L���E������ݒ肷��
	GetMenuItemInfo( PopupMenu, IDM_EXIT, FALSE, &ItemInfo );
	ItemInfo.fMask	= MIIM_STATE;
	ItemInfo.fState	= MFS_ENABLED;
	if( PropertySheetOpened == TRUE )
	{
		ItemInfo.fState	|= MFS_DISABLED;
	}
	SetMenuItemInfo( PopupMenu, IDM_EXIT, FALSE, &ItemInfo );

	//	�|�b�v�A�b�v ���j���[��\������
	SetForegroundWindow( Wnd );
	POINT	CursorPos;
	GetCursorPos( &CursorPos );
	TrackPopupMenuEx(
		 PopupMenu
		,TPM_LEFTALIGN | TPM_RIGHTBUTTON
		,CursorPos.x
		,CursorPos.y
		,Wnd
		,NULL );
	DestroyMenu( Menu );
	PostMessage( Wnd, WM_NULL, 0, 0 );
}

//----------------------------------------------------------------------------------------------
//	AutomaticChange
//----------------------------------------------------------------------------------------------
VOID CStatusAreaIcon::AutomaticChange( DWORD ProcessID )
{
	//	�v���Z�X�̃X�i�b�v�V���b�g���쐬����
	HANDLE	ProcessSnapshot	= CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	//	�v���Z�X�̃G���g�����擾����
	PROCESSENTRY32	ProcessEntry;
	ZeroMemory( &ProcessEntry, sizeof( PROCESSENTRY32 ) );
	ProcessEntry.dwSize = sizeof( PROCESSENTRY32 );
	Process32FirstW( ProcessSnapshot, &ProcessEntry );
	//	�S�Ẵv���Z�X����������
	do
	{
		//	�t�H�A�O���E���h�̃A�v���P�[�V������T��
		if( ProcessEntry.th32ProcessID == ProcessID )
		{
			//	���W���[������؂�o��
			WCHAR	ModuleName[MAX_PATH];
			if( wcschr( ProcessEntry.szExeFile, '\\' ) == NULL )
			{
				wcscpy_s( ModuleName, ProcessEntry.szExeFile );
			} else {
				wcscpy_s( ModuleName, &(wcsrchr( ProcessEntry.szExeFile, '\\' )[1]) );
			}

			//	���W���[�����ƈ�v����R���g���[���[�̐ݒ����������
			for( LONG Index = 1; Index < Setting->SettingCount; Index ++ )
			{
				if(	( _wcsicmp( ModuleName, Setting->SettingName[Index] ) == 0 )
				&&	( Setting->CurrentSettingIndex != Index ) )
				{
					//	�R���g���[���[�̐ݒ��ύX����
					Setting->Change( Index, TRUE );
					Setting->SaveRegistry();
					Device->Apply();
					break;
				}
			}
			break;
		}
	} while( Process32NextW(ProcessSnapshot, &ProcessEntry ) );

	//	�X�i�b�v�V���b�g�����
	CloseHandle( ProcessSnapshot );
}

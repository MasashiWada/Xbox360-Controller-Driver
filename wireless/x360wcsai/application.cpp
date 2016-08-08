
//	Status Area Icon for Microsoft Xbox 360 Wireless Controller

#include	"application.h"
#include	"status_area_icon.h"

//----------------------------------------------------------------------------------------------
//	�O���[�o���ϐ�
//----------------------------------------------------------------------------------------------

HINSTANCE	Instance;

//----------------------------------------------------------------------------------------------
//	�֐�
//----------------------------------------------------------------------------------------------

ATOM RegisterMyClass();
HWND CreateMyWindow();
LRESULT CALLBACK WindowProc( HWND, UINT, WPARAM, LPARAM );

//----------------------------------------------------------------------------------------------
//	WinMain
//----------------------------------------------------------------------------------------------
int WINAPI WinMain(
	 HINSTANCE	InstanceHandle
	,HINSTANCE	PrevInstanceHandle
	,LPSTR		CmdLine
	,int		CmdShow )
{
	//	�ϐ��錾
	MSG	Msg;

	//	�A�v���P�[�V�����̃C���X�^���X��ۑ�����
	Instance	= InstanceHandle;

	//	�A�v���P�[�V�����̕����N����h�~����
	HANDLE	Mutex	= CreateMutex( NULL, TRUE, X360WCSAI_MUTEX );
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		return( ERROR_ALREADY_EXISTS );
	}

	//	�E�B���h�E �N���X��o�^����
	ATOM	Atom	= RegisterMyClass();
	if( Atom == 0 )
	{
		return( 0 );
	}

	//	�o�^�����E�B���h�E �N���X����E�B���h�E���쐬����
	HWND	Wnd	= CreateMyWindow();
	if( Wnd == NULL )
	{
		return( 0 );
	}

	//	���b�Z�[�W ���[�v
	for(;;)
	{
		//	���b�Z�[�W �L���[���烁�b�Z�[�W���擾����
		BOOL	Result	= GetMessage( &Msg, NULL, 0, 0 );
		//	�擾�Ɏ��s�����ꍇ�͏I������
		if( Result == -1 )
		{
			return( 0 );
		}
		//	WM_QUIT ���b�Z�[�W���擾�����ꍇ�̓��[�v���I������
		if( Result == 0 )
		{
			break;
		}

		//	���z�L�[ ���b�Z�[�W�𕶎����b�Z�[�W�֕ϊ�����
		TranslateMessage( &Msg );
		//	���b�Z�[�W���E�B���h�E �v���V�[�W���֑��o����
		DispatchMessage( &Msg );
	}

	//	�~���[�e�b�N�X���J������
	CloseHandle( Mutex );

	//	WM_QUIT ���b�Z�[�W�̏I���R�[�h��Ԃ�
	return( (int)Msg.wParam );
}

//----------------------------------------------------------------------------------------------
//	RegisterMyClass
//----------------------------------------------------------------------------------------------
ATOM RegisterMyClass()
{
	//	�E�B���h�E �N���X�̑�����ݒ肷��
	WNDCLASSEX	wcex;
	ZeroMemory( &wcex, sizeof( wcex ) );
	wcex.cbSize			= sizeof( wcex );
	wcex.style			= 0;
	wcex.lpfnWndProc	= WindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= Instance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= X360WCSAI_CLASS_NAME;
	wcex.hIconSm		= NULL;

	//	�E�B���h�E �N���X��o�^����
	return( RegisterClassEx( &wcex ) );
}

//----------------------------------------------------------------------------------------------
//	CreateMyWindow
//----------------------------------------------------------------------------------------------
HWND CreateMyWindow()
{
	//	�o�^�����E�B���h�E �N���X����E�B���h�E���쐬����
	HWND	Wnd	= CreateWindowEx(
					 0
					,X360WCSAI_CLASS_NAME
					,X360WCSAI_WINDOW_NAME
					,WS_OVERLAPPEDWINDOW
					,CW_USEDEFAULT
					,CW_USEDEFAULT
					,CW_USEDEFAULT
					,CW_USEDEFAULT
					,NULL
					,NULL
					,Instance
					,NULL );

	//	�쐬���ʂ�Ԃ�
	return( Wnd );
}

//----------------------------------------------------------------------------------------------
//	WindowProc
//----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(
	 HWND	Wnd
	,UINT	Msg
	,WPARAM	wParam
	,LPARAM lParam )
{
	//	�ϐ��錾
			LRESULT				Result	= 0;
	static	CStatusAreaIcon	*	StatusAreaIcon;
	static	UINT				TaskbarCreated;

	//	���b�Z�[�W�ɂ���ď�����U�蕪����
	switch( Msg )
	{
		case WM_CREATE:
			//	�R���� �R���g���[��������������
			InitCommonControls();
			//	�X�e�[�^�X �G���A �A�C�R�����쐬����
			StatusAreaIcon	= new CStatusAreaIcon( Instance, Wnd );
			//	�^�X�N �o�[�̍č쐬�ɔ�����
			TaskbarCreated	= RegisterWindowMessage( L"TaskbarCreated" );
			break;

		case WM_STATUS_AREA_ICON_NOTIFY:
			//	�X�e�[�^�X �G���A �A�C�R���̃C�x���g����������
			StatusAreaIcon->wmNotify( wParam, lParam );
			break;

		case WM_COMMAND:
			//	�R�}���h����������
			StatusAreaIcon->wmCommand( LOWORD( wParam ) );
			break;

		case WM_TIMER:
			//	�^�C�}�[����������
			StatusAreaIcon->wmTimer( wParam );
			break;

		case WM_DEVICECHANGE:
			//	�f�o�C�X�̕ύX����������
			StatusAreaIcon->wmDeviceChange();
			Result	= TRUE;
			break;

		case WM_DESTROY:
			//	�X�e�[�^�X �G���A �A�C�R�����폜����
			delete StatusAreaIcon;
			//	�A�v���P�[�V�������I������
			PostQuitMessage( 0 );
			break;

		default:
			//	�^�X�N �o�[�̍č쐬����������
			if( Msg == TaskbarCreated )
			{
				StatusAreaIcon->wmTaskbarCtreated();
			}
			//	����̃E�B���h�E �v���V�[�W�����Ăяo��
			Result	= DefWindowProc( Wnd, Msg, wParam, lParam );
			break;
	}

	//	���b�Z�[�W�����̌��ʂ�Ԃ�
	return( Result );
}

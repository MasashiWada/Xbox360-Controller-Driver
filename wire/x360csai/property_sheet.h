
//	Status Area Icon for Microsoft Xbox 360 Controller

#pragma	once

#include	<windows.h>
#include	<commctrl.h>
#include	<objbase.h>
#include	<initguid.h>

//----------------------------------------------------------------------------------------------
//	�萔
//----------------------------------------------------------------------------------------------

#define	X360CPS_MUTEX				L"x360cps mutex"

//----------------------------------------------------------------------------------------------
//	GUID
//----------------------------------------------------------------------------------------------

DEFINE_GUID( ClassID_X360CPS,
	0xFFB0C361, 0x5623, 0x49AA, 0xBD, 0x51, 0xB3, 0x21, 0xDB, 0x96, 0x25, 0xCE );

DEFINE_GUID( IID_IDIGameCntrlPropSheet,
	0x7854FB22, 0x8EE3, 0x11D0, 0xA1, 0xAC, 0x00, 0x00, 0xF8, 0x02, 0x69, 0x77 );

//----------------------------------------------------------------------------------------------
//	�\����
//----------------------------------------------------------------------------------------------

typedef struct
{
	DWORD		dwSize;
	USHORT		nNumPages;
	LPWSTR		lpwszSheetCaption;
	BOOL		fSheetIconFlag;
	LPWSTR		lpwszSheetIcon;
} DIGCSHEETINFO, *LPDIGCSHEETINFO;

typedef struct
{
	DWORD		dwSize;
	LPWSTR		lpwszPageTitle;
	DLGPROC		fpPageProc;
	BOOL		fProcFlag;
	DLGPROC		fpPrePostProc;
	BOOL		fIconFlag;
	LPWSTR		lpwszPageIcon;
	LPWSTR		lpwszTemplate;
	LPARAM		lParam;
	HINSTANCE	hInstance;
} DIGCPAGEINFO, *LPDIGCPAGEINFO;

//----------------------------------------------------------------------------------------------
//	�C���^�[�t�F�C�X
//----------------------------------------------------------------------------------------------

#undef INTERFACE
#define INTERFACE IDIGameCntrlPropSheet

DECLARE_INTERFACE_( IDIGameCntrlPropSheet, IUnknown )
{
	//	IUnknown
	STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID * ) PURE;
	STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
	STDMETHOD_( ULONG, Release )( THIS ) PURE;

	//	IDIGameCntrlPropSheet
	STDMETHOD( GetSheetInfo )( THIS_ LPDIGCSHEETINFO * ) PURE;
	STDMETHOD( GetPageInfo )( THIS_ LPDIGCPAGEINFO * ) PURE;
	STDMETHOD( SetID )( THIS_ USHORT ) PURE;
	STDMETHOD_( USHORT, GetID )( THIS ) PURE;
};

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CPropertySheet
{

public:

	//	�R���X�g���N�^
	CPropertySheet( HINSTANCE, HWND );

	//	�v���p�e�B �V�[�g���J��
	VOID Open();

	//	�v���p�e�B �V�[�g���J���Ă��邩�ǂ�����Ԃ�
	BOOL IsOpened();

private:

	//	�C���X�^���X
	HINSTANCE	Instance;
	//	�E�B���h�E �n���h��
	HWND		Wnd;

protected:

};

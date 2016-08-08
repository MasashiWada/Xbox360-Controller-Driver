
//	Status Area Icon for Microsoft Xbox 360 Wireless Controller

#include	"property_sheet.h"

//----------------------------------------------------------------------------------------------
//	CPropertySheet
//----------------------------------------------------------------------------------------------
CPropertySheet::CPropertySheet(
	 HINSTANCE	Instance
	,HWND		Wnd )
{
	//	�C���X�^���X
	this->Instance	= Instance;
	//	�E�B���h�E �n���h��
	this->Wnd		= Wnd;
}

//----------------------------------------------------------------------------------------------
//	Open
//----------------------------------------------------------------------------------------------
VOID CPropertySheet::Open()
{
	//	COM ������������
	CoInitialize( NULL );

	//	IDIGameCntrlPropSheet ���쐬����
	IDIGameCntrlPropSheet *	PropSheet;
	HRESULT	 Result	= CoCreateInstance(
						 ClassID_X360WCPS
						,NULL
						,CLSCTX_SERVER
						,IID_IDIGameCntrlPropSheet
						,(LPVOID *)&PropSheet );
	if( Result != S_OK )
	{
		return;
	}

	//	�V�[�g�̏����擾����
	LPDIGCSHEETINFO	DiGCSheetInfo;
	Result	= PropSheet->GetSheetInfo( &DiGCSheetInfo );
	if( Result != S_OK )
	{
		return;
	}

	//	�y�[�W�̏����擾����
	LPDIGCPAGEINFO	DiGCPageInfo;
	Result	= PropSheet->GetPageInfo( &DiGCPageInfo );
	if( Result != S_OK )
	{
		return;
	}

	//	�y�[�W�̏����쐬����
	PROPSHEETPAGE *	PageInfo	= (PROPSHEETPAGE *)LocalAlloc(
									 LPTR
									,sizeof( PROPSHEETPAGE ) * DiGCSheetInfo->nNumPages );
	for( LONG Index = 0; Index < DiGCSheetInfo->nNumPages; Index ++ )
	{
		PageInfo[Index].dwSize		= sizeof( PROPSHEETPAGE );
		PageInfo[Index].dwFlags		= PSP_USEICONID;
		PageInfo[Index].hInstance	= DiGCPageInfo[Index].hInstance;
		PageInfo[Index].pszTemplate	= DiGCPageInfo[Index].lpwszTemplate;
		PageInfo[Index].pszIcon		= DiGCPageInfo[Index].lpwszPageIcon;
		PageInfo[Index].pszTitle	= DiGCPageInfo[Index].lpwszPageTitle;
		PageInfo[Index].pfnDlgProc	= DiGCPageInfo[Index].fpPageProc;
		PageInfo[Index].lParam		= DiGCPageInfo[Index].lParam;
		PageInfo[Index].pfnCallback	= NULL;
	}

	//	�V�[�g�̏����쐬����
	PROPSHEETHEADER	SheetHeader;
	SheetHeader.dwSize		= sizeof( PROPSHEETHEADER );
	SheetHeader.dwFlags		= PSH_USEICONID | PSH_PROPSHEETPAGE;
	SheetHeader.hwndParent	= Wnd;
	SheetHeader.hInstance	= Instance;
	SheetHeader.pszIcon		= DiGCSheetInfo->lpwszSheetIcon;
	SheetHeader.pszCaption	= DiGCSheetInfo->lpwszSheetCaption;
	SheetHeader.nPages		= DiGCSheetInfo->nNumPages;
	SheetHeader.nStartPage	= 0;
	SheetHeader.ppsp		= (LPCPROPSHEETPAGE)PageInfo;
	SheetHeader.pfnCallback	= NULL;

	//	�v���p�e�B �V�[�g��\������
	PropertySheet( &SheetHeader );

	//	IDIGameCntrlPropSheet ���J������
	PropSheet->Release();

	//	COM ���I������
	CoUninitialize();
}

//----------------------------------------------------------------------------------------------
//	IsOpened
//----------------------------------------------------------------------------------------------
BOOL CPropertySheet::IsOpened()
{
	//	�v���p�e�B �V�[�g���J���Ă��邩�m�F����
	HANDLE	Mutex	= CreateMutex( NULL, FALSE, X360WCPS_MUTEX );
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		//	�~���[�e�b�N�X���J������
		CloseHandle( Mutex );
		//	�v���p�e�B �V�[�g�͊J���Ă���
		return( TRUE );
	} else {
		//	�~���[�e�b�N�X���J������
		CloseHandle( Mutex );
		//	�v���p�e�B �V�[�g�͕��Ă���
		return( FALSE );
	}
}

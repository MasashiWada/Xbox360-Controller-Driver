
//	Force Feedback Driver for Microsoft Xbox 360 Controller

#include	"extension.h"
#include	"com.h"

//----------------------------------------------------------------------------------------------
//	�O���[�o���ϐ�
//----------------------------------------------------------------------------------------------

CRITICAL_SECTION	CriticalSection;

//----------------------------------------------------------------------------------------------
//	DllMain
//----------------------------------------------------------------------------------------------
BOOL WINAPI DllMain(
	 HINSTANCE	Instance
	,DWORD		Reason
	,LPVOID		Reserved )
{
	switch( Reason )
	{
		case DLL_PROCESS_ATTACH:
			//	�N���e�B�J�� �Z�N�V����������������
			InitializeCriticalSection( &CriticalSection );
			break;

		case DLL_PROCESS_DETACH:
			//	�N���e�B�J�� �Z�N�V�������폜����
			DeleteCriticalSection( &CriticalSection );
			break;
	}

	return( TRUE );
}

//----------------------------------------------------------------------------------------------
//	DllGetClassObject
//----------------------------------------------------------------------------------------------
STDAPI DllGetClassObject(
	 REFCLSID	ClassID
	,REFIID		InterfaceID
	,LPVOID *	Interface )
{
	//	�ϐ��錾
	HRESULT	Result	= S_OK;

	//	IClassFactory ���쐬����
	if( IsEqualGUID( ClassID, ClassID_X360CFFD ) )
	{
		CClassFactory *	Factory	= new CClassFactory();
		if( Factory != NULL )
		{
			Result	= Factory->QueryInterface( InterfaceID, Interface );
			Factory->Release();
		} else {
			Result	= E_OUTOFMEMORY;
		}
	} else {
		Result	= CLASS_E_CLASSNOTAVAILABLE;
	}

	return( Result );
}

//----------------------------------------------------------------------------------------------
//	DllCanUnloadNow
//----------------------------------------------------------------------------------------------
STDAPI DllCanUnloadNow( VOID )
{
	return( S_OK );
}

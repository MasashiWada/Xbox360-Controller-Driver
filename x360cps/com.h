
//	Property Sheet for Microsoft Xbox 360 Controller

#pragma	once

#include	<windows.h>
#include	<objbase.h>

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CClassFactory : public IClassFactory
{

public:

	//	�R���X�g���N�^
	CClassFactory( VOID );

	//	IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID, PVOID * );
	ULONG STDMETHODCALLTYPE AddRef( VOID );
	ULONG STDMETHODCALLTYPE Release( VOID );

	//	IClassFactory
	HRESULT STDMETHODCALLTYPE CreateInstance( IUnknown *, REFIID, PVOID * );
	HRESULT STDMETHODCALLTYPE LockServer( BOOL );

private:

	//	�Q�ƃJ�E���^
	LONG	ReferenceCount;

};

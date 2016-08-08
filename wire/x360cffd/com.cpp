
//	Force Feedback Driver for Microsoft Xbox 360 Controller

#include	"com.h"
#include	"effect_driver.h"

//----------------------------------------------------------------------------------------------
//	CClassFactory
//----------------------------------------------------------------------------------------------
CClassFactory::CClassFactory( VOID )
{
	//	�Q�ƃJ�E���^������������
	ReferenceCount	= 1;
}

//----------------------------------------------------------------------------------------------
//	QueryInterface
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CClassFactory::QueryInterface(
	 REFIID		InterfaceID
	,PVOID *	Interface )
{
	//	�ϐ��錾
	HRESULT	Result	= S_OK;

	//	���g�̃C���^�[�t�F�C�X��Ԃ�
	if(	IsEqualIID( InterfaceID, IID_IUnknown )
	||	IsEqualIID( InterfaceID, IID_IClassFactory ) )
	{
		AddRef();
		*Interface	= this;
	} else {
		*Interface	= NULL;
		Result		= E_NOINTERFACE;
	}

	return( Result );
}

//----------------------------------------------------------------------------------------------
//	AddRef
//----------------------------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CClassFactory::AddRef( VOID )
{
	//	�Q�ƃJ�E���^�����Z����
	return( InterlockedIncrement( &ReferenceCount ) );
}

//----------------------------------------------------------------------------------------------
//	Release
//----------------------------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CClassFactory::Release( VOID )
{
	//	�Q�ƃJ�E���^�����Z����
	if( InterlockedDecrement( &ReferenceCount ) == 0 )
	{
		delete this;
		return( 0 );
	}

	return( ReferenceCount );
}

//----------------------------------------------------------------------------------------------
//	CreateInstance
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CClassFactory::CreateInstance(
	 IUnknown *	UnknownInterface
	,REFIID		InterfaceID
	,PVOID *	Interface )
{
	//	�ϐ��錾
	HRESULT	Result	= S_OK;

	//	IDirectInputEffectDriver ���쐬����
	CEffectDriver *	EffectDriver	= new CEffectDriver();
	if( EffectDriver != NULL )
	{
		Result	= EffectDriver->QueryInterface( InterfaceID, Interface );
		EffectDriver->Release();
	} else {
		Result	= E_OUTOFMEMORY;
	}

	return( Result );
}

//----------------------------------------------------------------------------------------------
//	LockServer
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CClassFactory::LockServer( BOOL Lock )
{
	return( S_OK );
}
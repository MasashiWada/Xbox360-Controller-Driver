
//	Force Feedback Driver for Microsoft Xbox 360 Wireless Controller

#include	"effect_driver.h"

//----------------------------------------------------------------------------------------------
//	CEffectDriver
//----------------------------------------------------------------------------------------------
CEffectDriver::CEffectDriver( VOID )
{
	//	�Q�ƃJ�E���^������������
	ReferenceCount	= 1;

	//	�f�o�C�X �C���^�[�t�F�C�X
	Device			= NULL;

	//	�R���g���[���[ ID
	ControllerId	= 0;

	//	���[�J �X���b�h
	WorkerThread	= NULL;

	//	�G�t�F�N�g�ԍ�
	EffectIndex		= 1;
	//	�G�t�F�N�g��
	EffectCount		= 0;
	//	�G�t�F�N�g ���X�g
	EffectList		= NULL;

	//	��~
	Stopped			= TRUE;
	//	�ꎞ��~
	Paused			= FALSE;
	//	�ꎞ��~��������
	PausedTime		= 0;

	//	�Q�C��
	Gain			= 10000;
	//	�A�N�`���G�[�^
	Actuator		= TRUE;

	//	�I���V�O�i��
	Quit			= FALSE;
}

//----------------------------------------------------------------------------------------------
//	QueryInterface
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::QueryInterface(
	 REFIID		InterfaceID
	,PVOID *	Interface )
{
	//	�ϐ��錾
	HRESULT	Result	= S_OK;

	//	���g�̃C���^�[�t�F�C�X��Ԃ�
	if(	IsEqualIID( InterfaceID, IID_IUnknown )
	||	IsEqualIID( InterfaceID, IID_IDirectInputEffectDriver ) )
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
ULONG STDMETHODCALLTYPE CEffectDriver::AddRef( VOID )
{
	//	�Q�ƃJ�E���^�����Z����
	return( InterlockedIncrement( &ReferenceCount ) );
}

//----------------------------------------------------------------------------------------------
//	Release
//----------------------------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CEffectDriver::Release( VOID )
{
	//	�Q�ƃJ�E���^�����Z����
	if( InterlockedDecrement( &ReferenceCount ) == 0 )
	{
		//	�h���C�o�̏�Ԃ��I����Ԃɂ���
		Quit	= TRUE;

		//	���[�J�[ �X���b�h���I������̂�҂�
		for(;;)
		{
			DWORD	ExitCode;
			GetExitCodeThread( WorkerThread, &ExitCode );
			if( ExitCode != STILL_ACTIVE )
			{
				//	�X���b�h�����
				CloseHandle( WorkerThread );
				break;
			}
			//	1ms �҂�
			Sleep( 1 );
		}

		//	�f�o�C�X �C���^�[�t�F�C�X�����
		if( Device != NULL )
		{
			CloseHandle( Device );
		}

		delete this;
		return( 0 );
	}

	return( ReferenceCount );
}

//----------------------------------------------------------------------------------------------
//	DeviceID
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::DeviceID(
	 DWORD	DiVersion
	,DWORD	ExternalID
	,DWORD	Begin
	,DWORD	InternalID
	,LPVOID	DiHIDFFInitInfo )
{
	//	�ϐ��錾
	WCHAR *	ControllerIdPos;

	//	�f�o�C�X �C���^�[�t�F�C�X���J��
	Device	= CreateFile(
				 ((LPDIHIDFFINITINFO)DiHIDFFInitInfo)->pwszDeviceInterface
				,GENERIC_READ | GENERIC_WRITE
				,FILE_SHARE_READ | FILE_SHARE_WRITE
				,NULL
				,OPEN_EXISTING
				,FILE_ATTRIBUTE_NORMAL
				,NULL );
	if( Device == INVALID_HANDLE_VALUE )
	{
		return( E_FAIL );
	}

	//	�R���g���[���� ID ���擾����
	ControllerIdPos	= wcsstr(
						 ((LPDIHIDFFINITINFO)DiHIDFFInitInfo)->pwszDeviceInterface
						,L"&col0" );
	if( ControllerIdPos == NULL )
	{
		ControllerId	= 1;
	} else {
		ControllerId	= ControllerIdPos[5] - '0';
	}

	//	���[�J�[ �X���b�h���J�n����
	DWORD	ThreadID;
	WorkerThread	= CreateThread(
						 NULL
						,0
						,EffectProc
						,(LPVOID)this
						,0
						,&ThreadID );
	if( WorkerThread == NULL )
	{
		return( E_FAIL );
	}

	return( S_OK );
}

//----------------------------------------------------------------------------------------------
//	GetVersions
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::GetVersions( LPDIDRIVERVERSIONS DriverVersions )
{
	//	�h���C�o �o�[�W�����̃T�C�Y���`�F�b�N����
	if( DriverVersions->dwSize != sizeof( DIDRIVERVERSIONS ) )
	{
		return( E_INVALIDARG );
	}

	//	�h���C�o �o�[�W������Ԃ�
	DriverVersions->dwFirmwareRevision	= 1;
	DriverVersions->dwHardwareRevision	= 1;
	DriverVersions->dwFFDriverVersion	= 1;

	return( S_OK );
}

//----------------------------------------------------------------------------------------------
//	Escape
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::Escape(
	 DWORD			ExternalID
	,DWORD			Effect
	,LPDIEFFESCAPE	DIEffEscape )
{
	//	���T�|�[�g
	return( E_NOTIMPL );
}

//----------------------------------------------------------------------------------------------
//	SetGain
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::SetGain(
	 DWORD	ExternalID
	,DWORD	NewGain )
{
	//	�ϐ��錾
	HRESULT	Result	= S_OK;

	//	�N���e�B�J�� �Z�N�V�����ɓ���
	EnterCriticalSection( &CriticalSection );

	//	�Q�C����ݒ肷��
	if( 1 <= NewGain && NewGain <= 10000 )
	{
		Gain	= NewGain;
	} else {
		Gain	= max( 1, min( NewGain, 10000 ) );
		Result	= DI_TRUNCATED;
	}

	//	�N���e�B�J�� �Z�N�V�������o��
	LeaveCriticalSection( &CriticalSection );

	return( Result );
}

//----------------------------------------------------------------------------------------------
//	SendForceFeedbackCommand
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::SendForceFeedbackCommand(
	 DWORD	ExternalID
	,DWORD	Command )
{
	//	�ϐ��錾
	HRESULT	Result	= S_OK;

	//	�N���e�B�J�� �Z�N�V�����ɓ���
	EnterCriticalSection( &CriticalSection );

	//	�R�}���h�ɂ���ď�����U�蕪����
	switch( Command ) {

		case DISFFC_RESET:
			//	�S�ẴG�t�F�N�g���폜����
			for( LONG Index = 0; Index < EffectCount; Index ++ )
			{
				delete EffectList[Index];
			}
			//	�G�t�F�N�g���� 0 �ɂ���
			EffectCount	= 0;
			//	�G�t�F�N�g ���X�g���폜����
			free( EffectList );
			EffectList	= NULL;
			//	�Đ����~����
			Stopped	= TRUE;
			//	�ꎞ��~����������
			Paused	= FALSE;
			break;

		case DISFFC_STOPALL:
			//	�S�ẴG�t�F�N�g�̍Đ����~����
			for( LONG Index = 0; Index < EffectCount; Index ++ )
			{
				EffectList[Index]->Status	= NULL;
			}
			//	�Đ����~����
			Stopped	= TRUE;
			//	�ꎞ��~����������
			Paused	= FALSE;
			break;

		case DISFFC_PAUSE:
			//	�Đ����ꎞ��~����
			Paused		= TRUE;
			PausedTime	= GetTickCount();
			break;

		case DISFFC_CONTINUE:
			//	�ꎞ��~����������
			for( LONG Index = 0; Index < EffectCount; Index ++ )
			{
				//	�ꎞ��~�������Ԃ����G�t�F�N�g�̊J�n���Ԃ�x�点��
				EffectList[Index]->StartTime	+= ( GetTickCount() - PausedTime );
			}
			Paused	= FALSE;
			break;

		case DISFFC_SETACTUATORSON:
			//	�A�N�`���G�[�^��L���ɂ���
			Actuator	= TRUE;
			break;

		case DISFFC_SETACTUATORSOFF:
			//	�A�N�`���G�[�^�𖳌��ɂ���
			Actuator	= FALSE;
			break;

		default:
			Result	= E_NOTIMPL;
			break;
	}

	//	�N���e�B�J�� �Z�N�V�������o��
	LeaveCriticalSection( &CriticalSection );

	return( Result );
}

//----------------------------------------------------------------------------------------------
//	GetForceFeedbackState
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::GetForceFeedbackState(
	 DWORD				ExternalID
	,LPDIDEVICESTATE	DeviceState )
{
	//	�f�o�C�X��Ԃ̃T�C�Y���`�F�b�N����
	if( DeviceState->dwSize != sizeof( DIDEVICESTATE ) )
	{
		return( E_INVALIDARG );
	}

	//	�N���e�B�J�� �Z�N�V�����ɓ���
	EnterCriticalSection( &CriticalSection );

	//	�G�t�F�N�g �h���C�o�̏�Ԃ�Ԃ�
	DeviceState->dwState	= NULL;
	//	�G�t�F�N�g ���X�g���󂩂ǂ���
	if( EffectCount == 0 )
	{
		DeviceState->dwState	|= DIGFFS_EMPTY;
	}
	//	�G�t�F�N�g����~�����ǂ���
	if( Stopped == TRUE )
	{
		DeviceState->dwState	|= DIGFFS_STOPPED;
	}
	//	�G�t�F�N�g���ꎞ��~�����ǂ���
	if( Paused == TRUE )
	{
		DeviceState->dwState	|= DIGFFS_PAUSED;
	}
	//	�A�N�`���G�[�^���쓮���Ă��邩�ǂ���
	if( Actuator == TRUE )
	{
		DeviceState->dwState	|= DIGFFS_ACTUATORSON;
	} else {
		DeviceState->dwState	|= DIGFFS_ACTUATORSOFF;
	}
	//	�d���̓I���Œ�
	DeviceState->dwState	|= DIGFFS_POWERON;
	//	�Z�[�t�e�B �X�C�b�`�̓I�t�Œ�
	DeviceState->dwState	|= DIGFFS_SAFETYSWITCHOFF;
	//	���[�U�[ �X�C�b�`�̓I���Œ�
	DeviceState->dwState	|= DIGFFS_USERFFSWITCHON;

	//	���[�h�\���͖������Œ�
	DeviceState->dwLoad		= 0;

	//	�N���e�B�J�� �Z�N�V�������o��
	LeaveCriticalSection( &CriticalSection );

	return( S_OK );
}

//----------------------------------------------------------------------------------------------
//	DownloadEffect
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::DownloadEffect(
	 DWORD			ExternalID
	,DWORD			EffectType
	,LPDWORD		EffectHandle
	,LPCDIEFFECT	DiEffect
	,DWORD			Flags )
{
	//	�ϐ��錾
	CEffect *	Effect;

	//	DIEP_NODOWNLOAD �̏ꍇ�͖�������
	if( Flags & DIEP_NODOWNLOAD )
	{
		return( S_OK );
	}

	//	�N���e�B�J�� �Z�N�V�����ɓ���
	EnterCriticalSection( &CriticalSection );

	if( *EffectHandle == NULL )
	{
		//	�V�K�G�t�F�N�g���쐬����
		Effect	= new CEffect();
		Effect->Handle	= ( EffectIndex ++ );
		//	�G�t�F�N�g���𑝂₷
		EffectCount	++;
		//	�G�t�F�N�g ���X�g���g������
		CEffect * *	NewEffectList;
		NewEffectList	= (CEffect * *)realloc( EffectList, sizeof( CEffect * ) * EffectCount );
		if( NewEffectList != NULL )
		{
			EffectList	= NewEffectList;
		} else {
			//	�N���e�B�J�� �Z�N�V�������o��
			LeaveCriticalSection( &CriticalSection );
			//	�G���[��Ԃ�
			return( E_OUTOFMEMORY );
		}
		EffectList[EffectCount - 1]	= Effect;
		//	�G�t�F�N�g�̃n���h����Ԃ�
		*EffectHandle	= Effect->Handle;
	} else {
		//	�n���h������G�t�F�N�g����������
		for( LONG Index = 0; Index < EffectCount; Index ++ )
		{
			if( EffectList[Index]->Handle == *EffectHandle )
			{
				Effect	= EffectList[Index];
				break;
			}
		}
	}

	//	��ނ�ݒ肷��
	Effect->Type	= EffectType;

	//	Flags ��ݒ肷��
	Effect->DiEffect.dwFlags	= DiEffect->dwFlags;

	//	DIEP_DURATION ��ݒ肷��
	if( Flags & DIEP_DURATION )
	{
		Effect->DiEffect.dwDuration	= DiEffect->dwDuration;
	}

	//	DIEP_SAMPLEPERIOD ��ݒ肷��
	if( Flags & DIEP_SAMPLEPERIOD )
	{
		Effect->DiEffect.dwSamplePeriod	= DiEffect->dwSamplePeriod;
	}

	//	DIEP_GAIN ��ݒ肷��
	if( Flags & DIEP_GAIN )
	{
		Effect->DiEffect.dwGain	= DiEffect->dwGain;
	}

	//	DIEP_TRIGGERBUTTON ��ݒ肷��
	if( Flags & DIEP_TRIGGERBUTTON )
	{
		Effect->DiEffect.dwTriggerButton	= DiEffect->dwTriggerButton;
	}

	//	DIEP_TRIGGERREPEATINTERVAL ��ݒ肷��
	if( Flags & DIEP_TRIGGERREPEATINTERVAL )
	{
		Effect->DiEffect.dwTriggerRepeatInterval	= DiEffect->dwTriggerRepeatInterval;
	}

	//	DIEP_AXES ��ݒ肷��
	if( Flags & DIEP_AXES )
	{
		Effect->DiEffect.cAxes		= DiEffect->cAxes;
		Effect->DiEffect.rgdwAxes	= NULL;
	}

	//	DIEP_DIRECTION ��ݒ肷��
	if( Flags & DIEP_DIRECTION )
	{
		Effect->DiEffect.cAxes			= DiEffect->cAxes;
		Effect->DiEffect.rglDirection	= NULL;
	}

	//	DIEP_ENVELOPE ��ݒ肷��
	if( ( Flags & DIEP_ENVELOPE ) && DiEffect->lpEnvelope != NULL )
	{
		CopyMemory( &Effect->DiEnvelope, DiEffect->lpEnvelope, sizeof( DIENVELOPE ) );
		//	�A�^�b�N���Ԃƃt�F�[�h���Ԃ����Ԃ�ꍇ�͒l��␳����
		if( Effect->DiEffect.dwDuration - Effect->DiEnvelope.dwFadeTime
				< Effect->DiEnvelope.dwAttackTime )
		{
			Effect->DiEnvelope.dwFadeTime	= Effect->DiEnvelope.dwAttackTime;
		}
		Effect->DiEffect.lpEnvelope	= &Effect->DiEnvelope;
	}

	//	TypeSpecificParams ��ݒ肷��
	Effect->DiEffect.cbTypeSpecificParams	= DiEffect->cbTypeSpecificParams;

	//	DIEP_TYPESPECIFICPARAMS ��ݒ肷��
	if( Flags & DIEP_TYPESPECIFICPARAMS )
	{
		switch( EffectType )
		{
			case SPRING:
			case DAMPER:
			case INERTIA:
			case FRICTION:
				Effect->DiEffect.lpvTypeSpecificParams	= NULL;
				break;

			case CONSTANT_FORCE:
				CopyMemory(
					 &Effect->DiConstantForce
					,DiEffect->lpvTypeSpecificParams
					,DiEffect->cbTypeSpecificParams );
				Effect->DiEffect.lpvTypeSpecificParams	= &Effect->DiConstantForce;
				break;

			case CUSTOM_FORCE:
				Effect->DiEffect.lpvTypeSpecificParams	= NULL;
				break;

			case SQUARE:
			case SINE:
			case TRIANGLE:
			case SAWTOOTH_UP:
			case SAWTOOTH_DOWN:
				CopyMemory(
					 &Effect->DiPeriodic
					,DiEffect->lpvTypeSpecificParams
					,DiEffect->cbTypeSpecificParams );
				Effect->DiEffect.lpvTypeSpecificParams	= &Effect->DiPeriodic;
				break;

			case RAMP_FORCE:
				CopyMemory(
					 &Effect->DiRampforce
					,DiEffect->lpvTypeSpecificParams
					,DiEffect->cbTypeSpecificParams );
				Effect->DiEffect.lpvTypeSpecificParams	= &Effect->DiRampforce;
				break;
		}
	}

	//	DIEP_STARTDELAY ��ݒ肷��
	if( Flags & DIEP_STARTDELAY )
	{
		Effect->DiEffect.dwStartDelay	= DiEffect->dwStartDelay;
	}

	//	DIEP_START ���w�肳��Ă���΃G�t�F�N�g���Đ�����
	if( Flags & DIEP_START )
	{
		Effect->Status		= DIEGES_PLAYING;
		Effect->PlayCount	= 1;
		Effect->StartTime	= GetTickCount();
	}

	//	DIEP_NORESTART �͖�������
	if( Flags & DIEP_NORESTART )
	{
		;
	}

	//	�N���e�B�J�� �Z�N�V�������o��
	LeaveCriticalSection( &CriticalSection );

	return( S_OK );
}

//----------------------------------------------------------------------------------------------
//	DestroyEffect
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::DestroyEffect(
	 DWORD	ExternalID
	,DWORD	EffectHandle )
{
	//	�N���e�B�J�� �Z�N�V�����ɓ���
	EnterCriticalSection( &CriticalSection );

	//	�n���h������G�t�F�N�g����������
	for( LONG Index = 0; Index < EffectCount; Index ++ )
	{
		if( EffectList[Index]->Handle == EffectHandle )
		{
			//	�G�t�F�N�g���폜����
			delete EffectList[Index];
			//	�G�t�F�N�g�������炷
			EffectCount	--;
			if( EffectCount > 0 )
			{
				//	�G�t�F�N�g ���X�g���k������
				CopyMemory(
					 &EffectList[Index]
					,&EffectList[Index + 1]
					,sizeof( CEffect * ) * ( EffectCount - Index ) );
				CEffect * *	NewEffectList;
				NewEffectList	= (CEffect * *)realloc( EffectList, sizeof( CEffect * ) * EffectCount );
				if( NewEffectList != NULL )
				{
					EffectList	= NewEffectList;
				} else {
					//	�N���e�B�J�� �Z�N�V�������o��
					LeaveCriticalSection( &CriticalSection );
					//	�G���[��Ԃ�
					return( E_OUTOFMEMORY );
				}
			} else {
				//	�G�t�F�N�g ���X�g���폜����
				free( EffectList );
				EffectList	= NULL;
			}
			break;
		}
	}

	//	�N���e�B�J�� �Z�N�V�������o��
	LeaveCriticalSection( &CriticalSection );

	return( S_OK );
}

//----------------------------------------------------------------------------------------------
//	StartEffect
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::StartEffect(
	 DWORD	ExternalID
	,DWORD	EffectHandle
	,DWORD	Mode
	,DWORD	Count )
{
	//	�N���e�B�J�� �Z�N�V�����ɓ���
	EnterCriticalSection( &CriticalSection );

	//	�n���h������G�t�F�N�g����������
	for( LONG Index = 0; Index < EffectCount; Index ++ )
	{
		if( EffectList[Index]->Handle == EffectHandle )
		{
			//	�G�t�F�N�g�̍Đ����J�n����
			EffectList[Index]->Status		= DIEGES_PLAYING;
			EffectList[Index]->PlayCount	= Count;
			EffectList[Index]->StartTime	= GetTickCount();
			//	��~����������
			Stopped	= FALSE;
		} else {
			//	DIES_SOLO ���w�肳��Ă���ꍇ�͑��̃G�t�F�N�g���~����
			if( Mode & DIES_SOLO )
			{
				EffectList[Index]->Status	= NULL;
			}
		}
	}

	//	�N���e�B�J�� �Z�N�V�������o��
	LeaveCriticalSection( &CriticalSection );

	return( S_OK );
}

//----------------------------------------------------------------------------------------------
//	StopEffect
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::StopEffect(
	 DWORD	ExternalID
	,DWORD	EffectHandle )
{
	//	�N���e�B�J�� �Z�N�V�����ɓ���
	EnterCriticalSection( &CriticalSection );

	//	�n���h������G�t�F�N�g����������
	for( LONG Index = 0; Index < EffectCount; Index ++ )
	{
		if( EffectList[Index]->Handle == EffectHandle )
		{
			//	�G�t�F�N�g���~����
			EffectList[Index]->Status	= NULL;
			break;
		}
	}

	//	�N���e�B�J�� �Z�N�V�������o��
	LeaveCriticalSection( &CriticalSection );

	return( S_OK );
}

//----------------------------------------------------------------------------------------------
//	GetEffectStatus
//----------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CEffectDriver::GetEffectStatus(
	 DWORD		ExternalID
	,DWORD		EffectHandle
	,LPDWORD	Status )
{
	//	�N���e�B�J�� �Z�N�V�����ɓ���
	EnterCriticalSection( &CriticalSection );

	//	�n���h������G�t�F�N�g����������
	for( LONG Index = 0; Index < EffectCount; Index ++ )
	{
		if( EffectList[Index]->Handle == EffectHandle )
		{
			//	�G�t�F�N�g�̏�Ԃ�Ԃ�
			*Status	= EffectList[Index]->Status;
			break;
		}
	}

	//	�N���e�B�J�� �Z�N�V�������o��
	LeaveCriticalSection( &CriticalSection );

	return( S_OK );
}

//----------------------------------------------------------------------------------------------
//	EffectProc
//----------------------------------------------------------------------------------------------
STDAPI_(DWORD) WINAPI EffectProc( LPVOID EffectDriverInterface )
{
	//	�ϐ��錾
	HID_WRITE_REPORT_ID	Report;
	DWORD				ReqLen;

	//	�G�t�F�N�g �h���C�o���擾����
	CEffectDriver *	EffectDriver	= (CEffectDriver *)EffectDriverInterface;

	//	�O��̃G�t�F�N�g�̋���
	LONG	PrvLeftLevel	= -1;
	LONG	PrvRightLevel	= -1;

	while( EffectDriver->Quit == FALSE )
	{
		//	�N���e�B�J�� �Z�N�V�����ɓ���
		EnterCriticalSection( &CriticalSection );

		//	�G�t�F�N�g�̋���
		LONG	LeftLevel	= 0;
		LONG	RightLevel	= 0;
		LONG	Gain		= EffectDriver->Gain;

		if( EffectDriver->Actuator == TRUE )
		{
			//	�G�t�F�N�g�̋������v�Z����
			for( LONG Index = 0; Index < EffectDriver->EffectCount; Index ++ )
			{
				EffectDriver->EffectList[Index]->Calc( &LeftLevel, &RightLevel );
			}
		}

		//	�N���e�B�J�� �Z�N�V�������o��
		LeaveCriticalSection( &CriticalSection );

		//	�R���g���[���[�̐U����ݒ肷��
		if( PrvLeftLevel != LeftLevel || PrvRightLevel != RightLevel )
		{
			//	�f�o�C�X�փt�H�[�X�������o��
			Report.Id		= EffectDriver->ControllerId;
			Report.Type		= REPORT_TYPE_NORMAL;
			Report.Left		= (UCHAR)min( 255, ( LeftLevel * Gain / 10000 ) >> 8 );
			Report.Right	= (UCHAR)min( 255, ( LeftLevel * Gain / 10000 ) >> 8 );
			WriteFile(
				 EffectDriver->Device
				,&Report
				,sizeof( HID_WRITE_REPORT_ID )
				,&ReqLen
				,NULL );
		}

		//	�G�t�F�N�g�̋�����ޔ�����
		PrvLeftLevel	= LeftLevel;
		PrvRightLevel	= RightLevel;

		//	2ms �ҋ@����
		Sleep( 2 );
	}

	//	�f�o�C�X�փt�H�[�X�������o��
	Report.Id		= EffectDriver->ControllerId;
	Report.Type		= REPORT_TYPE_NORMAL;
	Report.Left		= 0;
	Report.Right	= 0;
	WriteFile( EffectDriver->Device, &Report, sizeof( HID_WRITE_REPORT_ID ), &ReqLen, NULL );

	return( 0 );
}

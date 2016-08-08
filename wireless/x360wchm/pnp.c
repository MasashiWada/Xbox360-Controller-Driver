
//	HID Minidriver for Microsoft Xbox 360 Wireless Controller

#include	"driver.h"

//----------------------------------------------------------------------------------------------
//	定数
//----------------------------------------------------------------------------------------------

#define	INTERVAL					-1
#define	INTERVAL_WAIT				-4 * 1000 * 10
#define	INTERVAL_STALL				-16 * 1000 * 10

//----------------------------------------------------------------------------------------------
//	関数
//----------------------------------------------------------------------------------------------

NTSTATUS StartDevice( IN PDEVICE_OBJECT, IN PDEVICE_EXTENSION, IN PIRP );
NTSTATUS QueryCapabilities( IN PDEVICE_OBJECT, IN PIRP, IN PIO_STACK_LOCATION );
NTSTATUS SurpriseRemoval( IN PDEVICE_OBJECT, IN PDEVICE_EXTENSION, IN PIRP );
NTSTATUS RemoveDevice( IN PDEVICE_OBJECT, IN PDEVICE_EXTENSION, IN PIRP );
KSTART_ROUTINE WorkerThreadIn;
KSTART_ROUTINE WorkerThreadOut;
KSTART_ROUTINE WorkerThreadIdc;

//----------------------------------------------------------------------------------------------
//	PnP
//----------------------------------------------------------------------------------------------
NTSTATUS PnP(
	 IN PDEVICE_OBJECT	DeviceObject
	,IN PIRP			Irp )
{
	//	変数宣言
	NTSTATUS			Status;
	PDEVICE_EXTENSION	DeviceExtension	= GET_MINIDRIVER_DEVICE_EXTENSION( DeviceObject );
	PIO_STACK_LOCATION	CurrentIrpStack;

	//	参照カウンタを加算する
	Status	= IncrementReference( DeviceExtension );
	if( !NT_SUCCESS( Status ) )
	{
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	マイナー ファンクションによって処理を振り分ける
	CurrentIrpStack	= IoGetCurrentIrpStackLocation( Irp );
	switch( CurrentIrpStack->MinorFunction )
	{
		case IRP_MN_START_DEVICE:
			Status	= StartDevice( DeviceObject, DeviceExtension, Irp );
			break;

		case IRP_MN_QUERY_CAPABILITIES:
			Status	= QueryCapabilities( DeviceObject, Irp, CurrentIrpStack );
			break;

		case IRP_MN_SURPRISE_REMOVAL:
			Status	= SurpriseRemoval( DeviceObject, DeviceExtension, Irp );
			break;

		case IRP_MN_REMOVE_DEVICE:
			Status	= RemoveDevice( DeviceObject, DeviceExtension, Irp );
			break;

		default:
			//	IRP_MJ_PNP 要求を下位ドライバに送る
			Status	= SendRequest( DeviceObject, Irp );
			break;
	}

	//	参照カウンタを減算する
	DecrementReference( DeviceExtension );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	StartDevice
//----------------------------------------------------------------------------------------------
NTSTATUS StartDevice(
	 IN PDEVICE_OBJECT		DeviceObject
	,IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp )
{
	//	変数宣言
	NTSTATUS	Status;

	//	先に IRP_MN_START_DEVICE 要求を下位ドライバに送る
	Status	= SendAndWaitRequest( DeviceObject, Irp );
	if( !NT_SUCCESS( Status ) )
	{
		CompleteRequest( Irp, Status, 0 );
		return( Status );
	}

	//	デバイスを有効にする
	Status	= EnableDevice( DeviceExtension );

	//	IRP_MN_START_DEVICE 要求を完了させる
	CompleteRequest( Irp, Status, 0 );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	QueryCapabilities
//----------------------------------------------------------------------------------------------
NTSTATUS QueryCapabilities(
	 IN PDEVICE_OBJECT		DeviceObject
	,IN PIRP				Irp
	,IN PIO_STACK_LOCATION	CurrentIrpStack )
{
	//	変数宣言
	NTSTATUS				Status;
	PDEVICE_CAPABILITIES	DeviceCapabilities;

	//	先に IRP_MN_QUERY_CAPABILITIES 要求を下位ドライバに送る
	Status	= SendAndWaitRequest( DeviceObject, Irp );
	if( !NT_SUCCESS( Status ) )
	{
		return( Status );
	}

	//	下位ドライバで設定されたデバイスの機能を変更する
	DeviceCapabilities	= CurrentIrpStack->Parameters.DeviceCapabilities.Capabilities;
	DeviceCapabilities->SurpriseRemovalOK	= TRUE;

	//	IRP_MN_QUERY_CAPABILITIES 要求を完了させる
	CompleteRequest( Irp, Status, 0 );

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	SurpriseRemoval
//----------------------------------------------------------------------------------------------
NTSTATUS SurpriseRemoval(
	 IN PDEVICE_OBJECT		DeviceObject
	,IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp )
{
	//	デバイスを無効にする
	DisableDevice( DeviceExtension );

	//	IRP_MN_SURPRISE_REMOVAL 要求を下位ドライバに送る
	return( SendRequest( DeviceObject, Irp ) );
}

//----------------------------------------------------------------------------------------------
//	RemoveDevice
//----------------------------------------------------------------------------------------------
NTSTATUS RemoveDevice(
	 IN PDEVICE_OBJECT		DeviceObject
	,IN PDEVICE_EXTENSION	DeviceExtension
	,IN PIRP				Irp )
{
	//	デバイスを無効にする
	DisableDevice( DeviceExtension );

	//	IRP_MN_REMOVE_DEVICE 要求を下位ドライバに送る
	return( SendRequest( DeviceObject, Irp ) );
}

//----------------------------------------------------------------------------------------------
//	EnableDevice
//----------------------------------------------------------------------------------------------
NTSTATUS EnableDevice( IN PDEVICE_EXTENSION DeviceExtension )
{
	//	変数宣言
	NTSTATUS	Status;
	HANDLE		ThreadHandle;
	UCHAR		ThreadId;

	//	デバイスを有効にする
	if( DeviceExtension->DeviceState == Disable )
	{
		//	USB ポートを初期化する
		Status	= SetupPort( DeviceExtension->NextDeviceObject );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	デバイス記述子を取得する
		Status	= GetDeviceDescriptor(
					 DeviceExtension->NextDeviceObject
					,&DeviceExtension->DeviceDescriptor );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	コンフィギュレーション記述子を取得する
		Status	= GetConfigurationDescriptor(
					 DeviceExtension->NextDeviceObject
					,&DeviceExtension->ConfigurationDescriptor );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	インターフェイス記述子を取得する
		Status	= GetInterfaceDescriptor(
					 DeviceExtension->ConfigurationDescriptor
					,&DeviceExtension->InterfaceDescriptor );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	コンフィギュレーションを選択する
		Status	= SelectConfiguration(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->ConfigurationDescriptor
					,DeviceExtension->InterfaceDescriptor
					,DeviceExtension->InputPipeHandle
					,DeviceExtension->OutputPipeHandle );
		if( !NT_SUCCESS( Status ) )
		{
			return( Status );
		}

		//	設定を読み込ませる
		for( ThreadId = 0; ThreadId < ControllerCount; ThreadId ++ )
		{
			DeviceExtension->ResetDevice[ThreadId]	= TRUE;
		}

		//	デバイスの状態を有効にする
		DeviceExtension->DeviceState	= Enable;
	}

	//	ワーカ スレッドを開始する
	if( DeviceExtension->ThreadStateIo == Disable )
	{
		DeviceExtension->ThreadStateIo	= Enable;
		for( ThreadId = 0; ThreadId < ControllerCount; ThreadId ++ )
		{
			//	スレッド パラメータを設定する
			DeviceExtension->ThreadParam[ThreadId].ThreadId			= ThreadId;
			DeviceExtension->ThreadParam[ThreadId].DeviceExtension	= DeviceExtension;
			//	入力用ワーカ スレッドを開始する
			if( DeviceExtension->InputPipeHandle[ThreadId] != NULL )
			{
				//	入力用ワーカ スレッドを開始する
				PsCreateSystemThread(
					 &ThreadHandle
					,THREAD_ALL_ACCESS
					,NULL
					,NULL
					,NULL
					,WorkerThreadIn
					,&DeviceExtension->ThreadParam[ThreadId] );
				ObReferenceObjectByHandle(
					 ThreadHandle
					,THREAD_ALL_ACCESS
					,NULL
					,KernelMode
					,&DeviceExtension->ThreadObjectIn[ThreadId]
					,NULL );
				ZwClose( ThreadHandle );
			}
			//	出力用ワーカ スレッドを開始する
			if( DeviceExtension->OutputPipeHandle[ThreadId] != NULL )
			{
				PsCreateSystemThread(
					 &ThreadHandle
					,THREAD_ALL_ACCESS
					,NULL
					,NULL
					,NULL
					,WorkerThreadOut
					,&DeviceExtension->ThreadParam[ThreadId] );
				ObReferenceObjectByHandle(
					 ThreadHandle
					,THREAD_ALL_ACCESS
					,NULL
					,KernelMode
					,&DeviceExtension->ThreadObjectOut[ThreadId]
					,NULL );
				ZwClose( ThreadHandle );
			}
		}
	}

	//	IRP 処理用ワーカ スレッドを開始する
	if( DeviceExtension->ThreadStateIdc == Disable )
	{
		DeviceExtension->ThreadStateIdc	= Enable;
		PsCreateSystemThread(
			 &ThreadHandle
			,THREAD_ALL_ACCESS
			,NULL
			,NULL
			,NULL
			,WorkerThreadIdc
			,DeviceExtension );
		ObReferenceObjectByHandle(
			 ThreadHandle
			,THREAD_ALL_ACCESS
			,NULL
			,KernelMode
			,&DeviceExtension->ThreadObjectIdc
			,NULL );
		ZwClose( ThreadHandle );
	}

	return( Status );
}

//----------------------------------------------------------------------------------------------
//	WorkerThreadIn
//----------------------------------------------------------------------------------------------
VOID WorkerThreadIn( IN PVOID Context )
{
	//	変数宣言
	NTSTATUS			Status;
	PTHREAD_PARAM		ThreadParam		= Context;
	PDEVICE_EXTENSION	DeviceExtension	= ThreadParam->DeviceExtension;
	UCHAR				ThreadId		= ThreadParam->ThreadId;
	SETTING				Setting;
	KIRQL				Irql;
	INPUT_REPORT		ControllerReport;
	LARGE_INTEGER		TimeOut;

	//	スレッドの優先順位を下げる
	KeSetPriorityThread( KeGetCurrentThread(), LOW_REALTIME_PRIORITY );

	for(;;)
	{
		//	スレッドの終了をチェックする
		if( DeviceExtension->ThreadStateIo == Disable )
		{
			break;
		}

		//	コントローラーの設定を読み込む
		if(	DeviceExtension->ResetDevice[ThreadId]		== TRUE
		&&	DeviceExtension->OutputPipeHandle[ThreadId]	== NULL )
		{
			//	設定を読み込む
			LoadSetting( &Setting );

			//	入力レポートからの割り込みを禁止する
			KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

			//	設定を反映する
			if( DeviceExtension->ResetDevice[ThreadId] == TRUE )
			{
				RtlCopyMemory( &DeviceExtension->Setting, &Setting, sizeof( SETTING ) );
				DeviceExtension->ResetDevice[ThreadId]	= FALSE;
			}

			//	入力レポートからの割り込みを許可する
			KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );
		}

		//	コントローラー レポートの入力を行う
		Status	= InputReport(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->InputPipeHandle[ThreadId]
					,&ControllerReport
					,sizeof( INPUT_REPORT ) );
		if( NT_SUCCESS( Status ) )
		{
			//	コントローラーが接続された場合
			if(	ControllerReport.Type		== 0x08
			&&	ControllerReport.ParamType	== 0x80 )
			{
				DeviceExtension->Connected[ThreadId]			= TRUE;
				DeviceExtension->ConnectionReported[ThreadId]	= TRUE;
				DeviceExtension->ResetDevice[ThreadId]			= TRUE;
			}
			//	コントローラーが切断された場合
			if(	ControllerReport.Type		== 0x08
			&&	ControllerReport.ParamType	== 0x00 )
			{
				DeviceExtension->Connected[ThreadId]			= FALSE;
				DeviceExtension->ConnectionReported[ThreadId]	= TRUE;
			}
			//	入力用レポートを取得した場合
			if(	ControllerReport.ParamType				== 0x01
			&&	ControllerReport.Param1					== 0xF0
			&&	DeviceExtension->Connected[ThreadId]	== TRUE )
			{
				//	入力レポートからの割り込みを禁止する
				KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

				//	コントローラー レポートを反映する
				RtlCopyMemory(
					 &DeviceExtension->ControllerReport[ThreadId]
					,&ControllerReport
					,sizeof( INPUT_REPORT ) );

				//	入力レポートからの割り込みを許可する
				KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );

				//	レポートされた時刻を退避する
				KeQuerySystemTime( &DeviceExtension->ControllerReportTime[ThreadId] );
			}
			TimeOut.QuadPart	= INTERVAL;
		} else {
			//	エラーが発生したらパイプをリセットする
			if( Status != STATUS_CANCELLED && Status != STATUS_DEVICE_NOT_CONNECTED )
			{
				ResetPipe(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->InputPipeHandle[ThreadId] );
			}
			TimeOut.QuadPart	= INTERVAL_STALL;
		}
		//	待機する
		KeDelayExecutionThread( KernelMode, FALSE, &TimeOut );
	}

	//	スレッドを開放する
	ObDereferenceObject( DeviceExtension->ThreadObjectIn[ThreadId] );
	DeviceExtension->ThreadObjectIn[ThreadId]	= NULL;

	//	スレッドが終了したことを通知する
	KeSetEvent( &DeviceExtension->ThreadExitingIn[ThreadId], 0 ,FALSE );

	//	スレッドを終了する
	PsTerminateSystemThread( STATUS_SUCCESS );
}

//----------------------------------------------------------------------------------------------
//	WorkerThreadOut
//----------------------------------------------------------------------------------------------
VOID WorkerThreadOut( IN PVOID Context )
{
	//	変数宣言
	NTSTATUS			Status;
	PTHREAD_PARAM		ThreadParam			= Context;
	PDEVICE_EXTENSION	DeviceExtension		= ThreadParam->DeviceExtension;
	UCHAR				ThreadId			= ThreadParam->ThreadId;
	SETTING				Setting;
	KIRQL				Irql;
	POUTPUT_REPORT		ConnectionReport	= &DeviceExtension->ConnectionReport[ThreadId];
	POUTPUT_REPORT		RingLightReport		= &DeviceExtension->RingLightReport[ThreadId];
	POUTPUT_REPORT		ActuatorsReport		= &DeviceExtension->ActuatorsReport[ThreadId];
	OUTPUT_REPORT		PrevActuatorsReport	= *ActuatorsReport;
	LONG				ReportCount			= 0;
	LARGE_INTEGER		TimeOut;

	//	スレッドの優先順位を下げる
	KeSetPriorityThread( KeGetCurrentThread(), LOW_REALTIME_PRIORITY );

	for(;;)
	{
		//	スレッドの終了をチェックする
		if( DeviceExtension->ThreadStateIo == Disable )
		{
			break;
		}

		//	コントローラーの設定を読み込む
		if( DeviceExtension->ResetDevice[ThreadId] == TRUE )
		{
			//	設定を読み込む
			LoadSetting( &Setting );

			//	入力レポートからの割り込みを禁止する
			KeAcquireSpinLock( &DeviceExtension->LockReport, &Irql );

			//	設定を反映する
			if( DeviceExtension->ResetDevice[ThreadId] == TRUE )
			{
				RtlCopyMemory( &DeviceExtension->Setting, &Setting, sizeof( SETTING ) );
				DeviceExtension->ActuatorsReported[ThreadId]	= FALSE;
				DeviceExtension->RingLightReported[ThreadId]	= FALSE;
				DeviceExtension->ResetDevice[ThreadId]			= FALSE;
			}

			//	入力レポートからの割り込みを許可する
			KeReleaseSpinLock( &DeviceExtension->LockReport, Irql );
		}

		//	コントローラーの接続確認レポートの出力を行う
		if( DeviceExtension->ConnectionReported[ThreadId] == FALSE )
		{
			//	コントローラーの接続確認レポートを作成する
			ConnectionReport->Type		= 0x08;
			ConnectionReport->ParamType	= 0x00;
			ConnectionReport->Param1	= 0x0F;
			ConnectionReport->Param2	= 0xC0;
			//	コントローラーの接続確認レポートを出力する
			Status	= OutputReport(
						 DeviceExtension->NextDeviceObject
						,DeviceExtension->OutputPipeHandle[ThreadId]
						,&DeviceExtension->ConnectionReport[ThreadId]
						,sizeof( OUTPUT_REPORT ) );
			if( NT_SUCCESS( Status ) )
			{
				;
			}
		}

		//	リング ライト レポートの出力を行う
		if(	DeviceExtension->RingLightReported[ThreadId]	== FALSE
		&&	DeviceExtension->Connected[ThreadId]			== TRUE )
		{
			//	リング ライト レポートを作成する
			RingLightReport->Type		= 0x00;
			RingLightReport->ParamType	= 0x00;
			RingLightReport->Param1		= 0x08;
			if( DeviceExtension->Setting.RingLightPattern == 16 )
			{
				RingLightReport->Param2	= 0x42 + ThreadId;
			} else {
				RingLightReport->Param2	= 0x40
											+ (UCHAR)DeviceExtension->Setting.RingLightPattern;
			}
			//	リング ライト レポートを出力する
			Status	= OutputReport(
						 DeviceExtension->NextDeviceObject
						,DeviceExtension->OutputPipeHandle[ThreadId]
						,&DeviceExtension->RingLightReport[ThreadId]
						,sizeof( OUTPUT_REPORT ) );
			if( NT_SUCCESS( Status ) )
			{
				DeviceExtension->RingLightReported[ThreadId]	= TRUE;
			}
		}

		//	アクチュエータ レポートを作成する
		if( DeviceExtension->Connected[ThreadId] == TRUE )
		{
			ActuatorsReport->Type		= 0x00;
			ActuatorsReport->ParamType	= 0x01;
			ActuatorsReport->Param1		= 0x0F;
			ActuatorsReport->Param2		= 0xC0;
			if( DeviceExtension->ActuatorsReported[ThreadId] == FALSE )
			{
				ActuatorsReport->LeftActuator	= 0;
				ActuatorsReport->RightActuator	= 0;
			}

			//	同一の値でレポートした回数を初期化する
			if(	ActuatorsReport->LeftActuator		!= PrevActuatorsReport.LeftActuator
			||	ActuatorsReport->RightActuator		!= PrevActuatorsReport.RightActuator
			||	DeviceExtension->ActuatorsReported[ThreadId]	== FALSE )
			{
				ReportCount	= 0;
			}

			//	アクチュエータ レポートを出力する
			if( ReportCount < 250 )
			{
				Status	= OutputReport(
							 DeviceExtension->NextDeviceObject
							,DeviceExtension->OutputPipeHandle[ThreadId]
							,&DeviceExtension->ActuatorsReport[ThreadId]
							,sizeof( OUTPUT_REPORT ) );
				if( NT_SUCCESS( Status ) )
				{
					//	レポートの内容を退避する
					PrevActuatorsReport								= *ActuatorsReport;
					ReportCount ++;
					DeviceExtension->ActuatorsReported[ThreadId]	= TRUE;
					TimeOut.QuadPart								= INTERVAL;
				} else {
					//	エラーが発生したらパイプをリセットする
					if( Status != STATUS_CANCELLED && Status != STATUS_DEVICE_NOT_CONNECTED )
					{
						ResetPipe(
							 DeviceExtension->NextDeviceObject
							,DeviceExtension->OutputPipeHandle[ThreadId] );
					}
					TimeOut.QuadPart	= INTERVAL_STALL;
				}
			} else {
				TimeOut.QuadPart	= INTERVAL_WAIT;
			}
		} else {
			TimeOut.QuadPart	= INTERVAL_STALL;
		}
		//	待機する
		KeDelayExecutionThread( KernelMode, FALSE, &TimeOut );
	}

	//	スレッドを開放する
	ObDereferenceObject( DeviceExtension->ThreadObjectOut[ThreadId] );
	DeviceExtension->ThreadObjectOut[ThreadId]	= NULL;

	//	スレッドが終了したことを通知する
	KeSetEvent( &DeviceExtension->ThreadExitingOut[ThreadId], 0 ,FALSE );

	//	スレッドを終了する
	PsTerminateSystemThread( STATUS_SUCCESS );
}

//----------------------------------------------------------------------------------------------
//	WorkerThreadIdc
//----------------------------------------------------------------------------------------------
VOID WorkerThreadIdc( IN PVOID Context )
{
	//	変数宣言
	NTSTATUS			Status;
	PDEVICE_EXTENSION	DeviceExtension	= Context;
	KIRQL				Irql;
	PIRP				Irp;
	PIRP *				Irps;
	LARGE_INTEGER		TimeOut;

	//	スレッドの優先順位を下げる
	KeSetPriorityThread( KeGetCurrentThread(), LOW_REALTIME_PRIORITY );

	for(;;)
	{
		//	スレッドの終了をチェックする（キューに IRP が残っている場合は全て処理する）
		if( DeviceExtension->ThreadStateIdc == Disable && DeviceExtension->IrpCount == 0 )
		{
			break;
		}

		//	排他制御を行う
		KeAcquireSpinLock( &DeviceExtension->LockIrp, &Irql );

		//	キューから IRP を取得する
		if( DeviceExtension->IrpCount > 0 )
		{
			//	キューから IRP を取得する
			Irp	= DeviceExtension->Irps[0];
			//	取得した IRP をキューから取り除く
			if( DeviceExtension->IrpCount > 1 )
			{
				Irps	= (PIRP *)ExAllocatePoolWithTag(
							 NonPagedPool
							,(DeviceExtension->IrpCount - 1) * sizeof( PIRP )
							,'300T' );
				RtlCopyMemory(
					 Irps
					,&DeviceExtension->Irps[1]
					,(DeviceExtension->IrpCount - 1) * sizeof( PIRP ) );
			}
			if( DeviceExtension->Irps != NULL )
			{
				ExFreePool( DeviceExtension->Irps );
				DeviceExtension->Irps = NULL;
			}
			//	新しいキューを設定する
			DeviceExtension->Irps	= Irps;
			DeviceExtension->IrpCount --;
		} else {
			Irp	= NULL;
		}

		//	排他制御を解除する
		KeReleaseSpinLock( &DeviceExtension->LockIrp, Irql );

		//	IRP を処理する
		if( Irp != NULL )
		{
			ReadReport( Irp, DeviceExtension );
		} else {
			//	待機する
			TimeOut.QuadPart	= INTERVAL;
			KeDelayExecutionThread( KernelMode, FALSE, &TimeOut );
		}
	}

	//	スレッドを開放する
	ObDereferenceObject( DeviceExtension->ThreadObjectIdc );
	DeviceExtension->ThreadObjectIdc	= NULL;

	//	スレッドが終了したことを通知する
	KeSetEvent( &DeviceExtension->ThreadExitingIdc, 0 ,FALSE );

	//	スレッドを終了する
	PsTerminateSystemThread( STATUS_SUCCESS );
}

//----------------------------------------------------------------------------------------------
//	DisableDevice
//----------------------------------------------------------------------------------------------
NTSTATUS DisableDevice( IN PDEVICE_EXTENSION DeviceExtension )
{
	//	変数宣言
	UCHAR	ThreadId;

	//	ワーカ スレッドが終了するのを待つ
	if( DeviceExtension->ThreadStateIo == Enable )
	{
		//	ワーカ スレッドを終了する
		DeviceExtension->ThreadStateIo	= Disable;
		for( ThreadId = 0; ThreadId < ControllerCount; ThreadId ++ )
		{
			if( DeviceExtension->InputPipeHandle[ThreadId] != NULL )
			{
				//	入力用パイプをアボートする
				AbortPipe(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->InputPipeHandle[ThreadId] );
				//	入力用ワーカ スレッドが完了するのを待つ
				KeWaitForSingleObject(
					 &DeviceExtension->ThreadExitingIn[ThreadId]
					,Executive
					,KernelMode
					,FALSE
					,NULL );
			}
			if( DeviceExtension->OutputPipeHandle[ThreadId] != NULL )
			{
				//	出力用パイプをアボートする
				AbortPipe(
					 DeviceExtension->NextDeviceObject
					,DeviceExtension->OutputPipeHandle[ThreadId] );
				//	出力用ワーカ スレッドが完了するのを待つ
				KeWaitForSingleObject(
					 &DeviceExtension->ThreadExitingOut[ThreadId]
					,Executive
					,KernelMode
					,FALSE
					,NULL );
			}
		}
	}

	//	ドライバの状態を無効にする
	DeviceExtension->DriverState	= Disable;

	//	IRP 処理用ワーカ スレッドが完了するのを待つ
	if( DeviceExtension->ThreadStateIdc == Enable )
	{
		//	IRP 処理用ワーカ スレッドを終了する
		DeviceExtension->ThreadStateIdc	= Disable;

		//	IRP 処理用ワーカ スレッドが完了するのを待つ
		KeWaitForSingleObject(
			 &DeviceExtension->ThreadExitingIdc
			,Executive
			,KernelMode
			,FALSE
			,NULL );
	}

	//	参照終了イベントを初期化する
	KeResetEvent( &DeviceExtension->ReferenceEnd );

	//	参照カウンタを減算する
	DecrementReference( DeviceExtension );
	DecrementReference( DeviceExtension );

	//	全ての I/O 要求が終了し、参照終了が通知されるまで待機する
	KeWaitForSingleObject( &DeviceExtension->ReferenceEnd, Executive, KernelMode, FALSE, NULL);

	//	デバイスを無効にする
	if( DeviceExtension->DeviceState == Enable )
	{
		//	コンフィギュレーションの選択を解除する
		DeselectConfiguration( DeviceExtension->NextDeviceObject );

		//	コンフィギュレーション記述子用バッファが確保されていれば開放する
		if( DeviceExtension->ConfigurationDescriptor != NULL )
		{
			ExFreePool( DeviceExtension->ConfigurationDescriptor );
			DeviceExtension->ConfigurationDescriptor	= NULL;
		}

		//	インターフェイス記述子用バッファが確保されていれば開放する
		if( DeviceExtension->InterfaceDescriptor != NULL )
		{
			ExFreePool( DeviceExtension->InterfaceDescriptor );
			DeviceExtension->InterfaceDescriptor	= NULL;
		}

		//	デバイスの状態を無効にする
		DeviceExtension->DeviceState	= Disable;
	}

	//	ドライバの状態を有効にする
	DeviceExtension->DriverState	= Enable;

	//	参照カウンタの数を合わせる
	IncrementReference( DeviceExtension );
	IncrementReference( DeviceExtension );

	return( STATUS_SUCCESS );
}

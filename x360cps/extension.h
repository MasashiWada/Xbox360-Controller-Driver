
//	Property Sheet for Microsoft Xbox 360 Controller

#pragma	once

#include	<windows.h>

//----------------------------------------------------------------------------------------------
//	GUID
//----------------------------------------------------------------------------------------------

DEFINE_GUID( ClassID_X360CPS,
	0xFFB0C361, 0x5623, 0x49AA, 0xBD, 0x51, 0xB3, 0x21, 0xDB, 0x96, 0x25, 0xCE );

//----------------------------------------------------------------------------------------------
//	グローバル変数
//----------------------------------------------------------------------------------------------

extern	HINSTANCE			Instance;
extern	CRITICAL_SECTION	CriticalSection;


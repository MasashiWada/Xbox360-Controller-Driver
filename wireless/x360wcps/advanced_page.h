
//	Property Sheet for Microsoft Xbox 360 Wireless Controller

#pragma	once

#include	"page.h"

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CAdvancedPage : public CPage
{

public:

	//	�R���X�g���N�^
	CAdvancedPage( CSetting * );

	//	�_�C�A���O������������
	VOID wmInitDialog( HWND );
	//	�R�}���h����������
	VOID wmCommand( HWND, WORD, WORD );

	//	�ݒ�𔽉f����
	VOID ApplySetting();

private:

protected:

};

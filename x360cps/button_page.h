
//	Property Sheet for Microsoft Xbox 360 Controller

#pragma	once

#include	"page.h"

//----------------------------------------------------------------------------------------------
//	�N���X
//----------------------------------------------------------------------------------------------

class CButtonPage : public CPage
{

public:

	//	�R���X�g���N�^
	CButtonPage( CSetting * );

	//	�_�C�A���O������������
	VOID wmInitDialog( HWND );
	//	�R�}���h����������
	VOID wmCommand( HWND, WORD, WORD );
	//	�����X�N���[������������
	VOID wmHScroll( HWND, WORD );

	//	�ݒ�𔽉f����
	VOID ApplySetting();

private:

protected:

};

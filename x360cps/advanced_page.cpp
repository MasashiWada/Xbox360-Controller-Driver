
//	Property Sheet for Microsoft Xbox 360 Controller

#include	"advanced_page.h"
#include	"resource.h"

//----------------------------------------------------------------------------------------------
//	CAdvancedPage
//----------------------------------------------------------------------------------------------
CAdvancedPage::CAdvancedPage( CSetting * Setting )
{
	//	�ݒ�
	this->Setting		= Setting;

	//	���݂̃R���g���[���[�̐ݒ�
	CurrentSetting		= &Setting->CurrentSetting;
}

//----------------------------------------------------------------------------------------------
//	wmInitDialog
//----------------------------------------------------------------------------------------------
VOID CAdvancedPage::wmInitDialog( HWND Wnd )
{
	//	�E�B���h�E �n���h��
	this->Wnd	= Wnd;

	//	�@�\�̈ꗗ
	WORD	SliderList[]	= {	 IDS_Z_AXIS
								,IDS_Z_ROTATION
								,IDS_SLIDER1
								,IDS_SLIDER2 };
	WORD	SliderListCount	= sizeof( SliderList ) / sizeof( WORD ) ;

	//	�݊������[�h
	WORD	LimitationMode[]	= {	 IDS_LIMITATION_MODE_0
									,IDS_LIMITATION_MODE_1
									,IDS_LIMITATION_MODE_2
									,IDS_LIMITATION_MODE_3 };
	WORD	LimitationModeCount	= sizeof( LimitationMode ) / sizeof( WORD ) ;

	//	�u�@�\�̈ꗗ�v
	InitListBox(
		 IDC_SLIDER_LIST
		,SliderList
		,SliderListCount );
	SetListBoxIndex( IDC_SLIDER_LIST, 0 );
	//	�u�݊������[�h�v
	InitComboBox(
		 IDC_LIMITATION_MODE
		,LimitationMode
		,LimitationModeCount );
}

//----------------------------------------------------------------------------------------------
//	wmCommand
//----------------------------------------------------------------------------------------------
VOID CAdvancedPage::wmCommand(
	 HWND	Control
	,WORD	ControlID
	,WORD	NotifyMessage )
{
	//	�R���g���[���ɂ���ď�����U�蕪����
	switch( ControlID )
	{
		case IDC_SLIDER_LIST:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case LBN_SELCHANGE:
					ApplySetting();
					break;
			}
			break;

		case IDC_SLIDE_SLIDER_VALUE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->SlideSliderValue[GetListBoxIndex( IDC_SLIDER_LIST )]
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_SLIDE_SLIDER_CENTER_VALUE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->SlideSliderCenterValue[GetListBoxIndex( IDC_SLIDER_LIST )]
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_REVERSE_SLIDER_VALUE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->ReverseSliderValue[GetListBoxIndex( IDC_SLIDER_LIST )]
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_LIMITATION_MODE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case CBN_SELCHANGE:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->LimitationMode	= GetComboBoxIndex( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_STICK_DEAD_ZONE_TYPE_SQUARE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->StickDeadZoneType[INDEX_LEFT_STICK]	= DEAD_ZONE_TYPE_SQUARE;
					CurrentSetting->StickDeadZoneType[INDEX_RIGHT_STICK]
						= CurrentSetting->StickDeadZoneType[INDEX_LEFT_STICK];
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_STICK_DEAD_ZONE_TYPE_CIRCLE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->StickDeadZoneType[INDEX_LEFT_STICK]	= DEAD_ZONE_TYPE_CIRCLE;
					CurrentSetting->StickDeadZoneType[INDEX_RIGHT_STICK]
						= CurrentSetting->StickDeadZoneType[INDEX_LEFT_STICK];
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_TRANSFORM_TYPE_SQUARE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					if( GetCheckBoxState( ControlID ) == BST_CHECKED )
					{
						CurrentSetting->StickTransformType[INDEX_LEFT_STICK]
							= TRANSFORM_TYPE_SQUARE;
						CurrentSetting->StickTransformType[INDEX_RIGHT_STICK]
							= TRANSFORM_TYPE_SQUARE;
						SetCheckBoxState( IDC_TRANSFORM_TYPE_CIRCLE, BST_UNCHECKED );
					} else {
						CurrentSetting->StickTransformType[INDEX_LEFT_STICK]
							= TRANSFORM_TYPE_NONE;
						CurrentSetting->StickTransformType[INDEX_RIGHT_STICK]
							= TRANSFORM_TYPE_NONE;
					}
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_TRANSFORM_TYPE_CIRCLE:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					if( GetCheckBoxState( ControlID ) == BST_CHECKED )
					{
						CurrentSetting->StickTransformType[INDEX_LEFT_STICK]
							= TRANSFORM_TYPE_CIRCLE;
						CurrentSetting->StickTransformType[INDEX_RIGHT_STICK]
							= TRANSFORM_TYPE_CIRCLE;
						SetCheckBoxState( IDC_TRANSFORM_TYPE_SQUARE, BST_UNCHECKED );
					} else {
						CurrentSetting->StickTransformType[INDEX_LEFT_STICK]
							= TRANSFORM_TYPE_NONE;
						CurrentSetting->StickTransformType[INDEX_RIGHT_STICK]
							= TRANSFORM_TYPE_NONE;
					}
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;

		case IDC_DEVICES_ARE_POLLED:

			//	�ʒm���b�Z�[�W�ɂ���ď�����U�蕪����
			switch( NotifyMessage )
			{
				case BN_CLICKED:
					//	���݂̐ݒ�ɔ��f����
					CurrentSetting->DevicesArePolled
						= GetCheckBoxState( ControlID );
					//	�ݒ�̕ύX��ʒm����
					SettingChanged();
					break;
			}
			break;
	}
}

//----------------------------------------------------------------------------------------------
//	ApplySetting
//----------------------------------------------------------------------------------------------
VOID CAdvancedPage::ApplySetting()
{
	//	�u�X�e�B�b�N�̗L���͈́v
	if( CurrentSetting->StickDeadZoneType[INDEX_LEFT_STICK] == DEAD_ZONE_TYPE_SQUARE )
	{
		SetCheckBoxState( IDC_STICK_DEAD_ZONE_TYPE_SQUARE, BST_CHECKED );
		SetCheckBoxState( IDC_STICK_DEAD_ZONE_TYPE_CIRCLE, BST_UNCHECKED );
	} else {
		SetCheckBoxState( IDC_STICK_DEAD_ZONE_TYPE_SQUARE, BST_UNCHECKED );
		SetCheckBoxState( IDC_STICK_DEAD_ZONE_TYPE_CIRCLE, BST_CHECKED );
	}

	//	�u�X�e�B�b�N�̉��͈́v
	if( CurrentSetting->StickTransformType[INDEX_LEFT_STICK] == TRANSFORM_TYPE_NONE )
	{
		SetCheckBoxState( IDC_TRANSFORM_TYPE_SQUARE, BST_UNCHECKED );
		SetCheckBoxState( IDC_TRANSFORM_TYPE_CIRCLE, BST_UNCHECKED );
	}
	if( CurrentSetting->StickTransformType[INDEX_LEFT_STICK] == TRANSFORM_TYPE_SQUARE )
	{
		SetCheckBoxState( IDC_TRANSFORM_TYPE_SQUARE, BST_CHECKED );
		SetCheckBoxState( IDC_TRANSFORM_TYPE_CIRCLE, BST_UNCHECKED );
	}
	if( CurrentSetting->StickTransformType[INDEX_LEFT_STICK] == TRANSFORM_TYPE_CIRCLE )
	{
		SetCheckBoxState( IDC_TRANSFORM_TYPE_SQUARE, BST_UNCHECKED );
		SetCheckBoxState( IDC_TRANSFORM_TYPE_CIRCLE, BST_CHECKED );
	}

	//	�u�@�\�̈ꗗ�v
	LONG	ListBoxIndex	= GetListBoxIndex( IDC_SLIDER_LIST );
	//	�u���S����ɂ���v
	SetCheckBoxState( IDC_SLIDE_SLIDER_VALUE, CurrentSetting->SlideSliderValue[ListBoxIndex] );
	//	�u���S�̊�l�����炷�v
	SetCheckBoxState(
		 IDC_SLIDE_SLIDER_CENTER_VALUE
		,CurrentSetting->SlideSliderCenterValue[ListBoxIndex] );
	//	�u�l�𔽓]����v
	SetCheckBoxState(
		 IDC_REVERSE_SLIDER_VALUE
		,CurrentSetting->ReverseSliderValue[ListBoxIndex] );

	//	�u�݊������[�h�v
	SetComboBoxIndex( IDC_LIMITATION_MODE, CurrentSetting->LimitationMode );

	//	�u�|�[�����O���s���v
	SetCheckBoxState( IDC_DEVICES_ARE_POLLED, CurrentSetting->DevicesArePolled );
}

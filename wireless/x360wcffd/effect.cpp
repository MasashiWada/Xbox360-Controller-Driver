
//	Force Feedback Driver for Microsoft Xbox 360 Wireless Controller

#include	"effect.h"

//----------------------------------------------------------------------------------------------
//	CEffect
//----------------------------------------------------------------------------------------------
CEffect::CEffect()
{
	//	���
	Type		= 0;
	//	DIEFFECT �\����
	ZeroMemory( &DiEffect, sizeof( DIEFFECT ) );
	//	DIENVELOPE �\����
	ZeroMemory( &DiEnvelope, sizeof( DIENVELOPE ) );
	//	DICONSTANTFORCE �\����
	ZeroMemory( &DiConstantForce, sizeof( DICONSTANTFORCE ) );
	//	DIPERIODIC �\����
	ZeroMemory( &DiPeriodic, sizeof( DIPERIODIC ) );
	//	DIRAMPFORCE �\����
	ZeroMemory( &DiRampforce, sizeof( DIRAMPFORCE ) );

	//	�n���h��
	Handle		= NULL;

	//	���
	Status		= 0;
	//	�Đ���
	PlayCount	= 0;
	//	�Đ�����
	StartTime	= 0;
}

//----------------------------------------------------------------------------------------------
//	Calc
//----------------------------------------------------------------------------------------------
VOID CEffect::Calc(
	 LONG *	LeftLevel
	,LONG *	RightLevel )
{
	//	�G�t�F�N�g�̍Đ����ԁA�J�n�����A�I�������A���ݎ������Z�o����
	ULONG	Duration	= max( 1, DiEffect.dwDuration / 1000 );
	ULONG	BeginTime	= StartTime + ( DiEffect.dwStartDelay / 1000 );
	ULONG	EndTime		= 0xFFFFFFFF;
	if( PlayCount != -1 )
	{
		EndTime	= BeginTime + Duration * PlayCount;
	}
	ULONG	CurrentTime	= GetTickCount();

	//	�G�t�F�N�g�͍Đ����H
	if( Status == DIEGES_PLAYING && BeginTime <= CurrentTime && CurrentTime <= EndTime )
	{
		//	�G���x���[�v���v�Z����
		LONG	NormalRate;
		LONG	AttackLevel;
		LONG	FadeLevel;
		CalcEnvelope(
			 Duration
			,( CurrentTime - BeginTime ) % Duration
			,&NormalRate
			,&AttackLevel
			,&FadeLevel );

		//	�t�H�[�X���v�Z����
		LONG	NormalLevel;
		LONG	WorkLeftLevel;
		LONG	WorkRightLevel;
		CalcForce(
			 Duration
			,( CurrentTime - BeginTime ) % Duration
			,NormalRate
			,AttackLevel
			,FadeLevel
			,&NormalLevel );

		//	�t�H�[�X�̐����𒲐�����
		WorkLeftLevel	= (NormalLevel > 0) ? NormalLevel : -NormalLevel;
		WorkRightLevel	= (NormalLevel > 0) ? NormalLevel : -NormalLevel;

		//	�t�H�[�X�̏���𒲐�����
		WorkLeftLevel	= min( 65535, WorkLeftLevel * 65535 / 10000 );
		WorkRightLevel	= min( 65535, WorkRightLevel * 65535 / 10000 );

		//	�t�H�[�X�����Z����
		*LeftLevel	= *LeftLevel + WorkLeftLevel;
		*RightLevel	= *RightLevel + WorkRightLevel;
	}
}

//----------------------------------------------------------------------------------------------
//	CalcEnvelope
//----------------------------------------------------------------------------------------------
VOID CEffect::CalcEnvelope(
	 ULONG	Duration
	,ULONG	CurrentPos
	,LONG *	NormalRate
	,LONG *	AttackLevel
	,LONG *	FadeLevel )
{
	//	�G���x���[�v���v�Z����
	if( ( DiEffect.dwFlags & DIEP_ENVELOPE ) && DiEffect.lpEnvelope != NULL )
	{
		//	�A�^�b�N�̊������Z�o����
		LONG	AttackRate	= 0;
		ULONG	AttackTime	= max( 1, DiEnvelope.dwAttackTime / 1000 );
		if( CurrentPos < AttackTime )
		{
			AttackRate	= ( AttackTime - CurrentPos ) * 100 / AttackTime;
		}
		//	�t�F�[�h�̊������Z�o����
		LONG	FadeRate	= 0;
		ULONG	FadeTime	= max( 1, DiEnvelope.dwFadeTime / 1000 );
		ULONG	FadePos		= Duration - FadeTime;
		if( FadePos < CurrentPos )
		{
			FadeRate	= ( CurrentPos - FadePos ) * 100 / FadeTime;
		}
		//	�Z�o�����l��Ԃ�
		*NormalRate		= 100 - AttackRate - FadeRate;
		*AttackLevel	= DiEnvelope.dwAttackLevel * AttackRate;
		*FadeLevel		= DiEnvelope.dwFadeLevel * FadeRate;
	} else {
		*NormalRate		= 100;
		*AttackLevel	= 0;
		*FadeLevel		= 0;
	}
}

//----------------------------------------------------------------------------------------------
//	CalcForce
//----------------------------------------------------------------------------------------------
VOID CEffect::CalcForce(
	 ULONG		Duration
	,ULONG		CurrentPos
	,LONG		NormalRate
	,LONG		AttackLevel
	,LONG		FadeLevel
	,LONG *		NormalLevel )
{
	//	�ϐ��錾
	LONG	Magnitude	= 0;
	LONG	Period;
	LONG	R;
	LONG	Rate;

	//	�G�t�F�N�g�̎�ނɂ���ď�����U�蕪����
	switch( Type )
	{
		//	����
		case SPRING:
		case DAMPER:
		case INERTIA:
		case FRICTION:
			;
			break;

		//	�R���X�^���g �t�H�[�X
		case CONSTANT_FORCE:
			//	�G���x���[�v���l�������}�O�j�`���[�h�����߂�
			Magnitude	= DiConstantForce.lMagnitude;
			Magnitude	= ( Magnitude * NormalRate + AttackLevel + FadeLevel ) / 100;
			break;

		//	�J�X�^�� �t�H�[�X
		case CUSTOM_FORCE:
			;
			break;

		//	�����I�G�t�F�N�g
		case SQUARE:
			//	�P�����̎��ԁi�~���b�j�ƌo�ߎ��Ԃ��� 0 �` 359 �x�̂ǂ��������߂�
			Period	= max( 1, ( DiPeriodic.dwPeriod / 1000 ) );
			R		= ( CurrentPos % Period ) * 360 / Period;
			//	�t�F�[�Y���v�Z����
			R	= ( R + ( DiPeriodic.dwPhase / 100 ) ) % 360;
			//	�G���x���[�v���l�������}�O�j�`���[�h�����߂�
			Magnitude	= DiPeriodic.dwMagnitude;
			Magnitude	= ( Magnitude * NormalRate + AttackLevel + FadeLevel ) / 100;
			//	�����`���l�������}�O�j�`���[�h�����߂�
			if( 180 <= R )
			{
				Magnitude	= Magnitude * -1;
			}
			//	�I�t�Z�b�g���v�Z����
			Magnitude	= Magnitude + DiPeriodic.lOffset;
			break;

		case SINE:
			//	�P�����̎��ԁi�~���b�j�ƌo�ߎ��Ԃ��� 0 �` 359 �x�̂ǂ��������߂�
			Period	= max( 1, ( DiPeriodic.dwPeriod / 1000 ) );
			R		= ( CurrentPos % Period ) * 360 / Period;
			//	�t�F�[�Y���v�Z����
			R		= ( R + ( DiPeriodic.dwPhase / 100 ) ) % 360;
			//	�G���x���[�v���l�������}�O�j�`���[�h�����߂�
			Magnitude	= DiPeriodic.dwMagnitude;
			Magnitude	= ( Magnitude * NormalRate + AttackLevel + FadeLevel ) / 100;
			//	�����g���l�������}�O�j�`���[�h�����߂�
			Magnitude	= (LONG)( Magnitude * sin( R * 3.1415 / 180.0 ) );
			//	�I�t�Z�b�g���v�Z����
			Magnitude	= Magnitude + DiPeriodic.lOffset;
			break;

		case TRIANGLE:
			//	�P�����̎��ԁi�~���b�j�ƌo�ߎ��Ԃ��� 0 �` 359 �x�̂ǂ��������߂�
			Period	= max( 1, ( DiPeriodic.dwPeriod / 1000 ) );
			R		= ( CurrentPos % Period ) * 360 / Period;
			//	�t�F�[�Y���v�Z����
			R		= ( R + ( DiPeriodic.dwPhase / 100 ) ) % 360;
			//	�G���x���[�v���l�������}�O�j�`���[�h�����߂�
			Magnitude	= DiPeriodic.dwMagnitude;
			Magnitude	= ( Magnitude * NormalRate + AttackLevel + FadeLevel ) / 100;
			//	�O�p�g���l�������}�O�j�`���[�h�����߂�
			if( 0 <= R && R < 90 )
			{
				Magnitude	= -Magnitude * ( 90 - R ) / 90;
			}
			if( 90 <= R && R < 180 )
			{
				Magnitude	= Magnitude * ( R - 90 ) / 90;
			}
			if( 180 <= R && R < 270 )
			{
				Magnitude	= Magnitude * ( 90 - ( R - 180 ) ) / 90;
			}
			if( 270 <= R && R < 360 )
			{
				Magnitude	= -Magnitude * ( R - 270 ) / 90;
			}
			//	�I�t�Z�b�g���v�Z����
			Magnitude	= Magnitude + DiPeriodic.lOffset;
			break;

		case SAWTOOTH_UP:
			//	�P�����̎��ԁi�~���b�j�ƌo�ߎ��Ԃ��� 0 �` 359 �x�̂ǂ��������߂�
			Period	= max( 1, ( DiPeriodic.dwPeriod / 1000 ) );
			R		= ( CurrentPos % Period ) * 360 / Period;
			//	�t�F�[�Y���v�Z����
			R		= ( R + ( DiPeriodic.dwPhase / 100 ) ) % 360;
			//	�G���x���[�v���l�������}�O�j�`���[�h�����߂�
			Magnitude	= DiPeriodic.dwMagnitude;
			Magnitude	= ( Magnitude * NormalRate + AttackLevel + FadeLevel ) / 100;
			//	�A�b�v�������l�������}�O�j�`���[�h�����߂�
			if( 0 <= R && R < 180 )
			{
				Magnitude	= -Magnitude * ( 180 - R ) / 180;
			}
			if( 180 <= R && R < 360 )
			{
				Magnitude	= Magnitude * ( R - 180 ) / 180;
			}
			//	�I�t�Z�b�g���v�Z����
			Magnitude	= Magnitude + DiPeriodic.lOffset;
			break;

		case SAWTOOTH_DOWN:
			//	�P�����̎��ԁi�~���b�j�ƌo�ߎ��Ԃ��� 0 �` 359 �x�̂ǂ��������߂�
			Period	= max( 1, ( DiPeriodic.dwPeriod / 1000 ) );
			R		= ( CurrentPos % Period ) * 360 / Period;
			//	�t�F�[�Y���v�Z����
			R		= ( R + ( DiPeriodic.dwPhase / 100 ) ) % 360;
			//	�G���x���[�v���l�������}�O�j�`���[�h�����߂�
			Magnitude	= DiPeriodic.dwMagnitude;
			Magnitude	= ( Magnitude * NormalRate + AttackLevel + FadeLevel ) / 100;
			//	�_�E���������l�������}�O�j�`���[�h�����߂�
			if( 0 <= R && R < 180 )
			{
				Magnitude	= Magnitude * ( 180 - R ) / 180;
			}
			if( 180 <= R && R < 360 )
			{
				Magnitude	= -Magnitude * ( R - 180 ) / 180;
			}
			//	�I�t�Z�b�g���v�Z����
			Magnitude	= Magnitude + DiPeriodic.lOffset;
			break;

		//	�X�΃t�H�[�X
		case RAMP_FORCE:
			//	�n�_�A�I�_�̊������Z�o����
			Rate		= ( Duration - CurrentPos ) * 100
						/ max( 1,  DiEffect.dwDuration / 1000 );
			//	�G���x���[�v���l�������}�O�j�`���[�h�����߂�
			Magnitude	= ( DiRampforce.lStart * Rate
							+ DiRampforce.lEnd * ( 100 - Rate ) ) / 100;
			Magnitude	= ( Magnitude * NormalRate + AttackLevel + FadeLevel ) / 100;
			break;
	}

	//	�Q�C�����l�������t�H�[�X��Ԃ�
	*NormalLevel	= Magnitude * (LONG)DiEffect.dwGain / 10000;
}

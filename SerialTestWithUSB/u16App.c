/*
 u16App.c
 KeyboardMouse.c �̃^�X�N���[�v����Ăяo����鏈���B
*/


/* UART��M�f�[�^�̃p�[�X���� */
uint8_t parseReceiveData ( void ) {
	static parseState_t parseState = PARSE_STATE_PREINIT;
	static uint8_t rest = 0;
	static uint8_t cmd = 0;
	static uint8_t head[2];
	uint8_t ret = 0;
	uint8_t i;
	
	/* �p�[�T�[�̃C�j�V�����C�Y */
	if ( parseState == PARSE_STATE_PREINIT ) {
		/* �h���C�o���̃C�j�V�����C�Y */
		parseState = PARSE_STATE_INIT;
	} else if ( parseState == PARSE_STATE_INIT ) {
		parseState = PARSE_STATE_HEAD;
		/* ���̃X�e�[�g�Ŏ�M���K�v�ȃo�C�g���i�Œ�l 2�j */
		rest = MEGA2560_PACKETHEADER_SIZE;
	/* �p�P�b�g�擪�P�o�C�g�ƃT�C�Y1�o�C�g���擾 */
	} else if ( parseState == PARSE_STATE_HEAD ) {
		/* UART�̎�M�p�P�b�g���P�o�C�g�P�ʂœǂݏo�� */
		while ( rest != 0 ) {
			/* ��M�f�[�^���Ȃ���΁A���[�v�����B ����Ζ���M�f�[�^����-1���čēx�ǂݏo�� */
			if ( readUARTRingBuffer ( head[MEGA2560_PACKETHEADER_SIZE - rest] ,1 ) == 1 ) {
				rest--;
			} else {
				break;
			}
		}
		
		/* ����M�f�[�^0�Ŏ��̃X�e�[�g�֑J�� */
		if ( rest == 0 ) {
			/* ��M�����f�[�^�̃T�j�^���`�F�b�N */
			if ( head[1] < 3 ) {
				/* ���X�e�[�g�̃f�[�^���ƃR�}���h���L�^�����X�e�[�g�� */
				cmd  = head[0];
				rest = head[1];
			} else {
				/* �f�[�^����2�ȉ��Ȃ�G���[�@�X�e�[�g�����ɖ߂��B */
				ret = ERROR_PARSE_DATA_LENGTH;
				parseState = PARSE_STATE_INIT;
			}
		}
	/* PAYLOAD�����̎�M */
	} else if ( parseState == PARSE_STATE_PAYLOAD ) {
	
	}
	
	return ret;
}


/* application main ���� */
void appMain ( void ) {
	// pollIOPort();
	
	parseReceiveData();
}

uint8_t parseReceiveData ( void ){
	
}

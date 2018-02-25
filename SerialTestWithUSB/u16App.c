/*
 u16App.c
 KeyboardMouse.c のタスクループから呼び出される処理。
*/


/* UART受信データのパース処理 */
uint8_t parseReceiveData ( void ) {
	static parseState_t parseState = PARSE_STATE_PREINIT;
	static uint8_t rest = 0;
	static uint8_t cmd = 0;
	static uint8_t head[2];
	uint8_t ret = 0;
	uint8_t i;
	
	/* パーサーのイニシャライズ */
	if ( parseState == PARSE_STATE_PREINIT ) {
		/* ドライバ等のイニシャライズ */
		parseState = PARSE_STATE_INIT;
	} else if ( parseState == PARSE_STATE_INIT ) {
		parseState = PARSE_STATE_HEAD;
		/* 次のステートで受信が必要なバイト数（固定値 2） */
		rest = MEGA2560_PACKETHEADER_SIZE;
	/* パケット先頭１バイトとサイズ1バイトを取得 */
	} else if ( parseState == PARSE_STATE_HEAD ) {
		/* UARTの受信パケットを１バイト単位で読み出し */
		while ( rest != 0 ) {
			/* 受信データがなければ、ループ完了。 あれば未受信データ数を-1して再度読み出し */
			if ( readUARTRingBuffer ( head[MEGA2560_PACKETHEADER_SIZE - rest] ,1 ) == 1 ) {
				rest--;
			} else {
				break;
			}
		}
		
		/* 未受信データ0で次のステートへ遷移 */
		if ( rest == 0 ) {
			/* 受信したデータのサニタリチェック */
			if ( head[1] < 3 ) {
				/* 次ステートのデータ長とコマンドを記録し次ステートへ */
				cmd  = head[0];
				rest = head[1];
			} else {
				/* データ長が2以下ならエラー　ステートを元に戻す。 */
				ret = ERROR_PARSE_DATA_LENGTH;
				parseState = PARSE_STATE_INIT;
			}
		}
	/* PAYLOAD部分の受信 */
	} else if ( parseState == PARSE_STATE_PAYLOAD ) {
	
	}
	
	return ret;
}


/* application main 処理 */
void appMain ( void ) {
	// pollIOPort();
	
	parseReceiveData();
}

uint8_t parseReceiveData ( void ){
	
}

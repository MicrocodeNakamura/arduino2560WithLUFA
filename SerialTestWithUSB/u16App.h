/*
  u16App.h
*/
#ifndef __U16APP_H__
#define __U16APP_H__

typedef enum {
	PARSE_STATE_PREINIT,
	PARSE_STATE_INIT,
	PARSE_STATE_HEAD,
	PARSE_STATE_PAYLOAD,
	PARSE_STATE_MAX
} parseState_t;

typedef enum {
	ERROR_PARSE_DATA_LENGTH,
	ERROR_PARSE_MAX
} parseError_t;


typedef struct {
	uint8_t nopData[1];
}	cmdTypeNop_t;

typedef struct {
	uint8_t keyData[5];
}	cmdTypeKey_t;

/* マウスの絶対値、相対値は先頭のcmdで判断する。
   設定できる値の範囲は、USB_MouseReport_Data_t */
typedef struct {
	
}	cmdTypeMouse_t;


typedef union {
	cmdTypeNop_t nop;
	cmdTypeKey_t key;
	/* マウスの絶対値、相対値は先頭のcmdで判断する */
	cmdTypeMouse_t mouse;
} payload_ut;

typedef struct {
	uint8_t cmd;
	uint8_t size;
	payload_ut payload;
} cmdPacket_t;

/* アプリケーションタスクメイン処理 */
void appMain ( void );

/* UART受信データのパース処理 */
uint8_t parseReceiveData ( void );

/* IOポートのチェック処理 */
// uint8_t pollIOPort ( void );

#endif /* __U16APP_H__ */


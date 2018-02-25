/*
 * u16Driver.c
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "u16Driver.h"
#include "ring_buffer.h"

static void uart_write ( unsigned char c );
static void rx_handler( void );

static InterruptCallbackHandler uartRXint = NULL;
static InterruptCallbackHandler timerInt = NULL;

static uint8_t uartRxBuf[64];
static uint8_t uartTxBuf[64];
static struct ring_buffer rxRingBufHandle;
static struct ring_buffer txRingBufHandle;

/* debug */
uint8_t debugFlag = 0;

uint16_t KeyEnterStatus = 0;
/* バッファオーバーフローしたデータの量がカウントアップされる */
uint8_t readBufferOverflow = 0;

/* メインタスクのキルスイッチ */
uint8_t breaker = 0;

/* Interrupt vector registration. ----- */
ISR(PCINT0_vect){
//	uart_write ( 'b' ); // send echo back
}

ISR(USART1_RX_vect){
	/* uartRXint is function pointer. */
	if ( uartRXint != NULL ){ uartRXint(); }
}

ISR(TIMER0_COMPA_vect){
	/* uartRXint is function pointer. */
	if ( timerInt != NULL ){ timerInt(); }
}

/* interrupt handler ----- */
static void rx_handler( void ){
	volatile unsigned char c;
	c = UDR1;         // read rx receive buffer
	if ( !ring_buffer_is_full( &rxRingBufHandle ) ) {
		ring_buffer_put ( &rxRingBufHandle, c );
	} else {
		/* リングバッファがあふれたので、変数に取りこぼしたデータ数を格納 */
		readBufferOverflow++;
	}
}

void timer_handler( void ) {
	static unsigned time = 0;

	if ( time < PORT_SCAN_TIMING_MS ) {
		time++;
	} else {
		if ( PB4_IN() != 1 ){
			/* ボタン押下中は1秒おきにUSBのキーコードを発行 */
			if ( KeyEnterStatus == 0 ) {
				/* 非同期にUSBのキーコード送出要求を発行 */
				KeyEnterStatus = 1;
			}
		} else {
			if ( KeyEnterStatus == 1 ) {
				/* 非同期にUSBのキーコード送出要求を発行 */
				KeyEnterStatus = 2;
			}
		}
		time = 0;
	}
}

/*
  for debug 
*/
/* HeartBeat用のLED制御関数 */
void debugCheckLedBlinker( void )
{
	static unsigned char led = 0;
	unsigned char reg;
	reg = DEBUG_LED_PORT;
	if ( led == 0 ){
		led = 1;
		reg |= 1 <<(DEBUG_LED_BIT);
		} else {
		reg &= ~(1 <<(DEBUG_LED_BIT));
		led = 0;
	}
	PORTB = reg;
}

//-------------------------------------------
// Section UART
//-------------------------------------------
/* This macro will replace device common */
void disableInterrupt( void ){
	asm volatile( "cli" );
}

void enableInterrupt( void ){
	asm volatile( "sei" );
}

void init_UART ( InterruptCallbackHandler handler ){
	PIND = 0xff;
	DDRD = 0x80;

	// set Baudrate to 115.2kbps
	UBRR1H = 0;
	UBRR1L = 102; // 9600 bps ok
	UBRR1L = 25;  // 38400 bps ok
	//	UBRR1L = 7;   // 115200 bps NG

	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(0<<TXCIE1);
	
	uartRXint = handler;
	rxRingBufHandle = ring_buffer_init( uartRxBuf, sizeof(uartRxBuf) );
	txRingBufHandle = ring_buffer_init( uartTxBuf, sizeof(uartTxBuf) );

	DDRB = 0xE0;
/* gpio 割り込み */
#if 0
	PCICR = 0x01;
	PCMSK0 = 0x10;
#endif
}

/* 内部関数。 UARTの送信をレジスタレベルで行う */
static void uart_write ( unsigned char c ){
	unsigned char t;
	do {
		t = UCSR1A & (unsigned char)(1<<UDRE1);
	} while ( t == 0 ); // Wating tx empty bit.
	UDR1 = c;
}

/* 割り込みハンドラが格納したUART受信データを取得する */
uint8_t readUARTRingBuffer( uint8_t *buf, uint8_t size ) {
	uint8_t i;
	uint8_t readSize = 0;
	
	for ( i = 0 ; i < size ; i++ ){
		if ( !ring_buffer_is_empty( &rxRingBufHandle ) ){
			*buf = ring_buffer_get( &rxRingBufHandle );
			buf++;
			readSize++;
		} else {
			break;
		}
	}
	return readSize;
}

/* 送信データをリングバッファに格納する。　送信には、flashUARTRingBufferを呼び出す。 */
uint8_t writeUARTRingBuffer( uint8_t *buf, uint8_t size ) {
	uint8_t i;
	uint8_t writeSize = 0;
	
	for ( i = 0 ; i < size ; i++ ){
		if ( !ring_buffer_is_full( &txRingBufHandle ) ){
			ring_buffer_put( &txRingBufHandle, *buf);
			buf++;
			writeSize++;
		}
	}
	return writeSize;
}

/* Ring buffer に格納された送信データを全てUARTで送信する */
void flashUARTRingBuffer( void ) {
	while ( !ring_buffer_is_empty( &txRingBufHandle ) ){
		uart_write( ring_buffer_get( &txRingBufHandle ) );
	}
}

/* timer section ----- */
/* Timeout interrupt will occur after 
   ( ( TimerUnitMode_div * time )/ ( 20*10^6 )[s]. */

void init_Timer ( 
	InterruptCallbackHandler handler , 
	 /* you can specify TIMER_CLKDIV_[n] in deviceint.h */
	unsigned char TimerUnitMode ,
	unsigned char time ){

	unsigned char c;

	timerInt = handler;

	c = (unsigned char)(0x02);
	asm volatile( "out %0,%1"::"I"(_SFR_IO_ADDR(TCCR0A)),"a"(c) );

	c = TimerUnitMode;
	asm volatile( "out %0,%1"::"I"(_SFR_IO_ADDR(TCCR0B)),"a"(c) );

	c = time;
	asm volatile( "out %0,%1"::"I"(_SFR_IO_ADDR(OCR0A)),"a"(c) );

	TIMSK0 = (unsigned char)(0x02);

	c = (unsigned char)(0x02);
	asm volatile( "out %0,%1"::"I"(_SFR_IO_ADDR(TIFR0)),"a"(c) );
}
/* void TimerHandler ( void ){} */

void init_u16Driver(void)
{
	/* UART intialize */
	init_UART(&rx_handler);

	/* time tick for 1[ms] */
	init_Timer( &timer_handler, TIMER_CLKDIV_64, 250 );
	
#ifdef MAIN_TASK_KILL_SWITCH_ENABLE
	breaker = 1;
#endif /* MAIN_TASK_KILL_SWITCH_ENABLE */		
	/* enableInterrupt(); */
}

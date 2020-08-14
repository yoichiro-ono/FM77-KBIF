/*
* FM77KB.c
* PS/2 Keyboard Interface for FM-77
* Created: 2020/07/10 20:42:33
* Author : yoichiro
*/

#define F_CPU 20000000UL

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "FM77KB.h"

#define CTC_MATCH_OVERFLOW (uint16_t)((F_CPU / 1000UL / 8UL))

static volatile uint8_t key_flg[16];
static volatile uint8_t break_key=0,key_led=0;
static volatile uint32_t millis=0;
static volatile uint8_t key_data=0;
static volatile uint8_t cmd=0, cmd_data, parity;


int main(void)
{
	uint8_t kd,led;
	init_kb();
	while(1) {
		cli();
		kd = key_data;
		key_data = 0;
		sei();
		if (kd) {
			update_keyflg(kd);
		}
		//PB2(INS),PC4(KANA),PC5(CAPS)
		led=0x07 ^ (((PINB & _BV(PINB2))>>PINB2)|(((PINC & (_BV(PORTC4)|_BV(PORTC5))))>>3));
		if (key_led != led) {
			key_led = led;
			update_key_led();
		}
	}
	return 0;
}

/************************************************************************/
/* �L�[�{�[�h��LED�̏�Ԃ��X�V����                                         */
/* CAPS : CAPS LOCK                                                     */
/* INS  : SCROLL LOCK													*/
/* KANA : NUM LOCK														*/
/************************************************************************/
void update_key_led()
{
	sendkbcmd(0xED);
	sendkbcmd(key_led);
}

/************************************************************************/
/* �L�[�{�[�h�ɃR�}���h�𑗐M����                                          */
/************************************************************************/
void sendkbcmd(uint8_t data)
{
	cli();					//disable interrupt	
	PCMSK0&=(~_BV(PCINT1));	//PB1(CLK)�̃s���ω������݂𖳌���
	sei();					//enable interrupt for TIMER1 and PCINT1
	
	cmd=1;
	cmd_data=data;
	parity=calc_odd_parity(data);
	//CLK���o�͂ɐݒ肷��(PB1��0�̂��߁ALOW�ɂȂ�)
	DDRB|=_BV(DDB1);		//PB1(CLK) output
	_delay_us(100.0);		//100uSec�҂�
	//DAT���o�͂ɐݒ肷��(PB0��0�̂��߁ALOW�ɂȂ�)
	DDRB|=_BV(DDB0);
	DDRB&=(~_BV(DDB1));		//CLK����͂ɐݒ肷��
	cli();					//disable interrupt
	PCMSK0|=_BV(PCINT1);	//PB1(CLK)�̃s���ω������݂�L����
	sei();					//enable interrupt for TIMER1 and PCINT0 and PCINT1
	while(cmd)
		;			//���M�I����҂�
}

/************************************************************************/
/* The ISR for the 1msec timer interrupt                                */
/************************************************************************/
ISR (TIMER1_COMPA_vect)
{
	millis++;
}

/************************************************************************/
/* The ISR for the external interrupt                                   */
/* HANDING PS/2 KEY BOARD                                               */
/************************************************************************/
ISR (PCINT0_vect)
{
	static uint8_t data = 0;			// received scan code
	static uint8_t edge;
	static uint32_t prev_ms=0;
	uint32_t now_ms;
	uint8_t n, val;

	edge = bit_is_set(PINB,PINB1);	//CLOCK
	if (edge) {
		//�����オ��G�b�W�F�������Ȃ�
		return;
	}
	//�ȉ��͗�����G�b�W
	val = bit_is_set(PINB,PINB0);
	if (cmd) {
		//�L�[�{�[�h�ւ̃R�}���h���M
		if (cmd>=1 && cmd<=8) {
			//cmd=1-8:�f�[�^�𑗐M����
			if (cmd_data&1) {
				SBI(PORTB,PORTB0);
			}
			else {
				CBI(PORTB,PORTB0);
			}
			cmd_data>>=1;
		}
		else if(cmd==9) {
			//cmd=3
			if (parity) {
				SBI(PORTB,PORTB0);
			}
			else {
				CBI(PORTB,PORTB0);
			}
		}
		else if(cmd==10) {
			//cmd=2
			//stop bit & data ����͂ɐݒ�
			DDRB&=(~_BV(DDB0));		//DAT����͂ɐݒ肷��
			CBI(PORTB,PORTB0);
		}
		else {
			//val��ack
			cmd=0;
			return;
		}
		cmd++;
		return;
	}
	now_ms = get_millis();
	if (now_ms - prev_ms > 250) {
		//�O�񂩂�250ms�𒴂��Ă���ꍇ�͎�M��Ԃ�����������
		bitcount=0;
		data=0;
	}
	prev_ms=now_ms;
	if (bitcount==0) {
		//start bit
	}
	else if (bitcount<9) {
		//data bit
		data >>= 1;
		if (val) {
			data |= 0x80;
		}
	}
	//9:parity bit
	//10:stop bit
	bitcount++;
	if (bitcount==11) {
		key_data = data;
		data=0;
		bitcount=0;
	}
}

/************************************************************************/
/* The ISR for the external interrupt                                   */
/* HANDING FM-77 KEY BOARD I/F                                          */
/************************************************************************/
ISR(PCINT1_vect)
{
	uint8_t row = (PINC & (_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3)));
	uint8_t kflg = key_flg[row];
	kflg &= 0x7f;
	if(break_key) {
		kflg |= 0x80;
	}
	PORTD = kflg;
}

/************************************************************************/
/* update key press flag                                                */
/************************************************************************/
void update_keyflg(uint8_t kcode)
{
	static uint8_t skpcnt = 0;
	static uint8_t flgMake = 0, flgE0 = 0, flgE1 = 0;
	uint8_t row, key_flag;

	//PORTB |= _BV(4);	// PORTB = 0x10(KSDATA=H)

	if (skpcnt > 0) {	// for PAUSE
		skpcnt--;
		return;
	}
	if (kcode == 0xF0) {
		//MAKE CODE
		flgMake = 1;
		return;
	}
	if (kcode == 0xE0) {
		flgE0 = 1;
		return;
	}
	if (kcode == 0xE1) {
		flgE1 = 1;
		
		skpcnt = 6;		// if PAUSE then skip 6bytes
		return;
	}
	key_flag = 0xff;
	if (flgE0) {
		search_table(kcode, &row, &key_flag, pscode_tbl2, sizeof(pscode_tbl2)/3);
	}
	else if (flgE1) {
		if (kcode == 0x77)  {
			//break_key = flgMake==0 ? 1 : 0;
		}
	}
	else {
		search_table(kcode, &row, &key_flag, pscode_tbl1, sizeof(pscode_tbl1)/3);
	}
	if (key_flag != 0xff) {
		if (key_flag) {
			if (flgMake==0) {
				//make code
				key_flg[row] = key_flag;
			}
			else {
				//break code
				key_flg[row] = 0xff;
			}
		}
		else {
			//BREAK
			break_key = !flgMake;
		}
	}
	flgMake = 0;
	flgE0 = 0;
	flgE1 = 0;
}

/************************************************************************/
/* Get Milli Seconds                                                    */
/************************************************************************/
uint32_t get_millis()
{
	uint32_t millis_return;
	cli();
	millis_return = millis;
	sei();
	return millis_return;
}

/************************************************************************/
/* SEARCH TABLE                                                         */
/************************************************************************/
void search_table(uint8_t code, uint8_t* row, uint8_t* key_flag, const uint8_t table[], const int8_t table_size) {
	int8_t	left=0;
	int8_t	right=table_size-1;
	int8_t	mid;
	size_t	p;
	uint8_t	d;
	while (1) {
		mid = left + (right-left) / 2;
		p = mid*3;
		d = pgm_read_byte(&table[p]);
		if (d == code) {
			*row=pgm_read_byte(&table[p+1]);
			*key_flag=pgm_read_byte(&table[p+2]);
			return;
		}
		if (d < code) {
			left = mid + 1;
		}
		else {
			right = mid - 1;
		}
		if (right < left) {
			break;
		}
	}
	return;
}

/************************************************************************/
/* INITIALIZE                                                           */
/************************************************************************/
void init_kb(void)
{
	cli();
	memset(key_flg, 0xff, sizeof(key_flg));
	MCUSR=_BV(PUD);	//�S�|�[�g�v���A�b�v�֎~
	
	//PS/2�C���^�[�t�F�[�X�ݒ�
	DDRB=0;		//PB0(DAT),PB1(CLK),PB2 input
	//PORTB=_BV(PORTB0)|_BV(PORTB1);	//PB0,PB1 Hi-Z,PB2:pull up
	PORTB=_BV(PORTB2);
	//PB1(CLK)�̃s���ω������݂�L���ɂ���
	PCMSK0=_BV(PCINT1);
	
	//�s���ω�0(PB0-7),1(PC0-PC7)�̊����݂�������
	PCICR=_BV(PCIE0)|_BV(PCIE1);
	
	//FM-77�L�[�{�[�hI/F�ݒ�
	DDRC=0;	//PC0-PC5 input
	PORTC=_BV(PORTC0)|_BV(PORTC1)|_BV(PORTC2)|_BV(PORTC3)|_BV(PORTC4)|_BV(PORTC5);	//PC0-PC3 Hi-Z
	//PC0-PC3�̃s���ω������݂�L���ɂ���
	PCMSK1=_BV(PCINT8)|_BV(PCINT9)|_BV(PCINT10)|_BV(PCINT11);

	//�o�̓|�[�g
	DDRD=0b11111111;	//PD0-PD7 output
	PORTD=0xff;			//HI

	bitcount = 11;
	
	////timer�ݒ�(millis�֐��p)
	// CTC mode, Clock/8
	TCCR1B |= (1 << WGM12) | (1 << CS11);
	
	// Load the high byte, then the low byte
	// into the output compare
	OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
	OCR1AL = (uint8_t)(CTC_MATCH_OVERFLOW & 0xff);
	
	// Enable the compare match interrupt
	TIMSK1 |= (1 << OCIE1A);

	_delay_ms(2000);			// wait keyboard reset
	
	//numlock led
	update_key_led();
	//key repeat
	sendkbcmd(0xF3);
	sendkbcmd(0x00);
	sei();
}

/************************************************************************/
/* ��p���e�B�v�Z                                                       */
/************************************************************************/
uint8_t calc_odd_parity(uint8_t val) {
	uint8_t count;
	//�r�b�g�����J�E���g����(POPCNT�̃A���S���Y��)
	val = (val & 0x55)+((val & 0xAA)>>1);
	val = (val & 0x33)+((val & 0xCC)>>2);
	//BYTE�̃j�u������������̂�4bit�V�t�g�͑���
	//��p���e�B�����߂邽�߃r�b�g����+1����
	count = (val & 0x0F)+((val & 0xF0)>>4)+1;	
	return count & 1;
}

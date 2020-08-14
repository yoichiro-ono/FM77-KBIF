/*
 * FM77KB.h
 *
 * Created: 2020/07/10 20:49:41
 *  Author: yoichiro
 */ 


#ifndef FM77KB_H_
#define FM77KB_H_


#define CBI(port, pin) (port)&=~_BV(pin)
#define SBI(port,pin) (port)|=_BV(pin)

// for convert
#define L_SHIFT 0x12
#define R_SHIFT 0x59
#define L_CTRL 0x14
#define R_CTRL 0x14

const uint8_t CAPS_L_LED=4;
const uint8_t NUM_L_LED=2;
const uint8_t SCRL_L_LED=1;

// for ISR
uint8_t	bitcount;

// LED on/off
uint8_t	flg_led;

void init_kb(void);
void update_keyflg(unsigned char kcode);
void update_key_led();
void sendkbcmd(uint8_t data);
void search_table(uint8_t code, uint8_t* row, uint8_t* key_flag, const uint8_t table[], const int8_t table_size);
uint8_t calc_odd_parity(uint8_t val);
uint32_t get_millis();


// PS/2キーボードのスキャンコード
// SCANCODEでソート済み
const uint8_t pscode_tbl1[] PROGMEM = {
	0x01, 9,0b11111110,	/* F9 :PF9 */
	0x03, 5,0b11111110,	/* F5 :PF5 */
	0x04, 3,0b11111110,	/* F3 :PF3 */
	0x05, 1,0b11111110,	/* F1 :PF1 */
	0x06, 2,0b11111110,	/* F2 :PF2 */
	0x07, 0,0b00000000,	/* F12:BREAK */
	0x09,10,0b11111110,	/* F10:PF10 */
	0x0a, 8,0b11111110,	/* F8 :PF8 */
	0x0b, 6,0b11111110,	/* F6 :PF6 */
	0x0c, 4,0b11111110,	/* F4 :PF4 */
	0x0d, 0,0b11011111,	/* Tab */
	0x0e, 0,0b11111111,	/* Kanji:(none) */
	0x11,15,0b11011111,	/* AltL:GRAPH */
	0x12,15,0b11101111,	/* Shift_L */
	0x13,15,0b10111111,	/* Kana:Kana */
	0x14,15,0b11110111,	/* Ctrl_L:CTRL */
	0x15, 0,0b11111011,	/* q */
	0x16, 0,0b11111101,	/* full_1 */
	0x1a, 0,0b11101111,	/* z */
	0x1b, 1,0b11110111,	/* s */
	0x1c, 0,0b11110111,	/* a */
	0x1d, 1,0b11111011,	/* w */
	0x1e, 1,0b11111101,	/* full_2 */
	0x21, 2,0b11101111,	/* c */
	0x22, 1,0b11101111,	/* x */
	0x23, 2,0b11110111,	/* d */
	0x24, 2,0b11111011,	/* e */
	0x25, 3,0b11111101,	/* full_4 */
	0x26, 2,0b11111101,	/* full_3 */
	0x29, 4,0b11011111,	/* Space */
	0x2a, 3,0b11101111,	/* v */
	0x2b, 3,0b11110111,	/* f */
	0x2c, 4,0b11111011,	/* t */
	0x2d, 3,0b11111011,	/* r */
	0x2e, 4,0b11111101,	/* full_5 */
	0x31, 5,0b11101111,	/* n */
	0x32, 4,0b11101111,	/* b */
	0x33, 5,0b11110111,	/* h */
	0x34, 4,0b11110111,	/* g */
	0x35, 5,0b11111011,	/* y */
	0x36, 5,0b11111101,	/* full_6 */
	0x3a, 6,0b11101111,	/* m */
	0x3b, 6,0b11110111,	/* j */
	0x3c, 6,0b11111011,	/* u */
	0x3d, 6,0b11111101,	/* full_7 */
	0x3e, 7,0b11111101,	/* full_8 */
	0x41, 7,0b11101111,	/* Full , */
	0x42, 7,0b11110111,	/* k */
	0x43, 7,0b11111011,	/* i */
	0x44, 8,0b11111011,	/* o */
	0x45, 9,0b11111101,	/* full_0 */
	0x46, 8,0b11111101,	/* full_9 */
	0x49, 8,0b11101111,	/* Full . */
	0x4a, 9,0b11101111,	/* Full / */
	0x4b, 8,0b11110111,	/* l */
	0x4c, 9,0b11110111,	/* ; */
	0x4d, 9,0b11111011,	/* p */
	0x4e,10,0b11111101,	/* Full - */
	0x51,10,0b11101111,	/* _ */
	0x52,10,0b11110111,	/* : */
	0x54,10,0b11111011,	/* @ */
	0x55,11,0b11111101,	/* ^ */
	0x58,15,0b11111011,	/* Caps:CAP */
	0x59,15,0b11101111,	/* ShiftR */
	0x5a,12,0b11110111,	/* Full_Enter */
	0x5b,11,0b11111011,	/* [ */
	0x5d,11,0b11110111,	/* ] */
	0x64, 0,0b11111111,	/* Conv:(none) */
	0x66,12,0b11111011,	/* Back Space */
	0x67, 0,0b11111111,	/* NonConv:(none) */
	0x69, 9,0b10111111,	/* Tenkey_1 */
	0x6a,12,0b11111101,	/* \ */
	0x6b, 9,0b11011111,	/* Tenkey_4 */
	0x6c, 8,0b11011111,	/* Tenkey_7 */
	0x70,13,0b10111111,	/* Tenkey_0 */
	0x71,14,0b10111111,	/* Tenkey_. */
	0x72,10,0b10111111,	/* Tenkey_2 */
	0x73,10,0b11011111,	/* Tenkey_5 */
	0x74,11,0b11011111,	/* Tenkey_6 */
	0x75, 7,0b11011111,	/* Tenkey_8 */
	0x76, 0,0b11111110,	/* Esc */
	0x77, 0,0b11111111,	/* NumLock:(none) */
	0x78, 0,0b11111111,	/* F11:(none) */
	0x79, 6,0b10111111,	/* Tenkey_+ */
	0x7a,11,0b10111111,	/* Tenkey_3 */
	0x7b, 5,0b10111111,	/* Tenkey_- */
	0x7c, 8,0b10111111,	/* Tenkey_* */
	0x7d, 6,0b11011111,	/* Tenkey_9 */
	0x7e, 0,0b11111111,	/* ScrollLock:(none) */
	0x83, 7,0b11111110,	/* F7 :PF7 */
};
/* EXTEND-E0 */
const uint8_t pscode_tbl2[] PROGMEM = {
	0x11,15,0b11011111,	/* AltR:GRAPH */
	0x14,15,0b11110111,	/* CtrlR:CTRL */
	0x1f, 0,0b11111111,	/* WinL:(none) */
	0x27, 0,0b11111111,	/* WinR:(none) */
	0x4a, 7,0b10111111,	/* Tenkey_/ */
	0x5a,12,0b10111111,	/* Tenkey_Enter */
	0x69,13,0b11111011,	/* End:EL */
	0x6b,13,0b11011111,	/* LEFT */
	0x6c,14,0b11110111,	/* Home:HOME */
	0x70,13,0b11111101,	/* INSERT:INS */
	0x71,13,0b11110111,	/* Delete:DEL */
	0x72,14,0b11101111,	/* DOWN */
	0x74,14,0b11011111,	/* RIGHT */
	0x75,13,0b11101111,	/* UP */
	0x7a,14,0b11111011,	/* PageDown:DUP */
	0x7c, 0,0b11111111,	/* PrtScreen:(none) */
	0x7d,14,0b11111101,	/* PageUp:CLS */
};

#endif /* FM77KB_H_ */
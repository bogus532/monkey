
// Ports for V-USB
#define ROWS_PORT1  PORTA
#define ROWS_DDR1   DDRA

#define ROWS_PORT2  PORTC
#define ROWS_DDR2   DDRC

#define ROWS_PORT3  PORTD
#define ROWS_DDR3   DDRD
#define ROWS_ALL3   (_BV(PD6)|_BV(PD7))

#define COLS_PORT   PORTB
#define COLS_DDR    DDRB
#define COLS_PIN    PINB

void init_col_row(void)
{
  // initialize row and col
  unselect_rows();
  // Input with pull-up(DDR:0, PORT:1)
  COLS_DDR = 0x00;
  COLS_PORT = 0xFF;
}

uint8_t read_columns(void)
{
  _delay_us(30);  // without this wait read unstable value.
  return COLS_PIN;
}

void unselect_rows(void)
{
	ROWS_DDR1=0x00;
	ROWS_PORT1=0xff;
  ROWS_DDR2=0x00;
  ROWS_PORT2=0xff;
	ROWS_DDR3&=~ROWS_ALL3;
  ROWS_PORT3|=ROWS_ALL3;
}

void select_row(uint8_t row)
{
	if(row<8) {
		ROWS_DDR1=_BV(row);
		ROWS_PORT1=~_BV(row);
	} else if(row<16) {
		ROWS_DDR2=_BV(row&0x07);
		ROWS_PORT2=~_BV(row&0x07);
	} else {
		uint8_t temp=_BV(row&0x03);
		ROWS_DDR3=(ROWS_DDR3&~ROWS_ALL3)|temp;
		ROWS_PORT3=(ROWS_PORT3|ROWS_PORT3)&~temp;
	}
}

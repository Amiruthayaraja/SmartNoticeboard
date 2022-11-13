#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//Usart 
#define BAUD 9600
#define UBRR_VALUE ((F_CPU/16/BAUD)-1)
unsigned char rxData;


#define LcdDataBus PORTD
#define LcdControlBus PORTB

#define LcdDataBusDirnReg DDRB

#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2


char *OldData;
volatile int toggle = 0;

void Turn_Power_Indicator_LED_ON(void);
void initPorts(void);
void Lcd_CmdWrite (char cmd);
void Lcd_DataWrite(char dat);
void Lcd_init (void);
void Lcd_str(char *str);
//Usart
//void transmitUSART(unsigned char data);
void init_usart(void);
unsigned char receiveUSART();



int main(void)
{
	/* Replace with your application code */
	initPorts();
	Turn_Power_Indicator_LED_ON();
	Lcd_init();
	init_usart();
	//Lcd_str("Hello World");
	//initUSART();
	/*Lcd_DataWrite('W');
	Lcd_DataWrite('E');
	Lcd_DataWrite('L');
	Lcd_DataWrite('C');
	Lcd_DataWrite('O');
	Lcd_DataWrite('M');
	Lcd_DataWrite('E');
	Lcd_DataWrite('$');
	*/
	EICRA |= 0x03;
	EIMSK |= 0x02;
	sei();
	
	while (1)
	{
		unsigned char rxData;
		rxData = receiveUSART();
		
		//transmitUSART('R');
		
		
	}
	return 0;
}


void initPorts(void)
{
	LcdDataBusDirnReg = 0xFF;
	DDRD &= 0B11111110;
	DDRC |= 0B00100000;
	
}

void Turn_Power_Indicator_LED_ON(void)
{
	PORTB |= 0B00100000;
}

void Lcd_CmdWrite (char cmd)
{
	LcdDataBus = (cmd & 0xF0);
	LcdControlBus &=~(1<<LCD_RS); //SET RS PIN TO LOW
	LcdControlBus &=~(1<<LCD_RW);  // SET RW PIN TO LOW
	LcdControlBus |=(1<<LCD_EN); // GENERATE HIGH TO LOW PULSE
	_delay_ms(10);
	LcdControlBus &=~(1<<LCD_EN);
	_delay_ms(10);
	
	
	LcdDataBus = ((cmd<<4) & 0xF0);
	LcdControlBus &=~(1<<LCD_RS); //SET RS PIN TO LOW
	LcdControlBus &=~(1<<LCD_RW);  // SET RW PIN TO LOW
	LcdControlBus |=(1<<LCD_EN); // GENERATE HIGH TO LOW PULSE
	_delay_ms(10);
	LcdControlBus &=~(1<<LCD_EN);
	_delay_ms(10);
}

void Lcd_DataWrite(char dat)
{
	LcdDataBus = (dat & 0xF0);
	LcdControlBus |=(1<<LCD_RS); //SET RS PIN TO high
	LcdControlBus &=~(1<<LCD_RW);  // SET RW PIN TO LOW
	LcdControlBus |=(1<<LCD_EN); // GENERATE HIGH TO LOW PULSE
	_delay_ms(10);
	LcdControlBus &=~(1<<LCD_EN);
	_delay_ms(10);
	
	
	LcdDataBus = ((dat<<4) & 0xF0);
	LcdControlBus |=(1<<LCD_RS); //SET RS PIN TO high
	LcdControlBus &=~(1<<LCD_RW);  // SET RW PIN TO LOW
	LcdControlBus |=(1<<LCD_EN); // GENERATE HIGH TO LOW PULSE
	_delay_ms(10);
	LcdControlBus &=~(1<<LCD_EN);
	_delay_ms(10);
}

void Lcd_init (void)
{
	Lcd_CmdWrite(0x02); //initialize LCD  in 4bit mode
	Lcd_CmdWrite(0x28); // enable 5x7 mode for chars
	Lcd_CmdWrite(0x0E); //Display OFF, cursor ON
	Lcd_CmdWrite(0x01); // clear Display
	Lcd_CmdWrite(0x80); // Move the cursor to beginning or first line
}

void Lcd_str(char *str)
{
	while(*str)
	Lcd_DataWrite(*str++);
}

ISR (INT1_vect){
	
	
	if(toggle){
		Lcd_CmdWrite(0x0E);
		Lcd_CmdWrite(0x80);
		Lcd_str("    Notice:");
				
		
	}else{
		Lcd_CmdWrite(0x08);
	}
	
	
	toggle = 1 - toggle;
	
}

ISR(USART_RX_vect){
	//rxData = UDR0;
	
	Lcd_CmdWrite(0xC0);
	//Lcd_DataWrite(rxData);
	// allocate buffer
	uint8_t bufSize = 100;
	char buf[bufSize];

	// get line from UART
	
	receiveStr(buf, bufSize);
	Lcd_str(buf);
	if(buf){
		PORTC |= 0B00100000;
		_delay_ms(1000);
		PORTC &= 0B11011111;
	}
	
	//PORTD |= 0B00000100;
}

// USART Commands
void init_usart(void){
	UBRR0H =(unsigned char)(UBRR_VALUE>>8); //Set UBBR upper byte
	UBRR0L =(unsigned char)(UBRR_VALUE); //Set UBBR lower byte
	UCSR0B =(1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0); //enable transmitter/receiver
	UCSR0C =(3<<UCSZ00); //8-data bits
}

unsigned char receiveUSART(void)
{
	while(!(UCSR0A & (1<<RXC0))); //wait till data received
	return UDR0;				// put data to transmit buffer
	
}

void receiveStr(char* buf, uint8_t n)
{
	uint8_t bufIdx = 0;
	char c;

	
	do
	{
		
		c = receiveUSART();

		
		buf[bufIdx++] = c;
	}
	while((bufIdx < n) && (c != '\r'));

	
	buf[bufIdx] = 0;
}
/*
void transmitStr(unsigned char *Message){
	unsigned char i = 0;
	while(i < 10){
		transmitUSART(Message[i]);
		i++;
	}
}

void transmitUSART(unsigned char data)
{
	while(!(UCSR0A &(1<<UDRE0))); //wait till transmit buffer is ready
	UDR0 = data; //put data to transmit buffer
}

*/
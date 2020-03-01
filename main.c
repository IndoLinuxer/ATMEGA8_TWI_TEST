#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/twi.h>
#define FOSC 16000000
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#define Fscl 400000
#define MYTWBR (FOSC/Fscl - 16)/2
#define MAX_STRING 100

char string[MAX_STRING];

int timer = 0;
int timer_1 = 0;

ISR(TIMER0_OVF_vect,ISR_NOBLOCK)
{
		timer++;
		timer_1++;
		if(timer == 7812) //TOGGLE EVERY 1 SECOND
		{
			PORTB ^= 0b00000001;
			timer = 0;
		}
		if(timer_1 == 3906) //toggle every 0.5 second
		{
			PORTB ^= 0b00000010;
			timer_1 = 0;
		}
}

void timer_init(void);

/* USART FUNCTION */
void usart_init(void);
void usart_send_char(unsigned char data);
unsigned char usart_recv_char(void);
void send_string(unsigned char *data);

/* TWI FUNCTION */
void twi_init(void);

void twi_start(char *status);
void twi_stop(void);
void twi_send_byte_ack(char data, char *status);
void twi_send_byte_nack(char data, char *status);
void twi_recv_byte_ack(char *data, char *status);
void twi_recv_byte_nack(char *data, char *status);

int main(void)
{
	char twi_stat;
	float accel_x,accel_y,accel_z;
	float accel[100];
	char dump_h,dump_l;
	int count;
	
	dump_h = 0;
	dump_l = 0;
	DDRB |= 0b00000011;
	
	timer_init();
	usart_init();		
	twi_init();
	
	sei(); //enable interrupt
	
	twi_start(&twi_stat);
	twi_send_byte_nack(0xD0,&twi_stat);
	twi_send_byte_nack(0x6B,&twi_stat);
	twi_send_byte_nack(0x00,&twi_stat);
	twi_stop();
	
	while(1)
	{
		usart_send_char(0x1B);
		usart_send_char('[');
		usart_send_char('1');
		usart_send_char('J');
		
		usart_send_char(0x1B);
		send_string("[10;10H");
		
		sprintf(string, "NEW DATA \r"); 
		send_string(string);
		
		for(count = 0; count < 100;count++)
		{
			/* RECEIVE HIGH X ACC REG */
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD0,&twi_stat);
			twi_send_byte_nack(0x3B,&twi_stat);
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD1,&twi_stat);
			twi_recv_byte_nack(&dump_h,&twi_stat);
			twi_stop();
			
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD0,&twi_stat);
			twi_send_byte_nack(0x3C,&twi_stat);
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD1,&twi_stat);
			twi_recv_byte_nack(&dump_l,&twi_stat);
			twi_stop();
			accel_x = (float)((dump_h << 8) | (dump_l))/16384.0;
			accel[count] = accel_x;
		}
		accel_x = 0;
		for(count = 0;count < 100;count++)
		{
			accel_x += accel[count];
		}
		accel_x = (accel_x / 100.0);
		sprintf(string," X accelerometer --> %3.4f \r",accel_x);
		send_string(string);
		
		/* Receive Y accelerometer */
		for(count = 0; count < 100;count++)
		{
			/* RECEIVE HIGH Y ACC REG */
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD0,&twi_stat);
			twi_send_byte_nack(0x3D,&twi_stat);
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD1,&twi_stat);
			twi_recv_byte_nack(&dump_h,&twi_stat);
			twi_stop();
			
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD0,&twi_stat);
			twi_send_byte_nack(0x3E,&twi_stat);
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD1,&twi_stat);
			twi_recv_byte_nack(&dump_l,&twi_stat);
			twi_stop();
			accel_y = (float)((dump_h << 8) | (dump_l))/16384.0;
			accel[count] = accel_y;
		}
		accel_y = 0;
		for(count = 0;count < 100;count++)
		{
			accel_y += accel[count];
		}
		accel_y = (accel_y / 100.0);
		sprintf(string," Y accelerometer --> %3.4f \r",accel_y);
		send_string(string);

		/* Receive Z accelerometer */
		for(count = 0; count < 100;count++)
		{
			/* RECEIVE HIGH Y ACC REG */
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD0,&twi_stat);
			twi_send_byte_nack(0x3F,&twi_stat);
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD1,&twi_stat);
			twi_recv_byte_nack(&dump_h,&twi_stat);
			twi_stop();
			
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD0,&twi_stat);
			twi_send_byte_nack(0x40,&twi_stat);
			twi_start(&twi_stat);
			twi_send_byte_nack(0xD1,&twi_stat);
			twi_recv_byte_nack(&dump_l,&twi_stat);
			twi_stop();
			accel_z = (float)((dump_h << 8) | (dump_l))/16384.0;
			accel[count] = accel_z;
		}
		accel_z = 0;
		for(count = 0;count < 100;count++)
		{
			accel_z += accel[count];
		}
		accel_z = (accel_z / 100.0);
		sprintf(string,"Z accelerometer --> %3.4f \r",accel_z);
		send_string(string);
		
		//_delay_ms(100);
	}

	return 0;
}

/* TWI FUNCTION */
void twi_init(void)
{
	TWBR = (unsigned char) MYTWBR;
	TWCR |= (1<<TWEN);
}

void twi_start(char *status)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA);
	while(!(TWCR & (1<<TWINT)));
	*status = (TWSR & 0xF8);
}

void twi_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

void twi_send_byte_ack(char data, char *status)
{
	TWDR = data;
	TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
	*status = (TWSR & 0xF8);
}

void twi_send_byte_nack(char data, char *status)
{
	TWDR = data;
	TWCR = (1<<TWEN) | (1<<TWINT);
	while(!(TWCR & (1<<TWINT)));	
	*status = (TWSR & 0xF8);
}

void twi_recv_byte_ack(char *data, char *status)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
	*data = TWDR;
	*status = (TWSR & 0xF8);
}

void twi_recv_byte_nack(char *data, char *status)
{
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	*status = (TWSR & 0xF8);
	*data = TWDR;	
}


/* USART FUNCTION */
void send_string(unsigned char *data)
{
	int count;
	for(count = 0;data[count] != '\0';count++)
	{
		if(data[count] == '\r' || data[count] == '\n')
		{
			usart_send_char('\n');
			usart_send_char('\r');
		}
		else
		{
			usart_send_char(data[count]);
		}
	}
}

void timer_init(void)
{
	TCCR0 |= (0<<CS02) | (1<<CS01) | (0<<CS00);
	TIMSK = (1<<TOIE0);
	TCNT0 = 0;
}

void usart_init(void)
{
	UCSRB = (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2);
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<USBS);
	UBRRH = (unsigned char) (MYUBRR>>8);
	UBRRL = (unsigned char) (MYUBRR);
}

void usart_send_char(unsigned char data)
{
	while(!(UCSRA & (1<<UDRE)));
	UDR = data;
}

unsigned char usart_recv_char(void)
{
	while(!(UCSRA & (1<<RXC)));
	return UDR;
}

/*
 * 
 * 
   DEBUG EXAMPLE
 	while(1)
	{
		
		sprintf(string,"TEST %5.3f \n",78.23234);
		send_string(string);
		twi_send_byte_nack(0xff,&twi_stat);
		sprintf(string,"TWSR --> %X \n",twi_stat);
		send_string(string);
		_delay_ms(500);
		twi_send_byte_nack(0x00,&twi_stat);
		sprintf(string,"TWSR --> %X \n",twi_stat);
		send_string(string);
		_delay_ms(500);
	}

 * 
 * 
 * */

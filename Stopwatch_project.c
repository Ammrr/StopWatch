
// #include <avr/io.h>
# define F_CPU 8000000UL
#include <util/delay.h>

#define SET_BIT(reg,bit) (reg|=(1<<bit))
#define CLR_BIT(reg,bit) (reg&=~(1<<bit))
#define READ_BIT(reg,bit) ((reg>>bit)&1)

#define TOG_BIT(reg,bit) (reg^=(1<<bit))
#define SEG_PORT PORTA

#define  PORTA (*(volatile unsigned char *)0x3B)
#define  DDRA (*(volatile unsigned char *)0x3A)
#define  PINA (*(const volatile unsigned char *)0x39)

#define  PORTB (*(volatile unsigned char *)0x38)
#define  DDRB (*(volatile unsigned char *)0x37)
#define  PINB (*(const volatile unsigned char *)0x36)

#define  PORTC (*(volatile unsigned char *)0x35)
#define  DDRC (*(volatile unsigned char *)0x34)
#define  PINC (*(const volatile unsigned char *)0x33)

#define  PORTD (*(volatile unsigned char *)0x32)
#define  DDRD (*(volatile unsigned char *)0x31)
#define  PIND (*(const volatile unsigned char *)0x30)

	unsigned char reset_flag=0,resume_flag=1,adjust_flag=0,inc_one=0,inc_ten=0;
	
	int SEGMENT_DISPLAY (char num)
	{
		const char arr[10]={0x3f, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6f};
		char sec,n=num,c=num;
		for (sec=0;sec<50;sec++)
		{
			
			//normal
			if (adjust_flag==0)
			{
				if (READ_BIT(PIND,4)==0)
				{
					while(!READ_BIT(PIND,4));
					adjust_flag= 1;
					return 0;
				}
				if (READ_BIT(PIND,2)==0)
				{
					while(!READ_BIT(PIND,2));
					reset_flag= 1;
					return 0;
				}				
				if (READ_BIT(PIND,3)==0)
				{
					while(!READ_BIT(PIND,3));
					resume_flag^= 1;
					return 0;
				}
			}
			else if (adjust_flag==1)
			{
				if (READ_BIT(PIND,4)==0)
				{
					while(!READ_BIT(PIND,4));
					adjust_flag= 0;
					resume_flag=1;
					return num;
				}
				if (!READ_BIT(PIND,2))
				{
					while(!READ_BIT(PIND,2));
					inc_ten=1;
				}
				if (!READ_BIT(PIND,3))
				{
					while(!READ_BIT(PIND,3));
					inc_one=1;
				}
				if(inc_one==1)
				{
					c=c+1;
					n=c;
					inc_one=0;
				}
				if(inc_ten==1)
				{
					c=c+10;
					n=c;
					inc_ten=0;
				}
				if (n>=100 )
				{
					n=n%10;
					c=n;
				}
			}
			
			if (resume_flag)
			{
				SET_BIT(PORTB,2);
			} 
			else
			{
				CLR_BIT(PORTB,2);
			}
			
			if (reset_flag)
			{
				SET_BIT(PORTB,1);
			}
			else
			{
				CLR_BIT(PORTB,1);
			}
			
			if (adjust_flag)
			{
				SET_BIT(PORTB,3);
			}
			else
			{
				CLR_BIT(PORTB,3);
			}
			
			SET_BIT(PORTC,6);                                           //off
			SEG_PORT=  arr[num%10]<<1 ;
			CLR_BIT(PORTC,7);                                          //on
			_delay_ms(10);

			SET_BIT(PORTC,7);
			SEG_PORT=  arr[num/10]<<1 ;
			CLR_BIT(PORTC,6);
			_delay_ms(10);

		}
		return n;
	}
	void SEGMENT_DISPLAY_2 (char n)
	{
		const char arr[10]={0x3f, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6f};
		
		SET_BIT(PORTC,6);                                           //off
		SEG_PORT=  arr[n%10]<<1 ;
		CLR_BIT(PORTC,7);                                          //on
		_delay_ms(10);

		SET_BIT(PORTC,7);
		SEG_PORT=  arr[n/10]<<1 ;
		CLR_BIT(PORTC,6);
		_delay_ms(10);
		
	}

	int main (void)
	{
		signed char c=30,n=c;
		DDRA=0xff;
		DDRB=0xff;
		DDRC =0xff;
		DDRD=0x00;
		PORTD=0x1c;
		while(1)
		{
			for (c=n;c>=0 ;c--)
			{
				
				if( reset_flag==1)
				{
					c=n;
					while(resume_flag==0 &&reset_flag==1)
					{
						SEGMENT_DISPLAY(n);
						while(adjust_flag==1)
						{
							n=SEGMENT_DISPLAY(c);
							c=n;
						}
						
					}
					reset_flag=0;
					resume_flag=1 ;
					break;
				}
				if( resume_flag==1 )
				{
					while(resume_flag==1 )
					{
						SEGMENT_DISPLAY(c);
						while(adjust_flag==1)
						{
							n=SEGMENT_DISPLAY(c);
							c=n;
						}
						if(reset_flag==1)
						{
							resume_flag=0;
							break;
						}
					}
				}
				while(adjust_flag==1)
				{
					n=SEGMENT_DISPLAY(c);
					c=n;
				}
				
				
				SEGMENT_DISPLAY(c);
				if (c==0)
				{
					PORTA=0;
					SET_BIT(PORTC,5);
					_delay_ms(1000);
					CLR_BIT(PORTC,5);
					SEGMENT_DISPLAY(n);
					resume_flag^= 1;
					break;
				}
				
			}
			
			
		}
	}

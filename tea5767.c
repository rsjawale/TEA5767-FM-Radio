#include <regx51.h>
#include <i2c.h>
#include <lcd.h>

#define SDA P0_0
#define SCL P0_1
#define SW1 P1_0               //sw1 to increment freq
#define SW2 P1_1               //sw2 to decrement freq  

unsigned char frequencyH;
unsigned char frequencyL;
unsigned int  frequencyB;
double frequency = 91.1;       //starting frequency
double freq_available=0; 
unsigned char digit[4]={0,0,0,0};
unsigned int freq;
unsigned char readf[8]={1,1,1,1,1,1,0,1};
unsigned char signal[4] = {0,0,0,0};
unsigned char str[1]={0};
unsigned int siglvl,stereo;
unsigned int read1;
unsigned int read2;
unsigned int read3;
unsigned int read4;
unsigned int read5;

void delay(unsigned int a)
{
unsigned int i,j;
for (i=0;i<a;i++) 
{
for (j=0;j<1275;j++);
}
}

void TEA5767()
{
	
	frequencyB=4*(frequency*1000000+225000)/32768+1; //calculating PLL word

  frequencyH=frequencyB>>8;

  frequencyL=frequencyB & 0XFF;
}

void send()                          //send bytes to TEA5767
{
	TEA5767();
	
	delay(10);
	
	I2CInit();
	
	I2CStart();
	
	I2CSend(0xC0);
	
	I2CSend(frequencyH);
	
	I2CSend(frequencyL);
	
	I2CSend(0xB0);
	
	I2CSend(0x10);
	
	I2CSend(0x00);
	
	I2CStop();
}

void read()
{
	I2CInit();
	I2CStart();
	I2CSend(0xC1);
	read1 = I2CRead();
	I2CAck();
	read2 = I2CRead();
	I2CAck();
	read3 = I2CRead();
	I2CAck();
	read4 = I2CRead();
	I2CAck();
	read5 = I2CRead();
	I2CAck();
	I2CNak();
	I2CStop();
}
void main()
{
	
	send();
	delay(10);
	lcd_config();
	delay(50);
	
	while(1)
	{
		LCD_data = 0x01;
	  command();
	  busy();
		
		read();
		freq_available = (((read1&0x3F)<<8)+read2)*32768/4-225000;
		freq= freq_available/10000;
		readf[4] = (unsigned char)(freq/10000);
		readf[3] = (unsigned char) (freq/1000)-readf[4]*100;
	  readf[2] = (unsigned char)(freq/100)-readf[4]*100-readf[3]*10;
	  readf[1] = (unsigned char)(freq/10)-readf[3]*100-readf[2]*10;
	  readf[0] = (unsigned char)freq- readf[3]*1000-readf[2]*100-readf[1]*10;
		
		siglvl = read4;
		signal[2]=(unsigned char)(siglvl/100);
		signal[1]=(unsigned char) (siglvl/10)-signal[2]*10;
		signal[0]=(unsigned char)siglvl-signal[2]*100-signal[1]*10;
		
		stereo = read3 & 0x80;
		str [0]= stereo/10;
		dispdata("Freq ");
		
		
			datacon(readf[4]+0x30);
	    datacon(readf[3]+0x30);
		  datacon(readf[2]+0x30);
		  dispdata(".");
		  datacon(readf[1]+0x30);
		  datacon(readf[0]+0x30);
	
		dispdata("MHz");
		LCD_data=0xC0;
		command();
		busy();
		dispdata("Signal:");
		  datacon(signal[2]+0x30);
		  datacon(signal[1]+0x30);
		  datacon(str[0]+0x30);
     
			if(SW1==0)
		{
			delay(20);
		  if(SW1==0)
		 {
			 frequency=frequency+0.01;
			 send();
		 }
	  }
		if(SW2==0)
		{
			delay(20);
			if(SW2==0)
			{
				frequency=frequency-0.01;
				send();
			}
	  }
	  delay(10);
	}
}


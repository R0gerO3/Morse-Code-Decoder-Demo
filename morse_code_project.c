#include "TM4C123GH6PM.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
//create array of morsecode and alphabet
//create an infinite loop that assigns what each pushbutton is supposed to do
// Configure pushbuttons

/* LCD configuration

*/
void delayMs(int ms);
void mDelayMs(int ttime);
///////////////////////////////////////////////////////////////////////////////////
//PORTS_INIT FUNCTION
void PORTS_init(){
SYSCTL -> RCGCGPIO |= 0x01; //enable clk for port A
SYSCTL -> RCGCGPIO |= 0x10; //enable clk for port E
SYSCTL -> RCGCGPIO |= 0x04; //enable clk for port C
	
	
GPIOA->AMSEL &= ~0x3C;//uses pins 2-5 for the data pins D4-D7 0011 1100
GPIOA->DATA &= ~0x3C;
GPIOA->DIR |= 0x3C;
GPIOA->DEN |= 0x3C; 
	
GPIOE->AMSEL &= ~0x01;//for SS SPI for R/S of LCD pin 0  0000 0001
GPIOE->DIR |= 0x01; //Register select pin
GPIOE->DEN |= 0x01;
GPIOE->DATA |= 0x01;

GPIOC->AMSEL &= ~0x50;//CS or EN for LCD pin 4 0001 0000
GPIOC->DIR |= 0x50;//enable pin for LCD
GPIOC->DEN |= 0x50;
GPIOC->DATA &= ~0x50;
}

//LCD write data/command function
void LCD_nibble_write(char data, unsigned char control) {
 GPIOA->DIR |= 0x3C; //setting PA2-PA5 as outputs (interface to LCD)
 GPIOA->DATA &= ~0x3C; //clearing the line
 GPIOA->DATA |= (data & 0xF0) >> 2; //extracting the upper nibble
 /* set RS bit */
 if (control & 1) //control & RS
 GPIOE->DATA |= 1; //check if LCD data is data
 else
 GPIOE->DATA &= ~1; //check if LCD data is command

 /* sending a high to low transition pulse on LCD enable pin (PC6) */
 GPIOC->DATA |= 1 << 6;
 delayMs(0);
 GPIOC->DATA &= ~(1 << 6);
 GPIOA->DIR &= ~0x3C; //clearing the line
}

void LCD_command(unsigned char command) {//8 bit commands function
 LCD_nibble_write(command & 0xF0, 0); //upper nibble
 LCD_nibble_write(command << 4, 0); //lower nibble
 if (command < 3)
 delayMs(2); //maximum delay of 2 ms for the first two instructions
 else
 delayMs(1); //maximum delay of 1 ms for all other instructions
}

void LCD_init(void) {
 PORTS_init(); // CONTAINS CODE FROM LCD CONFIGURATION SECTION
 delayMs(20);
 LCD_nibble_write(0x30, 0);
 delayMs(5);
 LCD_nibble_write(0x30, 0);
 delayMs(1);
 LCD_nibble_write(0x30, 0);
 delayMs(1);
 LCD_nibble_write(0x20, 0); // use 4-bit data mode
 delayMs(1);
 LCD_command(0x28); // set 4-bit data, 2-line, 5x7 font
 LCD_command(0x06); // move cursor right
 LCD_command(0x01); // clear screen, move cursor to home
 LCD_command(0x0F); // turn on display, cursor blinking
}
//LCD write character data function
void LCD_data(char data) {
 LCD_nibble_write(data & 0xF0, 1); //upper nibble
 LCD_nibble_write(data << 4, 1); //lower nibble
 delayMs(1);
}
void TIMER_init(void){
SYSCTL->RCGCTIMER |= 1; //enable timer for timer block 0
	TIMER0->CTL = 0; //disable timer 0
	TIMER0->CFG = 0x04; //enable 16-bit operation
	TIMER0->TAMR = 0x02; //sets timer 0 to periodic mode & down counter
	TIMER0->TAILR = 16 - 1; //sets timer A interval value register to 15;;;number of clk cycles before timer reaches timeout
	TIMER0->ICR = 0x1; //clear timer A timeout flag
	TIMER0->CTL |= 0x01; //enable timer A
	
}
///////////////////////////////////////////////////////////////////////////////
const char english[40]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','1','2',
	'3','4','5','6','7','8','9','0','@','!','?',' '};//array of characters
const char* morsecode[40]={".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",
".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..",".----","..---","...--",
"....-",".....","-....","--...","---..","----.","-----",".--.-.","-.-.--","..--.."," "};//arrray of morsecode
//const char dash='-';
//const char period='.';
//const char space=' ';
int data;
unsigned char Data;
int count=0;
int C4=1911;
//int D4=1702;


int main(void){
    SYSCTL->RCGCGPIO |= 0x08; // Enable clk Port D
    GPIOD->DIR &= ~0x0F; // Set PD0-3 as input pins
    GPIOD->AMSEL &= ~0x0F; // Disable analog functionality
    GPIOD->DEN |= 0x0F; // Enable digital pins

    SYSCTL->RCGCGPIO |= 0x02; // Enable clk for port B
    GPIOB->DIR |= 0x0F; // Set PB0-3 as output pins
    GPIOB->DEN |= 0x0F; // Enable digital pins
		
		TIMER_init();
    LCD_init();
    char morse_input[6] = {0}; // Buffer to store Morse code input
    int morse_index = 0;
		
		
    while (1) {
    data = GPIOD->DATA & 0x0F;
		Data=GPIOD->DATA;
		GPIOB->DATA=GPIOD->DATA;
		GPIOC->DATA=Data;
			
        if (data) { // Check if any button is pressed
            switch(data) {
                case 0x08:
								for(int j=0;j<25;j++){
								GPIOC->DATA |=0x10;
								mDelayMs(C4);
								GPIOC->DATA &= ~0x10;
								mDelayMs(C4);
								}
		
                LCD_data('.');
                morse_input[morse_index++] = '.';
                delayMs(100);
                break;
                
								case 0x04:
								for(int j=0;j<75;j++){
								GPIOC->DATA |=0x10;
								mDelayMs(C4);
								GPIOC->DATA &= ~0x10;
								mDelayMs(C4);
								}
                    
								LCD_data('-');
                morse_input[morse_index++] = '-';
								delayMs(100);
                break;
                
								case 0x02://when use pushbutton, before decoding wait until sound stops
								for(int j=0;j<250;j++){
								GPIOC->DATA |=0x10;
								mDelayMs(C4);
								GPIOC->DATA &= ~0x10;
								mDelayMs(C4);
								}
                LCD_data(' ');
                morse_input[morse_index++] = ' ';
                delayMs(100);
                break;
   
								case 0x01:
                // Convert Morse code to character and display
                for (int i = 0; i < 40; i++) {
                if (strcmp(morse_input, morsecode[i]) == 0) {//equals to 0 means it's identical
									
								LCD_command(0xC0);//moves the answer down
								switch(count){
									case 1:
									LCD_command(0x14);
									break;
									case 2:
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 3:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 4:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 5:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 6:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 7:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 8:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 9:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 10:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 11:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 12:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 13:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 14:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
									case 15:
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									LCD_command(0x14);
									break;
								}//shift
								
                LCD_data(english[i]);//decodes							
								LCD_command(0x02);//moves cursor back to home					
								count++;
                break;									
								}
								}
                memset(morse_input, 0, sizeof(morse_input)); // Clear buffer
                morse_index = 0;
								delayMs(100);
								break;
   }//switch data
  }//if data
 } //inf while
		return 0;
}

void delayMs(int ms){
   int i, j;
    for(i = 0 ; i< ms; i++)
        for(j = 0; j < 6265; j++)
            {}  /* do nothing for 1 ms */
}
void mDelayMs(int ttime)//for sound
{
	int i;
	for(i = 0; i < ttime; i++) {
while ((TIMER0->RIS & 0x1) == 0); // wait for TimerA timeout flag
TIMER0->ICR = 0x1; // clear the TimerA timeout flag
}
}
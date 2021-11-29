/*Author: Jake Kim & Sami Zeremariam
*Date: 2021/02/18
*UCID: 30061206 & 30078936
*Tutorial: TUT 08
*Desc: This program interacts with the SNES controller from the Raspberry Pi
*. When a user presses a button, the corresponding button pressed will be displayed
* on the screen. When the user presses the start button, the program will terminate.
*/

/* C libraries that are used in the code below
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "initGPIO.h"
#include <wiringPi.h>

/* Constants used for the GPIO lines 
*/ 
#define GPSET0 7
#define GPCLR0 10
#define GPLEV0 13
#define LAT 9
#define DAT 10
#define CLK 11
unsigned int *GPIO_BASE;
#define INP_GPIO(g) *(GPIO_BASE+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(GPIO_BASE+((g)/10)) |= (1<<(((g)%10)*3))

/* This function initializes the GPIO lines, namely, the LATCH, CLOCK, and DATA lines. 
 * If the code argument is set to 1, then the line is first set to input then output. Otherwise,
 * the line is set to input
 * @param int line - an integer variable of the pin number
 * @param int code - an integer variable of input(0) or output(1)
 * @return void
*/ 
void init_GPIO(int line, int code){
	if (code == 1){
		INP_GPIO(line);
		OUT_GPIO(line);
	}else{
		INP_GPIO(line);
	}
}	

/* This function utilizes the built-in delayMicroseconds to wait for a certain amount of 
 * microseconds based on the argument passed as a parameter
 * @param int time - int variable of the amount of microseconds to be waited
 * @return void
*/
void wait(int time){
	delayMicroseconds(time);
}

/* This function writes a bit to the LATCH line. If the code given as an argument is 0, then 
 * the LATCH line bit is cleared, meaning 0 is written. Otherwise, the LATCH line is set to 1.
 * @param int code - int variable of the code. Either 0 or 1.
 * @return void
*/
void write_Latch(int code){
	if (code == 0){	
		GPIO_BASE[GPCLR0] = 1 << LAT;
	}else{
		GPIO_BASE[GPSET0] = 1 << LAT;
	}
}

/* This function writes a bit to the CLOCK line. If the code given as an argument is 0, then 
 * the CLOCK line bit is cleared, meaning 0 is written. Otherwise, the CLOCK line is set to 1.
 * @param int code - int variable of the code. Either 0 or 1.
 * @return void
*/
void write_Clock(int code){
	if (code == 0){	
		GPIO_BASE[GPCLR0] = 1 << CLK;
	}else{
		GPIO_BASE[GPSET0] = 1 << CLK;	
	}
}

/* This function reads a bit from the DATA line. 
 * @param - None
 * @return int - the bit read from the DATA line
*/ 
int read_Data(){
	return (GPIO_BASE[GPLEV0] >> DAT) & 1;
}

/* This function prints the proper message depending on which buttons have been pressed.
 * First, an array of strings is initialized which is used to print the proper button pressed.
 * Then a for loop is utilized to loop through the buttons and checks if a button has been pressed.
 * If the button that has been pressed is the "Start" button, then the program terminates.
 * Otherwise, proper message is displayed.
*/ 
void print_Message(int *array, int *check){
	
	char but[12][100] = {
							"B",
							"Y",
							"Select",
							"Start",
							"Joy-pad UP",
							"Joy-pad DOWN",
							"Joy-pad LEFT",
							"Joy-pad RIGHT",
							"A",
							"X",
							"Left",
							"Right"
						};
	for (int i = 0; i < 16; i++){
		
		//if (*(array + i) == 0 && i == 3) {
			//printf("Program is terminating...\n");
			//break;
		//} 
		// The "check" array is used to ensure that when a button is pressed continually,
		// the pressed message is only printed once.
		//else if (*(array + i) == 0 && i != 3 && *(array + i) != *(check + i)) {
			//printf("You have pressed ");
			//printf("%s\n", but+i);
		//	*(check + i) = 0;
	//	}
		
		if (*(array + i) == 1) {
			*(check + i) = 1;
		}
	}	
} 

/* This function loops through the DATA line 16 times and reads the state of the SNES
 * controller. Then, based on the state read, a value is stored in the array to determine 
 * whether a button has been pressed or not.
 * @param int *array - the array of ints for the buttons pressed or not
 * @param int *check - the array of ints for checking if the button has been pressed continually or not
 * @return void
*/ 
void read_SNES(int *array, int *check){
	for(int i = 0; i < 16; i++){
		wait(6);
		write_Clock(0);
		wait(6);
		int val = read_Data();	
		array[i] = val;
		write_Clock(1);
	}
}

/* This is the main function of this program. 
*/ 


int *setup () {
	
	// First, the base address of the GPIO is retrieved. Then, the CLOCK, LATCH, AND DATA
	// lines are initialized.

	//printf("Created by: Jake Kim and Sami Zeremariam\n");
	static int buttons[16];
	int check[16];
	
	// The two int arrays for the buttons are declared and initialized to 1
	for(int i = 0; i < 16; i++){
		buttons[i] = 1;
		check[i] = 1;
	}
	 
	//printf("Please press a button...\n");
	
	// The do while loop is utilized to start reading from the SNES controller. 
	// The do while loop condition fails and exits when the user presses the "Start" button,
	// causing the program to terminate. 
	//do{
		write_Clock(1);
		write_Latch(1);
		wait(12);
		write_Latch(0);
		read_SNES((int *)buttons, (int *)check);
		print_Message((int *)buttons, (int *)check);
		//wait(60000);
		
		//tweat
		return buttons;
		
//	}while(buttons[3] == 1);
	
	//return 0;
}

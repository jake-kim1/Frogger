/*Author: Jake Kim & Sami Zeremariam
*Date: 2021/04/22
*UCID: 30061206 & 30078936
*Tutorial: TUT 08
*Desc: This a a recreation of the classic game "Frogger". A user can control
* the movements of a frog using a SNES controller and the objective is to 
* make it to the destination after completing all challenges without running out of
* time or losing all lives or exceeding the maximum number of moves. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "framebuffer.h"
#include "frog.h"
#include "PP1.h"
#include "MainMenu1.h"
#include "MainMenu2.h"
#include "PauseMenu1.h"
#include "PauseMenu2.h"
#include "GameScreenGood.h"
#include "gift.h"
#include "gift2.h"
#include "gift3.h"
#include "gift4.h"
#include "pthread.h"
#include "car1.h"
#include "car2.h"
#include "car3.h"
#include "car4.h"
#include "GameWon.h"
#include "GameLost.h"
#include "snake1.h"
#include "snake2.h"

//define constants 
#define ROW 24
#define COLUMN 32

/* Definitions */
typedef struct {
	short int color;
	int x, y;
} Pixel;

struct fbs framebufferstruct;
void drawPixel(Pixel *pixel);
void *controllerThread (void *param);
void *obstacleThread (void *param);
void initializeScreens();



/* This function inititalizes the framebufferstruct 
 * @param - none
 * @return void
*/ 
void initializeFB() {
	framebufferstruct = initFbInfo();
}

int gameEnvTable[COLUMN][ROW];
int gameEnvCopy[COLUMN][ROW];


//Declare a struct of pointers to each image
struct screenPoints{
	short int *menu1Ptr;
	short int *menu2Ptr;
	short int *frogPtr;
	short int *gameScreenPtr;
	short int *pause1Ptr;
	short int *pause2Ptr;
	short int *car1Ptr;
	short int *car2Ptr;
	short int *car3Ptr;
	short int *car4Ptr;
	short int *gameWon;
	short int *gameLost;
	short int *gift1;
	short int *gift2;
	short int *gift3;
	short int *gift4;
	short int *snake1;
	short int *snake2;
};

//Inititalize the struct screenPoints
struct screenPoints sp;

//Declare a struct to monitor the game state, including score,
//coordinates, time, and moves
struct gameState{
		int score;
		int lives;
		int time;
		int moves;
		bool winFlag;
		bool loseFlag;
		bool transitionFlag;
		int frogX;
		int frogY;
};

//Inititalize the struct screenPoints
struct gameState state;

/* This is the main function of this program. 
*/ 
int main(){
	
	initializeFB(); //call function to initialize framebufferstruct
    pthread_t controlT; //create name for pthread controlT
    pthread_attr_t attr; //create attributes for pthread
    pthread_attr_init(&attr); //create attributes for pthread
    pthread_create(&controlT, &attr, controllerThread, NULL); //create pthread controlT

	// /* initialize + get FBS */
	// framebufferstruct = initFbInfo();

	initializeScreens(); //initialize screens with pixel data
	
	drawScreen(sp.menu1Ptr, 720, 1280, 0); //draw the main screen of the game
	
	pthread_join(controlT, NULL); //join pthread controlT with main
	return 0;
}

void initializeScreens(){
	sp.menu1Ptr=(short int *) MainMenu1.pixel_data; //set sp.menu1Ptr to contain MainMenu1.pixel_data
	sp.menu2Ptr = (short int *) MainMenu2.pixel_data; //set sp.menu2Ptr to contain MainMenu2.pixel_data
	
	sp.frogPtr = (short int *) Frog.pixel_data; //set sp.frogPtr to contain Frog.pixel_data
	sp.gameScreenPtr = (short int *) GameScreen.pixel_data; //set sp.gameScreenPtr to contain GameScreen.pixel_data
	
	sp.pause1Ptr = (short int *) PauseMenu1.pixel_data; //set sp.pause1Ptr to contain PauseMenu1.pixel_data
	sp.pause2Ptr = (short int *) PauseMenu2.pixel_data; //set sp.pause2Ptr to contain PauseMenu2.pixel_data
	
	sp.car1Ptr = (short int *) car1.pixel_data; //set sp.car1Ptr to contain car1.pixel_data
	sp.car2Ptr = (short int *) car2.pixel_data; //set sp.car2Ptr to contain car2.pixel_data
	sp.car3Ptr = (short int *) car3.pixel_data; //set sp.car3Ptr to contain car3.pixel_data
	sp.car4Ptr = (short int *) car4.pixel_data; //set sp.car4Ptr to contain car4.pixel_data

	sp.gameWon = (short int *) GameWon.pixel_data; //set sp.gameWon to contain GameWon.pixel_data
	sp.gameLost = (short int *) GameLost.pixel_data; //set sp.gameLost to contain GameLost.pixel_data

	sp.gift1 = (short int *) Gift.pixel_data; //set sp.gift1 to contain Gift.pixel_data
	sp.gift2 = (short int *) gift2.pixel_data; //set sp.gift2 to contain gift2.pixel_data
	sp.gift3 = (short int *) gift3.pixel_data; //set sp.gift3 to contain gift3.pixel_data
	sp.gift4 = (short int *) gift4.pixel_data; //set sp.gift4 to contain gift4.pixel_data

	sp.snake1 = (short int *) snake1.pixel_data; //set sp.snake1 to contain sanke1.pixel_data
	sp.snake2 = (short int *) snake2.pixel_data; //set sp.snake2 to contain sanke2.pixel_data
}

/* This function is a pointer to a function which
 * takes in a pointer to any type of variable 
 * @param *param - a pointer to any type of variable
 * @return void
*/ 
void *controllerThread (void *param){

	state.score = 0; //initialize state.score to 0
	state.lives = 4; //initialize state.lives to 4
	state.time = time(0); //initialize state.time to 0
	state.moves = 100; //initialize state.moves to 100
	state.winFlag = false; //initialize state.winFlag to false
	state.loseFlag = false; //initialize state.loseFlag to false
	state.transitionFlag = false; //initialize state.transitionFlag to false
	state.frogX = 15; //initialize state.frogX to 15
	state.frogY = 23; //initialize state.frogX to 23



	for (int i = 0; i < ROW; i++) //iterate through every cell in array 
	{
		for (int j = 0; j < COLUMN; j++)
		{
			gameEnvTable[i][j] = 0; //set cell [i][j] to 0 in gameEnvTable
			gameEnvCopy[i][j] = 0; //set cell [i][j] to 0 in gameEnvCopy
			gameEnvTable[15][23] = 1; //set cell [15][23] to 1 in gameEnvTable
			gameEnvCopy[15][23] = 1; //set cell [15][23] to 1 in gameEnvCopy
		}
	}


    //randomly set initial positions of obstacles (cars & snakes)
	gameEnvTable[2][1] = 6;
	gameEnvTable[6][1] = 7;
	gameEnvTable[16][1] = 6;
	gameEnvTable[30][1] = 7;
	gameEnvTable[4][2] = 6;
	gameEnvTable[10][2] = 7; 
	gameEnvTable[14][2] = 6;
	gameEnvTable[25][2] = 7;
	gameEnvTable[10][3] = 6;
	gameEnvTable[15][3] = 7;
	gameEnvTable[22][3] = 6;
	gameEnvTable[27][3] = 7;
	gameEnvTable[1][4] = 6;
	gameEnvTable[8][4] = 7;
	gameEnvTable[15][4] = 6;
	gameEnvTable[20][4] = 7;

	gameEnvTable[13][6] = 6;
	gameEnvTable[16][6] = 7;
	gameEnvTable[23][6] = 6;
	gameEnvTable[0][7] = 7;
	gameEnvTable[25][7] = 6;
	gameEnvTable[29][7] = 7;
	gameEnvTable[3][8] = 6;
	gameEnvTable[10][8] = 7;
	gameEnvTable[22][8] = 6;
	gameEnvTable[4][9] = 7;
	gameEnvTable[7][9] = 6;
	gameEnvTable[28][9] = 7;
	gameEnvTable[10][10] = 6;
	gameEnvTable[14][10] = 7;
	gameEnvTable[20][10] = 6;

	gameEnvTable[3][12] = 2;
	gameEnvTable[6][12] = 3;
	gameEnvTable[20][12] = 4;
	gameEnvTable[17][13] = 5;
	gameEnvTable[23][13] = 2;
	gameEnvTable[28][13] = 3;
	gameEnvTable[2][14] = 4;
	gameEnvTable[11][14] = 5;
	gameEnvTable[26][14] = 2;
	gameEnvTable[0][15] = 3;
	gameEnvTable[5][15] = 4;
	gameEnvTable[29][15] = 5;
	gameEnvTable[10][16] = 2;
	gameEnvTable[15][16] = 3;
	gameEnvTable[31][16] = 4;

	gameEnvTable[4][18] = 2;
	gameEnvTable[15][18] = 3;
	gameEnvTable[20][18] = 4;
	gameEnvTable[9][19] = 5;
	gameEnvTable[11][19] = 2;
	gameEnvTable[17][19] = 3;
	gameEnvTable[14][20] = 4;
	gameEnvTable[20][20] = 5;
	gameEnvTable[24][20] = 2;
	gameEnvTable[1][21] = 3;
	gameEnvTable[5][21] = 4;
	gameEnvTable[17][21] = 5;
	gameEnvTable[18][22] = 2;
	gameEnvTable[24][22] = 3;
	gameEnvTable[28][22] = 4;

	gameEnvTable[13][4] = 8;
	gameEnvTable[17][7] = 9;
	gameEnvTable[10][12] = 10;
	gameEnvTable[10][19] = 11;

	for (int j = 0; j < ROW; j++) //iterate through every cell in array
	{
		for (int i = 0; i < COLUMN; i++)
		{
			gameEnvCopy[i][j] = gameEnvTable[i][j]; //set every cell in EnvCopy to be equal to EnvTable
		}
	}

	int *newB; //initialize int newB to track button clicks 
	
	int mainScreen[5] = {1,0,0,0,0};  //initialize array mainScreen to track state of main screen 
	
	int gameScreen[5] = {1,0,0,0,0}; //initialize array gameScreen to track state of game screen
	
	int currentScreen[3] = {1, 0, 0}; //initialize array currentScreen to track which screen we are on 
	
	GPIO_BASE = getGPIOPtr(); //initialize GPIO_BASe by calling method in PP1.h
	init_GPIO(CLK, 1); //initialize clock line
	init_GPIO(LAT, 1); //initialize latch line
	init_GPIO(DAT, 0); //initialize data line 

	while (1){ //infinite while 
	    newB = setup(); //get array of buttons clicked 

        if ((time(0) - state.time) > 90){ //check if time has run out 
            int *endTransitionLose;
            state.loseFlag = 1; // set lose flag to set
			state.frogX = 8; // determine x position
			state.frogY = 5; //determine y position
			drawFrog(sp.gameLost, 360, 640); //draw the lose screen
			bool out = false; //set bool to false
			while (out == false){ //infinite loop
				endTransitionLose = setup(); //set up button inputs
					for (int i = 0; i < 16; i ++){
						if (endTransitionLose[i] == 0){ //check if ANY button pressed
							currentScreen[0] = 1;
							currentScreen[1] = 0;
							currentScreen[2] = 0;
							state.score = 0;
							state.lives = 4;
							state.time = time(0); //initialize state.time to 0
							state.moves = 100;
							state.winFlag = false;
							state.loseFlag = false;
							state.transitionFlag = false;
							state.frogX = 15;
							state.frogY = 23;

							drawScreen(sp.menu1Ptr, 720, 1280, 1); //draw main menu
							out = true; //exit while
						}
					}
       	 	}
		}

	

		for (int j = 0; j < ROW; j++)
		{
			for (int i = 0; i < COLUMN; i++)
			{
				gameEnvCopy[i][j] = gameEnvTable[i][j]; //set every cell in EnvCopy to be equal to EnvTable
			}
		}
		
		//control code for main screen 
		if (currentScreen[0] == 1){  //check if we are on the main menu
			
			if (newB[5] == 0){ //check if the down button has been clicked 
				drawScreen(sp.menu2Ptr, 720, 1280, 1 ); //draw menu2 
				mainScreen[0] = 0; //adjust main screen state 
				mainScreen[1] = 1; //adjust main screen state 
			}
			
			else if (newB[4] == 0){
				drawScreen(sp.menu1Ptr, 720, 1280, 1); //draw menu1 
				mainScreen[0] = 1; //adjust main screen state 
				mainScreen[1] = 0; //adjust main screen state 
					
				
			}
			
			if (mainScreen[0] == 1){
					if (newB[8] == 0){ //check if A button pressed 
						drawScreen(sp.gameScreenPtr, 720, 1280, 0); //draw the game screen
						drawFrog(sp.frogPtr, 30, 40); //draw the frog
						currentScreen[0] = 0; //adjust current screen state 
						currentScreen[1] = 1; //adjust current screen state 
					}
			}
				
			else if (mainScreen[1] == 1){ //check if quit game is selected 
					if (newB[8] == 0){ //Check if A button pressed 
						system("clear"); //clear console
						exit(0); //exit program 
					}
				}

		}
		
		//control code for game 	
		if (currentScreen[1] == 1){ //check current game state 
			if (newB[3] == 0){
				state.frogX = 5;
				state.frogY = 4; //center pause screen
				drawFrog(sp.pause1Ptr, 499, 875); //draw pause screen
				gameScreen[0] = 1; // adjust game screen states 
				gameScreen[1] = 0; // adjust game screen states 
				currentScreen[2] = 1; //adjust current screen state 
				
			}
		
			
			if (currentScreen[2] == 1){  //check current game state 
				
				if (newB[5] == 0){ //check if down is pressed
				    state.frogX = 5;
					state.frogY = 4;  //center pause screen
					drawFrog(sp.pause2Ptr, 499, 875); //draw pause screen
					gameScreen[0] = 0; // adjust game screen states 
					gameScreen[1] = 1;// adjust game screen states 
				}
			
				else if (newB[4] == 0){ //check if up is pressed 
					state.frogX = 5;
					state.frogY = 4;	//center pause screen		
					drawFrog(sp.pause1Ptr, 499, 875); //draw pause screen
					gameScreen[0] = 1; // adjust game screen states
					gameScreen[1] = 0; // adjust game screen states  
						
				}		
			
				if (gameScreen[0] == 1){ //check game state 
					if (newB[8] == 0){ //check if A is pressed 
						drawScreen(sp.gameScreenPtr, 720, 1280, 0); //draw the game screen 
						state.frogX = 15;
						state.frogY = 23; //centre frog 
						drawFrog(sp.frogPtr, 30, 40); //draw frog 
						currentScreen[0] = 0; //adjust current screen state
						currentScreen[1] = 1; //adjust current screen state
						currentScreen[2] = 0; //adjust current screen state

						gameScreen[0] = 1; // adjust game screen states
						gameScreen[1] = 0; // adjust game screen states
					}
				}
				
				else if (gameScreen[1] == 1){ //check game state
					if (newB[8] == 0){   //check if A is pressed 
						drawScreen(sp.menu1Ptr, 720, 1280, 1); //draw the main menu 
						currentScreen[0] = 1; //adjust current screen state
						currentScreen[1] = 0; //adjust current screen state
						currentScreen[2] = 0; //adjust current screen state

						gameScreen[0] = 1; // adjust game screen states
						gameScreen[1] = 0; // adjust game screen states

						state.frogX = 15;
						state.frogY = 23; //centre frog
					}
				
				}
				
			
			} else { //check if we are not in a menu 

				

				if (newB[4] == 0) { //check if up is pressed
					int upY = state.frogY - 1; //update frog y position in structure
					if (upY >= 0) { //check if frog is in valid y position 
						gameEnvTable[state.frogX][state.frogY] = 0;
						gameEnvCopy[state.frogX][state.frogY] = 0;

						state.frogY = upY;

						gameEnvTable[state.frogX][state.frogY] = 1;
						gameEnvCopy[state.frogX][state.frogY] = 1;

						drawScreen(sp.gameScreenPtr, 720, 1280, 1); //draw the game screen 
						drawFrog(sp.frogPtr, 30, 40); //draw the frog 

						state.transitionFlag = true; //set transition flag to true 
						state.moves--; //decrement moves by one 
						state.score += 10; //add 10 to total score 

						if (state.frogY == 0) { //check if frog has reached finish 
							int *endTransitionWin;
							state.winFlag = 1; //set the win flag 
							state.frogX = 8;
							state.frogY = 5; //center win screen 
							drawFrog(sp.gameWon, 360, 640); //draw the win screen 
							bool out = false;
							while (out == false){ //infinite loop 
								endTransitionWin = setup(); //set up button control 
								for (int i = 0; i < 16; i ++){
									if (endTransitionWin[i] == 0){ //check if ANY button pressed, reset game states
										currentScreen[0] = 1;
										currentScreen[1] = 0;
										currentScreen[2] = 0;
										state.score = 0;
										state.lives = 4;
										state.time = time(0); //initialize state.time to 0
										state.moves = 100;
										state.winFlag = false;
										state.loseFlag = false;
										state.transitionFlag = false;
										state.frogX = 15;
										state.frogY = 23;

										drawScreen(sp.menu1Ptr, 720, 1280, 1); //draw the main menu
										out = true; //exit infinite while 
									}
								}
								
							}

						}
					}
					
				} else if (newB[5] == 0) { //check if down is pressed 
					int downY = state.frogY + 1; //update frog y position in structure
					if (downY <= 23) { //check if frog is in valid y position 
						gameEnvTable[state.frogX][state.frogY] = 0;
						gameEnvCopy[state.frogX][state.frogY] = 0;

						state.frogY = downY;

						drawScreen(sp.gameScreenPtr, 720, 1280, 1); //draw the game screen 
						drawFrog(sp.frogPtr, 30, 40); //draw the frog

						gameEnvTable[state.frogX][state.frogY] = 1;
						gameEnvCopy[state.frogX][state.frogY] = 1;

						state.transitionFlag = true; //set transition flag to true 
						state.moves--;  //decrement moves by one 
					}
				} else if (newB[6] == 0) { //check if left is clicked 
					int leftX = state.frogX - 1; //update frog x position in structure
					if (leftX >= 0) { //check if frog is in valid x position
						gameEnvTable[state.frogX][state.frogY] = 0;
						gameEnvCopy[state.frogX][state.frogY] = 0;

						state.frogX = leftX;

						drawScreen(sp.gameScreenPtr, 720, 1280, 1); //draw the game screen 
						drawFrog(sp.frogPtr, 30, 40); //draw the frog

						gameEnvTable[state.frogX][state.frogY] = 1;
						gameEnvCopy[state.frogX][state.frogY] = 1;

						state.transitionFlag = 1; //set transition flag to true
						state.moves--; //decrement moves by one 
					}
				} else if (newB[7] == 0) { //check if right is clicked 
					int rightX = state.frogX + 1; //update frog x position in structure
					if (rightX <= 31) {  //check if frog is in valid y position
						gameEnvTable[state.frogX][state.frogY] = 0;
						gameEnvCopy[state.frogX][state.frogY] = 0;

						state.frogX = rightX;
						drawScreen(sp.gameScreenPtr, 720, 1280, 1); //draw the game screen
						drawFrog(sp.frogPtr, 30, 40); //draw the frog

						gameEnvTable[state.frogX][state.frogY] = 1;
						gameEnvCopy[state.frogX][state.frogY] = 1;

						state.transitionFlag = true; //set transition flag to true
						state.moves--; //decrement moves by one 
					}
				}

				for (int j = 0; j < ROW; j++)
				{
					for (int i = 0; i < COLUMN; i++) //iterate through 2d array 
					{
							int newCol = i - 1; 

							if (newCol >= 0) { //check if column valid
								gameEnvTable[newCol][j] = gameEnvTable[i][j]; //move corrdinate left
								gameEnvTable[i][j] = 0;
							} else { //if column invalid reset to rightmost cell
								gameEnvTable[31][j] = gameEnvTable[i][j];
								gameEnvTable[i][j] = 0;
							}
					
					}
				}


				wait(100000); //delay microseconds
				drawScreen(sp.gameScreenPtr, 720, 1280, 1); //draw the game screen 


				//draw objects based on structure 
				//frog is value one, car is 2-5, snake is 6-7
				// then check if 30 seconds has elaspsed, if it has draw value packs
				for (int j = 0; j < ROW; j++) 
				{
					for (int i = 0; i < COLUMN; i++)
					{
						if (gameEnvTable[i][j] == 1) {
							drawFrog(sp.frogPtr, 30, 40);
						} else if (gameEnvTable[i][j] == 2) {
							drawObstacles(sp.car1Ptr, 30, 30, i, j);
						} else if (gameEnvTable[i][j] == 3) {
							drawObstacles(sp.car2Ptr, 30, 30, i, j);
						} else if (gameEnvTable[i][j] == 4) {
							drawObstacles(sp.car3Ptr, 30, 30, i, j);
						} else if (gameEnvTable[i][j] == 5) {
							drawObstacles(sp.car4Ptr, 30, 30, i, j);
						} else if (gameEnvTable[i][j] == 6) {
							drawObstacles(sp.snake1, 30, 30, i, j);
						} else if (gameEnvTable[i][j] == 7) {
							drawObstacles(sp.snake2, 30, 30, i, j);
						} else if (gameEnvTable[i][j] == 8) {
							if ((time(0) - state.time) > 30)
							{
								drawObstacles(sp.gift1, 30, 30, i, j);
							}
						} else if (gameEnvTable[i][j] == 9) {
							if ((time(0) - state.time) > 30)
							{
								drawObstacles(sp.gift2, 30, 30, i, j);
							}
						} else if (gameEnvTable[i][j] == 10) {
							if ((time(0) - state.time) > 30)
							{
								drawObstacles(sp.gift3, 30, 30, i, j);
							}
						} else if (gameEnvTable[i][j] == 11) {
							if ((time(0) - state.time) > 30)
							{
								drawObstacles(sp.gift4, 30, 30, i, j);
							}
						}

					}
				}


			}
					
			
		}
	}

	
}



/* This function is a a function used to draw the standard screens the user interacts
 * with, including the main menu and game screen. 
 * @param *screenPtr - a pointer to the image to draw
 * @param height - an integer representing the image's height
 * @param width - an integer representing the image's width 
 * @param flag - a integer representing true or false, if true than free pixels
 * @return void
*/
void drawScreen(short int *screenPtr, int height, int width, int flag){

	short int *scr = screenPtr; //set scr to be screenPtr
	int h = height; 	//set variable h to height
	int w = width; //set variable w to width
	int f = flag; //set variable f to flag
	
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i = 0;
	//unsigned int quarter,byte,word;
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++) 
		{	
				pixel->color = scr[i]; //get color data from image
				pixel->x = x;
				pixel->y = y;
	
				drawPixel(pixel); //draw the pixel on the screen 
				i++;				
		}
	}
	if (f == 1){
		/* free pixel's allocated memory */
		free(pixel);
	}


}


/* This function is a a function used to draw the updated position of objects,
 * including the frog and menus. It takes in a pointer to the image to draw, as well as the width
 * and height of the image 
 * @param *screenPtr - a pointer to the image to draw
 * @param height - an integer representing the image's height
 * @param width - an integer representing the image's width 
 * @return void
*/
void drawFrog(short int *screenPtr, int height, int width){

	short int *scr = screenPtr; //set scr to be screenPtr
	int h = height; // set variable h to height
	int w = width; //set variable w to width
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	//unsigned int quarter,byte,word;
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++) 
		{	
				pixel->color = scr[i]; //get color data from image
				pixel->x = x + (40 * state.frogX); //determine offset of x coordinate 
				pixel->y = y + (30 * state.frogY); //determine offset of y coordinate 
	
				drawPixel(pixel); //draw the pixel on the screen
				i++;				
		}
	}
		/* free pixel's allocated memory */
		free(pixel);
}


/* This function is a a function used to draw the obstacles of the game the
 * frog must try and avoid (eg the cars and the snakes)
 * @param *screenPtr - a pointer to the image to draw
 * @param height - an integer representing the image's height
 * @param width - an integer representing the image's width 
 * @param xcoor - an integer representing the x coordinate to the place the image
 * @param ycoor - an integer representing the y coordinate to place the image
 * @return void
*/
void drawObstacles(short int *screenPtr, int height, int width, int xcoor, int ycoor){
	
	short int *scr = screenPtr; //set scr to be screenPtr
	int h = height; // set variable h to height
	int w = width; //set variable w to width
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	//unsigned int quarter,byte,word;
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++) 
		{	
				pixel->color = scr[i]; //get color data from image
				pixel->x = x + (40 * xcoor); //determine offset of x coordinate
				pixel->y = y + (30 * ycoor); //determine offset of y coordinate
	
				drawPixel(pixel); //draw the pixel on the screen 
				i++;				
		}
	}
		/* free pixel's allocated memory */
		free(pixel);
}


/* Draw a pixel */
void drawPixel(Pixel *pixel){
	long int location =(pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	*((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
}

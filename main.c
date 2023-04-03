#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

/***Subroutine prototypes***/
int get_user_input();
void clear_screen();
void wait_for_vsync();
void plot_pixel(int x, int y, short int color);
void generate_blocks();

/***Global variables***/
int pixel_buffer_start;

int main(void) {
  bool game_over = false;
  bool win_lose = false; // true for win, false for lose
  int highest_num = 1;
  int direction; //3 for up, 2 for down, 1 for left, 0 for right
  
  srand(time(NULL));  
  
  int random_num = rand() % 5; // generate a random number from 0-4
  
  volatile int *pixel_ctrl_ptr = (int *) 0xFF203020; // DMA

  // initialize the board
  int board [4][4] = {{0, 0, 0, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0}};
  *(pixel_ctrl_ptr + 1) = 0xC8000000;
  pixel_buffer_start = *(pixel_ctrl_ptr + 1);
  clear_screen();

  // swap BackBuffer and Buffer
  wait_for_vsync();
  *(pixel_ctrl_ptr + 1) = 0xC0000000;
  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // draw on BackBuffer

  // game loop
  while (!game_over) {
    // randomly generate blocks

    // get user input
    direction = get_user_input();

    // move blocks

    // merge blocks
  
  }

  // end of game
  
  // win or lose
  if (win_lose) {
      
  } else {
      
  }
  
  // print play time (timer)
    
  printf("%d ", board[0][0]);
  return 0;
}

// poll for user input on KEY3-0
int get_user_input() {
  volatile int *key_ptr = 0xff200050;
  volatile int *key_edge = 0xff20005c;

  int key = *key_ptr & 0x15;
  int edge = *key_edge;
  //check if key is pressed 
  while (key == 0){
     key = *key_ptr;
  }
  //wait for key to be released  
  while(edge != 0){
    *key_edge = edge;
    edge = *key_edge;
  }

  if (key == 1) {
    return 0;
  } else if(key ==  2){
    return 1;
  }else if(key == 4){
   return 2;
  }else if(key == 8){
   return 3;
  }
  else{
    return 0;
  }
}

// clear the drawing on the screen
void clear_screen() {
  int y, x;

  for (x = 0; x < 320; x++) {
    for (y = 0; y < 240; y++) {
      plot_pixel (x, y, 0);
    }
  }
}

void plot_pixel(int x, int y, short int color) {
  *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = color;
}

void wait_for_vsync() { 
  volatile int * pixel_ctrl_ptr = (int *) 0xFF203020; // DMA
  
  int status;
  
  *pixel_ctrl_ptr = 1; // start the synchronization process
  
  status = *(pixel_ctrl_ptr + 3);
  
  while ((status & 0x01) != 0) {
    status = *(pixel_ctrl_ptr + 3); 
  }
}

int** check_zeros() {
    
}

// randomly generate blocks
void generate_blocks(int highest) {
    
    // check where there are zeros
    
    // make an array with the x, y positions of the zeros
    
    // randomly generate a number to index the array
    
    // only generate 1's
    if (highest < 16) {
        
    } 
    // generate 1's or 2's
    else if (highest < 64) {
        
    } 
}



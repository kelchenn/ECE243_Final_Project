#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/***Subroutine prototypes***/
void get_user_input();
void clear_screen();
void wait_for_vsync();
void plot_pixel(int x, int y, short int color);

/***Global variables***/
int pixel_buffer_start;

int main(void) {
  bool game_over = false;
  
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
    get_user_input();

    // move blocks

    // merge blocks
  
  }

  // end of game
  // win or lose
  // print play time (timer)
    
  printf("%d ", board[0][0]);
  return 0;
}

// poll for user input on KEY3-0
void get_user_input() {
  
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



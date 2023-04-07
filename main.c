#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
	
/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

/***Subroutine prototypes***/
int get_user_input();
void clear_screen();
void wait_for_vsync();
void plot_pixel(int x, int y, short int color);
void generate_blocks();
void move_blocks(int dir);
void merge(int dir, int highest_num);
void draw_box(int xidx,int yidx, short int color, int size);
void draw_board();
void draw_line_horizontal(int xidx,int yidx, short int color, int size);
void draw_line_vertical(int xidx,int yidx, short int color, int size);

/***Global variables***/
int pixel_buffer_start;

// initialize the board
int board [4][4] = {{0, 0, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0}};

/** 1024 title name*/

int zeros[16][2];

int main(void) {
  bool game_over = false;
  bool win_lose = false; // true for win, false for lose
  int highest_num = 1;
  int direction; //3 for up, 2 for down, 1 for left, 0 for right
  
  srand(time(NULL));  
  
  volatile int *pixel_ctrl_ptr = (int *) 0xFF203020; // DMA

  *(pixel_ctrl_ptr + 1) = 0xC8000000;
  pixel_buffer_start = *(pixel_ctrl_ptr + 1);
  clear_screen();
	


  // swap BackBuffer and Buffer
  wait_for_vsync();
  *(pixel_ctrl_ptr + 1) = 0xC0000000;
  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // draw on BackBuffer
  clear_screen();
	
  wait_for_vsync();
  
	draw_board();

  // game loop
  while (!game_over) {
    // randomly generate blocks
    generate_blocks(highest_num);

    // get user input
    direction = get_user_input();

    // move blocks
    move_blocks(direction);

    // merge blocks
    merge(direction, highest_num);
  
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
  volatile int *key_ptr = (int *)0xff200050;
  volatile int *key_edge =  (int *)0xff20005c;

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

int check_zeros() {

    int counter = 0;
    
    // make an array with the x, y positions of the zeros
    for (int i = 0 ; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == 0) {
                zeros[counter][0] = i;
                zeros[counter][1] = j;
                counter++;
            }
        }
    }
    
   return counter;
}

// randomly generate blocks
void generate_blocks(int highest) {
    
    // check where there are zeros
    int count = check_zeros();

    // randomly generate a number to index the array
    int random_one = 0; 
    int random_two = 0; 
    
    // start of the game, generate 2 1 blocks
    if (highest == 1) {
        while (random_one == random_two) {
            // generate a random number from 0 to count - 1
            random_one = rand() % count; 
            random_two = rand() % count;
        }
        
        board[zeros[random_one][0]][zeros[random_one][1]] = 1;
        board[zeros[random_two][0]][zeros[random_two][1]] = 1;
    } 
    // generate 1 block of 1 or 2
    else {
        
        // generate a random number from 1 - 2
        random_one = rand() % (2 - 1) + 1; 
        
        // generate a random number from 0 to count - 1
        random_two = rand() % count;
        
        board[zeros[random_two][0]][zeros[random_two][1]] = random_one;
    } 
}

void move_blocks(int dir) {
    
    int move_x;
    int move_y;
    
    // move right
    if (dir == 0) {
        move_x = 1;
        move_y = 0;
        
        for (int row = 0; row < 4; row ++) {
            for (int col = 0; col < 4; col ++) {
                if (board[row][col] != 0) {
                    if (board[row + move_y][col + move_x] == 0) {
                        board[row + move_y][col + move_x] = board[row][col];
                        board[row][col] = 0;
                    }
                }
            }
        }
  
    } 
    // move left
    else if (dir == 1) {
        move_x = -1;
        move_y = 0;
        
        for (int row = 0; row < 4; row ++) {
            for (int col = 3; col >= 0; col --) {
                if (board[row][col] != 0) {
                    if (board[row + move_y][col + move_x] == 0) {
                        board[row + move_y][col + move_x] = board[row][col];
                        board[row][col] = 0;
                    }
                }
            }
        }
    } 
    // move down
    else if (dir == 2) {
        move_x = 0;
        move_y = 1;
        
        for (int row = 0; row < 4; row ++) {
            for (int col = 0; col < 4; col ++) {
                if (board[row][col] != 0) {
                    if (board[row + move_y][col + move_x] == 0) {
                        board[row + move_y][col + move_x] = board[row][col];
                        board[row][col] = 0;
                    }
                }
            }
        }
    } 
    // move up
    else {
        move_x = 0;
        move_y = -1;
        
        for (int row = 3; row >= 0; row --) {
            for (int col = 0; col < 4; col ++) {
                if (board[row][col] != 0) {
                    if (board[row + move_y][col + move_x] == 0) {
                        board[row + move_y][col + move_x] = board[row][col];
                        board[row][col] = 0;
                    }
                }
            }
        }
    }
}

void merge(int dir, int highest) {
    // check if next block is the same number, if it is merge
    // check if merged number is larger than highest number
}

//draw the box 
void draw_box(int xidx,int yidx, short int color, int size){
	int i, j;
	for(i = xidx; i<(xidx+ size);i++){ // need to check in the pixel space
		for(j = yidx; j<(yidx+ size);j++){
		 	plot_pixel(i, j, color);
		}
	}
}

void draw_board(){
	//draw the box 
	draw_box(70, 50, GREY, 180);
	
	//draw the verical lines 
	draw_line_vertical(115, 50, 0, 5);
	draw_line_vertical(160, 50, 0, 5);
	draw_line_vertical(205, 50, 0, 5);
	
	//draw horizontal lines 
	draw_line_horizontal(70, 95, 0, 5);
	draw_line_horizontal(70, 140, 0, 5);
	draw_line_horizontal(70, 185, 0, 5);
	
}

void draw_line_vertical(int xidx,int yidx, short int color, int size){
	int j;

	for(j = yidx; j<(yidx+180);j+= size){
		draw_box(xidx, j, color, size);
	}
}

void draw_line_horizontal(int xidx,int yidx, short int color, int size){
	int i;

	for(i = xidx; i<(xidx+180);i+= size){
		draw_box(i, yidx, color, size);
	}
}

	
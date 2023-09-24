# ECE243_Final_Project
ECE243 Final Project: the game 1024 on a DE1-SoC board with the ARMv7 and PS/2 keyboard inputs

# How to Play
To run the program there are two ways:
1. download main.c then create a project using the Monitor Program, compile and load the program
- include the flags -std=c99 -std=gnu11
- connect the PS/2 keyboard
2. go to https://cpulator.01xz.net/?sys=arm-de1soc and change the language to C
- copy and paste the code
- press Compile and Load (F5)
- press continue
   
To play:
- press any KEY3-0 to start
- use the up, down, left, right arrow keys on the PS/2 keyboard to move the blocks
- goal is to merge the blocks to get 1024
- there is a timer on the HEX and LEDR displays using interrupts
- when you win or lose the game, you will see an end screen telling you if you won or lose
- it will show your score (highest number reached) and the play time in seconds
- the HEX displays will show a scrolling message with your score 
- the LEDRs will show your play time
- to play again press any KEY3-0

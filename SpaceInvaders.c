// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10
// Last Modified: 3/6/2015 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5
// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "LCD.h"
#include "Sound.h"
#include "DAC.h"
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void SysTick_Handler(void);
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
//Variables
uint8_t *pt;

// *************************** Images ***************************
// enemy ship that starts at the top of the screen (arms/mouth closed)
// width=16 x height=10
const unsigned short SmallEnemy10pointA[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x079F, 0x0000, 0x0000, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x079F, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x079F, 0x0000, 0x0000,
 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000,
 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x079F, 0x079F, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};
// enemy ship that starts at the top of the screen (arms/mouth open)
// width=16 x height=10
const unsigned short SmallEnemy10pointB[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000,
 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000,
 0x0000, 0x0000, 0x079F, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x079F, 0x0000, 0x0000,
 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};
// enemy ship that starts in the middle of the screen (arms together)
// width=16 x height=10
const unsigned short SmallEnemy20pointA[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000,
 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000,
 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};
// enemy ship that starts in the middle of the screen (arms apart)
// width=16 x height=10
const unsigned short SmallEnemy20pointB[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000,
 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000,
 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000,
 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};
// enemy ship that starts at the bottom of the screen (arms down)
// width=16 x height=10
const unsigned short SmallEnemy30pointA[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x0000, 0x8ABF, 0x8ABF, 0x0000, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x8ABF, 0x0000, 0x8ABF, 0x8ABF, 0x0000, 0x8ABF, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};
// enemy ship that starts at the bottom of the screen (arms up)
// width=16 x height=10
const unsigned short SmallEnemy30pointB[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x8ABF, 0x0000, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x0000, 0x8ABF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x0000, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x0000, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x8ABF, 0x0000, 0x8ABF, 0x8ABF, 0x0000, 0x8ABF, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x8ABF, 0x8ABF, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8ABF, 0x8ABF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};
// image of the player's ship
// includes two blacked out columns on the left and right sides of the image to prevent smearing when moved 2 pixels to the left or right
// width=18 x height=8
const unsigned short PlayerShip0[] = {
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x20FD,
 0x20FD, 0x20FD, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x20FD, 0x20FD, 0x20FD, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x20FD, 0x20FD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};
// small shield floating in space to cover the player's ship from enemy fire (undamaged)
// width=18 x height=5
const unsigned short Bunker0[] = {
 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000,
};
const unsigned short bullet[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000,
 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000
};
const unsigned short star[] = {
 0xFFFF
};
const unsigned short staroff[]={
	0x0000
};
const unsigned short BlastBitMap[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x019F, 0xffff, 0x0000, 0x0000, 0x019F, 0x019F, 0x0000, 0x0000, 0x019F, 0x019F, 0x0000,
 0x0000, 0x019F, 0x019F, 0x0000, 0x0000, 0x019F, 0x019F, 0x0000, 0x0000, 0x019F, 0x019F, 0x0000, 0x0000, 0x019F, 0x019F, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000
};
// *************************** Capture image dimensions out of BMP**********
// *************************** Capture image dimensions out of BMP**********
typedef struct enemy_type {	//enemy of struct enemy_type, has life, x coordinate to build from, and y coordinate to build from
	int life;
	int x;
	int y;
} enemy;
typedef struct bulletStuff{//bS of struct bulletStuff with life, x corrdinate, and up coordinate to build up from
	int life;
	int up;
	int x;
} bS;
typedef struct Blast{//same as bS but for the Blast
	int life;
	int up;
	int x;
} BLAST;
typedef struct stars{//didnt use
	int x;
	int y;
} starstruct;
struct stars draw [100] = {0};
extern uint8_t shoot[];
extern uint8_t invaderkilled[];
volatile int delay;
enemy enemies[18];
BLAST blast[1];
bS bullets[4];
uint32_t soundcheck = 0;
uint32_t adcValue = 0;
uint8_t buttonPressed = 0;
uint8_t secondaryweapon = 0;
uint32_t switchAnimation = 0;
uint8_t eX = 1;
uint8_t eY = 0;
uint8_t moveBackward = 0;
uint8_t moveDown = 0;
uint32_t score = 0;
uint8_t endGame = 0;
uint8_t deadCount = 0;
uint8_t level = 4;
uint8_t button1previous=0;
uint8_t button1Released=0;
void Enemy_Init(void) {
	for (int i = 0; i < 18; i++) {
		enemies[i].x = (i % 6) * 20;				//spaces out the enemies (every 20 pixels)
		if (i < 6) {												//applies various life points and y coordinates to the enemy
			enemies[i].life = 3;							//makes first (ones at top) with 3 hp
			enemies[i].y = 9;
		}
		else if (i > 11) {
			enemies[i].life = 1;
			enemies[i].y = 41;								//sets y for middle row
		}
		else {
			enemies[i].life = 2;
			enemies[i].y = 25;								// sets y for closest row
		}
	}
	
}

void GPIO_Init (void){
	SYSCTL_RCGC2_R |= 0X10; // ENABLES THE CLOCK FOR PORT E
	delay = SYSCTL_RCGC2_R; // NOPS FOR 2 CYCLE
	GPIO_PORTE_DIR_R &= ~0X03; // MAKES BITS 0 1  INPUTS
	GPIO_PORTE_AFSEL_R &= ~0X03; // TURNS OFF ALTERNATE FUNCTION
	GPIO_PORTE_DEN_R |= 0X03; // DIGITAL ENABLE 0 1 
}
void SysTick_Init (void){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x0028b0aa;// reload value - setting systick 
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007; // enable SysTick with core clock and interrupts
};
void createstars(void){
	
}
void CreateBullet (void){																			//generates a bullet
	for(int i=0;i<4;i++){																				//only 4 bullets allowed at once
	if((bullets[i].life==0) && (soundcheck==1)) {								//if a bullet was not generated in one of the 4 bullet slots and soundcheck indicates a button had been pressed
		bullets[i].x = adcValue+8;																//offsets bullet to appear at center of spaceship rather than adc value
		bullets[i].life = 1;																			//bullet dies when it hits something
		bullets[i].up = 151;																			//bullet starts from top of ship
		Sound_Shoot(shoot,1);																			//calls sound shoot function to generate noise for bullet
		return;
		}
	}
}
void CreateBlast (void){																			//generates a blast
	
	if((blast[0].life==0) && (secondaryweapon==1)) {						//same as above but only 1 BLAST allowed and secondary weapon button pressed
		blast[0].x = adcValue+8;
		blast[0].life = 1;
		blast[0].up = 151;	
		Sound_Shoot(shoot,1);
		return;
	}
}
void SysTick_Handler(void){
	adcValue = ADC_In(); 																				//sets adc value to the adc input
	adcValue = (adcValue * 110) / 4096;													//converts the adcvalue to x location on the screen
	soundcheck  = (GPIO_PORTE_DATA_R & 0X01);										//indicates which buttons have been pressed
	secondaryweapon = (GPIO_PORTE_DATA_R & 0X02)>>1;
	if((secondaryweapon == 1) && (buttonPressed == 0 )){				//buttonPressed is state of acknowledgment of a button pressed
		CreateBlast();
		buttonPressed = 1;
	}
	if(secondaryweapon == 0) {																	
		buttonPressed = 0;																				//needs to be 0 for checks
	}
	if((soundcheck == 1) && (button1Released == 1)){						//only fires primary weapon on release of button
		CreateBullet();														
		buttonPressed = 1;																				
		button1Released=0;
}
	if((soundcheck == 0) && (button1previous==1)) {																				
		button1Released = 1;																			
	}
button1previous=soundcheck;																		//sets the previous state to a global variable that it can compare to
}																														
void CheckBullet(void){
	for(int i = 0; i < 4; i++){																
		if (bullets[i].life == 1) {
		if (bullets[i].up == -1){
			bullets[i].life = 0;
		}
		for (int j = 0; j < 18; j++) {
			if (enemies[j].life > 0) {
			if ((bullets[i].up - 8 < (eY+enemies[j].y)) && bullets[i].up - 8 > (eY+enemies[j].y-10)) {//subs 8 to get to top of bullet
				if ((bullets[i].x > (eX+enemies[j].x + 2)) && bullets[i].x < (eX+enemies[j].x+14)) {
					bullets[i].life = 0;
					enemies[j].life--;
					ST7735_FillRect(bullets[i].x, bullets[i].up - 8,4,9,0x0000);		//builds bullet from bottom to top, left to right
					if (enemies[j].life <= 0) {
						score += 1000;
						deadCount++;
						Sound_Killed(invaderkilled, 1);
					}
				}
				}
			}
		}
	}
}
}
void CheckBlast(void){
	for(int i = 0; i < 1; i++){	//goes through the blast array of 1
		if (blast[i].life == 1) {
		if (blast[i].up == -1){			//moves it up and kills the blast if the position is above the top of the screen
			blast[i].life = 0;
		}
		for (int j = 0; j < 18; j++) {	//hit check for all 18 enemies
			if (enemies[j].life > 0) {		//checks to see if the enemy is still alive
			if ((blast[i].up - 8 < (eY+enemies[j].y)) && blast[i].up - 8 > (eY+enemies[j].y-10)) {
				if ((blast[i].x > (eX+enemies[j].x )) && blast[i].x < (eX+enemies[j].x+14)) {
					blast[i].life = 0;
					enemies[j].life--;			//makes blast worth 2 hp
					enemies[j].life--;
					ST7735_FillRect(blast[i].x, blast[i].up - 8,4,9,0x0000);		//updates the position of the blast on tthe screen
					if (enemies[j].life <= 0) {
						score += 1000;				//increments score
						deadCount++;					//increments dead count
						Sound_Killed(invaderkilled, 1);		//plays the sound
					}
				}
				}
			}
		}
	}
}
}
uint32_t bulletCount = 0;
uint32_t blastCount = 0;
void MoveBullet(void){
	for(int i = 0; i < 4; i++){
		if(bullets[i].life == 1){
			if(bulletCount%25 == 0){		//only moves a bullet every 25th check
				if(bullets[i].up>=0){
					ST7735_DrawBitmap(bullets[i].x, bullets[i].up, bullet,4, 9);
					bullets[i].up--;;
					CheckBullet();
				}	
			} 
		}
	}
	bulletCount++;
}
void MoveBlast(void){
	for(int i = 0; i < 1; i++){
		if(blast[i].life == 1){
			if(blastCount%25 == 0){
				if(blast[i].up>=0){
					ST7735_DrawBitmap(blast[i].x, blast[i].up, BlastBitMap,4, 9);
					blast[i].up--;;
					CheckBlast();
				}	
			} 
		}
	}
	blastCount++;
}
void DrawEnemies(uint8_t level) {
	if (switchAnimation % (2000 / level) == 0) {//makes three rows of enemies of the three types
		for (int i = 0; i < 18; i++) {
			if (enemies[i].life > 0) {
				if (i < 6)
						ST7735_DrawBitmap(eX+enemies[i].x, eY+enemies[i].y, SmallEnemy30pointA,16,10);	//top 6 enemies
				else if (i > 11)
					ST7735_DrawBitmap(eX+enemies[i].x, eY+enemies[i].y, SmallEnemy10pointA, 16,10);		//bottom 6 enemies
				else
					ST7735_DrawBitmap(eX+enemies[i].x, eY+enemies[i].y, SmallEnemy20pointA, 16,10);		//middle 6 enemies
			}
			if (enemies[i].life <= 0) {
				ST7735_FillRect(eX+enemies[i].x, eY+enemies[i].y - 10,16,10,0x0000);								//makes black rectangle if the enemies are dead
			}
		}
	}
	else if (switchAnimation % (1000 / level) == 0) {																					//rotates between the two animations every 1000/level count
		for (int i = 0; i < 18; i++) {
			if (enemies[i].life > 0) {
				if (i < 6)
						ST7735_DrawBitmap(eX+enemies[i].x, eY+enemies[i].y, SmallEnemy30pointB, 16,10);
				else if (i > 11)
					ST7735_DrawBitmap(eX+enemies[i].x, eY+enemies[i].y, SmallEnemy10pointB, 16,10);
				else
					ST7735_DrawBitmap(eX+enemies[i].x, eY+enemies[i].y, SmallEnemy20pointB, 16,10);
			}
			if (enemies[i].life <= 0) {
				ST7735_FillRect(eX+enemies[i].x, eY+enemies[i].y - 10,16,10,0x0000);
			}
		}
		if (((eX == 0) || (eX == 12)) && (moveDown == 0)) {				//once it hits the a side of the screen move enemies down
				moveDown = 1;
				eY++;
				switchAnimation++;																		
				return;
			}
		if ((moveDown == 1) && (eY % 10 != 0)) {									//continue moving down until it reaches 10 pixels down
			eY++;
		}
		else if ((moveDown == 1) && (eY % 10 == 0)) { 						//after 10 pixels down, stop moving down, switchdirections
			moveDown = 0;
			moveBackward++;
			switchAnimation++;
		}
		if (moveDown == 0) {
			if ((eX < 12) && (moveBackward % 2 == 0)) {							//on even move backwards counts, go right
				eX += 1;
			}
			if ((eX > 0) && (moveBackward % 2 == 1)) {							//on odd move backwards counts, go left
				eX -= 1;
			}
		}
	}
	
	switchAnimation++;
}
void EndGame(void) {
	if (deadCount == 18) {
		if (endGame == 0) {
			DisableInterrupts();
			deadCount = 0;
			endGame = 1;
			Enemy_Init();
			level = 8;
			eX = 0;
			eY = 0;
		ST7735_FillScreen(0x0000);            // set screen to black
		ST7735_SetCursor(1, 5);
		ST7735_OutString("Good Job!");
		ST7735_SetCursor(1, 6);
		ST7735_OutString("Get Ready!");
		ST7735_SetCursor(1, 7);
		ST7735_OutString("Score: ");
		LCD_OutDec(score);
		Delay100ms(20);			
		ST7735_FillScreen(0x0000); 
		ST7735_SetCursor(7,8);
		ST7735_OutString("Level 2");	
		Delay100ms(20);
		ST7735_FillScreen(0x0000); 
		
 
	
		EnableInterrupts();
		return;
		}
		if (endGame == 1) {
			DisableInterrupts();
			deadCount = 0;
			endGame = 2;
			Enemy_Init();
			level = 10;
			eX = 0;
			eY = 0;
		ST7735_FillScreen(0x0000);            // set screen to black
		ST7735_SetCursor(1, 5);
		ST7735_OutString("They're even");
		ST7735_SetCursor(1, 6);
		ST7735_OutString("faster now...");
		ST7735_SetCursor(1, 7);
		ST7735_OutString("Score: ");
		LCD_OutDec(score);
		Delay100ms(20);			
		ST7735_FillScreen(0x0000); 
		ST7735_SetCursor(7,8);
		ST7735_OutString("Level 3");	
		Delay100ms(20);
		ST7735_FillScreen(0x0000);

			
		EnableInterrupts();
		return;
		}
		if (endGame == 2) {
		endGame = 2;
		ST7735_FillScreen(0x0000);  			// set screen to black
		ST7735_SetCursor(1, 5);
		ST7735_OutString("You beat the aliens");
		ST7735_SetCursor(1, 6);
		ST7735_OutString("Good job, earthling");
		ST7735_SetCursor(1, 7);
		ST7735_OutString("Score: ");
		LCD_OutDec(score);
			while (1){}
		}
}
	int i=0;
	for (i=0; i<18;	i++){
		if ((enemies[i].life>=1) && enemies[i].y+eY>=150){
		endGame = 3;
		ST7735_FillScreen(0x0000);            // set screen to black
		ST7735_SetCursor(1, 5);
		ST7735_OutString("The aliens beat you");
		ST7735_SetCursor(1, 6);
		ST7735_OutString("Sorry, earthling...");
		ST7735_SetCursor(1, 7);
		ST7735_OutString("Score: ");
		LCD_OutDec(score);
		}
	}
}

int main(void){
	Enemy_Init();
  TExaS_Init();  // set system clock to 80 MHz
	Random_Init(1);
	Sound_Init();
	GPIO_Init(); 
	SysTick_Init();
	ADC_Init();
	Output_Init();

	
	ST7735_FillScreen(0x0000);            // set screen to black
	ST7735_SetCursor(7,8);
	
	ST7735_OutString("Level 1");
	Delay100ms(20);
	
	int16_t loc = 0;
	ST7735_FillScreen(0x0000);
	EnableInterrupts();
		 uint32_t x1 = 0;
	 uint32_t y1=0;
	 uint32_t i = 0;

	
 while(1){

	 if (endGame < 3) {
	 if(adcValue>loc){
		 while(adcValue> loc){
		 loc = loc+2;
		ST7735_DrawBitmap(loc, 159, PlayerShip0, 18, 8);
}
		 }
	 else if(adcValue<loc){
		 while(loc>adcValue){
			 loc = loc-2;
		 ST7735_DrawBitmap(loc, 159, PlayerShip0, 18, 8);
	 }	
 }		  
		 else if(adcValue == loc){
			 ST7735_DrawBitmap(loc, 159, PlayerShip0, 18, 8);
		 }
		DrawEnemies(level);
		MoveBlast();
		MoveBullet();
		MoveBlast();
		EndGame();
	 }
 }
}

// You can use this timer only if you learn how it works
void Delay100ms(uint32_t count){
	uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
} 
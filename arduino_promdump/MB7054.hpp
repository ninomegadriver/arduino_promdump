#include "common.h"

/*
 * MB7054: 1K x 4 TTL PROM MEMORY
 * 
 * [Arduino Pin] [  Device Pin  ] [Arduino Pin]
 * 
 *        A0  <==[A6         VCC]==> Vcc
 *        A1  <==[A5    M     A7]==> 12
 *        A2  <==[A4    B     A8]==> 11
 *        A3  <==[A3    7     A9]==> 10
 *        A4  <==[A0    0     O1]==> 9
 *        A5  <==[A1    5     O2]==> 8
 *        A6  <==[A2    4     O3]==> 7
 *        A7  <==[CS~         O4]==> 6
 *        GND <==[GND       ~CS2]==> 5
 * 
 */

uint8_t MB7054_A[]  =     {A4, A5,  A6, A7, A3, A2, A1, A0, 12, 11, 10}; // Address Pins
uint8_t MB7054_O[]  =     { 9,  8,   7,  6};                             // Output Pins
uint8_t MB7054_CE[] =     {A7,  5};                                      // Chip Select/Enable Pins
uint8_t MB7054_SELECT[] = {0,   0};                                      // Status for enabling the chip

void config_MB7054(){
    A_pins =  sizeof  MB7054_A;
    CE_pins = sizeof  MB7054_CE;
    O_pins =  sizeof  MB7054_O;
    A=MB7054_A;
    O=MB7054_O;
    CE=MB7054_CE;
    SELECT = MB7054_SELECT;
    PROMsize = 1024;
}

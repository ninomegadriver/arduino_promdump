#include "common.h"

/*
 * MB7123: SHOCTTKY 4096 BIT DEAP PROM (512 x 8 BITS)
 * 
 * [Arduino Pin] [  Device Pin  ] [Arduino Pin]
 * 
 *        A0  <==[A0         VCC]==> +5v
 *        A1  <==[A1          A8]==> 12
 *        A2  <==[A2    8     A7]==> 11
 *        A3  <==[A3    2     A6]==> 10
 *        A4  <==[A4    S     A5]==> 9
 *        A5  <==[O1    1    ~CE]==> 8
 *        A6  <==[O2    4     O8]==> 7
 *        A7  <==[O3    1     O7]==> 6
 *        A8  <==[O4          O6]==> 5
 *        GND <==[GND         O5]==> 4
 * 
 */

uint8_t MB7123_A[]  =     {A0, A1,  A2, A3, A4, 9, 10, 11, 12};      // Address Pins
uint8_t MB7123_O[]  =     {A5, A6,  A7, A8,  4, 5,  6,  7};          // Output Pins
uint8_t MB7123_CE[] =     { 8};                                      // Chip Select/Enable Pins
uint8_t MB7123_SELECT[] = { 0};                                      // Status for enabling the chip

void config_MB7123(){
    A_pins =  sizeof  MB7123_A;
    CE_pins = sizeof  MB7123_CE;
    O_pins =  sizeof  MB7123_O;
    A=MB7123_A;
    O=MB7123_O;
    CE=MB7123_CE;
    SELECT = MB7123_SELECT;
    PROMsize = pow(2,A_pins);
    Vcc = 13;
    GND = A9;
}

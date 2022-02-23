#include "common.h"

/*
 * 82S141: 4096-BIT BIPOLAR PROM (512x8)
 * 
 * [Arduino Pin] [  Device Pin  ] [Arduino Pin]
 * 
 *        A0  <==[A7         VCC]==> Vcc
 *        A1  <==[A6          A8]==> 12
 *        A2  <==[A5    8     NC]==> 11
 *        A3  <==[A4    2   ~CE1]==> 10
 *        A4  <==[A3    S   ~CE2]==> 9
 *        A5  <==[A2    1    CE3]==> 8
 *        A6  <==[A1    4    CE4]==> 7
 *        A7  <==[A0    1     O8]==> 6
 *        A8  <==[O1          O7]==> 5
 *        A9  <==[O2          O6]==> 4
 *        A10 <==[O3          O5]==> 3
 *        GND <==[GND         O4]==> 2
 * 
 */

uint8_t S141_A[]  =     {A7, A6,  A5, A4, A3, A2, A1, A0, 12}; // Address Pins
uint8_t S141_O[]  =     {A8, A9, A10,  2,  3,  4,  5, 6};      // Output Pins
uint8_t S141_CE[] =     {10,  9,   8, 7};                      // Chip Select/Enable Pins
uint8_t S141_SELECT[] = {0,   0,   1, 1};                      // Status for enabling the chip

void config_S141(){
    A_pins =  sizeof  S141_A;
    CE_pins = sizeof  S141_CE;
    O_pins =  sizeof  S141_O;
    A=S141_A;
    O=S141_O;
    CE=S141_CE;
    SELECT = S141_SELECT;
    PROMsize = 512;
}

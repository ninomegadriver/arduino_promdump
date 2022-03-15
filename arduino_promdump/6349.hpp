#include "common.h"

/*
 * 53/6349-1, -2: 512x8 BIPOLAR PROM
 * 53/6348-1
 * 
 * [Arduino Pin] [  Device Pin  ] [Arduino Pin]
 * 
 *        A0  <==[A0         VCC]==> Vcc
 *        A1  <==[A1          A8]==> 12
 *        A2  <==[A2    8     A7]==> 11
 *        A3  <==[A3    2     A6]==> 10
 *        A4  <==[A4    S     A5]==> 9
 *        A5  <==[O1         ~CE]==> 8
 *        A6  <==[O2          O8]==> 7
 *        A7  <==[O3          O7]==> 6
 *        A8  <==[O4          O6]==> 5
 *        GND <==[GND         O5]==> 4
 * 
 */


uint8_t STFN_A[]  =     {A0, A1,  A2, A3, A4, 9, 10, 11, 12}; // Address Pins
uint8_t STFN_O[]  =     {A5, A6, A7,  A8,  4, 5,  6,  7};     // Output Pins
uint8_t STFN_CE[] =     {8};                                  // Chip Select/Enable Pins
uint8_t STFN_SELECT[] = {0};                                  // Status for enabling the chip

void config_6349(){
    A_pins =  sizeof  STFN_A;
    CE_pins = sizeof  STFN_CE;
    O_pins =  sizeof  STFN_O;
    A=STFN_A;
    O=STFN_O;
    CE=STFN_CE;
    SELECT = STFN_SELECT;
    PROMsize = pow(2,A_pins);
}

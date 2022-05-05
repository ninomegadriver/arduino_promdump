#include "common.h"

/*
 * 82S123: 256 BIT TTL BIPOLAR PROM 
 * 
 * [Arduino Pin] [  Device Pin  ] [Arduino Pin]
 * 
 *        A0  <==[O1         VCC]==> +5v
 *        A1  <==[O2     8   ~CE]==> 12
 *        A2  <==[O3     2    A4]==> 11
 *        A3  <==[O4     S    A3]==> 10
 *        A4  <==[O5     1    A2]==> 9
 *        A5  <==[O6     2    A1]==> 8
 *        A6  <==[O7     3    A0]==> 7
 *        GND <==[GND         O8]==> 6
 */

uint8_t S123_A[]  =     {7 ,  8,  9, 10, 11};            // Address Pins
uint8_t S123_O[]  =     {A0, A1, A2, A3, A4, A5, A6, 6}; // Output Pins
uint8_t S123_CE[] =     {12};                            // Chip Select/Enable Pins
uint8_t S123_SELECT[] = {0 };                            // Status for enabling the chip

void config_S123(){
    A_pins =  sizeof  S123_A;
    CE_pins = sizeof  S123_CE;
    O_pins =  sizeof  S123_O;
    A=S123_A;
    O=S123_O;
    CE=S123_CE;
    SELECT = S123_SELECT;
    PROMsize = pow(2,A_pins);
    Vcc = 13;
    GND = A7;
}

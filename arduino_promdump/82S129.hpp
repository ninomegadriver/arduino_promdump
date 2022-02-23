#include "common.h"

/*
 * 82S129: 1K BIT TTL BIPOLAR PROM
 * 
 * [Arduino Pin] [  Device Pin  ] [Arduino Pin]* 
 * 
 *        A0  <==[A6         VCC]==> Vcc
 *        A1  <==[A5    8     A7]==> 12
 *        A2  <==[A4    2   ~CE2]==> 11
 *        A3  <==[A3    S   ~CE1]==> 10
 *        A4  <==[A0    1     O1]==> 9
 *        A5  <==[A1    2     O2]==> 8
 *        A6  <==[A2    9     O3]==> 7
 *        Vcc <==[GND         O4]==> 6
 */


uint8_t S129_A[]  =     {A4, A5, A6, A3, A2, A1, A0, 12}; // Address Pins
uint8_t S129_O[]  =     { 9,  8,  7,  6};                 // Output Pins
uint8_t S129_CE[] =     {10, 11};                         // Chip Select/Enable Pins
uint8_t S129_SELECT[] = { 0, 0};                          // Status for enabling the chip

void config_S129(){
    A_pins =  sizeof  S129_A;
    CE_pins = sizeof  S129_CE;
    O_pins =  sizeof  S129_O;
    A=S129_A;
    O=S129_O;
    CE=S129_CE;
    SELECT = S129_SELECT;
    PROMsize = 32;
}

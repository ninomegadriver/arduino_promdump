/*  Arduino PROMdump
 *  Nino MegaDriver - nino@nino.com.br
 *  https://megadriver.com.br
 *  
 *  GPL v3.0
 *  
 *  Origianlly developed for the Mega 2560, but it may be also
 *  compatible with UNO, if your device has a compatible pin
 *  quantity.
 *  
 *  Pin and configurations for each supported device in
 *  it's own included file
 *  
 *  IMPORTANT:
 *  
 *  DONT'T EXCEED the 40ma current draw limit of the Arduino.
 *  Check the datasheet of your device first.
 *  
 *  Due to Arduino's "lack" of speed, reads will occur in >250ns, so the
 *  operation should not drain much current on most devices.
 *  
 *  But you've been warned.
 *  
 */

// Include the CRC library by Christopher Baker
#include "CRC32.h"


// Global variables
uint8_t  *A;                // Pointer to array holding the Address pins
uint8_t  *O;                // Pointer to array holding the Output pins
uint8_t  *CE;               // Pointer to array holding the CE/E pins
uint8_t  *SELECT;           // Pointer with the status of CE/E pins for enabling the chip
uint8_t  O_pins=0;          // Number of Output pins
uint8_t  A_pins=0;          // Number of Address pins
uint8_t  CE_pins=0;         // Number of CE pins
uint32_t PROMsize = 0;      // Prom size

// To ease bitwise operations
uint16_t bits[] = {
  0b0000000000000001,
  0b0000000000000010,
  0b0000000000000100,
  0b0000000000001000,
  0b0000000000010000,
  0b0000000000100000,
  0b0000000001000000,
  0b0000000010000000,
  0b0000000100000000,
  0b0000001000000000,
  0b0000010000000000,
  0b0000100000000000,
  0b0001000000000000,
  0b0010000000000000,
  0b0100000000000000,
  0b1000000000000000
};

// Individual device's configuration
#include "82S141.hpp"
#include "82S123.hpp"
#include "82S129.hpp"
#include "MB7054.hpp"
#include "MB7123.hpp"
#include "6349.hpp"

// Basic pin configuration for all devices
void basicSetup(){
  for(int i=0;i<CE_pins;i++) pinMode(CE[i], OUTPUT);
  for(int i=0;i<A_pins;i++) {
    pinMode(A[i] , OUTPUT);
    digitalWrite(A[i],LOW);
  }
  for(int i=0;i<O_pins;i++)  pinMode(O[i] , INPUT);

  
}

// Enables or disables the CHIP
void ENABLE(bool select){
  if(select == true){  // Enable CHIP
    for(int i=0;i<CE_pins;i++){
      if(SELECT[i] == 1) digitalWrite(CE[i], HIGH);
      else digitalWrite(CE[i], LOW);
    }
  }else{ // Disable CHIP, i.e: invert CE states
    for(int i=0;i<CE_pins;i++){ 
      if(SELECT[i] == 1) digitalWrite(CE[i], LOW); 
      else digitalWrite(CE[i], HIGH);
    }
  }
}

// Reads and returns the byte in "usable" form from the
// Output pins of the device
uint8_t getByte(bool fuse = false){
  String  fuses;
  uint8_t result = 0x00;
  uint8_t status;
  ENABLE(true); // Enable CHIP for reading
  for(int i=0; i<O_pins;i++){
    status = digitalRead(O[i]);
    result |= status << i;
    fuses.concat(status);  // Save each output state
    fuses.concat(":");
  }
  ENABLE(false); // Disable it for next read
  fuses.remove(fuses.length()-1,1);
  if(fuse == true) Serial.print(fuses); // Print each Ouput status
  return result;
}

String setAddress(uint16_t address){
  String ret;
  uint8_t status;
  for(uint8_t i=0; i<A_pins;i++){                    // Process all address pins
    if ((address & bits[i])==0) {                    // Check each bit
      digitalWrite(A[i], LOW);
      status = 0;
    }else{
      digitalWrite(A[i], HIGH);
      status = 1;
    }
    ret.concat(status); 
    ret.concat(":");    
  }
  ret.remove(ret.length()-1,1);
  return ret;
}


// Helper for parsing Serial commands
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// Run the configuration for selected device
bool config_device(String device){
  if(device.equals("82s141")){
    config_S141();
    basicSetup();
    return true;
  }else if(device.equals("82s123")){
    config_S123();
    basicSetup();
    return true;
  }else if(device.equals("82s129")){
    config_S129();
    basicSetup();
    return true;
  }else if(device.equals("mb7054")){
    config_MB7054();
    basicSetup();
    return true;
  }else if(device.equals("mb7123")){
    config_MB7123();
    basicSetup();
    return true;
  }else if(device.equals("6349") || device.equals("6348") ) {
    config_6349();
    basicSetup();
    return true;
  }
  return false;
}

// Main Menu :)
void MainMenu()
{
  Serial.println("****************************************************************************************");
  Serial.println("*                  PROMdump - Nino MegaDriver, nino@nino.com.br                        *");
  Serial.println("****************************************************************************************");
  Serial.println("Please input: [dump format] [lowercase device name]");
  Serial.println("Ex: text 82s123");
  Serial.println("");
  Serial.println("Dump formats:");
  Serial.println("");
  Serial.println("text   => Dump in readable text format");
  Serial.println("fuse   => Fuse/bits like text readable dump");
  Serial.println("bin    => Dump in raw binary format (for client applications)");
  Serial.println("");
  Serial.println("Available devices:");
  Serial.println("");
  Serial.println("82s123, 82s129, 82s141, 6348, 6349, mb7054, mb7123");
  Serial.println("****************************************************************************************");
  Serial.write(0x00); // Sync char for external C++ client

  
  while(!Serial.available() ){ }                   // Hang until any data is available
  String input = Serial.readStringUntil('\n');     // Read input command
  input.trim();                                    // Then parse it
  String cmd = getValue(input, ' ', 0);
  String device_string = getValue(input, ' ', 1);

  if(config_device(device_string) == false){       // Check and configure the selected device
    Serial.println("Invalid device");
    return;
  }

  // Let's start dumping!
  char message[200]; // To hold response strings
  CRC32 crc;
  uint32_t checksum;
  if(cmd.equals("fuse")){
    Serial.println(" ");
    Serial.println("# Fuse/bits like dump");
    Serial.println("# ");
    Serial.println("# Format: ");
    Serial.println("# [Adress Int][Address Pins Bits]:[Output Pins Bits]");
    Serial.println("# [0.........][0:1:2:3:4:5:6:8..]:[0:1:2:3:4:5:6...]");
    for(uint16_t address=0;address<PROMsize;address++){
        String ret = setAddress(address);
        sprintf(message, "[%03d][%s]:[", address, ret.c_str());
        Serial.print(message);
        uint8_t received_byte = getByte(true);
        crc.update(received_byte);
        sprintf(message, "]", received_byte);
        Serial.println(message);
    }
    checksum = crc.finalize();
    sprintf(message, "# CRC32: 0x%lX", checksum);
    Serial.println(message);
    Serial.println("#!"); // Sync char ("!") for client application
  }else if(cmd.equals("text")){
    Serial.println(" ");
    int col=0;
    for(uint16_t address=0;address<PROMsize;address++){
      String ret = setAddress(address);
      uint8_t received_byte = getByte(false);
      crc.update(received_byte);
      if(col==0){
        sprintf(message, "0x%0.6X:", address);
        Serial.print(message);
      }
      sprintf(message, "0x%0.2X ", received_byte);
      Serial.print(message);
      col++;
      if(col > 15) {
        col=0;
        Serial.println(" "); 
      }
    }
    checksum = crc.finalize();
    sprintf(message, "CRC32: 0x%lX", checksum);
    Serial.println(message);
    Serial.println(" ");
  }else if(cmd.equals("bin")){
    for(uint16_t address=0;address<PROMsize;address++){
      String ret = setAddress(address);
      uint8_t received_byte = getByte(false);
      Serial.write(received_byte);
    }
  }
    
}


void setup(){
  Serial.begin(115200);
  Serial.println("                      "); // Small flush for the Arduino's IDE serial monitor
}

void loop(){
  MainMenu();
}

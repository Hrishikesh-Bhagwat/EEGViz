
#include "PicoADS1299.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"

/*Original code developed by Joel Murphy and Conor Russomanno (Summer 2013) for Arduino Uno.
 * Modded by Indrayudd Roy Chowdhury (Spring 2023)
  This example uses the PicoADS1299 Arduino Library, a software bridge between the ADS1299 TI chip and 
  Raspberry Pi Pico.


  
  
  Pi Pico - Pin Assignments
  SCK = 2
  MISO [DOUT] = 4
  MOSI [DIN] = 3
  CS = 5; 
  RESET = 9;
  DRDY = 8;
  
*/

//#include "PicoADS1299.h"
// #include <filters.h>
//#include <FIRFilter.h> 

PicoADS1299 ADS;                           // create an instance of ADS1299

bool isDataReady=false;
bool isUpdated=false;
bool isStreaming=false;
unsigned long streamTime;
int count=0;

String cols[8]={"A","B","C","D","E","F","G","H"};

String longToCSV(long doublesArray[], int size_arr)
{
 String csvString = "";
 for (int i = 0; i < size_arr; i++)
 {
   csvString+=cols[i];
   csvString += String(doublesArray[i]*2.23*0.00001); // Convert double to string with 2 decimal places
   if (i < size_arr - 1)
   {
     csvString += "z"; // Add comma separator if not the last element
   }
 }
 return csvString;
}

void gpio_callback(uint gpio, uint32_t events) {
  if(gpio==10){
    isDataReady=true;
  }
}


void setup() {
//  delay(5000);
  gpio_set_irq_enabled_with_callback(10,GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
  // don't put anything before the initialization routine for recommended POR  
  ADS.initialize(10, 8, 5, 3, false); // (DRDY pin, RST pin, CS pin, SCK frequency in MHz);
  //Serial.println("ADS1299-Arduino UNO Example 2"); 
  delay(2000);             

  ADS.verbose = false;      // when verbose is true, there will be Serial feedback 
  ADS.RESET();             // send RESET command to default all registers
  ADS.SDATAC();            // exit Read Data Continuous mode to communicate with ADS
  //ADS.WREG(ID,0b00000001);
  //ADS.RREG(ID);
  ADS.WREG(CONFIG1,0x96);
  ADS.WREG(CONFIG2,0xC0);
  ADS.WREG(CONFIG3,0xEC);
  ADS.WREG(CH1SET,0x60);
  ADS.WREG(CH2SET,0xE1);
  ADS.WREG(CH3SET,0xE1);
  ADS.WREG(CH4SET,0xE1);
  ADS.WREG(CH5SET,0xE1);
  ADS.WREG(CH6SET,0xE1);
  ADS.WREG(CH7SET,0xE1);
  ADS.WREG(CH8SET,0xE1);
  ADS.WREG(MISC1,0b00100000);
  ADS.WREG(LOFF,0x00);
  ADS.WREG(BIAS_SENSP,0x06);
  ADS.WREG(BIAS_SENSN,0x02);
  ADS.WREG(LOFF_SENSP,0x00);
  ADS.WREG(LOFF_SENSN,0x00);
  //ADS.WREG(MISC1,0b00100000);
  //ADS.WREG(CONFIG4,0x00);
  
  ADS.RREGS(0x00,0x17);     // read all registers starting at ID and ending at CONFIG4
//  ADS.WREG(CONFIG3,0xE0);  // enable internal reference buffer, for fun
//  ADS.RREG(CONFIG3);       // verify write
  ADS.RDATAC();            // enter Read Data Continuous mode
  ADS.START();                    // start sampling at the default rate
  
  //Serial.println("Press 'x' to begin test");    // ask for prompt
} // end of setup

void loop(){
  if(isDataReady){
    ADS.updateChannelData();
    isDataReady=false;
    isUpdated=true;
  }
} // end of loop

void setup1(){
  Serial.begin(115200);
}

void loop1(){
  if(isUpdated && isStreaming){
    Serial.print("T"+String(millis()-streamTime)+"z");
    Serial.println(longToCSV(ADS.channelData,5));
    isUpdated=false;
  }
  if(Serial.available()){
    char c=Serial.read();
    if(c=='s'){
      isStreaming=true;
      count=0;
      streamTime=millis();
    }
    if(c=='!'){
      isStreaming=false;
      count=0;
    }
  }
}
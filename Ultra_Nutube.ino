// Code modified for "Ultra Nutube Headphone amplifier with AK4493"
// Mar. 31, 2019 kodera2t
/***************************************************************************************************

aKduino: Controlling an AK4490 with an Arduino.

http://www.dimdim.gr

v1.35    20/12/2016 : - Code cleanup for first public release.

v1.33    19/12/2016 : - Added full control of sound parameters through serial port.

v1.27    18/12/2016 : - First functional version.
                      - Automatic switching between PCM and DSD by monitoring DSDPIN.    
***************************************************************************************************/

/***************************************************************************************************
  Code starts here
 ***************************************************************************************************/

/*************************************************************************************************** 
  Make sure you use the correct chip address for your board. 
  The chip address is determined by how CAD0 & CAD1 are wired.
  (see below)                                                 
****************************************************************************************************/
#include <avr/sleep.h>
#include <Wire.h>
byte ak4490 = 0x10; // device ak4490, 0010011 = 0x13 (if CAD1=CAD0=H, see datasheet p. 50)


byte error;
byte r;
int mode = 0;
int dsdsignal = 0;    // Variable to hold whether the incoming signal is PCM or DSD. PCM = 0, DSD = 1.

#define PDN 4         // Set PDN pin to PB4 (for use with an STM32 uC) or any digital pin (for an Arduino). You should connect this pin to your 4490's PDN pin.
#define DSDPIN 3      // Set DSD pin to PB3 (for use with an STM32 uC) or any digital pin (for an Arduino). You should connect this pin to your USB-to-I2S module's "DSD indicator" pin.
//DSDPIN is not used in Ultra Nutube
/************************ MAIN PROGRAM ************************************************************/

void setup()
{
  for(int i=8;i<13;i++)
  pinMode(i, OUTPUT);

  digitalWrite(8,LOW);
  digitalWrite(9,LOW); 
  digitalWrite(10,LOW);
  digitalWrite(11,LOW); 
  digitalWrite(12,HIGH); //AK4490=LOW, AK4493=HIGH(1.8 LDO Enable)

  pinMode(PDN,OUTPUT);       // ak4490 PDN pin. 
  digitalWrite(PDN, LOW);    // Keep PDN pin low.
//  pinMode(DSDPIN,INPUT);     // DSD detection pin. 

  Serial.begin(9600);   // Start the serial port (for troubleshooting)
  
  Wire.begin();           // Join The I2C Bus As A Master 
  
  Wire.beginTransmission(ak4490);
  error = Wire.endTransmission();
  if (error == 0)
  {
    Serial.println("ak4490 found!");
  }
  else if (error==4) 
  {
    Serial.println("Unknown error at address 0x13");
  }
  else
  {
    Serial.println("No response from ak4490!");
  }

  DeviceInit(ak4490);
  SetPCM();
  //mode = 1;
//  displayHelp();
  SetVol(200);
  sei();
 set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
 sleep_enable();
 sleep_cpu ();
}                               

/*********************** Loop Section *************************************************************/

void loop()                 
{
//loop function is totally disable for Ultra Nutube
//If you want change or select AK4493 function (for exampke pre-defined 6-types digital filter)
//Please set it in setup() function.


//  SetPCM() ;
//dsdsignal = digitalRead(DSDPIN);
//if (dsdsignal == 1 && mode == 1)
//  {
//    SetDSD();
//    EnableDirectDSD(1);
//    SetDSDFilter(50);
//    mode = 2;
//    PrintDebugReg();
//  }
//
//if (dsdsignal == 0 && mode == 2)
//  {
//    EnableDirectDSD(0);
//    SetPCM();
//    mode = 1;
//    PrintDebugReg();
//  }
//
//char command = getCommand();
//switch (command)
//  {
//    case 'a':
//      EnableDirectDSD(1);
//      break;
//    case 'b':
//      EnableDirectDSD(0);
//      break;
//    case 'c':
//      SetDSDFilter(50);
//      break;
//    case 'd':
//      SetDSDFilter(150);
//      break;
//    case 'e':
//      SetFilter(1);
//      break;
//    case 'f':
//      SetFilter(2);      
//      break;
//    case 'g':
//      SetFilter(3);      
//      break;
//    case 'h':
//      SetFilter(4);
//      break;
//    case 'i':
//      EnableSuperslow(0);
//      break;
//    case 'j':
//      EnableSuperslow(1);
//      break;
//    case 'k':
//      SetSoundMode(1);
//      break;
//    case 'l':
//      SetSoundMode(2);
//      break;
//    case 'm':
//      SetSoundMode(3);      
//      break;
//    case 'p':
//      SetPCM();
//      break;
//    case 'q':
//      SetDSD();
//      break;
//    case '1':
//      SetVol(100);
//      break;
//    case '2':
//      SetVol(150);
//      break;
//    case '3':
//      SetVol(200);
//      break;
//    case '4':
//      SetVol(210);
//      break;
//    case '5':
//      SetVol(220);
//      break;
//    case '6':
//      SetVol(230);
//      break;
//    case '7':
//      SetVol(240);
//      break;
//    case '8':
//      SetVol(250);
//      break;
//    case '9':
//      SetVol(255);
//      break;
//    case 'y':
//      PrintDebugReg();
//      break;
//    case 'z':
//      displayHelp();
//      break;
//    default:
//      break;
//  }
} // loop



/****************************** Functions **********************************************************/

void SetVol (byte RegVal) 
{ 
  WriteRegister (ak4490, 0x03, RegVal); // Set Up Volume In DAC-L ATT 
  WriteRegister (ak4490, 0x04, RegVal); // Set Up Volume In DAC-R ATT
  Serial.print("Volume set to: ");
  Serial.println(RegVal);
} 

void SetDSD() 
{
  Serial.println("");
  Serial.println("Setting up for DSD.");
  byte temp = 0;
  temp = ReadRegister(ak4490, 0x02);
  bitSet(temp, 7);
  WriteRegister(ak4490,0x02,temp);          // Set To DSD Mode
  temp = 0;  
  WriteRegister(ak4490,0x00,B00000000);     // Reset
  WriteRegister(ak4490,0x00,B10001111);     // Set To Master Clock Frequency Auto / 24Bit I2S Mode
  WriteRegister(ak4490,0x06,B10011001);     // Set To DSD Data Mute / DSD Mute Control / DSD Mute Release
  WriteRegister(ak4490,0x09,B00000001);     // Set To DSD Sampling Speed Control  
}

void SetPCM() 
{
  Serial.println("");
  Serial.println("Setting up for PCM.");
  byte temp = 0;
  temp = ReadRegister(ak4490, 0x02);
  bitClear(temp, 7);
  WriteRegister(ak4490,0x02,temp);          // Set To PCM Mode
  temp = 0;  
  WriteRegister(ak4490,0x00,B00000000);     // Reset
  WriteRegister(ak4490,0x00,B10001111);     // Set To Master Clock Frequency Auto / 24Bit I2S Mode 
}

void SetFilter(int FiltNum) 
{
  byte filt = 0;
  switch (FiltNum) 
    {
      case 1:
      Serial.println("Sharp roll-off filter");              // SD=0, SLOW=0
      filt = ReadRegister(ak4490, 0x01);
      bitClear(filt, 5);
      WriteRegister(ak4490,0x01,filt);
      filt = 0;
      filt = ReadRegister(ak4490, 0x02);
      bitClear(filt, 0);
      WriteRegister(ak4490,0x02,filt);
      filt = 0;      
      break;

      case 2:
      Serial.println("Slow roll-off filter");               // SD=0, SLOW=1
      filt = ReadRegister(ak4490, 0x01);
      bitClear(filt, 5);
      WriteRegister(ak4490,0x01,filt);
      filt = 0;
      filt = ReadRegister(ak4490, 0x02);
      bitSet(filt, 0);
      WriteRegister(ak4490,0x02,filt);
      filt = 0;  
      break;

      case 3:
      Serial.println("Short delay sharp roll off filter");  // SD=1, SLOW=0
      filt = ReadRegister(ak4490, 0x01);
      bitSet(filt, 5);
      WriteRegister(ak4490,0x01,filt);
      filt = 0;
      filt = ReadRegister(ak4490, 0x02);
      bitClear(filt, 0);
      WriteRegister(ak4490,0x02,filt);
      filt = 0; 
      break;

      case 4:
      Serial.println("Short delay slow roll off filter");   // SD=1, SLOW=1
      filt = ReadRegister(ak4490, 0x01);
      bitSet(filt, 5);
      WriteRegister(ak4490,0x01,filt);
      filt = 0;
      filt = ReadRegister(ak4490, 0x02);
      bitSet(filt, 0);
      WriteRegister(ak4490,0x02,filt);
      filt = 0; 
      break;
    }
//PrintDebugReg();
}

void EnableSuperslow(bool superslow) 
{
  byte filt = 0;
  switch (superslow) 
    {
      case 0:
      Serial.println("Super Slow Roll-off Filter Disabled");
      filt = ReadRegister(ak4490, 0x05);
      bitClear(filt, 0);
      WriteRegister(ak4490,0x05,filt);
      filt = 0;
      break;

      case 1:
      Serial.println("Super Slow Roll-off Filter Enabled");
      filt = ReadRegister(ak4490, 0x05);
      bitSet(filt, 0);
      WriteRegister(ak4490,0x05,filt);
      filt = 0;
      break;
    }
//PrintDebugReg();
}

void EnableDirectDSD(bool DirectDSD) 
{
  byte dsd = 0;
  switch (DirectDSD) 
    {
      case 0:
      Serial.println("DSD Normal Path Set");
      dsd = ReadRegister(ak4490, 0x06);
      bitClear(dsd, 1);
      WriteRegister(ak4490,0x06,dsd);
      dsd = 0;
      break;

      case 1:
      Serial.println("DSD Direct Path Set");
      dsd = ReadRegister(ak4490, 0x06);
      bitSet(dsd, 1);
      WriteRegister(ak4490,0x06,dsd);
      dsd = 0;
      break;
    }
//PrintDebugReg();
}

void SetDSDFilter(int dsdfilter) 
{
byte dsd = 0;
switch (dsdfilter) {
      
      case 50:
      Serial.println("DSD Cut Off Filter at 50KHz");
      dsd = ReadRegister(ak4490, 0x09);
      bitClear(dsd, 1);
      WriteRegister(ak4490,0x09,dsd);
      dsd = 0;
      break;

      case 150:
      Serial.println("DSD Cut Off Filter at 150KHz");
      dsd = ReadRegister(ak4490, 0x09);
      bitSet(dsd, 1);
      WriteRegister(ak4490,0x09,dsd);
      dsd = 0;
      break;
    }
//PrintDebugReg();
}

void SetSoundMode(int soundcontrol) 
{
switch (soundcontrol) {
      
      case 1:
      Serial.println("Sound Setting 1");
      WriteRegister(ak4490,0x08,B00000000);
      break;

      case 2:
      Serial.println("Sound Setting 2");
      WriteRegister(ak4490,0x08,B00000001);
      break;
            
      case 3:
      Serial.println("Sound Setting 3");
      WriteRegister(ak4490,0x08,B00000010);
      break;
    }
//PrintDebugReg();
}

char getCommand()                                            // read a character from the serial port
{
  char c = '\0';
  if (Serial.available())
  {
    c = Serial.read();
  }
  return c;
}

void displayHelp()                                            // displays available commands through the serial port
{
  Serial.println("AK4490 DAC Controller");
  Serial.println();
  Serial.println("Press p to manually select PCM");
  Serial.println("Press q to manually select DSD");
  Serial.println("Press a to enable DirectDSD");
  Serial.println("Press b to disable DirectDSD");
  Serial.println("Press c to set DSD Filter at 50KHz");
  Serial.println("Press d to set DSD Filter at 150KHz");
  Serial.println("Press e to set the Sharp roll-off filter");
  Serial.println("Press f to set the Slow roll-off filter");
  Serial.println("Press g to set the Short delay sharp roll off filter");
  Serial.println("Press h to set the Short delay slow roll off filter");
  Serial.println("Press i to disable the Superslow filter");
  Serial.println("Press j to enable the Superslow filter");
  Serial.println("Press k to set Sound Setting 1");
  Serial.println("Press l to set Sound Setting 2");
  Serial.println("Press m to set Sound Setting 3");
  Serial.println("Press 1 to 9 to set volume from 100 to 255");
  Serial.println("Press y to display all registers");
  Serial.println("Press z to display this menu");
  Serial.println();
}


void DeviceInit(int devaddr)                                            // resets and initializes the DAC
{  
  Serial.println("Initializing AK4490");
  digitalWrite(PDN, LOW);   // Reset the DAC
  delay(1);
  digitalWrite(PDN, HIGH);  // Power up the DAC
  WriteRegister(devaddr, 0x00, B1001111);    // Initialize the DAC. Sets Auto MCLK & SF, I2S 32bit and RSTN=1
}

byte ReadRegister(int devaddr, byte regaddr)                                // Read a data register value
  {                              
    Wire.beginTransmission(devaddr);
    Wire.write(regaddr);
    Wire.endTransmission();
    Wire.requestFrom(devaddr, 1);                 // only one byte
    if(Wire.available())                          // Wire.available indicates if data is available
      return Wire.read();                         // Wire.read() reads the data on the wire
    else
    return 9999;                                  // In no data in the wire, then return 9999 to indicate error
  }

void WriteRegister(int devaddr, byte regaddr, byte dataval)                // Write a data register value
  {
    Wire.beginTransmission(devaddr); // device
    Wire.write(regaddr); // register
    Wire.write(dataval); // data
    Wire.endTransmission();
  }

void PrintDebugReg()
{
Serial.println("");

byte r = 0;
Serial.print("Register 00: ");
r = ReadRegister(ak4490, 0x00);
Serial.println(r,BIN);
r = 0;

Serial.print("Register 01: ");
r = ReadRegister(ak4490, 0x01);
Serial.println(r,BIN);
r = 0;

Serial.print("Register 02: ");
r = ReadRegister(ak4490, 0x02);
Serial.println(r,BIN);
r = 0;

Serial.print("Register 03: ");
r = ReadRegister(ak4490, 0x03);
Serial.println(r,BIN);
r = 0;

Serial.print("Register 04: ");
r = ReadRegister(ak4490, 0x04);
Serial.println(r,BIN);
r = 0;

Serial.print("Register 05: ");
r = ReadRegister(ak4490, 0x05);
Serial.println(r,BIN);
r = 0;

Serial.print("Register 06: ");
r = ReadRegister(ak4490, 0x06);
Serial.println(r,BIN);
r = 0;

Serial.print("Register 07: ");
r = ReadRegister(ak4490, 0x07);
Serial.println(r,BIN);
r = 0;

Serial.print("Register 08: ");
r = ReadRegister(ak4490, 0x08);
Serial.println(r,BIN);
r = 0;

Serial.print("Register 09: ");
r = ReadRegister(ak4490, 0x09);
Serial.println(r,BIN);
r = 0;
}

/****************************************************************************** 
MCP4725 Example Waveform Sketch
Joel Bartlett
SparkFun Electronics
Sept. 11, 2014
https://github.com/sparkfun/MCP4725_Breakout

This sketch takes data from a lookup table to provide 
waveforms to be generated by the MCP4725 DAC. 

Development environment specifics:
Arduino 1.0+
Hardware Version V14

This code is beerware; if you see me (or any other SparkFun employee) at the local, 
and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given. 

This code builds off the sketch written by Mark VandeWettering, which can be found here:
http://brainwagon.org/2011/02/24/arduino-mcp4725-breakout-board/
*/

#include <Wire.h>//Include the Wire library to talk I2C

//This is the I2C Address of the MCP4725, by default (A0 pulled to GND).
//Please note that this breakout is for the MCP4725A0. 
#define MCP4725_ADDR1 0x60
#define MCP4725_ADDR2 0x61 // A0 pin pulled up

//user difinition
#define STEER_CH_PIN           6     //pin connected to steering ch
#define THROTTLE_CH_PIN        7     //pin connected to throttle ch 
#define PWM_VAL_MAX            2000  //PWM input value max
#define PWM_VAL_MID            1500  //PWM input value middle
#define PWM_VAL_MIN            1000  //PWM input value min
#define PULSEIN_RETVAL_TIMEOUT 0     //pulseIn return value when it is timeout
#define PULSEIN_TIMEOUT        25000 //pulseIn timeout [us]
#define ONEBYTE_OFFSET         4     //offset bitnumber
#define DA_OUTVAL_MAX          4095  //MCP4725 I2C output value max
#define DA_OUTVAL_MID          2047  //MCP4725 I2C output value middle
#define DA_OUTVAL_MIN          0     //MCP4725 I2C output value min

int ch1; // Here's where we'll keep our channel values
int ch2;

//For devices with A0 pulled HIGH, use 0x61

//Sinewave Tables were generated using this calculator:
//http://www.daycounter.com/Calculators/Sine-Generator-Calculator.phtml

void setup()
{ 
  pinMode(STEER_CH_PIN, INPUT); // Set our input pins as such
  pinMode(THROTTLE_CH_PIN, INPUT);
  
  Wire.begin();
}
//---------------------------------------------------
void loop()
{
  int ch1  = PWM_VAL_MID;   //initial value is middle because it is safe
  int ch2  = PWM_VAL_MID;   //
  int val1 = DA_OUTVAL_MID; //if these are middle, motors stop.
  int val2 = DA_OUTVAL_MID; //
  
  
  ch1 = pulseIn(STEER_CH_PIN, HIGH, PULSEIN_TIMEOUT);    // Read the pulse width of 
  ch2 = pulseIn(THROTTLE_CH_PIN, HIGH, PULSEIN_TIMEOUT); // each channel
  
  if(ch1 != PULSEIN_RETVAL_TIMEOUT && ch2 != PULSEIN_RETVAL_TIMEOUT)
  {
    // convert value
    val1 = cnvrng(ch1);
    val2 = cnvrng(ch2);
  }
  else
  {
    // output middle value
    val1 = DA_OUTVAL_MID;
    val2 = DA_OUTVAL_MID;
  }
  // output I2C to MCP4725s
  Wire.beginTransmission(MCP4725_ADDR1);
  Wire.write(64);                                  // cmd to update the DAC
  Wire.write(val1 >> ONEBYTE_OFFSET);              // the 8 most significant bits...
  Wire.write((val1 & 0x0F) << ONEBYTE_OFFSET);     // the 4 least significant bits...
  Wire.endTransmission();
  
  Wire.beginTransmission(MCP4725_ADDR2);
  Wire.write(64);                                  // cmd to update the DAC
  Wire.write(val2 >> ONEBYTE_OFFSET);              // the 8 most significant bits...
  Wire.write((val2 & 0x0F) << ONEBYTE_OFFSET);     // the 4 least significant bits...
  Wire.endTransmission();
}

//range convert from PWM pulse width to MCP4725 12bit value 
int cnvrng(int _pwm)
{
  int out pwm;
  
  //constrain PWM value
  if(_pwm < PWM_VAL_MIN){
    pwm = PWM_VAL_MIN;
  }else if(_pwm > PWM_VAL_MAX){
    pwm = PWM_VAL_MAX;
  }else{
    pwm = _pwm;
  }

  //scale convert from PWM to 12bit output value
  out = (DA_OUTVAL_MAX - DA_OUTVAL_MIN) * (pwm - PWM_VAL_MIN) / (PWM_VAL_MAX - PWM_VAL_MIN) + DA_OUTVAL_MIN;

  //constrain 12bit output value
  if(out < DA_OUTVAL_MIN){
    out = DA_OUTVAL_MIN;
  }
  if(out > DA_OUTVAL_MAX){
    out = DA_OUTVAL_MAX;
  }
   
  return(out);
}



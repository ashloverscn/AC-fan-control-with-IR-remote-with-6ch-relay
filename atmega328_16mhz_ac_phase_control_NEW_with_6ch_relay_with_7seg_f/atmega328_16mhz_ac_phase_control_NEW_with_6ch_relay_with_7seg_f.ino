#include "Adafruit_NECremote.h"

#include <atmega328_16mhz_ac_phase_control.h>

#include <EEPROM.h>

#include <avr/io.h>

int spd[14]={600,500,480,450,400,380,350,300,250,180,150,80,50,1};//our good stage's values

//or direct valus from 623 to 1 NOTE: MIN_AC_POWER=623 and MAX_AC_POWER=1;

// Connect a 38KHz remote control sensor to the pin below

#define IRpin 3

#define up_key 6

#define down_key 5

#define toggle_rate 60 //optimised value for toggle

int value = 0; 

const int latchPin = A4;//Pin connected to ST_CP of 74HC595
const int clockPin = A5;//Pin connected to SH_CP of 74HC595 
const int dataPin = A3; //Pin connected to DS of 74HC595 
//display 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F,g,H
int datArray[18] = {252, 96, 218, 242, 102, 182, 190, 224, 254, 246, 239, 63, 157, 123, 159, 143, 247, 111};

Adafruit_NECremote remote(IRpin);

int L1=LOW;int L2=LOW;int L3=LOW;int L4=LOW;int L5=LOW;int L6=LOW;int L7=LOW;int L8=LOW;

int lastcode = -1;

int dim_value = 0;

void setup() {
  setup_io();//set pins as output for relays and triac
  //reboot_complete_indication();
  dim_value = EEPROM.read(0);//re innitialize fan value
  atmega328_16mhz_ac_phase_control.init();
  //atmega328_16mhz_ac_phase_control.uninit();
  atmega328_16mhz_ac_phase_control.set_ac_power(spd[dim_value]);//speed[0 to 19] or 0 to 360 through 400
  last_value_initialize();//inialize last value from epprom
  //Serial.begin(115200);
  //Serial.println("Ready to decode IR!");
   //set pins to output
}


void loop() {
  
  //Serial.println(value);
  // You can set the listen() time out to 'n' seconds
  int c = remote.listen(5);  // seconds to wait before timing out!
  // Or you can wait 'forever' for a valid code
  //int c = remote.listen();  // Without a #, it means wait forever

  if (c >= 0) {
    //Serial.print("Received code #"); 
    //Serial.println(c, DEC);
    lastcode = c;
    relay(c);
    //Serial.println(c);
    
  } 
    /*else if (c == -3) {
    delay(toggle_rate);
    relay(lastcode);
    //Serial.println(lastcode);
  } else if (c == -2) {
    //Serial.println("Data error");
  } else {
    //Serial.println("Timed out waiting!");
  }*/
  /*#########################################################################################################*/
}
 /*#########################################################################################################*/ 
 
void setup_io()
{
  for(int i=4;i<=13;i++)
  {
    pinMode(i,OUTPUT);
  }
  //shift register output pins
  pinMode(latchPin,OUTPUT);
  pinMode(clockPin,OUTPUT);
  pinMode(dataPin,OUTPUT);
}

 /*#########################################################################################################*/ 
 
 void last_value_initialize()
  {
    for(int i=4;i<=8;i++)
    {
    digitalWrite(i,EEPROM.read(i-3));
    }//skipping 9 which is triac pulse pin
    
    for(int i=10;i<=12;i++)
    {
    digitalWrite(i,EEPROM.read(i-3));
    }
    //reload fan speed display
    show_fan_speed(dim_value);
  }    
  
 /*#########################################################################################################*/ 
  
void relay(int key_pressed)
{
  switch(key_pressed)
  {
    case 10: 
      digitalWrite(4,L1);
      EEPROM.write(1,L1);
      L1 = ! L1;
    //Serial.println("1");
    break;

    case 27: 
      digitalWrite(5,L2);
      EEPROM.write(2,L2);
      L2 = ! L2;      
    //Serial.println("2");
    break;

    case 31: 
      digitalWrite(6,L3);
      EEPROM.write(3 , L3);
      L3 = ! L3;
    //Serial.println("3");
    break;

    case 12: 
      digitalWrite(7,L4);
      EEPROM.write(4 , L4);
      L4 = ! L4;
    //Serial.println("4");
    break;

    case 13: 
      digitalWrite(8,L5);
      EEPROM.write(5 , L5);
      L5 = ! L5;
    //Serial.println("5");
    break;

    case 14: 
      digitalWrite(10,L6);
      EEPROM.write(6 , L6);
      L6 = ! L6;
    //Serial.println("6");
    break;

    case 0: 
      digitalWrite(11,L7);
      EEPROM.write(7 , L7);
      L7 = ! L7;
    //Serial.println("7");
    break;

    case 15: 
      digitalWrite(12,L8);
      //EEPROM.write(8 , L8);
      L8 = ! L8;
    //Serial.println("8");
    break;

        case 5: 
      //Serial.println(EEPROM.read(0));
      if(dim_value>0)
      {
        dim_value -= 1 ;
        atmega328_16mhz_ac_phase_control.set_ac_power(spd[dim_value]);//speed[0 to 19] or 0 to 360 through 400 
        EEPROM.write(0, dim_value);
        show_fan_speed(dim_value);        
      }
    break;
        case 6: 
      //Serial.println(EEPROM.read(0));
      if(dim_value<13)
      {
       dim_value += 1;
       atmega328_16mhz_ac_phase_control.set_ac_power(spd[dim_value]);//speed[0 to 19] or 0 to 360 through 400 
       EEPROM.write(0, dim_value);
       show_fan_speed(dim_value);
      }
    break;

    default: Serial.println(key_pressed);
    break;
  }
  
}

 /*#########################################################################################################*/

 void show_fan_speed(int value)
 {  digitalWrite(latchPin,LOW);
    //clear the old date in the shift register
    shiftOut(dataPin,clockPin,LSBFIRST,~(0B00000000));
    //now write new data to the register
    shiftOut(dataPin,clockPin,LSBFIRST,~(datArray[value]));
    //inverting data for common andode display
    //return the latch pin high to signal chip that it 
    //no longer needs to listen for information
    digitalWrite(latchPin,HIGH); //pull the latchPin to save the data
 }
/*
 void reboot_complete_indication()
 {
  digitalWrite(latchPin,LOW);
  //clear the old date in the shift register
  shiftOut(dataPin,clockPin,LSBFIRST,~(0B00000000));
    //now write new data to the register
  shiftOut(dataPin,clockPin,LSBFIRST,~(0B00000001));//display a dot when boot completes
  //inverting data for common andode display
  //return the latch pin high to signal chip that it 
  //no longer needs to listen for information
  digitalWrite(latchPin,HIGH); //pull the latchPin to save the data
 }
*/


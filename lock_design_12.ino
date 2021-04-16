//need to include the servo library
#include <Servo.h>
#include <Arduino.h>
#include <ESP8266WiFi.h> 


//These are the pins variables4
#define Redpin 6
#define analogPin A0
#define Greenpin 5
const int buttonPin = 19;
//const int servoPin  = 18;
const int magnetPin =12;
const int sensorClosed = 21;
const int RFIDpin = 23;





//These are the other variables
int startTime, buttonPosition, RFIDcheck, closedCheck,RFIDstartTime,open1,closed,voltageIn,magnetlockstate;
bool opencheck; 

//This is the servo
//Servo myservo;


void setup()
{
  //Variables not pins
  closed = 1;
  open1 = 0;
  RFIDcheck = 0;
//  RFIDbadcheck = 0;
  buttonPosition =0;
  closedCheck = 0;
  //servoPosition = 0;
  magnetlockstate = 0;
  opencheck = false;
  



 // myservo.write(0);   // Represents a closed lock
 // myservo.attach(servoPin);
  
 
  pinMode(Redpin,OUTPUT);
  pinMode(Greenpin,OUTPUT);
  
  pinMode(buttonPin, INPUT);
  pinMode(RFIDpin, INPUT);
 
  
}

void loop()
{
  
  
//Here we need to check battery level, the only way to do this is to measure the analog c
///We need a voltage divider for this part. Max input to ADC is 1 V
   voltageIn = analogRead(analogPin)/1023;
   if(voltageIn<.8){
    //Battery is at 20% (2.66Volts)
    
   }
   else if(voltageIn<.87){
    //Batttery is at 50% capacity (2.9 Volts)
    
   }

  //servoPosition = myservo.read(); 
  magnetlockstate = digitalRead(magnetPin);
  if(digitalRead(buttonPin)==HIGH){
    buttonPosition =1;
    startTime = millis();
    
  }
  if(!(opencheck)&&(buttonPosition == 1)){
    if((millis()-startTime)<4000){
    
    //This is if someone wants to scan a card
    // If the Scan is good
      RFIDcheck = RFIDscan(RFIDpin,RFIDcheck,0,250,0,0);  
    // If the Scan is bad
    if((RFIDcheck ==0)&&(magnetlockstate==open1)){
      buttonPosition = 0;
      opencheck = true;
  }
  }
  buttonPosition = 0;
  }
  if((digitalRead(sensorClosed)==HIGH)&&(opencheck)){
  if(buttonPosition ==1){ //This is if somone presses the lock button
      
      lockButton(buttonPosition);
      buttonPosition = 0;
      opencheck = false;
     // goToSleep();
      
      //This is where we need to set the arduinon to sleep again
      }
  else if((digitalRead(sensorClosed)==HIGH)&&(closedCheck==0)){ //This is if someone forgets to close    
      if(magnetlockstate != closed){
        closedCheck=1;  
        startTime = millis();     
      }
        }  
  
    if(closedCheck==1)
    {  
      if((millis()-startTime) >= 1000)
      {
        setRGB(250,0);
         //myservo.write(closed);
         digitalWrite(magnetPin,HIGH);
         buttonPosition = 0;
        //This is where we need to set the arduino to sleep again
        closedCheck =0; 
        opencheck = false;
        //goToSleep();
              
        }
      }

  }
}



/*
void goToSleep(){
ESP.deepSleep(0);

}
*/ 


 


void lockButton(int buttonPosition)
{
 
  //locks together
  if(buttonPosition == 1){
   setRGB(250,0);   

   // myservo.write(closed);
    digitalWrite(magnetPin,HIGH);
  }
    if(magnetlockstate == closed){
      setRGB(100,0);
      buttonPosition =0; 
    }


  
  

}



 //_______________________________________________\\
/////         RFID SCAN FUNCTION     \\\\
//This encompases the RFID scan to be general for both scans which means less code
  
int RFIDscan(int RFIDpin,int RFIDcheck, int red,int green, int red2, int magnetpos){
  
  
  if((getScanResults(RFIDpin))&&(RFIDcheck==0)){
      RFIDcheck=1;
      RFIDstartTime = millis();
  }else if(!(getScanResults(RFIDpin))&&(RFIDcheck==0)){
     red = 250;
     green = 0;
     red2 = 250;
     magnetpos = closed;
     RFIDcheck=1;
  }
  
  if(RFIDcheck==1){     
      setRGB(red,green);
    
    //myservo.write(servopos);
     digitalWrite(magnetPin,magnetpos);
        if((millis()-RFIDstartTime)>=500)
        {
          setRGB(red2,0);        
          RFIDcheck = 0;         
        }
  }  
  return RFIDcheck;
}
/////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\



  







 //_______________________________________________\\
////   This sets the rgb to its distinct colours \\\\

// Red - Bad Scan / Locked
// Green - Good Scan
// Blue - not used
void setRGB(int red,int green)
{
      analogWrite(Redpin,red);
    analogWrite(Greenpin,green);
        
}

////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\







 //_______________________________________________\\
///     This checks the state of a given button \\\\\\

// In Final Product it will be replaced with checking 
// whether a user is recognized in the system.

/////The digitalRead is to be replaced with the RDID Scan code

bool getScanResults(int RFID){
  if(digitalRead(RFID)==HIGH){///Here is where we check wether the user is recognized
  return true;                /// If the user is recognized
  } 
  else if(digitalRead(RFID)==LOW){
    return false;             /// If the user is not recognized
    
  }  
}

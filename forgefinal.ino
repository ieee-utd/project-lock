
int buttonstate, startTime, buttonchange,startTimeRFID;
int goodscan, badscan, noscan,scancheck;
int truecheck;
int redblinktime;
int noscanblinktime;
int isopen,isclosed;
bool hasBeenOpened = false;
/////////////////////////////
#define SS_PIN 21  //D2
#define RST_PIN 22 //D1

#define buttonPin 33
#define magnetPin 25
#define redPin 27
#define greenPin 26
#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int statuss = 0;
int out = 0;
//////////////////////////////
void setup() {
  buttonchange = 0;
  Serial.begin(9600);
  truecheck = 0;
  isopen = 650;
  isclosed = 25;
 
 scancheck = 0;
 
  buttonstate = 0;

  goodscan = 2;
  badscan = 1;
  noscan = 0;
  ////////////////////////////////////////
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();
  ////////////////////////////////////////
  pinMode(buttonPin,INPUT);
  pinMode(magnetPin,OUTPUT);
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
}

void loop() {
  if ((buttonstate == 0) && (digitalRead(33)==HIGH)) {
    buttonchange = 1;
    Serial.println("button was pressed");
    startTimeRFID = millis();
    startTime = millis();
    truecheck = 0;
    Serial.println("button first time");
  }
  if ((buttonchange == 1) && (digitalRead(33)==LOW)) {
    buttonstate = 1;
    buttonchange = 0;
    Serial.println("button was released");
  }
 
    if (buttonstate == 1) {
      Serial.println("button state = 1");
      if ((hasBeenOpened == false)) {
        if ((millis() - startTime) < 2000) {
          Serial.println("begining of scan");
          if (RFIDScan(startTimeRFID) == true) {
            Serial.println("a scan");

            buttonstate = 0;
            if (truecheck == 1) {
              hasBeenOpened = true;
            }
          }
        }
        if (buttonstate == 1) {
          noscanblinktime = millis();
          digitalWrite(magnetPin,HIGH);
          ledcontrol(0,0);
          while((millis()-noscanblinktime)<500){
                Serial.println("no scan wait");
          
          }
          ledcontrol(0,1);
          //go to sleep
          hasBeenOpened = false;
          buttonstate = 0;
        }
      } else if ((hasBeenOpened == true)) {
        Serial.println("button lock");
        ledcontrol(0, 1);
        digitalWrite(magnetPin, 1);
        buttonstate = 0;
        hasBeenOpened = false;

      }

    }
  

}

bool RFIDScan(int startTimeRFID) {
  int scanresults = getScanResults(startTimeRFID);
  if (scanresults == goodscan) {
    Serial.println("good scan");
    ledcontrol(1, 0); //green on, red off
    digitalWrite(magnetPin, LOW);
    truecheck =1;
    return true;
  }
  else if (scanresults == badscan) {
    Serial.println("bad scan");
    redblinktime = millis();
    digitalWrite(magnetPin, HIGH);
    ledcontrol(0,0);
    while((millis()-redblinktime)<500){
      Serial.println("red wair");
      
    }
    ledcontrol(0,1); //green on, red off
    
    truecheck = 0;
    return true;
  }
  else {
    Serial.println("no scan");
    return false;
  }
}
void ledcontrol(int green, int red) {
  Serial.println("led is changed");
  if(green ==1){
  digitalWrite(greenPin, HIGH);
  Serial.println("Green on");
  }
  if(green ==0){
digitalWrite(greenPin, LOW);
}
  if( red ==1){
  digitalWrite(redPin, HIGH);
  }
  if( red ==0){
digitalWrite(redPin, LOW);
}
}


int getScanResults(int startTimeRFID) {
  // Look for new cards
  while((millis()-startTimeRFID)< 3000){
  if (  mfrc522.PICC_IsNewCardPresent())
  {
    scancheck =1;
    break;
  }
  }
  if(scancheck ==1){
    scancheck = 0;        
  }
  else{
      return noscan;
    
  }
  while( true) 
  {
    if(! mfrc522.PICC_ReadCardSerial()){
      
    break;
    }
  }
  // Select one of the cards
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :" );
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.print(" UID tag :" + content.substring(1));
  Serial.println();
  if (content.substring(1) == "C7 6D FC D8") //change UID of the card that you want to give access
  {
    //this is where instead of checking the value we need to check if the log in works
    statuss = 1;
    return goodscan;
  }

  else   {
    Serial.println(" Access Denied ");
    return badscan;
  }




}

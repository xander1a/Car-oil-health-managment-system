#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//GSM Module TX is connected to Arduino D8
#define SIM800_TX_PIN 11
#define SIM800_RX_PIN 10
SoftwareSerial mySerial(SIM800_TX_PIN, SIM800_RX_PIN);
//GSM Module RX is connected to Arduino D9

LiquidCrystal_I2C lcd(0x27,20,4); 

int device_1 = 4; // attach pin D13 of Arduino to Device-1
int device_2 = 12; // attach pin D13 of Arduino to Device-2
#define REPORTING_PERIOD_MS2     1000
uint32_t tsLastReport2 = 0;
#define REPORTING_PERIOD_MS3     1000
uint32_t tsLastReport3 = 0;
///////////////////
// defines variables
int _timeout;
int index = 0;
String number = "";
String message = "";
String mynbr = "0783159293";
char incomingByte;
String incomingData;
bool atCommand = true;
String _buffer;
////////////////////////////////////////////////////////////////////////////////

const int buttonPin = 9;  

int sensor = 2;

// the pin that the pushbutton is attached to
const int ledPin = 4;       // the pin that the LED is attached to
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
unsigned long AddressRom = 0; 
int remained = 0;
int buzzer1 =7;
///////////////////////////////////////////////////////////////////////////////////





unsigned long start_time = 0;
unsigned long end_time = 0;
int steps=0;
float steps_old=0;
float temp=0;
float rps=0;
int driven=0;







void setup()
{
 
  
  /*lcd.init();                      // initialize the lcd 
  lcd.init();
  lcd.backlight();
 ////////////////////////////////////////////////////////////////////////////////////////////////////////
  */ 
  Serial.begin(9600); 
  Serial.println("Welcome");
  mySerial.begin(9600); 
  pinMode(device_1, OUTPUT); //Sets the device_1 as an OUTPUT
  pinMode(device_2, OUTPUT); //Sets the device_2 as an OUTPUT
  pinMode(buzzer1, OUTPUT);
  digitalWrite(device_1, LOW); //Sets the device_1 in to OFF state at the beginning
  digitalWrite(device_2, LOW); //Sets the device_2 in to OFF state at the beginning
  pinMode(buttonPin, INPUT);
// Check if you're currently connected to SIM800L
  while (!mySerial.available()) {
    mySerial.println("AT");
  delay(1000);
  Serial.println("connecting....");
  
  }

  Serial.println("Connected..");

  mySerial.println("AT+CMGF=1");  //Set SMS Text Mode
  delay(1000);
  mySerial.println("AT+CNMI=1,2,0,0,0");  //procedure, how to receive messages from the network
  delay(1000);
  mySerial.println("AT+CMGL=\"REC UNREAD\""); // Read unread messages

  Serial.println("Ready to received Commands..");
    digitalWrite(device_1, HIGH); //Sets the device_1 in to OFF state at the beginning
  digitalWrite(device_2, HIGH);
   digitalWrite(buzzer1, HIGH);
   delay(500); 
   digitalWrite(buzzer1, LOW);  
delay(1000);
lcd.init();                      // initialize the lcd 
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(device_1, OUTPUT); //Sets the device_1 as an OUTPUT
  pinMode(device_2, OUTPUT); 
  pinMode(sensor,INPUT_PULLUP);
  lcd.setCursor(2,0);
  lcd.print("Hello Driver!");
  lcd.setCursor(0,1);
  lcd.print("Are you doing well!");
  lcd.setCursor(0,2);
  lcd.print(" DRIVEN -");
  lcd.setCursor(0,3);
  lcd.print(" REMAINED - 0.00");
  digitalWrite(device_1, HIGH); //Sets the device_1 in to OFF state at the beginning
  digitalWrite(device_2, HIGH);
 callNumber();
}

void loop()
{
 if (mySerial.available()) {
    delay(100);

    // Serial buffer
    while (mySerial.available()) {
      incomingByte = mySerial.read();
      incomingData += incomingByte;
    }

    delay(10);
    if (atCommand == false) {
      receivedMessage(incomingData);
    } else {
      atCommand = false;
    }

    //delete messages to save memory
    if (incomingData.indexOf("OK") == -1) {
      mySerial.println("AT+CMGDA=\"DEL ALL\"");
      delay(1000);
      atCommand = true;
    }

    incomingData = "";
  }


  

   start_time=millis();
   end_time=start_time+1000;
 while(millis()<end_time)
 {
   if(digitalRead(sensor))
   {
    steps=steps+1; 
    while(digitalRead(sensor));
   }
   //int stepss=steps;
   lcd.setCursor(11,2);
   lcd.print(steps);
   lcd.print("   ");
 }
    temp=steps-steps_old;
    steps_old=steps;
    rps=(temp/20);
    int stepss=steps/10;
    lcd.setCursor(11,3);
    lcd.print(stepss);
    lcd.print("   ");
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
void receivedMessage(String inputString) {

  //Get The number of the sender
  index = inputString.indexOf('"') + 1;
  inputString = inputString.substring(index);
  
  index = inputString.indexOf('"');
  number = inputString.substring(0, index);
  Serial.println("Number: " + number);
  //Get The Message of the sender
  index = inputString.indexOf("\n") + 1;
  message = inputString.substring(index);
  message.trim();
  Serial.println("Message: " + message);
  message.toUpperCase(); // uppercase the message received
  delay(50);// Added delay between two readings
    if (message.indexOf("START") > -1) {
    
       button();
       digitalWrite(device_1, HIGH);
       digitalWrite(device_2, HIGH);
      Serial.println("DEVICE IS ACTIVATED.");

  }

  //turn OFF system
  if (message.indexOf("STOP") > -1) {
       digitalWrite(device_1, LOW);
       digitalWrite(device_2, LOW);
       Serial.println("Command: Device 1 Turn Off.");
        EEPROM.write(AddressRom,0);
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("Hello Driver!");
        lcd.setCursor(0,1);
        lcd.print("Car has stoped");
       delay(1000);  
  }


}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void button(){
      buttonState = digitalRead(buttonPin);
      //Serial.println(buttonState);
      if (buttonState != lastButtonState) {
      if (buttonState == HIGH) {
      remained=151-buttonPushCounter;
      EEPROM.get(AddressRom,buttonPushCounter); 
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
      digitalWrite(ledPin, HIGH);
      EEPROM.put(AddressRom,  buttonPushCounter);
//..........................................................................................................
//...........................................................................................................
      if(buttonPushCounter<80){
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Hello Driver!");
        lcd.setCursor(2,1);
        lcd.print("Good Condition");
        lcd.setCursor(0,2);
        lcd.print("DRIVEN KM:");
        lcd.setCursor(12,2);
        lcd.print(buttonPushCounter);
        lcd.setCursor(0,3);
        lcd.print("REMAINED KM:");
        lcd.setCursor(12,3);
        lcd.print(remained);
        
        }
           if(buttonPushCounter==80){
            
            Reply("Oil are geting old km is over 4800");
            }

           
           if(buttonPushCounter>=84 &&buttonPushCounter<=85){
         lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Hello Driver!");
        lcd.setCursor(2,1);
        lcd.print("Good Condition");
        lcd.setCursor(0,2);
        lcd.print("DRIVEN KM:");
        lcd.setCursor(12,2);
        lcd.print(buttonPushCounter);
        lcd.setCursor(0,3);
        lcd.print("REMAINED KM:");
        lcd.setCursor(12,3);
        lcd.print(remained);
        
        
          //allNumber();
        }
 //.................................................................................
 //...................................................................................       
   else     if(buttonPushCounter==100){
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Hello Driver!");
        lcd.setCursor(2,1);
        lcd.print("Risk Condition");
        lcd.setCursor(0,2);
        lcd.print("DRIVEN KM:");
        lcd.setCursor(12,2);
        lcd.print(buttonPushCounter);
        lcd.setCursor(0,3);
        lcd.print("REMAINED KM:");
        lcd.setCursor(12,3);
        lcd.print(remained);
         Reply("Time is over Car has started tolelant time");
        
        }

  //........................................................................................................
  //.........................................................................................................
 
        else if(buttonPushCounter>=100&&buttonPushCounter<=120)
        
        {
       
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Hello Driver!");
        lcd.setCursor(0,1);
        lcd.print("Oil are getting old ");
        lcd.setCursor(0,2);
        lcd.print("DRIVEN KM:");
        lcd.setCursor(12,2);
        lcd.print(buttonPushCounter);
        lcd.setCursor(0,3);
        lcd.print("REMAINED KM:");
        lcd.setCursor(12,3);
        lcd.print(remained);
       
          }
       

   if(buttonPushCounter>120){
       digitalWrite(ledPin, HIGH);
        digitalWrite(device_1, HIGH);
          digitalWrite(device_2, HIGH);
          
       //
        callNumber();
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("Hello Driver!");
        lcd.setCursor(2,1);
        lcd.print("You are OUT of oil!");
        lcd.setCursor(0,2);
        lcd.print("DRIVEN KM:");
        lcd.setCursor(12,2);
        lcd.print(buttonPushCounter);
        lcd.setCursor(0,3);
        lcd.print("REMAINED KM:");
        lcd.setCursor(12,3);
        lcd.print(remained);
          //digitalWrite(buzzer1, HIGH);
         // delay(500);
          digitalWrite(buzzer1, LOW);
          
        }
//............................................................................................
  if(buttonPushCounter==150){
       EEPROM.write(AddressRom,0); 
       Serial.print("reset");
       digitalWrite(ledPin, LOW);
       Reply("car is stoped");
       callNumber();
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("Hello Driver!");
        lcd.setCursor(2,1);
        lcd.print("Car stoped!");
        lcd.setCursor(0,2);
        lcd.print("DRIVEN KM:");
        lcd.setCursor(12,2);
        lcd.print(buttonPushCounter);
        lcd.setCursor(0,3);
        lcd.print("REMAINED KM:");
        lcd.setCursor(12,3);
        lcd.print(remained);
        delay(10);
        Reply("car is in risk stoped");
        }
 //.........................................................................................

    else {
      digitalWrite(ledPin, LOW);
      Serial.println("off");
     }
   /*  
   if(buttonPushCounter>100){
     if (millis() - tsLastReport2 > REPORTING_PERIOD_MS2 ) {
      digitalWrite(buzzer1, HIGH);
   
 
      tsLastReport2 = millis();
    
    }   
         if (millis() - tsLastReport3 > REPORTING_PERIOD_MS3 ) {
      digitalWrite(buzzer1,LOW);
   
 
      tsLastReport3 = millis();
    
    }  
     }*/
      lastButtonState = buttonState;
     }}} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  String _readSerial() {
  _timeout = 0;
  while  (!mySerial.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (mySerial.available()) {
    return mySerial.readString();
  }
}
  
  
  void callNumber() {
  mySerial.print (F("ATD"));
  mySerial.print (mynbr);
  mySerial.print (F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);
}

void Reply(String text)
{
  
    mySerial.print("AT+CMGF=1\r");
    delay(1000);
    mySerial.print("AT+CMGS=\""+number+"\"\r");
    delay(1000);
    mySerial.print(text);
    delay(100);
    mySerial.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
    delay(1000);
    Serial.println("SMS Sent Successfully.");
}

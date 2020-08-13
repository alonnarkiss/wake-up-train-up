//************libraries**************//
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h> // includes the LiquidCrystal Library 

LiquidCrystal lcd(12, 11, 4, 5, 6, 7); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7) 

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


//************const Time**************//
int hourupg;
int minupg;
int yearupg;
int monthupg;
int dayupg;

//************Alarm Time set here**************//
int alarmHours = 7 ;
int alarmMinutes = 0;

bool alarm_stat = true;
//************const sensor**************//
const byte interruptPin = 2;
int POWER_PIN = 9;
int POWER_PIN_2 = 9 ; // not available
const int buzzer = 13; //buzzer to arduino pin 13
const int RELAY_PIN = 3;  // the Arduino pin, which connects to the IN pin of relay
const int relay_light =10;
bool relay_mode = true;
bool relay_stat_light = true;
//************Global const**************//

int counter_hit = 0;

//************const for noamal alarm**************//

int counter_limt = 60; // need to punch more the the limt to cancel the alarm

//************const for extrem alarm**************//
byte randNumber; // make random number for the sensor punch
int timeerr = 30;

/*
 * TODO
 * 1. function for Alarm NOOB (Done)
 * 2. function for Alarm normal : in 25 secend need to mak 30 punch (Done)
 * 3. function for Alarm expert : use random for trigin the punch (need at lest 3 sensore)
 * 
 */
void setup() {
    
    Serial.begin(9600);
    if (! rtc.begin()) {
    lcd.print("Couldn't find RTC");
    while (1);
  }
  
  if (rtc.lostPower()) {
   lcd.print("RTC lost power, lets set the time!");
    // If the RTC have lost power it will sets the RTC to the date & time this sketch was compiled in the following line
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }


  // If you need to set the time of the uncomment line 34 or 37
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
   //rtc.adjust(DateTime(2020, 4, 3, 15, 2, 0));
    pinMode(buzzer, OUTPUT); // Set buzzer
    pinMode(POWER_PIN, INPUT);
    //pinMode(ledPin, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(relay_light, OUTPUT);
    lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display } 
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), change_alarm_time, FALLING  ); // interupt 
}
void loop() {  
//  change_alarm_time();
     delay(1000); 
  
  //response();
  DisplayDateTime();
  if(alarm_stat){ // bool for alarm on off
    if( hourupg == alarmHours && minupg == alarmMinutes){ // check if the time is the time to get the fuck up
      digitalWrite(relay_light, HIGH); //open litght
      //Alarm_noob();
      Alarm_normal();
      digitalWrite(relay_light, LOW); // close light
    }
  }
  
}
void DisplayDateTime ()
{
// We show the current date and time
  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  lcd.print("Hour : ");
  
  if (now.hour()<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.hour(), DEC);
  hourupg=now.hour();
  lcd.print(":");
  if (now.minute()<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC);
  minupg=now.minute();

  if(alarm_stat)
    lcd.print(" *");
  
  lcd.setCursor(0, 1);
  //lcd.print("Date : ");
  if (now.day()<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.day(), DEC);
  dayupg=now.day();
  lcd.print("/");
  if (now.month()<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.month(), DEC);
  monthupg=now.month();
  lcd.print("/");
  lcd.print(now.year(), DEC);
  yearupg=now.year();
  delay(2500);
  //lcd.setCursor(0, 1);  
  lcd.print(" ");
  int temp = rtc.getTemperature();

  
  if(relay_mode == true && hourupg < 6 && temp > 27 ){ // open the ac only on the hot nigth
    digitalWrite(RELAY_PIN, HIGH); 
    Serial.println("on");
    Serial.println(dayupg);
    Serial.println(relay_mode);
    relay_mode = false;
  }
  else if (relay_mode == false && (hourupg > 6|| temp < 26 )){ // close ac on the morning
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("off");
    Serial.println(dayupg);
    Serial.println(relay_mode);
    relay_mode = true;
    
  }
  
  lcd.print(temp); 
  lcd.print("c"); 
  delay(1500);
}

void Alarm_noob(){
  tone(buzzer, 5000); // Send 5KHz sound signal...
  lcd.clear();
      lcd.print("Alarm!!!! onn");
      lcd.print("punch to stop");
    while(digitalRead(POWER_PIN) == LOW ){ //check if yo punch the button if not buzz
      delay(50);
      
    }
    lcd.clear(); // if punch
    lcd.print("niceee");
    noTone(buzzer);     // Stop sound...
    delay(2500);
    DisplayDateTime();
    while(hourupg == alarmHours && minupg == alarmMinutes){ //if the time didnt change we dont want to return to the Alarm
      DisplayDateTime();
      delay(5000);
    }
  
}

void Alarm_normal(){
    tone(buzzer, 5000); // Send 5KHz sound signal...
    int last_time = 30;
    counter_hit = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("waiting to start");
    while(digitalRead(POWER_PIN) == LOW ){
      delay(100);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm! be rady");
    for(int i = 9 ; i >0 ; i --){
      lcd.setCursor(0, 1);
      lcd.print("in : ");
      lcd.print(i);
      delay(1000);
    }
    
    lcd.clear();
    noTone(buzzer);     // Stop sound...
    float currentMillis = millis(); 
    while((millis()-currentMillis)/1000 <30){// <<<<<< 30 need to change to const time limt for punch
      int timeer = 30-(millis()-currentMillis)/1000 ;
      if(last_time != timeer){ // check if one sec passed
        
        //lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Time : ");
        lcd.print(timeer,DEC);
        last_time --;
      }
         
      
    if(digitalRead(POWER_PIN) == HIGH ){ //check if yo punch the button if not buzz
      counter_hit++;
      printcounter();
      delay(200);
      
      }
     
    }
    noTone(buzzer);     // Stop sound...
    lcd.clear(); 
    if(counter_hit < counter_limt ){
      lcd.setCursor(0, 0);
      lcd.print("not good enough!");
      lcd.setCursor(0, 1);
      lcd.print("try again!");
      delay(2000);
       Alarm_normal();
    }
    lcd.print("noiceee");
    
    delay(2500);
    lcd.clear();
    DisplayDateTime();
    while(hourupg == alarmHours && minupg == alarmMinutes){ //if the time didnt change we dont want to return to the Alarm
      DisplayDateTime();
      delay(5000);
    }
  
}


void Alarm_expert(){
    tone(buzzer, 5000); // Send 5KHz sound signal...
    int last_time = 30;              //<< neeed to be with const up
    int time_between_punch = 30;     //<< neeed to be with const up
    counter_hit = 0;
    lcd.clear();
    
    for(int i = 9 ; i >0 ; i --){
      lcd.setCursor(0, 0);
      lcd.print("Alarm!! be ready!");
      lcd.setCursor(0, 1);
      lcd.print("in : ");
      lcd.print(i);
      delay(1000);
    }
    noTone(buzzer);     // Stop sound...
    lcd.clear();
    float currentMillis = millis(); 
    while((millis()-currentMillis)/1000 <timeerr){// time limt for punch
      int prev_time = 30-(millis()-currentMillis)/1000 ;
      if(last_time != prev_time){ // check if one sec passed
        lcd.setCursor(0, 0);
        lcd.print("Time : ");
        if(prev_time <10){
          lcd.print("0");
        }
        lcd.print(prev_time,DEC);
        last_time --;
      }
         
      if(last_time == time_between_punch){ // check if one sec passed
        randNumber = random(1,3); // generate random number between 1 & 3 (minimum is inclusive, maximum is exclusive)
        //lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Time : ");
        lcd.print(prev_time,DEC);
        lcd.print("-> ");
        lcd.print(randNumber,DEC);
        lcd.print("   "); // make space in lcd becuse the "hit"
        time_between_punch = time_between_punch - 5 ;
      }
    if(digitalRead(POWER_PIN) == HIGH){ //check if you punch the button 
      if(randNumber == 1){
        counter_hit++;
      printcounter();
      lcd.setCursor(12, 0);
      lcd.print("hit");
      delay(200);
      }
      else{
      counter_hit--;
      printcounter();
      delay(200);
    }
      
    }
    if(digitalRead(POWER_PIN_2) == HIGH){ //check if you punch the button 
      if(randNumber == 2){
        counter_hit++;
      printcounter();
      lcd.setCursor(12, 0);
      lcd.print("hit");
      delay(200);
      }
      else{
      counter_hit--;
      printcounter();
      delay(200);
    }
      
    }
    }
    if (counter_hit<4){ //if uou pice of shit -> do it again !
      Alarm_expert();
    }
    lcd.clear();
    lcd.setCursor(0, 0);
      lcd.print("GooD job ");
      lcd.setCursor(0, 1);
      lcd.print("You pice of shit");
      delay(3000);
      lcd.clear();
}
void printcounter(){
      lcd.setCursor(0, 1);
      lcd.print("hit: ");
      if(counter_hit <10){
        lcd.print("0");
      }
      if(counter_hit<0){
        lcd.setCursor(5,1);
      }
      lcd.print(counter_hit);
}
void response(){
  Serial.println("kick fast as you can!!");
   digitalWrite(13, HIGH);
   float currentMillis = millis(); 
   while(digitalRead(POWER_PIN) == LOW ){
    continue;
    
  }
  float endMillis = millis();// start time
  digitalWrite(13, LOW);
  float delta = (endMillis - currentMillis)/1000; //giving the response time
  
  Serial.print("your time is :");// the app geting data from serial.print (maybe need to delet these line)
  Serial.println(delta);
  delay(500);
}

void relay_control(int relay_pin,int relay_stat){
  if(relay_stat)
    digitalWrite(relay_pin, LOW);
  else
    digitalWrite(relay_pin, HIGH);
}

void change_alarm_time(){
  lcd.clear();
  lcd.print(alarmHours );
  lcd.print(":" );
  if(alarmMinutes <9)
    lcd.print("0" );
  lcd.print(alarmMinutes );
  Serial.println("interupte");
  alarm_stat = !alarm_stat; // change true or false (alarm work or not)
  lcd.setCursor(0, 1); // new line
  if(alarm_stat)
    lcd.print("alarm on now" );
  else
    lcd.print("alarm off now" );
}

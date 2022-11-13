#include <Wire.h>

#include <LiquidCrystal_I2C.h>  //Including I2C header file
int lcdColumns = 16;
int lcdRows = 2;  
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); //Initializing LCD

char Display;
String Word;
String StaticMessage; 
String ScrollMessage;

int FirstMessage;
int SecondMessage;

//Enable Buzzer port
int buzzer = 9;

//Function to scroll the text
void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}

//
void setup()
{
  pinMode(9,OUTPUT);
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.begin(16,2);// Size of the LCD
  Serial.begin(9600);
}

void loop() {

  if(Serial.available())
  {
    Display = Serial.read();  
    Word = Word + Display;
    if(StaticMessage){
        tone(buzzer, 10);
        }
        delay(25);
        noTone(buzzer);
        delay(25);
  }

if (Display == '*') {

      Serial.println(Word);
      Serial.println();
      FirstMessage = Word.indexOf(',');
      StaticMessage = Word.substring(0, FirstMessage);
      SecondMessage = Word.indexOf(',', FirstMessage+1);
      ScrollMessage = Word.substring(FirstMessage+1, 80);
      

      Serial.print("StaticMessage");
      Serial.println(StaticMessage); 
      Serial.print("ScrollMessage");
      Serial.println(ScrollMessage);
      Word = "";
      lcd.setCursor(0,0);
      lcd.print(StaticMessage);
      
      lcd.setCursor(0,1);
      for(int i = 0; i<10;i++){
        lcd.print(ScrollMessage);
        scrollText(1, ScrollMessage, 250, lcdColumns);
        }        
  }      
}

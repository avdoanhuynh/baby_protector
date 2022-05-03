#include <Adafruit_GFX_AS.h>
#include <Adafruit_ILI9341_AS.h>
#include <UTouch.h>
#include <MySignals.h>
#include <MySignals_BLE.h>
#include <MySignals_BLE_MAC.h>
#include <Wire.h>
#include <SPI.h>
#include <MySignals_Extender.h>

// Digital Input Pin (Mikrofon Sensor)
const int mikrofon = 28;  
bool valmikrofon = 0;
bool valmikrofonMem = 0;
int counter;

// Analog Input Pin (POT1)
const int potionmeter1 = POT1;
// Analog Output Pin (Buzzer) 
const int buzzer = BUZZER; 

const int redLed = LED1;
const int yellowLed = LED2;
const int greenLed = LED3;

// gelesenen Wert von POT1
int potvalue = 0; 
// Output Wert für Buzzer
int buzzervalue = 0;

//Wert der Temperatur
float temperature;

Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);
UTouch  myTouch(TOUCH_CLK,TOUCH_CS,TOUCH_DIN,TOUCH_DOUT,TOUCH_IRQ);

//Koordinaten der Touchscreen Eingabe
int touch_x;
int touch_y;

//Zustand: "Start" oder "Stop"
bool mode = 1; 
//wird benutzt, um zu prüfen, ob es der ersten Loop ist, nachdem "Stop" gedrückt wird.
bool firstStop; 




void setup() 
{
    Serial.begin (9600);
    MySignals.begin();
    bitSet(MySignals.expanderState, EXP_TOUCH_CS);  //Enable Touchscreen SPI
    MySignals.expanderWrite(MySignals.expanderState);
  
    Wire.begin();
    myTouch.InitTouch();
    tft.init();
    tft.setRotation(2);

    pinMode(potionmeter1, INPUT);
    pinMode(mikrofon,INPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(redLed, OUTPUT);
    pinMode(greenLed, OUTPUT);
    pinMode(yellowLed, OUTPUT);

    tft.fillScreen(ILI9341_WHITE);

    //Ausgabe des Name der Geräte auf Touchscreen: "Baby Protector"
    tft.fillRect(0,0,320,30,ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.drawString("Baby Protector",5,5,4);

    //"Stop" Button zeichnen
    tft.setTextColor(ILI9341_WHITE);
    tft.fillRect(150,250,90,70,ILI9341_PURPLE);
    tft.drawString("STOP",170,275,4);
    
    //"Start" Button zeichnen
    tft.setTextColor(ILI9341_WHITE);
    tft.fillRect(0,250,90,70,ILI9341_BLUE);
    tft.drawString("START",5,275,4);
}





void loop() 
{
   //Prüfen, ob "Start" oder "Stop" gedrückt wird.  
   if (myTouch.dataAvailable())
   {
      myTouch.read();
      touch_x=myTouch.getX();
      touch_y=myTouch.getY();
      
      //Wenn "Stop" gedrückt wird
      if( touch_x > 0 && touch_x < 70 && touch_y > 150 && touch_y < 239)
      {
        mode = 0;
        firstStop = 1; 
      }
      
      //Wenn "Start" gedrückt wird
      if( touch_x > 0 && touch_x < 70 && touch_y > 0 && touch_y < 90)
      {
        mode = 1;
      }

      //Ausgabe der Koordinaten der Touchscreen-Eingabe auf seriellen Monitor --> Nur zum Kontrollieren
      Serial.print("x=");
      Serial.print(touch_x);
      Serial.print("  y=");
      Serial.println(touch_y);
   }
   SPI.end();
   
   
   //Wenn "Start" gedrückt wird, fängt der Gerät an, zu arbeiten
   if (mode)
   {
      //Digital Signal der Mikrofon ablesen
      valmikrofon = digitalRead(mikrofon);

      //Zustand der Mikrofon für 10 Loop speichern
      if (valmikrofon)
      {
        valmikrofonMem = 1;
        counter = 10;
      }
      if (--counter < 0)
      {
        valmikrofonMem = 0;
      }

      
      //Potionmeter wird benutzt, um die Töne des Buzzers zu steuern
      //Analog Wert der Potionmeter ablesen
      potvalue = analogRead(potionmeter1); 
      //Output Wert für Buzzer skalieren
      buzzervalue = map(potvalue,0,1023,300,1800); 

      //Wert der Temperatur ablesen
      temperature = MySignals.getTemperature();
 
      // Wert der Temperatur auf Display ausgeben.
      tft.fillRect(150,45,100,30,ILI9341_WHITE);
      tft.setTextColor(ILI9341_BLACK);
      tft.drawFloat(temperature,2,150,45,4);

      tft.setTextColor(ILI9341_BLACK);
      tft.drawString("Temp(*C):",5,45,4);
      
      //Ausgabe auf seriellen Monitor: das Signal der Mikrofon und Wert der Temperatur --> Nur zum Kontrollieren
      Serial.println(valmikrofon);
      Serial.println(temperature,2);


      //Wenn es kein Signal von dem Mikrofon gibt und die Temperatur in erlaubte Bereich liegt
      //Dann leuchtet der grünen LED und Ausgabe auf Display: "Everything is OK"
      //Erlaubte Bereich der Temperatur ist hier 18-28 Grad Celcius definiert
      if (valmikrofonMem == 0 && temperature < 28 && temperature > 18)
      {
        digitalWrite(greenLed,HIGH);
        tft.setTextColor(ILI9341_BLACK,ILI9341_GREEN);
        tft.drawString("  Everything is OK!             ",0,95,4);
    
      }
      else
      {
        digitalWrite(greenLed,LOW);
        tft.fillRect(0,90,320,31,ILI9341_WHITE);
      }

      
      //Wenn es Signal von dem Mikrofon gibt
      //Dann leuchtet der gelben LED und Ausgabe auf Display: "Baby is crying"
      if (valmikrofonMem == 1)
      {
        tone(buzzer,buzzervalue,10);
        digitalWrite(yellowLed,HIGH);
        tft.setTextColor(ILI9341_BLACK,ILI9341_YELLOW);
        tft.drawString("  Baby is crying                ",0,185,4);
      }
      else
      {
        digitalWrite(yellowLed,LOW);
        tft.fillRect(0,180,320,31,ILI9341_WHITE);
      }



      //Wenn die Temperatur zu hoch ist
      //Dann leuchtet der roten LED und Ausgabe auf Display: "Temp too high"
      //Wenn die Temperatur zu niedrig ist
      //Dann leuchtet der roten LED und Ausgabe auf Display: "Temp too low "
      if (temperature > 28 || temperature < 18)
      {    
         if (temperature > 28)
         {
            tone(buzzer,buzzervalue,10);
            digitalWrite(redLed,HIGH);
            tft.setTextColor(ILI9341_BLACK,ILI9341_RED);
            tft.drawString("  Temp too high              ",0,140,4);
         }
         
         if (temperature < 18)
         {
            tone(buzzer,buzzervalue,10);
            digitalWrite(redLed,HIGH);
            tft.setTextColor(ILI9341_BLACK,ILI9341_RED);
            tft.drawString("  Temp too low               ",0,140,4);
         }
      }
      else
         {
            digitalWrite(redLed,LOW);
            tft.fillRect(0,135,320,31,ILI9341_WHITE);
         }
  }


  //Wenn "Stop" gedrückt wird, hört der Gerät auf, zu arbeiten
  //Hinweise auf Display werden gelöscht
  //Ausgabe auf Display: "Please press Start" 
  else
  {
      tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
      if(firstStop)
      {
          tft.fillRect(0,45,320,170,ILI9341_WHITE);
          tft.drawString("Please press Start!      ",5,140,4);
          digitalWrite(yellowLed,LOW);
          digitalWrite(redLed,LOW);
          digitalWrite(greenLed,LOW);
          firstStop = 0;
      }
      else
      {
        tft.drawString("Please press Start!      ",5,140,4);
      } 
  }
  SPI.end();
  
}

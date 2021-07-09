/*******************************************************

   Description:
   Reads an analog input on pin 1, prints the result to the LCD.
   This program takes the temperture sensor LM35 for example.

   Connection:
   Plug the LCD Keypad to the UNO(or other controllers)
   Temperture sensor:
   S(blue) -- A1()
     Note: A0 has been occupied.
   VCC(red) -- VCC
   GND(black) -- GND

********************************************************/

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);        // select the pins used on the LCD panel

// UI variables
unsigned long tepTimer;
int val;                               // variable to store the value coming from the analog pin
byte button;                           // 5 - nothing, 4 - select, 3 - left, 2 - down, 1 - up, 0 - right

// Output variables
const byte pin = 2;

// Train variables
const unsigned int pulsewidth = 150; // in millisecs
const unsigned int pulsecycle = 300; // in millisecs
const byte pulsenum = 5; // in millisecs
unsigned long traincycle = 60000; // in millisecs
const byte trainnum = 15; // in millisecs

byte pulseremain;
byte trainremain;
bool pulseon = false;
bool trainon = false;

// Pulse time variables
unsigned long t0, tnow, t0train, tnowtrain;

// Button buffer
unsigned long startbuttonhold = 2000; // in millisecs
unsigned long startbuttondowntime, startbuttoncurrenttime;
bool startbuttondown = false;

void setup(){
  // LCD
  lcd.begin(16, 2);                       // start the library

  // Pin
  pinMode(pin, OUTPUT);

  // Serial
  if (Serial.available() > 0){
    Serial.begin(9600);
    Serial.print("Pulse width (ms): ");
    Serial.println(pulsewidth);
    Serial.print("Pulse cycle (ms): ");
    Serial.println(pulsecycle);
    Serial.print("Pulses per train: ");
    Serial.println(pulsenum);
    Serial.print("Train cycle (s): ");
    Serial.println(traincycle/1000);
    Serial.print("Number of trains: ");
    Serial.println(trainnum);
  }
  // print train info
  /*
  lcd.setCursor(0, 0);                   // set the LCD cursor   position
  lcd.print(pulsewidth);
  lcd.print("-");
  lcd.print(pulsecycle);
  lcd.print("x");       
  lcd.print(pulsenum);
  lcd.print(" ");
  lcd.print(traincycle/1000);
  lcd.print("x");
  lcd.print(trainnum);
  */

  // Time
  t0 = millis();
  t0train = millis();

  // Reset
  pulseremain = pulsenum;
  trainremain = trainnum;
}

void loop(){
  // Time
  tnow = millis() - t0;
  tnowtrain = millis() - t0train;
  
  val=analogRead(0);        // read the analog in value:
  button = val / 200;       // temperature conversion formula

  if (button == 4){
    startbuttoncurrenttime = millis();
    // First time start button down
    if (startbuttondown == false){
      startbuttondowntime = startbuttoncurrenttime;
      startbuttondown = true;
    }
    else if ((startbuttoncurrenttime - startbuttondowntime > startbuttonhold) && (!trainon == false)){
      trainon = true;
      t0train = millis();
    }
  }
  else{
    startbuttondown = false;
  }
  
  // LED off
  if ((tnow > pulsewidth) && (pulseon == true)){
    pulseon = false;
    digitalWrite(13, LOW);
    digitalWrite(pin, LOW);
  }

  // LED on
  if ((tnow >= pulsecycle) && (pulseon == false) && (pulseremain > 0) && (trainon == true)){
    pulseremain = pulseremain - 1;
    pulseon = true;
    t0 = millis();
    digitalWrite(13, HIGH);
    digitalWrite(pin, HIGH);
  }

  // Train
  if ((tnowtrain >= traincycle) && (trainremain > 0) && (trainon == true)){
    trainremain = trainremain - 1;
    t0train = millis();
    pulseremain = pulsenum;
  }

  
  if(millis() - tepTimer > 200){         // output a temperature value per 500ms
    tepTimer = millis();

    // print the results to the lcd
    lcd.setCursor(0, 1);                   // set the LCD cursor   position
    if (trainon == true){
      lcd.print("Train ON ");
    }
    else{
      lcd.print("Train OFF");
    }
    lcd.print(" A0: ");
    lcd.print(button);  

    // Serial
    lcd.setCursor(0, 0);                   // set the LCD cursor   position
    lcd.print("P");
    lcd.print(pulseremain);
    lcd.print(" ");
    lcd.print("T");
    lcd.print(trainremain);
    lcd.print(" ");
    lcd.print("R");
    lcd.print(tnowtrain/1000);
    if (startbuttondown == true){
      lcd.print(" B");
    }
    lcd.print("    ");
  }
}

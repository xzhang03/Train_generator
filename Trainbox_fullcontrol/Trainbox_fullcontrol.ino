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
unsigned int val;                             // variable to store the value coming from the analog pin
byte button;                          // 5 - nothing, 4 - select, 3 - left, 2 - down, 1 - up, 0 - right
byte cursorind = 6;                   // 1 - Pulse width, 2 - Pulse cycle, 3 - Pulses per train, 4 - Train cycle, 5 - Number of trains, 6 - Start
bool showinstruction = true; // Show instructions or not

// Output variables
const byte pin = 2;

// Train variables
unsigned int pulsewidth = 150; // in millisecs
unsigned int pulsecycle = 300; // in millisecs
byte pulsenum = 5; // in integers
unsigned long traincycle = 60000; // in millisecs
byte trainnum = 15; // in integers

// Train increments
unsigned int pulsewidthinc = 50; // in millisecs
unsigned int pulsecycleinc = 50; // in millisecs
byte pulsenuminc = 1; // in integers
unsigned long traincycleinc1 = 1000; // in millisecs
unsigned long traincycleinc2 = 10000; // in millisecs
byte trainnuminc = 1; // in integers

// Runningtrain variables
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
bool leftbuttondown = false;
bool rightbuttondown = false;
bool upbuttondown = false;
bool downbuttondown = false;

// Button noise
/*
byte buttonreads = 3; // Number of button reads before output. Remove button jitter.
byte i; // For the same purpos as above
byte buttongoodreads = 0;
unsigned int val0;
*/

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
  // Initialize
  lcd.setCursor(0, 0); 
  lcd.print("Initilializing...");

  // Instructions
  if (showinstruction){
    lcd.setCursor(0, 0);
    lcd.print("Hold select ");
    lcd.print(startbuttonhold/1000);
    lcd.print("s  ");
    lcd.setCursor(0, 1);
    lcd.print("to start anytime");
    delay(1000);
  }
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

  // Input low noise
  /*
  val = 0;
  buttongoodreads = 0;
  for (i = 0; i < buttonreads; i++){
    val0 = analogRead(0);
    if (val0 <= 900){
      val = val + val0;        // read the analog in value:
      buttongoodreads++;
    }
  }
  // button = val / 200 / buttongoodreads;       // 5 - nothing, 4 - select, 3 - left, 2 - down, 1 - up, 0 - right
  */
  
  // Input
  val = analogRead(0);        // read the analog in value:
  button = val / 200;       // 5 - nothing, 4 - select, 3 - left, 2 - down, 1 - up, 0 - right

  // Page down
  if ((button == 2) && !downbuttondown){
    if (cursorind < 6){
      cursorind++;
    }
    else{
      cursorind = 1;
    }
    downbuttondown = true;
  }
  if (button != 2){
    downbuttondown = false;
  }

  // Page up
  if ((button == 1) && !upbuttondown){
    if (cursorind > 1){
      cursorind--;
    }
    else{
      cursorind = 6;
    }
    upbuttondown = true;
  }
  if (button != 1){
    upbuttondown = false;
  }

  // Value up
  if ((button == 0) && !rightbuttondown){
    switch (cursorind){
      case 1:
        // pulse width
        pulsewidth = pulsewidth + pulsewidthinc;
        break;
      case 2:
        // pulse cycle
        pulsecycle = pulsecycle + pulsecycleinc;
        break;
      case 3:
        // pulse num
        pulsenum = pulsenum + pulsenuminc;
        break;
      case 4:
        // train cycle
        if (traincycle >= traincycleinc2){
          traincycle = traincycle + traincycleinc2;
        }
        else{
          traincycle = traincycle + traincycleinc1;
        }
        break;
      case 5:
        // train num
        trainnum = trainnum + trainnuminc;
        break;
    }
    rightbuttondown = true;
  }
  if (button != 0){
    rightbuttondown = false;
  }
  
  // Value down
  if ((button == 3) && !leftbuttondown){
    switch (cursorind){
      case 1:
        // pulse width
        if (pulsewidth > pulsewidthinc){
          pulsewidth = pulsewidth - pulsewidthinc;
        }
        break;
      case 2:
        // pulse cycle
        if (pulsecycle > pulsecycleinc){
          pulsecycle = pulsecycle - pulsecycleinc;
        }
        break;
      case 3:
        // pulse num
        if (pulsenum > pulsenuminc){
          pulsenum = pulsenum - pulsenuminc;
        }
        break;
      case 4:
        // train cycle
        if (traincycle > traincycleinc2){
          traincycle = traincycle - traincycleinc2;
        }
        else if(traincycle > traincycleinc1){
          traincycle = traincycle - traincycleinc1;
        }
        break;
      case 5:
        // train num
        if (trainnum > trainnuminc){
          trainnum = trainnum - trainnuminc;  
        }
        break;
    }
    leftbuttondown = true;
  }
  if (button != 3){
    leftbuttondown = false;
  }
  
  // lcd, set up stage
  if((millis() - tepTimer > 200) && !trainon){         // output a temperature value per 500ms
    tepTimer = millis();

    // print the results to the lcd
    lcd.setCursor(0, 0);                   // set the LCD cursor   position

    switch (cursorind){
      case 1:
        lcd.print("Pulse width:    ");
        lcd.setCursor(0, 1);
        lcd.print(pulsewidth);
        lcd.print(" ms           ");
        break;
      case 2:
        lcd.print("Pulse cycle:    ");
        lcd.setCursor(0, 1);
        lcd.print(pulsecycle);
        lcd.print(" ms           ");
        break;
      case 3:
        lcd.print("Pulses per train:");
        lcd.setCursor(0, 1);
        lcd.print(pulsenum);
        lcd.print("               ");
        break;
      case 4:
        lcd.print("Train cycle:    ");
        lcd.setCursor(0, 1);
        lcd.print(traincycle/1000);
        lcd.print(" s           ");
        break;
      case 5:
        lcd.print("Num of trains:   ");
        lcd.setCursor(0, 1);
        lcd.print(trainnum);
        lcd.print("              ");
        break;
      case 6:
        lcd.print("P ");
        lcd.print(pulsenum);
        lcd.print(" X ");
        lcd.print(pulsewidth);
        lcd.print(" / ");
        lcd.print(pulsecycle);
        lcd.print("     ");
        lcd.setCursor(0, 1);
        lcd.print("T ");
        lcd.print(trainnum);
        lcd.print(" X ");
        lcd.print(traincycle/1000);
        lcd.print("         ");
        break;
    }

    // Pushing
    if (startbuttondown){
      lcd.setCursor(12, 1);
      lcd.print("HOLD");
    }
    else{
      lcd.setCursor(12, 1);
      lcd.print("    ");
    }
    
  }
  
  if (button == 4){
    startbuttoncurrenttime = millis();
    // First time start button down
    if (!startbuttondown){
      startbuttondowntime = startbuttoncurrenttime;
      startbuttondown = true;
    }
    else if ((startbuttoncurrenttime - startbuttondowntime > startbuttonhold) && !trainon){
      trainon = true;
      t0train = millis();
      pulseremain = pulsenum;
      trainremain = trainnum;
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

  //
  if((millis() - tepTimer > 200) && trainon){         // output a temperature value per 500ms
    tepTimer = millis();

    // print the results to the lcd
    lcd.setCursor(0, 1);                   // set the LCD cursor   position
    if ((trainremain > 0) || (pulseremain > 0)){
      lcd.print("Train ON        ");
    }
    else{
      lcd.print("Train DONE       ");
    }


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
    lcd.print("        ");
  }
  //
}

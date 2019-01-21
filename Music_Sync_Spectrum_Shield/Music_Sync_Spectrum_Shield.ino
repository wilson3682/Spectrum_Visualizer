#include <FastLED.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

// Arduino Music Visualizer

// This music visualizer works off of analog input from a 3.5mm headphone jack
// Just touch jumper wire from A0 to tip of 3.5mm headphone jack

// The code is dynamic and can handle variable amounts of LEDs
// as long as you adjust NUM_LEDS according to the amount of LEDs you are using

// This code uses the Sparkfun Spectrum Shield

//And a 128x64 OLED Display

//This project is based on his stuff https://www.youtube.com/channel/UCQdi3MDHHMm7u3cMkEY329Q

// LED lightning setup
#define LED_PIN     13                   //LED PIN
#define NUM_LEDS    144 // 250           //Numbre of LEDs in your strip
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// Display + Button Setup + Menu
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display (you may have to edit the address of your display)
int upButton = 12;
int downButton = 11;
int selectButton = 3;
int menu = 1; 

#define UPDATES_PER_SECOND 100

// Audio input setup
int strobe = 4;
int reset = 5;
int audio1 = A0;
int audio2 = A1;
int left[7];
int right[7];
int band;
int audio_input = 0;
int freq = 0;

// Standart Visualizer Variables
int midway = NUM_LEDS / 2; // Centre mark from double level visualizer
int loop_max = 2;
int k = 255; // Color wheel position
int decay = 0; // How may ms befor decay
int decay_check = 0;
long pre_react = 0; // New spike conversion
long react = 0; // Numbre of leds beeing lit
long post_react = 0; // Old spike conversion
// Rainbow wave setting
int wheel_speed = 2;

void setup()
{
  // Display + Buttons
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);    //Sets the Font
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  updateMenu();
  //setup2();
}  

  // Frequency select
  void updateMenu() {                   //Switch leds of
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = CRGB(0, 0, 0);
  FastLED.show();
  
  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      u8x8.drawString(0,0,">63       Hz  ");          //I know its not the best way to create a menu but I needed a quick and easy way.
      u8x8.drawString(0,1," 160      Hz");
      u8x8.drawString(0,2," 400      Hz");
      u8x8.drawString(0,3," 1       kHz");
      u8x8.drawString(0,4," 2.5     kHz");
      u8x8.drawString(0,5," 6.25    kHz");
      u8x8.drawString(0,6," 16      kHz");
      u8x8.drawString(0,7,"-----------------");
      break;
    case 2:
      u8x8.drawString(0,0," 63       Hz  ");
      u8x8.drawString(0,1,">160      Hz");
      u8x8.drawString(0,2," 400      Hz");
      u8x8.drawString(0,3," 1       kHz");
      u8x8.drawString(0,4," 2.5     kHz");
      u8x8.drawString(0,5," 6.25    kHz");
      u8x8.drawString(0,6," 16      kHz");
      u8x8.drawString(0,7,"-----------------");
      break;
    case 3:
      u8x8.drawString(0,0," 63       Hz  ");
      u8x8.drawString(0,1," 160      Hz");
      u8x8.drawString(0,2,">400      Hz");
      u8x8.drawString(0,3," 1       kHz");
      u8x8.drawString(0,4," 2.5     kHz");
      u8x8.drawString(0,5," 6.25    kHz");
      u8x8.drawString(0,6," 16      kHz");
      u8x8.drawString(0,7,"-----------------");
      break;
    case 4:
      u8x8.drawString(0,0," 63       Hz  ");
      u8x8.drawString(0,1," 160      Hz");
      u8x8.drawString(0,2," 400      Hz");
      u8x8.drawString(0,3,">1       kHz");
      u8x8.drawString(0,4," 2.5     kHz");
      u8x8.drawString(0,5," 6.25    kHz");
      u8x8.drawString(0,6," 16      kHz");
      u8x8.drawString(0,7,"-----------------");
      break;
      case 5:
      u8x8.drawString(0,0," 63       Hz  ");
      u8x8.drawString(0,1," 160      Hz");
      u8x8.drawString(0,2," 400      Hz");
      u8x8.drawString(0,3," 1       kHz");
      u8x8.drawString(0,4,">2.5     kHz");
      u8x8.drawString(0,5," 6.25    kHz");
      u8x8.drawString(0,6," 16      kHz");
      u8x8.drawString(0,7,"-----------------");
      break;
      case 6:
      u8x8.drawString(0,0," 63       Hz  ");
      u8x8.drawString(0,1," 160      Hz");
      u8x8.drawString(0,2," 400      Hz");
      u8x8.drawString(0,3," 1       kHz");
      u8x8.drawString(0,4," 2.5     kHz");
      u8x8.drawString(0,5,">6.25    kHz");
      u8x8.drawString(0,6," 16      kHz");
      u8x8.drawString(0,7,"-----------------");
      break;
    case 7:
      u8x8.drawString(0,0," 63       Hz  ");
      u8x8.drawString(0,1," 160      Hz");
      u8x8.drawString(0,2," 400      Hz");
      u8x8.drawString(0,3," 1       kHz");
      u8x8.drawString(0,4," 2.5     kHz");
      u8x8.drawString(0,5," 6.25    kHz");
      u8x8.drawString(0,6,">16      kHz");
      u8x8.drawString(0,7,"-----------------");
      break;
    case 8:
      menu = 7;
      break;
    }
   }

      // Menu selcect actions
      void executeAction() {
  switch (menu) {
    case 1:
      action1();
      break;
    case 2:
      action2();
      break;
    case 3:
      action3();
      break;
    case 4:
      action4();
      break;
    case 5:
      action5();
      break;
    case 6:
      action6();
      break;
    case 7:
      action7();
      break;
  }
}
// Real action after select
void action1() {
   u8x8.clear();
   u8x8.drawString(0,0,"-Selected:");
   u8x8.drawString(0,1," 63 Hz");
   freq = 0;
  delay(1000);
}
void action2() {
   u8x8.clear();
   u8x8.drawString(0,0,"-Selected:");
   u8x8.drawString(0,1," 160 Hz");
    freq = 1;
  delay(1000);
}
void action3() {
   u8x8.clear();
   u8x8.drawString(0,0,"-Selected:");
   u8x8.drawString(0,1," 400 Hz");
    freq = 2;
  delay(1000);
}
void action4() {
   u8x8.clear();
   u8x8.drawString(0,0,"-Selected:");
   u8x8.drawString(0,1," 1 kHz");
    freq = 3;
  delay(1000);
}
void action5() {
   u8x8.clear();
   u8x8.drawString(0,0,"-Selected:");
   u8x8.drawString(0,1," 2.5 kHz");
    freq = 4;
  delay(1000);
}
void action6() {
   u8x8.clear();
   u8x8.drawString(0,0,"-Selected:");
   u8x8.drawString(0,1," 6.25 kHz");
    freq = 5;
  delay(1000);
}
void action7() {
   u8x8.clear();
   u8x8.drawString(0,0,"-Selected:");
   u8x8.drawString(0,1," 16 kHz");
    freq = 6;
  delay(1000);
}



void setup2()
{
  
  // Display
    u8x8.clear();
    u8x8.drawString(0,0,"-Listening...");

  //FREQUENCY DISPLAY
  switch (freq){
     case 0:
     u8x8.drawString(0,2," Freq: 63 Hz");
     u8x8.drawString(0,7,"-Made by haag991");
     break;
     case 1:
     u8x8.drawString(0,2," Freq: 160 Hz");
     u8x8.drawString(0,7,"-Made by haag991");
     break;
     case 2:
     u8x8.drawString(0,2," Freq: 400 Hz");
     u8x8.drawString(0,7,"-Made by haag991");
     break;
     case 3:
     u8x8.drawString(0,2," Freq: 1 kHz");
     u8x8.drawString(0,7,"-Made by haag991");
     break;
     case 4:
     u8x8.drawString(0,2," Freq: 2.5 kHz");
     u8x8.drawString(0,7,"-Made by haag991");
     break;
     case 5:
     u8x8.drawString(0,2," Freq: 6.25 kHz");
     u8x8.drawString(0,7,"-Made by haag991");
     break;
     case 6:
     u8x8.drawString(0,2," Freq: 16 kHz");
     u8x8.drawString(0,7,"-Made by haag991");
     break;    
  }
    
  // Spectrum setup
  pinMode(audio1, INPUT);
  pinMode(audio2, INPUT);
  pinMode(strobe, OUTPUT);
  pinMode(reset, OUTPUT);
  digitalWrite(reset, LOW);
  digitalWrite(strobe, HIGH);
  
  // Led lightning setup
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  // Clear leds
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = CRGB(0, 0, 0);
  FastLED.show();

  // Serial and input setup
  Serial.begin(115200);
  Serial.println("\nListening...");
}

// Function to generate color based on virtual wheel
CRGB Scroll(int pos) {
  CRGB color (0,0,0);
  if(pos < 85) {
    color.g = 0;
    color.r = ((float)pos / 85.0f) * 255.0f;
    color.b = 255 - color.r;
  } else if(pos < 170) {
    color.g = ((float)(pos - 85) / 85.0f) * 255.0f;
    color.r = 255 - color.g;
    color.b = 0;
  } else if(pos < 256) {
    color.b = ((float)(pos - 170) / 85.0f) * 255.0f;
    color.g = 255 - color.b;
    color.r = 1;
  }
  return color;
}

// Function to get and set color
void singleRainbow()
{
  for(int i = NUM_LEDS - 1; i >= 0; i--) {
    if (i < react)
      leds[i] = Scroll((i * 256 / 50 + k) % 256);
    else
      leds[i] = CRGB(0, 0, 0);      
  }
  FastLED.show(); 
}

// Function to mirrored visualizer
void doubleRainbow()
{
  for(int i = NUM_LEDS - 1; i >= midway; i--) {
    if (i < react + midway) {
      leds[i] = Scroll((i * 256 / 50 + k) % 256);
      leds[(midway - i) + midway] = Scroll((i * 256 / 50 + k) % 256);
    }
    else
      leds[i] = CRGB(0, 0, 0);
      leds[midway - react] = CRGB(0, 0, 0);
  }
  FastLED.show();
}

void readMSGEQ7()
// Function to read 7 band equalizers
{
  digitalWrite(reset, HIGH);
  digitalWrite(reset, LOW);
  for(band=0; band <7; band++)
  {
    digitalWrite(strobe, LOW); // strobe pin on the shield - kicks the IC up to the next band 
    delayMicroseconds(30); // 
    left[band] = analogRead(audio1); // store left band reading
    right[band] = analogRead(audio2); // ... and the right
    digitalWrite(strobe, HIGH); 
  }
}

void convertSingle()
{
  if (left[freq] > right[freq])
    audio_input = left[freq];
  else
    audio_input = right[freq];

  if (audio_input > 80)
  {
    pre_react = ((long)NUM_LEDS * (long)audio_input) / 1023L; // Translate audio level to numbres of leds

    if (pre_react > react) // Only adjust levelof led if level is higher than current level
    react = pre_react;

    Serial.print(audio_input);
    Serial.print(" -> ");
    Serial.println(pre_react);
  }
}

void convertDouble()
{
  if (left[freq] > right[freq])
    audio_input = left[freq];
  else
    audio_input = right[freq];

  if (audio_input > 80)
  {
    pre_react = ((long)midway * (long)audio_input) / 1023L; // Translate audio level to numbre of leds

    if (pre_react > react) // Only adjust levelof led if level is higher than current level
      react = pre_react;

    Serial.print(audio_input);
    Serial.print(" -> ");
    Serial.println(pre_react);
  }
}

// Function to visualize with a single level
void singleLevel()
{
  readMSGEQ7();

  convertSingle();

  singleRainbow(); // Apply color

  k = k - wheel_speed; // Speed of the color
  if (k < 0) // Reset color wheel
    k = 255;

  // REMOVE LEDs
  decay_check++;
  if (decay_check > decay)
  {
    decay_check = 0;
    if (react > 0)
      react--;
  }
}

// Function to visualize with mirrored levels
void doubleLevel()
{
  readMSGEQ7();

  convertDouble();

  doubleRainbow();

  k = k - wheel_speed; // Speed of the color
  if (k < 0) // Reset color wheel
    k = 255;

  // Remove leds
  decay_check++;
  if (decay_check > decay)
  {
    decay_check = 0;
    if (react > 0)
      react--;
  }
}


void loop()
{  
  //singleLevel();      //Single level visualizer    -just commend one out and the other one in. You can only use one at a time!!!!! 
  doubleLevel();        //Mirrored visualizer        -just commend one out and the other one in. You can only use one at a time!!!!! 
  //delay(1);
  
  if (!digitalRead(downButton)){
    menu++;
    updateMenu();
    delay(100);
    while (!digitalRead(downButton));
  }
  if (!digitalRead(upButton)){
    menu--;
    updateMenu();
    delay(100);
    while (!digitalRead(upButton));
  }
  if (!digitalRead(selectButton)){
    executeAction();
    setup2();
    delay(100);
    while (!digitalRead(selectButton));
  }
}

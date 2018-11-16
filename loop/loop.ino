//led requirements
#include <FastLED.h>
#define LED_PIN 5
#define NUM_LEDS 55
int brightness = 200;

CRGB leds[NUM_LEDS];
//CHSV ledsHSV[NUM_LEDS];

//Enums
enum ShowMode
{
  NONE,
  STARTING_UP,
  BLACKING_OUT,
  SHOWING_BATTERY
};
const String showModeStrings[] = {"NONE", "STARTING_UP", "BLACKING_OUT", "SHOWING_BATTERY"};

enum currentButtonState
{
  PRESSED = HIGH,
  NOT_PRESSED = LOW
};

//Button information
int buttonPin = 2;
int currentButtonState;  // the current reading from the input pin
int previousButtonState; // the previous reading from the input pin

//LED information
const int ledPin = 13;


//Used to store currentTime
long currentTime;
//Used to store currentTime, after an interval
long previousTime;
//Stores the time when blackout was activated, so it can correctly fade over time
long blackOutActivatedTime;
//Stores the time that the battery was last increased, so we can fade up from this time upwards
long batteryIncreasedTime;
int interval = 5;

//Mode and previous mode
ShowMode currentMode = NONE;
ShowMode previousMode;

//To keep track at how far the animations have progressed
int startUpSeqLedCounter = 0;
int batteryPercentage = 0;

//Configuration
int chargingCircleSnakeLength = 55;
int blackOutTime = 1000;

void setup()
{
  //Open serial monitor at 9600 to see console
  Serial.begin(9600);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(buttonPin, INPUT);

  //clearing all led's at setup
  clearAllLeds();
}

void readButton()
{
  int reading = digitalRead(buttonPin);
  previousButtonState = currentButtonState;
  currentButtonState = reading;
}

void readTime()
{
  currentTime = millis();
}
void loop()
{
  readTime();
  readButton();

  //If button changes from not pressed to pressed
  if (previousButtonState == NOT_PRESSED && currentButtonState == PRESSED)
  {
    setCurrentMode(STARTING_UP);
    startUpSeqLedCounter = 0;
    clearAllLeds();
    FastLED.setBrightness(brightness);
  }
  //Button changes from pressed to not pressed
  if (previousButtonState == PRESSED && currentButtonState == NOT_PRESSED)
  {
    setCurrentMode(BLACKING_OUT);
  }

  //This switch case gets executed every _ miliseconds, put code here that continuously has to run according to the currentMode
  switch (currentMode)
  {
  case NONE:
    break;
  case BLACKING_OUT:
    blackOut();
    break;
  case STARTING_UP:
    startUpSeq();
    break;
  case SHOWING_BATTERY:
    interval = 1000;
    timerRed();
    break;
  }
}

void startUpSeq()
{
  if (startUpSeqLedCounter == NUM_LEDS - 1 + chargingCircleSnakeLength)
  {
    setCurrentMode(SHOWING_BATTERY);
    startUpSeqLedCounter = 0;
    return;
  }

  if (startUpSeqLedCounter < NUM_LEDS)
  {
    leds[startUpSeqLedCounter] = CHSV(200, 0, brightness);
  }

  if (startUpSeqLedCounter >= chargingCircleSnakeLength)
  {
    int turnOffLedIndex = startUpSeqLedCounter - chargingCircleSnakeLength;
    leds[turnOffLedIndex] = CHSV(0, 0, 0);
  }

  if (currentTime - previousTime > 30)
  {
    previousTime = currentTime;
    startUpSeqLedCounter++;
  }

  FastLED.show();
}

void blackOut()
{
  if (currentTime - blackOutActivatedTime < blackOutTime)
  {
    float timeDifference = currentTime - blackOutActivatedTime;
    FastLED.setBrightness((1 - (timeDifference / blackOutTime)) * brightness);
  }

  if (currentTime - blackOutActivatedTime > blackOutTime)
  {
    setCurrentMode(NONE);
  }

  FastLED.show();
}

void timerRed()
{

  /*if (currentTime - previousTime > interval) {
    timerDone = true;
    previousTime = currentTime;
    if (++chargingCircleLedIndex == NUM_LEDS - 1) {
      startUp = false;
    }
  }*/
  if (random(0, 1000) > 990)
  {
    batteryPercentage++;
    batteryIncreasedTime = currentTime;
  }

  int chargingCircleLedIndex = (float)batteryPercentage / 100 * NUM_LEDS;

  float timeDifference = (currentTime - batteryIncreasedTime);
  float adjustedBrightness = timeDifference / interval * brightness;
  for (int i = 0; i < chargingCircleLedIndex; i++)
  {
    leds[i] = CHSV(0 - map(i, 0, NUM_LEDS, 0, 180), 255, brightness);
  }
  leds[chargingCircleLedIndex] = CHSV(0 - map(chargingCircleLedIndex, 0, NUM_LEDS, 0, 180), 255, adjustedBrightness);
  FastLED.show();
}

void clearAllLeds()
{
  for (int t = 0; t < NUM_LEDS; t++)
  {
    leds[t] = CHSV(0, 0, 0);
  }
  FastLED.show();
}

void setCurrentMode(ShowMode mode)
{
  if (currentMode != mode)
  {
    previousMode = currentMode;
    currentMode = mode;

    //This switch case only gets executed when the mode CHANGES, to do thigs continuously according to a
    //specific mode, do it in the switch case in the 'loop' method
    switch (currentMode)
    {
    case NONE:
      clearAllLeds();
      break;
    case BLACKING_OUT:
      blackOutActivatedTime = currentTime;
      startUpSeqLedCounter = 0;
      break;
    }

    Serial.print("Switched to mode: ");
    Serial.println(showModeStrings[currentMode]);
  }
}

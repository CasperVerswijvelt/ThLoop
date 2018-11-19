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

//Debounce
int buttonState;
int lastButtonState;
int debounceDelay = 50;
long lastDebounceTime;

//LED information
const int ledPin = 13;

//Used to store currentTime
long currentTime;
//Used to store currentTime, after an interval
long previousTime;
//Stores the time when blackout was activated, so it can correctly fade over time
long blackOutActivatedTime;
//Stores the time that the battery was last increased, so we can fade up from this time upwards
long batteryChangedTime;
int interval = 5;

//Mode and previous mode
ShowMode currentMode = NONE;
ShowMode previousMode;

//To keep track at how far the animations have progressed
int startUpSeqLedCounter = 0;
int batteryPercentage = 50;
int timeNeededForChangingBatteryIndicator;
int amountOfLEDsToChange;
int currentBatteryIndicatorIndex = 0;
int targetBatteryIndicatorIndex = 0;

//Configuration
int chargingCircleSnakeLength = 10;
int blackOutTime = 400;
int timePerChargingLEDStartup = 80;
int timePerChargingLEDNormal = 1000;
int timePerChargingLED;                               //This variable will update according to the boolean underneath
bool useStartUpTimePerLEDForChargingIndicator = true; //This wil automatically change to false after the first time

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

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState)
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    buttonState = reading;

    previousButtonState = currentButtonState;
    currentButtonState = buttonState;
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
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
  if (previousButtonState == NOT_PRESSED && currentButtonState == PRESSED && currentMode == NONE)
  {
    setCurrentMode(STARTING_UP);
  }
  //Button changes from pressed to not pressed
  if (previousButtonState == PRESSED && currentButtonState == NOT_PRESSED && currentMode != NONE)
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

  if (currentTime - timeNeededForChangingBatteryIndicator < batteryChangedTime)
  {
    if (targetBatteryIndicatorIndex > currentBatteryIndicatorIndex)
    {
      //Battery percentage increased
      float percentage = (currentTime - batteryChangedTime) % timePerChargingLED / (float)timePerChargingLED;
      float adjustedBrightness = percentage * brightness;
      int currentLedToTurnOn = (currentTime - batteryChangedTime) / timePerChargingLED + currentBatteryIndicatorIndex;

      //In case we have any delays, we turn on all previous LED's here, so none stay black
      for (int i = 0; i < currentLedToTurnOn; i++)
      {
        leds[i] = CHSV(0 - map(i, 0, NUM_LEDS, 0, 180), 255, brightness);
      }

      leds[currentLedToTurnOn] = CHSV(0 - map(currentLedToTurnOn, 0, NUM_LEDS, 0, 180), 255, adjustedBrightness);
    }
    else if (targetBatteryIndicatorIndex < currentBatteryIndicatorIndex)
    {
      //Battery percentage decreased

      //TODO: If battery has decreased we should fade OUT led's until the indicator is correct again
    }
  }
  else
  {
    //If battery is full, show green circle
    if (batteryPercentage == 100)
    {
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CHSV(85, 255, brightness);
      }
    }

    //We know all animations are done here, so  our currentBatteryIndicatorIndex should be the same as targetBatteryIndicatorIndex
    currentBatteryIndicatorIndex = targetBatteryIndicatorIndex;
    retrieveBatteryPercentage();
  }

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
    {
      currentBatteryIndicatorIndex = 0;
      useStartUpTimePerLEDForChargingIndicator = true;
      clearAllLeds();
      break;
    }
    case STARTING_UP:
    {
      startUpSeqLedCounter = 0;
      clearAllLeds();
      FastLED.setBrightness(brightness);
      break;
    }
    case SHOWING_BATTERY:
    {
      retrieveBatteryPercentage();
      break;
    }
    case BLACKING_OUT:
    {
      blackOutActivatedTime = currentTime;
      startUpSeqLedCounter = 0;
      break;
    }
    }

    Serial.print("Switched to mode: ");
    Serial.println(showModeStrings[currentMode]);
  }
}

void retrieveBatteryPercentage()
{
  //Check for battery, here it is random because we don't have information about the battery yet
  if (random(0, 10000) > 9950)
  {
    int ledIndex = (float)batteryPercentage / 100 * NUM_LEDS;

    batteryPercentage = min(100, batteryPercentage + 1); //Instead: here  we should retrieve battery info from phone

    /*Serial.print("Battery percentage increased to ");
        Serial.println(batteryPercentage);*/

    int ledIndexAfter = (float)(batteryPercentage) / 100 * NUM_LEDS;
    if (ledIndex != ledIndexAfter)
    {
      targetBatteryIndicatorIndex = ledIndexAfter;
    }
  }

  if (currentBatteryIndicatorIndex != targetBatteryIndicatorIndex)
  {
    amountOfLEDsToChange = abs(currentBatteryIndicatorIndex - targetBatteryIndicatorIndex);

    if (useStartUpTimePerLEDForChargingIndicator)
    {
      timePerChargingLED = timePerChargingLEDStartup;
      useStartUpTimePerLEDForChargingIndicator = false;
    }
    else
    {
      timePerChargingLED = timePerChargingLEDNormal;
    }

    timeNeededForChangingBatteryIndicator = amountOfLEDsToChange * timePerChargingLED;
    batteryChangedTime = currentTime;

    //DEBUGGING PRINTS
    /*Serial.println("--- Showing battery startup information ---");
    Serial.print("Battery percentage: ");
    Serial.println(batteryPercentage);
    Serial.print("Amount of leds to turn on: ");
    Serial.println(amountOfLEDsToChange+1);
    Serial.print("Time needed to turn these leds on ");
    if (useStartUpTimePerLEDForChargingIndicator)
    {
      Serial.print("(startup time per led was used)");
    }
    Serial.print(":");
    Serial.println(timeNeededForChangingBatteryIndicator);
    Serial.println("-------------------------------------------");*/
  }
}

//led requirements
#include <FastLED.h>
#define LED_PIN 5
#define NUM_LEDS 55
int brightness = 100;

CRGB leds[NUM_LEDS];
//CHSV ledsHSV[NUM_LEDS];

//buttonrequirements
int buttonPin = 2;
int buttonState; // the current reading from the input pin
int lastButtonState = HIGH; // the previous reading from the input pin

//button debounce
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50; // the debounce time; increase if the output flickers

//testing purposes
const int ledPin = 13;
int ledState = HIGH;
int amountOfButtonPresses = 0;

//timer requirements

long currentTime;
long previousTime;
long blackOutActivatedTime;
int interval = 5;
bool timerDone = false;
bool startUp = false;
bool startUpDone = false;
bool ini = false;
int startUpSeqLedCounter = 0;
int chargingCircleLedCounter = 0;
int wait = 1000;
bool waitTimer = false;
bool waitTimerRunning = false;
bool blackOutDone = false;
int chargingCircleSnakeLength = 55;
int blackOutTime = 1000;


void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(buttonPin, INPUT);

  //blackout at setup
  for (int j = 0; j <= NUM_LEDS; j++) {
    leds[j] = CHSV(0, 0, 0);
    FastLED.show();
  }
}

void readButton() {
  int reading = digitalRead(buttonPin);
  currentTime = millis();
  // Checking if the state of the button has changed since last reading
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        amountOfButtonPresses++;
        Serial.print("buttonState= ");
        Serial.println(buttonState);
        Serial.print("lastButtonState= ");
        Serial.println(lastButtonState);
        Serial.print("amountOfButtonPresses= ");
        Serial.println(amountOfButtonPresses);
      }
    }
  }
  lastButtonState = reading;

}
void resetAll() {
  timerDone = false;
  startUp = false;
  startUpDone = false;
  blackOutDone = true;
  ini = false;

  waitTimer = false;
  waitTimerRunning = false;
  interval = 5;

  startUpSeqLedCounter = 0;
  chargingCircleLedCounter = 0;


  clearAllLeds();
}
void loop() {
  // read the state of the switch into a local variable:
  readButton();


  if ( buttonState == HIGH && !ini && !startUpDone) {
    startUp = true;
    ini = true;
    Serial.println("first press");
    Serial.println(ini);
  }
  if (buttonState == HIGH && lastButtonState == LOW ) {
    blackOutDone = false;
    //Call method that shows cool standby configuration
  }
  if (buttonState == LOW ) {
    blackOut();
  }


  if (startUp) {
    startUpSeq();
  } else if (startUpDone) {
    interval = 1000;
    timerRed();
  }

}

void startUpSeq() {
  if (startUpSeqLedCounter == NUM_LEDS - 1 + chargingCircleSnakeLength && startUp) {
    startUp = false;
    startUpDone = true;
    Serial.println("end of startUpSeq");
    startUpSeqLedCounter = 0;
    return;
  }

  if (startUpSeqLedCounter < NUM_LEDS) {
    leds[startUpSeqLedCounter] = CHSV(200, 0, brightness);
  }


  if (startUpSeqLedCounter >= chargingCircleSnakeLength) {
    int turnOffLedIndex = startUpSeqLedCounter - chargingCircleSnakeLength;
    leds[turnOffLedIndex] = CHSV(0, 0, 0);
  }

  if (currentTime - previousTime > 30) {
    timerDone = true;
    previousTime = currentTime;
    startUpSeqLedCounter++;
  }

  FastLED.show();

}

void blackOut() {
  //If blackoutActivated time is not yet set and a blackout is not done yet, we should fill the variable
  if (blackOutActivatedTime == 0 && !blackOutDone) {
    blackOutActivatedTime = currentTime;
    Serial.println("black out activated time set");
    Serial.println(blackOutActivatedTime);
  }
  
  if (currentTime - blackOutActivatedTime < blackOutTime && !blackOutDone) {
    float timeDifference = currentTime - blackOutActivatedTime;
    FastLED.setBrightness((1 - (timeDifference / blackOutTime)) * 200);
  }
  
  if (currentTime - blackOutActivatedTime > blackOutTime && !blackOutDone) {
    blackOutActivatedTime = 0;
    blackOutDone = true;
    resetAll();
    FastLED.setBrightness(200);
  }


  FastLED.show();


}

void timerRed() {

  if (currentTime - previousTime > interval) {
    timerDone = true;
    previousTime = currentTime;
    if (++chargingCircleLedCounter == NUM_LEDS - 1) {
      startUp = false;
    }
  }
  float timeDifference = (currentTime - previousTime);
  float adjustedBrightness = timeDifference / interval * brightness;
  leds[chargingCircleLedCounter] = CHSV(0 - map(chargingCircleLedCounter, 0, NUM_LEDS, 0, 180), 255, adjustedBrightness);
  FastLED.show();
}

void clearAllLeds() {
  Serial.print("All leds reset");
  for(int t=0; t < NUM_LEDS; t++) {
    leds[t] = CHSV(0,0,0);
  }
}

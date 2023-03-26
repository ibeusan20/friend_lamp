#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "{template_id}" //change {value} into your own (without any brackets)
#define BLYNK_DEVICE_NAME "{device_name}"  // same as above
#define BLYNK_AUTH_TOKEN "{auth_token)"  // same as above

#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <BlynkSimpleEsp8266.h>

// insert your own wifi domain/password and Blynk project authorization code here
char auth[] = "{auth_token)"
//char ssid[];
//char pass[];

//implementation lol
const int maxLength = 30; // not necessary
const char* KNOWN_SSID[] = {"aaaaaaaaaaa", "bbbbbbbbbb", "cccccccccccc", "dddddddddddd"};
const char* KNOWN_PASSWORD[] = {"passaaaaaaaa", "passbbbbbbbbb", "passccccccccccc", "passdddddddddd"};
const int KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]); // number of known networks

const int neo = D5; // ESP8266 pin connected to Neopixel's data pin
const int numLEDs = 12; // number of LEDs
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLEDs, neo, NEO_GRB + NEO_KHZ800);

// initialize constants
int red = 10;       // low brightness
int green = 10;
int blue = 10;
int onoff = HIGH; // variable for lamp's power button
int rbow = LOW; // variable for side buttons
int animation = 0; // for switching animation types


BLYNK_WRITE(V0) { // V0 = power button control
  onoff = param.asInt();
  if (onoff == LOW) { // want to turn lamp off
    Blynk.virtualWrite(V7, LOW); // turn off all other buttons if applicable
    Blynk.virtualWrite(V8, LOW);
    Blynk.virtualWrite(V9, LOW);
    Blynk.virtualWrite(V10, LOW);
    red = 0; // turn off neopixels (ie. colors go to 0)
    green = 0;
    blue = 0;
    animation = 0; // set animation type to colorWipe
    Blynk.virtualWrite(V4, red); // sync sliders in app with new values
    Blynk.virtualWrite(V5, green);
    Blynk.virtualWrite(V6, blue);
  }
  else { // turn lamps on
    Blynk.virtualWrite(V7, LOW); // turn off all buttons if applicable
    Blynk.virtualWrite(V8, LOW);
    Blynk.virtualWrite(V9, LOW);
    Blynk.virtualWrite(V10, LOW);
    animation = 0; // start with colorWipe
    red = 255;// turn on neopixels
    green = 255;
    blue = 255;
    Blynk.virtualWrite(V4, red); // sync sliders on app with new values
    Blynk.virtualWrite(V5, green);
    Blynk.virtualWrite(V6, blue);
  }
}

BLYNK_WRITE(V4) { // red slider was changed
  if (onoff == HIGH) { // only change if lamp is on
    red = param.asInt(); // send slider value to updated colorWipe animation
  }
  else { // if lamp is not on, change slider back to 0
    red = 0;
    Blynk.virtualWrite(V4, red);
  }
}

BLYNK_WRITE(V5) { // green slider was changed (same logic as red control)
  if (onoff == HIGH) {
    green = param.asInt();
  }
  else {
    green = 0;
    Blynk.virtualWrite(V5, green);
  }
}

BLYNK_WRITE(V6) { // blue slider was changed (same logic as red control)
  if (onoff == HIGH) {
    blue = param.asInt();
  }
  else {
    blue = 0;
    Blynk.virtualWrite(V6, blue);
  }
}

BLYNK_WRITE(V7) { // happy button = rainbow animation
  rbow = param.asInt();
  if (onoff == HIGH) { // only change animation if lamp is on
    if (rbow == HIGH) { // if happy button was turned on
      animation = 1; // animation type is rainbow (see void loop())
      Blynk.virtualWrite(V8, LOW); // turn off all other buttons if they're on
      Blynk.virtualWrite(V9, LOW);
      Blynk.virtualWrite(V10, LOW);
    }
    else { // if happy button was turned off, switch animation to colorWipe
      animation = 0;
    }
  }
  else if (onoff == LOW) { // if lamp is off, turn the happy button off
    Blynk.virtualWrite(V8, LOW);
  }
}

BLYNK_WRITE(V8) // angry button = red and yellow (same logic as happy button)
{
  rbow = param.asInt();
  if (onoff == HIGH) {
    if (rbow == HIGH) {
      animation = 3;
      Blynk.virtualWrite(V7, LOW);
      Blynk.virtualWrite(V9, LOW);
      Blynk.virtualWrite(V10, LOW);
    }
    else {
      animation = 0;
    }
  }
  else if (onoff == LOW) {
    Blynk.virtualWrite(V8, LOW);
  }
}

BLYNK_WRITE(V9) // sad button = blue and teal (same logic as happy button)
{
  rbow = param.asInt();
  if (onoff == HIGH) {
    if (rbow == HIGH) {
      animation = 4;
      Blynk.virtualWrite(V7, LOW);
      Blynk.virtualWrite(V8, LOW);
      Blynk.virtualWrite(V10, LOW);
    }
    else {
      animation = 0;
    }
  }
  else if (onoff == LOW) {
    Blynk.virtualWrite(V9, LOW);
  }
}

BLYNK_WRITE(V10) // meh button = rainbow cycle (same logic as happy button)
{
  rbow = param.asInt();
  if (onoff == HIGH) {
    if (rbow == HIGH) {
      animation = 2;
      Blynk.virtualWrite(V7, LOW);
      Blynk.virtualWrite(V8, LOW);
      Blynk.virtualWrite(V9, LOW);
    }
    else {
      animation = 0;
    }
  }
  else if (onoff == LOW) {
    Blynk.virtualWrite(V10, LOW);
  }
}

// next few functions are borrowed from the Adafruit Neopixel example code
// colorWipe: Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
/////////////////////////////////////////

// rainbow: flash the neopixels in rainbow colors
void rainbow(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// rainbowCycle: makes a rotation rainbow wheel in the LEDs
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256 * 3; j++) { // 3 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// wheel: subfunction for rainbowCycle
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void setup() {
  
  // start the lamp with white blinking animations
  int red = 255; // RGB 255,255,255 makes white whereas 0,0,0 makes black (i.e. off)
  int green = 255;
  int blue = 255;

  boolean wifiFound = false;
  int i, n;

  Serial.begin(115200); // initialize serial monitor in case of debugging

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");

  // WiFi.scanNetworks will return the number of networks found
  Serial.println(F("Scan start"));
  int nbVisibleNetworks = WiFi.scanNetworks();
  Serial.println(F("Scan Completed"));
  if (nbVisibleNetworks == 0) {
    Serial.println(F("No networks found. Reset to try again"));
    while (true); // no need to go further, hang in there, will auto launch the Soft WDT reset
  }

  // if you arrive here at least some networks are visible
  Serial.print(nbVisibleNetworks);
  Serial.println(" network(s) found");

  // check if we recognize one by comparing the visible networks
  // one by one with our list of known networks

  for (i = 0; i < nbVisibleNetworks; ++i) {
    Serial.println(WiFi.SSID(i)); // Print current SSID
    for (n = 0; n < KNOWN_SSID_COUNT; n++) { // walk through the list of known SSID and check for a match
      if (strcmp(KNOWN_SSID[n], WiFi.SSID(i).c_str())) {
        Serial.print(F("\tNot matching "));
        Serial.println(KNOWN_SSID[n]);
      } else { // we got a match
        wifiFound = true;
        break; // n is the network index we found
      }
    } // end for each known wifi SSID
    if (wifiFound) break; // break from the "for each visible network" loop
  }

  if (!wifiFound) {
    Serial.println(F("No Known Network identified. Reset to try again"));
    while (true);

  }

  const char* ssid = (KNOWN_SSID[n]);
  const char* pass = (KNOWN_PASSWORD[n]);
  Serial.println(WiFi.localIP());
  Blynk.begin(auth, ssid, pass);
  Serial.println("Blynk Connected"); //Connected and Authenticated with Blynk Server
  
  strip.begin();
  strip.show();
}


// void loop() defines what happens continuously after void setup() occurs once
void loop() {
  Blynk.run(); // allow for blynk communication

  if (animation == 0) { // colorWipe animation (blink between colors)
    colorWipe(strip.Color(red, green, blue), 30); // flash desired color
    //colorWipe(strip.Color(red/10, green/10, blue/10), 30); // flash dimmer version of desired color (to make blinking effect)
  }
  else if (animation == 1) { // rainbow animation (cycle through rainbow colors)
    rainbow(15);
  }
  else if (animation == 2) { // rainbowCycle animation (rotating rainbow wheel)
    rainbowCycle(5);
  }
  else if (animation == 3) { // angry animation (flash between red and yellow)
    colorWipe(strip.Color(255, 0, 0), 30);
    colorWipe(strip.Color(90, 70, 0), 30);
  }
  else if (animation == 4) { // sad animation (flash between blue and teal)
    colorWipe(strip.Color(0, 0, 255), 30);
    colorWipe(strip.Color(0, 90, 90), 30);
  }
}

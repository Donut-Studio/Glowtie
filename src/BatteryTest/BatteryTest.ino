#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#include "webserver.h"

#define PIN            12
#define NUMPIXELS      13
#define EFFECTREFRESHTIME 25

const char *ssid = "Glowtie battery test";

ADC_MODE(ADC_VCC);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);

unsigned long lastEffectUpdate = 0;
long firstPixelHue = 0;

void setup() 
{
  pixels.begin();
  pixels.setBrightness(20);

  if (WiFi.softAP(ssid))
    successAnim();
  else 
    errorAnim();

  server.on("/", handleRoot);
  server.on("/vcc", handleGetVCC);
  server.on("/avg", handleGetAVG);
  server.begin();

  delay(100);
}
void loop() 
{
  server.handleClient();

  if (millis() - lastEffectUpdate >= EFFECTREFRESHTIME)
  {
    if (firstPixelHue >= 5*65536)
      firstPixelHue = 0;
    
    pixels.rainbow(firstPixelHue);
    pixels.show();

    firstPixelHue += 256;
    lastEffectUpdate = millis();
  }
}


/* --- HTML --- */

void handleGetVCC() { server.send(200, "text/plane", String(ESP.getVcc()) + " mV"); }
void handleGetAVG() { server.send(200, "text/plane", String(getAverageBatteryReading()) + " mV (avg)"); }
void handleRoot() 
{
  server.send(200, "text/html", index_html);
}


/* --- UTILITY --- */

int getAverageBatteryReading()
{
  // get the average supply voltage from 6 readings
  int avg = 0;
  for (byte b = 0; b < 6; b++)
  {
    avg += ESP.getVcc();
    delay(25);
  }
  avg /= 6;

  return avg;
}
void successAnim()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, 0, 255, 0);
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, 0, 0, 0);
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }
}
void errorAnim()
{
  for (int i = 0; i < 10; i++)
  {
    if (i % 2 == 0)
      pixels.fill(pixels.Color(255, 0, 0));
    else
      pixels.clear();
    
    pixels.show();
    delay(EFFECTREFRESHTIME);
  }
}

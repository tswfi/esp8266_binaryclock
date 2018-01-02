#include <Arduino.h>

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <Adafruit_NeoPixel.h>

#define PIN D1

const char *ssid = "Vilkas4G";
const char *password = "verkkokauppa";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ400);

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    timeClient.begin();

    strip.begin();
    strip.show();
}

void loop()
{

    timeClient.update();

    Serial.println(timeClient.getFormattedTime());

    delay(1000);

/*     for (uint16_t i = 1; i < strip.numPixels() - 1; i++)
    {
        strip.setPixelColor(i - 1, strip.Color(0, 32, 0));
        strip.setPixelColor(i, strip.Color(0, 64, 0));
        strip.setPixelColor(i + 1, strip.Color(0, 255, 0));
        strip.show();
        delay(50);
        strip.clear();
    }

    for (uint16_t i = strip.numPixels() - 1; i > 1; i--)
    {
        strip.setPixelColor(i - 1, strip.Color(0, 255, 0));
        strip.setPixelColor(i, strip.Color(0, 64, 0));
        strip.setPixelColor(i + 1, strip.Color(0, 32, 0));
        strip.show();
        delay(50);
        strip.clear();
    }
 */
}
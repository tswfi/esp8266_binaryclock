#include <Arduino.h>

#ifndef UNIT_TEST

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <Adafruit_NeoPixel.h>


//
// Connect a 4x6 matrix to D1 pin
// the first 4 leds will be the ones of seconds and the second four the
// tens of seconds and so on
//
//                 L5 - L4
//                 |    |
//                 L6   L3
//            etc  |    |
//            L1   L7   L2
//            |    |    |
//            L9 - L8   L1
//

#define PIN D1
#define PIXELS 24 // 6 by 4 array

const char *ssid = "Vilkas4G";
const char *password = "verkkokauppa";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600*2, 60000);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ400);

uint8_t temp,ones,tens;
volatile bool tick;

void inline timer0_ISR (void) {
    tick = true;
    // reprime the timer
    timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
}

void setup()
{
    // start serial so we can easily debug through it
    Serial.begin(115200);

    // start the strip
    strip.begin();
    strip.clear();

    // connect to wifi
    WiFi.begin(ssid, password);
    temp = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        strip.setPixelColor(temp, strip.Color(32, 32, 32));
        strip.show();
        Serial.print(".");
        temp++;
    }

    Serial.println("Connected");

    // start ntp client
    timeClient.begin();

    // connect timer interrupt
    noInterrupts();
    timer0_isr_init();
    timer0_attachInterrupt(timer0_ISR);
    timer0_write(ESP.getCycleCount() + 80000000L); // 80MHZ / 1sec
    interrupts();

    // prime our first tick
    tick = true;
}

void show(uint8_t offset, int t) {
    // offset 0 for seconds
    // offset 8 for minutes
    // offset 12 for hours
    tens = t / 10;
    ones = t % 10;

    // ones go up
    for (uint8_t i = 0; i < 4; i++) {
        if(bitRead(ones, i)) {
            strip.setPixelColor(offset+i, strip.Color(0, 0, 255));
        }
    }

    // tens go down
    for (uint8_t i = 0; i < 4; i++) {
        if(bitRead(tens, i)) {
            strip.setPixelColor(offset+7-i, strip.Color(0, 0, 255));
        }
    }

}

void loop()
{
    // everytime tick is on update everything
    // update on every tick
    if(tick) {
        timeClient.update();
        Serial.println(timeClient.getFormattedTime());

        strip.clear();

        // update the strip
        temp = timeClient.getSeconds();
        show(0, temp);

        temp = timeClient.getMinutes();
        show(8, temp);

        temp = timeClient.getHours();
        show(16, temp);

        // push to the strip
        strip.show();

        // wait for next tick
        tick = false;
    }
}

#endif
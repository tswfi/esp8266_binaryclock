#include <Arduino.h>

#ifndef UNIT_TEST

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <Adafruit_NeoPixel.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>


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

WiFiManager wifiManager;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600*2, 60000);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ400);

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

    // start and clear the strip
    strip.begin();
    strip.clear();
    strip.show();

    // start wifi manager, it will either make connection
    // or open ap for saving the network config
    wifiManager.autoConnect();

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
    uint8_t tens = t / 10;
    uint8_t ones = t % 10;

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
        // update timeclient
        timeClient.update();

        // debug output to serial
        Serial.println(timeClient.getFormattedTime());

        // clear the strip
        strip.clear();

        // update the strip
        show(0, timeClient.getSeconds());
        show(8, timeClient.getMinutes());
        show(16, timeClient.getHours());

        // push to the strip
        strip.show();

        // wait for next tick
        tick = false;
    }
}

#endif
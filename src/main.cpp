#include <configs/certs/certificate.h>

#include <3_OTA.hpp>

void setup()
{
    int counter = 0;
    Serial.begin(115200);
    delay(100);
    Serial.println("Started...");

    OTA::init();

    if (!OTA::isUpdateAvailable("/version"))
    {
        Serial.println("No update available");
    }
    else
    {
        Serial.println("Updating...");
        OTA::update("/firmware.bin");
    }
}

void loop()
{
    delay(1000);
}
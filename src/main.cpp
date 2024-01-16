#include <configs/certs/certificate.h>

#include "WiFi.h"
#include <bricks/OTA.hpp>
#include <bricks/SIMCOM.hpp>

void setup()
{
    int counter = 0;
    Serial.begin(115200);
    delay(100);
    Serial.println("Started...");

    SIMCOM::client.setCACert(AWS_CERT_CA);
    SIMCOM::init();

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
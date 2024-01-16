// configs
#include <configs/certs/certificate.h>
#include <configs/networking.h>
#include <configs/esp32board.h>

// Inits (as needed)
#include <inits/Version.hpp>

// Other bricks (not recommended to include other bricks, but I'm being lazy)
#include <bricks/SIMCOM.hpp>
#include <bricks/HTTP.hpp>

// Libs
#include <ArduinoHttpClient.h>
#include <Update.h>

namespace OTA
{
    String _availableVersion;

    void confirmConnected() {
        if (HTTP::http.connected()) {
            
        }
    }

    bool isUpdateAvailable(const char *version_path)
    {
        Serial.println("Checking for updates.... ");
        Serial.print(SERVER);
        Serial.println(version_path);
        HTTP::HttpResponse response = HTTP::getFromHTTPServer(version_path);
        if (response.success())
        {

            String newVersion = response.body;
            Serial.print("CurrentVersion : ");
            Serial.print(Version::version());
            Serial.print(", latest version: ");
            Serial.println(newVersion);
            // if (newVersion.compareTo(Version::version()) != 0)
            // {
            //     _availableVersion = newVersion;
            // }
            Serial.println("Force updating!");
            return true;
        }
        else
        {
            Serial.println("Something went wrong while requesting new version number");
            Serial.print("HTTP Code: ");
            Serial.println(response.status_code);
            Serial.print("Error message: ");
            Serial.println(response.error_message);
        }

        return false;
    }

    void update(const char *file_path)
    {
        bool isValidContentType = false;
        int contentLength = 0;

        Serial.print("Connecting to: ");
        Serial.println(SERVER);
        Serial.println("Fetching Bin: " + String(file_path));
        HTTP::HttpRequest request;
        request.addHeader("Cache-Control", "no-cache");
        request.addHeader("Connection", "close");
        HTTP::HttpResponse response = HTTP::getFromHTTPServer(file_path, &request, true);
        
        for (int i_header = 0; i_header < response.header_count; i_header++)
        {
            if (response.headers[i_header].key.compareTo("Content-Length") == 0)
            {
                contentLength = response.headers[i_header].value.toInt();
            }
            if (response.headers[i_header].key.compareTo("Content-Type") == 0)
            {
                String contentType = response.headers[i_header].value;
                isValidContentType =
                    contentType == "application/octet-stream" || contentType == "application/macbinary";
            }
        }

        if (contentLength && isValidContentType)
        {
            Serial.println("Begin OTA update. This may take a while...");
            if (Update.begin(contentLength))
            {
                Update.writeStream(HTTP::http);
                if (Update.end())
                {
                    Serial.println("OTA done!");
                    if (Update.isFinished())
                    {
                        Serial.println("Reboot...");
                        Version::updateVersion(_availableVersion);
                        ESP.restart();
                    }
                }

                Serial.println("------------------------------ERROR------------------------------");
                Serial.printf("    ERROR CODE: %d", Update.getError());
                Serial.println("-----------------------------------------------------------------");
            }
            else
            {
                Serial.println("Not enough space available.");
            }
        }
        else
        {
            Serial.println("There was no content in the response");
        }
    }
}
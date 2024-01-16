#pragma once

// WiFi
static const char *WIFI_SSID = "EE-Hub-cLM4";
static const char *WIFI_PASS = "flag-DID-ahead";

#define SERVER "otas-esp32.s3.eu-west-2.amazonaws.com" // Change to your own *publicly availabe* S3 bucket!
#define PORT 443

// 4G
#define APN_4G "id" // Change to your network provider
#define DUMP_AT_COMMANDS
#define AT_BAUD 115200
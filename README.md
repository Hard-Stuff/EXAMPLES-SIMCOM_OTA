# OTA update example from Amazon S3 bucket

In this example, we're going to update the firmware of our device via a download from an Amazon S3 bucket. We'll do this via SIMCOM's SIM7600 module, and we'll use the in-built SSL functions to create a secure connection to AWS.

I've made the S3 bucket public (with read-only access) so you can try it out yourself!

## Notes before we start

-   This has only been tested on a SIM7600G and an ESP32 S3R2.
-   This is just an example of what has worked for me. By all means leave comments/ideas and if I find time I'll get back to you.
-   Check your UART pins are correct (in /configs/esp32board.h)
-   I'm using namespaces everywhere instead of classes, this is because I only anticipate using one of everything (your application may differ). This is an artificate of `config->lib->brick->task->main` project design, which I haven't utilised here.

## Layers / Operation

-   The base modem layer is what's included in my forked version of `TinyGSM`. Here I can access SIM7600's TinyGSMClientSecure layer, enabling the native SSL connection.
-   `1_SIMCOM.hpp` is a wrapper around the `TinyGSM` functionalities that I use on most projects (namely init, connect, some GPS stuff, etc.)
-   `2_HTTP.hpp` is a wrapper around the `ArduinoHttpClient` library specficially for GET and POST requests. I have similar ones for MQTT and SOCKET, because I'm lazy. This also calls upon the `1_SIMCOM.hpp` functionalities.
-   `3_OTA.hpp` is a wrapper around the ESP32's `Update` library, connected to both the `1_SIMCOM` and `2_HTTP` wrappers.

_Hard Stuff is launching a new generation of impactful hardware startups. Find out more about prototyping and hardware venture building at [hard-stuff.com](hard-stuff.com)_

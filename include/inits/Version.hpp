#pragma once

#include <Arduino.h>
#include <Preferences.h>

namespace Version
{
    namespace
    {
        const char *collection = "device";
    }
    uint64_t device_id = ESP.getEfuseMac();
    String version()
    {
        Preferences pref;
        pref.begin(collection);
        String version = pref.getString("Version", "0");
        pref.end();
        return version;
    }

    void updateVersion(String new_version)
    {
        Preferences pref;
        pref.begin(collection);
        pref.putString("Version", new_version);
        pref.end();
    }
}
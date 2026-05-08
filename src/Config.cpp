#include "Config.h"
#include "FaceRenderer.h"
#include <Preferences.h>
#include "version.h"

Config config;
static Preferences prefs;
static const char *NVS_NS = "rsc";

// ---- Conversion helpers ----
static uint16_t rgb888_to_565(uint32_t c) {
    return ((c >> 8) & 0xF800) | ((c >> 5) & 0x07E0) | ((c >> 3) & 0x001F);
}

static uint32_t rgb565_to_888(uint16_t c) {
    uint8_t r5 = (c >> 11) & 0x1F;
    uint8_t g6 = (c >> 5)  & 0x3F;
    uint8_t b5 = c         & 0x1F;
    uint32_t r = (r5 << 3) | (r5 >> 2);
    uint32_t g = (g6 << 2) | (g6 >> 4);
    uint32_t b = (b5 << 3) | (b5 >> 2);
    return (r << 16) | (g << 8) | b;
}

// ---- NVS load/save ----
static void saveConfig() {
    prefs.begin(NVS_NS, false);
    prefs.putBool("touchDebug", config.touchDebug);
    prefs.putBool("moodCycle",  config.moodAutoCycle);
    prefs.putBool("showDots",   config.showTouchDots);
    prefs.putUShort("eyeCol",   config.eyeColour);
    prefs.putUShort("bgCol",    config.bgColour);
    prefs.putString("mood",     config.mood);
    prefs.end();
}

void initConfig() {
    prefs.begin(NVS_NS, true);
    config.touchDebug    = prefs.getBool("touchDebug", config.touchDebug);
    config.moodAutoCycle = prefs.getBool("moodCycle",  config.moodAutoCycle);
    config.showTouchDots = prefs.getBool("showDots",   config.showTouchDots);
    config.eyeColour     = prefs.getUShort("eyeCol",   config.eyeColour);
    config.bgColour      = prefs.getUShort("bgCol",    config.bgColour);
    String savedMood     = prefs.getString("mood",     String(config.mood));
    strncpy(config.mood, savedMood.c_str(), sizeof(config.mood) - 1);
    config.mood[sizeof(config.mood) - 1] = '\0';
    prefs.end();
}

// ---- Parsing ----
static bool parseBool(const String &v, bool &out) {
    String s = v; s.toLowerCase();
    if (s == "on"  || s == "true"  || s == "1" || s == "yes") { out = true;  return true; }
    if (s == "off" || s == "false" || s == "0" || s == "no")  { out = false; return true; }
    return false;
}

static bool parseColour(const String &v, uint16_t &rgb565) {
    if (v.length() != 6) return false;
    char *end;
    uint32_t rgb888 = strtoul(v.c_str(), &end, 16);
    if (*end != '\0') return false;
    rgb565 = rgb888_to_565(rgb888);
    return true;
}

static const char *KNOWN_MOODS[] = {
    "NEUTRAL", "HAPPY", "ANGRY", "SAD", "EXCITED",
    "ANNOYED", "QUESTIONING", "IDLE1", "IDLE2", "IDLE3"
};

static bool isKnownMood(const String &m) {
    for (auto p : KNOWN_MOODS) if (m == p) return true;
    return false;
}

// ---- Commands ----
static void cmdHelp() {
    Serial.println("--- Commands ---");
    Serial.println("touch_debug:on|off       Raw touch coord prints");
    Serial.println("mood_cycle:on|off        Auto-cycle through moods");
    Serial.println("touch_dots:on|off        Show green dots on tap");
    Serial.println("eye_colour:RRGGBB        6 hex digits, e.g. 00FF00");
    Serial.println("bg_colour:RRGGBB");
    Serial.println("mood:NAME                NEUTRAL|HAPPY|ANGRY|SAD|EXCITED|");
    Serial.println("                         ANNOYED|QUESTIONING|IDLE1|IDLE2|IDLE3");
    Serial.println("status                   Print current config");
    Serial.println("reset                    Clear NVS, reboot to defaults");
    Serial.println("help                     This message");
    Serial.println("version                  Print firmware version");
}

static void cmdStatus() {
    Serial.println("--- Status ---");
    Serial.printf("version:        %s\n", FW_VERSION);
    Serial.printf("touch_debug:    %s\n", config.touchDebug    ? "on" : "off");
    Serial.printf("mood_cycle:     %s\n", config.moodAutoCycle ? "on" : "off");
    Serial.printf("touch_dots:     %s\n", config.showTouchDots ? "on" : "off");
    Serial.printf("eye_colour:     %06X\n", (unsigned)rgb565_to_888(config.eyeColour));
    Serial.printf("bg_colour:      %06X\n", (unsigned)rgb565_to_888(config.bgColour));
    Serial.printf("mood:           %s\n", config.mood);
}

static void cmdReset() {
    prefs.begin(NVS_NS, false);
    prefs.clear();
    prefs.end();
    Serial.println("OK: NVS cleared, rebooting in 1 second...");
    delay(1000);
    ESP.restart();
}

// ---- Dispatch ----
static void parseLine(const String &raw) {
    String line = raw;
    line.trim();
    if (line.length() == 0) return;

    String lower = line; lower.toLowerCase();
    if (lower == "help")   { cmdHelp();   return; }
    if (lower == "status") { cmdStatus(); return; }
    if (lower == "reset")  { cmdReset();  return; }
    if (lower == "version") {
        Serial.printf("version: %s\n", FW_VERSION);
        return;
    }
    int colon = line.indexOf(':');
    if (colon < 0) {
        Serial.printf("ERR: bad syntax '%s' (try 'help')\n", line.c_str());
        return;
    }

    String key = line.substring(0, colon);
    String val = line.substring(colon + 1);
    key.trim(); key.toLowerCase();
    val.trim();

    if (key == "touch_debug") {
        bool b;
        if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
        config.touchDebug = b;
        saveConfig();
        Serial.println("OK");
    }
    else if (key == "mood_cycle") {
        bool b;
        if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
        config.moodAutoCycle = b;
        saveConfig();
        Serial.println("OK");
    }
    else if (key == "touch_dots") {
        bool b;
        if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
        config.showTouchDots = b;
        saveConfig();
        Serial.println("OK");
    }
    else if (key == "eye_colour" || key == "eye_color") {
        uint16_t c;
        if (!parseColour(val, c)) { Serial.println("ERR: bad colour (need 6 hex digits)"); return; }
        setEyeColour(c);
        saveConfig();
        Serial.println("OK");
    }
    else if (key == "bg_colour" || key == "bg_color") {
        uint16_t c;
        if (!parseColour(val, c)) { Serial.println("ERR: bad colour"); return; }
        setBackgroundColour(c);
        saveConfig();
        Serial.println("OK");
    }
    else if (key == "mood") {
        String upper = val; upper.toUpperCase();
        if (!isKnownMood(upper)) { Serial.printf("ERR: unknown mood '%s'\n", val.c_str()); return; }
        setMood(upper.c_str());
        saveConfig();
        Serial.println("OK");
    }
    else {
        Serial.printf("ERR: unknown key '%s'\n", key.c_str());
    }
}

// Non-blocking line reader — accumulates across loop iterations, parses on '\n'
void serviceConfig() {
    static String buffer;
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (buffer.length() > 0) {
                parseLine(buffer);
                buffer = "";
            }
        } else {
            buffer += c;
            if (buffer.length() > 200) {
                Serial.println("ERR: input too long, discarding");
                buffer = "";
            }
        }
    }
}
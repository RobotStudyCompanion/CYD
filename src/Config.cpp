#include "Config.h"
#include "FaceRenderer.h"
#include <Preferences.h>
#include "version.h"
#include "DisplayInit.h"
#include "DebugOverlay.h"
#include "LED_Solution.h"
#include <TFT_eSPI.h>
extern TFT_eSPI tft;

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
    prefs.putUChar("bright", config.brightness);
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
    config.brightness = prefs.getUChar("bright", config.brightness);
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
    Serial.println("Setters (key:value):");
    Serial.println("  touch_debug:on|off");
    Serial.println("  mood_cycle:on|off");
    Serial.println("  touch_dots:on|off");
    Serial.println("  eye_colour:RRGGBB        6 hex digits, e.g. 00FF00");
    Serial.println("  bg_colour:RRGGBB");
    Serial.println("  mood:NAME                NEUTRAL|HAPPY|ANGRY|SAD|EXCITED|");
    Serial.println("                           ANNOYED|QUESTIONING|IDLE1|IDLE2|IDLE3");
    Serial.println("  bright:0-255             Backlight PWM");
    Serial.println("  led:NAME|r,g,b           off|on|red|green|blue|white|");
    Serial.println("                           yellow|cyan|magenta or 1,0,1");
    Serial.println("  tap:x,y[,z]              Inject touch event");
    Serial.println("Getters (key?):");
    Serial.println("  touch_debug? mood_cycle? touch_dots? eye_colour? bg_colour?");
    Serial.println("  mood? bright?");
    Serial.println("Plain:");
    Serial.println("  status     Print current config");
    Serial.println("  version    Print firmware version");
    Serial.println("  mem        Memory snapshot");
    Serial.println("  uptime     Time since boot");
    Serial.println("  ldr        Light sensor reading");
    Serial.println("  clear      Fill screen with bg colour");
    Serial.println("  splash     Re-show RSC splash");
    Serial.println("  reboot     Soft reboot (config preserved)");
    Serial.println("  reset      Clear NVS, reboot to defaults");
    Serial.println("  help       This message");
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
    Serial.printf("bright:         %u\n", config.brightness);
}

static void cmdReset() {
    prefs.begin(NVS_NS, false);
    prefs.clear();
    prefs.end();
    Serial.println("OK: NVS cleared, rebooting in 1 second...");
    delay(1000);
    ESP.restart();
}

static void cmdReboot() {
    Serial.println("OK: rebooting in 1 second...");
    delay(1000);
    ESP.restart();
}

// ---- Dispatch ----
static void parseLine(const String &raw) {
    String line = raw;
    line.trim();
    if (line.length() == 0) return;

    String lower = line; lower.toLowerCase();

    // ---- Plain commands ----
    if (lower == "help")    { cmdHelp();   return; }
    if (lower == "status")  { cmdStatus(); return; }
    if (lower == "reset")   { cmdReset();  return; }
    if (lower == "reboot")  { cmdReboot(); return; }
    if (lower == "version") { Serial.printf("version: %s\n", FW_VERSION); return; }
    if (lower == "mem")     { printMemoryReport(); return; }
    if (lower == "uptime")  { printUptime(); return; }
    if (lower == "ldr")     { printLdr(); return; }
    if (lower == "clear")   { tft.fillScreen(config.bgColour); Serial.println("OK"); return; }
    if (lower == "splash")  { showSplash(); Serial.println("OK"); return; }

    // ---- Getters (key?) ----
    if (lower.endsWith("?")) {
        String key = lower.substring(0, lower.length() - 1);
        key.trim();
        if      (key == "touch_debug") Serial.printf("touch_debug: %s\n", config.touchDebug    ? "on" : "off");
        else if (key == "mood_cycle")  Serial.printf("mood_cycle: %s\n",  config.moodAutoCycle ? "on" : "off");
        else if (key == "touch_dots")  Serial.printf("touch_dots: %s\n",  config.showTouchDots ? "on" : "off");
        else if (key == "eye_colour" || key == "eye_color")
            Serial.printf("eye_colour: %06X\n", (unsigned)rgb565_to_888(config.eyeColour));
        else if (key == "bg_colour" || key == "bg_color")
            Serial.printf("bg_colour: %06X\n", (unsigned)rgb565_to_888(config.bgColour));
        else if (key == "mood")        Serial.printf("mood: %s\n", config.mood);
        else if (key == "bright")      Serial.printf("bright: %u\n", config.brightness);
        else                           Serial.printf("ERR: cannot get '%s'\n", key.c_str());
        return;
    }

    // ---- Setters (key:value) ----
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
        config.touchDebug = b; saveConfig(); Serial.println("OK");
    }
    else if (key == "mood_cycle") {
        bool b;
        if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
        config.moodAutoCycle = b; saveConfig(); Serial.println("OK");
    }
    else if (key == "touch_dots") {
        bool b;
        if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
        config.showTouchDots = b; saveConfig(); Serial.println("OK");
    }
    else if (key == "eye_colour" || key == "eye_color") {
        uint16_t c;
        if (!parseColour(val, c)) { Serial.println("ERR: bad colour (need 6 hex digits)"); return; }
        setEyeColour(c); saveConfig(); Serial.println("OK");
    }
    else if (key == "bg_colour" || key == "bg_color") {
        uint16_t c;
        if (!parseColour(val, c)) { Serial.println("ERR: bad colour"); return; }
        setBackgroundColour(c); saveConfig(); Serial.println("OK");
    }
    else if (key == "mood") {
        String upper = val; upper.toUpperCase();
        if (!isKnownMood(upper)) { Serial.printf("ERR: unknown mood '%s'\n", val.c_str()); return; }
        setMood(upper.c_str()); saveConfig(); Serial.println("OK");
    }
    else if (key == "bright") {
        int v = val.toInt();
        if (v < 0 || v > 255) { Serial.println("ERR: bright 0-255"); return; }
        setBacklight((uint8_t)v); saveConfig(); Serial.println("OK");
    }
    else if (key == "led") {
        String v = val; v.toLowerCase();
        bool r = false, g = false, b = false;
        bool ok = true;
        if      (v == "off")     { /* all false */ }
        else if (v == "on" || v == "white") { r = g = b = true; }
        else if (v == "red")     { r = true; }
        else if (v == "green")   { g = true; }
        else if (v == "blue")    { b = true; }
        else if (v == "yellow")  { r = true; g = true; }
        else if (v == "cyan")    { g = true; b = true; }
        else if (v == "magenta") { r = true; b = true; }
        else {
            int rn, gn, bn;
            if (sscanf(v.c_str(), "%d,%d,%d", &rn, &gn, &bn) != 3) ok = false;
            else { r = (rn != 0); g = (gn != 0); b = (bn != 0); }
        }
        if (!ok) { Serial.println("ERR: bad led (try off|on|red|green|blue|white|yellow|cyan|magenta or r,g,b)"); return; }
        setLedColor(r, g, b);
        Serial.println("OK");
    }
    else if (key == "tap") {
        int x, y, z = 1500;
        int n = sscanf(val.c_str(), "%d,%d,%d", &x, &y, &z);
        if (n < 2) { Serial.println("ERR: bad tap (need x,y or x,y,z)"); return; }
        if (x < 0 || x >= 320 || y < 0 || y >= 240) { Serial.println("ERR: out of range"); return; }
        recordTouch((uint16_t)x, (uint16_t)y, (uint16_t)z);
        Serial.printf("OK: tap (%d, %d) z=%d\n", x, y, z);
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
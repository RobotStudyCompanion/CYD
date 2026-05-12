#include "Config.h"
#include "FaceRenderer.h"
#include "DisplayInit.h"
#include "DebugOverlay.h"
#include "LED_Solution.h"
#include "version.h"
#include <Preferences.h>
#include <TFT_eSPI.h>
#include "LdrSensor.h"
#include "UIManager.h"

extern TFT_eSPI tft;

Config config;
static Preferences prefs;
static const char *NVS_NS = "rsc";
static bool     _nvsDirty = false;
static uint32_t _nvsLastChange = 0;
static const uint32_t NVS_DEBOUNCE_MS = 5000;
static void saveConfig();

void markDirty() {
    _nvsDirty = true;
    _nvsLastChange = millis();
}
void serviceNvs() {
    if (_nvsDirty && (millis() - _nvsLastChange) > NVS_DEBOUNCE_MS) {
        saveConfig();
        _nvsDirty = false;
        // Serial.println("NVS: flushed");
    }
}
static void flushNvs() {   // for forced flush before reboot
    if (_nvsDirty) { saveConfig(); _nvsDirty = false; }
}

// ============ Conversion helpers ============
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

// ============ NVS load/save ============
static void saveConfig() {
    prefs.begin(NVS_NS, false);
    prefs.putBool("touchDebug",     config.touchDebug);
    prefs.putBool("moodCycle",      config.moodAutoCycle);
    prefs.putBool("showDots",       config.showTouchDots);
    prefs.putUShort("eyeCol",       config.eyeColour);
    prefs.putUShort("bgCol",        config.bgColour);
    prefs.putString("mood",         config.mood);
    prefs.putUChar("bright",        config.brightness);
    prefs.putBool ("autoBright",    config.autoBright);
    prefs.putUChar("brLight",       config.brightLight);
    prefs.putUChar("brDark",        config.brightDark);
    prefs.putBool("hudOn",          config.hudOn);
    prefs.putBool("idleAnim",       config.idleAnim);
    prefs.putUShort("menuTo",       config.menuTimeoutSec);
    prefs.putUChar("theme",         (uint8_t)config.theme);
    prefs.end();
}

void initConfig() {
    prefs.begin(NVS_NS, true);
    config.touchDebug    = prefs.getBool("touchDebug", config.touchDebug);
    config.moodAutoCycle = prefs.getBool("moodCycle",  config.moodAutoCycle);
    config.showTouchDots = prefs.getBool("showDots",   config.showTouchDots);
    config.eyeColour     = prefs.getUShort("eyeCol",   config.eyeColour);
    config.bgColour      = prefs.getUShort("bgCol",    config.bgColour);
    config.brightness    = prefs.getUChar("bright",    config.brightness);
    if (config.brightness > 100) {
        config.brightness = (config.brightness * 100) / 255;  // legacy 0-255 -> 0-100
    }
    config.autoBright  = prefs.getBool ("autoBright", config.autoBright);
    config.brightLight = prefs.getUChar("brLight",    config.brightLight);
    config.brightDark  = prefs.getUChar("brDark",     config.brightDark);
    config.idleAnim    = prefs.getBool("idleAnim",       config.idleAnim);
    String savedMood   = prefs.getString("mood",     String(config.mood));
    config.menuTimeoutSec = prefs.getUShort("menuTo", config.menuTimeoutSec);
    strncpy(config.mood, savedMood.c_str(), sizeof(config.mood) - 1);
    config.mood[sizeof(config.mood) - 1] = '\0';
    config.theme = (ThemeMode)prefs.getUChar("theme", (uint8_t)config.theme);
    prefs.end();
}

// ============ Value parsers ============
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

static bool parseFaceShape(const String &v, FaceShape &out) {
    float t, b, ti, pr, r;
    if (sscanf(v.c_str(), "%f,%f,%f,%f,%f", &t, &b, &ti, &pr, &r) != 5) return false;
    out = {t, b, ti, pr, r};
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

// ============ Setter handlers ============
static void cmdSetTouchDebug(const String &val) {
    bool b;
    if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
    config.touchDebug = b; markDirty(); Serial.println("OK");
}
static void cmdSetMoodCycle(const String &val) {
    bool b;
    if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
    config.moodAutoCycle = b; markDirty(); Serial.println("OK");
}
static void cmdSetIdleAnim(const String& val) {
    bool b;
    if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
    config.idleAnim = b; markDirty(); Serial.println("OK");   // markDirty defined in step 3
}
static void cmdGetIdleAnim() {
    Serial.printf("idle_anim:      %s\n", config.idleAnim ? "on" : "off");
}
static void cmdSetTouchDots(const String &val) {
    bool b;
    if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
    config.showTouchDots = b; markDirty(); Serial.println("OK");
}
static void cmdSetEyeColour(const String &val) {
    uint16_t c;
    if (!parseColour(val, c)) { Serial.println("ERR: bad colour (need 6 hex digits)"); return; }
    setEyeColour(c); markDirty(); Serial.println("OK");
}
static void cmdSetBgColour(const String &val) {
    uint16_t c;
    if (!parseColour(val, c)) { Serial.println("ERR: bad colour"); return; }
    setBackgroundColour(c); markDirty(); Serial.println("OK");
}
static void cmdSetTheme(const String& val) {
    String v = val; v.toLowerCase();
    uint16_t bg, eye;
    if      (v == "light") { bg = 0xFFFF; eye = 0x0000; config.theme = THEME_LIGHT; }
    else if (v == "dark")  { bg = 0x0000; eye = 0xFFFF; config.theme = THEME_DARK;  }
    else { Serial.println("ERR: light or dark"); return; }
    config.bgColour  = bg;  setBackgroundColour(bg);
    config.eyeColour = eye; setEyeColour(eye);
    markDirty(); Serial.println("OK");
}
static void cmdSetMood(const String &val) {
    String upper = val; upper.toUpperCase();
    if (!isKnownMood(upper)) { Serial.printf("ERR: unknown mood '%s'\n", val.c_str()); return; }
    setMood(upper.c_str()); markDirty(); Serial.println("OK");
}
static void cmdSetBright(const String &val) {
    int v = val.toInt();
    if (v < 1 || v > 100) { Serial.println("ERR: 1-100"); return; }
    config.autoBright = false;
    setBacklight((uint8_t)v); markDirty(); Serial.println("OK");
}

static void cmdSetAutoBright(const String &val) {
    bool b;
    if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
    config.autoBright = b; markDirty();
    Serial.println(b ? "OK: auto on" : "OK: auto off");
}
static void cmdSetBrightLight(const String &val) {
    int v = val.toInt();
    if (v < 0 || v > 100) { Serial.println("ERR: 0-100"); return; }
    config.brightLight = (uint8_t)v; markDirty(); Serial.println("OK");
}
static void cmdSetBrightDark(const String &val) {
    int v = val.toInt();
    if (v < 0 || v > 100) { Serial.println("ERR: 0-100"); return; }
    config.brightDark = (uint8_t)v; markDirty(); Serial.println("OK");
}
static void cmdSetMenuTimeout(const String& val) {
    int v = val.toInt();
    if (v < 0 || v > 300) { Serial.println("ERR: 0-300"); return; }
    config.menuTimeoutSec = (uint16_t)v; markDirty(); Serial.println("OK");
}
static void cmdGetMenuTimeout() {
    Serial.printf("menu_timeout:   %u\n", config.menuTimeoutSec);
}
static void cmdGetTheme() {
    Serial.printf("theme:          %s\n", config.theme == THEME_DARK ? "dark" : "light");
}
static void cmdSetMode(const String& val) {
    String u = val; u.toUpperCase();
    if (u == "FACE")      { setUIMode(MODE_FACE); Serial.println("OK"); }
    else if (u == "MENU") { setUIMode(MODE_MENU); Serial.println("OK"); }
    else                  { Serial.println("ERR: FACE|MENU"); }
}

static void cmdSetLed(const String &val) {
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
static void cmdSetTap(const String &val) {
    int x, y, z = 1500;
    int n = sscanf(val.c_str(), "%d,%d,%d", &x, &y, &z);
    if (n < 2) { Serial.println("ERR: bad tap (need x,y or x,y,z)"); return; }
    if (x < 0 || x >= 320 || y < 0 || y >= 240) { Serial.println("ERR: out of range"); return; }
    recordTouch((uint16_t)x, (uint16_t)y, (uint16_t)z);
    Serial.printf("OK: tap (%d, %d) z=%d\n", x, y, z);
}

static void cmdSetFace(const String &val) {
    FaceShape s;
    if (!parseFaceShape(val, s)) { Serial.println("ERR: bad face (need topH,botH,tilt,pR,r as floats)"); return; }
    setFaceCustom(s); Serial.println("OK");
}
static void cmdSetFaceL(const String &val) {
    FaceShape s;
    if (!parseFaceShape(val, s)) { Serial.println("ERR: bad face_l (need topH,botH,tilt,pR,r as floats)"); return; }
    setFaceLeft(s); Serial.println("OK");
}
static void cmdSetFaceR(const String &val) {
    FaceShape s;
    if (!parseFaceShape(val, s)) { Serial.println("ERR: bad face_r (need topH,botH,tilt,pR,r as floats)"); return; }
    setFaceRight(s); Serial.println("OK");
}

static void cmdSetLook(const String &val) {
    int x, y;
    if (sscanf(val.c_str(), "%d,%d", &x, &y) != 2) { Serial.println("ERR: bad look (need x,y)"); return; }
    setLookAt((int16_t)x, (int16_t)y); Serial.println("OK");
}
static void cmdSetHud(const String &val) {
    bool b;
    if (!parseBool(val, b)) { Serial.println("ERR: bad bool"); return; }
    config.hudOn = b; setHud(b); markDirty(); Serial.println("OK");
}
static void cmdBlink()    { triggerBlink(); Serial.println("OK"); }
static void cmdGetLook()  { int16_t x, y; getLookAt(x, y); Serial.printf("look:           %d,%d\n", x, y); }
static void cmdGetHud()   { Serial.printf("hud:            %s\n", config.hudOn ? "on" : "off"); }
static void cmdGetFace() {
    FaceShape l, r; getFaceLeft(l); getFaceRight(r);
    Serial.printf("face:           L=%g,%g,%g,%g,%g R=%g,%g,%g,%g,%g\n",
                  l.topH, l.botH, l.tilt, l.pR, l.radius,
                  r.topH, r.botH, r.tilt, r.pR, r.radius);
}
static void cmdGetFaceL() {
    FaceShape s; getFaceLeft(s);
    Serial.printf("face_l:         %g,%g,%g,%g,%g\n", s.topH, s.botH, s.tilt, s.pR, s.radius);
}
static void cmdGetFaceR() {
    FaceShape s; getFaceRight(s);
    Serial.printf("face_r:         %g,%g,%g,%g,%g\n", s.topH, s.botH, s.tilt, s.pR, s.radius);
}
static void cmdGetMode()       { printMode(); }

// ============ Getter handlers ============
static void cmdGetTouchDebug() { Serial.printf("touch_debug:    %s\n", config.touchDebug    ? "on" : "off"); }
static void cmdGetMoodCycle()  { Serial.printf("mood_cycle:     %s\n", config.moodAutoCycle ? "on" : "off"); }
static void cmdGetTouchDots()  { Serial.printf("touch_dots:     %s\n", config.showTouchDots ? "on" : "off"); }
static void cmdGetEyeColour()  { Serial.printf("eye_colour:     %06X\n", (unsigned)rgb565_to_888(config.eyeColour)); }
static void cmdGetBgColour()   { Serial.printf("bg_colour:      %06X\n", (unsigned)rgb565_to_888(config.bgColour)); }
static void cmdGetMood()       { Serial.printf("mood:           %s\n", config.mood); }
static void cmdGetBright()     { Serial.printf("bright:         %u%%\n", config.brightness); }
static void cmdGetAutoBright() { Serial.printf("auto_bright:    %s\n", config.autoBright ? "on" : "off"); }
static void cmdGetBrightLight(){ Serial.printf("bright_light:   %u%%\n", config.brightLight); }
static void cmdGetBrightDark() { Serial.printf("bright_dark:    %u%%\n", config.brightDark); }
static void cmdGetVersion()    { Serial.printf("version:        %s\n", FW_VERSION); }
static String fmtBright() { return String(config.brightness) + "%"; }
static String fmtMood()   { return String(config.mood); }
static String fmtUptime() {
    uint32_t s = millis() / 1000;
    char buf[16];
    snprintf(buf, sizeof(buf), "%um %us", s / 60, s % 60);
    return String(buf);
}
static String fmtMem() { return String(ESP.getFreeHeap() / 1024) + "K"; }

// ============ Action handlers (no value, no get/set distinction) ============
static void cmdReset() {
    prefs.begin(NVS_NS, false); prefs.clear(); prefs.end();
    Serial.println("OK: NVS cleared, rebooting in 1 second...");
    delay(1000); ESP.restart();
}
static void cmdReboot() {
    Serial.println("OK: rebooting in 1 second...");
    flushNvs();
    delay(1000); ESP.restart();
}
static void cmdMem()        { printMemoryReport(); }
static void cmdUptime()     { printUptime(); }
static void cmdLdr()        { printLdr(); }
static void cmdClear()      { tft.fillScreen(config.bgColour); Serial.println("OK"); }
static void cmdGetLight()   { Serial.printf("light: %u\n", getLdrBrightnessPct()); }
static void cmdGetLux()     { Serial.printf("lux: %u\n",   getLdrLuxish()); }
static void cmdSplash()     { showSplash(); Serial.println("OK"); }
static void cmdPause()      { pauseFace();  Serial.println("OK: face paused"); }
static void cmdResume()     { resumeFace(); Serial.println("OK: face resumed"); }

// Forward decls for cmds that iterate the table
static void cmdHelp();
static void cmdStatus();

// ============ The dispatch table ============
// struct Command {
//     const char *name;
//     void (*set)(const String &val);   // nullptr if not settable
//     void (*get)();                    // nullptr if not gettable; also serves plain commands
//     const char *help;                 // nullptr to hide from help (aliases)
// };

static const Command commands[] = {
    // Settable + gettable config (appear in 'status')
    {"touch_debug",  cmdSetTouchDebug,  cmdGetTouchDebug,  "on|off"},
    {"mood_cycle",   cmdSetMoodCycle,   cmdGetMoodCycle,   "on|off"},
    {"touch_dots",   cmdSetTouchDots,   cmdGetTouchDots,   "on|off"},
    {"eye_colour",   cmdSetEyeColour,   cmdGetEyeColour,   "RRGGBB hex"},
    {"eye_color",    cmdSetEyeColour,   cmdGetEyeColour,   nullptr},   // alias, hidden
    {"bg_colour",    cmdSetBgColour,    cmdGetBgColour,    "RRGGBB hex"},
    {"bg_color",     cmdSetBgColour,    cmdGetBgColour,    nullptr},   // alias, hidden
    {"idle_anim",    cmdSetIdleAnim,    cmdGetIdleAnim,    "on|off random idle moods"},
    {"theme",        cmdSetTheme,       cmdGetTheme,       "light | dark (preset bg+eye; use bg_colour/eye_colour for arbitrary)"},
    {"mood",         cmdSetMood,        cmdGetMood,        "NEUTRAL|HAPPY|ANGRY|SAD|EXCITED|ANNOYED|QUESTIONING|IDLE1-3"},
    {"bright",       cmdSetBright,      cmdGetBright,      "0-100 backlight %"},
    {"auto_bright",  cmdSetAutoBright,  cmdGetAutoBright,  "on|off LDR-driven brightness"},
    {"bright_light", cmdSetBrightLight, cmdGetBrightLight, "1-100 target % when bright"},
    {"bright_dark",  cmdSetBrightDark,  cmdGetBrightDark,  "1-100 target % when dark"},
    {"menu_timeout", cmdSetMenuTimeout, cmdGetMenuTimeout, "0-300 seconds (0=disabled)"},
    {"look",         cmdSetLook,        cmdGetLook,        "x,y canvas offset"},
    {"hud",          cmdSetHud,         cmdGetHud,         "on|off Grobot HUD overlay"},
    {"face",         cmdSetFace,        cmdGetFace,        "topH,botH,tilt,pR,r — symmetric custom mood (floats)"},
    {"face_l",       cmdSetFaceL,       cmdGetFaceL,       "topH,botH,tilt,pR,r — left eye only"},
    {"face_r",       cmdSetFaceR,       cmdGetFaceR,       "topH,botH,tilt,pR,r — right eye only"},
    {"mode",         cmdSetMode,        cmdGetMode,        "FACE|MENU"},

    // Set-only commands
    {"led",          cmdSetLed,         nullptr,           "off|on|red|green|blue|white|yellow|cyan|magenta or r,g,b"},
    {"tap",          cmdSetTap,         nullptr,           "x,y[,z] inject touch"},

    // Plain actions (and queries with no setter)
    {"menu_back",    nullptr,           cmdMenuBack,       "pop one level"},
    {"menu",         nullptr,           cmdMenuState,      "current menu state"},
    {"help",         nullptr,           cmdHelp,           "this message"},
    {"status",       nullptr,           cmdStatus,         "print current config"},
    {"version",      nullptr,           cmdGetVersion,     "firmware version"},
    {"mem",          nullptr,           cmdMem,            "memory snapshot"},
    {"uptime",       nullptr,           cmdUptime,         "time since boot"},
    {"blink",        nullptr,           cmdBlink,          "trigger one blink"},
    {"ldr",          nullptr,           cmdLdr,            "light sensor reading"},
    {"light",        nullptr,           cmdGetLight,       "ambient brightness 0-100%"},
    {"lux",          nullptr,           cmdGetLux,         "inverted ldr (higher=brighter)"},
    {"clear",        nullptr,           cmdClear,          "fill screen with bg colour"},
    {"splash",       nullptr,           cmdSplash,         "re-show RSC splash"},
    {"pause",        nullptr,           cmdPause,          "freeze face renderer (use 'resume' to unfreeze)"},
    {"resume",       nullptr,           cmdResume,         "resume face renderer"},
    {"reboot",       nullptr,           cmdReboot,         "soft reboot (config preserved)"},
    {"reset",        nullptr,           cmdReset,          "clear NVS, reboot to defaults"},
    {"mood",         cmdSetMood,        cmdGetMood,        "NEUTRAL|HAPPY|ANGRY|SAD|EXCITED|ANNOYED|QUESTIONING|IDLE1-3",       fmtMood},
    {"bright",       cmdSetBright,      cmdGetBright,      "0-100 backlight %",                                                 fmtBright},
    {"uptime",       nullptr,           cmdUptime,         "time since boot",                                                   fmtUptime},
    {"mem",          nullptr,           cmdMem,            "memory snapshot",                                                   fmtMem},
};

static const size_t COMMAND_COUNT = sizeof(commands) / sizeof(commands[0]);

const Command *findCommand(const String &name) {
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        if (name == commands[i].name) return &commands[i];
    }
    return nullptr;
}

// ============ Help / status — auto-generated from the table ============
static void cmdHelp() {
    Serial.println("--- Commands ---");

    Serial.println("Setters (key:value):");
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        const Command &c = commands[i];
        if (!c.help || !c.set) continue;
        Serial.printf("  %-14s %s\n", c.name, c.help);
    }

    Serial.println("Getters (key?):");
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        const Command &c = commands[i];
        if (!c.help || !c.set || !c.get) continue;
        Serial.printf("  %s?\n", c.name);
    }

    Serial.println("Plain:");
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        const Command &c = commands[i];
        if (!c.help || c.set) continue;
        Serial.printf("  %-14s %s\n", c.name, c.help);
    }
}

static void cmdStatus() {
    Serial.println("--- Status ---");
    cmdGetVersion();
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        const Command &c = commands[i];
        if (!c.help) continue;          // skip aliases
        if (!c.set || !c.get) continue; // only settable+gettable config
        c.get();
    }
}

// ============ The dispatcher ============
static void parseLine(const String &raw) {
    String line = raw;
    line.trim();
    if (line.length() == 0) return;

    String key;
    String val;
    bool isGetter = false;
    bool isSetter = false;

    if (line.endsWith("?")) {
        key = line.substring(0, line.length() - 1);
        key.trim();
        if (key.indexOf(':') >= 0) {
            Serial.printf("ERR: bad query '%s' (use 'key?', not 'key:?')\n", line.c_str());
            return;
        }
        isGetter = true;
    } else {
        int colon = line.indexOf(':');
        if (colon >= 0) {
            key = line.substring(0, colon);
            val = line.substring(colon + 1);
            key.trim();
            val.trim();
            isSetter = true;
        } else {
            key = line;
        }
    }

    key.toLowerCase();
    const Command *cmd = findCommand(key);
    if (!cmd) {
        Serial.printf("ERR: unknown command '%s' (try 'help')\n", key.c_str());
        return;
    }

    if (isGetter) {
        if (cmd->get) cmd->get();
        else Serial.printf("ERR: '%s' has no getter\n", key.c_str());
    } else if (isSetter) {
        if (cmd->set) cmd->set(val);
        else Serial.printf("ERR: '%s' is not settable\n", key.c_str());
    } else {
        // Plain form: prefer get (acts as query for settable, action for plain)
        if (cmd->get) cmd->get();
        else if (cmd->set) Serial.printf("ERR: '%s' needs a value (try '%s:value')\n", key.c_str(), key.c_str());
        else Serial.printf("ERR: '%s' is not invokable\n", key.c_str());
    }
}

// ============ Non-blocking line reader ============
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
#include "LdrSensor.h"
#include "DisplayInit.h"   // setBacklight
#include "Config.h"        // config

static const int LDR_PIN = 34;

// LDR wiring: 3V3 -> fixed R -> GPIO34 -> LDR -> GND.
// analogRead therefore reads LOWER for BRIGHTER (bright ~0, dark ~LDR_DARK_REF).
static constexpr uint16_t LDR_DARK_REF = 1000;   // observed dark ceiling — tune empirically

// Hysteresis thresholds for auto-brightness state machine (used by next step).
// Trip when raw rises above _ENTER (room got dim), clear when it drops below _EXIT.
static constexpr uint16_t LDR_DARK_ENTER = 700;
static constexpr uint16_t LDR_DARK_EXIT  = 400;

static bool     _isDark          = false;
static bool     _initialised     = false;
static float    _currentEffective = 100.0f;
static uint32_t _lastTick        = 0;


uint16_t getLdrRaw() {
    uint32_t sum = 0;
    for (int i = 0; i < 8; i++) sum += analogRead(LDR_PIN);
    return sum / 8;
}

uint16_t getLdrLuxish() {
    return 4095 - getLdrRaw();
}

uint8_t getLdrBrightnessPct() {
    uint16_t raw = getLdrRaw();
    if (raw >= LDR_DARK_REF) return 0;
    return (uint8_t)(((LDR_DARK_REF - raw) * 100UL) / LDR_DARK_REF);
}

void printLdr() {
    Serial.printf("ldr: %u\n", getLdrRaw());
}

void serviceLdr() {
    if (!config.autoBright) {
        _initialised = false;   // reset so re-enable starts fresh
        return;
    }

    uint32_t now = millis();
    if (!_initialised) {
        _lastTick = now;
        _currentEffective = (float)config.brightness;   // start from current
        _initialised = true;
        return;
    }

    uint32_t elapsed = now - _lastTick;
    if (elapsed < 100) return;   // cap at ~10 Hz to keep ADC overhead low
    _lastTick = now;

    // Hysteresis state machine using existing thresholds
    uint16_t raw = getLdrRaw();
    if (!_isDark && raw > LDR_DARK_ENTER) _isDark = true;
    else if (_isDark && raw < LDR_DARK_EXIT) _isDark = false;

    uint8_t target = _isDark ? config.brightDark : config.brightLight;

    // Output-side lerp toward target — ~1s feel
    float dt   = elapsed / 1000.0f;
    float rate = 2.0f;   // tune for snappier/softer transitions
    _currentEffective += ((float)target - _currentEffective) * dt * rate;

    // Only call setBacklight when the integer value actually changes (avoid PWM thrash)
    uint8_t newPwm = (uint8_t)(_currentEffective + 0.5f);
    static uint8_t lastApplied = 255;
    if (newPwm != lastApplied) {
        setBacklight(newPwm);
        lastApplied = newPwm;
    }
}
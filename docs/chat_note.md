=== CYD firmware — context for new chat ===

Repo: https://github.com/RobotStudyCompanion/CYD
Branches:
  main      branch-protected, integration target
  dev/mbz   my branch (current work)
  dev/gio   student's parallel Grobot test
Licence: Apache 2.0

=== Hardware ===
ESP32-2432S028R (CYD original variant)
- ESP32 WROOM, no PSRAM, ~302 KB heap free at boot, 107 KB largest block
- ILI9341 320x240, SPI on (DC=2, CS=15, SCK=14, MOSI=13, MISO=12), BL=21
- XPT2046 resistive touch (independent bus, bitbang)
- RGB LED on GPIO 4/16/17 (active low)
- LDR: investigated, none of GPIO 32/33/34/35/36/39 show light-responsive
  behaviour. Floating-input signatures on most pins suggest this variant
  doesn't have an LDR connected to any standard ADC1 pin. Either a clone
  without LDR, or wired non-standardly. Hardware investigation needed
  (visual / multimeter) before further software trial. External LDR on a
  free GPIO is a fallback if light sensing genuinely needed.

=== Software stack ===
- platform = espressif32 6.4.0 (Arduino-ESP32 2.0.11)
- bodmer/TFT_eSPI 2.5.43 (ILI9341_2_DRIVER, 55 MHz SPI)
- tanmaywankar/Grobot_Animations 0.1.3, 320x140 sprite (~90 KB at RGB565)
- hexeguitar/CYD28_Touchscreen (bitbang fork, avoids HSPI collision)
- bitbank2/JPEGDEC (kept for future serial-JPEG, currently unused)
- Built-in Preferences for NVS persistence
- Git-describe versioning via PlatformIO extra_script

=== Source layout ===
include/
  Config.h         shared config struct + initConfig/serviceConfig
  DebugOverlay.h   touch dots + memory/uptime/ldr/version helpers
  DisplayInit.h    TFT_eSPI init + setBacklight (PWM on GPIO 21)
  FaceRenderer.h   Grobot wrapper, setEyeColour/setBackgroundColour/setMood
  LED_Solution.h   setLedColor(r,g,b) for RGB LED
  MjpegClass.h     JPEG decoder wrapper (unused, kept for future)
  TouchHandler.h   initTouch + getTouchPoint (raw)
  version.h        AUTO-GENERATED, gitignored
src/
  Config.cpp       dispatch-table command parser, NVS load/save
  DebugOverlay.cpp dot rendering + edge detection + diag prints
  DisplayInit.cpp  display init + backlight PWM (LEDC channel 0)
  FaceRenderer.cpp Grobot wrapper, splash, mood/colour application
  LED_Solution.cpp
  TouchHandler.cpp
  main.cpp         pure orchestration (~30 lines)
scripts/
  version.py       git describe -> include/version.h pre-build hook

=== Architecture ===
- Config struct is single source of truth for stateful settings
  (touchDebug, moodAutoCycle, showTouchDots, eyeColour, bgColour, mood, brightness)
- Config command parser uses dispatch table (Command struct with set/get
  function pointers); auto-generates help and status output from the table
- FaceRenderer reads from config; setters update config and rebuild eyes/sprite
- DebugOverlay owns all visual debug + serial diagnostic helpers
- DisplayInit owns BL pin via LEDC (channel 0, 5 kHz, 8-bit PWM)
- All persistent settings stored in NVS namespace "rsc", saved on every change
- main.cpp = pure orchestration: printVersion, printMemoryReport, init*, service*

=== UART command grammar ===
- Setter form:    key:value     e.g. mood:HAPPY, eye_colour:00FF00, bright:128
- Getter form:    key?          e.g. mood?, bright?
- Plain form:     key           acts as getter for settable, action for plain
- 22 commands total. Type 'help' on serial monitor for the list.
- All settable values persisted to NVS automatically on change.

=== Key build flags (platformio.ini) ===
USER_SETUP_LOADED=1, ILI9341_2_DRIVER=1
TFT_MISO=12 TFT_MOSI=13 TFT_SCLK=14 TFT_CS=15 TFT_DC=2 TFT_RST=-1 TFT_BL=21
TFT_BACKLIGHT_ON=HIGH, SPI_FREQUENCY=55000000
LOAD_GLCD=1 LOAD_FONT2=1 LOAD_FONT4=1 LOAD_GFXFF=1 SMOOTH_FONT=1
extra_scripts = pre:scripts/version.py
monitor_echo = yes

=== Done across the whole journey ===
- Track 1 quick fixes (HSPI swap, halt removal, debounce, debug gating)
- Migration Arduino_GFX -> TFT_eSPI + Grobot_Animations
- Sprite resize 120 -> 140 px tall (crisper eyes within memory budget)
- Renames: AnimationPlayer -> DisplayInit, TouchOverlay -> DebugOverlay
- DebugOverlay extracted: dots, edge detection, mem/uptime/ldr/version helpers
- Config layer: NVS-persisted struct + non-blocking serial parser
- Dispatch-table refactor: 22 commands, auto-generated help/status
- Backlight PWM, LED control, tap injection, splash/clear actions
- Version system: git-describe -> version.h, monitor echo enabled

=== Open / next ===
- LDR: probably absent on this variant; the 'ldr' command currently always
  returns 0 (or noise). Either remove the command or hook to an external
  sensor on a free GPIO. Defer until light-sensing actually needed.- PR dev/mbz to main; tag v0.1.0 (version string auto-upgrades from hash to semver)
- Coordinate with Gio on Grobot before merging anything that touches FaceRenderer
- Track 3: UIManager (modes FACE/MENU/TEXT), long-press menu trigger,
  UART 'text:...' command for MODE_TEXT, PNG via bitbank2/PNGdec,
  larger fonts via Adafruit GFX free fonts, mode-aware loop()
- Future: light-driven auto-brightness once LDR pin is sorted
- Future: status/loading screens (sprite + static background pattern from whiteboard)
- Future: serial-JPEG path (re-activate MjpegClass for 'image:' command)
- Minor: showSplash() blocks 800ms; non-blocking version wanted for Track 3
- Minor: 'clear' command is partial in eye band (sprite repaints over)
- Expose Grobot's fine-grained controls over UART:
    face:topH,botH,tilt,pR,r        custom mood (symmetric)
    face_l:... / face_r:...         asymmetric per-eye
    look:x,y                        canvas offset (lookAt)
    blink                           manual blink trigger
    hud:on|off                      FPS overlay toggle
  All map to existing Grobot public API; ~50-60 lines via dispatch table.
  Unblocks host-side tooling (e.g. live-control extension).

=== Tooling / host-side ===
- VS Code / Codium extension idea: live remote control of CYD over serial,
  panel UI sends UART commands, reads back via getters. Lives in separate repo.
  Bottleneck is firmware API surface, addressed by face-control commands above.
- Tanmay's GrobotAnimator (pre-alpha) at tanmaywankar.github.io/GrobotAnimator
  — could contribute back to it as a richer offline simulator, separate effort
- Future fork of Grobot to expose eyeGap/blinkTime/spring constants as overridable
  via #ifndef guards; submit as PR upstream rather than maintaining a private fork
- Future RPi/laptop counterpart controller: speaks the UART grammar, drives the
  CYD in production. Existing 22 commands + face controls cover the surface.

=== User preferences (carry forward) ===
British English, active voice. Brief diff points (file + location + change),
not full file rewrites where avoidable. Ask before heavy refactor / token-
burny work. Working within hardware limitations — no PSRAM expansion unless
forced. Like the rhythm of one fix at a time with checkpoints.
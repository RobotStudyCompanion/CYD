=== CYD firmware — context for new chat ===

Repo: https://github.com/RobotStudyCompanion/CYD
My branch: dev/mbz
Student branch: dev/gio (Grobot library author's parallel test)
Integration: main (branch-protected)
Licence: Apache 2.0

=== Stack ===
- ESP32 WROOM (no PSRAM, ~305 KB heap, 107 KB largest block)
- Display: ILI9341 320x240 via TFT_eSPI, ILI9341_2_DRIVER, 55 MHz SPI
- Touch: XPT2046 via hexeguitar/CYD28_Touchscreen (bitbang, independent bus)
- LEDs: 3 GPIO pins, active-low RGB
- Eyes: tanmaywankar/Grobot_Animations v0.1.3, 320x120 sprite ~76 KB

=== Files ===
- main.cpp — Serial setup, init sequence, loop calls serviceFaceRenderer()
- AnimationPlayer.cpp — TFT_eSPI display init (poorly named, will rename)
- FaceRenderer.cpp — wraps GrobotEyes, exposes setEyeColour/setBackgroundColour,
  MOOD_AUTO_CYCLE compile toggle for demo cycling vs static NEUTRAL
- TouchHandler.cpp — XPT2046 wrapper, debug prints currently unconditional
- LED_Solution.cpp — RGB LED control
- MjpegClass.h — kept for future serial-JPEG work, currently unused

=== Build flags (platformio.ini) ===
USER_SETUP_LOADED=1, ILI9341_2_DRIVER=1, pins TFT_MISO=12 TFT_MOSI=13
TFT_SCLK=14 TFT_CS=15 TFT_DC=2 TFT_RST=-1 TFT_BL=21 BACKLIGHT_ON=HIGH
SPI_FREQUENCY=55000000

=== Done ===
- Track 1 quick fixes (HSPI swap, halt removal, debounce, etc)
- Migration: Arduino_GFX → TFT_eSPI + Grobot_Animations
- Smooth flicker-free eye animation, white on black, mood cycler

=== Open / next ===
- Track 3: UI scaffold (UIManager, modes, long-press menu, UART text:... cmd, PNG)
- Rename AnimationPlayer → DisplayInit
- Gate TouchHandler debug prints behind TOUCH_DEBUG
- Future: serial-driven config (eye colour, mood from UART)
- Future: status screens, loading screens (sprite + static background pattern)
- Gio testing Grobot on dev/gio in parallel — coordinate before merging

=== User preferences ===
British English, active voice. Brief diff points (file + location + change),
not full file rewrites. Ask before heavy refactor / token-burny work.
Working within hardware limitations — no PSRAM expansion unless forced.
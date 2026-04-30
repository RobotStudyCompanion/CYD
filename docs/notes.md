# RSC CYD firmware — Track 1 handover

End-of-session notes for the next chat. Branch: `dev/mbz`.

## What we set out to do

Track 1 quick fixes against student's `dev/gio` branch. Original list had ten items focused on robustness (HSPI collision, missing-folder halts, dead debounce, etc.) of the existing MJPEG-from-SD animation pipeline.

## What we actually did

The scope shifted mid-session. SD card was lost and the user decided not to architect around SD/MJPEG playback — declared obsolete. The MJPEG pipeline came out wholesale; an orbiting-dot placeholder went in instead. Track 1 effectively reduced to: HSPI fix, display rotation, UART timeout, plus a structural cleanup of the animation player.

Decisions reached during the session:

- **Touch driver swapped** to `hexeguitar/CYD28_Touchscreen` (MIT) for the bitbang HSPI fix. We considered `TheNitek/XPT2046_Bitbang_Arduino_Library` first but rejected on licence grounds — GPL-3.0 conflicts with this repo's Apache 2.0.
- **MJPEG/SD pipeline deleted**, not commented. `AnimationPlayer.cpp` slimmed down to a thin display init wrapper.
- **Sprite-driven face is the architectural direction.** The original UART command set (`caring`/`surprised`/`pride` → SD folder switch) is dead. Future protocol will drive procedural face rendering with parameters, eventually steered by a small LM.
- **Stay on Arduino_GFX for now**, switch to TFT_eSPI in the next chat. Reasoning: Grobot, RandomNerd, every CYD tutorial, and most face-rendering prior art uses TFT_eSPI; switching aligns us with the ecosystem before we build anything real on top.
- **PSRAM mod is a "nice to have"**, not a blocker. Stock CYD has no PSRAM (confirmed by runtime check — see Diagnostics below).
- **Module swap (ESP32 → ESP32-S3) considered and rejected.** S3 modules share rough footprint with WROOM-32 but differ in GPIO assignments, strapping pins, USB pin positions, and U4 PSRAM routing. A swap amounts to a partial board redesign, not a drop-in mod. For future S3 capability, source CYD variants that ship with S3 already on board (Sunton makes these). Existing 5 CYDs stay on classic ESP32 + U4 PSRAM mod.
- **Flicker on the placeholder dot is acceptable.** Burning canvas-buffering work into Arduino_GFX is throwaway code given the upcoming library swap.

## File state at end of session

```
src/
├── AnimationPlayer.cpp    — slim display init, calls FaceRenderer
├── FaceRenderer.cpp       — splash + orbiting dot (placeholder)
├── FaceRenderer.h
├── TouchHandler.cpp       — bitbang touch via CYD28_TouchscreenR
├── TouchHandler.h
├── LED_Solution.cpp       — unchanged
├── LED_Solution.h
└── main.cpp               — splash → idle dot → touch + serial concurrent
archive/
└── AnimationPlayer.cpp    — original MJPEG/SD code, kept for reference
```

`platformio.ini` adds `build_flags = -DBOARD_HAS_PSRAM`. Harmless on stock board (PSRAM detection just returns false), required for when the mod lands.

## Diagnostics from final flash

```
PSRAM found: no
Heap free: 302 KB
Largest heap block: 107 KB
```

107 KB largest contiguous block bounds any future canvas to roughly 230×230 RGB565 (~106 KB) or smaller. A 200×120 face-region canvas (48 KB) sits comfortably within budget without PSRAM.

## Track 1 line-by-line status

| # | Original fix | Status |
|---|---|---|
| 1 | HSPI bus collision — bitbang touch | **Done** |
| 2 | Missing-folder halt | **Moot** — SD code removed |
| 3 | `used_to_be_setup` halts | **Moot** — function rewritten |
| 4 | Dead debounce | **Moot** — playback function removed |
| 5 | Rename `used_to_be_setup` → `initAnimationPlayer` | **Done** as part of rewrite |
| 6 | Dead `static File mjpegFile` | **Moot** — file removed |
| 7 | `mjpegFileSizes[]` decision | **Moot** — file removed |
| 8 | `gfx->setRotation(1)` | **Done** |
| 9 | Wrap touch debug prints in `#ifdef TOUCH_DEBUG` | **Pending** — kept live for smoke testing |
| 10 | `Serial.setTimeout(50)` | **Done** |

## What we know works

Display init, splash render, touch reads, RGB LED writes, and serial I/O all run concurrently without HSPI contention. The original symptom (touch reads corrupting display SPI) is fixed and verified.

## What's queued for the next chat

Next chat (call it Track 1.5 — library swap):

1. Switch `Arduino_GFX_Library` → `TFT_eSPI`. Lift the known-good `User_Setup.h` from RandomNerd's CYD tutorial; verify display + touch + LED still work concurrently.
2. Resolve fix #9 (debug-print gating) once we know what the new touch-coord pipeline looks like.
3. Drop the orbiting dot, replace with a minimal `TFT_eSprite`-backed face primitive (two procedural eyes, no mouth yet). Steal architecture from `tanmaywankar/Grobot_Animations`, don't depend on it directly — Grobot's emotion-string API is too coarse for the LM-tuning vision.
4. Add the serial `tune <param> <value>` command for live parameter editing.

Track 2 (non-blocking animation refactor from the original brief): now moot — there's no MJPEG pipeline left to refactor.

Track 3 begins after Track 1.5: `UIManager` with mode enum, `Rect::contains()` hotspot helper, long-press detection, mode-aware `loop()` switch, UART `text:...` command for transient text mode. Stays roughly as originally scoped.

## Hardware to procure

For the 5-unit RSC prototype run, BoM for the U4 PSRAM mod per hexeguitar's [CYD mod doc](https://github.com/hexeguitar/ESP32_TFT_PIO):

| Qty | Part | Digi-Key P/N | Notes |
|-----|------|--------------|-------|
| 5 | Adafruit 4677 PSRAM (rebranded ESP-PSRAM64H) | `1528-4677-ND` | 8 Mbit chip, ESP32 maps 4 MB usable. SOP-8, hand-solderable. |
| 5 | 1206 red LED, ~620 nm, Vf ~2V | search MFR P/N `LTST-C150KRKT` (Lite-On) or equivalent | Replaces red channel of removed RGB LED, drives off GPIO4 via existing R17. |

Total approx. €10–12 + VAT for both items × 5. Bundles into the existing Digi-Key cart cleanly.

### Why this part choice

- **Adafruit 4677 over the AP Memory branded `APS6404L-3SQR-SN`:** identical silicon, identical pinout (verified across both datasheets), MIT-rebranded by Adafruit. Digi-Key stocks the Adafruit version; AP Memory direct goes through Mouser. Single-supplier sourcing wins.
- **4 MB usable over 2 MB (`APS1604M-3SQR-SN`):** the 2 MB part isn't stocked at Digi-Key in any recognisable form. The 4 MB part is the same price and the same effort, so capacity is essentially free. ESP32's classic memory controller caps PSRAM mapping at 4 MB regardless of chip size, so the 8 Mbit chip's "wasted" 4 MB is moot.
- **`-3SQR-SN` suffix is mandatory.** The `-1SQR` variant is 1.8V and won't work in the CYD. The USON-8 (`-ZR` suffix) is a different package.

### Mod sequence (per hexeguitar)

1. Cut CS trace and SCK trace at marked points on the back of the board.
2. Solder PSRAM chip onto U4 footprint.
3. Remove existing RGB LED (sacrifices green and blue channels — RSC doesn't need them).
4. Solder 1206 red LED across middle pads of the removed RGB LED footprint (now driven by GPIO4 via existing R17).
5. Add fine enamel wire (magnet wire, ~30 AWG) jumpers from PSRAM CS and SCK pads to the freed-up RGB LED pads as shown in the mod photo. Existing R16/R20 (1kΩ) become the pull-ups for SCK/CS — no new resistors needed.

### Caveats

- PSRAM uses GPIO16 and GPIO17 — the green and blue RGB LED channels. Mod sacrifices those LED colours. RSC accepts red-only LED.
- Hand-solderable with iron + flux (1.27 mm pitch gull-wing leads, no hot-air required).
- Magnet wire isn't a Digi-Key item — usually scavenged from transformer salvage, or order separately as "magnet wire 30 AWG" from any electronics supplier.

## Reference material worth keeping

- Grobot Animator (live web tuner for parameter design): https://tanmaywankar.github.io/GrobotAnimator/
- Grobot library source: https://github.com/tanmaywankar/Grobot_Animations
- Hexeguitar CYD reference project + mod docs: https://github.com/hexeguitar/ESP32_TFT_PIO
- ThomasHelman CYD face (band-compositor flicker trick): https://github.com/ThomasHelman123/Animated-Robot-Face-on-ESP32-CYD-with-Floating-Heart-Eye-Animations
- Random Nerd CYD page (User_Setup.h for TFT_eSPI swap): https://randomnerdtutorials.com/cheap-yellow-display-esp32-2432s028r/
- CYD community hub: https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display

## Open questions for next session

- Once on TFT_eSPI, is the flicker actually solved by `TFT_eSprite` alone, or does the band-compositor pattern from ThomasHelman matter?
- Serial protocol shape — `tune eye_radius 12` (text) vs binary frames? Text is friendlier for the LM and for hand-debugging; binary scales better. Pick one before writing the parser.
- `FaceState` schema — what parameters does the LM actually need to control? Lift Grobot's list (eye height, slope, radius, offset, pupil position) as v0, extend later.
- Sprite module question (raised but not resolved): if face primitives are procedural, the only sprites we need are decorative accents (logos, status icons). Probably not worth a dedicated module yet.
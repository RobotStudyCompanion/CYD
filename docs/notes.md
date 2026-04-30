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

- **APS6404L-3SQR-SN** (8 Mbit / 8 MB QSPI PSRAM, SOP-8, 3.3V) — fits unpopulated U4 footprint per hexeguitar's [CYD mod doc](https://github.com/hexeguitar/ESP32_TFT_PIO). Sources: TinyTronics (NL), Rutronik24 (DE), Mouser (878-APS6404L-3SQR-SN), or Adafruit's rebranded equivalent (Digi-Key 1528-4677-ND). Avoid the USON-8 variant — different package.
- **Caveat:** PSRAM uses GPIO16 and GPIO17 — the green and blue RGB LED channels. Mod sacrifices those LED colours unless the red channel gets rewired to GPIO4. User has accepted "red-only or no LED" as acceptable.
- Hand-solderable with iron + flux (1.27 mm pitch gull-wing leads, no hot-air required).

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
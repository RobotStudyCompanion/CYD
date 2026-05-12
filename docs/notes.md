## continuing dev for RSC CYD firmware, v0.3.0 in progress, branch dev/mbz

* GUISlice based Builder-side UI design well underway. 
  * E_PG_MAIN laid out: burger (top-right), help (?), RSC logo (centre-left), two sliders (volume + brightness) with toggle icons below (audio mute, auto-bright A-in-sun), plus power (top-left), back-arrow (right middle), mic (bottom-right).
* Icon pipeline partly automated (lives at ~/Documents/RSC/CYDisplays_resources/image2c-linux-x64-3.02/resources/):
  * icon_prep.sh — bash + ImageMagick, PNG → resize → auto-invert → threshold → bilevel → transparent. Flags: -s WxH, -i auto|on|off, -c (emit C array).
  * pbm_to_gslc_c.py — Python helper, byte-identical to Image2C output except for one cosmetic last-line trailing-whitespace pad. Functionally identical.
  * All ~12 icons in 1bpp monochrome, white-on-transparent.
* Design discipline locked to black-and-white for future e-ink compatibility + flash savings.

### Open queue, priority-ordered:

1. Create four popup pages in Builder — E_PG_POPUP_MENU (from burger), E_PG_POPUP_HELP (?), E_PG_POPUP_STATUS (RSC logo), E_PG_POPUP_PWR (power confirm "[Yes]/[Cancel]").
2. Wire main-page triggers via each button's Popup Page Enum property.
3. Convert three icons to Togglable Image Buttons — mic (mic_on ↔ mute_on), audio (volume_loud/volume_low (whether slider above/below 50% maybe?) ↔ volume_mute (backend send to alsamixer via daemon service to host - mutes RSC; no daemon/host side app yet)), auto-bright (with/without A (invokes onboard LDR to auto adjust brightness, works - needs plumbing after CYD UI deployed...)). Set Toggle?=true. Manually update Image Select Extern (Builder doesn't propagate?)... new to this... 
4. Clarify the back-arrow's role on main page: exits menu to face-mode (Grobot animated faces, MIT); retains position for popup submenus as a 'back'.

(DONE, test pending once face built) Phase B firmware-side integration — main.cpp refactor for FACE/MENU mode switching, GUIslice init wiring, touch routing branch, Start/Face overlay via direct TFT_eSPI (not GUIslice). Builder output gets pasted into project via four surgical edits already documented.

(PENDING, after phase B validated...) Phase C — dispatch table wiring in button callbacks via findCommand(). Toggles read state via gslc_ElemXTogglebtnGetState().

### Filed for later:

* Power-off semantic for USB-powered CYD (deep-sleep + wake-on-touch, or just blank display). ...actually the power popup menu i want it to show host ctrl (RPi) e.g. reboot/shutdown; and cyd ctrl, e.g. reset (loads defaults, wipes NVS)/reboot (reloads as is)
* Status popup dynamic redraw — per-element gslc_ElemSetRedraw for live fields (uptime, mem). Uses the format() callback pattern from v0.2.1. ...thought is that user taps the big RSC icon in base menu and that pops up a bunch of stats, e.g. fw version, uptime, then host side stuff like loaded model, or some usage facts... 
* Submenu nav within popups if needed later; for now single-level popups only.
* Host-side Codium extension (face simulator + menu authoring + serial bridge).
* TEXT mode design pass.
* Grobot mouth (fork vs rewrite, defer).
* we dont have a BMS or much PMIC stuff onboard but later might add e.g. voltages per rail in status maybe... maybe battery % (progress bar)

### Stuff to remember:

Dispatch table is the spine. Every UI surface, UART command, menu tap routes through findCommand(). The durable contract. Communicates with host service (pending separately) which would reside on the RPi in the RSC... the CYD talks to RPi via Serial and the dispatch table (config.cpp) is the API we can work with, extend, adjust etc.
UART grammar key:value, key?, plain key is the wire contract.
Wired-only project (USB to RPi/laptop). No WiFi/BT/OTA.
CYD: ESP32-2432S028R, 320×240 ILI9341 SPI 55 MHz, XPT2046 resistive touch bitbanged, ~300 KB heap, 4 MB flash.
platformio.ini: TFT_eSPI pinned 2.5.43. GUIslice via lib_deps. -DGSLC_FEATURE_COMPOUND=1 required for keypads. -DTOUCH_DEBUG commented for clean builds.
Audio routes through host running alsamixer over serial; CYD is the control surface, not the audio path.
Touch targets ≥40 px, 48 px ideal (i put icons as 40x40px, maybe ok). Sliders preferred over keypads for continuous values.
Builder generates .ino + _GSLC.h — paste-into-project workflow, not platformio template.
No Base Page needed (single-pane with overlays). (technically this is phase B and mostly done, pending tests...)
Every popup must have a dismiss path (Close button or similar). (that's the same back icon thingy we have on base menu - i want to keep that exact same location for each menu)

### Preferences carried forward:

British English, active voice. Brief diff points (file + location + change), not full file rewrites. Ask before token-burny work. Save tokens by default. One-fix-at-a-time rhythm with checkpoints. Omit time/effort estimates entirely. Don't invent API specifics — verify from docs or examples. Don't pre-conclude on architectural decisions; surface options with trade-offs (prefer tabular representations over long prose).
#include "test_GSLC.h"
#include "UIManager.h"
#include "Config.h"
#include "DisplayInit.h" // setBacklight
#include "version.h"


// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef* m_pElemOutTxt11   = NULL;
gslc_tsElemRef* m_pElemOutTxt8    = NULL;
gslc_tsElemRef* m_pElemSlider2    = NULL;
gslc_tsElemRef* m_pElemSlider2_3  = NULL;
gslc_tsElemRef* m_pElemToggle2_7  = NULL;
gslc_tsElemRef* m_pElemToggle2_7_10= NULL;
gslc_tsElemRef* m_pElemToggleImg29= NULL;
gslc_tsElemRef* m_pElemToggleImg32= NULL;

extern "C" const unsigned short volume_low_icon2_40x40px[]  PROGMEM;
extern "C" const unsigned short volume_mute_icon2_40x40px[] PROGMEM;
extern "C" const unsigned short mute_on_icon_40x40px[]      PROGMEM;
extern "C" const unsigned short bright_40x40px[]            PROGMEM;

//<Save_References !End!>

// Confirm popup: pending command stashed by destructive buttons, executed by YES
#define PENDING_CMD_MAX 24

static char m_acPendingCmd[PENDING_CMD_MAX] = {0};
static bool _muted = false;
static bool _micMuted = false;
static int16_t _volumeLevel = 50;
static const void* _lastVolIcon = nullptr;

// Cached refs — populated by initMenuIcons()
static gslc_tsElemRef* _refVolume = nullptr;
static gslc_tsElemRef* _refMic    = nullptr;
static gslc_tsElemRef* _refBright = nullptr;


static gslc_tsElemRef* _refStatsUp   = nullptr;
static gslc_tsElemRef* _refStatsHeap = nullptr;
static char _bufStatsUp[24]   = "";
static char _bufStatsHeap[24] = "";

// Routes host_* commands to Serial; everything else to the local dispatch table
static void executePending() {
    if (strncmp(m_acPendingCmd, "host_", 5) == 0) {
        Serial.println(m_acPendingCmd);  // host service consumes via its serial listener
    } else {
        const Command* cmd = findCommand(String(m_acPendingCmd));
        if (cmd && cmd->get) cmd->get();
        else Serial.printf("ERR: no handler for '%s'\n", m_acPendingCmd);
    }
}


static void applyIcon(gslc_tsElemRef* ref, const void* iconArr) {
    if (!ref) return;
    gslc_tsImgRef img = gslc_GetImageFromProg((const unsigned char*)iconArr, GSLC_IMGREF_FMT_RAW1);
    gslc_ElemSetImage(&m_gui, ref, img, img);
}

static void refreshVolumeIcon() {
    const void* iconArr;
    if (_muted)                  iconArr = volume_mute_icon2_40x40px;
    else if (_volumeLevel > 50)  iconArr = volume_loud_icon2_40x40px;
    else                         iconArr = volume_low_icon2_40x40px;
    if (iconArr == _lastVolIcon) return;
    _lastVolIcon = iconArr;
    applyIcon(_refVolume, iconArr);
}

void initMenuIcons() {
  _refVolume = gslc_PageFindElemById(&m_gui, E_PG_MAIN, E_ELEM_IMAGEBTN_VOLUME);
  _refMic    = gslc_PageFindElemById(&m_gui, E_PG_MAIN, E_ELEM_IMAGEBTN_MIC);
  _refBright = gslc_PageFindElemById(&m_gui, E_PG_MAIN, E_ELEM_IMAGEBTN_BRIGHTNESS);
  refreshVolumeIcon();
  applyIcon(_refMic, mic_on_icon_40x40px);
  applyIcon(_refBright, config.autoBright ? auto_bright_40x40px : bright_40x40px);
  // Programmatically add two more stats lines on the burger menu page
  _refStatsUp = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_BURGER_MENU,
      (gslc_tsRect){20, 110, 240, 30}, _bufStatsUp, sizeof(_bufStatsUp), E_BUILTIN10X16);
  gslc_ElemSetTxtCol(&m_gui, _refStatsUp, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, _refStatsUp, GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);

  _refStatsHeap = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_BURGER_MENU,
      (gslc_tsRect){20, 150, 240, 30}, _bufStatsHeap, sizeof(_bufStatsHeap), E_BUILTIN10X16);
  gslc_ElemSetTxtCol(&m_gui, _refStatsHeap, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, _refStatsHeap, GSLC_COL_BLACK, GSLC_COL_BLACK, GSLC_COL_BLACK);
}

void refreshStatsText() {
    if (!m_pElemOutTxt8 || !_refStatsUp || !_refStatsHeap) return;

    // Shorten FW_VERSION: keep "v<tag>-<commits>", drop "-g<hash>-dirty"; mark dirty with "+"
    char shortVer[20];
    const char* ver = FW_VERSION;
    const char* hashMark = strstr(ver, "-g");
    int keepLen = hashMark ? (int)(hashMark - ver) : (int)strlen(ver);
    if (keepLen > (int)sizeof(shortVer) - 2) keepLen = sizeof(shortVer) - 2;
    memcpy(shortVer, ver, keepLen);
    shortVer[keepLen] = '\0';
    bool dirty = strstr(ver, "-dirty") != nullptr;
    if (dirty && keepLen + 1 < (int)sizeof(shortVer)) {
        shortVer[keepLen] = '+';
        shortVer[keepLen + 1] = '\0';
    }

    static char buf[40];
    uint32_t s  = millis() / 1000;
    uint32_t h  = s / 3600;
    uint32_t mn = (s % 3600) / 60;
    uint32_t sc = s % 60;

    snprintf(buf, sizeof(buf), "fw  %s", shortVer);
    gslc_ElemSetTxtStr(&m_gui, m_pElemOutTxt8, buf);

    snprintf(buf, sizeof(buf), "up  %02u:%02u:%02u", (unsigned)h, (unsigned)mn, (unsigned)sc);
    gslc_ElemSetTxtStr(&m_gui, _refStatsUp, buf);

    snprintf(buf, sizeof(buf), "heap  %u", (unsigned)ESP.getFreeHeap());
    gslc_ElemSetTxtStr(&m_gui, _refStatsHeap, buf);
}

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>
      case E_ELEM_IMAGEBTN_BACK:
        setUIMode(MODE_FACE);
        break;
      case E_ELEM_IMAGEBTN_VOLUME:
        _muted = !_muted;
        Serial.println("host_mute");
        refreshVolumeIcon();
        break;

      case E_ELEM_IMAGEBTN_MIC:
        _micMuted = !_micMuted;
        Serial.println("host_mic");
        applyIcon(_refMic, _micMuted ? mute_on_icon_40x40px : mic_on_icon_40x40px);
        break;

      case E_ELEM_IMAGEBTN_BRIGHTNESS: {
        const Command* cmd = findCommand(String("auto_bright"));
        if (cmd && cmd->set) cmd->set(config.autoBright ? "off" : "on");
        applyIcon(_refBright, config.autoBright ? auto_bright_40x40px : bright_40x40px);
        break;
      }
      case E_ELEM_IMAGEBTN5:    // RSC logo — returns to face per UI design
        setUIMode(MODE_FACE); // RSC logo — exit via long-press only
        break;
      case E_ELEM_IMAGEBTN_BURGER:
        gslc_PopupShow(&m_gui, E_PG_BURGER_MENU, true);
        break;
      case E_ELEM_IMAGEBTN_PWR:
        gslc_PopupShow(&m_gui, E_PG_PWR, true);
        break;
      case E_ELEM_BTN_MENU_CLOSE:
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_TOGGLE_THEME: {
        bool state = gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_7_10);
        const Command* cmd = findCommand(String("theme"));
        if (cmd && cmd->set) cmd->set(state ? "light" : "dark");
        break;
      }
      case E_ELEM_TOGGLE_DEBUG: {
        bool state = gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_7);
        const Command* cmd = findCommand(String("touch_debug"));
        if (cmd && cmd->set) cmd->set(state ? "on" : "off");
        break;
      }
      case E_ELEM_IMAGEBTN_PWR_CLOSE:
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_BTN5:  // Pi Poweroff (destructive)
        strncpy(m_acPendingCmd, "host_poweroff", PENDING_CMD_MAX);
        gslc_ElemSetTxtStr(&m_gui, m_pElemOutTxt11, "Power off Pi?");
        gslc_PopupHide(&m_gui);
        gslc_PopupShow(&m_gui, E_PG_POPUP_CONFIRM, true);
        break;
      case E_ELEM_BTN6:  // Pi Reboot (recoverable)
        Serial.println("host_reboot");
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_BTN10:  // CYD Reset (destructive, wipes NVS)
        strncpy(m_acPendingCmd, "reset", PENDING_CMD_MAX);
        gslc_ElemSetTxtStr(&m_gui, m_pElemOutTxt11, "Reset CYD? (wipes NVS)");
        gslc_PopupHide(&m_gui);
        gslc_PopupShow(&m_gui, E_PG_POPUP_CONFIRM, true);
        break;
      case E_ELEM_BTN11: {  // CYD Reboot (recoverable)
        const Command* cmd = findCommand(String("reboot"));
        if (cmd && cmd->get) cmd->get();
        gslc_PopupHide(&m_gui);
        break;
      }
      case E_ELEM_IMAGEBTN_CONFIRM_BACK:
        gslc_PopupHide(&m_gui);
        gslc_PopupShow(&m_gui, E_PG_PWR, true);
        break;
      case E_ELEM_BTN_YES:
        executePending();
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_BTN_CANCEL:
        gslc_PopupHide(&m_gui);
        gslc_PopupShow(&m_gui, E_PG_PWR, true);
        break;

//<Button Enums !End!>
      default:
        break;
    }
  }
  return true;
}
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
//<Keypad Callback !Start!>
//<Keypad Callback !End!>
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);
  int16_t         nVal;

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
//<Slider Enums !Start!>
    case E_ELEM_SLIDER2: {  // volume — throttled host updates (~10 Hz)
    _volumeLevel = nPos;
    static uint32_t _lastVolSendMs = 0;
    if (millis() - _lastVolSendMs >= 100) {
        Serial.printf("host_vol:%d\n", nPos);
        _lastVolSendMs = millis();
    }
    if (!_muted) refreshVolumeIcon();
    break;
    }
    case E_ELEM_SLIDER3: {  // brightness — direct call, no "OK" spam
      int16_t v = (nPos < 1) ? 1 : nPos;
      config.autoBright = false;
      setBacklight((uint8_t)v);
      markDirty();
      break;
    }

//<Slider Enums !End!>
    default:
      break;
  }

  return true;
}
//<Tick Callback !Start!>
//<Tick Callback !End!>
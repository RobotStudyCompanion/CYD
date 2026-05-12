#include "test_GSLC.h"
#include "UIManager.h"
#include "Config.h"


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
//<Save_References !End!>

// Confirm popup: pending command stashed by destructive buttons, executed by YES
#define PENDING_CMD_MAX 24
static char m_acPendingCmd[PENDING_CMD_MAX] = {0};

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
        // Exit via long-press only — handled in UIManager::serviceUI  
        setUIMode(MODE_FACE);
        break;
      case E_ELEM_IMAGEBTN_VOLUME:
        break;
      case E_ELEM_IMAGEBTN_MIC:
        break;
      case E_ELEM_IMAGEBTN_BRIGHTNESS:
        break;
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
      case E_ELEM_TOGGLE_THEME:
        // TODO Add code for Toggle button ON/OFF state
        if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle2_7_10)) {
          ;
        }
        break;
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
    case E_ELEM_SLIDER2:  // volume — sends to host service
      Serial.printf("host_vol:%d\n", nPos);
      break;
    case E_ELEM_SLIDER3: {  // brightness
      int16_t v = (nPos < 1) ? 1 : nPos;   // cmdSetBright requires 1-100
      const Command* cmd = findCommand(String("bright"));
      if (cmd && cmd->set) cmd->set(String(v));
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
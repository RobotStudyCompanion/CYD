//<File !Start!>
// FILE: [test_GSLC.h]
// Created by GUIslice Builder version: [0.17.b41]
//
// GUIslice Builder Generated GUI Framework File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>

#ifndef _GUISLICE_GEN_H
#define _GUISLICE_GEN_H

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>
// Include extended elements
#include "elem/XSlider.h"
#include "elem/XToggleImgbtn.h"
#include "elem/XTogglebtn.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#if !defined(DRV_DISP_TFT_ESPI)
  #error E_PROJECT_OPTIONS tab->Graphics Library should be Adafruit_GFX
#endif
#include <TFT_eSPI.h>
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
extern "C" const unsigned short auto_bright_40x40px[] PROGMEM;
extern "C" const unsigned short back_40x40px[] PROGMEM;
extern "C" const unsigned short burger_icon_40x40px[] PROGMEM;
extern "C" const unsigned short mic_on_icon_40x40px[] PROGMEM;
extern "C" const unsigned short power_icon_40x40px[] PROGMEM;
extern "C" const unsigned short rsc_130x130[] PROGMEM;
extern "C" const unsigned short volume_loud_icon2_40x40px[] PROGMEM;
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN,E_PG_BURGER_MENU,E_PG_PWR,E_PG_POPUP_CONFIRM};
enum {E_DRAW_LINE1,E_DRAW_LINE2,E_DRAW_LINE3,E_ELEM_BOX1
      ,E_ELEM_BOX_CONFIRM,E_ELEM_BTN10,E_ELEM_BTN11,E_ELEM_BTN5
      ,E_ELEM_BTN6,E_ELEM_BTN_CANCEL,E_ELEM_BTN_MENU_CLOSE
      ,E_ELEM_BTN_YES,E_ELEM_IMAGEBTN5,E_ELEM_IMAGEBTN_BACK
      ,E_ELEM_IMAGEBTN_BRIGHTNESS,E_ELEM_IMAGEBTN_BURGER
      ,E_ELEM_IMAGEBTN_CONFIRM_BACK,E_ELEM_IMAGEBTN_MIC
      ,E_ELEM_IMAGEBTN_PWR,E_ELEM_IMAGEBTN_PWR_CLOSE
      ,E_ELEM_IMAGEBTN_VOLUME,E_ELEM_MENU_BOX,E_ELEM_PWR_BOX
      ,E_ELEM_SLIDER2,E_ELEM_SLIDER3,E_ELEM_TEXT10,E_ELEM_TEXT9
      ,E_ELEM_TEXT_CONFIRM,E_ELEM_TEXT_DEBUG_TOGGLE,E_ELEM_TEXT_STATS
      ,E_ELEM_TEXT_THEME,E_ELEM_TOGGLE_DEBUG,E_ELEM_TOGGLE_THEME};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_BUILTIN10X16,E_BUILTIN15X24,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                4

#define MAX_ELEM_PG_MAIN 10 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

#define MAX_ELEM_PG_BURGER_MENU 11 // # Elems total on page
#define MAX_ELEM_PG_BURGER_MENU_RAM MAX_ELEM_PG_BURGER_MENU // # Elems in RAM

#define MAX_ELEM_PG_PWR 9 // # Elems total on page
#define MAX_ELEM_PG_PWR_RAM MAX_ELEM_PG_PWR // # Elems in RAM

#define MAX_ELEM_PG_POPUP_CONFIRM 5 // # Elems total on page
#define MAX_ELEM_PG_POPUP_CONFIRM_RAM MAX_ELEM_PG_POPUP_CONFIRM // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                     m_asPopup5Elem[MAX_ELEM_PG_BURGER_MENU_RAM];
gslc_tsElemRef                  m_asPopup5ElemRef[MAX_ELEM_PG_BURGER_MENU];
gslc_tsElem                     m_asPopup6Elem[MAX_ELEM_PG_PWR_RAM];
gslc_tsElemRef                  m_asPopup6ElemRef[MAX_ELEM_PG_PWR];
gslc_tsElem                     m_asPopup7Elem[MAX_ELEM_PG_POPUP_CONFIRM_RAM];
gslc_tsElemRef                  m_asPopup7ElemRef[MAX_ELEM_PG_POPUP_CONFIRM];
gslc_tsXSlider                  m_sXSlider2;
gslc_tsXToggleImgbtn            m_sToggleImg32;
gslc_tsXToggleImgbtn            m_sToggleImg29;
gslc_tsXSlider                  m_sXSlider3;
gslc_tsXTogglebtn               m_asXToggle10;
gslc_tsXTogglebtn               m_asXToggle7;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Element References for direct access
//<Extern_References !Start!>
extern gslc_tsElemRef* m_pElemOutTxt11;
extern gslc_tsElemRef* m_pElemOutTxt8;
extern gslc_tsElemRef* m_pElemSlider2;
extern gslc_tsElemRef* m_pElemSlider2_3;
extern gslc_tsElemRef* m_pElemToggle2_7;
extern gslc_tsElemRef* m_pElemToggle2_7_10;
extern gslc_tsElemRef* m_pElemToggleImg29;
extern gslc_tsElemRef* m_pElemToggleImg32;
//<Extern_References !End!>

// Define debug message function
static int16_t DebugOut(char ch);

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY);
bool CbCheckbox(void* pvGui, void* pvElemRef, int16_t nSelId, bool bState);
bool CbDrawScanner(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);
bool CbKeypad(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbListbox(void* pvGui, void* pvElemRef, int16_t nSelId);
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos);
bool CbSpinner(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbTickScanner(void* pvGui,void* pvScope);

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
void InitGUIslice_gen()
{
  gslc_tsElemRef* pElemRef = NULL;

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_BUILTIN10X16,GSLC_FONTREF_PTR,NULL,2)) { return; }
    if (!gslc_FontSet(&m_gui,E_BUILTIN15X24,GSLC_FONTREF_PTR,NULL,3)) { return; }
//<Load_Fonts !End!>

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_BURGER_MENU,m_asPopup5Elem,MAX_ELEM_PG_BURGER_MENU_RAM,m_asPopup5ElemRef,MAX_ELEM_PG_BURGER_MENU);
  gslc_PageAdd(&m_gui,E_PG_PWR,m_asPopup6Elem,MAX_ELEM_PG_PWR_RAM,m_asPopup6ElemRef,MAX_ELEM_PG_PWR);
  gslc_PageAdd(&m_gui,E_PG_POPUP_CONFIRM,m_asPopup7Elem,MAX_ELEM_PG_POPUP_CONFIRM_RAM,m_asPopup7ElemRef,MAX_ELEM_PG_POPUP_CONFIRM);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
   
  // Create E_ELEM_MENU_BOX box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_MENU_BOX,E_PG_MAIN,(gslc_tsRect){0,0,320,240});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_ELEM_IMAGEBTN_BACK button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_IMAGEBTN_BACK,E_PG_MAIN,(gslc_tsRect){270,100,40,40},
          gslc_GetImageFromProg((const unsigned char*)back_40x40px,GSLC_IMGREF_FMT_RAW1),
          gslc_GetImageFromProg((const unsigned char*)back_40x40px,GSLC_IMGREF_FMT_RAW1),
          &CbBtnCommon);

  // Create slider E_ELEM_SLIDER2 
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_ELEM_SLIDER2,E_PG_MAIN,&m_sXSlider2,
          (gslc_tsRect){210,10,40,180},0,100,0,10,true);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_WHITE,5,10,GSLC_COL_WHITE);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  m_pElemSlider2 = pElemRef;
  
  // Create E_ELEM_IMAGEBTN_VOLUME button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_IMAGEBTN_VOLUME,E_PG_MAIN,(gslc_tsRect){210,190,40,40},
        gslc_GetImageFromProg((const unsigned char*)volume_loud_icon2_40x40px,GSLC_IMGREF_FMT_RAW1),
        gslc_GetImageFromProg((const unsigned char*)volume_loud_icon2_40x40px,GSLC_IMGREF_FMT_RAW1),
        &CbBtnCommon);
  m_pElemToggleImg32 = pElemRef;
  
  // Create E_ELEM_IMAGEBTN_MIC button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_IMAGEBTN_MIC,E_PG_MAIN,(gslc_tsRect){270,190,40,40},
        gslc_GetImageFromProg((const unsigned char*)mic_on_icon_40x40px,GSLC_IMGREF_FMT_RAW1),
        gslc_GetImageFromProg((const unsigned char*)mic_on_icon_40x40px,GSLC_IMGREF_FMT_RAW1),
        &CbBtnCommon);
  m_pElemToggleImg29 = pElemRef;

  // Create slider E_ELEM_SLIDER3 
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_ELEM_SLIDER3,E_PG_MAIN,&m_sXSlider3,
          (gslc_tsRect){150,10,40,180},0,100,0,10,true);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_WHITE,5,10,GSLC_COL_WHITE);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  m_pElemSlider2_3 = pElemRef;
  
  // Create E_ELEM_IMAGEBTN_BRIGHTNESS button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_IMAGEBTN_BRIGHTNESS,E_PG_MAIN,(gslc_tsRect){150,190,40,40},
          gslc_GetImageFromProg((const unsigned char*)auto_bright_40x40px,GSLC_IMGREF_FMT_RAW1),
          gslc_GetImageFromProg((const unsigned char*)auto_bright_40x40px,GSLC_IMGREF_FMT_RAW1),
          &CbBtnCommon);
  
  // Create E_ELEM_IMAGEBTN5 button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_IMAGEBTN5,E_PG_MAIN,(gslc_tsRect){10,70,130,130},
          gslc_GetImageFromProg((const unsigned char*)rsc_130x130,GSLC_IMGREF_FMT_RAW1),
          gslc_GetImageFromProg((const unsigned char*)rsc_130x130,GSLC_IMGREF_FMT_RAW1),
          &CbBtnCommon);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // Create E_ELEM_IMAGEBTN_BURGER button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_IMAGEBTN_BURGER,E_PG_MAIN,(gslc_tsRect){270,10,40,40},
          gslc_GetImageFromProg((const unsigned char*)burger_icon_40x40px,GSLC_IMGREF_FMT_RAW1),
          gslc_GetImageFromProg((const unsigned char*)burger_icon_40x40px,GSLC_IMGREF_FMT_RAW1),
          &CbBtnCommon);
  
  // Create E_ELEM_IMAGEBTN_PWR button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_IMAGEBTN_PWR,E_PG_MAIN,(gslc_tsRect){10,10,40,40},
          gslc_GetImageFromProg((const unsigned char*)power_icon_40x40px,GSLC_IMGREF_FMT_RAW1),
          gslc_GetImageFromProg((const unsigned char*)power_icon_40x40px,GSLC_IMGREF_FMT_RAW1),
          &CbBtnCommon);

  // -----------------------------------
  // PAGE: E_PG_BURGER_MENU
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_BURGER_MENU,(gslc_tsRect){10,10,300,220});
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // Create E_ELEM_BTN_MENU_CLOSE button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_BTN_MENU_CLOSE,E_PG_BURGER_MENU,(gslc_tsRect){270,100,40,40},
          gslc_GetImageFromProg((const unsigned char*)back_40x40px,GSLC_IMGREF_FMT_RAW1),
          gslc_GetImageFromProg((const unsigned char*)back_40x40px,GSLC_IMGREF_FMT_RAW1),
          &CbBtnCommon);
  
  // Create toggle button E_ELEM_TOGGLE_THEME
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE_THEME,E_PG_BURGER_MENU,&m_asXToggle10,
    (gslc_tsRect){180,20,60,30},GSLC_COL_WHITE,GSLC_COL_WHITE,GSLC_COL_BLACK,
    true,false,&CbBtnCommon);
  m_pElemToggle2_7_10 = pElemRef;
  
  // Create E_ELEM_TEXT_THEME text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT_THEME,E_PG_BURGER_MENU,(gslc_tsRect){240,20,60,30},
    (char*)"Theme",0,E_BUILTIN10X16);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create toggle button E_ELEM_TOGGLE_DEBUG
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE_DEBUG,E_PG_BURGER_MENU,&m_asXToggle7,
    (gslc_tsRect){20,20,60,30},GSLC_COL_WHITE,GSLC_COL_WHITE,GSLC_COL_BLACK,
    true,false,&CbBtnCommon);
  m_pElemToggle2_7 = pElemRef;
  
  // Create E_ELEM_TEXT_DEBUG_TOGGLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT_DEBUG_TOGGLE,E_PG_BURGER_MENU,(gslc_tsRect){80,20,60,30},
    (char*)"Debug",0,E_BUILTIN10X16);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create E_ELEM_TEXT_STATS runtime modifiable text
  static char m_sDisplayText8[121] = "Stats";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT_STATS,E_PG_BURGER_MENU,(gslc_tsRect){20,70,240,30},
    (char*)m_sDisplayText8,121,E_BUILTIN10X16);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  m_pElemOutTxt8 = pElemRef;

  // Create E_DRAW_LINE1 line 
  pElemRef = gslc_ElemCreateLine(&m_gui,E_DRAW_LINE1,E_PG_BURGER_MENU,10,60,310,60);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_WHITE);

  // Create E_DRAW_LINE2 line 
  pElemRef = gslc_ElemCreateLine(&m_gui,E_DRAW_LINE2,E_PG_BURGER_MENU,160,10,160,60);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_WHITE);

  // -----------------------------------
  // PAGE: E_PG_PWR
  
   
  // Create E_ELEM_PWR_BOX box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_PWR_BOX,E_PG_PWR,(gslc_tsRect){10,10,300,220});
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_ELEM_IMAGEBTN_PWR_CLOSE button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_IMAGEBTN_PWR_CLOSE,E_PG_PWR,(gslc_tsRect){270,100,40,40},
          gslc_GetImageFromProg((const unsigned char*)back_40x40px,GSLC_IMGREF_FMT_RAW1),
          gslc_GetImageFromProg((const unsigned char*)back_40x40px,GSLC_IMGREF_FMT_RAW1),
          &CbBtnCommon);
  
  // create E_ELEM_BTN5 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN5,E_PG_PWR,
    (gslc_tsRect){20,20,110,40},(char*)"Poweroff",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN6 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN6,E_PG_PWR,
    (gslc_tsRect){190,20,110,40},(char*)"Reboot",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);

  // Create E_DRAW_LINE3 line 
  pElemRef = gslc_ElemCreateLine(&m_gui,E_DRAW_LINE3,E_PG_PWR,10,120,270,120);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY_LT2,GSLC_COL_GRAY_LT2);
  
  // create E_ELEM_BTN10 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN10,E_PG_PWR,
    (gslc_tsRect){20,180,110,40},(char*)"Reset",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN11 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN11,E_PG_PWR,
    (gslc_tsRect){190,180,110,40},(char*)"Reboot",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // Create E_ELEM_TEXT9 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT9,E_PG_PWR,(gslc_tsRect){50,70,216,24},
    (char*)"Raspberry Pi",0,E_BUILTIN15X24);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_ELEM_TEXT10 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT10,E_PG_PWR,(gslc_tsRect){50,150,198,24},
    (char*)"Front panel",0,E_BUILTIN15X24);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_POPUP_CONFIRM
  
   
  // Create E_ELEM_BOX_CONFIRM box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX_CONFIRM,E_PG_POPUP_CONFIRM,(gslc_tsRect){10,50,300,130});
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // Create E_ELEM_IMAGEBTN_CONFIRM_BACK button with image label
  pElemRef = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_IMAGEBTN_CONFIRM_BACK,E_PG_POPUP_CONFIRM,(gslc_tsRect){270,100,40,40},
          gslc_GetImageFromProg((const unsigned char*)back_40x40px,GSLC_IMGREF_FMT_RAW1),
          gslc_GetImageFromProg((const unsigned char*)back_40x40px,GSLC_IMGREF_FMT_RAW1),
          &CbBtnCommon);
  
  // Create E_ELEM_TEXT_CONFIRM runtime modifiable text
  static char m_sDisplayText11[51] = "Message";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT_CONFIRM,E_PG_POPUP_CONFIRM,(gslc_tsRect){20,60,240,60},
    (char*)m_sDisplayText11,51,E_BUILTIN10X16);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  m_pElemOutTxt11 = pElemRef;
  
  // create E_ELEM_BTN_YES button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_YES,E_PG_POPUP_CONFIRM,
    (gslc_tsRect){30,130,90,30},(char*)"Yes",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // create E_ELEM_BTN_CANCEL button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_CANCEL,E_PG_POPUP_CONFIRM,
    (gslc_tsRect){160,130,90,30},(char*)"Cancel",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
//<InitGUI !End!>

//<Startup !Start!>
  gslc_GuiRotate(&m_gui, 1);
//<Startup !End!>

}

#endif // end _GUISLICE_GEN_H

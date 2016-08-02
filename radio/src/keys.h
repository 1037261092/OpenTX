/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _KEYS_H_
#define _KEYS_H_

enum EnumKeys {
#if defined(PCBHORUS)
  KEY_PGUP,
  KEY_PGDN,
  KEY_ENTER,
  KEY_MODEL,
  KEY_UP=KEY_MODEL,
  KEY_EXIT,
  KEY_DOWN=KEY_EXIT,
  KEY_TELEM,
  KEY_RIGHT=KEY_TELEM,
  KEY_RADIO,
  KEY_LEFT=KEY_RADIO,
#elif defined(PCBFLAMENCO)
  KEY_MENU,
  KEY_EXIT,
  KEY_ENTER,
  KEY_PAGE,
  KEY_PLUS, // TODO remove
  KEY_MINUS, // TODO remove
#elif defined(PCBTARANIS)
  KEY_MENU,
  KEY_EXIT,
  KEY_ENTER,
  KEY_PAGE,
  KEY_PLUS,
  KEY_MINUS,
#else
  KEY_MENU,
  KEY_ENTER=KEY_MENU,
  KEY_EXIT,
  KEY_DOWN,
  KEY_UP,
  KEY_RIGHT,
  KEY_LEFT,
#endif

  TRM_BASE,
  TRM_LH_DWN = TRM_BASE,
  TRM_LH_UP,
  TRM_LV_DWN,
  TRM_LV_UP,
  TRM_RV_DWN,
  TRM_RV_UP,
  TRM_RH_DWN,
  TRM_RH_UP,

#if defined(PCBHORUS)
  TRM_LS_DWN,
  TRM_LS_UP,
  TRM_RS_DWN,
  TRM_RS_UP,
  TRM_LAST = TRM_RS_UP,
#else
  TRM_LAST = TRM_RH_UP,
#endif

#if ROTARY_ENCODERS > 0 || defined(ROTARY_ENCODER_NAVIGATION)
  BTN_REa,
#endif
#if ROTARY_ENCODERS > 0
  BTN_REb,
#endif

  NUM_KEYS,
  SW_BASE=NUM_KEYS,

#if defined(PCBFLAMENCO)
  SW_SA0=SW_BASE,
  SW_SA1,
  SW_SA2,
  SW_SB0,
  SW_SB2,
  SW_SC0,
  SW_SC1,
  SW_SC2,
  SW_SC3,
  SW_SC4,
  SW_SC5,
  SW_SE0,
  SW_SE2,
  SW_SF0,
  SW_SF1,
  SW_SF2,
#elif defined(PCBTARANIS) || defined(PCBHORUS)
  SW_SA0=SW_BASE,
  SW_SA1,
  SW_SA2,
  SW_SB0,
  SW_SB1,
  SW_SB2,
  SW_SC0,
  SW_SC1,
  SW_SC2,
  SW_SD0,
  SW_SD1,
  SW_SD2,
  SW_SE0,
  SW_SE1,
  SW_SE2,
  SW_SF0,
  SW_SF1,
  SW_SF2,
  SW_SG0,
  SW_SG1,
  SW_SG2,
  SW_SH0,
  SW_SH1,
  SW_SH2,
#if defined(PCBX9E)
  SW_SI0,
  SW_SI1,
  SW_SI2,
  SW_SJ0,
  SW_SJ1,
  SW_SJ2,
  SW_SK0,
  SW_SK1,
  SW_SK2,
  SW_SL0,
  SW_SL1,
  SW_SL2,
  SW_SM0,
  SW_SM1,
  SW_SM2,
  SW_SN0,
  SW_SN1,
  SW_SN2,
  SW_SO0,
  SW_SO1,
  SW_SO2,
  SW_SP0,
  SW_SP1,
  SW_SP2,
  SW_SQ0,
  SW_SQ1,
  SW_SQ2,
  SW_SR0,
  SW_SR1,
  SW_SR2,
#endif
#else
  SW_ID0=SW_BASE,
  SW_ID1,
  SW_ID2,
  SW_THR,
  SW_RUD,
  SW_ELE,
  SW_AIL,
  SW_GEA,
  SW_TRN,
#endif

};

#define EVT_KEY_MASK(e)      ((e) & 0x1f)

#if defined(PCBHORUS)
#define _MSK_KEY_BREAK       0x0200
#define _MSK_KEY_REPT        0x0400
#define _MSK_KEY_FIRST       0x0600
#define _MSK_KEY_LONG        0x0800
#define _MSK_KEY_FLAGS       0x0e00
#define EVT_ENTRY            0x1000
#define EVT_ENTRY_UP         0x2000
#else
#define _MSK_KEY_BREAK       0x20
#define _MSK_KEY_REPT        0x40
#define _MSK_KEY_FIRST       0x60
#define _MSK_KEY_LONG        0x80
#define _MSK_KEY_FLAGS       0xe0
#define EVT_ENTRY            0xbf
#define EVT_ENTRY_UP         0xbe
#endif

#define EVT_KEY_BREAK(key)   ((key)|_MSK_KEY_BREAK)
#define EVT_KEY_FIRST(key)   ((key)|_MSK_KEY_FIRST)
#define EVT_KEY_REPT(key)    ((key)|_MSK_KEY_REPT)
#define EVT_KEY_LONG(key)    ((key)|_MSK_KEY_LONG)

#define IS_KEY_BREAK(evt)    (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_BREAK)
#define IS_KEY_FIRST(evt)    (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_FIRST)
#define IS_KEY_LONG(evt)     (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_LONG)
#define IS_KEY_REPT(evt)     (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_REPT)

#if defined(PCBHORUS)
  #define EVT_ROTARY_BREAK   EVT_KEY_BREAK(KEY_ENTER)
  #define EVT_ROTARY_LONG    EVT_KEY_LONG(KEY_ENTER)
  #define EVT_ROTARY_LEFT    0xDF00
  #define EVT_ROTARY_RIGHT   0xDE00
#elif defined(PCBTARANIS) || defined(PCBFLAMENCO)
  #define EVT_ROTARY_BREAK   EVT_KEY_BREAK(KEY_ENTER)
  #define EVT_ROTARY_LONG    EVT_KEY_LONG(KEY_ENTER)
#else
  #define EVT_ROTARY_BREAK   0xcf
  #define EVT_ROTARY_LONG    0xce
  #define EVT_ROTARY_LEFT    0xdf
  #define EVT_ROTARY_RIGHT   0xde
#endif

#if defined(COLORLCD)
  #define EVT_REFRESH        0xDD00
#endif

class Key
{
#define FILTERBITS      4

#ifdef SIMU
  #define FFVAL 1
#else
  #define FFVAL          ((1<<FILTERBITS)-1)
#endif

#define KSTATE_OFF      0
#define KSTATE_RPTDELAY 95 // gruvin: delay state before key repeating starts
#define KSTATE_START    97
#define KSTATE_PAUSE    98
#define KSTATE_KILLED   99

  private:
    uint8_t m_vals;   // key debounce?  4 = 40ms
    uint8_t m_cnt;
    uint8_t m_state;
  public:
    void input(bool val);
    bool state()       { return m_vals > 0; }
    void pauseEvents() { m_state = KSTATE_PAUSE; m_cnt = 0; }
    void killEvents()  { m_state = KSTATE_KILLED; }
    EnumKeys key() const;
};

extern Key keys[NUM_KEYS];

#if defined(COLORLCD)
typedef uint16_t event_t;
#else
typedef uint8_t event_t;
#endif

extern event_t s_evt;

#define putEvent(evt) s_evt = evt

void pauseEvents(uint8_t enuk);
void killEvents(uint8_t enuk);

#if defined(CPUARM)
  bool clearKeyEvents();
  event_t getEvent(bool trim=false);
#else
  void clearKeyEvents();
  event_t getEvent();
#endif

uint8_t keyDown();

#endif // _KEYS_H_

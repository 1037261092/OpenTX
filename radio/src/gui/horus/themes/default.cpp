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

#include "opentx.h"

const uint8_t LBM_TOPMENU_MASK_OPENTX[] = {
#include "mask_topmenu_opentx.lbm"
};

const ZoneOption OPTIONS_THEME_DEFAULT[] = {
  { "Background color", ZoneOption::Color, OPTION_DEFAULT_VALUE_UNSIGNED(WHITE) },
  { "Main color", ZoneOption::Color, OPTION_DEFAULT_VALUE_UNSIGNED(RED) },
  { NULL, ZoneOption::Bool }
};

class DefaultTheme: public Theme
{
  public:
    DefaultTheme():
      Theme("Default", OPTIONS_THEME_DEFAULT)
    {
      loadColors();
    }

    void loadColors() const
    {
      lcdColorTable[TEXT_COLOR_INDEX] = BLACK;
      lcdColorTable[TEXT_BGCOLOR_INDEX] = WHITE;
      lcdColorTable[TEXT_INVERTED_COLOR_INDEX] = WHITE;
      lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = RED;
      lcdColorTable[LINE_COLOR_INDEX] = GREY;
      lcdColorTable[SCROLLBOX_COLOR_INDEX] = RED;
      lcdColorTable[MENU_TITLE_BGCOLOR_INDEX] = DARKGREY;
      lcdColorTable[MENU_TITLE_COLOR_INDEX] = WHITE;
      lcdColorTable[MENU_TITLE_DISABLE_COLOR_INDEX] = RGB(130, 1, 5);
      lcdColorTable[HEADER_COLOR_INDEX] = DARKGREY;
      lcdColorTable[ALARM_COLOR_INDEX] = RED;
      lcdColorTable[WARNING_COLOR_INDEX] = YELLOW;
      lcdColorTable[TEXT_DISABLE_COLOR_INDEX] = GREY;
      lcdColorTable[CURVE_AXIS_COLOR_INDEX] = LIGHTGREY;
      lcdColorTable[CURVE_COLOR_INDEX] = RED;
      lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = RED;
      lcdColorTable[TITLE_BGCOLOR_INDEX] = RED;
      lcdColorTable[TRIM_BGCOLOR_INDEX] = RED;
      lcdColorTable[TRIM_SHADOW_COLOR_INDEX] = BLACK;
      lcdColorTable[MAINVIEW_PANES_COLOR_INDEX] = WHITE;
      lcdColorTable[MAINVIEW_GRAPHICS_COLOR_INDEX] = RED;
      lcdColorTable[HEADER_BGCOLOR_INDEX] = DARKRED;
      lcdColorTable[HEADER_ICON_BGCOLOR_INDEX] = RED;
      lcdColorTable[HEADER_CURRENT_BGCOLOR_INDEX] = RED;
      lcdColorTable[OVERLAY_COLOR_INDEX] = BLACK;
    }

    virtual void load() const
    {
      loadColors();
      Theme::load();
      if (!backgroundBitmap) backgroundBitmap = BitmapBuffer::load(getThemePath("mainbg.bmp"));
      if (!aboutBackgroundBitmap) aboutBackgroundBitmap = BitmapBuffer::load(getThemePath("aboutbg.bmp"));
    }

    virtual void drawBackground() const
    {
      if (backgroundBitmap) {
        lcd->drawBitmap(0, 0, backgroundBitmap);
      }
      else {
        lcdSetColor(g_eeGeneral.themeData.options[0].unsignedValue);
        lcdDrawSolidFilledRect(0, 0, LCD_W, LCD_H, CUSTOM_COLOR);
      }
    }

    virtual void drawAboutBackground() const
    {
      lcd->drawBitmap(0, 0, aboutBackgroundBitmap);
    }

    virtual void drawTopbarBackground(const uint8_t * icon) const
    {
      // TODO should be in updateOptions
      if (lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] != g_eeGeneral.themeData.options[1].unsignedValue) {
        lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = g_eeGeneral.themeData.options[1].unsignedValue;
        lcdColorTable[SCROLLBOX_COLOR_INDEX] = g_eeGeneral.themeData.options[1].unsignedValue;
        lcdColorTable[CURVE_COLOR_INDEX] = g_eeGeneral.themeData.options[1].unsignedValue;
        lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = g_eeGeneral.themeData.options[1].unsignedValue;
        lcdColorTable[TITLE_BGCOLOR_INDEX] = g_eeGeneral.themeData.options[1].unsignedValue;
        lcdColorTable[TRIM_BGCOLOR_INDEX] = g_eeGeneral.themeData.options[1].unsignedValue;
        lcdColorTable[MAINVIEW_GRAPHICS_COLOR_INDEX] = g_eeGeneral.themeData.options[1].unsignedValue;
        lcdColorTable[HEADER_ICON_BGCOLOR_INDEX] = g_eeGeneral.themeData.options[1].unsignedValue;
        lcdColorTable[HEADER_CURRENT_BGCOLOR_INDEX] = g_eeGeneral.themeData.options[1].unsignedValue;
        // TODO quick and really dirty, create a new macro for that ...
        lcdColorTable[HEADER_BGCOLOR_INDEX] = g_eeGeneral.themeData.options[1].unsignedValue - RGB(69, 32, 30);
      }

      lcdDrawSolidFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
      lcdDrawBitmapPattern(0, 0, LBM_TOPMENU_POLYGON, TITLE_BGCOLOR);

      if (icon) {
        lcdDrawBitmapPattern(5, 7, icon, MENU_TITLE_COLOR);
      }
      else {
        lcdDrawBitmapPattern(4, 10, LBM_TOPMENU_MASK_OPENTX, MENU_TITLE_COLOR);
      }

      drawTopbarDatetime();
    }

  protected:
    static const BitmapBuffer * backgroundBitmap;
    static const BitmapBuffer * aboutBackgroundBitmap;
};

const BitmapBuffer * DefaultTheme::backgroundBitmap = NULL;
const BitmapBuffer * DefaultTheme::aboutBackgroundBitmap = NULL;

DefaultTheme defaultTheme;
Theme * theme = &defaultTheme;

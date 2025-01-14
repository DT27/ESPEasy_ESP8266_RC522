#ifndef PLUGINSTRUCTS_P012_DATA_STRUCT_H
#define PLUGINSTRUCTS_P012_DATA_STRUCT_H

#include "../../_Plugin_Helper.h"

#ifdef USES_P012

# include <LiquidCrystal_I2C.h>

enum class P012_splashState_e : uint8_t {
  SplashCleared      = 0u,
  SplashTimerRunning = 1u,
  SplashInitial      = 2u
};

struct P012_data_struct : public PluginTaskData_base {
  P012_data_struct(uint8_t addr,
                   uint8_t lcd_size,
                   uint8_t mode,
                   uint8_t timer);
  P012_data_struct()          = delete;
  virtual ~P012_data_struct() = default;

  void init();

  void setBacklightTimer(uint8_t timer);

  void checkTimer();

  void lcdWrite(const String& text,
                uint8_t       col,
                uint8_t       row);

  String P012_parseTemplate(String& tmpString,
                            uint8_t lineSize);

  void   createCustomChars();


  LiquidCrystal_I2C  lcd;
  int                Plugin_012_cols = 16;
  int                Plugin_012_rows = 2;
  int                Plugin_012_mode = 1;
  uint8_t            displayTimer    = 0;
  P012_splashState_e splashState     = P012_splashState_e::SplashInitial;
};

#endif // ifdef USES_P012

#endif // ifndef PLUGINSTRUCTS_P012_DATA_STRUCT_H

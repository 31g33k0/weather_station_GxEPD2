

// NOTE: you may need to adapt or select for your wiring in the processor specific conditional compile sections below

// select the display class (only one), matching the kind of display panel
#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
//#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
//#define GxEPD2_DISPLAY_CLASS GxEPD2_7C

// select the display driver class (only one) for your  panel
//#define GxEPD2_DRIVER_CLASS GxEPD2_154_M10 // GDEW0154M10 152x152, UC8151D

// SS is usually used for CS. define here for easy change
#ifndef EPD_CS
#define EPD_CS SS
#endif

#if defined(GxEPD2_DISPLAY_CLASS) && defined(GxEPD2_DRIVER_CLASS)

// somehow there should be an easier way to do this
#define GxEPD2_BW_IS_GxEPD2_BW true
#define GxEPD2_3C_IS_GxEPD2_3C true
#define GxEPD2_7C_IS_GxEPD2_7C true
#define GxEPD2_1248_IS_GxEPD2_1248 true
#define GxEPD2_1248c_IS_GxEPD2_1248c true
#define IS_GxEPD(c, x) (c##x)
#define IS_GxEPD2_BW(x) IS_GxEPD(GxEPD2_BW_IS_, x)
#define IS_GxEPD2_3C(x) IS_GxEPD(GxEPD2_3C_IS_, x)
#define IS_GxEPD2_7C(x) IS_GxEPD(GxEPD2_7C_IS_, x)
#define IS_GxEPD2_1248(x) IS_GxEPD(GxEPD2_1248_IS_, x)
#define IS_GxEPD2_1248c(x) IS_GxEPD(GxEPD2_1248c_IS_, x)

#include "GxEPD2_selection_check.h"

#if defined(ESP32)
#define MAX_DISPLAY_BUFFER_SIZE 65536ul // e.g.
#if IS_GxEPD2_BW(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
#elif IS_GxEPD2_3C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#elif IS_GxEPD2_7C(GxEPD2_DISPLAY_CLASS)
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
#endif
// adapt the constructor parameters to your wiring
#if !IS_GxEPD2_1248(GxEPD2_DRIVER_CLASS) && !IS_GxEPD2_1248c(GxEPD2_DRIVER_CLASS)
#if defined(ARDUINO_LOLIN_D32_PRO)
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 0, /*RST=*/ 2, /*BUSY=*/ 15)); // my LOLIN_D32_PRO proto board
#else
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // my suggested wiring and proto board
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // LILYGO_T5_V2.4.1
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 19, /*RST=*/ 4, /*BUSY=*/ 34)); // LILYGO® TTGO T5 2.66
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 2, /*RST=*/ 0, /*BUSY=*/ 4)); // e.g. TTGO T8 ESP32-WROVER
#endif
#else // GxEPD2_1248 or GxEPD2_1248c
// Waveshare 12.48 b/w or b/w/r SPI display board and frame or Good Display 12.48 b/w panel GDEW1248T3 or b/w/r panel GDEY1248Z51
// general constructor for use with all parameters, e.g. for Waveshare ESP32 driver board mounted on connection board
GxEPD2_DISPLAY_CLASS < GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS) > display(GxEPD2_DRIVER_CLASS(/*sck=*/ 13, /*miso=*/ 12, /*mosi=*/ 14,
    /*cs_m1=*/ 23, /*cs_s1=*/ 22, /*cs_m2=*/ 16, /*cs_s2=*/ 19,
    /*dc1=*/ 25, /*dc2=*/ 17, /*rst1=*/ 33, /*rst2=*/ 5,
    /*busy_m1=*/ 32, /*busy_s1=*/ 26, /*busy_m2=*/ 18, /*busy_s2=*/ 4));
#endif
#undef MAX_DISPLAY_BUFFER_SIZE
#undef MAX_HEIGHT
#endif

#endif

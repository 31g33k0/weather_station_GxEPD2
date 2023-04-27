#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>

#include <Fonts/FreeMonoBold9pt7b.h>
//#include "./GxEPD2_display_selection_new_style.h"
#include "GxEPD2_display_selection.h"
#include "./GxEPD2_selection_added.h"

#include "time.h"
#include "sntp.h"

#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
#define GxEPD2_DRIVER_CLASS GxEPD2_154_M10 // GDEW0154M10 152x152, UC8151D
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "551D_gl0up1_p01n7";
const char* password = "W1f1_4cc355_p01n7";
const int httpPort  = 80;
const int httpsPort = 443;
const char* fp_api_github_com = "df b2 29 c6 a6 38 1a 59 9d c9 ad 92 2d 26 f5 3c 83 8f a5 87"; // as of 25.11.2020
const char* fp_github_com     = "5f 3f 7a c2 56 9f 50 a4 66 76 47 c6 a1 8c a0 07 aa ed bb 8e"; // as of 25.11.2020
//const char* fp_rawcontent     = "70 94 de dd e6 c4 69 48 3a 92 70 a1 48 56 78 2d 18 64 e0 b7"; // as of 25.11.2020
const char* fp_rawcontent     = "8F 0E 79 24 71 C5 A7 D2 A7 46 76 30 C1 3C B7 2A 13 B0 01 B2"; // as of 29.7.2022

String openWeatherMapApiKey = "getyourtokenonthewebsite";

String city = "Berlin";
String countryCode = "DE";

unsigned long lastTime = 0;
unsigned long timerDelay = 60000;

String jsonBuffer;

// SS is usually used for CS. define here for easy change
/*
#ifndef EPD_CS
#define EPD_CS SS
#endif
*/

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
//GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // my suggested wiring and proto board
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
#endif
//

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)

//
  uint16_t x_cursor= 0;
  uint16_t y_cursor=12;

//

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("No time available (yet)");
    y_cursor = 144;
    display.setCursor(x_cursor, y_cursor);
    display.print("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
/*  display.setCursor(0,0);
  display.print(&timeinfo, "%A,%d %B");
*/
  display.setCursor(x_cursor, y_cursor);
  display.print(&timeinfo,"%H:%M:%S" );
}
void timeavailable(struct timeval *t)
{
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void get_weater() {
  //get weather data from openweathermap.org
  //https://openweathermap.org/current
  //https://openweathermap.org/weather-conditions
  //https://openweathermap.org/weather-data
   
/*    else {
      Serial.println("WiFi Disconnected");
    }
*/


}

void display_weather() {
//put the next lines in get_weather fcn
  String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

  display.setRotation(3);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds("display_weather", 0, 0, &tbx, &tby, &tbw, &tbh);



// end of "put the next lines"

  // center the bounding box by transposition of the origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x_cursor, y_cursor);

//      display.print("JSON object = ");
//      display.print(myObject);
      display.print("Temp : ");
      display.print(myObject["main"]["temp"]);
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      display.print("Druck :  ");
      display.println(myObject["main"]["pressure"]);
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      display.print("Feuchtigk: ");
      display.println(myObject["main"]["humidity"]);
//      display.print(" %");
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      display.print("Luft:");
      display.print(myObject["wind"]["speed"]);
      display.print("km/h");
            
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      display.print(myObject["weather"][0]["main"]);
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      display.print(myObject["weather"][0]["description"]);
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      //display.print(myObject["name"]);
      display.print(city);
      /*
      time_t t = time(NULL);
      struct tm res;
      localtime_r(&t, &res);
      */
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      printLocalTime();
    //weather=get_weather();
    //display.print(weather);
  }
  while (display.nextPage());

}

void setup() {
  Serial.begin(115200);
  display.init(115200);
  WiFi.begin(ssid, password);

  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Timer set to ");
  Serial.print(timerDelay/1000);
  Serial.print(" seconds (timerDelay variable)");
  sntp_set_time_sync_notification_cb( timeavailable );
  sntp_servermode_dhcp(1); 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  display_weather();
  printLocalTime();
  // 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}

void loop() {
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
      //

  do {
    display.fillScreen(GxEPD_WHITE);
y_cursor = 12 ;
    display.setCursor(x_cursor, y_cursor);

//      display.print("JSON object = ");
//      display.print(myObject);
      display.print("Temp : ");
      display.print(myObject["main"]["temp"]);
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      display.print("Druck :  ");
      display.println(myObject["main"]["pressure"]);
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      display.print("Feuchtigk: ");
      display.println(myObject["main"]["humidity"]);
//      display.print(" %");
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      display.print("Luft:");
      display.print(myObject["wind"]["speed"]);
      display.print("km/h");
            
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      display.print(myObject["weather"][0]["main"]);
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      display.print(myObject["weather"][0]["description"]);
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      //display.print(myObject["name"]);
      display.print(city);
      /*
      time_t t = time(NULL);
      struct tm res;
      localtime_r(&t, &res);
      */
      y_cursor= 18+y_cursor;
      display.setCursor(x_cursor, y_cursor);
      printLocalTime();
    //weather=get_weather();
    //display.print(weather);
      //
      }
  while (display.nextPage());


    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

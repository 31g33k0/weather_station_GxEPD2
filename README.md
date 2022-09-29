# weather_station_GxEPD2

## First of all get a token here :  
https://openweathermap.org   

modify the request if you are not in Berlin ( change DE too if you are not in germany )

## includes 
   

GxEPD2

time  
sntp

###### TODO maybe too much wifi libs - check it
WiFi  
HTTPClient  
Arduino_JSON  
WiFiClient  
WiFiClientSecure

### stuff :

Goodisplay DESPI-CO2 display adapter -- resistor button position left  
Pinout :
adapter - ESP32 DOIT Devkit V1  

Busy - 04  
RST - 16 [RX2]  
D/C - 17 [TX2]  
CS - 5 [SS]  
SCK - 18 [SCK]  
SDI - 23 [MOSI]  
GND - GND  
3.3 V - 3.3 V

I used the GxEPD2_DRIVER_CLASS GxEPD2_154_M10 // GDEW0154M10 152x152, UC8151D. change it if you own another screen.

# description
 
* Connect to wifi (insert your credentials first)  
---
begin of main after ending the first pass in setup
-

* get a json from weather site and time from sntp (/or from dhcp server ? seems to not work TODO check)
* display stuff on screen  
1. temp
2. air pressure
3. moisture
4. air speed
5. main
6. description
7. city
8. time
----
end of setup
-
* wait 60000 ms ( 1 minute)
* refresh and restart the main loop
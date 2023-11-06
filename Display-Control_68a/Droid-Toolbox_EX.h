// Droid Toolbox Extended
#include "Wire.h"   // for I2C communication
#include <WiFi.h>   // used to set time with UTC
#include "time.h"

// Adafruit Temprature & Humidity Sensor
#include "Adafruit_Si7021.h"

// Adafruit LIS3DH Triple-Axis Accelerometer
#include <Adafruit_LIS3DH.h>

// Real Time Clock
#include "RTClib.h"

// added for Temp/Humid Sensor
#include <DHT11.h>

/* Clock Colors
 * A few colour codes:
 * Default color definitions
 * TFT_BLACK       0x0000      [   0,   0,   0 ]
 * TFT_NAVY        0x000F      [   0,   0, 128 ]
 * TFT_DARKGREEN   0x03E0      [   0, 128,   0 ]
 * TFT_DARKCYAN    0x03EF      [   0, 128, 128 ]
 * TFT_MAROON      0x7800      [ 128,   0,   0 ]
 * TFT_PURPLE      0x780F      [ 128,   0, 128 ]
 * TFT_OLIVE       0x7BE0      [ 128, 128,   0 ]
 * TFT_LIGHTGREY   0xD69A      [ 211, 211, 211 ]
 * TFT_DARKGREY    0x7BEF      [ 128, 128, 128 ]
 * TFT_BLUE        0x001F      [   0,   0, 255 ]
 * TFT_GREEN       0x07E0      [   0, 255,   0 ]
 * TFT_CYAN        0x07FF      [   0, 255, 255 ]
 * TFT_RED         0xF800      [ 255,   0,   0 ]
 * TFT_MAGENTA     0xF81F      [ 255,   0, 255 ]
 * TFT_YELLOW      0xFFE0      [ 255, 255,   0 ]
 * TFT_WHITE       0xFFFF      [ 255, 255, 255 ]
 * TFT_ORANGE      0xFDA0      [ 255, 180,   0 ]
 * TFT_GREENYELLOW 0xB7E0      [ 180, 255,   0 ]
 * TFT_PINK        0xFE19      [ 255, 192, 203 ] Lighter pink, was 0xFC9F
 * TFT_BROWN       0x9A60      [ 150,  75,   0 ]
 * TFT_GOLD        0xFEA0      [ 255, 215,   0 ]
 * TFT_SILVER      0xC618      [ 192, 192, 192 ]
 * TFT_SKYBLUE     0x867D      [ 135, 206, 235 ]
 * TFT_VIOLET      0x915C      [ 180,  46, 226 ]
*/

// Define Custom colors
#define TFT_GREY            0x39C4

// Set Clock Colors
#define CLK_FOREGROUND      TFT_ORANGE
#define CLK_BACKGROUND      TFT_BLACK
#define CLK_COLONSEC        TFT_GREY

#define DATE_TEXT_SIZE      DEFAULT_TEXT_SIZE
#define ENVIRO_TEXT_SIZE    DEFAULT_TEXT_SIZE
#define TIME_TEXT_SIZE      DEFAULT_TEXT_SIZE

// Custom Button Define
#define BUTTON_UP_PIN       18
#define BUTTON_DOWN_PIN     17
#define BUTTON_SELECT_PIN   21
#define BUTTON_BACK_PIN     16

//Temp/Humid declarations
// Create an instance of the DHT11 class and set the digital I/O pin.
DHT11 dht11(13);

// RTC Values
// Which RTC is Connected  Uncomment the RTC Board connected to IC2
//#define PCF8523  // Prototype Board
#define DS1307   // Sith Bracer

#ifdef PCF8523
  RTC_PCF8523 rtc;
#endif

#ifdef DS1307
  RTC_DS1307 rtc;
#endif

byte omm = 99, oss = 99, ohh = 99, pss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char spcdaysOfTheWeek[7][12] = {"Sunday  ", "Monday  ", "Tuesday ", "Wednesday", "Thursday ", "Friday ", "Saturday "};   // padding added so all days are same size

// Setting for using Wifi to set RTC
// Wifi Network Settings
// Mobile Connect (iPhone)
const char* ssid_m     = "R2-EK0";
const char* password_m = "onetruedarth";

// Home Connect (Darth's LAN)
const char* ssid_h     = "mustafar";
const char* password_h = "V@d3rWasFram3d";

// NTP Time Server Settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -6*60*60;
const int   daylightOffset_sec = 3600;

// Temp & Humidity Sensor Values
Adafruit_Si7021 sensor = Adafruit_Si7021();

// I2C Adafruit LIS3DH Triple-Axis Accelerometer
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Calibration settings to adjuxt for sensor variants
#define TEMPF_OFFSET  -1.3
#define HUMID_OFFSET  -2

// Declare variables to hold Temprature and Humidity
float humidity, temp_c, temp_f;

// Helped with Battery not powering Display
#define PIN_POWER_ON                 15

const char msg_clock_enviro[]                = "CLOCK";
const char msg_temp[]                        = "Temprature:     °F";
const char msg_humid[]                       = "% Humidity";
const char msg_set_time[]                    = "SET TIME (WIFI)";
const char msg_ssid_m[]                      = "R2-EKO (iPhone)";
const char msg_ssid_h[]                      = "MUSTAFAR (Home)";

const menu_item_t top_menu[] = {
  { SCANNER_SCANNING,     msg_scanner      },
  { BEACON_TYPE_MENU,     msg_beacon       },
  { CLOCK_ENVIRO,         msg_clock_enviro },
  { SET_TIME,             msg_set_time     }, 
  { ABOUT,                msg_about        },
};

const menu_item_t set_menu[] = {
  { MOBILE_WIFI,          msg_ssid_m    },
  { HOME_WIFI,            msg_ssid_h    },
};

// Function Declaration from Droid-Toolbox Main Application
void set_location_beacon(uint8_t location);
void set_droid_beacon(uint8_t personality);
void init_advertisement_data(const char* name);
void set_payload_from_beacon();
void droid_disconnect();
void set_random_beacon();
void init_advertisement_data(const char* name);
void display_captioned_menu(const char* caption, uint8_t list_index);
void dtb_draw_string(const char* str, int32_t draw_x, int32_t draw_y, uint32_t draw_width, uint16_t text_size, uint16_t text_color, uint8_t text_datum);
void set_ofr_alignment_by_datum(uint8_t d);

// Functions
void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

void updateBL(){
  lis.read();      // get X Y and Z data at once

  if((lis.z > 10000) || (lis.z < -10000)) {
      if(!digitalRead(38)){          // Read current Backlight state
        digitalWrite(38, HIGH);     // Turn on Backlight
      }
  } else {
      if(digitalRead(38)){          // Read current Backlight state
        digitalWrite(38, LOW);      // Turn off Backlight
      }
  }
}

void wifi_setTime_h() {
  struct tm timeinfo;

  int year = timeinfo.tm_year;
  int rotation = 0;
  char rotChar[2];
  char timeYear[5];
  char timeMonth[3];
  char timeDay[3];
  char timeHour[3];
  char timeMinute[3];
  char timeSecond[3];

  uint16_t y = 0, c;
  char strLine[MSG_LEN_MAX];

  // Connect to Wi-Fi
  snprintf(strLine, MSG_LEN_MAX, "Connecting to %s", ssid_h);
  tft.setTextSize(1);
  y = (tft.height() - (tft.fontHeight() * ((DATE_TEXT_SIZE * 9) + 1))) / 2;
  tft.setCursor(0, y);

  dtb_draw_string(strLine, tft.getViewportWidth()/2, y, tft.getViewportWidth(), DATE_TEXT_SIZE + 1, CLK_FOREGROUND, TC_DATUM);
  WiFi.begin(ssid_h, password_h);
  y = (tft.height() - (tft.fontHeight() * ((DATE_TEXT_SIZE * 7) + 1))) / 2;
  while (WiFi.status() != WL_CONNECTED) {
    switch (rotation) {
      case 0:
        snprintf(strLine, MSG_LEN_MAX,"/");
        rotation = 1;
        break;

      case 1:
        snprintf(strLine, MSG_LEN_MAX,"-");
        rotation = 2;
        break;
      
      case 2:
        snprintf(strLine, MSG_LEN_MAX,"\\");
        rotation = 0;
        break; 
    }
    delay(500);
    ofr.setBackgroundFillMethod(BgFillMethod::Minimum);
    dtb_draw_string(strLine, tft.getViewportWidth()/2, y, tft.getViewportWidth(), DATE_TEXT_SIZE + 1, TFT_YELLOW, TC_DATUM);
  }

  dtb_draw_string("WiFi connected.", tft.getViewportWidth()/2, y, tft.getViewportWidth(), DATE_TEXT_SIZE + 1, TFT_YELLOW, TC_DATUM);
  ofr.setBackgroundFillMethod(BgFillMethod::None);
  
  // Init and get the time
  y = (tft.height() - (tft.fontHeight() * ((DATE_TEXT_SIZE * 5) + 1))) / 2;
  
  snprintf(strLine, MSG_LEN_MAX, "Get Time/Date from %s", ntpServer);
  
  dtb_draw_string(strLine, tft.getViewportWidth()/2, y, tft.getViewportWidth(), DATE_TEXT_SIZE + 1, TFT_YELLOW, TC_DATUM);
 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //printLocalTime();

  getLocalTime(&timeinfo);

  strftime(timeYear,5, "%Y", &timeinfo);
  strftime(timeMonth,3, "%m", &timeinfo);
  strftime(timeDay,3, "%d", &timeinfo);
  strftime(timeHour,3, "%H", &timeinfo);
  strftime(timeMinute,3, "%M", &timeinfo);
  strftime(timeSecond,3, "%S", &timeinfo);

  rtc.adjust(DateTime(atoi(timeYear), atoi(timeMonth), atoi(timeDay), atoi(timeHour), atoi(timeMinute), atoi(timeSecond)));

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void wifi_setTime_m() {
  struct tm timeinfo;

  int year = timeinfo.tm_year;
  char timeYear[5];
  char timeMonth[3];
  char timeDay[3];
  char timeHour[3];
  char timeMinute[3];
  char timeSecond[3];

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid_m);
  WiFi.begin(ssid_m, password_m);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  Serial.print("Get Time/Date from ");
  Serial.println(ntpServer);
 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  getLocalTime(&timeinfo);

  strftime(timeYear,5, "%Y", &timeinfo);
  // Serial.print(atoi(timeYear));
  // Serial.print(" ");
  strftime(timeMonth,3, "%m", &timeinfo);
  // Serial.print(atoi(timeMonth));
  // Serial.print(" ");
  strftime(timeDay,3, "%d", &timeinfo);
  // Serial.print(atoi(timeDay));
  // Serial.print(" ");
  strftime(timeHour,3, "%H", &timeinfo);
  // Serial.print(atoi(timeHour));
  // Serial.print(" ");
  strftime(timeMinute,3, "%M", &timeinfo);
  // Serial.print(atoi(timeMinute));
  // Serial.print(" ");
  strftime(timeSecond,3, "%S", &timeinfo);
  // Serial.print(atoi(timeSecond));
  // Serial.println();

  rtc.adjust(DateTime(atoi(timeYear), atoi(timeMonth), atoi(timeDay), atoi(timeHour), atoi(timeMinute), atoi(timeSecond)));

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void get_hum_temp() {
    // Read the humidity from the sensor.
    humidity = sensor.readHumidity() + HUMID_OFFSET;

    // Read the temperature from the sensor.
    temp_c = sensor.readTemperature();

    // Calculate temperature from C
    temp_f = (((temp_c*9)/5)+32) + TEMPF_OFFSET;
}

void draw_date() {
  // positioning of Date Line
  uint16_t y = 0, c;
  char strDate[MSG_LEN_MAX];
 
  DateTime now = rtc.now();

  if ((now.month() < 10) && (now.day() > 10)) snprintf(strDate, MSG_LEN_MAX, "%s 0%d/%d/%d", spcdaysOfTheWeek[now.dayOfTheWeek()], now.month(), now.day(), now.year());
  if ((now.month() > 10) && (now.day() < 10)) snprintf(strDate, MSG_LEN_MAX, "%s %d/0%d/%d", spcdaysOfTheWeek[now.dayOfTheWeek()], now.month(), now.day(), now.year());
  if ((now.month() < 10) && (now.day() < 10)) snprintf(strDate, MSG_LEN_MAX, "%s 0%d/0%d/%d", spcdaysOfTheWeek[now.dayOfTheWeek()], now.month(), now.day(), now.year());
  if ((now.month() > 10) && (now.day() > 10)) snprintf(strDate, MSG_LEN_MAX, "%s %d/%d/%d", spcdaysOfTheWeek[now.dayOfTheWeek()], now.month(), now.day(), now.year());

  // location the Y position to begin drawing to center vertically the text
  tft.setTextSize(1);
  y = (tft.height() - (tft.fontHeight() * ((DATE_TEXT_SIZE * 9) + 1))) / 2;
  tft.setCursor(0, y);

  // Date
  dtb_draw_string(strDate, tft.getViewportWidth()/2, y, tft.getViewportWidth(), DATE_TEXT_SIZE + 1, CLK_FOREGROUND, TC_DATUM);
}

void draw_enviro() {
  // Positioning of Temp & Humid
  uint16_t y = 0, c;
  uint16_t y1 = 0;
  uint16_t y2 = 0;

  char strTemp[MSG_LEN_MAX];
  char strHumd[MSG_LEN_MAX];

  tft.setTextSize(1);
  
  get_hum_temp();

  y = (tft.height() - (tft.fontHeight() * ((DATE_TEXT_SIZE * 8) + 1))) / 2;

  y1 = (tft.height() - (tft.fontHeight() * 3) - 5);
  y2 = (tft.height() - (tft.fontHeight() * 7) - 5);
  
  tft.setCursor(0, y);

  snprintf(strTemp, MSG_LEN_MAX, "Temprature: %.0fF", temp_f);
  snprintf(strHumd, MSG_LEN_MAX, "Humidity: %.0f%c", humidity, '%');

  dtb_draw_string(strTemp, tft.getViewportWidth()/2, y2, tft.getViewportWidth(), DATE_TEXT_SIZE + 1, CLK_FOREGROUND, TC_DATUM);
  dtb_draw_string(strHumd, tft.getViewportWidth()/2, y1, tft.getViewportWidth(), DATE_TEXT_SIZE, CLK_FOREGROUND, TC_DATUM);
}

void draw_time() {
  // Update digital time
  uint16_t y = 0, c, x = 0;
  char strTime[MSG_LEN_MAX];
  
  DateTime now = rtc.now();

  tft.setTextSize(1);
  y = (tft.height() - (tft.fontHeight() * ((DATE_TEXT_SIZE * 5) + 1))) / 2;
  tft.setCursor(0, y);
  
  snprintf(strTime, MSG_LEN_MAX, "%02.0f:%02.0f:%02.0f", (float)now.hour(), (float)now.minute(), (float)now.second());
  
  // Date
  dtb_draw_string(strTime, tft.getViewportWidth()/2, y, tft.getViewportWidth(), DATE_TEXT_SIZE + 5, CLK_FOREGROUND, TC_DATUM);
}

void clock_envio_disp() {
  DateTime now = rtc.now();

  if (pss != now.second()) 
  {
    pss = now.second();
    if (ohh != now.hour()) 
    {
      ohh = now.hour();
      draw_date();
    }
    if (omm != now.minute())
    {
      draw_enviro();
    }
    ofr.setBackgroundFillMethod(BgFillMethod::Minimum);
    draw_time();
    ofr.setBackgroundFillMethod(BgFillMethod::None);
  }
}

void list_init_ex() {
  uint8_t i;

  // populate TOP MENU list

  // calculate num_items and allocate memory for items
  lists[LIST_SET_TIME].num_items = sizeof(set_menu) / sizeof(menu_item_t);
  lists[LIST_SET_TIME].items = (const char**)malloc(sizeof(char*) * lists[LIST_SET_TIME].num_items);

  // populate items
  for (i=0; i<lists[LIST_SET_TIME].num_items; i++) {
    lists[LIST_SET_TIME].items[i] = set_menu[i].text;
  }

  // set render options
  lists[LIST_SET_TIME].render_options.text_size             = MENU_SELECT_TEXT_SIZE;
  lists[LIST_SET_TIME].render_options.text_color            = MENU_SELECT_TEXT_COLOR;
  lists[LIST_SET_TIME].render_options.text_padding          = MENU_SELECT_TEXT_PADDING;
  lists[LIST_SET_TIME].render_options.selected_text_color   = MENU_SELECT_SELECTED_TEXT_COLOR;
  lists[LIST_SET_TIME].render_options.selected_border_color = MENU_SELECT_SELECTED_BORDER_COLOR;
  lists[LIST_SET_TIME].render_options.ofr_font_size         = 0;
  lists[LIST_SET_TIME].render_options.ofr_font_height       = 0;
  lists[LIST_SET_TIME].render_options.row_width             = 0;
}

// void button1(button_press_t press_type);  // trying to use an enum as a parameter triggers a bug in arduino. adding an explicit prototype resolves the issue.
// void button1(button_press_t press_type) {

//   uint8_t i;

//   SERIAL_PRINTLN("Button 1 Press");

//   switch (state) {
//   // ** darth_update - UPDATE - start
//   // change SPLASH to ABOUT

//       // on splash screen
//       case ABOUT:

//   // ** darth_update - end

//       // only swap fonts if USE_OFR_FONTS is defined
//       #ifdef USE_OFR_FONTS

//         // a long button1 press will cause the font to change
//         if (press_type == LONG_PRESS) {
//             dtb_font++;
//             if (dtb_font > NUM_FONTS) {
//               dtb_font = 0;
//             }

//             // load the new font
//             ofr.unloadFont();
//             if (dtb_font != 0 && ofr.loadFont(dtb_fonts[dtb_font - 1].data, dtb_fonts[dtb_font - 1].size)) {
//               dtb_font = 0;
//             }

//             // recalculate font properties based on new font
//             list_calculate_dynamic_font_properties();

//         // otherwise go to the top menu
//         } else {
//       #endif

//         state = TOP_MENU;
//         selected_item = 0;

//       #ifdef USE_OFR_FONTS
//         }
//       #endif

//       tft_update = true;
//       break;

//     case TOP_MENU:
//       state = top_menu[selected_item].state;
//       selected_item = 0;
//       tft_update = true;
//       break;

//     case BEACON_TYPE_MENU:
//       state = beacon_type_menu[selected_item].state;

//       if (state == BEACON_RANDOM) {
//         set_random_beacon();

//         // the following if/else block is setting selected_item to the value of the randomly selected beacon
//         //
//         // in its respective beacon list. this is being done so when you go back (long button 2 press)
//         // you'll be brought to the element in the beacon list where the randomly selected beacon is
//         if (beacon.type == DROID) {
//           for(i=0; i<emulatable_personalities_size; i++) {
//             if (emulatable_personalities[i]->id == beacon.setting[BEACON_PARAM_DROID_ID]) {
//               selected_item = i;
//               break;
//             }
//           }
//         } else {
//           for(i=0; i<LOCATIONS_SIZE; i++) {
//             if (locations[i].id == beacon.setting[BEACON_PARAM_LCTN_ID]) {
//               selected_item = i;
//               break;
//             }
//           }
//         }
//         state = BEACON_ACTIVATE;

//       // initiate a rotating beacon
//       } else if (state == BEACON_ROTATING) {
//         state = BEACON_SET_INTERVAL;
//         selected_item = 0;

//       } else if (state == BEACON_EXPERT) {

//         // let whatever is currently in beacon be displayed on the expert screen.
//         // let's users see what the stock beacons look like. if they figure out this little trick of set a beacon, then back out to expert mode.
//         //set_random_beacon();

//         selected_item = 0;
//       } else {
//         selected_item = 0;
//       }
//       tft_update = true;
//       break;

//     case BEACON_SET_INTERVAL:
//       if (selected_item == 0) {
//         if (beacon_rotate_interval >= MAX_BEACON_CHANGE_INTERVAL) {
//           beacon_rotate_interval = 6;
//         } else {
//           if (press_type == SHORT_PRESS) {
//             beacon_rotate_interval += SHORT_PRESS_INTERVAL_INC;
//           } else {
//             beacon_rotate_interval += LONG_PRESS_INTERVAL_INC;
//           }
//           if (beacon_rotate_interval > MAX_BEACON_CHANGE_INTERVAL) {
//             beacon_rotate_interval = MAX_BEACON_CHANGE_INTERVAL;
//           }
//         }
//       } else {
//         set_rotating_beacon();
//         next_beacon_time = 1;
//         state = BEACON_ACTIVATE;
//         selected_item = 0;
//         button1(SHORT_PRESS);
//       }
//       tft_update = true;
//       break;

//     case BEACON_LOCATION_LIST:
//       set_location_beacon(locations[selected_item].id);
//       state = BEACON_ACTIVATE;
//       tft_update = true;
//       break;

//     case BEACON_DROID_LIST:
//       set_droid_beacon(emulatable_personalities[selected_item]->id);
//       state = BEACON_ACTIVATE;
//       tft_update = true;
//       break;

//     case BEACON_ACTIVATE:
//       if (next_beacon_time != 0) {
//         next_beacon_time = millis() + (beacon_rotate_interval * 10000); 
//       }
//       init_advertisement_data(nullptr);
//       set_payload_from_beacon();
//       pAdvertising->start();
//       state = BEACON_ACTIVE;
//       tft_update = true;
//       break;

//     case BEACON_ACTIVE:
//       pAdvertising->stop();
//       state = BEACON_ACTIVATE;
//       tft_update = true;
//       break;

//     case BEACON_EXPERT:

//       // change beacon type; (re)initialize the beacon object
//       if (selected_item == 0) {

//         if (beacon.type == DROID) {
//           beacon.type = LOCATION;
//           beacon.setting[BEACON_PARAM_LCTN_ID]      = 1;  // default location
//           beacon.setting[BEACON_PARAM_LCTN_REACT]   = 2;  // default interval
//           beacon.setting[BEACON_PARAM_LCTN_RSSI]    = 38; // default minimim rssi
//           beacon.setting[BEACON_PARAM_LCTN_PARAM4]  = 0;  // unused
//         } else {
//           beacon.type = DROID;
//           beacon.setting[BEACON_PARAM_DROID_ID]     = 1; // default personality
//           beacon.setting[BEACON_PARAM_DROID_AFFL]   = 1; // default affiliation
//           beacon.setting[BEACON_PARAM_DROID_PAIRED] = 1; // default paired
//           beacon.setting[BEACON_PARAM_DROID_BDROID] = 0; // be a droid
//         }

//       // change beacon parameters
//       } else if (selected_item-1 < NUM_BEACON_PARAMS) {

//         // store the current beacon parameter index based on current selected_item value
//         i = selected_item - 1;

//         // on a long press, reset the beacon setting to 0
//         if (press_type == LONG_PRESS) {
//           beacon.setting[i] = 0;

//         // otherwise, increment the parameter value by 1
//         } else {
//           beacon.setting[i]++;
//         }

//         // adjust parameter values as needed for a DROID beacon
//         if (beacon.type == DROID) {
//           switch(i) {

//             // reset affiliation if affiliation goes above 0x3F
//             case BEACON_PARAM_DROID_AFFL:
//               if (beacon.setting[i] > 0x3F) {
//                 beacon.setting[i] = 0;
//               }
//               break;

//             // these are yes/no parameters, keep their values as either 0 or 1
//             case BEACON_PARAM_DROID_BDROID:
//             case BEACON_PARAM_DROID_PAIRED:
//               if (beacon.setting[i] > 1) {
//                 beacon.setting[i] = 0;
//               }
//               break;
//           }

//         // adjust parameter values as needed for a LOCATION beacon
//         } else {
//           switch(i) {

//             // reset location if it goes above 7
//             case BEACON_PARAM_LCTN_ID:
//               if (beacon.setting[i] > 7) {
//                 beacon.setting[i] = 0;
//               }
//               break;

//             // reset minimium rssi value if it goes outside of limits
//             case BEACON_PARAM_LCTN_RSSI:

//               // -20dBm is about 1ft from the droid and I think a reasonable lower limit
//               // -80dBm is very weak and I think a reasonable upper limit
//               // but you can change these limits if you want
//               if (beacon.setting[i] < 20 || beacon.setting[i] > 80) {
//                 beacon.setting[i] = 20;
//               }
//               break;
//           }
//         }

//       // if we get to here then we're on the activate function
//       } else {

//         // if a droid beacon with the be a droid parameter enabled
//         if (beacon.type == DROID && beacon.setting[BEACON_PARAM_DROID_BDROID] != 0) {

//           // create a beacon with the name DROID so as to appear as a droid to other toolboxes
//           init_advertisement_data(ble_adv_name_droid);

//         // otherwise initialize beacon as normal
//         } else {
//           init_advertisement_data(nullptr);
//         }

//         // set the payload based on the beacon data
//         set_payload_from_beacon();

//         // start advertising the beacon
//         pAdvertising->start();

//         // change state to active beacon
//         state = BEACON_EXPERT_ACTIVE;
//       }

//       // set flag to update the display
//       tft_update = true;
//       break;

//     case BEACON_EXPERT_ACTIVE:
//       pAdvertising->stop();
//       state = BEACON_EXPERT;
//       tft_update = true;
//       break;

//     case SCANNER_RESULTS:
//       if (droid_count > 0) {
//         if (press_type == LONG_PRESS) {
//           state = SCANNER_CONNECTING;
//           tft_update = true;
//         } else if (droid_count > 1) {
//           current_droid = (current_droid + 1) % droid_count;
//           tft_update = true;
//         }
//       } else {
//         state = SCANNER_SCANNING;
//         tft_update = true;
//       }
//       break;

//     case CONNECTED_MENU:
//       state = connected_menu[selected_item].state;
//       tft_update = true;
//       break;

//     case SOUND_GROUP:
//       if (press_type == SHORT_PRESS) {
//         current_group = (current_group + 1) % 12;
//         current_track = 0;
//       } else {
//         state = SOUND_TRACK;
//       }
//       tft_update = true;
//       break;

//     case SOUND_TRACK:
//       if (press_type == SHORT_PRESS) {
//         current_track = (current_track + 1) % 99;
//       } else {
//         state = SOUND_PLAY;
//       }
//       tft_update = true;
//       break;

//     case SOUND_PLAY:
//       SERIAL_PRINTLN("Play selected!");
//       state = SOUND_PLAYING;
//       tft_update = true;
//       break;

//     case VOLUME_UP:
//       if (droid_volume > 90) {
//         droid_volume = 100;
//       } else {
//         droid_volume += 10;
//       }
//       tft_update = true;
//       break;

//     case VOLUME_DOWN:
//       if (droid_volume < 10) {
//         droid_volume = 0;
//       } else {
//         droid_volume -= 10;
//       }
//       tft_update = true;
//       break;

//     case VOLUME_TEST:
//       state = VOLUME_TESTING;
//       tft_update = true;
//       break;
//   }
// }

// void button2(button_press_t press_type);  // trying to use an enum as a parameter triggers a bug in arduino. adding an explicit prototype resolves the issue.
// void button2(button_press_t press_type) {

//   uint8_t i;

//   SERIAL_PRINTLN("Button 2 Press");

//   // do button 2 stuff
//   switch (state) {
//   // ** darth_update - UPDATE - start    
//   // Change SPLASH to ABOUT

//       case ABOUT:
//         state = TOP_MENU;
//         selected_item = 0;
//         tft_update = true;
//         break;

//   // ** darth_update - end

//     case TOP_MENU:
//       if (press_type == LONG_PRESS) {
//         state = ABOUT;
//         selected_item = 0;
//       } else {
//         selected_item++;
//         if (selected_item >= sizeof(top_menu) / sizeof(menu_item_t)) {
//           selected_item = 0;
//         }
//       }
//       tft_update = true;
//       break;

//     case BEACON_TYPE_MENU:
//       if (press_type == LONG_PRESS) {
//         state = TOP_MENU;
//         selected_item = 1;  // beacon
//       } else {
//         selected_item++;
//         if (selected_item >= sizeof(beacon_type_menu) / sizeof(menu_item_t)) {
//           selected_item = 0;
//         }
//       }
//       tft_update = true;
//       break;

//     case BEACON_SET_INTERVAL:

//       // if a back menu button press...
//       if (press_type == LONG_PRESS) {

//         // find the location of rotating beacon in the beacon type menu
//         for (selected_item=0; selected_item < (sizeof(beacon_type_menu) / sizeof(menu_item_t)); selected_item++) {
//           if (beacon_type_menu[selected_item].state == BEACON_ROTATING) {
//             break;
//           }
//         }

//         // if rotating beacon type wasn't found in the menu then just set selected_item to the first item in the list
//         if (selected_item >= (sizeof(beacon_type_menu) / sizeof(menu_item_t))) {
//           next_beacon_time = 0;
//           selected_item = 0;
//         }
//         state = BEACON_TYPE_MENU;

//       } else {
//         selected_item = (selected_item+1) % 2;
//       }
//       tft_update = true;
//       break;

//     case BEACON_LOCATION_LIST:
//       if (press_type == LONG_PRESS) {
//         state = BEACON_TYPE_MENU;
//         selected_item = 0;
//       } else {
//         selected_item++;
//         if (selected_item >= LOCATIONS_SIZE) {
//           selected_item = 0;
//         }
//       }
//       tft_update = true;
//       break;

//     case BEACON_DROID_LIST:
//       if (press_type == LONG_PRESS) {
//         state = BEACON_TYPE_MENU;
//         selected_item = 1;
//       } else {
//         selected_item++;
//         if (selected_item >= emulatable_personalities_size) {
//           selected_item = 0;
//         }
//       }
//       tft_update = true;
//       break;

//     case BEACON_ACTIVE:
//     case BEACON_ACTIVATE:

//       // stop advertising the active beacon
//       if (state == BEACON_ACTIVE) {
//         pAdvertising->stop();
//       }

//       // if a back menu button press...
//       if (press_type == LONG_PRESS) {

//         // if exiting a rotating beacon
//         if (next_beacon_time != 0) {
//           state = BEACON_SET_INTERVAL;
//           next_beacon_time = 0;
//           selected_item = 1;
//         } else if (beacon.type == DROID) {
//           state = BEACON_DROID_LIST;
//         } else {
//           state = BEACON_LOCATION_LIST;
//         }

//       // change state to an inactive beacon
//       } else if (state == BEACON_ACTIVE) {
//         state = BEACON_ACTIVATE;

//       // activate the beacon
//       } else {
//         if (next_beacon_time != 0) {
//           next_beacon_time = millis() + (beacon_rotate_interval * 10000); 
//         }
//         init_advertisement_data(nullptr);
//         set_payload_from_beacon();
//         pAdvertising->start();
//         state = BEACON_ACTIVE;
//       }
//       tft_update = true;
//       break;

//     case BEACON_EXPERT:
//       if (press_type == LONG_PRESS) {
//         state = BEACON_TYPE_MENU;
//         selected_item = 3;
//       } else {
//         selected_item++;

//         // skip expert beacon parameters that have no label
//         i = selected_item - 1;
//         if (i < NUM_BEACON_PARAMS && ((beacon.type == DROID) ? msg_beacon_droid_param[i] : msg_beacon_location_param[i]) == nullptr) {
//           selected_item++;
//         }

//         // return to the first beacon parameter after reaching the end
//         if (selected_item >= NUM_BEACON_PARAMS+2) {
//           selected_item = 0;
//         }
//       }
//       tft_update = true;
//       break;

//     case BEACON_EXPERT_ACTIVE:
//       pAdvertising->stop();
//       if (press_type == LONG_PRESS) {
//         state = BEACON_TYPE_MENU;
//         selected_item = 3;
//       } else {
//         state = BEACON_EXPERT;
//       }
//       tft_update = true;
//       break;

//     case SCANNER_RESULTS:
//       if (press_type == LONG_PRESS || droid_count < 1) {
//         state = TOP_MENU;
//         selected_item = 0;    // scanner
//         tft_update = true;
//       } else {
//         if (droid_count > 1) {
//           if (current_droid == 0) {
//             current_droid = droid_count - 1;
//           } else {
//             current_droid = (current_droid - 1) % droid_count;
//           }
//           tft_update = true;
//         }
//       }
//       break;

//     case CONNECTED_MENU:
//       if (press_type == SHORT_PRESS) {
//         selected_item++;
//         if (selected_item >= sizeof(connected_menu) / sizeof(menu_item_t)) {
//           selected_item = 0;
//         }
//       } else {
//         droid_disconnect();
//         state = SCANNER_RESULTS;
//       }
//       tft_update = true;
//       break;

//     case SOUND_GROUP:
//     case SOUND_TRACK:
//     case SOUND_PLAY:
//       if (press_type == SHORT_PRESS) {
//         switch (state) {
//           case SOUND_GROUP:
//             state = SOUND_TRACK;
//             break;
//           case SOUND_TRACK:
//             state = SOUND_PLAY;
//             break;
//           case SOUND_PLAY:
//             state = SOUND_GROUP;
//             break;
//         }
//       } else {
//         state = CONNECTED_MENU;
//         selected_item = 0;          // todo: don't hardcode this
//       }
//       tft_update = true;
//       break;

//     case VOLUME_UP:
//     case VOLUME_DOWN:
//     case VOLUME_TEST:
//       if (press_type == SHORT_PRESS) {
//         switch(state) {
//           case VOLUME_UP:
//             state = VOLUME_DOWN;
//             break;
//           case VOLUME_DOWN:
//             state = VOLUME_TEST;
//             break;
//           case VOLUME_TEST:
//             state = VOLUME_UP;
//             break;
//         }
//       } else {
//         state = CONNECTED_MENU;
//         selected_item = 1;          // todo: don't hardcode this
//       }
//     tft_update = true;
//     break;
//   }
// }

void buttonup(button_press_t press_type);  // trying to use an enum as a parameter triggers a bug in arduino. adding an explicit prototype resolves the issue.
void buttonup(button_press_t press_type) {

  static uint32_t last_time_btn_up = 0;
  static uint32_t last_time_btn_up_down = 0;

  // do button Down stuff
  switch (state) {
    case ABOUT:
      state = TOP_MENU;
      selected_item = 0;
      tft_update = true;
      break;

    case TOP_MENU:
      selected_item--;        // What is the value of sizeof(top_menu) / sizeof(menu_item_t)
      if (selected_item >= sizeof(top_menu) / sizeof(menu_item_t)) {
        selected_item = sizeof(top_menu) / sizeof(menu_item_t)-1;
      }
      tft_update = true;
      break;

    case SET_TIME:
      selected_item--;
      if (selected_item >= sizeof(set_menu) / sizeof(menu_item_t)) {
        selected_item = sizeof(set_menu) / sizeof(menu_item_t)-1;
      }
      tft_update = true;
      break;


    case BEACON_TYPE_MENU:
      selected_item--;
      if (selected_item >= sizeof(beacon_type_menu) / sizeof(menu_item_t)) {
        selected_item = sizeof(beacon_type_menu) / sizeof(menu_item_t)-1;
      }
      tft_update = true;
      break;

    case BEACON_LOCATION_LIST:
      selected_item--;
      if (selected_item >= LOCATIONS_SIZE) {
        selected_item = LOCATIONS_SIZE-1;
      }
      tft_update = true;
      break;

    case BEACON_DROID_LIST:
      selected_item--;
      if (selected_item >= emulatable_personalities_size) {
        selected_item = emulatable_personalities_size-1;
      }
      tft_update = true;
      break;

    case BEACON_EXPERT:
      selected_item--;
      if (selected_item < 0) {
        selected_item = NUM_BEACON_PARAMS+2;
      }
      tft_update = true;
      break;

    case BEACON_EXPERT_ACTIVE:
      pAdvertising->stop();
      if (press_type == LONG_PRESS) {
        state = BEACON_TYPE_MENU;
        selected_item = 3;
      } else {
        state = BEACON_EXPERT;
      }
      tft_update = true;
      break;

    case SCANNER_RESULTS:
      if (press_type == LONG_PRESS || droid_count < 1) {
        state = TOP_MENU;
        selected_item = 0;    // scanner
        tft_update = true;
      } else {
        if (droid_count > 1) {
          if (current_droid == 0) {
            current_droid = droid_count - 1;
          } else {
            current_droid = (current_droid - 1) % droid_count;
          }
          tft_update = true;
        }
      }
      break;

    case CONNECTED_MENU:
      selected_item--;
      if (selected_item >= sizeof(connected_menu) / sizeof(menu_item_t)) {
        selected_item = sizeof(connected_menu) / sizeof(menu_item_t)-1;
      }
      tft_update = true;
      break;

    case SOUND_GROUP:
    case SOUND_TRACK:
    case SOUND_PLAY:
      if (press_type == SHORT_PRESS) {
        switch (state) {
          case SOUND_GROUP:
            state = SOUND_PLAY;
            break;
          case SOUND_TRACK:
            state = SOUND_GROUP;
            break;
          case SOUND_PLAY:
            state = SOUND_TRACK;
            break;
        }
      } else {
        state = CONNECTED_MENU;
        selected_item = 0;          // todo: don't hardcode this
      }
      tft_update = true;
      break;

    case VOLUME_UP:
    case VOLUME_DOWN:
    case VOLUME_TEST:
      if (press_type == SHORT_PRESS) {
        switch(state) {
          case VOLUME_UP:
            state = VOLUME_TEST;
            break;
          case VOLUME_DOWN:
            state = VOLUME_UP;
            break;
          case VOLUME_TEST:
            state = VOLUME_DOWN;
            break;
        }
      } else {
        state = CONNECTED_MENU;
        selected_item = 1;          // todo: don't hardcode this
      }
    tft_update = true;
    break;
  }
}

void buttondwn(button_press_t press_type);  // trying to use an enum as a parameter triggers a bug in arduino. adding an explicit prototype resolves the issue.
void buttondwn(button_press_t press_type) {

  static uint32_t last_time_btn_dwn = 0;
  static uint32_t last_time_btn_dwn_down = 0;

  // do button Down stuff
  switch (state) {
    case ABOUT:
      state = TOP_MENU;
      selected_item = 0;
      tft_update = true;
      break;

    case TOP_MENU:
      selected_item++;
      if (selected_item >= sizeof(top_menu) / sizeof(menu_item_t)) {
        selected_item = 0;
      }
      tft_update = true;
      break;

    case SET_TIME:
      selected_item++;
      if (selected_item >= sizeof(set_menu) / sizeof(menu_item_t)) {
        selected_item = 0;
      }
      tft_update = true;
      break;

    case BEACON_TYPE_MENU:
      selected_item++;
      if (selected_item >= sizeof(beacon_type_menu) / sizeof(menu_item_t)) {
        selected_item = 0;
      }
      tft_update = true;
      break;

    case BEACON_LOCATION_LIST:
      selected_item++;
      if (selected_item >= LOCATIONS_SIZE) {
        selected_item = 0;
      }
      tft_update = true;
      break;

    case BEACON_DROID_LIST:
      selected_item++;
      if (selected_item >= emulatable_personalities_size) {
        selected_item = 0;
      }
      tft_update = true;
      break;

    case BEACON_EXPERT:
      selected_item++;
      if (selected_item >= NUM_BEACON_PARAMS+2) {
        selected_item = 0;
      }
      tft_update = true;
      break;

    case BEACON_EXPERT_ACTIVE:
      pAdvertising->stop();
      if (press_type == LONG_PRESS) {
        state = BEACON_TYPE_MENU;
        selected_item = 3;
      } else {
        state = BEACON_EXPERT;
      }
      tft_update = true;
      break;

    case SCANNER_RESULTS:
      if (press_type == LONG_PRESS || droid_count < 1) {
        state = TOP_MENU;
        selected_item = 0;    // scanner
        tft_update = true;
      } else {
        if (droid_count > 1) {
          if (current_droid == 0) {
            current_droid = droid_count - 1;
          } else {
            current_droid = (current_droid - 1) % droid_count;
          }
          tft_update = true;
        }
      }
      break;

    case CONNECTED_MENU:
      selected_item++;
      if (selected_item >= sizeof(connected_menu) / sizeof(menu_item_t)) {
        selected_item = 0;
      }
      tft_update = true;
      break;

    case SOUND_GROUP:
    case SOUND_TRACK:
    case SOUND_PLAY:
      if (press_type == SHORT_PRESS) {
        switch (state) {
          case SOUND_GROUP:
            state = SOUND_TRACK;
            break;
          case SOUND_TRACK:
            state = SOUND_PLAY;
            break;
          case SOUND_PLAY:
            state = SOUND_GROUP;
            break;
        }
      } else {
        state = CONNECTED_MENU;
        selected_item = 0;          // todo: don't hardcode this
      }
      tft_update = true;
      break;

    case VOLUME_UP:
    case VOLUME_DOWN:
    case VOLUME_TEST:
      if (press_type == SHORT_PRESS) {
        switch(state) {
          case VOLUME_UP:
            state = VOLUME_DOWN;
            break;
          case VOLUME_DOWN:
            state = VOLUME_TEST;
            break;
          case VOLUME_TEST:
            state = VOLUME_UP;
            break;
        }
      } else {
        state = CONNECTED_MENU;
        selected_item = 1;          // todo: don't hardcode this
      }
    tft_update = true;
    break;
  }
}

void buttonsel(button_press_t press_type);  // trying to use an enum as a parameter triggers a bug in arduino. adding an explicit prototype resolves the issue.
void buttonsel(button_press_t press_type) {

  static uint32_t last_time_btn_sel = 0;
  static uint32_t last_time_btn_sel_down = 0;
  uint8_t i;

  switch (state) {
    case ABOUT:
      state = TOP_MENU;
      selected_item = 0;
      tft_update = true;
      break;

    case TOP_MENU:
      state = top_menu[selected_item].state;
      selected_item = 0;
      tft_update = true;
      break;

    case SET_TIME:
      // Add two new states to handle the set time functions.
      if (selected_item == 0) {
      //  wifi_setTime_m();
      } else {
      //  wifi_setTime_h();
      }
      break;
      
    case CLOCK_ENVIRO:
      break;

    case BEACON_TYPE_MENU:
      state = beacon_type_menu[selected_item].state;
      if (state == BEACON_RANDOM) {
        set_random_beacon();

        // the following if/else block is setting selected_item to the value of the randomly selected beacon
        // in its respective beacon list. this is being done so when you go back (long button 2 press)
        // you'll be brought to the element in the beacon list where the randomly selected beacon is
        if (beacon.type == DROID) {
          for(i=0; i<emulatable_personalities_size; i++) {
            if (emulatable_personalities[i]->id == beacon.setting[0]) {
              selected_item = i;
              break;
            }
          }
        } else {
          for(i=0; i<LOCATIONS_SIZE; i++) {
            if (locations[i].id == beacon.setting[0]) {
              selected_item = i;
              break;
            }
          }
        }
        state = BEACON_ACTIVATE;
      } else if (state == BEACON_EXPERT) {

        // let whatever is currently in beacon be displayed on the expert screen.
        // let's users see what the stock beacons look like. if they figure out this little trick of set a beacon, then back out to expert mode.
        //set_random_beacon();

        selected_item = 0;
      } else {
        selected_item = 0;
      }
      tft_update = true;
      break;

    case BEACON_LOCATION_LIST:
      set_location_beacon(locations[selected_item].id);
      state = BEACON_ACTIVATE;
      tft_update = true;
      break;

    case BEACON_DROID_LIST:
      set_droid_beacon(emulatable_personalities[selected_item]->id);
      state = BEACON_ACTIVATE;
      tft_update = true;
      break;

    case BEACON_ACTIVATE:
      init_advertisement_data(nullptr);
      set_payload_from_beacon();
      pAdvertising->start();
      state = BEACON_ACTIVE;
      tft_update = true;
      break;

    case BEACON_ACTIVE:
      pAdvertising->stop();
      state = BEACON_ACTIVATE;
      tft_update = true;
      break;

    case BEACON_EXPERT:
      // change beacon type
      if (selected_item == 0) {
        if (beacon.type == DROID) {
          beacon.type = LOCATION;
          beacon.setting[0] = 1;  // default location
          beacon.setting[1] = 2;  // default interval
          beacon.setting[2] = 38; // default minimim rssi
        } else {
          beacon.type = DROID;
          beacon.setting[0] = 1;  // default personality
          beacon.setting[1] = 1;  // default affiliation
          beacon.setting[2] = 1;  // default paired
        }

      // change beacon parameter 1
      } else if (selected_item == 1) {
         beacon.setting[0]++; 

        // droid:personality
        if (beacon.type == DROID) {
          if (press_type == LONG_PRESS) {
            beacon.setting[0] = 0;  // personality 0 doesn't exist...
          }

        // location:location
        } else {
          if (press_type == LONG_PRESS) {
            beacon.setting[0] = 0;  // location 0 doesn't exist...
          }
        }

      // change beacon parameter 2
      } else if (selected_item == 2) {
        beacon.setting[1]++;

        // droid:affiliation
        if (beacon.type == DROID) {
          if (press_type == LONG_PRESS || beacon.setting[1] > 0x3F) {
            beacon.setting[1] = 0;  // affiliation values beyond 0x3F break the formula
          }

        // location: reaction interval
        } else {
          if (press_type == LONG_PRESS || beacon.setting[1] > 0x33) {
            beacon.setting[1] = 0;  // can't have a reaction interval value greater than 0x33 (51) as it breaks the formula
          }
        }

      // change beacon parameter 3
      } else if (selected_item == 3) {
        beacon.setting[2]++;

        // droid:paired with remote?
        if (beacon.type == DROID) {
          if (beacon.setting[2] > 1) {    // swap betweeen paired (1) and not paired (0)
            beacon.setting[2] = 0;
          }

        // location:minimum rssi for a reaction
        } else {
          if (press_type == LONG_PRESS || beacon.setting[2] > 80) {
                                          // -80dBm is a pretty weak signal
            beacon.setting[2] = 20;       // from my experience an rssi of -28dBm is about 1 foot from the droid
                                          // is there any reason to go lower than -20dBm?
          }
        }

      // change beacon state (active or inactive)
      } else if (selected_item == 4) {
        init_advertisement_data(nullptr);
        set_payload_from_beacon();
        pAdvertising->start();
        state = BEACON_EXPERT_ACTIVE;
      }
      tft_update = true;
      break;

    case BEACON_EXPERT_ACTIVE:
      pAdvertising->stop();
      state = BEACON_EXPERT;
      tft_update = true;
      break;

    case SCANNER_RESULTS:
      if (droid_count > 0) {
        if (press_type == LONG_PRESS) {
          state = SCANNER_CONNECTING;
          tft_update = true;
        } else if (droid_count > 1) {
          current_droid = (current_droid + 1) % droid_count;
          tft_update = true;
        }
      } else {
        state = SCANNER_SCANNING;
        tft_update = true;
      }
      break;

    case CONNECTED_MENU:
      state = connected_menu[selected_item].state;
      tft_update = true;
      break;

    case SOUND_GROUP:
      if (press_type == SHORT_PRESS) {
        current_group = (current_group + 1) % 12;
        current_track = 0;
      } else {
        state = SOUND_TRACK;
      }
      tft_update = true;
      break;

    case SOUND_TRACK:
      if (press_type == SHORT_PRESS) {
        current_track = (current_track + 1) % 99;
      } else {
        state = SOUND_PLAY;
      }
      tft_update = true;
      break;

    case SOUND_PLAY:
      Serial.println("Play selected!");
      state = SOUND_PLAYING;
      tft_update = true;
      break;

    case VOLUME_UP:
      if (droid_volume > 90) {
        droid_volume = 100;
      } else {
        droid_volume += 10;
      }
      tft_update = true;
      break;

    case VOLUME_DOWN:
      if (droid_volume < 10) {
        droid_volume = 0;
      } else {
        droid_volume -= 10;
      }
      tft_update = true;
      break;

    case VOLUME_TEST:
      state = VOLUME_TESTING;
      tft_update = true;
      break;
  }
}

void buttonbk(button_press_t press_type);  // trying to use an enum as a parameter triggers a bug in arduino. adding an explicit prototype resolves the issue.
void buttonbk(button_press_t press_type) {

  static uint32_t last_time_btn_bk = 0;
  static uint32_t last_time_btn_bk_down = 0;

  // do button 2 stuff
  switch (state) {
    case ABOUT:
      state = TOP_MENU;
      selected_item = 4;
      tft_update = true;
      break;

    case TOP_MENU:
      state = TOP_MENU;
      selected_item = 0;
      tft_update = true;
      break;

    case SET_TIME:
      state = TOP_MENU;
      selected_item = 3;
      tft_update = true;
      break;

    case CLOCK_ENVIRO:
      state = TOP_MENU;
      selected_item = 2;
      ohh = 99;
      omm = 99;
      tft_update = true;
      break;

    case BEACON_TYPE_MENU:
      state = TOP_MENU;
      selected_item = 1;  // beacon
      tft_update = true;
      break;

    case BEACON_LOCATION_LIST:
      state = BEACON_TYPE_MENU;
      selected_item = 0;
      tft_update = true;
      break;

    case BEACON_DROID_LIST:
      state = BEACON_TYPE_MENU;
      selected_item = 1;
      tft_update = true;
      break;

    case BEACON_ACTIVATE:
      if (beacon.type == DROID) {
        state = BEACON_DROID_LIST;
      } else {
        state = BEACON_LOCATION_LIST;
      }
      tft_update = true;
      break;

    case BEACON_ACTIVE:
      pAdvertising->stop();
      if (beacon.type == DROID) {
        state = BEACON_DROID_LIST;
      } else {
        state = BEACON_LOCATION_LIST;
      }
      tft_update = true;
      break;

    case BEACON_EXPERT:
      state = BEACON_TYPE_MENU;
      selected_item = 3;
      tft_update = true;
      break;

    case BEACON_EXPERT_ACTIVE:
      pAdvertising->stop();
      state = BEACON_TYPE_MENU;
      selected_item = 3;
      tft_update = true;
      break;

    case SCANNER_RESULTS:
      if (press_type == SHORT_PRESS || droid_count < 1) {
        state = TOP_MENU;
        selected_item = 0;    // scanner
        tft_update = true;
      } else {
        if (droid_count > 1) {
          if (current_droid == 0) {
            current_droid = droid_count - 1;
          } else {
            current_droid = (current_droid - 1) % droid_count;
          }
          tft_update = true;
        }
      }
      break;

    case CONNECTED_MENU:
      droid_disconnect();
      state = SCANNER_RESULTS;
      tft_update = true;
      break;

    case SOUND_GROUP:
    case SOUND_TRACK:
    case SOUND_PLAY:
      if (press_type == SHORT_PRESS) {
        switch (state) {
          case SOUND_GROUP:
            state = SOUND_TRACK;
            break;
          case SOUND_TRACK:
            state = SOUND_PLAY;
            break;
          case SOUND_PLAY:
            state = SOUND_GROUP;
            break;
        }
      } else {
        state = CONNECTED_MENU;
        selected_item = 0;          // todo: don't hardcode this
      }
      tft_update = true;
      break;

    case VOLUME_UP:
    case VOLUME_DOWN:
    case VOLUME_TEST:
      state = CONNECTED_MENU;
      selected_item = 1;          // todo: don't hardcode this
      tft_update = true;
      break;
  }
}

void button_handler_ex() {
  // Additional Buttons Added for Costume Build
  static uint32_t last_btn_u_time = 0;
  static uint8_t last_btn_u_state = HIGH;
  static uint32_t last_btn_d_time = 0;
  static uint8_t last_btn_d_state = HIGH;
  static uint32_t last_btn_s_time = 0;
  static uint8_t last_btn_s_state = HIGH;
  static uint32_t last_btn_b_time = 0;
  static uint8_t last_btn_b_state = HIGH;
  static uint32_t last_btn_h_time = 0;
  static uint8_t last_btn_h_state = HIGH;

  // Additional Buttons Added for Costume Build
  uint8_t now_btn_u_state = digitalRead(BUTTON_UP_PIN);
  uint8_t now_btn_d_state = digitalRead(BUTTON_DOWN_PIN);
  uint8_t now_btn_s_state = digitalRead(BUTTON_SELECT_PIN);
  uint8_t now_btn_b_state = digitalRead(BUTTON_BACK_PIN);
  //uint8_t now_btn_h_state = digitalRead(BUTTON_HOME_PIN);
  
  uint32_t now_time = millis();

  if (now_btn_u_state != last_btn_u_state && now_time - last_btn_u_time > LAZY_DEBOUNCE) {
    if (now_btn_u_state == HIGH) {
      if (now_time - last_btn_u_time > SHORT_PRESS_TIME) {
        buttonup(LONG_PRESS);
      } else {
        buttonup(SHORT_PRESS);
      }
    }
    last_btn_u_state = now_btn_u_state;
    last_btn_u_time = now_time;

    last_activity = millis();
  }

  if (now_btn_d_state != last_btn_d_state && now_time - last_btn_d_time > LAZY_DEBOUNCE) {
    if (now_btn_d_state == HIGH) {
      if (now_time - last_btn_d_time > SHORT_PRESS_TIME) {
        buttondwn(LONG_PRESS);
      } else {
        buttondwn(SHORT_PRESS);
      }
    }
    last_btn_d_state = now_btn_d_state;
    last_btn_d_time = now_time;

    last_activity = millis();
  }

  if (now_btn_s_state != last_btn_s_state && now_time - last_btn_s_time > LAZY_DEBOUNCE) {
    if (now_btn_s_state == HIGH) {
      if (now_time - last_btn_s_time > SHORT_PRESS_TIME) {
        buttonsel(LONG_PRESS);
      } else {
        buttonsel(SHORT_PRESS);
      }
    }
    last_btn_s_state = now_btn_s_state;
    last_btn_s_time = now_time;

    last_activity = millis();
  }

  if (now_btn_b_state != last_btn_b_state && now_time - last_btn_b_time > LAZY_DEBOUNCE) {
    if (now_btn_b_state == HIGH) {
      if (now_time - last_btn_b_time > SHORT_PRESS_TIME) {
        buttonbk(LONG_PRESS);
      } else {
        buttonbk(SHORT_PRESS);
      }
    }
    last_btn_b_state = now_btn_b_state;
    last_btn_b_time = now_time;

    last_activity = millis();
  }
}

void update_display_ex() {
    switch (state) {
      case SET_TIME:
        display_captioned_menu(msg_select, LIST_SET_TIME);
        break;

      case CLOCK_ENVIRO:
        clock_envio_disp();
        break;
    }
}

void setup_sensors(void){
  //Setup SDA, SCL for RTC
  Wire.begin(43, 44);
  
  // helped with Battery not powering display
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

   // Is the RTC Present?
  if (! rtc.begin()) {

    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  #ifdef PCF8523
    if (! rtc.initialized() || rtc.lostPower()) {
      // When time needs to be set on a new device, or after a power loss, the
      // following line sets the RTC to the date & time this sketch was compiled
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      wifi_setTime();

      // Note: allow 2 seconds after inserting battery or applying external power
      // without battery before calling adjust(). This gives the PCF8523's
      // crystal oscillator time to stabilize. If you call adjust() very quickly
      // after the RTC is powered, lostPower() may still return true.
    }

    // When the RTC was stopped and stays connected to the battery, it has 
    // to be restarted by clearing the STOP bit. Let's do this to ensure
    // the RTC is running.
    rtc.start();
  #endif

  #ifdef DS1307
    rtc.begin();
  #endif

  // The PCF8523 can be calibrated for:
  //        - Aging adjustment
  //        - Temperature compensation
  //        - Accuracy tuning
  // The offset mode to use, once every two hours or once every minute.
  // The offset Offset value from -64 to +63. See the Application Note for calculation of offset values.
  // https://www.nxp.com/docs/en/application-note/AN11247.pdf
  // The deviation in parts per million can be calculated over a period of observation. Both the drift (which can be negative)
  // and the observation period must be in seconds. For accuracy the variation should be observed over about 1 week.
  // Note: any previous calibration should cancelled prior to any new observation period.
  // Example - RTC gaining 43 seconds in 1 week

  float drift = 43; // seconds plus or minus over oservation period - set to 0 to cancel previous calibration.
  float period_sec = (7 * 86400);  // total obsevation period in seconds (86400 = seconds in 1 day:  7 days = (7 * 86400) seconds )
  float deviation_ppm = (drift / period_sec * 1000000); //  deviation in parts per million (μs)
  float drift_unit = 4.34; // use with offset mode PCF8523_TwoHours

  // float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)

  int offset = round(deviation_ppm / drift_unit);

  // rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
  // rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration

  // Serial.print("Offset is "); Serial.println(offset); // Print to control offset

  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }

}

void ex_setup() {
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  
  setup_sensors();
}

void loop_ex() {
  
  if (state == CLOCK_ENVIRO) clock_envio_disp();
  updateBL();
}
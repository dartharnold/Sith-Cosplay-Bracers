

// added for Bracer LEDs
#include <Adafruit_NeoPixel.h>

// added for Temp/Humid Sensor
#include <DHT11.h>

#define NUM_BEACON_PARAMS                   3

#define ABOUT_TEXT_SIZE                    DEFAULT_TEXT_SIZE       // title is ABOUT_TEXT_SIZE+1 size, all other text is ABOUT_TEXT_SIZE
#define ABOUT_TITLE_COLOR                  TFT_RED
#define ABOUT_SUBTITLE_COLOR               TFT_ORANGE
#define ABOUT_TEXT_COLOR                   TFT_LIGHTGREY
#define ABOUT_VERSION_COLOR                C565(64,64,64)          // TFT_VERYDARKGREY

// Custom Button Define

#define BUTTON_UP_PIN       18
#define BUTTON_DOWN_PIN     17
#define BUTTON_SELECT_PIN   21
#define BUTTON_BACK_PIN     16
//#define BUTTON_HOME_PIN     21    // May remove this button and use a LONGPRESS on the Back Button Instead. 

//Temp/Humid declarations
// Create an instance of the DHT11 class and set the digital I/O pin.
DHT11 dht11(13);
// Declare variables to hold Temprature and Humidity
float humidity, temp_c, temp_f;

// Rather than declaring the whole NeoPixel object here, we just create
// a pointer for one, which we'll then allocate later...
Adafruit_NeoPixel *pixels;

void get_hum_temp()
{
    // Read the humidity from the sensor.
    humidity = dht11.readHumidity();

    // Read the temperature from the sensor.
    temp_c = dht11.readTemperature();

    // Calculate temperature from C
    temp_f = ((temp_c*9)/5)+32;
}


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
      init_advertisement_data();
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
        init_advertisement_data();
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
      selected_item = 0;
      tft_update = true;
      break;

    case TOP_MENU:
      state = ABOUT;
      selected_item = 0;
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

void button_handler() {
  static uint32_t last_btn1_time = 0;
  static uint8_t last_btn1_state = HIGH;
  static uint32_t last_btn2_time = 0;
  static uint8_t last_btn2_state = HIGH;

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

  // gather current state of things
  uint8_t now_btn1_state = digitalRead(BUTTON1_PIN);
  uint8_t now_btn2_state = digitalRead(BUTTON2_PIN);

  // Additional Buttons Added for Costume Build
  uint8_t now_btn_u_state = digitalRead(BUTTON_UP_PIN);
  uint8_t now_btn_d_state = digitalRead(BUTTON_DOWN_PIN);
  uint8_t now_btn_s_state = digitalRead(BUTTON_SELECT_PIN);
  uint8_t now_btn_b_state = digitalRead(BUTTON_BACK_PIN);
  //uint8_t now_btn_h_state = digitalRead(BUTTON_HOME_PIN);
  
  uint32_t now_time = millis();

  if (now_btn1_state != last_btn1_state && now_time - last_btn1_time > LAZY_DEBOUNCE) {
    if (now_btn1_state == HIGH) {
      if (now_time - last_btn1_time > SHORT_PRESS_TIME) {
        button1(LONG_PRESS);
      } else {
        button1(SHORT_PRESS);
      }
    }
    last_btn1_state = now_btn1_state;
    last_btn1_time = now_time;

    last_activity = millis();
  }

  if (now_btn2_state != last_btn2_state && now_time - last_btn2_time > LAZY_DEBOUNCE) {
    if (now_btn2_state == HIGH) {
      if (now_time - last_btn2_time > SHORT_PRESS_TIME) {
        button2(LONG_PRESS);
      } else {
        button2(SHORT_PRESS);
      }
    }
    last_btn2_state = now_btn2_state;
    last_btn2_time = now_time;

    last_activity = millis();
  }

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

  // if (now_btn_h_state != last_btn_h_state && now_time - last_btn_h_time > LAZY_DEBOUNCE) {
  //   if (now_btn_h_state == HIGH) {
  //     if (now_time - last_btn_h_time > SHORT_PRESS_TIME) {
  //       buttonhome(LONG_PRESS);
  //     } else {
  //       buttonhome(SHORT_PRESS);
  //     }
  //   }
  //   last_btn_h_state = now_btn_h_state;
  //   last_btn_h_time = now_time;

  //   last_activity = millis();
  // }
}

void setup_add() {
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);
  //pinMode(BUTTON_HOME_PIN, INPUT_PULLUP);
}
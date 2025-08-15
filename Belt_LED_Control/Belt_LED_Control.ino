#include <Adafruit_NeoPixel.h>

// NeoPixel declarations
// Which pin on the Arduino is connected to the NeoPixels?
#define NEOPIN 9
#define PIXELCOUNT 2  // How many NeoPixels are attached to the Arduino?
#define NEOBRIGHT 50
#define PIXELFORMAT NEO_GRB + NEO_KHZ800  // NeoPixel color format & data rate. See the strandtest example for information on possible values.

uint32_t RED, GREEN, BLUE, CYAN, PURPLE, YELLOW, ORANGE, WHITE, BLACK;

// Rather than declaring the whole NeoPixel object here, we just create
// a pointer for one, which we'll then allocate later...
Adafruit_NeoPixel* pixels;

uint8_t pass = 0;

// Timing
uint32_t last_activity;

void setup() {
  Serial.begin(115200);
  // Create a new NeoPixel object dynamically with these values:
  pixels = new Adafruit_NeoPixel(PIXELCOUNT, NEOPIN, PIXELFORMAT);

  // Setup some Basic Colors to work with
  RED = pixels->Color(255, 0, 0);
  GREEN = pixels->Color(0, 255, 0);
  BLUE = pixels->Color(0, 0, 255);
  CYAN = pixels->Color(0, 255, 255);
  PURPLE = pixels->Color(255, 0, 255);
  YELLOW = pixels->Color(255, 191, 0);
  ORANGE = pixels->Color(255, 165, 0);
  WHITE = pixels->Color(255, 255, 255);
  BLACK = pixels->Color(0, 0, 0);

  pixels->begin();  // INITIALIZE NeoPixel strip object (REQUIRED)

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
}

void updatePixels() {
  uint32_t DelayOff = random(200, 500);
  uint32_t DelayOn = random(200, 800);
  uint8_t pixel;
  
  pixels->clear();  // Set all pixel colors to 'off'
  pixels->show();

  delay(DelayOff);  // Pause Pixel Off for this pass through loop
  
  if(pass == 1){
    pass = 0;
  } else {
    pass = 1;
  } 

  for (pixel = 0; pixel <= (PIXELCOUNT/2)-1; pixel++) {
    if (pass == 1) {
      pixels->setPixelColor(pixel, RED);
    } else {
      pixels->setPixelColor(pixel, BLUE);
    }
  }
  for (pixel = PIXELCOUNT/2; pixel <= PIXELCOUNT; pixel++) {
    if (pass == 1) {
      pixels->setPixelColor(pixel, BLUE);
    } else {
      pixels->setPixelColor(pixel, RED);
    }
  }

  pixels->setBrightness(NEOBRIGHT);
  pixels->show();  // Send the updated pixel colors to the hardware.
  delay(DelayOn);  // Pause Pixel On before next pass through loop
}

void loop() {
  updatePixels();
}

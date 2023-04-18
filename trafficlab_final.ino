//The Adafruit NeoPixel library for controlling the LEDs is added
#include <Adafruit_NeoPixel.h> 

/* Initialize the LED light strip (each light is called a NeoPixel) */
/* Defines the pin on the Arduino the NeoPixel light strip is connected to */
#define LEDPin         2
/* Defines how many NeoPixels are attached to the Arduino */
#define NUMPIXELS      4
/* Define type of NeoPixel strip */
#define NP_type        NEO_GRB + NEO_KHZ800
/* define number of lanes */
#define NUMLANES       4

/* typedef'ing the lane list structure. */
typedef struct ll_ele {
  int lane;
  struct ll_ele *next;
} lane_t;

int car_count[NUMLINES];
bool occupied_lanes[NUMLANES];
int current_green;
lane_t *queue;
unsigned long timer;


void setup() {
  // put your setup code here, to run once:
  queue = NULL;
  current_green = -1;
  for (size_t i = 0; i < NUMLANES; i++) occupied_lanes[i] = false;
}

void loop() {
  // put your main code here, to run repeatedly
  bool sensor_data[NUMLANES];
  load_lane_data(sensor_data);
  set_green(sensor_data);
}


void load_lane_data(bool *sensor_data) {
  for (size_t i = 0; i < NUMLANES; i++)
    if (!occupied_lanes[i] && sensor_data[i]) {
      // enq(i); /* write the enqueue later */
      queue = NULL;
    }
}

void set_green(bool *sensor_data) {
  if (current_green == -1) {

  }
}
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
/* declare the NeoPixel Pixel object */
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPin, NP_type);

/* typedef'ing the lane queue structure.
 * I will store the lane queue as a circular singly-linked list of lanes,
 * where `queue` will always point to the last lane in the queue (NULL if
 * empty). To access the top of the queue, we access queue->next; to insert
 * we place an element between queue and queue->next and make the new element
 * the tail of the queue (`queue` now points to the new element). Rather
 * convoluted, but space-efficient.
 */
typedef struct ll_ele {
  int lane;
  struct ll_ele *next;
} lane_t;

int car_count[NUMLANES] = {0,0,0,0};
bool occupied_lanes[NUMLANES] = {false,false,false,false};
int f_lanes[NUMLANES] = {A6,A3,A2,A1};
int exit_lane = A4;

int threshold = 50;

int current_green;
lane_t *queue;
unsigned long timer;


void setup() {
  Serial.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.

  // put your setup code here, to run once:
  queue = NULL;
  current_green = -1;
  for (size_t i = 0; i < NUMLANES; i++) {
    occupied_lanes[i] = false;
    car_count[i] = 0;
  }
}

void loop() {
  // put your main code here, to run repeatedly
  bool sensor_data[NUMLANES];
  record_lane_data(sensor_data);

  update_counts(sensor_data);

  load_lane_data(sensor_data);
  if (set_green(sensor_data)) return;

  /* incorporate Dalton's code to check whether to send red to current lane.
   */

}

void record_lane_data(bool *sensor_data) {
  int f_lane_vals[NUMLANES];
  /* this loop records the data into an array containing values
   * and records which lanes have cars in occupied_lanes
   */
  for (size_t i = 0; i < NUMLANES; i++) {
    f_lane_vals[i] = map(analogRead(f_lanes[i]), 0, 1023, 0, 100);
    /* sensor_data[i] is set to whether there is a car over that sensor) */
    sensor_data[i] = (f_lane_vals[i] > threshold);
  }
}

void update_counts(bool *sensor_data) {
  if (!sensor_data[current_lane] && occupied_lanes[current_lane])
    car_count[current_lane] = car_count[current_lane] + 1;
}

void load_lane_data(bool *sensor_data) {
  for (size_t i = 0; i < NUMLANES; i++)
    if (!occupied_lanes[i] && sensor_data[i] && i != current_green) {
      /* add lane `i` to the queue. see line 14. */
      lane_t *next = queue;
      lane_t *new_lane = malloc(sizeof(ll_ele));
      new_lane->lane = i;
      if (queue == NULL) // make new_lane circular with itself
        new_lane->next = new_lane;
      else { // insert new_lane after the queue's tail
        new_lane->next = queue->next;
        queue->next = new_lane;
      }
      queue = new_lane; // new_lane becomes the queue's new tail
    }
}

bool set_green(bool *sensor_data) {
  if (current_green == -1) { // if there is no current lane, set that lane
    // dequeue top of queue; set current_green to that lane
    lane_t *lane = queue->next;
    current_green = lane->lane;
    if (queue != lane) // there is a new head for tail to link to
      queue->next = lane->next;
    else // head and tail are identical and we have dequeued that lane
      queue = NULL;
    free(lane); // in any case, free lane so we don't memleak :D

    pixels.setPixelColor(current_green, pixels.Color(0, 150, 0));
    return true;
  }
  return false;
}
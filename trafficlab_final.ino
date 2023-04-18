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

int car_count[NUMLANES];
bool occupied_lanes[NUMLANES];
int current_green;
lane_t *queue;
unsigned long timer;


void setup() {
  Serial.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.

  // put your setup code here, to run once:
  queue = NULL;
  current_green = -1;
  for (size_t i = 0; i < NUMLANES; i++) occupied_lanes[i] = false;
}

void loop() {
  // put your main code here, to run repeatedly
  bool sensor_data[NUMLANES];
  // load sensor data into sensor_data
  load_lane_data(sensor_data);
  set_green(sensor_data);
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

void set_green(bool *sensor_data) {
  if (current_green == -1) { // if there is no current lane, set that lane.
    // dequeue top of queue; set current_green to that lane
    lane_t *lane = queue->next;
    current_green = lane->lane;
    queue->next = lane->next;
    free(lane);

    pixels.setPixelColor(current_green, pixels.Color(0, 150, 0));
  }
}
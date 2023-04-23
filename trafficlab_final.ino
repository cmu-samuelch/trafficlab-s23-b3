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
unsigned long lane_green_time;

unsigned long max_green_time = 30000; // max green time: 30s
unsigned long lane_empty_threshold = 1500; // time to treat lane as empty: 1.5s


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

  set_all_red();
}

void loop() {
  // put your main code here, to run repeatedly
  bool sensor_data[NUMLANES];
  record_lane_data(sensor_data);

  update_counts(sensor_data);

  load_lane_data(sensor_data);
  if (set_green(sensor_data)) return;

  send_red(sensor_data);
}

void set_all_red(void) {
  for (size_t i = 0; i < NUMLANES; i++)
    pixels.setPixelColor(i, pixels.Color(150, 0, 0));
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
  /* if a car is currently in the current lane, reset the timer since last car
   * detected in open lane
   */
  if (sensor_data[current_green])
    last_car_time = millis();
  /* if a car was in this lane at last check but is no longer in the lane,
   * increment the current lane's car count by 1.
   */
  if (!sensor_data[current_green] && occupied_lanes[current_green])
    car_count[current_green] = car_count[current_green] + 1;
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
    lane_green_time = millis();
    return true;
  }
  return false;
}

/* header comment */
void set_red(void) {
  unsigned long current_time = millis();

  /* if we exceed the max green time on the current lane, cycle the light
   * to red.
   *
   * otherwise, check if the currently-green lane is empty.
   * We accomplish this by checking if the time between now and the most recent
   * car to pass through exceeds a certain time, say, 3 seconds.
   * if we find that lane has been empty for `lane_empty_threshold` ms, cycle
   * the light to red.
   */
  if (current_time - lane_green_time >= max_green_time
   || current_time - last_car_time >= lane_empty_threshold) {
    pixels.setPixelColor(i,  pixels.Color(150, 150, 0)); // Yellow
    pixels.show(); delay(5000);

    pixels.setPixelColor(i,  pixels.Color(150, 0, 0)); // Red
    pixels.show(); delay(1000);
    
    current_green = -1;
  }
}
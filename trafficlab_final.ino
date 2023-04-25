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

/* @brief Lane queue structure, implemented as a singly-linked list.
 *
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
int f_lanes[NUMLANES] = {A5,A2,A1,A0};    // set the pin for each lane
int exit_lane = A4;

int threshold[NUMLANES] = {90,50,50,50};  // thresholds for each lane.

int current_green;
lane_t *queue;
unsigned long lane_green_time, last_car_time;

const unsigned long yellow_time = 1500;
const unsigned long red_time = 1000;
const unsigned long max_green_time = 30000; // max green time: 30s
const unsigned long lane_empty_threshold = 1000; // time to treat lane as empty


void setup() {
  Serial.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.

  // put your setup code here, to run once:
  queue = NULL;
  current_green = -1;
  last_car_time = 0;
  for (int i = 0; i < NUMLANES; i++) {
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
  if (set_green()) return;

  if (queue != NULL) set_red();
}

/* flashes all lights blue, then sets all lights to red. */
void set_all_red(void) {
  for (int i = 0; i < NUMLANES; i++) // set all lanes to blue for 1s
    pixels.setPixelColor(i, pixels.Color(0,0,200));
  pixels.show(); delay(1000);

  for (int i = 0; i < NUMLANES; i++)
    pixels.setPixelColor(i, pixels.Color(150, 0, 0));
  pixels.show();
  Serial.println("initialized. set to red."); // then set all to red
}

/* reads raw sensor data for each lane, then records whether each lane has a 
 * car present in `sensor_data`.
 *
 * INPUT sensor_data Array containing lane occupancies.
 */
void record_lane_data(bool *sensor_data) {
  int f_lane_vals[NUMLANES];
  /* this loop records the data into an array containing values
   * and records which lanes have cars in occupied_lanes
   */
  for (int i = 0; i < NUMLANES; i++) {
    f_lane_vals[i] = map(analogRead(f_lanes[i]), 0, 1023, 0, 100);
    /* sensor_data[i] is set to whether there is a car over lane i's sensor */
    sensor_data[i] = (f_lane_vals[i] > threshold[i]);
  }
}

/* checks whether a car has left the lane on this check; if so increments the
 * corresponding lane's car count.
 *
 * INPUT sensor_data Array containing lane occupancies.
 */
void update_counts(bool *sensor_data) {
  /* if a car is currently in the current lane, reset the timer since last car
   * detected in open lane
   */
  if (sensor_data[current_green])
    last_car_time = millis();
  
  /* if a car was in current lane at last check but is no longer in the lane,
   * increment current lane's car count by 1.
   */
  if (!sensor_data[current_green] && occupied_lanes[current_green]) {
    car_count[current_green] = car_count[current_green] + 1;
    Serial.print("lane "); Serial.print(current_green);
    Serial.print(": car count "); Serial.println(car_count[current_green]);
  }
}

/* After reading sensor data into the `sensor_data` array, determines which (if
 * any) lanes ought to be added to queue, and adds them. Does a bunch of queue
 * stuff, elaborated upon in the lane_t header comment.
 *
 * INPUT sensor_data Array containing lane occupancies.
 */
void load_lane_data(bool *sensor_data) {
  for (int i = 0; i < NUMLANES; i++) {
    /* if lane `i` was not occupied at last check but is now, and lane `i` is
     * not the currently-green lane, add it to queue.
     */
    if (!occupied_lanes[i] && sensor_data[i] && i != current_green) {
      /* add lane `i` to the queue. see line 14. */
      lane_t *next = queue;
      lane_t *new_lane = malloc(sizeof(ll_ele));
      new_lane->lane = i;
      if (queue == NULL) // make new_lane circular with itself
        new_lane->next = new_lane;
      else { // insert new_lane after the queue's tail
        new_lane->next = queue->next; // queue head remains lane following tail
        queue->next = new_lane;
      }
      queue = new_lane; // new_lane becomes the queue's new tail
    }
    /* no matter what the sensor data was, we want to update occupied_lanes to
     * have the same lane occupancies from current check. we do this here since
     * we no longer need to compare the difference between current data and
     * previous data.
     */
    occupied_lanes[i] = sensor_data[i];
  }
}

/* Sets a green light, if applicable.
 * 
 * First checks if there is no current lane and that there is a lane in queue.
 * If both of these are satisfied, then we remove the lane at the top of the
 * queue, and set `current_green` to whatever that lane is. We then manage the
 * queue to maintain its structure so it is still a valid lane queue.
 *
 * RETURN VALUE: `true` if we set a lane to green, `false` otherwise.
 */
bool set_green(void) {
  // if there is no current lane, set that lane
  if (current_green == -1 && queue != NULL) {
    // dequeue top of queue; set current_green to that lane
    lane_t *lane = queue->next;
    current_green = lane->lane;
    if (queue != lane) // there is a new head for tail to link to
      queue->next = lane->next;
    else // head and tail are identical and we have dequeued that lane
      queue = NULL;
    free(lane); // in any case, free lane so we don't memleak :D

    pixels.setPixelColor(current_green, pixels.Color(0, 150, 0));
    pixels.show();
    lane_green_time = millis(); // reset time since a lane was made green
    return true;
  }
  return false;
}

/* Checks whether a lane should be set to red, and does so if applicable.
 * 
 * The current lane should be set to red if either:
 * I.   it has been green for longer than the green-threshold
 * II.  it has been empty for longer than the empty-threshold
 * If either of these is satisfied, then we start the yellow cycle. Note that
 * since there are delays built into the yellow and red cycles, car-counting
 * cannot occur during this time, and the car count may suffer error from this.
 * Cars leaving the lane while the light is yellow cannot be counted, and will
 * need to be accounted for manually.
 */
void set_red(void) {
  unsigned long current_time = millis();

  /* compare time elapsed since light set to green to green-threshold
   * and time elapsed since last car present in lane to empty-threshold;
   * if either of these times are above threshold then set current lane to
   * yellow and then red.
   */
  if (current_time - lane_green_time >= max_green_time
   || current_time - last_car_time >= lane_empty_threshold) {
    pixels.setPixelColor(current_green,  pixels.Color(150, 150, 0)); // Yellow
    pixels.show(); delay(yellow_time);

    pixels.setPixelColor(current_green,  pixels.Color(150, 0, 0)); // Red
    pixels.show(); delay(red_time);
    
    current_green = -1;
  }
}
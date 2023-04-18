// Starting (incomplete) code for sensing vehicles using photoresisters and controlling traffic lights 
// The LED strip is added, but only two sensors have been added to the code. The other sensors should be added.
// The algorithms needed to control the lights using sensor data have not been added.
unsigned long Timer;
unsigned long Sensor_off;

//The Adafruit NeoPixel library for controlling the LEDs is added
#include <Adafruit_NeoPixel.h> 

//Initialize the LED light strip (each light is called a NeoPixel)
#define LEDPin         2 // Defines the pin on the Arduino to which the NeoPixel light strip is connected
#define NUMPIXELS      4 // Defines how many NeoPixels are attached to the Arduino

//declare the NeoPixel Pixel object
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPin, NEO_GRB + NEO_KHZ800);
/* Argument 1 = Number of pixels in NeoPixel strip
   Argument 2 = Arduino pin number to which the NeoPixel strip is connected
   Argument 3 = Pixel type indicates the type of NeoPixel strip
      NEO_GRB = Pixels are wired for GRB bitstream
      NEO_KHZ800 = 800 KHz bitstream
*/

int delayVal = 500; // delay for half a second when power on 

int photoPin7 = A6;    //  For Model A, the input pin for the photoresistor for Lane 0 (senses light )
int photoPin4 = A3;    //  For Model A, the input pin for the photoresistor for Lane 1 (senses light )
int photoPin2 = A2;
int photoPin3 = A1;
// add more photoPins to use the other sensors!

int photoSensor7Val = 0;  // variable to store the value coming from the sensor for Lane 0 initialized at 0
int photoSensor4Val = 0;  // variable to store the value coming from the sensor for Lane 1 initialized at 0
int photoSensor2Val = 0;
int photoSensor3Val = 0;
// add more variables for the rest of the sensors!

int sensorThreshold = 50; // represents the change from not detecting a car to detecting a car. if sensor value is less than threshold then represents car is over sensor
// change sensorThreshold depending on actual sensor behavior, you may have to add additional threshold values if sensors behave differently



// the setup function r//uns once when you press reset or power the board
void setup() { 

Serial.begin(9600); //sets the rate in bits per second (bauds) at which data is sent to the serial monitor serial monitor is opened by clicking the magnifying glass in upper right hand corner
pixels.begin(); // This initializes the NeoPixel library.
//color_cycle_startup(); //this is used to confirm that the NeoPixel strip works upon initial start up. (This function is defined below the loop function)

}



// the loop function runs over and over again forever
void loop() {

 photoSensor7Val = analogRead(photoPin7); // Reads the value of the Photoresistor Sensor 1
 photoSensor7Val= map(photoSensor7Val, 0, 1023, 0, 100); //Maps the value read from the Photoresistor sensor that ranges from 0 to 1023 to a corresponding value from 0 to 100
 Serial.print("Photo 7: ");Serial.println(photoSensor7Val); //Prints the photoresistor value to the serial monitor
  
 photoSensor4Val = analogRead(photoPin4); // Reads the Val of the Photoresistor Sensor 2
 photoSensor4Val= map(photoSensor4Val, 0, 1023, 0, 100); //Maps the value read from the Photoresistor sensor that ranges from 0 to 1023 to a corresponding value from 0 to 100
 Serial.print("Photo 4: ");Serial.println(photoSensor4Val); //Prints the photoresistor value to the serial monitor
  
 photoSensor2Val = analogRead(photoPin2); // Reads the value of the Photoresistor Sensor 1
 photoSensor2Val= map(photoSensor2Val, 0, 1023, 0, 100); //Maps the value read from the Photoresistor sensor that ranges from 0 to 1023 to a corresponding value from 0 to 100
 Serial.print("Photo 2: ");Serial.println(photoSensor7Val);

 photoSensor3Val = analogRead(photoPin3); // Reads the value of the Photoresistor Sensor 1
 photoSensor3Val= map(photoSensor3Val, 0, 1023, 0, 100); //Maps the value read from the Photoresistor sensor that ranges from 0 to 1023 to a corresponding value from 0 to 100
 Serial.print("Photo 3: ");Serial.println(photoSensor7Val);
 // if (photoSensor7Val <= sensorThreshold ){

    //Each LED (aka pixels) is assigned a color by setting the brightness values for Red, Green, and Blue (RGB) lights 
    //Color is set using pixels.Color (Red,Green,Blue). Where the brightness for each color light is chosen from the range of 0-255. 
  
   // pixels.setPixelColor(0, pixels.Color(0, 0, 150)); // Set medium bright blue color at the first pixel, 0 corresponds to the LED for Lane 0.
 // }
 // else {
 //   pixels.setPixelColor(0, pixels.Color(0,0,0)); // Turns LED off
//  }
 // pixels.show(); //pushes color data to pixels


 // if (photoSensor4Val <= sensorThreshold ){
//    pixels.setPixelColor(1, pixels.Color(0, 0, 150)); // Set medium bright blue color at the first pixel, 1 corresponds to the LED for Lane 1.
//  }
//  else {
 //   pixels.setPixelColor(1, pixels.Color(0,0,0));
 // }
 // pixels.show();


  pixels.setPixelColor(2, pixels.Color(0,0,0)); // LED 2 off
  pixels.setPixelColor(3, pixels.Color(0,0,0)); // LED 3 off
  pixels.show(); // This sends the updated pixel color to the hardware.

// To start, I am assuming that a lane has been selected and the light cycle must begin. 
  Timer = 0;
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i,  pixels.Color(0, 150, 0)); // Green
    Timer = millis();
    int Timermax = 15000;
    if (Timer >= Timermax){
      pixels.setPixelColor(i,  pixels.Color(150, 150, 0)); // Yellow
      pixels.show(); 
      delay(5000);

      pixels.setPixelColor(i,  pixels.Color(150, 0, 0)); // Red
      pixels.show(); 
      delay(1000);
      break;
    }  
     
    if (photoSensor7Val <= sensorThreshold){
      Sensor_off = 0;
      Sensor_off = millis();
      if (Sensor_off >= 3000) {
        pixels.setPixelColor(i,  pixels.Color(150, 150, 0)); // Yellow
        pixels.show(); 
        delay(5000);

        pixels.setPixelColor(i,  pixels.Color(150, 0, 0)); // Red
        pixels.show(); 
        delay(1000);
        break;
      }       
    }
    if (photoSensor4Val <= sensorThreshold){
      Sensor_off = 0;
      Sensor_off = millis();
      if (Sensor_off >= 3000) {
        pixels.setPixelColor(i,  pixels.Color(150, 150, 0)); // Yellow
        pixels.show(); 
        delay(5000);

        pixels.setPixelColor(i,  pixels.Color(150, 0, 0)); // Red
        pixels.show(); 
        delay(1000);
        break;
      }  
    }     
    if (photoSensor2Val <= sensorThreshold){
      Sensor_off = 0;
      Sensor_off = millis();
      if (Sensor_off >= 3000) {
        pixels.setPixelColor(i,  pixels.Color(150, 150, 0)); // Yellow
        pixels.show(); 
        delay(5000);

        pixels.setPixelColor(i,  pixels.Color(150, 0, 0)); // Red
        pixels.show(); 
        delay(1000);
        break;
      }  
    }     
    if (photoSensor3Val <= sensorThreshold){
      Sensor_off = 0;
      Sensor_off = millis();
      if (Sensor_off >= 3000) {
        pixels.setPixelColor(i,  pixels.Color(150, 150, 0)); // Yellow
        pixels.show(); 
        delay(5000);

        pixels.setPixelColor(i,  pixels.Color(150, 0, 0)); // Red
        pixels.show(); 
        delay(1000);
        break;
      }  
    }     
  }
}






int f_lane0 = A6; // pins in front of lane
int f_lane1 = A3;
int f_lane2 = A2;
int f_lane3 = A1;
int exit_lane = A4;
int f_lane0Val;
int f_lane1Val;
int f_lane2Val;
int f_lane3Val;
int free_lanes[];
int laneVal[];
int fLane[];
int threshold = 50;
int carCount;
int laneCount[]={0,0,0,0};
unsigned long timer;
bool car_present;




void setup() {
  // put your setup code here, to run once:
  car_present = false;
  carCount = 0;
  Serial.begin(9600);
  laneVal[]={f_lane0Val,f_lane1Val, f_lane2Val,f_lane3Val};
  free_lanes[]={0,0,0,0};
  fLane[]={f_lane0,f_lane1,f_lane2,f_lane3};


}


void loop() {
  // put your main code here, to run repeatedly:
  //this code can be used during testing by removed for final code, just checking if the values we are
  //meant to recieve are "correct"
  /*
  f_lane0Val= analogRead(f_lane0);
  f_lane0Val = map(f_lane0,0,1023,0,100);
  f_lane1Val= analogRead(f_lane1);
  f_lane1Val = map(f_lane0,0,1023,0,100);
  f_lane2Val= analogRead(f_lane2);
  f_lane2Val = map(f_lane0,0,1023,0,100);
  f_lane3Val= analogRead(f_lane3);
  f_lane3Val = map(f_lane0,0,1023,0,100);
  /* set up if statements checking if a car is present, if so adds to counter of cars
  and prints that there is a car present in said lane
  this boolean can help to decide the queue and what lanes to put within the initial queue
  */
  for(int i =0; 1<4 ; i++){
    if(laneVal[i] >= threshold){
      car_present= true;
      free_lanes[i] = 1
    }
   if(lane(i) == green){
        if(lane(i) == false){
          carCount++;
      }
     }
   }

  
  
  








}

 #include <SR04.h>
 
 
 #define IRright A0
 #define IRmiddle A1
 #define IRleft A2
 #define TRIG_PIN 13
 #define ECHO_PIN 12


 long distance;
 bool isSpinning = false;

 unsigned long startTime; 
unsigned long currTime;
unsigned long ultrasonicStartTime;
unsigned long ultrasonicCurrTime;


SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);

 
  int pin1 = 1;
  int modeSwitch = 2;
  int wheelsON = 3;
  int pin4 = 4;
  int speedR = 5;
  int speedL = 6;
  int rightDirection = 7;
  int leftDirection = 8;


void setup() {
  pinMode(speedR, OUTPUT);
  pinMode(speedL, OUTPUT);
  pinMode(rightDirection, OUTPUT);
  pinMode(leftDirection, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(modeSwitch, INPUT);
  pinMode(wheelsON, OUTPUT); 
  pinMode(pin4, OUTPUT);
  pinMode(IRleft, INPUT);
  pinMode(IRmiddle, INPUT);
  pinMode(IRright, INPUT);

  startTime = millis();
  ultrasonicStartTime = millis();

  delay(3000);
}


//////////////////////////////////////////////////////  Functions //////////////////////////////////////////////////



void forward(int speed) {
    digitalWrite(wheelsON, HIGH);
    digitalWrite(rightDirection, HIGH);
    digitalWrite(leftDirection, HIGH);
    analogWrite(speedR, speed);
    analogWrite(speedL, speed);
}

void backward(int speed) {
    digitalWrite(wheelsON, HIGH);
    digitalWrite(rightDirection, LOW);
    digitalWrite(leftDirection, LOW);
    analogWrite(speedR, 180);
    analogWrite(speedL, 180);
}

void clockwise(int speed) {
    digitalWrite(wheelsON, HIGH);
    digitalWrite(rightDirection, LOW);
    digitalWrite(leftDirection, HIGH);
    analogWrite(speedR, speed);
    analogWrite(speedL, speed);
}

void counterClockwise(int speed) {
    digitalWrite(wheelsON, HIGH);
    digitalWrite(rightDirection, HIGH);
    digitalWrite(leftDirection, LOW);
    analogWrite(speedR, speed);
    analogWrite(speedL, speed);
}



//for the infrared sensors. If sees white on LEFT = 0; MIDDLE = 1; RIGHT = 2; BLACK = 4
int infrared(){
  float left = analogRead(IRleft);
  float right = analogRead(IRright);
  float middle = analogRead(IRmiddle);

  if (left < 100 && middle < 100 && right < 100){
    return 1;
  }
  else if (left < 100){
    return 0;
  }
  else if (right < 100){
    return 2;
  }
  else {
    return 4;     //Safty return 
  }
}



//Looks for the edge of the mat
bool matEdge(){
  //check for edge of mat          
  if(infrared() == 2){
    backward(255);
    delay(300);
    counterClockwise(255);
    delay(250);

    isSpinning = false;

    return true;
  }
  else if (infrared() == 0){
    backward(255);
    delay(300);
    clockwise(255);
    delay(250);    
    isSpinning = false;

    return true;
  }
  else if (infrared() == 1){
    backward(255);
    delay(300);
    clockwise(255);
    delay(350);

    isSpinning = false;
    
    return true;
  }
  else{
    return false;
  }
}


////////////////////////////////////////////////////////// Main Loop ////////////////////////////////////////////


void loop() {

  if (ultrasonicCurrTime - ultrasonicStartTime > 50){   //for ultrasonic sensor
    distance = sr04.Distance();
    ultrasonicStartTime = ultrasonicCurrTime;
  }
  
  currTime = millis();
  ultrasonicCurrTime = millis();



  if (distance < 75){    //when found
          
    
    if (isSpinning){
      counterClockwise(255);    //stop spinning
    delay(50);
    }
    
    while (distance < 75){
      ultrasonicCurrTime = millis();    //just for ultrasonic sensor
      forward(255);           //push off mat
      if (matEdge()){
        startTime = currTime;
      }
      if (ultrasonicCurrTime - ultrasonicStartTime > 50){  // just for ultrasonic sensor
        distance = sr04.Distance();
        ultrasonicStartTime = ultrasonicCurrTime;
      }

      while (distance < 8){                   //When it is actually pushing a bot
        ultrasonicCurrTime = millis();    //just for ultrasonic sensor
        forward(255);           //push off mat
        if (matEdge()){
          startTime = currTime;
        }
        if (ultrasonicCurrTime - ultrasonicStartTime > 50){  // just for ultrasonic sensor
          distance = sr04.Distance();
          ultrasonicStartTime = ultrasonicCurrTime;
        }
      }
    }
  }
  else{

    
    if (currTime - startTime <= 700){    // Drive forward
      forward(180);
      isSpinning = false;
    }
    else if (currTime - startTime <= 1900){
      clockwise(170);           //searching for opponent  Spinning
      isSpinning = true;
    }
    else{
      startTime = currTime;                 // reset cycle
    }

    if (matEdge()){
      startTime = currTime;
    }
  }  
  
}

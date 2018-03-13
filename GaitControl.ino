// gait array
/*
const int Relay[][8] = {{22, 24, 26, 28, 30, 32, 34, 36},
  {34, 36, 22, 24, 26, 28, 30, 32},
  {30, 32, 34, 36, 22, 24, 26, 28},
  {26, 28, 30, 32, 34, 36, 22, 24}
};
*/

/*
const int Relay[][8] = {{30, 32, 34, 36, 38, 40, 42, 44},
  {30, 32, 34, 36, 38, 40, 42, 44},
  {30, 32, 34, 36, 38, 40, 42, 44},
  {30, 32, 34, 36, 38, 40, 42, 44}
};
*/

/*
//int leg1[4] = {4, 6, 4, 5};
//int leg2[4] = {6, 3, 7, 1};
int leg1[4] = {0, 6, 4, 5};
int leg2[4] = {1, 3, 7, 1};
//int delay1[4] = {150, 150, 150, 150};
//int delay2[4] = {150, 75, 75, 50};
int delay1[4] = {200, 200, 200, 200};
int delay2[4] = {200, 100, 100, 100};
*/

int actPins[8] = {30, 32, 34, 36, 38, 40, 42, 44};

int legPin1[][4] = {
  {40, 44, 44, 32},
  {40, 44, 44, 36},
  {36, 40, 32, 32} 
};
int legPin2[][4] = {
  {36, 40, 32, 36},
  {34, 38, 34, 30},
  {42, 30, 42, 38} 
};

int analogPins1[][4] = {
  {A5, A7, A7, A1},
  {A5, A7, A7, A3},
  {A3, A5, A1, A1}
};

int analogPins2[][4] = {
  {A3, A5, A1, A3},
  {A2, A4, A2, A0},
  {A6, A0, A6, A4}
};

int del1 = 1500;
int del2 = 1500;
int del3 = 1500;

int delLeg1[][4] = {
  {del1, del1, del1, del1},
  {del2, del2, del2, del2},
  {del2, del2, del2, del2}
};
int delLeg2[][4] = {
  {del1, del1, del1, del1},
  {del3, del3, del3, del3},
  {del3, del3, del3, del3}
};

int restdelay = 300; 


/** Additional parameters by me**/
int actuationTime = 1000; //Interrupt time in ms
int deactuationTime = 1000;
int outputPinNum = 6; //Single output actuating pin
int curveInput = A3;

String strc, strTime, strCurvature, strGait, outputString;

int curvaturePins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
//int pressurePins[8] = {A8, A9, A10, A11, A12, A13, A14, A15};

int pressureReading;
int curvatureReading;
int serTrigger = 0;
int gait = 0; // default
int newGait = 1; // for recognizing a new gait
int gaitDirection = 0;
int subGait = 0;

//float interruptTime = 0.000064;
float interruptTime = 0.064; 
int legTimer = 0;
int timerCount;
int legTimeStatus = 0; //A
int prevTimeStatus = 0; //B

int numValves = 8;
int characterInput;

int codeStatus = 0;

void setup() {   
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps   57600 115200
  
  strc = String(", ");
  outputString = String();
  strTime = String();
  strCurvature = String();
  strGait = String();
  
  pinMode(13, OUTPUT);
  digitalWrite(13,LOW);
  
  for (int i = 0; i < numValves; i++) {
    pinMode(actPins[i], OUTPUT);
    digitalWrite(actPins[i], LOW);
  }

  // Single leg actuating pin
  pinMode(outputPinNum, OUTPUT);
  digitalWrite(outputPinNum, LOW);
  
  // timer interrupt
  cli();          // disable global interrupts
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
 
  // set compare match register to desired timer count:
  
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  // enable global interrupts:
  sei();
  
}

void loop() {
  
  // Do nothing unti something is received over serial
  if (serTrigger == 0) {
    Serial.print("Waiting for initial gait input...");
    Serial.print("\n");
    while(Serial.available() == 0){}
    serTrigger = 1; 
  }
  
        
  // See if there's a new gait input
  if (Serial.available() > 0) {
    characterInput = Serial.read();
//    switch(characterInput){
//      case 'a':
//        gait = 0;
//        break; 
//      case 'b':
//        gait = 1;
//        break; 
//      case 'c':
//        gait = 2;
//        break; 
//      case 'd':
//        gait = 3;   
//        break;    
//      case 'e':
//        gait = 4;
//        break;       
//      case 'f':
//        gait = 5;
//        break;       
//      case 'g':
//        gait = 6;
//        break; 
//      case 'h':
//        gait = 7;
//        break; 
//      case 'i':
//        gait = 8;
//        break; 
//      case 'j':
//        gait = 9;
//        break; 
//      case 'k':
//        gait = 10;
//        break; 
//      case 'l':
//        gait = 11;
//        break; 
//      default:
//        //Serial.print("DEFAULT SELECTED \n");
//        gait = 0;
//        break; 
//    }
//    gaitDirection = floor(gait/3);
//    subGait = gait % 3;
//    //newGait = 1;
    legTimeStatus = 1; // reset leg timer status  
    prevTimeStatus = 1;
    
    // reset all output pins
    digitalWrite(outputPinNum, LOW);
    
    for (int i = 0; i < numValves; i++) {
       digitalWrite(actPins[i], LOW);
    }
  }  
  
  /*
  // Activate gait 
  if (newGait == 1){
    legTimeStatus = 1; // reset leg timer status  
    prevTimeStatus = 1;
    newGait = 0;
  }
  */
  
  
  
  // Manage Timers  
  // States actuation and deactuation. Other states are below
  if(legTimeStatus == prevTimeStatus) {
    
    // State actuating
    if (legTimeStatus == 1) {
      // set first leg timer
      cli();
      legTimer = actuationTime;
      //legTimer = delLeg1[subGait][gaitDirection];
      //digitalWrite(Relay[gaitDirection][leg1[subGait]], HIGH);
      //digitalWrite(legPin1[subGait][gaitDirection], HIGH);
      digitalWrite(outputPinNum, HIGH);
      timerCount = int(legTimer/interruptTime) - 1; //Number of clock cycles
      OCR1A = timerCount;
      TCNT1 = 0;
      legTimeStatus = 2;
      sei();
      //digitalWrite(13, HIGH);
    }
    // State deactuating
    else if (legTimeStatus == 2) {
      cli();
      legTimer = deactuationTime;
      //digitalWrite(Relay[gaitDirection][leg1[subGait]], LOW);
      //digitalWrite(legPin1[subGait][gaitDirection], LOW);
      digitalWrite(outputPinNum, LOW);
      timerCount = int(legTimer/interruptTime) - 1;
      OCR1A = timerCount;
      TCNT1 = 0;
      legTimeStatus = 3;
      sei();
      //digitalWrite(13, LOW);
    }
//    else if (legTimeStatus == 3) {
//      cli();
//      legTimer = delLeg2[subGait][gaitDirection];
//      //digitalWrite(Relay[gaitDirection][leg2[subGait]], HIGH);
//      digitalWrite(legPin2[subGait][gaitDirection], HIGH);
//      timerCount = int(legTimer/interruptTime) - 1;
//      OCR1A = timerCount;
//      TCNT1 = 0;
//      legTimeStatus = 4;
//      sei();
//      //digitalWrite(13, HIGH);
//    }  
//    else if (legTimeStatus == 4) {
//      cli();
//      legTimer = restdelay;
//      //digitalWrite(Relay[gaitDirection][leg2[subGait]], LOW);
//      digitalWrite(legPin2[subGait][gaitDirection], LOW);
//      timerCount = int(legTimer/interruptTime) - 1;
//      OCR1A = timerCount;
//      TCNT1 = 0;
//      legTimeStatus = 1;
//      sei();
//      //digitalWrite(13, LOW);
//    }  
  }
  
  
  // All other states beside acutating and deactuating
  if (legTimeStatus == 1) {
    // Take Data and print to serial
    //pressureReading = analogRead(pressurePins[leg1[subGait]]);
    //curvatureReading = analogRead(curvaturePins[leg1[subGait]]);
    //curvatureReading = analogRead(analogPins1[subGait][gaitDirection]);
    curvatureReading = analogRead(curveInput);   
  }
//  else if (legTimeStatus == 3) {
//    //pressureReading = analogRead(pressurePins[leg2[subGait]]);
//    //curvatureReading = analogRead(curvaturePins[leg2[subGait]]);
//    curvatureReading = analogRead(analogPins2[subGait][gaitDirection]);   
//  }
  else {
    pressureReading = 0;
    curvatureReading = 0;
  } 
  
  //strTime = "Time: " + millis();
  //strGait = gait;
  //strCurvature = curvatureReading;
  //outputString = strTime + strc + strGait + strc + strCurvature + "\n";
  //Serial.print(millis() + strc + gait + strc + subGait + strc + gaitDirection + strc + curvatureReading + strc + legPin1[subGait][gaitDirection] + strc + legPin2[subGait][gaitDirection] + "\n");  
  //Serial.print(millis() + strc + gait + strc + subGait + strc + gaitDirection + strc + curvatureReading + "\n");
  Serial.print(millis() + strc + curvatureReading + "\n");
  /*
  // Transmit data over serial
  Serial.print(millis());
  Serial.print(", ");
  Serial.print(gait);
  Serial.print(", ");
  //Serial.print(gaitDirection);
  //Serial.print(", ");
  //Serial.print(subGait);
  //Serial.print(", ");
  //Serial.print(legTimeStatus);
  //Serial.print(", ");
  //Serial.print(pressureReading);
  //Serial.print(", ");
  Serial.print(curvatureReading);
  Serial.print("\n");
  */
  
}


ISR(TIMER1_COMPA_vect)
{
  if (prevTimeStatus == 4) {
    prevTimeStatus = 1;
  }
  else {
    prevTimeStatus = prevTimeStatus + 1;
  }
}


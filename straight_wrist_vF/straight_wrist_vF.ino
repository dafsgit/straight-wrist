const int numSamples = 10000;
int f_up_thr = 525; // up flex sensor
int f_down_thr = 604; // down flex sensor

const int v_up = 10; // up vibration motor
const int v_down = 11; // down vibration motor
int n_trials = 0;
bool vibration = true;
const int minDifference = 0;      // Min diff for speed mapping
const int maxDifference = 20;     // Max diff for speed mapping
const int minSpeedPercent = 50;   // Min motor speed as %
const int maxSpeedPercent = 100;  // Max motor speed as %

unsigned long startMillis;
unsigned long currentMillis;

void setup()
{
  pinMode(v_up, OUTPUT);  // up vibration motor
  pinMode(v_down, OUTPUT); // down vibration motor
  pinMode(A0, INPUT); // up flex sensor
  pinMode(A1, INPUT); // down flex sensor
  Serial.begin(9600);
  delay(3000);
}

void loop()
{
  if(Serial.available() > 0){
    char x = Serial.read();

    if(x == 'h'){
      upThreshold();  
    } else if(x == 'd'){
      downThreshold();
    } else if(x == 'v'){
      vibration = !vibration;
      if(vibration){
        Serial.println("Vibration ON");
      } else {
        Serial.println("Vibration OFF");
      }
    } else if(x == 't'){
      n_trials += 1;
      Serial.print("Trial ");
      Serial.print(n_trials);
      Serial.println(" started");
      Serial.print("time");
      Serial.print(", ");
      Serial.print("up");
      Serial.print(", ");
      Serial.print("down");
      Serial.print(", ");
      Serial.print("lbl_up");
      Serial.print(", ");
      Serial.println("lbl_down");
      getData();
    } else{
      Serial.println(" ");
    }
  }
}

void getData(){
  bool collectingData = true;
  startMillis = millis();
  while(collectingData){
    currentMillis = millis();
    float curr_seg = (currentMillis - startMillis) / 1000.0;
    int f1 = analogRead(A0);
    int f2 = analogRead(A1);
    int lbl_up = 0;
    int lbl_down = 0;
    int speed = 0;
    
    if(f1<f_up_thr){
      if(vibration){
        int difference = f_up_thr-f1;
        if (difference > maxDifference) {
          speed = (maxSpeedPercent*255)/100;
        } else {
          int speedPercent = map(difference, minDifference, maxDifference, minSpeedPercent, maxSpeedPercent);
          speed = (speedPercent*255)/100;
        }
        analogWrite(v_up, speed);
        analogWrite(v_down, 0);
      }
      lbl_up = 1;
    } else {
      if(vibration){
        analogWrite(v_up, 0);
      }
      lbl_up = 0;
    }

    if(f2<f_down_thr){
      if(vibration){
        int difference = f_down_thr-f2;
        if (difference > maxDifference) {
          speed = (maxSpeedPercent*255)/100;
        } else {
          int speedPercent = map(difference, minDifference, maxDifference, minSpeedPercent, maxSpeedPercent);
          speed = (speedPercent*255)/100;
        }
        analogWrite(v_up, 0);
        analogWrite(v_down, speed);
      }
      lbl_down = 1;
    } else {
      if(vibration){
        analogWrite(v_down, 0);
      }
      lbl_down = 0;
    }
    
    Serial.print(curr_seg,4);
    Serial.print(", ");
    Serial.print(f1);
    Serial.print(", ");
    Serial.print(f2);
    Serial.print(", ");
    Serial.print(lbl_up);
    Serial.print(", ");
    Serial.println(lbl_down);

    if (Serial.available() > 0) {
      char stopChar = Serial.read();
      if(stopChar == 's'){
        collectingData = false;
        Serial.print("Trial ");
        Serial.print(n_trials);
        Serial.println(" ended");
        if(vibration){
          analogWrite(v_up, 0);
          analogWrite(v_down, 0);
        }
      }
    }
  }
}

void upThreshold(){
  int32_t up_sum = 0;
  for(int i = 0; i < numSamples+100; i++){
    if (i>=100){
      int16_t up = analogRead(A0);
      up_sum += up;
    }
  }
  f_up_thr = up_sum / numSamples;

  Serial.print("Up threshold:");
  Serial.print("   f_up_thr = "); Serial.println(f_up_thr);
}

void downThreshold(){
  int32_t down_sum = 0;
  for(int i = 0; i < numSamples+100; i++){
    if (i>=100){
      int16_t down = analogRead(A1);
      down_sum += down;
    }
  }
  f_down_thr = down_sum / numSamples;

  Serial.print("Down threshold:");
  Serial.print("   f_down_thr = "); Serial.println(f_down_thr);
}

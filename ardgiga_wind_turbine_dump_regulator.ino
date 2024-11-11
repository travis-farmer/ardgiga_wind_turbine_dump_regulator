int ADC_input = 0; // analog input with a voltage divider (10K over 1K)
int PWM_output = 2; // active high, to drive a 100A SSR to connect the turbine output to a dump load
int Estop_input = 4; // short this pin to ground for Estop
int Estop_output = 3; // active high, to drive a 100A SSR to short the output of the turbine

int Estop_counter = 0;
unsigned long lastTim = 0UL;
float readVolts(int inPin, float LowR, float HighR) {
  return ((((3.3/ 1024) * analogRead(inPin)) / LowR) * (HighR + LowR));
}
void setup() {
  pinMode(Estop_input,INPUT_PULLUP);
  pinMode(Estop_output,OUTPUT);
  digitalWrite(Estop_output,LOW);

}

void loop() {
  unsigned long timer = millis();
  if (timer - lastTim >= 100) {
    lastTim = timer;
    float WindVolts = readVolts(ADC_input,1000.00,10000.00);

    if (WindVolts >= 18 && digitalRead(Estop_input) == HIGH) {
      digitalWrite(Estop_output,LOW);
      Estop_counter = 0;
      int PWMval = constrain((int)map(WindVolts,18.00,30.00,0,255),0,255);
      analogWrite(PWM_output,PWMval);
    } else if (digitalRead(Estop_input) == LOW) {
      if (Estop_counter <= 300) { // 300 counts, or 30 seconds to total Estop
        int ESpwm = constrain(map(Estop_counter,0,300,0,255),0,255);
        analogWrite(PWM_output,ESpwm);
        Estop_counter++;
      } else {
        digitalWrite(Estop_output,HIGH);
      }
    } else {
      digitalWrite(Estop_output,LOW);
      Estop_counter = 0;
      analogWrite(PWM_output,0);
    }
  }
}

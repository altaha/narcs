//int counter = 255;
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = A5;
const int ledPin =  13;
void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  pinMode(2, OUTPUT);   // digital sensor is on digital pin 2
  
}

void loop()
{
  analogWrite(analogOutPin, 125);
  byte start='c';
  int x=analogRead(analogInPin);
  byte byteArray[3];
  byteArray[0]=start;
  byteArray[1] = (byte)(x & 0xff);
  byteArray[2] = (byte)(x >> 8);
  
  byte MotorState;
  MotorState=Serial.read();
  Serial.write(byteArray,3);
  
  if(MotorState=='o')
     digitalWrite(ledPin, HIGH);
  else
      digitalWrite(ledPin, LOW);
}

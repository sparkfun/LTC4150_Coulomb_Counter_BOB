/*
VCC to VCC
GND to GND
INT to 2
POL to 4

Close SJ1 and SJ2

For 3.3V Arduinos, close SJ3 and SJ4 (on the bottom of the board)
For 5V Arduinos, open SJ3 and SJ4

*/


// 12140000 51mA 4.026
// 15120000 39mA

volatile boolean isrflag = false;
volatile long int time, lasttime;

volatile double percent = 100.0;
volatile double charge = 2000.0;

#define ah_quanta 0.0001707
#define percent_quanta 0.00853388

void setup()
{
  pinMode(2,INPUT);
  //digitalWrite(2,HIGH);
  pinMode(4,INPUT);
  pinMode(13,OUTPUT);

  Serial.begin(9600);
  Serial.println("hello.");

  isrflag = false;

  attachInterrupt(0,myISR,FALLING);
}

void loop()
{
  static int n = 0;
  
  if (isrflag)
  {
    isrflag = false;
    Serial.print("mah: ");
    Serial.print(charge);
    Serial.print(" soc: ");
    Serial.print(percent);
    Serial.print("% time: ");
    Serial.print((time-lasttime)/1000000.0);
    Serial.print("s ma: ");
    Serial.println(614.4/((time-lasttime)/1000000.0));
    lasttime = time;
  }
//  delay(500);
}

void myISR()
{
  static boolean polarity;
  
  isrflag = true;
  polarity = digitalRead(4);
  time = micros();
  if (polarity)
  {
    percent += ah_quanta;
    charge += percent_quanta;
  }
  else
  {
    percent -= ah_quanta;
    charge -= percent_quanta;
  }
}

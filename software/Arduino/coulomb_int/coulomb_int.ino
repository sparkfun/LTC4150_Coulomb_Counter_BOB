/*
Before connecting this board to your Arduino, double check that
the four solder jumpers are set appropriately.

SJ1 / SJ2 (top of board):

For this sketch, leave SJ1 and SJ2 closed (soldered).
This enables the board and clears interrupts automatically.

SJ3 / SJ4 (bottom of board):

Leave both SJ3 and SJ4 open (unsoldered) for 5V Arduinos.
Close (solder) both SJ3 and SJ4 for 3.3V Arduinos.

VIO to VCC
GND to GND
INT to 3
POL to 4

*/

#define VIO 2
#define INT 3 // On Uno and Pro, pins 2 and 3 support interrupts
#define POL 4
#define GND 5
#define CLR 6
#define SHDN 7

#define LED 13

volatile boolean isrflag = false;
volatile long int time, lasttime;

volatile double percent = 100.0;
volatile double charge = 2000.0;

#define ah_quanta 0.0001707
#define percent_quanta 0.00853388

void setup()
{
  pinMode(GND,OUTPUT);
  digitalWrite(GND,LOW);

  pinMode(VIO,OUTPUT);
  digitalWrite(VIO,HIGH);

  pinMode(SHDN,OUTPUT);
  digitalWrite(SHDN,HIGH);

  pinMode(CLR,OUTPUT);
  digitalWrite(CLR,HIGH);

  pinMode(POL,INPUT);

  pinMode(INT,INPUT);

  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);  

  Serial.begin(9600);
  Serial.println("hello.");

  isrflag = false;

  attachInterrupt(1,myISR,FALLING);
}

void loop()
{
  static int n = 0;
  
  if (isrflag)
  {
    isrflag = false;
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
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
  
  polarity = digitalRead(4);
  time = micros();
  isrflag = true;
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

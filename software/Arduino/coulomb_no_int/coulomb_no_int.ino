// 12140000 51mA 4.026
// 15120000 39mA

long int time, lasttime;

double percent = 100.0;
double charge = 2000.0;

#define VIO 2
#define INT 3 // On Uno and Pro, pins 2 and 3 support interrupts
#define POL 4
#define GND 5
#define CLR 6
#define SHDN 7

#define LED 13

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
}

void loop()
{
  static boolean polarity;

  if (digitalRead(INT)==0) // received a tick
  {
    time = micros();
    digitalWrite(LED,HIGH);
    digitalWrite(CLR,LOW);
    delay(100);
    digitalWrite(LED,LOW);
    digitalWrite(CLR,HIGH);
    polarity = digitalRead(POL);
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
    Serial.print("mah: ");
    Serial.print(charge);
    Serial.print(" soc: ");
    Serial.print(percent);
    Serial.print("% time: ");
    Serial.print((time-lasttime)/1000000.0);
    Serial.print(" s ma: ");
    Serial.println(614.4/((time-lasttime)/1000000.0));
    lasttime = time;
  }
}


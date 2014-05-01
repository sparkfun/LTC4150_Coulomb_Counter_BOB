// 12140000 51mA 4.026
// 15120000 39mA

long int time, lasttime;

double percent = 100.0;
double charge = 2000.0;

#define SHDN 2
#define CLR 3
#define GND 4
#define POL 5
#define INT 6
#define VIO 7
#define LED 13


#define ah_quanta 0.0001707
#define percent_quanta 0.00853388

void setup()
{
  pinMode(SHDN,OUTPUT);
  digitalWrite(SHDN,HIGH);
  pinMode(CLR,OUTPUT);
  digitalWrite(CLR,HIGH);
  pinMode(GND,OUTPUT);
  digitalWrite(GND,LOW);
  pinMode(POL,INPUT);
  pinMode(INT,INPUT);
  pinMode(VIO,OUTPUT);
  digitalWrite(VIO,HIGH);

  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);  

  Serial.begin(9600);
  Serial.println("hello.");
}

void loop()
{
  static boolean polarity;

  digitalWrite(CLR,HIGH);


  if (0) //(digitalRead(2)==0)
  {
    digitalWrite(13,HIGH);
    digitalWrite(5,LOW);
    delay(100);
    digitalWrite(13,LOW);
    digitalWrite(5,HIGH);
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
//  delay(500);
}


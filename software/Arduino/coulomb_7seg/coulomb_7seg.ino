/* LTC4150 Coulomb Counter 7-segment example code
Mike Grusin, SparkFun Electronics

This sketch uses the LTC4150 Coulomb Counter breakout
board to implement a battery "gas gauge" on a SparkFun
7-segment serial display.

HARDWARE CONNECTIONS:

Before connecting the Coulomb Counter to your Arduino, double
check that all three solder jumpers are set appropriately:

For this sketch, leave SJ1 soldered (closed).
This connects INT and CLR to clear interrupts automatically.

If you're using a 5V Arduino, leave both SJ2 and SJ3 open (unsoldered).

If you're using a 3.3V Arduino, close (solder) both SJ2 and SJ3.

Connect the following pins to your Arduino:

VIO to VCC
GND to GND
INT to D3
POL to D4

Note that if you solder headers to the bottom of the breakout board,
you can plug it directly into Arduino header pins D2 (VIO) through
D7 (SHDN).

LICENSE:

Our example code uses the "beerware" license. You can do anything
you like with this code. No really, anything. If you find it useful
and you meet one of us in person someday, consider buying us a beer.

Have fun! -Your friends at SparkFun.
*/

#include <SoftwareSerial.h>

SoftwareSerial Serial7Segment(7, 8); // RX pin, TX pin

// For this sketch you only need D3 and D4,
// but you can plug the board directly
// into the Arduino header (D2-D7) for convenience.

// (If you are not plugging the board directly into the
// header, you can remove all references to VIO, GND,
// CLR and SHDN.)

#define VIO 2 // Just used for the HIGH reference voltage
#define INT 3 // On 328 Arduinos, only pins 2 and 3 support interrupts
#define POL 4 // Polarity signal
#define GND 5 // Just used for the LOW reference voltage
#define CLR 6 // Unneeded in this sketch, set to input (hi-Z)
#define SHDN 7 // Unneeded in this sketch, set to input (hi-Z)

#define LED 13 // Standard Arduino LED

// Change the following two lines to match your battery
// and its initial state-of-charge:

volatile double battery_mAh = 2000.0; // milliamp-hours (mAh)
volatile double battery_percent = 100.0;  // state-of-charge (percent)

// Global variables ("volatile" means the interrupt can
// change them behind the scenes):

volatile boolean isrflag;
volatile long int time, lasttime;
volatile double mA;
double ah_quanta = 0.17067759; // mAh for each INT
double percent_quanta; // calculate below

void setup()
{
  // Set up I/O pins:
  
  pinMode(GND,OUTPUT); // Make this pin LOW for "ground"
  digitalWrite(GND,LOW);

  pinMode(VIO,OUTPUT); // Make this pin HIGH for logic reference
  digitalWrite(VIO,HIGH);

  pinMode(INT,INPUT); // Interrupt input pin (must be D2 or D3)

  pinMode(POL,INPUT); // Polarity input pin

  pinMode(CLR,INPUT); // Unneeded, disabled by setting to input
  
  pinMode(SHDN,INPUT); // Unneeded, disabled by setting to input

  pinMode(LED,OUTPUT); // Standard Arduino status LED
  digitalWrite(LED,LOW);  

  // Enable serial output:

  Serial.begin(9600);
  Serial.println("LTC4150 Coulomb Counter BOB interrupt example");

  // Set up serial 7-segment display

  Serial7Segment.begin(9600); // 9600 bps, TX on pin 8
  Serial7Segment.write('v');  // Reset the display

  Serial7Segment.write(0x77); // Turn on the middle decimal point
  Serial7Segment.write(0x02);

  // One INT is this many percent of battery capacity:
  
  percent_quanta = 1.0/(battery_mAh/1000.0*5859.0/100.0);

  // Enable active-low interrupts on D3 (INT1) to function myISR().
  // On 328 Arduinos, you may also use D2 (INT0), change '1' to '0'. 

  isrflag = false;
  attachInterrupt(1,myISR,FALLING);
}

void loop()
{
  char tempString[10]; // Used for sprintf
  static int n = 0;

  // When we detect an INT signal, the myISR() function
  // will automatically run. myISR() sets isrflag to TRUE
  // so that we know that something happened.

  if (isrflag)
  {
    // Reset the flag to false so we only do this once per INT
    
    isrflag = false;

    // Blink the LED

    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);

    // Print out current status (variables set by myISR())

//    Serial.print("mAh: ");
//    Serial.print(battery_mAh);
//    Serial.print(" soc: ");
//    Serial.print(battery_percent);
//    Serial.print("% time: ");
//    Serial.print((time-lasttime)/1000000.0);
//    Serial.print("s mA: ");
//    Serial.println(mA);
    Serial7Segment.write(0x79); // Cursor to left
    Serial7Segment.write((byte)0);
//    Serial7Segment.write(0x77); // Turn on the middle decimal point
//    Serial7Segment.write(0x02);
    sprintf(tempString, "%4d", int(battery_percent*100.0)); //Convert deciSecond into a string that is right adjusted
    Serial7Segment.print(tempString); //Send serial string out the soft serial port to the S7S
  }
  
}

void myISR() // Run automatically for falling edge on D3 (INT1)
{
  static boolean polarity;
  
  // Determine delay since last interrupt (for mA calculation)
  // Note that first interrupt will be incorrect (no previous time!)

  lasttime = time;
  time = micros();

  // Get polarity value 

  polarity = digitalRead(POL);
  if (polarity) // high = charging
  {
    battery_mAh += ah_quanta;
    battery_percent += percent_quanta;
  }
  else // low = discharging
  {
    battery_mAh -= ah_quanta;
    battery_percent -= percent_quanta;
  }

  // Calculate mA from time delay (optional)

  mA = 614.4/((time-lasttime)/1000000.0);

  // If charging, we'll set mA negative (optional)
  
  if (polarity) mA = mA * -1.0;
  
  // Set isrflag so main loop knows an interrupt occurred
  
  isrflag = true;
}

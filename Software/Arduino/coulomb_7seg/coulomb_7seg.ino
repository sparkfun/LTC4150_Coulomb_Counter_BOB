/* LTC4150 Coulomb Counter 7-segment example code
Mike Grusin, SparkFun Electronics

This sketch uses the LTC4150 Coulomb Counter breakout
board to implement a battery "gas gauge" on a SparkFun
7-segment serial display. (We tried it on the OpenSegment Shield
but it should work on any similar display.) It will display the
percentage left on a battery based on how much power you're
pulling out of it.

The sketch assumes that you are starting with a full battery,
NOTE that you should change line 85 to enter the capacity
of the battery you are using.

COULOMB COUNTER HARDWARE CONNECTIONS:

Before connecting the Coulomb Counter to your Arduino, double
check that all three solder jumpers are set appropriately:

For this sketch, leave SJ1 soldered (closed).
This connects INT and CLR to clear interrupts automatically.

If you're using a 5V Arduino, leave both SJ2 and SJ3 open (unsoldered).

If you're using a 3.3V Arduino, close (solder) both SJ2 and SJ3.

Connect the following Coulomb Counter pins to your Arduino:

VIO to VCC
GND to GND
INT to D3
POL to D4

Note that if you solder headers to the breakout board,
you can plug it directly into Arduino header pins
D2 (VIO) through D7 (SHDN).

7-SEGMENT HARDWARE CONNECTIONS:

Connect the following serial 7-segment display pins to your Arduino:

VCC to VCC
GND to GND
RXI to D8

LICENSE:

Our example code uses the "beerware" license. You can do anything
you like with this code. No really, anything. If you find it useful
and you meet one of us in person someday, consider buying us a beer.

Have fun! -Your friends at SparkFun.
*/

// We'll use the SoftwareSerial library to talk to the serial
// 7-segment display. TX on pin 8 is compatible with the 
// 7 Segment Shield.

#include <SoftwareSerial.h>
SoftwareSerial Serial7Segment(7, 8); // RX pin, TX pin

// I/O pin definitions

// For this sketch we only need D3 and D4,
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

#define LED 13 // Standard Arduino LED (optional)
#define BUZZ 11 // Buzzer on pin 11 (optional)

// Change the following two lines to match YOUR battery
// and its initial state-of-charge:

volatile double battery_mAh = 110.0; // milliamp-hours (mAh)
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

  pinMode(LED,OUTPUT); // LED (optional)
  digitalWrite(LED,LOW);

  pinMode(BUZZ,OUTPUT); // Buzzer (optional)
  digitalWrite(BUZZ,LOW);

  // Set up serial 7-segment display

  Serial7Segment.begin(9600); // 9600 bps, TX on pin 8
  Serial7Segment.write('v');  // Reset the display

  // One INT is this many percent of battery capacity:
  
  percent_quanta = 1.0/(battery_mAh/1000.0*5859.0/100.0);

  // Enable active-low interrupts on D3 (INT1) to function myISR().
  // On 328 Arduinos, you may also use D2 (INT0), change '1' to '0'. 

  isrflag = false;
  attachInterrupt(1,myISR,FALLING);
}

void loop()
{
  // If an interrupt occurs (tick from the Coulomb Counter),
  // it will set isrflag to true to let us know that something
  // changed.

  if (isrflag)
  {
    // Clear the flag (so we only run this once per int)
    isrflag = false;
    
    // Send the updated percent value to 7-segment display
    print7SegFloat(battery_percent);

    // Blink LED and buzzer (optional)
    digitalWrite(LED,HIGH);
    tone(BUZZ,2000);

    delay(100);

    digitalWrite(LED,LOW);
    noTone(BUZZ);
  }
  
  // You can put anything else you want in the main loop,
  // just be sure you check for isrflag at least every
  // half-second (which is the highest frequency you'll see
  // at the 1A maximum current through the Coulomb Counter).
}

void myISR()// Run automatically for falling edge on D3 (INT1)
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

void print7SegFloat(double val) // Display a floating point value
// Send a floating-point value to a 7-segment display
// Properly handle 4 significant digits with up to 2 decimal points
// and negative numbers including minus sign
{
  char tempstring[6];

  // Convert number to string with two decimal points
  // ("100.00" -> "10000")
  sprintf(tempstring,"%4ld",long(val*100.0));

  // Truncate to leftmost 4 characters
  tempstring[4] = 0;

  // Move cursor to leftmost position
  // (Prevents sync problems if you don't send 4 chars to display)
  Serial7Segment.write(0x79);
  Serial7Segment.write((byte)0);

  // Handle overflow condition if > 4 significant digits
  if (val > 9999.0 || val < -999.0)
  {
    Serial7Segment.print(" OF "); // Display "OF" for overflow
    Serial7Segment.write(0x77); // Turn off decimal point
    Serial7Segment.write((byte)0);
  }
  else
    // Send 4-character string to display
    Serial7Segment.print(tempstring); // Send string out the softserial port

  // Set correct decimal point: two decimal places
  if ((val >= 0 && val < 100.0) || (val < 0 && val > -10.0))
  {
    Serial7Segment.write(0x77);
    Serial7Segment.write((byte)2); // 2nd digit
    return;
  }

  // One decimal place:
  if ((val >= 0 && val < 1000.0) || (val < 0 && val > -100.0))
  {
    Serial7Segment.write(0x77);
    Serial7Segment.write((byte)4); // 3rd digit
    return;
  }

  // No decimal places:
  Serial7Segment.write(0x77); // Turn off decimal point
  Serial7Segment.write((byte)0);
  return;
}


/* LTC4150 Coulomb Counter interrupt example code
Mike Grusin, SparkFun Electronics

This sketch shows how to use the LTC4150 Coulomb Counter breakout
board along with interrupts to implement a battery "gas gauge."

Product page: https://www.sparkfun.com/products/12052
Software repository: https://github.com/sparkfun/LTC4150_Coulomb_Counter_BOB

HOW IT WORKS:

Battery capacity is measured in amp-hours (Ah). For example, a one
amp-hour battery can provide 1 amp of current for one hour, or 2 amps
for half an hour, or half an amp for two hours, etc.

The LTC4150 monitors current passing into or out of a battery.
It has an output called INT (interrupt) that will pulse low every
time 0.0001707 amp-hours passes through the part. Or to put it
another way, the INT signal will pulse 5859 times for one amp-hour.

If you hook up a full 1Ah (1000mAh) battery to the LTC4150, you
can expect to get 5859 pulses before it's depleted. If you keep track
of these pulses, you can accurately determine the remaining battery
capacity.

There is also a POL (polarity) signal coming out of the LTC4150.
When you detect a pulse, you can check the POL signal to see whether
current is moving into or out of the battery. If POL is low, current is
coming out of the battery (discharging). If POL is high, current is
going into the battery (charging).

(Note that because of chemical inefficiencies, it takes a bit more current
to charge a battery than you will eventually get out of it. This sketch
does not take this into account. For better accuracy you might provide
a method to "zero" a full battery, either automatically or manually.)

Although it isn't the primary function of the part, you can also
measure the time between pulses to calculate current draw. At 1A
(the maximum allowed), INT will pulse every 0.6144 seconds, or
1.6275 Hz. Note that for low currents, pulses will be many seconds
apart, so don't expect frequent updates.

There are two methods you can use to keep track of the INT pulses. You
can use an interrupt input to monitor the INT signal in the background,
or you can monitor the INT line yourself and use the CLR signal to reset
the LTC4150 for the next pulse.

** This sketch shows how to use interrupts with the LTC4150. **

HARDWARE CONNECTIONS:

Before connecting this board to your Arduino, double check that
all three solder jumpers are set appropriately:

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

RUNNING THE SKETCH:

This sketch monitors current moving into and out of a battery.
Whenever it detects a low INT signal from the LTC4150, it will
update the battery state-of-charge (how full the battery is),
current draw, etc.

The sketch is hardcoded for a 2000mAh battery that is 100% full
when the sketch starts. You can easily change this by editing
line 120 and 121:

  volatile double battery_mAh = 2000.0; // milliamp-hours (mAh)
  volatile double battery_percent = 100.0;  // state-of-charge (percent)

After uploading the sketch, open the Serial Monitor and set the
baud rate to 9600. Whenever the sketch detects an INT pulse, it
will update its calculations and print them out.

LICENSE:

Our example code uses the "beerware" license. You can do anything
you like with this code. No really, anything. If you find it useful
and you meet one of us in person someday, consider buying us a beer.

Have fun! -Your friends at SparkFun.
*/

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

  // One INT is this many percent of battery capacity:
  
  percent_quanta = 1.0/(battery_mAh/1000.0*5859.0/100.0);

  // Enable active-low interrupts on D3 (INT1) to function myISR().
  // On 328 Arduinos, you may also use D2 (INT0), change '1' to '0'. 

  isrflag = false;
  attachInterrupt(1,myISR,FALLING);
}

void loop()
{
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

    Serial.print("mAh: ");
    Serial.print(battery_mAh);
    Serial.print(" soc: ");
    Serial.print(battery_percent);
    Serial.print("% time: ");
    Serial.print((time-lasttime)/1000000.0);
    Serial.print("s mA: ");
    Serial.println(mA);
  }

  // You can run your own code in the main loop()
  // myISR() will automatically update information
  // as it needs to, and set isrflag to let you know
  // that something has changed.
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

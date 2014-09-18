LTC4150_Coulomb_Counter_BOB
===========================

*Breakout board for the Linear Tech LTC4150 Coulomb Counter (bidirectional current sensor / battery gauge)*

* Product page: [https://www.sparkfun.com/products/12052](https://www.sparkfun.com/products/12052)
* Hookup guide: [https://learn.sparkfun.com/tutorials/ltc4150-coulomb-counter-hookup-guide](https://learn.sparkfun.com/tutorials/ltc4150-coulomb-counter-hookup-guide)
* Datasheet: [https://cdn.sparkfun.com/datasheets/BreakoutBoards/LTC4150fc.pdf](https://cdn.sparkfun.com/datasheets/BreakoutBoards/LTC4150fc.pdf)
* Schematic: [https://cdn.sparkfun.com/datasheets/BreakoutBoards/LTC4150_BOB_v10.pdf](https://cdn.sparkfun.com/datasheets/BreakoutBoards/LTC4150_BOB_v10.pdf)

###What it does:

The LTC4150 Coulomb Counter monitors current passing through it (from the IN header or JST connector to the OUT header). Each time 0.1707 mAh passes through the device, the INT pin will pulse low. Or to put it another way, the INT pin will pulse low 5859 times for each Ah that passes through it.

There is also a POL pin that you can check each time INT goes low. POL will be L for current going from IN to OUT, and H for current going from OUT to IN (battery charging).

If you count these pulses and add or subtract from a total depending on the POL signal, you can keep accurate track of the state of a battery connected to the board.

The Coulomb Counter can accommodate power supplies up to 8.5V and up to 1A.

###Quickstart (Arduino):

* Configure the jumpers:

    If you're connecting the Coulomb Counter to a 5V system, leave solder jumpers SJ2 and SJ3 (on the bottom of the board) open (the default).

    If you're connecting the Coulomb Counter to a 3.3V system, close both SJ2 and SJ3.

* Hook up the I/O lines:

    Connect VIO to your system's VCC (3.3V or 5V) line.

    Connect GND to your system's ground line.

    Connect INT to your system's interrupt input (D3)

    Connect POL to an I/O pin (D4)

* Connect the IN header or the JST connector to your power source (up to 8.5V)

* Connect the OUT header to your system's power input

* Upload and run the Coulomb_Interrupt example code from this repository, and open a Serial Monitor set to 9600 bps. It should display a hello message and begin giving you periodic updates about the battery state. (See the comments for instructions on customizing it for your exact battery.)

See the [Hookup Guide](https://learn.sparkfun.com/tutorials/ltc4150-coulomb-counter-hookup-guide) for much more information.

Have fun! -Your friends at SparkFun

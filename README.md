# LIFI-Tx-Rx
A simple LIFI system with Arduino and ESP32

Transmitter System
• Converts analog signals to digital using an ADC.
• Converts parallel data to serial using a shift register.
• Modulates the digital signal into light using an LED.
Receiver System
• Converts light signals back to electrical using a phototransistor.
• Reconstructs digital signals using a comparator.
• Decodes serial data into parallel form for display.Design of the system
• Clock Generator
A 555 timer is configured in astable mode to provide the required clock signal, The values of the
resistance and capacitance can be calculated based on the equations given below -
We have replaced Rb with a potentiometer of 10K and use a resistance of 220 ohms for Ra and a capacitance
of 1uF which gives a Frequency of 130 Hz to 3.2 KHz and maximum tH of 7ms and maximum tL of 0.15 ms
• Frequency Divider
For dividing the frequency we use IC 4060 which a counter using D flip flops internally. We need a clock
16 times slower than the 555 timer clock . The ADC will work using the clk/16 whereas the rest of the circuit
will continue to use the 555 timer clock
• Analog-to-Digital Conversion
The ADC0820 is utilized to convert analog signals into an 8-bit digital format. The WR pin triggers the
conversion, and the converted value becomes available at the falling edge of WR. The INT output provides apulse indicating the completion of conversion, which is inverted and used as the SH/LD signal for the shift
register.
We used the above circuit with input as the voltage of a potentiometer connected to Vcc and ground.
The clock/16 from the 4060 counter is provided into the ADC.
The conversion of the input can be noticed by connecting LEDs on DB 0-7 and varying the pot to see the LEDs
change in a upcounter manner.
• Data Serialization with Shift Registers
Two 74HC165 shift registers are used in cascade to serialize the parallel data from the ADC. This
arrangement allows handling 8-bit data along with start (0) and stop (1) bits for UART format.
The SH/LD input is triggered by the inverted INT signal from the ADC. CLK input shifts the data serially for
transmission. The serial output from the first shift register feeds into the second shift register.
A clock signal, CLK2, shifts the data at a frequency 10 times that of CLK1. This clock signal is generated using
a 4060 counter circuit. Counter output is used as WR input of the ADC. A conversion is started at each rising
edge of WR, and converted value is available at the falling edge of WR (and, at this time INT has a positive
pulse). The high level of WR must be long enough to let time to the data word of the shift register (+ start
and stop bits, then 10 bits) to be shifted
For SH/LD we can use the INT output of the ADC, but it has to be inverted, as SH/LD needs to be a
negative pulse.
The not gate is implemented using a 222 TransistorWe used the above circuit to complete the conversion of parallel data from the ADC to serial data in UART
format. The first bit will be the start bit and following 8 bits are the data from ADC and the 10th bit is the stop
bit, the remaning 6 bits are all 1s to adhere to the UART protocol.
To following the UART protocol and send the start bit first the connections of the above circuit need to
be altered as follows – SER on 2nd shift register must be grounded and Qh must be connected to the SER of
1st register, the Qh of the 1st register will be used for transmitting the data.
• Light Transmission
The serialized digital signal is transmitted using a LED. A resistor is calculated based on the LED's
datasheet to ensure optimal forward current for efficient light emission.
There are 2 options here to use a Visible Light LED or Infrared LED, Operating the system in Visible
light can cause issues in transmission due to noise from the ambient lighting
Whereas usage of IR LED offers tolerance to noise but the components are costlier and harder to
setup
• Light Reception and Conversion
We have utilized a IR LED for transmitting the data, hence we used OPL550 which is IR receiver. Both
the IR LED and IR Receiver are connected to powersupply along with a 220 ohm resistorTests and Results
Unit Tests
1. Clock Generator Testing
o Verified the 555 timer clock and 4060 counter clock/16 outputs using LEDs
2. ADC Testing
o Verified the conversion of analog voltage into an 8-bit digital format by varying the
potentiometer and observing the 8 bit output on LEDs
3. Shift Register Testing, IR LED Transmitter and IR Receiver
o Verified the serial output for given parallel inputs by using LogiPort probes and Logiport
softwareLIFI using Arduino
The system uses an Arduino Uno (TX) to read temperature data, convert it based on a toggle switch
(Celsius, Fahrenheit, or Kelvin), and transmit the value with a unit identifier (C, F, or K). An ESP32 (RX)
receives the data, verifies it, and displays the temperature and unit on an LCD.Arduino transmitter code)

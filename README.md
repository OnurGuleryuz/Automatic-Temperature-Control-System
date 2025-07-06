# Automatic-AirConditioner
The aim of this project is to control the temperature in an enclosed environment.
Heating is achieved using a Ptc Heater, while cooling is performed with a Peltier module. Additionally, a fan is used to circulate the air in the environment.
Relays are used to drive both the heater and the cooler.

The current and target temperatures are displayed on an LCD screen. The target temperature can be increased or decreased using push buttons.

The system continuously compares the current temperature with the target temperature and activates either the heating or cooling process accordingly. Once the target temperature is reached, the system stops itself.

There is also a blinking LED to indicate that the system is operating, and a reset button is included to restart the system. A potentiometer is used to adjust the contrast of the LCD screen.

Components
MSP430G2553 MCU,
Lcd Screen,
Push Button,
LED,
Resistor (220,390,1k,10k),
10k Potentiometer,
Capacitor (0.1uF,0.33uF,1uF Polar),
1N4001 Diode,
Relay,
2N2222 Transistor,
LM35DZ Temperature Sensor,
LM317T Regulator,
L7805CV Regulator,
Connector 1x2,
12V 5A Adaptor (For Power Supply),

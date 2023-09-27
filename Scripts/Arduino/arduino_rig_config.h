/*	Header of a behaviour board configuration and peripherals connected to it for an experimental rig.
 *  Have one configuration file per rig, unless same peripherals are connected to same positions in all boards AND all boards are identical.
 *  RIG: --REPLACE THIS WITH RIG NAME/NUMBER--
 *  # file should be placed in: 'C:\voyeur_rig_config\'
 * */
#ifndef RIGCONFIG_H
#define RIGCONFIG_H

#define LED_PIN   13   // Front panel green LED beside the USB port

// Pulse generator trigger pins (internal to behaviour box). Although 3 exist, only 1 and 2 have pulse outputs with current firmware.
// TRIGGER1 can be accessed if BNC between USB port and green LED is jumpered in the 'PGTRIG1' position
// Used to trigger a pulse, via the pulsegenerator microcontroller, on the PG1/PG2 output channels (BNC ports)
#define PULSETRIG1   4
#define PULSETRIG2   5
#define PULSETRIG3   6

// Valve solenoid pins. Connecting to the IDC header at the back of the behaviour box
// and into the terminal breakout box via ribbon cable.
#define SOLENOID1 29
#define SOLENOID2 28
#define SOLENOID3 27
#define SOLENOID4 26
#define SOLENOID5 25
#define SOLENOID6 24
#define SOLENOID7 23
#define SOLENOID8 22

// BEAM break sensor pins. Used to detect phototransistor state when a beam break circuit is connected to respective RJ9 port
// Can be used for detecting licks when a DC lick circuit is connnected to the RJ9 port, with the output of the circuit
// connecting to one of these pins. RJ9 port should be configured with the BEAM BREAK jumpers.
#define BEAM1     37
#define BEAM2     36
#define BEAM3     35
#define BEAM4     34
#define BEAM5     33
#define BEAM6     32
#define BEAM7     31
#define BEAM8     30

// CUE pins on the RJ9 ports. Used for turning CUEs, such as LEDs or Buzzers, ON/OFF.
// RJ9 port should be configured with jumpers on the CUE positions.
#define CUE1      45
#define CUE2      44
#define CUE3      43
#define CUE4      42
#define CUE5      41
#define CUE6      40
#define CUE7      39
#define CUE8      38

// Pins that connect the analog-to-digital and digital-to-analog converter chips to the Arduino.
#define ADC_PIN   49
#define DAC1_PIN  53
#define DAC2_PIN  48
// Pin that connects to the teensy microcontroller when installed in the behaviour board and jumpered properly.
#define TEENSY_PIN 47

// Digital Pins/Channels accessed at the BNC ports, when the BNC ports are configured in the digital mode via jumpers.
#define DIGITAL1  62
#define DIGITAL2  63
#define DIGITAL3  64
#define DIGITAL4  65
#define DIGITAL5  66
#define DIGITAL6  67
#define DIGITAL7  68
#define DIGITAL8  69
// Digital Pins connecting to the IDC header and subsequently to a terminal board via ribbon cable.
#define DIGITAL9  54
#define DIGITAL10 55
#define DIGITAL11 56
#define DIGITAL12 57
#define DIGITAL13 58
#define DIGITAL14 59
#define DIGITAL15 60
#define DIGITAL16 61

// Labels of digital output trigger pins (TTL) for debugging. Connect to BNC ports when the BNCs are configured as digital channels.
#define SNIFF_T DIGITAL6
#define FV_T DIGITAL5
#define LEDMASK_T DIGITAL7
#define LASER_T DIGITAL3

// Valve solenoids. Connect to terminal breakout board.
#define FINALVALVE SOLENOID8
#define WATERVALVE1 SOLENOID1
#define WATERVALVE2 SOLENOID2
#define WATERVALVE3 SOLENOID4
#define WATERVALVE4 SOLENOID5
#define CLEARVALVE SOLENOID7

// Digital pins found in the IDC header, which is connected to a breakout board. Access at breakout board.
#define STARTTRIAL DIGITAL10
#define LEDMASK DIGITAL11

#endif

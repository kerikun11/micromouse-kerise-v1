EESchema Schematic File Version 2
LIBS:74xgxx
LIBS:74xx
LIBS:ac-dc
LIBS:actel
LIBS:adc-dac
LIBS:Altera
LIBS:analog_devices
LIBS:analog_switches
LIBS:atmel
LIBS:audio
LIBS:brooktre
LIBS:cmos_ieee
LIBS:cmos4000
LIBS:conn
LIBS:connectors
LIBS:contrib
LIBS:cypress
LIBS:dc-dc
LIBS:device
LIBS:digital-audio
LIBS:diode
LIBS:display
LIBS:dsp
LIBS:elec-unifil
LIBS:ESD_Protection
LIBS:ftdi
LIBS:gennum
LIBS:graphic
LIBS:hc11
LIBS:ic-cpu
LIBS:ic-io
LIBS:ic-misc
LIBS:ic-power
LIBS:intel
LIBS:interface
LIBS:ir
LIBS:Lattice
LIBS:linear
LIBS:logo
LIBS:maxim
LIBS:memory
LIBS:microchip
LIBS:microchip_dspic33dsc
LIBS:microchip_pic10mcu
LIBS:microchip_pic12mcu
LIBS:microchip_pic16mcu
LIBS:microchip_pic18mcu
LIBS:microchip_pic32mcu
LIBS:microcontrollers
LIBS:motor_drivers
LIBS:motorola
LIBS:msp430
LIBS:nordicsemi
LIBS:nxp_armmcu
LIBS:onsemi
LIBS:opto
LIBS:Oscillators
LIBS:passive
LIBS:philips
LIBS:power
LIBS:Power_Management
LIBS:powerint
LIBS:pspice
LIBS:references
LIBS:regul
LIBS:relays
LIBS:rfcom
LIBS:sensors
LIBS:silabs
LIBS:siliconi
LIBS:stm8
LIBS:stm32
LIBS:supertex
LIBS:switches
LIBS:texas
LIBS:transf
LIBS:transistors
LIBS:ttl_ieee
LIBS:valves
LIBS:video
LIBS:Xicor
LIBS:xilinx
LIBS:Zilog
LIBS:KERISEv1
LIBS:kerikun11
LIBS:KERISEv1-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 13
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Q_NMOS_GSD Q1
U 1 1 57CF03B8
P 5800 4800
AR Path="/57CEFE78/57CF03B8" Ref="Q1"  Part="1" 
AR Path="/57CF2D36/57CF03B8" Ref="Q2"  Part="1" 
F 0 "Q1" H 6100 4850 50  0000 R CNN
F 1 "IRFML8244" H 6450 4750 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 6000 4900 50  0001 C CNN
F 3 "" H 5800 4800 50  0000 C CNN
	1    5800 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 4800 5100 4800
$Comp
L GND #PWR54
U 1 1 57CF03C0
P 5900 5100
AR Path="/57CEFE78/57CF03C0" Ref="#PWR54"  Part="1" 
AR Path="/57CF2D36/57CF03C0" Ref="#PWR76"  Part="1" 
F 0 "#PWR54" H 5900 4850 50  0001 C CNN
F 1 "GND" H 5900 4950 50  0000 C CNN
F 2 "" H 5900 5100 50  0000 C CNN
F 3 "" H 5900 5100 50  0000 C CNN
	1    5900 5100
	1    0    0    -1  
$EndComp
$Comp
L LED D3
U 1 1 57CF03C6
P 5900 3500
AR Path="/57CEFE78/57CF03C6" Ref="D3"  Part="1" 
AR Path="/57CF2D36/57CF03C6" Ref="D9"  Part="1" 
F 0 "D3" H 5900 3600 50  0000 C CNN
F 1 "VSLY5850" H 5950 3350 50  0000 C CNN
F 2 "kerikun11:LED-5MM_Angle" H 5900 3500 50  0001 C CNN
F 3 "" H 5900 3500 50  0000 C CNN
	1    5900 3500
	0    -1   -1   0   
$EndComp
$Comp
L +BATT #PWR53
U 1 1 57CF03CD
P 5900 2700
AR Path="/57CEFE78/57CF03CD" Ref="#PWR53"  Part="1" 
AR Path="/57CF2D36/57CF03CD" Ref="#PWR75"  Part="1" 
F 0 "#PWR53" H 5900 2550 50  0001 C CNN
F 1 "+BATT" H 5900 2840 50  0000 C CNN
F 2 "" H 5900 2700 50  0000 C CNN
F 3 "" H 5900 2700 50  0000 C CNN
	1    5900 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 2700 5900 2800
$Comp
L R R7
U 1 1 57CF03D4
P 5350 4800
AR Path="/57CEFE78/57CF03D4" Ref="R7"  Part="1" 
AR Path="/57CF2D36/57CF03D4" Ref="R17"  Part="1" 
F 0 "R7" V 5430 4800 50  0000 C CNN
F 1 "10" V 5350 4800 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5280 4800 50  0001 C CNN
F 3 "" H 5350 4800 50  0000 C CNN
	1    5350 4800
	0    1    1    0   
$EndComp
Wire Wire Line
	5600 4800 5500 4800
Text HLabel 5100 4800 0    60   Input ~ 0
IR_LED
Wire Wire Line
	5900 5100 5900 5000
$Comp
L LED D2
U 1 1 57CF03DE
P 5900 3000
AR Path="/57CEFE78/57CF03DE" Ref="D2"  Part="1" 
AR Path="/57CF2D36/57CF03DE" Ref="D8"  Part="1" 
F 0 "D2" H 5900 3100 50  0000 C CNN
F 1 "VSLY5850" H 5950 2850 50  0000 C CNN
F 2 "kerikun11:LED-5MM_Angle" H 5900 3000 50  0001 C CNN
F 3 "" H 5900 3000 50  0000 C CNN
	1    5900 3000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5900 3200 5900 3300
$Comp
L NSI45090 U7
U 1 1 57CF03E6
P 5900 4100
AR Path="/57CEFE78/57CF03E6" Ref="U7"  Part="1" 
AR Path="/57CF2D36/57CF03E6" Ref="U9"  Part="1" 
F 0 "U7" H 5600 4350 60  0000 C CNN
F 1 "NSI45090" H 5450 4250 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:TO-252-2Lead" H 5850 4600 100 0001 C CNN
F 3 "" H 5850 4600 100 0000 C CNN
	1    5900 4100
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 57CF03ED
P 5650 4500
AR Path="/57CEFE78/57CF03ED" Ref="R6"  Part="1" 
AR Path="/57CF2D36/57CF03ED" Ref="R16"  Part="1" 
F 0 "R6" V 5730 4500 50  0000 C CNN
F 1 "100" V 5650 4500 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 5580 4500 50  0001 C CNN
F 3 "" H 5650 4500 50  0000 C CNN
	1    5650 4500
	0    1    1    0   
$EndComp
Wire Wire Line
	5900 4400 5900 4600
Connection ~ 5900 4500
Wire Wire Line
	5900 3700 5900 3800
Wire Wire Line
	5800 4500 5900 4500
Wire Wire Line
	5600 4100 5400 4100
Wire Wire Line
	5400 4100 5400 4500
Wire Wire Line
	5400 4500 5500 4500
Text Label 5900 3250 0    60   ~ 0
IR_LED_1
Text Label 5900 3750 0    60   ~ 0
IR_LED_2
Text Label 5900 4500 0    60   ~ 0
IR_LED_3
Text Notes 4700 2600 0    100  ~ 0
IR LED
$EndSCHEMATC

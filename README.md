[Embedded System](https://corsi.unige.it/en/off.f/2022/ins/59432?codcla=10635)<br>
**Programmer(s):** [Ankur Kohli](https://github.com/ankurkohli007), [Ammar Iqbal](https://github.com/ammariqbal48) & [Basit Akram](https://github.com/abdulbasit656)<br>
[M.Sc Robotics Engineering](https://corsi.unige.it/corsi/10635)<br>
[University of Genoa (UniGe)](https://unige.it/en)<br>
**Supervisor:** [Prof. Enrico Simetti](https://rubrica.unige.it/personale/UkNGW15g)

## Abstract ##

This assignemnt report is about the Embedded Systems in which the of timers, interrupts, SPI, UART, parser, ADC, PWM, Scheduling, and so on to determine about the operations performance. The specific goal in this case is that to know the implementation of operations with the real- time hardware. Also, case study of a real time control system. This report to bring the light on Embedded Systems for operations. Further- more, the purpose of this report is to provide the approaches used during the development of code and implementation on microcontroller board. For this assignment, MPLAB IDE Software, XC16 Compiler and Microchip Microcontroller Board, HTerm serial software is used and also Embed- ded C programming is platform for the development of code.

## Introduction ##

A microprocessor-based computer system with software that is intended to carry out a specific task, either independently or as a component of a larger system, is known as an embedded system. An integrated circuit built to perform computing for real-time processes is at the heart of the system.

From a single microcontroller to a group of connected processors with networks and peripherals, complexity can range from having no user interface to having intricate graphical user interfaces. Depending on the task for which it is created, an embedded system’s complexity varies greatly.

Applications for embedded systems include hybrid cars, avionics, digital watches, microwaves, and more. Embedded systems consume up to 98 percent of all produced microprocessors.

## UART Protocol used in this assignment ##

* The PC sends $MCREF,RPM*, where RPM is a value going from 0 to 1000. The references can come as fast as the baudrate allows. Example: $MCREF,400*
* The micro sends $MCFBK,CURRENT,TEMP*, where CURRENT is the current value in Amperes and TEMP is the temperature value at 1 Hz. Example: $MCFBK,4.3,22.2*

## Requirements for the Assignment ##

1. execute a control loop at 200 Hz.
2. the reference speed for the motor is received through UART communications. Setup bau- drate at 9600 bps.
3. set the voltage to the armature of the DC motor using PWM2H. Assume that the motor spins at 1000 RPM at 5V.
4. simulate an analog current sensor using AN2 (potentiometer). The sensor outputs 3 V at 0 A, and has a scale of 10 A/V.
5. send the current and temperature (AN3) feedback through UART at 1 Hz.
6. Whenever button S6 is pressed, clear the first row and reset the characters received counter.
7. blink D3 at 1 Hz to show that the program is running correctly.
8. turn on D4 whenever the current exceeds 15A.


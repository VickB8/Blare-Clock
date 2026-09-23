# BLARE CLOCK

A Small custom desk clock powered by a XIAO ESP32-C3 with a color display and Wi-Fi time sync.

![image alt](https://github.com/VickB8/Blare-Clock/blob/main/BLARE%20CLOCK/Images/final%20output.png?raw=true)

# Inspiration

I wanted to make a small desk clock that was more interesting than a normal digital clock I decided to design the PCB, Case and firmware myself use an ESP32-C3 so the clock could connect to Wifi and automatically keep the synchronisation.
I also wanted the final clock to look like an actual finished product instead of just a developement board connected to a display.

# How it Works

The clock uses a Seeed Studio XIAO ESP32-C3 as the main microcontroller. When it starts up, it connects to Wi-Fi and gets the current time using NTP.

The time is then displayed on the display.

The electronics are mounted inside a custom 3D-printed case that I designed specifically for the PCB and display.

# Challenges

One of the biggest challenges was getting the display and XIAO ESP32-C3 working together. I had to figure out the correct pins and how to control the display.

I also had some problems with the KiCad libraries and footprints for the XIAO ESP32-C3. Getting the PCB and its 3D models to show correctly took some troubleshooting.

Another challenge was designing the case. I originally made the display opening too small and didn't leave enough room for the display wires and USB connection. I had to go back and modify the case so the display could actually be connected and the USB port could be accessed.

# What I Learned

How to design a PCB in KiCad
How to work with custom footprints and libraries
How to use an ESP32-C3 with Wi-Fi
How NTP time synchronization works
How to design a 3D-printed enclosure around electronics
How to troubleshoot PCB and 3D-model issues

# Components 

Seeed Studio XIAO ESP32-C3
TFT display
Custom PCB
Connecting wires
3D-printed enclosure
MX style keyboard switches
white blank dsa caps
3.3v piezo buzzer
M3x5x4 heat inserts M3x8mm screws

# Software

Arduino IDE
ESP32 Arduino Core
Adafruit GFX Library
Adafruit ST7789 Library
SPI
WiFi
Time / NTP

# Features

Wi-Fi time synchronization
NTP-based accurate time
Color display

# Design Files

PCB

![image alt](https://github.com/VickB8/Blare-Clock/blob/main/BLARE%20CLOCK/Images/pcb.png?raw=true)


The PCB was designed in KiCad and contains the connections between the XIAO ESP32-C3 and the display.

Schematic

![image alt](https://github.com/VickB8/Blare-Clock/blob/main/BLARE%20CLOCK/Images/schematic.png?raw=true)

Case

![image alt](https://github.com/VickB8/Blare-Clock/blob/main/BLARE%20CLOCK/Images/body%20view.png?raw=true)

![image alt](https://github.com/VickB8/Blare-Clock/blob/main/BLARE%20CLOCK/Images/body%20with%20pcb.png?raw=true)


The enclosure was designed to hold the electronics while leaving access for the display and USB port.

Firmware

The firmware is written in Arduino/C++ and handles Wi-Fi connection, NTP time synchronization, and drawing the clock interface on the display.

Final Result




This project helped me learn more about PCB design, embedded programming, displays, and designing physical enclosures for electronics. I started with the individual components and gradually worked toward making the complete clock.

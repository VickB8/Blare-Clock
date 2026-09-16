# Blare-Clock
This repository contains the design files, firmware, and documentation for a custom Wi-Fi synchronized desk clock built using the Seeed Studio XIAO ESP-C and an ST display. The project was created as part of the BLARE program and features automatic time synchronization via NTP, a custom display driver, and a 3D-printed case.
# BLARE Clock

A custom digital clock built using a **Seeed Studio XIAO ESP32-C3**, **ST7789 TFT display**, custom PCB, and a 3D-printed enclosure.

## Project Overview

This project is a compact Wi-Fi-enabled digital clock. The ESP32-C3 connects to Wi-Fi and retrieves the current time using an NTP time server. The time and date are displayed on an ST7789 TFT screen.

## Features

* Wi-Fi connectivity
* Automatic time synchronization using NTP
* Digital time display
* Date display
* Custom PCB
* Custom 3D-printed enclosure
* ST7789 TFT display
* Seeed Studio XIAO ESP32-C3

## Hardware

| Component       | Description                |
| --------------- | -------------------------- |
| Microcontroller | Seeed Studio XIAO ESP32-C3 |
| Display         | ST7789 284 × 76 TFT        |
| PCB             | Custom-designed PCB        |
| Enclosure       | Custom 3D-printed case     |
| Firmware        | Arduino / ESP32            |
| Connectivity    | Wi-Fi                      |


## Firmware

The firmware is written using the Arduino IDE and runs on the XIAO ESP32-C3.

The firmware:

1. Initializes the ST7789 display.
2. Connects to Wi-Fi.
3. Synchronizes the clock using an NTP server.
4. Retrieves the local date and time.
5. Displays the time and date on the TFT.

### Libraries

* Adafruit GFX Library
* Adafruit ST7735 and ST7789 Library
* SPI
* WiFi
* ESP32 time functions

## Bill of Materials

| Component                   |                  Quantity |
| --------------------------- | ------------------------: |
| Seeed Studio XIAO ESP32-C3  |                         1 |
| ST7789 284 × 76 TFT Display |                         1 |
| Custom PCB                  |                         1 |
| 3D-printed enclosure        |                         1 |
| Other electronic components | As specified in schematic |

## Project Structure

```text
BLARE-Clock/
├── CAD/
│   └── Clock.step
│
├── PCB/
│   ├── clock.kicad_pro
│   ├── clock.kicad_sch
│   └── clock.kicad_pcb
│
├── Firmware/
│   └── clock.ino
│
├── Production/
│   ├── gerbers.zip
│   ├── Top.step
│   ├── Bottom.step
│
└── README.txt
```

## Tools Used

* KiCad — schematic and PCB design
* Arduino IDE — firmware development
* Blender / CAD software — enclosure design
* 3D printer — enclosure manufacturing

## Credits

This project was created as part of the **Hack Club BLARE** hardware project.

Built with:

* Seeed Studio XIAO ESP32-C3
* ST7789 TFT
* KiCad
* Arduino
* 3D printing

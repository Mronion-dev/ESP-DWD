## **NOTE THAT SOME OF THIS IS OUTDATED AND WAS A PART OF AN OLDER VERSION OF THE DRONE !!**
# DWDC — Double Wobble Drone

**DWDC (Double Wobble Drone Controller)** is an open-source budget drone project that includes:

- Semi-Complete drone-side code
- Serial monitor to arduino controller code `Deprecated`
- 3D printable files for the drone frame and mounting parts `Unfinished`
- (almost) All required design files to build the project from scratch `Unfinished`

The goal of this project is to provide a simple and affordable drone platform that anyone can build at home using commonly available components.

---
## Required Parts for the Drone
|Part|amount|
|----|------|
|ESP32|1|
|BLDCs|2|
|SG90 Servos|2|
|MPU9250|1|
|Step down Buck converter|1|

### Notes

- signal interference sucks, just manage your cables man
- capacitors are recommended (47uF 10V+) but not needed 

### Optional

- A custom 3D-printed shield or enclosure can be designed to mount the microcontroller and radio to your controller.
- If you create a good case that might be cheaper to print or doesn't need printing, consider uploading it

---

## Features

- Low-cost design using world-wide hobby components, ~$52 in Lebanon
- Stabilization support through an I2C gyroscope/IMU
- Fully 3D-printable structure

---

## Project Goal

The purpose of DWDC is to make drone building accessible to hobbyists, students, and makers without breaking the bank on some 500 dollar drone that doesn't even come with a controller.

---

## License and Usage

These files are provided for **personal and educational use only**.

> **Commercial use is strictly prohibited without prior written permission from the author.**

If you wish to use any part of this project commercially, contact me, MO-D, for permission

---

## Contributing

- Suggestions, improvements, and custom 3D designs are welcome. If you develop better parts, enclosures, or software enhancements, consider sharing them so others can benefit.
- Donations unfortunately wont be available due to the shitty country I live in.
- Discord link: https://discord.gg/kGYJnFMWCF

---

## AI Usage

AI was not, and will not be used, on this project. Any submissions that include fully AI generated content that has no prior human alterations and inspections will not be **ALLOWED**.

---

## Future Goals

- Become a plausible competitor against other Drone brands
- Include GPS Stablization and path finding
- Improve GUI for new users
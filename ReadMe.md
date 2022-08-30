# DMD Clock

## Introduction
inspired by https://go-dmd.de

I liked the idea to use cheap LED matrixes to build a 128x32 Pixel DMD as in a pinball.
To display the time - and some sequences from pinball machines

I did not liked the expensive hardware and closed source.

This project uses an ESP32 ($5) + 2 x 64x32 matrix ($15 each), a SD-card reader ($2) done.

Put on the card animation clips, produced with free software.

Project is work in process.
For testing it uses today a little 3.5" display, later to be replaced with DMD.

## Open jobs:
- add wifi connector (to get current time)
- add web interface (for setup)
- add a switch to use real dmd or display
- display time
- interface for color/config of time
- add tetris routine to display time, optionally
- documentation

## libraries

# Hardware
best to use a ttgo-t8 as ESP32.
https://github.com/LilyGO/TTGO-T8-ESP32
It includes a SD-card reader on board, avoids adding that yourself, reduces number of needed connections.
Price is around $10 (Aliexpress), so a little bit more expensive than plain ESP32+card reader.

Pixel Matrix: I used one with P2.5, fitting the size of a real pinball DMD.
Chose P3 or bigger if you want bigger display.
Chose one with HUB-75D (1/16 scan, pins A-D used, pin E should be unused)

### ideas found at:
https://github.com/SuperMakeSomething/mini-video-player

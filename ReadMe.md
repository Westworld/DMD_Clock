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


### clips
format SD card as Fat32
Create folder "clips"

use DMDlibrary.ini, set [Video] to true and add path to folder
Play your games, try to play as long as possible with all rewards such as multiball, extraball, etc.

The folder will contain Romname.avi with the full game recording.
Now you need to cut that in parts.
On Mac I use VLC to convert the avi to m4v.
Start VLC, use File/Convert, select recording, select profil "Video - H.264+MP3" and save as file.
Open the converted file with QuickTime Player.
Scroll near (a little bit before) to begin of scene, click marker. 
Use Edit/split clip.
Move to end of scene (a little bit behind), click marker, use Edit/split clip
delete begin/end, to keep only scene.
Now select exact begin/end, split clip, then save the final scene with a name such as
ACDC_Multiball.m4v
Return to full video, find all scenes
Finally, we need to convert all clips to .mjpeg.
I use FFMPEG for that, executing in terminal on Mac:

for i in /Users/thomas/ownCloud/Flipper/dmdrecordings/clips_m4v/*.m4v; do /Users/thomas/Documents/ffmpeg/ffmpeg -i "$i" -vf "fps=30,scale=128:32:flags=lanczos" -q:v 9 "${i%.*}.mjpeg"; done


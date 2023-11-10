# DMD Clock

## Introduction
inspired by https://go-dmd.de

I liked the idea to use cheap LED matrixes to build a 128x32 Pixel DMD as in a pinball.
To display the time - and some sequences from pinball machines

I did not liked the expensive hardware and closed source.

This project uses an ESP32 ($5) + 2 x 64x32 matrix ($15 each), a SD-card reader ($2) done.

Put on the card animation clips, produced with free software.

Project is work in process.


# Hardware to purchase
best to use a ttgo-t8 as ESP32.
https://github.com/LilyGO/TTGO-T8-ESP32
It includes a SD-card reader on board, avoids adding that yourself, reduces number of needed connections.
Price is around $10 (Aliexpress), so a little bit more expensive than plain ESP32+card reader.
Update (Nov 2022): prices seems to went crazy, it might be way cheaper to use an external card reader

Pixel Matrix: I used one with P2.5, fitting the size of a real pinball DMD.
Chose P3 or bigger if you want bigger display.
Chose one with HUB-75D (1/16 scan, pins A-D used, pin E should be unused)

Power Supply, 5V, 2-3 Ah.
For the matrix you normally need a 4 Ah supply, but as never all leds are on, it never takes as much.
I measured for a while and never saw even 2 Ah.
You can use an old notebook supply (if it delivers 5V) or use a 3 Ah 5V USB power supply, as used for Raspberry Pi.

Use anything to build your case. I used 19mm glued beech wood, as I had some remaining parts.

Build tips below

# Firmware
I recommend to use Platformio/Visual Code
As build target, select "ttgo-t1_DMD" (even if you are using another ESP32, does not matter)
There are two other builds, both with 'TFT' in the name, these are for prototyping, using a 2.4" TFT, make development possible in living room without power supply, etc.

Modify platformio.ini for "ttgo-t1_DMD" if needed.

if the build flags contains:
    '-Dwebdebug=1'  
    
Then I missed to remove that, sorry. Please remove it!
It sends debug infos via serial port and UDP (network to 192.168.0.34), which you don't need.

Compile and upload to your ESP.

To upload first time, modify platformio.ini:

upload_protocol = espota
upload_port = 192.168.0.139   ;   /dev/cu.usbserial*

to:
;upload_protocol = espota
;upload_port = 192.168.0.139   ;   /dev/cu.usbserial*

As long as you have only one ESP32 connected, it should find the port automatically.
For future updates, you might use Over the Air update, then you need to modify the IP address with the address of the ESP.
This avoid to remove it from a case and makes updating easy.

# First Software Setup
When you first launch (with DMD connected), it will display "DMD Clock" and then try to connect to your Wifi.
For first launch, this will fail, so it will launch its own Wifi network,  named "DMD Clock" - no password.
Connect to this Wifi using a Smartphone or Notebook, this will open a browser window where you can select your Wifi SSID and enter your password. Then it will restart.

After the restart (and following launches), it will display below "DMD Clock" for some seconds the IP address used.
Now you can use a browser to connect to this IP for detailed setup, see below.

# SD Card setup
Format SD card as Fat32
Copy from this repository the content of folder "SDCard" on your card, so it has the folders "clips", "fonts" and "TimeZones" on top level.
You can delete clips or fonts if you don't like them.
After you launched the clock, top level will contain text files used for caching (faster launch)
If you modify anything inside the folders (add or remove files), delete this text files, forcing a refresh of the cache.
See "clips" below how to create your own clips

# Hardware setup
I used a prototype board, soldered two 8 pin long pinhead sockets (as shipped with Wemos D1), close together, used as Hub75 connector.
These were soldered on one side of the board.
On the other side I soldered two pinhead sockets fitting to the size of the ESP32.
Then used normal cables and connected the required pins together.

That's not a hard job, you just need to check twice that you connect the right pins, as both sides of the board is used.

Finally I used a socket block to connect the power cable to the matrix and the external power supply. The socket blocks allows easily to connect/disconnect.
And I added a jumper (using a 2 pin header + jumper) to connect 5V from external power supply to the ESP - or cut it.
Then i need to connect a computer via USB to update/debug, I remove the jumper, supplying power to the ESP from the computer, but feeding Matrix from external supply.
With the jumper the external power supply feeds both ESP + matrix.

Pictures:
![Connector board 1](/documentation/IMG_3103.jpeg)
![Connector board 2](/documentation/IMG_3104.jpeg)

When you solder, make sure to remember that the pins of the Hub75 might be needed to be seen from reverse (backside), to connect the right pins.
It could help - to be sure - to solder only one and then measure if you connected the right one, when you plug it in.

Pictures for Hub75 and ESP (but that depends from the ESP32 you chosed)
![Hub75](/documentation/Hub75E.jpg)
![ESP32](/documentation/T81.jpg)

# Case example
I used 19mm glued beech wood.
Based on the rest pieces I had, I chosed the case to be 8 cm deep.
So I had one long piece with 8 cm.
I cut that in 4 parts. 2 small ones. Both 8 cm, as my DMD matrix is 160x80 mm.
The wide ones are 160 + 19 + 19.
I rounded the corners with a router and destroyed, so needed to redo :-)
Picture of the 4 pieces (before redo):
![wood pieces](/documentation/IMG_3099.jpeg)

Then I used a 3D printer to print a holder for the panels. 
The holder is a combination of 3 parts, so is small enough for standard printers.
It has on both sides holes to screw from the inside with the wood:
![Front panel](/documentation/IMG_3098.jpeg)

This is how it looks mounted from the inside. On top/bottom you can see the holes for the screws:
![front from inside](/documentation/IMG_3102.jpeg)
![front from inside2](/documentation/IMG_3106.jpeg)

The picture above also shows a 3D printed cover for the back side.
Mounted:
![front from back](/documentation/IMG_3102.jpeg)

The back cover also has holes for screwing to the wood cover. When you print, the holes might be not free, depending on your screws you might drill 2-3 mm to open those.

https://www.tinkercad.com/things/5EdWpAjOED1-pinball-dmd

### ideas found at:
https://go-dmd.de
https://github.com/SuperMakeSomething/mini-video-player


### clips
format SD card as Fat32
Create folder "clips"

best to use raw video clips, in RGB format (24 bit per pixel), with extension .rgb
Also supported: MJPEG (but reduced quality).

To record in raw/RGB:

use DMDlibrary.ini, set [Video] to true and add path to folder
Play your games, try to play as long as possible with all rewards such as multiball, extraball, etc.

The folder will contain Romname.avi with the full game recording.

Now you need to find a software able to convert that avi to raw format and to cut it...

to just convert:
ffmpeg -i acd_170c.avi -vcodec rawvideo -pix_fmt rgb24 test.rgb

to convert a folder with many files
for i in *.gif; do ffmpeg -i "$i" -format rawvideo -pix_fmt rgb24  "${i%.*}.rgb"; done  



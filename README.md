# ShieldDemo1

## Exercise for "WeMOS" shield on D1 R32

Banggood sells this shield with the usual breathless hype.
I admit it was an impulse buy. I will never admit how much time I spent writing this hack job and getting it (sort of) working.

It merely tries out most of the junk on the shield.

ShieldDemo1.ino is, of course, the sketch itself.
D1R32pins.h defines the gpio --> data mapping used on the D1 R32 board.

## Some comments on the hardware.

WeMOS (really? Does LOLIN claim this? They don't seem to document it.) D1 R32 is an ESP32 board in Arduino (Uno) format. 
As always with cards like this, some liberties had to be taken. Here is a rough pinout:
|--------------|---|
|              |SDA|
|              |SCL|
|              |Reset|
|00            |GND|
|5V            |15|
|RST           |19|
|3V3           |23|
|5V            |05|
|GND           |13|
|GND           |12|
|Vin           |  |
|              |14|
|02            |27|
|04            |16|
|36            |17|
|34            |25|
|36            |26|
|39            |TX0|
|              |RX0|
              
So, if I may be permitted to opine, Uno pinout/layout is goofy and this thing tries to be useful whilst cleaving to it.

the shield is labelled with Uno-style Dnumbers, which is the reason for D1R32pins.h's existence.
(That this works implies that A0 is gpio02, A1 is gpio04, and A2 is gpio36 -- odd.)

On my sample LM35 released magic smoke (the footpring is made for an un-bent TO92 package and tends to solder bridge.)
I pulled it and omitted that section of the code.

Neither do I use the buzzer on "D5".

Have fun!

As always, comments, questions, issues, corrections, suggestions welcome.

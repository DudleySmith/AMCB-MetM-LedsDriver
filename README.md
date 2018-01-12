# AMCB-MetM-LedsDriver

### Content :
##### Arduino Sketches : 
* _AMCB-OscReceiverAndLeds-Simple :_ Arduino sketch managing OSC, Leds (Working, THIS IS THE MAIN SKETCH)
* _AMCB-OscReceiverAndLeds :_ Arduino sketch managing OSC, Leds and storage of the default values (not working, not in production)
* _Simple-Osc :_ OSC library (required with feather, add it in your document/arduino/librairies folder). 
  ** The point is that ArdOsc lib consider you have an ethernet or standard wifi shield ( = basic arduino). So, I use the UDP to receive the message and the rest of the lib to _decode_ it. It would be not necessary with Esp2866, because you can find more libs for the Esp8266 wifi chip

##### Other : 
* _AMCB - Pcb.fzz :_ electronical schematic (Fritzing file : download fritzing here)
* _Box Design.skp :_ Sketchup file for the boxes (mainly for holes)
* _TesUDPSend 3.vzr :_ use it to test the feather boards


### Notes
Arduino used are Feather M0 : https://www.adafruit.com/product/3010

#### OSC Messages awaited :
* _/animation/color/red, /animation/color/green, /animation/color/blue_ : to set the color
* _/animation/brightness_ : to set the main brightness

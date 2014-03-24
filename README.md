Fake Linux On Arduino
===

This is in the preliminary phases.

**Libraries required:**

* USB - for interacting with the keyboard
* SD - for SD card file management
* TVout - controlling the interface

The code is driven around the `KbdRptParser` class, which automatically runs the `OnKeyPressed` function. To adapt for use with a PS/2 keyboard, remove the `KbdRptParser` class except fot the `onKeyPressed` and call the `onKeyPressed` function.

I submitted this code at the [Arduino Stackexchange Project Showcase](http://arduino.stackexchange.com/a/887/8). I'll see how it does.

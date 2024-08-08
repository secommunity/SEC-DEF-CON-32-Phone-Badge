# Hacking Your Social Engineering Community Badge - 2024
## Programming Tips
1. Use the [Arduino IDE](https://www.arduino.cc/en/software) (Recommend version 2+)
2. Add an additional boards manager URL in File -> Preferences:
```
https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json
```
3. Install the **minicore** boards (by MCUdude) into Arduino using the **Boards Manager**
4. Install the **buttonmatrix** library (by Rene Richter) into Arduino using the **Library Manager**
5. Install the **Adafruit MCP23017** (by Adafruit) library into Arduino using the **Library Manager**
6. Use the following settings for programming your badge:
- Board: Minicore - "ATMEGA328"
- Clock: Internal 8MHz
- Variant: 328PB
7. If you are programming using the tools in the Social Engineering Community Village:
- Port: COM Port of the USB Programmer
- Programmer: Arduino as ISP
- **Remove the batteries from your badge! Or disconnect the USB port for power**
- Press the pogo pins of the programmer to the programming pins on the badge. Pin 6 (Black)
should be touching the square pad. Hold during upload, which takes about 10-20 seconds.
- **Use Sketch -> Upload Using Programmer (Not the "play button" to program the board)**
## Challenge Ideas
- Beginner
  
Make your badge LEDs alternate between blue and red when you type "911"
- Experienced
  
When you type your own 10 (or more!) digit phone number, make your badge light up two different
LEDs, two different colors at the same time. Can you do more than two?

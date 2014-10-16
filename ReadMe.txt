Files in this directory:

data.html
/xPico/xPico.cpp
/xPico/xPico.h
/xPico/example/xPico.ino

SAMPLE SETUP
In xPico Web Manager:
1) Create an /http directory on your device.
2) Copy data.html into it.
3) Set Line 1 Protocol to Mux.
4) Set Line 1 Baud Rate to 115200

Arduino:
1) Copy the xPico directory to the Arduino IDE library folder
2) Compile and upload xPico.ino to Arduino Leonardo. No support for other Arduinos yet.

Computer:
1) Connect to the xPico AP and point your web browser to 192.168.0.1
2) You should see in your web browser:

The data from the device is:
Hello World: XX

with the XX replaced by a random number updating every ~250ms.

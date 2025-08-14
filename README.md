# SilentCatFlap
My cat used to get startled by the loud click the flap made whenever his microchip was detected.
To solve this, I replaced the original motor with a 28BYJ-48 stepper motor that moves smoothly and quietly.

The stock motor would drive against a stop at -3 V to open and +3 V to close. In the first phase of the project, I kept as much of the existing cat flap electronics as possible, building a standalone controller that listens for those +3 V and -3 V signals and uses them to drive the stepper motor.

The controller is powered by an M5 Atom Lite (about $10), which requires an external USB‑C power adapter. The stepper motor itself needs minor modifications — swapping the connector and removing a few protruding tabs.

From the accompanying ULN2003 driver board, I only use the driver IC, which I desolder and mount onto a custom PCB (see Fritzing layout). I also designed several custom parts for 3D printing (see .stl files). The build additionally uses a 2 mm shaft and two screws from a shaft collar.

Finally, a few small adjustments to the original cat flap housing are necessary to ensure that all moving parts operate freely.

https://youtu.be/ug-eX6cvsHM

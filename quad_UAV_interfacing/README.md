# CprE_488_Quad_UAV_Interfacing

- In this project we perform software filtering where we implemented a hardware module that intervenes the signals coming from a remote control. The signals can be analyzed and harmful signals can be detected and filtered before giving them to the Quad
- The ZedBoard reads PPM signals from the remote control thorugh PMOD pins
- The hardware module designed in VHDL records the input signals usinag a Finite State Machine and then they are read in SK via GPIO
- The hardware module generates the PPM output signals using another State Machine so as to send the signals to the Quad
- The module can also save the signals and recreate the previous flights
- The buttons and switches are used to switch between the hardare relay mode and software mode and to record, rewind and replay the signals

Click on the image for the video


[![Watch the video](https://img.youtube.com/vi/BC8dgxj4jBE/hqdefault.jpg)](https://www.youtube.com/watch?v=BC8dgxj4jBE)

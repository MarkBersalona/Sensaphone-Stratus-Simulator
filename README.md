# Abstract
This is the Sensaphone Stratus Simulator, used to simulate a Stratus EMS by transmitting the same JSON strings, to test a Stratus Display. Stratus Simulator runs on a Raspberry Pi with a 480x320 touchscreen.
# Runs in Debian-based Linux too
Although intended for a SBC with a touchscreen display, Stratus Simulator can also be built and run on a Linux PC. I've tried it in Linux Mint, but it probably works in any Debian-based Linux desktop (I haven't tried it in Arch or Fedora.)
1. Install the GTK+3 developer library and JSON library
   - sudo apt-get update
   - sudo apt-get install libgtk-3-dev
   - sudo apt-get install libjson-glib-dev
2. Clone Sensaphone-Stratus-Simulator. If you're reading this, you're already likely in my GitHub project for Sensaphone-Stratus-Simulator, but if you're not it's at https://github.com/MarkBersalona/Sensaphone-Stratus-Simulator.git
3. In a terminal move to the directory in which the repository has been cloned. Example: on my Linux laptop I cloned it to /home/mark/GTKProjects/Sensaphone-Stratus-Simulator
4. From the terminal use the following command: `make build`
5. If all goes well, the application 'rmsp_simulator' will be in ../dist/Debug/GNU-Linux
6. Move or copy 'rmsp_simulator' back up to the Sensaphone-Stratus-Simulator directory, the one with RMSPSimulator.glade, theme.css and all the .png images.
   - Will need 2 USB-to-serial cables and a null modem to connect the 2 serial ends
   - Plug one USB end into the Linux PC, the other into Stratus Display
   - Run rmsp_simulator


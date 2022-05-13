# Mower-Arduino
Mower-Arduino and raspberry pi documentation.

Raspberry Pi setting up:

Install operative system 32-bit of Raspberry Pi on SD card.
Put the SD card in the Raspberry Pi.
Connect Raspberry Pi to a screen via HDMI cable.
Connect Raspberry Pi to power supply to power micro USB.
Connect micro USB to USB data port adapter to connect mouse, keyboard and the robot.

Arduino setting up:
Download Arduino IDE on PC.
Download makeblock library, and add it as a zip file to make block following the steps below: 
Open Arduino IDE Sketch → Include Library → add .ZIP Library and select the library of make block
To connect the robot to your pc, you connect with USB port on your pc and follow the steps below: 
Tools → Board → Ardunio Mega or Mega 2560
Tools → Port → Choose which port you connected the robot to

Camera Zero setting up:
Open Raspberry Pi terminal and run “sudo raspi-config”.
Select interface options and select legacy camera and enable it. 
Select Finish, and reboot the Raspberry Pi.
From the terminal again you need to write the following commands:
sudo apt-get update
sudo apt-get upgrade
pip3 install picamera
Import picamera in your code.

Bluetooth setting up:
Open preferences → Rasspberry Configuration → Interfaces → enable Serial Port → Reboot
After rebooting:
Open the terminal and write the following command:
  sudo nano /etc/systemd/system/dbus-org.bluez.service
    Add -C in the end of the following line
    ExecStart=/usr/lib/bluetooth/bluetoothd 
    Add the following line:
    ExecStartPost=/usr/bin/sdptool add SP
    Then save the file and exit, by pressing ctrl x then ctrl s. 
  sudo systemctl daemon-reload
  sudo systemctl restart bluetooth.service
  sudo pip3 install bluedot

import the following:
from bluedot.btcomm import BluetoothServer
from signal import pause




















#!/usr/bin/env python3
# This program will scan for available
# evdev devices on you computer and track events
# Evdev library must be installed.
# Install it using pip with the following command
# sudo pip install evdev
# For further informations look at :
# https://python-evdev.readthedocs.io/en/latest/install.html

from __future__ import print_function
import evdev

devices = [evdev.InputDevice(path) for path in evdev.list_devices()]

idevice = 0

for device in devices:
    print(idevice, device.name, "Path :", device.path, "phys", device.phys)
    idevice += 1

selectedTarget = False

while not selectedTarget:
    temp = raw_input("Select tracking target :")

    try:
        int(temp)
        selectedTarget = True
    except ValueError:
        print("Enter an integer !")

    if selectedTarget:
        target = int(temp)

for event in devices[target].read_loop():
    if event.type == evdev.events.EV_KEY:
        print("New EV_KEY \n", end="")
        print(" ", evdev.categorize(event))
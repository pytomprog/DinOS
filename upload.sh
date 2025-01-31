#!/bin/bash
scriptDir=$(dirname "$0")

sudo mount /dev/sda1 /media/usb
sudo cp $scriptDir/build/os.bin /media/usb/os.bin
sudo umount /media/usb
echo "Upload done !"

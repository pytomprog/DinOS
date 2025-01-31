#!/bin/bash
scriptDir=$(dirname "$0")

qemu-system-x86_64 -hda $scriptDir/build/os.bin -m 1m -nographic

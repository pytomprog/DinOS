#!/bin/bash
scriptDir=$(dirname "$0")

ndisasm -b 16 $scriptDir/build/mbr.bin > $scriptDir/build/mbr.disasm16
ndisasm -b 32 $scriptDir/build/mbr.bin > $scriptDir/build/mbr.disasm32
ndisasm -b 16 $scriptDir/build/kernel.bin > $scriptDir/build/kernel.disasm16
ndisasm -b 32 $scriptDir/build/kernel.bin > $scriptDir/build/kernel.disasm32
ndisasm -b 16 $scriptDir/build/os.bin > $scriptDir/build/os.disasm16
ndisasm -b 32 $scriptDir/build/os.bin > $scriptDir/build/os.disasm32

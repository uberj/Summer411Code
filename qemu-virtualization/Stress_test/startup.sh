#!/bin/bash
QEMU_BIN=/scratch/uberj/qemu-0.15.0/x86_64-softmmu/qemu-system-x86_64
KERNEL=/scratch/uberj/linux-3.0.y/arch/x86/boot/bzImage
HDD=/scratch/uberj/hdd.img
SSH_PORT=$1
GDB_PORT=$2
DEV_CONSOLE=stdio
VNC_PORT=$3

    $QEMU_BIN \
    -vnc :$VNC_PORT \
    -gdb tcp::$GDB_PORT \
    -kernel $KERNEL \
    -hda $HDD \
    -append "root=/dev/sda console=ttyS0" \
    -redir tcp:$SSH_PORT::22 \
    -serial $DEV_CONSOLE

#Note: Ping from the QEMU Guest is unreliable. Do not use ping to test connectivity from a QEMU Guest when the network model is "User Net".

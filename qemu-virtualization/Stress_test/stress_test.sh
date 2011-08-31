#!/bin/bash

num_vms=3
base_ssh=5010
base_gdb=6000
base_vnc=0

echo "Starting $num_vms VMs with info:"
for (( i=1; i <= $num_vms; i++ ))
do
    echo "SSH_PORT: $((base_ssh+i)) GDB_PORT: $((base_gdb+i)) VNC_PORT: $((base_vnc+i))" 
    ./startup.sh $((base_ssh+i)) $((base_gdb+i)) $((base_vnc+i)) > ./log/vm$i.log &
done
echo "Done."

echo "processes involving qemu"
pgrep qemu
echo "Kill all these processes? [Y/N]"
read answer
if [ $answer == "Y" ]
then
    for i in $(pgrep qemu)
    do
        kill $i
    done
fi

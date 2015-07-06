#!/bin/sh

modprobe rtnet
modprobe rtpacket
modprobe rt_fec rx_pool_size=1000
modprobe rtdup

rtifconfig rteth0 up <IP addr> promisc
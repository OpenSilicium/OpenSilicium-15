#!/bin/bash
set -x

##  Argument Validation

EXPECTED_ARGS=1
if [ $# == $EXPECTED_ARGS ]
then
	echo "Assuming Default Locations for Prebuilt Images"
	$0 $1 $PWD/tmp/deploy/images/MLO $PWD/tmp/deploy/images/u-boot.img $PWD/tmp/deploy/images/uImage $PWD/tmp/deploy/images/uImage-am335x-bone.dtb $PWD/tmp/deploy/images/core-image-basic-beaglebone.tar.gz
	exit
fi

if [[ -z $1 || -z $2 || -z $3 || -z $4 ]]
then
	echo "mkmmc-beaglebone.sh Usage:"
	echo "	mkmmc-beaglebone.sh <device> <MLO> <u-boot.img> <uImage> <uImage-am335x-bone.dtb> <rootfs tar.gz > <uEnv.txt>"
	echo "	Example: mkmmc-beaglebone.sh /dev/sdc MLO u-boot.img uImage uImage-am335x-bone.dtb rootfs.tar.gz uEnv.txt"
	exit
fi

if ! [[ -e $2 ]]
then
	echo "Incorrect MLO location!"
	exit
fi

if ! [[ -e $3 ]]
then
	echo "Incorrect u-boot.bin location!"
	exit
fi

if ! [[ -e $4 ]]
then
	echo "Incorrect uImage location!"
	exit
fi

if ! [[ -e $5 ]]
then
	echo "Incorrect boot.scr location!"
	exit
fi

if ! [[ -e $6 ]]
then
	echo "Incorrect rootfs location! "
	exit
fi

if ! [[ -e $7 ]]
then
	echo "Incorrect uEnv.txt location! "
	exit
fi

echo "All data on "$1" now will be destroyed! Continue? [y/n]"
read ans
if ! [ $ans == 'y' ]
then
	exit
fi

## Unmounting all partitioins

echo "[Unmounting all existing partitions on the device ]"
umount ${1}[1-4]

## Writing new partition table using sfdisk
 
echo "[Partitioning $1...]"
DRIVE=$1

dd if=/dev/zero of=$DRIVE bs=1024 count=1024
	 
SIZE=`fdisk -l $DRIVE | grep Disk | awk '{print $5}'`
	 
echo DISK SIZE - $SIZE bytes
 
CYLINDERS=`echo $SIZE/255/63/512 | bc`
 
echo CYLINDERS - $CYLINDERS
{ echo ,9,0x0C,*; echo ,,,-; } | sudo sfdisk -D -H 255 -S 63 -C $CYLINDERS $1 &> /dev/null



## Making New file systems

echo "[Making filesystems...]"

mkfs.vfat -F 32 -n boot "$1"1 &> /dev/null
mkfs.ext4 -L rootfs "$1"2 &> /dev/null


## Copying Contents

echo "[Copying files...]"

mount "$1"1 /mnt
cp $2 /mnt/MLO
cp $3 /mnt/u-boot.img
cp $4 $5 $7 /mnt

sync
umount "$1"1

mount -t ext4 "$1"2 /mnt

export PATH_TO_TMPFS_DIR=$PWD/tmp_fs
export PATH_TO_SDROOTFS=/mnt
echo "[Copying File System to rootfs partition]"

tar -x -f $6 --numeric-owner -C $PATH_TO_SDROOTFS &> /dev/null

sync
umount "$1"2


echo "[Done]"
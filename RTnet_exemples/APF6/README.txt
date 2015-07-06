1- Application patch Xeno3

$ cd xenomai-3.0-rc4
$ patch -p1 < <path>/xeno3.patch

2- Préparation noyau

$ ./scripts/prepare-kernel.sh --arch=arm --ipipe=./kernel/cobalt/arch/arm/patches/ipipe-core-3.16.7-arm-5.patch --linux=../linux-3.16.7/

3- Compilation noyau + installation modules:

$ . ~/bin/set_env_linaro_2014.sh
$ cd linux-3.16.17
$ cp <path>/apf6_linux_3.16.7_xeno3.config .config
$ make oldconfig
$ make

$ make INSTALL_MOD_PATH=<rootfs>/rootfs modules_install


4- Compilation Xenomai 3 (user)

$ ./configure --host=arm-linux-gnueabihf --with-core=cobalt --prefix=<path>/rootfs 
$ make
$ make install

-> rootfs contient alors les binaires Xenomai 3 -> à copier sur le rootfs de la cible (cp -a -r -f -v ...). Le rootfs est construit avec l'environnement 
Armadeus.

$ git clone git://git.code.sf.net/p/armadeus/code armadeus
$ cd armadeus
$ make apf6_defconfig
$ make


5- Compilation openPOWERLINK

$ cd <path>/openPOWERLINK-V2.1.1/stack/build/linux
$ ./build-stacks-arm.sh

$ cd <path>/openPOWERLINK-V2.1.1/apps/demo_cn_console/build/linux
$ ./build-app-arm.sh

$ cd <path>/openPOWERLINK-V2.1.1/apps/demo_mn_console/build/linux
$ ./build-app-arm.sh

6- Test

Lancer le script rtnet.sh sur la carte

# /root/rtnet.sh

Lancer l'appli CN ou MN

# ./demo_mn_console_arm 
----------------------------------------------------
openPOWERLINK console MN DEMO application
using openPOWERLINK Stack: V2.1.1
----------------------------------------------------
Initializing openPOWERLINK stack...
--------------------------------------------------
List of Ethernet cards found in this system: 
--------------------------------------------------
0. rteth0
--------------------------------------------------
Select the interface to be used for POWERLINK (0-0):0
--------------------------------------------------
Initializing process image...
Size of input process image: 4
Size of output process image: 4
oplk_allocProcessImage(): Alloc(4, 4)
oplk_allocProcessImage: Alloc(0x7c5950, 4, 0x7c5960, 4)

-------------------------------
Press Esc to leave the program
Press r to reset the node
-------------------------------

1970/01/01 00:05:38 - StateChangeEvent(0x19) originating event = 0x10 (NmtEventSwReset)
1970/01/01 00:05:38 - StateChangeEvent(0x29) originating event = 0x20 (NmtEventEnterResetApp)
...

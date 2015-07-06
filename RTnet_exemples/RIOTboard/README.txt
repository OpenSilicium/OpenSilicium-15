27/5/2014
=========

- Compilation uImage avec

$ LOADADDR=0x10008000 make -j5 uImage

- Test openPOWERLINK

Même compilation qu'avec APF6, mais erreur à l'exécution:

# ./demo_cn_console 
----------------------------------------------------
openPOWERLINK console CN DEMO application
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
open_live() error ioctl, can't find interface
rxThread() error open_live: 19
edrv_init() couldn't set rxthread scheduling parameters!
^Cedrv_init() Couldn't create tx-queue!
oplk_init() failed with "Initialisation error" (0x0013)
Stack is in state off ... Shutdown


-> voir dans stack/src/kernel/edrv/edrv-pcap_linux.c

pour raw-ethernet.c fonctionne et utilise le même appel système -> compiler
avec les mêmes bibliothèques dans le MEME ordre !!

28/5/2014
=========

- Création d'un programme ifindex.c effectuant le même ioctl() -> OK

ifindex (RTnet) -> pas de trace dans "strace"
ifindex.linux (Linux) -> trace dans "strace"

???

- Erreur sur la création de socket

    if ((pInstance->sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_POWERLINK))) < 0) { --> invalid argument -> ETH_P_POWERLINK  (ETH_P_ALL)

-> ajout de l'option dans Xenomai + recompilation -> OK

- Test CN/MN sur RIOTboar -> OK (erreur bizarre sur les queues ???)

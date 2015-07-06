CC=gcc

PROG=  libusb_brb 

OBJS= libusb_brb.o


all: $(PROG)

clean: 
	rm -f *~ Module.markers Modules.symvers *.o $(OBJS) $(PROG)

$(PROG) : $(OBJS)
	$(CC) -o $@ $(OBJS) -lusb

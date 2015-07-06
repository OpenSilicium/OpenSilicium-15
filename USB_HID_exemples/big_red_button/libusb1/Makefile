CC=gcc

PROG=  libusb_brb

OBJS= libusb_brb.o


all: $(PROG)

clean: 
	rm -f *~ $(OBJS) $(PROG)

$(PROG) : $(OBJS)
	$(CC) -o $@ $(OBJS) -lusb-1.0

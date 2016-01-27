CFLAGS = -c -fPIC -Wall
INC = include
OBJECTS = mixer.o pcm.o
LIB = libtinyalsa.so
CROSS_COMPILE =

all: $(LIB) tinyplay tinycap tinymix tinypcminfo

tinyplay: $(LIB) tinyplay.o
	$(CROSS_COMPILE)gcc -m32 tinyplay.o -L. -ltinyalsa -o tinyplay

tinycap: $(LIB) tinycap.o
	$(CROSS_COMPILE)gcc -m32 tinycap.o -L. -ltinyalsa -o tinycap

tinymix: $(LIB) tinymix.o
	$(CROSS_COMPILE)gcc -m32 tinymix.o -L. -ltinyalsa -o tinymix

tinypcminfo: $(LIB) tinypcminfo.o
	$(CROSS_COMPILE)gcc -m32 tinypcminfo.o -L. -ltinyalsa -o tinypcminfo

$(LIB): $(OBJECTS)
	$(CROSS_COMPILE)gcc -m32  -shared $(OBJECTS) -o $(LIB)

.c.o:
	$(CROSS_COMPILE)gcc -m32  $(CFLAGS) $< -I$(INC)

clean:
	-rm $(LIB) $(OBJECTS) tinyplay.o tinyplay tinycap.o tinycap \
	tinymix.o tinymix tinypcminfo.o tinypcminfo

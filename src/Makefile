
# vim: ft=make noexpandtab

INCDIRS=-I../../libvcol/src
CFLAGS=-Wall -DDEBUG $(INCDIRS)
SOURCES=graphics.c
OBJECTS=$(subst .c,.o,$(SOURCES))
OBJDIR=../obj/Debug-Linux-Allegro

MD=mkdir -p -v

all: ../bin/Debug-Linux-Allegro/libcamgraph.a

../bin/Debug-Linux-Allegro/libcamgraph.a: $(OBJECTS) graphics/allegrog.o
	$(MD) ../bin/Debug-Linux-Allegro
	$(AR) rcs $@ $(addprefix $(OBJDIR)/,$^)

%.o: %.c
	$(MD) $(OBJDIR)/$(dir $@)
	$(CC) -c -o $(OBJDIR)/$@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf ../obj; \
	rm -rf ../bin


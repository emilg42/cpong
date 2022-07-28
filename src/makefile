CC = gcc
TARGETS = pong
DESTDIR = /usr/local/bin
LIBS = -lm
override CFLAGS += -Wall

pong: pong.c config.h
	$(CC) $< $(CFLAGS) -o $@ $(LIBS)

install: pong
	cp $(TARGETS) $(DESTDIR)

clean:
	rm -rf $(TARGETS)
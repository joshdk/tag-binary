CC 	 	    = gcc
CFLAGS    = -std=c99
TARGET    = tag
SOURCE    = tag.c tfmanip.h res.h
INSTALL   = /usr/bin/

all: build

build:
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	-rm -f $(TARGET) *~
	-rm -f $(TARGET).zip

rebuild: clean build

install:
	cp -f $(TARGET) $(INSTALL)


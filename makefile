CC 	 	    = gcc
CFLAGS    = -std=c99
TARGET    = tag
SOURCE    = tag.c tfmanip.h res.h
INSTALL   = ~/Code/bin/

all: build

build:
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	-rm -f $(TARGET) *~
	-rm -f $(TARGET).zip

rebuild: clean build

install:
	-mkdir -p $(INSTALL)
	cp -f $(TARGET) $(INSTALL)

archive:
	zip -r $(TARGET).zip $(SOURCE) readme.*


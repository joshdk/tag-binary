CC 	 	    = gcc
CFLAGS    = -std=c99

SRC_DIR   = src
BUILD_DIR = build

TARGET    = $(BUILD_DIR)/tag

SOURCE    = $(SRC_DIR)/tag.c\
						$(SRC_DIR)/tfmanip.h\
						$(SRC_DIR)/res.h

INSTALL_DIR = /usr/bin/


all: build

build: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	-rm -f $(BUILD_DIR)/*
	-rm -f .tags

rebuild: clean build

install: build
	cp -f $(TARGET) $(INSTALL_DIR)



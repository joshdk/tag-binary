CC 	 	    = gcc
CFLAGS    = -std=c99 -Wall

SRC_DIR   = src
BUILD_DIR = build

TARGET    = tag

SOURCE    = $(SRC_DIR)/tag.c\
						$(SRC_DIR)/tfmanip.h\
						$(SRC_DIR)/res.h

INSTALL_DIR = /usr/bin/


all: build

build: $(TARGET)
	-mv $(BUILD_DIR)/$(TARGET) ./

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(SOURCE)

clean:
	-rm -f $(TARGET) $(BUILD_DIR)/*
	-rm -f .tags

rebuild: clean build

install: build
	cp -f $(TARGET) $(INSTALL_DIR)



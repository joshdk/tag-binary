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

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(SOURCE) $(SRC_DIR)/version.h
	./$(BUILD_DIR)/reversion
	-mv -fu $(BUILD_DIR)/$(TARGET) ./

clean:
	-rm -f $(TARGET) .tags

rebuild: clean build

install: build
	cp -f $(TARGET) $(INSTALL_DIR)



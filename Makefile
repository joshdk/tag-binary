CC        = gcc
CFLAGS    = -std=c99 -Wall -Wextra

SRC_DIR   = ./src/
BUILD_DIR = ./build/

TARGET    = ./tag

SOURCE    = $(SRC_DIR)tag.c\
            $(SRC_DIR)tfmanip.h\
            $(SRC_DIR)res.h

INSTALL_DIR = /usr/bin/

SUCCESS_MSG = "  [\e[32m DONE \e[0m]"

all: build

build: $(TARGET)

$(TARGET): $(SOURCE)
	@echo "Building tag:"
	@$(CC) $(CFLAGS) -o $(BUILD_DIR)$(TARGET) $(SOURCE) $(SRC_DIR)version.h
	@mv -fu $(BUILD_DIR)$(TARGET) ./
	@./$(BUILD_DIR)reversion
	@echo -e $(SUCCESS_MSG)

clean:
	@echo "Cleaning workspace:"
	@rm -f $(TARGET) .tags
	@echo -e $(SUCCESS_MSG)

rebuild: clean build

install: build
	@echo "Installing tag:"
	@cp -f $(TARGET) $(INSTALL_DIR)
	@echo -e $(SUCCESS_MSG)

uninstall:
	@echo "Uninstalling tag:"
	@rm -f $(INSTALL_DIR)$(TARGET)
	@echo -e $(SUCCESS_MSG)


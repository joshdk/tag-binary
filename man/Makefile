TARGET    = tag.1.gz
SOURCE    = tag.1
INSTALL_DIR = /usr/share/man/man1/
SUCCESS_MSG = "  [\e[32m DONE \e[0m]"

all: build

build: $(TARGET)

$(TARGET): $(SOURCE)
	@echo "Building tag manpage:"
	@gzip -c $(SOURCE) > $(TARGET)
	@echo -e $(SUCCESS_MSG)

clean:
	@echo "Cleaning workspace:"
	@rm -f $(TARGET)
	@echo -e $(SUCCESS_MSG)

rebuild: clean build

install: build
	@echo "Installing tag manpage:"
	@cp -f $(TARGET) $(INSTALL_DIR)
	@echo -e $(SUCCESS_MSG)

uninstall:
	@echo "Uninstalling tag manpage:"
	@rm -f $(INSTALL_DIR)$(TARGET)
	@echo -e $(SUCCESS_MSG)



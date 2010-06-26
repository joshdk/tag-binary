SRC_DIR   = src

TARGET    = tag

SOURCE    = ${SRC_DIR}/tag.c\
						${SRC_DIR}/tfmanip.h\
						${SRC_DIR}/res.h

INSTALL_DIR = /usr/bin/

CC 	 	    = gcc
CFLAGS    = -std=c99

all: build

build:
	${CC} ${CFLAGS} -o ${TARGET} ${SOURCE}

clean:
	-rm -f ${TARGET} *.o .tags

rebuild: clean build

install: build
	cp -f ${TARGET} ${INSTALL_DIR}


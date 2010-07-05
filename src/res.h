#ifndef RES_H
#define RES_H

#include <stdlib.h>
#include <string.h>

#define VERSION_MAJOR			1
#define VERSION_MINOR			2
#define VERSION_BUILD     1
//#define VERSION_REVISION  10

char *helptext="\
Usage: %s [OPTION] [+|-|:|.][TAGS]... FILE\n\
Tag or search for files by keywords\n\
\n\
  -d, --dump      dump tagfile data associated with FILE\n\
  -f, --find      find file(s) inside of directory FILE\n\
  -h, --help      print this message\n\
  -q, --query     list all of the tags associated with FILE\n\
  -t, --tag       add or remove tags from FILE\n\
  -v, --version   display version info\n";

char *versiontext="\
tag %d.%d.%d\n\
Copyright (C) 2010, Joshua Komoroske\n\
License BSD: New BSD License.\n\
\n\
Written by Josh Komoroske\n";


#endif

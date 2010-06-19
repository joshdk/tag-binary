#ifndef RES_H
#define RES_H

#include <stdlib.h>
#include <string.h>

#define VERSION_MAJOR			1
#define VERSION_MINOR			0
#define VERSION_BUILD     0
#define VERSION_REVISION  19

char *helptext="\
Usage: %s [OPTION] [TAGS]... FILE\n\
Tag or search for files by keywords\n\
\n\
  -t, --tag       add or remove tags from FILE\n\
  -f, --find      find file(s) inside of directory FILE\n\
  -h, --help      print this message\n\
  -q, --query     list all of the tags associated with FILE\n\
  -v, --version   display version info\n";

char *versiontext="\
tag %d.%d%s%d\n\
Written by Josh Komoroske\n";


#endif

#ifndef _RES_H_
#define _RES_H_

#include <stdlib.h>
#include <string.h>

#define VERSION_MAJOR			0
#define VERSION_MINOR			9
#define VERSION_BUILD     0
#define VERSION_REVISION  0

char *helptext="\
Usage: tag [OPTION] [TAGS]... FILE\n\
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

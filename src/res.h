#ifndef RES_H
#define RES_H

#include "version.h"


char *helptext="\
Usage: tag [option] [[prefix]tag]... target\n\
Tag or search for files by keywords\n\
\n\
Options:\n\
  -d, --dump      dump tagfile data associated with target\n\
  -f, --find      find file(s) inside of target directory\n\
  -h, --help      print this message\n\
  -q, --query     list all of the tags associated with target\n\
  -t, --tag       add or remove tags from target\n\
  -v, --version   display version info\n";

char *versiontext="\
tag %d.%d.%d\n\
Copyright (C) 2010, Joshua Komoroske\n\
License BSD: New BSD License.\n\
\n\
Written by Josh Komoroske\n";

#endif


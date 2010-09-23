#ifndef RES_H
#define RES_H

#include "version.h"

const char helptext[]="\
Usage: tag [option] [[prefix]tag]... target\n\
Tag or search for files by keywords\n\
\n\
Options:\n\
  -f              find file(s) inside of target directory\n\
  -q              list all of the tags associated with target\n\
  -t              add or remove tags from target\n\
      --help      print this message\n\
      --version   display version info\n";

const char versiontext[]="\
tag %d.%d.%d\n\
Copyright (C) 2010, Joshua Komoroske\n\
License BSD: New BSD License.\n";

#endif


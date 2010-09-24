#ifndef FILESYS_H
#define FILESYS_H

#include "tagdb.h"


int tag_tagfile(const char*,const char**,int);

//int dump_tagfile(const char *);


//int contains(const char *,const char *);
int extract_paths(const char *,char **,char **,char **);


#endif

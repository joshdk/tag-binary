#ifndef TAGDB_H
#define TAGDB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "version.h"
//#include "res.h"

#define NAME_BUFFER_SIZE     256
#define TAG_COUNT            8
#define TAG_BUFFER_SIZE      8

#define TAGFILE_FILENAME     ".tags"
#define TAGFILE_MAGIC        0x73676174 //"tags"

#define INVALID_OFFSET       -1
#define INVALID_SIZE         -1

#define DEFRAG_RATIO         0.750
#define DEFRAG_THRESHOLD     16


int lstat(const char *path, struct stat *buf);
int ftruncate(int,int);
int fileno(FILE *);

struct header{//8 bytes
	unsigned int magic;
	unsigned char major;
	unsigned char minor;
	unsigned short build;
};

struct table{//8 bytes
	unsigned int virt;
	unsigned int real;
};

struct rowinfo{//4 bytes
	unsigned short name_buffer_size;
	unsigned char tag_count;
	unsigned char tag_buffer_size;
};

struct row{
	char *name;
	char **tags;
};

struct tagbd{
	struct header *header;
	struct table *table;
	struct rowinfo *info;
	struct row *rows;
	int count;
};



struct row *create_rowdata(struct rowinfo *);
void destroy_rowdata(struct row *,struct rowinfo *);

int read_row(struct row *,struct rowinfo *,FILE *);
int write_row(struct row *,struct rowinfo *,FILE *);
int append_row(struct rowinfo *,FILE *);

int search_tagfile(const char *,const char **,int,FILE *);

int query_tagfile(const char *,FILE *);

int tag_row(const char*,const char**,int,struct rowinfo *,FILE *,int);



int defrag_tagfile(struct table *,struct rowinfo *,FILE *);


#endif

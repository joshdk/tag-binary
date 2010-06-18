#ifndef _TFMANIP_H_
#define _TFMANIP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME			"tag"

#define TAGFILE_FILENAME  ".tags"
#define TAGFILE_MAGIC			0x73676174 //"tags"

#define NAME_MAX 					255
#define NAME_BUFFER_SIZE 	256

#define TAG_COUNT 				16
#define TAG_BUFFER_SIZE 	16

#define ROW_BUFFER_SIZE		(NAME_BUFFER_SIZE+TAG_BUFFER_SIZE*TAG_COUNT)

#define INVALID_OFFSET 		-1
#define INVALID_SIZE			-1

#define TRUE 							1
#define FALSE							0

struct info{
	int header;
	unsigned char vera,verb,verc,verd;
};

struct table{
	int virt,real;
};

struct row{
	char name[NAME_BUFFER_SIZE+1],tags[TAG_COUNT][TAG_BUFFER_SIZE+1];
};


int read_row(struct row *,FILE *);
int search_tagfile(const char *,FILE *,char **,int);
int query_tagfile(const char *,FILE *);




int read_row(struct row *entry,FILE *ftags){
	fread(entry->name,sizeof(char),NAME_BUFFER_SIZE,ftags);
	for(int n=0;n<TAG_COUNT;++n){
		fread(entry->tags[n],sizeof(char),TAG_BUFFER_SIZE,ftags);
	}
	return 1;
}



int search_tagfile(const char *path,FILE *ftags,char **tags,int tagc){

	struct info i;
	char name[NAME_BUFFER_SIZE+1],tag[TAG_BUFFER_SIZE+1];
	

	fread(&i,sizeof(struct info),1,ftags);
	//printf("Header: [%x]\n",tf->t_info.header);
	if(i.header!=TAGFILE_MAGIC){return 0;}

	int ventries=0,rentries=0;
	fread(&ventries,sizeof(int),1,ftags);
	fread(&rentries,sizeof(int),1,ftags);

	for(int n=0;n<ventries;++n){
		int offset=ftell(ftags);
		fread(name,sizeof(char),NAME_BUFFER_SIZE,ftags);
		for(int m=0;m<TAG_COUNT;++m){
			fread(tag,sizeof(char),TAG_BUFFER_SIZE,ftags);
			int trip=0;
			for(int l=0;l<tagc;++l){
				if(!strcmp(tag,tags[l])){//found a match!
					printf("%s%s\n",path,name);
					trip=1;
					break;
				}
			}
			if(trip){
				fseek(ftags,offset+ROW_BUFFER_SIZE,SEEK_SET);
				break;
			}
		}

	}


return 1;
}





int query_tagfile(const char *name,FILE *ftags){

	struct info i;

	fread(&i,sizeof(struct info),1,ftags);
	//printf("Header: [%x]\n",tf->t_info.header);
	if(i.header!=TAGFILE_MAGIC){return 0;}

	int ventries=0,rentries=0;
	fread(&ventries,sizeof(int),1,ftags);
	fread(&rentries,sizeof(int),1,ftags);
	
	struct row entry;
	

	for(int n=0;n<ventries;++n){
		read_row(&entry,ftags);
		if(!strcmp(entry.name,name)){//if the names match
			for(int c=0;c<TAG_COUNT;++c){
				if(entry.tags[c][0]!='\0'){
					printf("%s\n",entry.tags[c]);
				}
			}
			break;
		}
	}

	return 1;
}





#endif

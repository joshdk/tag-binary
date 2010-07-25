#ifndef TFMANIP_H
#define TFMANIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "res.h"

#define NAME_BUFFER_SIZE 	256
#define TAG_COUNT 				8
#define TAG_BUFFER_SIZE 	8

#define TAGFILE_FILENAME  ".tags"
#define TAGFILE_MAGIC			0x73676174 //"tags"

#define INVALID_OFFSET 		-1
#define INVALID_SIZE			-1

//#define DEFRAG_RATIO      0.875


int lstat(const char *path, struct stat *buf);

struct header{
	unsigned int magic;
	unsigned char major;
	unsigned char minor;
	unsigned short build;
};

struct table{
	unsigned int virt;
	unsigned int real;
};

struct rowinfo{
	unsigned short name_buffer_size;
	unsigned char tag_count;
	unsigned char tag_buffer_size;
};

struct row{
	char *name;
	char **tags;
};

struct row *create_rowdata(struct rowinfo *);
void destroy_rowdata(struct row *,struct rowinfo *);

int read_row(struct row *,struct rowinfo *,FILE *);
int write_row(struct row *,struct rowinfo *,FILE *);
int append_row(struct rowinfo *,FILE *);

int search_tagfile(const char *,const char **,int,FILE *);

int query_tagfile(const char *,FILE *);

int tag_tagfile(const char*,const char**,int);
int tag_row(const char*,const char**,int,struct rowinfo *,FILE *,int);

int dump_tagfile(const char *);

int contains(const char *,const char *);
int extract_paths(const char *,char **,char **,char **);




struct row *create_rowdata(struct rowinfo *ri){
	struct row *rowdata=malloc(sizeof(struct row));
	rowdata->name=calloc(ri->name_buffer_size+1,sizeof(char));
	rowdata->tags=malloc(ri->tag_count*sizeof(char*));
	for(int n=0;n<ri->tag_count;++n){
		rowdata->tags[n]=calloc(ri->tag_buffer_size+1,sizeof(char));
	}

	return rowdata;
}




void destroy_rowdata(struct row *rowdata,struct rowinfo *ri){
	for(int n=0;n<ri->tag_count;++n){
		free(rowdata->tags[n]);rowdata->tags[n]=NULL;
	}
	free(rowdata->tags);rowdata->tags=NULL;
	free(rowdata->name);rowdata->name=NULL;
	free(rowdata);rowdata=NULL;
}




int read_row(struct row *rowdata,struct rowinfo *ri,FILE *ftags){
	fread(rowdata->name,sizeof(char),ri->name_buffer_size,ftags);
	for(int n=0;n<ri->tag_count;++n){
		fread(rowdata->tags[n],sizeof(char),ri->tag_buffer_size,ftags);
	}

	return 1;
}




int write_row(struct row *rowdata,struct rowinfo *ri,FILE *ftags){
	if(rowdata==NULL){//if this entry doesn't have any tags, then remove it altogether
		int row_buffer_size=ri->name_buffer_size+ri->tag_count*ri->tag_buffer_size;
		char *buf=calloc(row_buffer_size,sizeof(char));
		fwrite(buf,sizeof(char),row_buffer_size,ftags);
		free(buf);
	}else{
		fwrite(rowdata->name,sizeof(char),ri->name_buffer_size,ftags);
		for(int n=0;n<ri->tag_count;++n){
			fwrite(rowdata->tags[n],sizeof(char),ri->tag_buffer_size,ftags);
		}
	}

	return 1;
}




int append_row(struct rowinfo *ri,FILE *ftags){
	fseek(ftags,0,SEEK_END);
	int offset=ftell(ftags);
	write_row(NULL,ri,ftags);
	return offset;
}




int tag_row(const char *filename,const char **tags,int tagc,struct rowinfo *ri,FILE *ftags,int offset){
	
	struct row *rowdata=create_rowdata(ri);
	
	fseek(ftags,offset,SEEK_SET);
	read_row(rowdata,ri,ftags);
	fseek(ftags,offset,SEEK_SET);

	if(rowdata->name[0]=='\0'){
		strcpy(rowdata->name,filename);
	}

	for(int n=0;n<tagc;++n){//loop over tags

		if(tags[n][0]=='-'){//remove a tag from the list via exact match
			for(int m=0;m<ri->tag_count;++m){
				if(rowdata->tags[m][0]=='\0')continue;
				if(!strcmp(rowdata->tags[m],tags[n]+1)){//if this tag matches
					//printf("(-) %s\n",rowdata->tags[m]);
					memset(rowdata->tags[m],'\0',ri->tag_buffer_size);//zero out the tag
					break;//since the tags are unique (and we removed it), move on to the next rule
				}
			}

		}else if(tags[n][0]==':'){//remove a tag from the list via partial match
			for(int m=0;m<ri->tag_count;++m){
				if(!rowdata->tags[m][0])continue;
				if(contains(rowdata->tags[m],tags[n]+1)){//if this tag matches
					//printf("(-) %s\n",rowdata->tags[m]);
					memset(rowdata->tags[m],'\0',ri->tag_buffer_size);//zero out the tag
				}
			}

		}else if(tags[n][0]=='.'){//remove a tag from the list via partial match
			for(int m=0;m<ri->tag_count;++m){
				if(!rowdata->tags[m][0])continue;
				if(!contains(rowdata->tags[m],tags[n]+1)){//if this tag matches
					//printf("(-) %s\n",rowdata->tags[m]);
					memset(rowdata->tags[m],'\0',ri->tag_buffer_size);//zero out the tag
				}
			}

		}else{//add a tag to the list
			int index=INVALID_OFFSET,mod=tags[n][0]=='+';
			if(strlen(tags[n]+mod)<=ri->tag_buffer_size){
				for(int m=0;m<ri->tag_count;++m){
					if(!strcmp(rowdata->tags[m],tags[n]+mod)){//this tag already exists
						index=INVALID_OFFSET;
						break;//stop caring about it
					}else if(rowdata->tags[m][0]=='\0'){//found an empty tag
						if(index==INVALID_OFFSET){
							index=m;
						}
					}
				}
				if(index!=INVALID_OFFSET){//if we found a place to put the tag
					strcpy(rowdata->tags[index],tags[n]+mod);
					//printf("(+) %s\n",rowdata->tags[index]);
				}
			}

		}

	}

	int count=0;
	for(int n=ri->tag_count;n--;){
		if(rowdata->tags[n][0]!='\0'){
			++count;
			puts(rowdata->tags[n]);
		}
	}

	if(count==0){//if this entry doesn't have any tags, then remove it altogether
		write_row(NULL,ri,ftags);
	}else{
		write_row(rowdata,ri,ftags);
	}

	destroy_rowdata(rowdata,ri);

	return count>0;
}




int tag_tagfile(const char *target,const char **tags,int tagc){
	if(tagc<1){
		fprintf(stderr,"tag: No tags specified\n");
		return 0;
	}

	char *path,*name,*tagfile;
	if(!extract_paths(target,&path,&name,&tagfile)){return 0;}

	int existing=0,offset=INVALID_OFFSET,entries_offset=-1;
	struct header i={TAGFILE_MAGIC,VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD};
	struct rowinfo ri={NAME_BUFFER_SIZE,TAG_COUNT,TAG_BUFFER_SIZE};
	struct table tdata={0,0};

	FILE *ftags;

	if((ftags=fopen(tagfile,"rb+"))!=NULL){//open an old file
		fread(&i,sizeof(struct header),1,ftags);
		if(i.magic!=TAGFILE_MAGIC){
			free(path);path=NULL;
			free(name);name=NULL;
			free(tagfile);tagfile=NULL;
			return 0;
		}

		fread(&ri,sizeof(struct rowinfo),1,ftags);
		struct row *rowdata=create_rowdata(&ri);

		entries_offset=ftell(ftags);
		fread(&tdata,sizeof(struct table),1,ftags);

		for(unsigned int n=0;n<tdata.virt;++n){
			int pos=ftell(ftags);
			read_row(rowdata,&ri,ftags);
			if(rowdata->name[0]=='\0' && offset==INVALID_OFFSET){//found the first blank row
				offset=pos;
			}else if(!strcmp(name,rowdata->name)){//found the target row
				offset=pos;
				existing=1;
				break;
			}
			
		}
		if(offset==INVALID_OFFSET){
			offset=append_row(&ri,ftags);//append a new row
			++tdata.virt;
		}

		destroy_rowdata(rowdata,&ri);

	}else if((ftags=fopen(tagfile,"wb+"))!=NULL){//create a new file
		fwrite(&i,sizeof(struct header),1,ftags);
		fwrite(&ri,sizeof(struct rowinfo),1,ftags);

		entries_offset=ftell(ftags);
		fwrite(&tdata,sizeof(struct table),1,ftags);
		offset=append_row(&ri,ftags);
		++tdata.virt;

	}else{
		free(path);path=NULL;
		free(name);name=NULL;
		free(tagfile);tagfile=NULL;
		fprintf(stderr,"tag: `%s\': Could not tag\n",target);
		return 0;
	}


	
	if(tag_row(name,tags,tagc,&ri,ftags,offset)){//if data was added
		if(!existing){++tdata.real;}
	}else{//if no data was added (or removed)
		if(existing){--tdata.real;}
	}

	//update size of virual and real entries
	fseek(ftags,entries_offset,SEEK_SET);
	fwrite(&tdata,sizeof(struct table),1,ftags);
	/*
	if(((double)tdata.real)/((double)tdata.virt) <= DEFRAG_RATIO){//too many empty rows in the file, time to defrag it
		//TODO: defrag tagfile
	}
	//*/
	fclose(ftags);
	free(path);path=NULL;
	free(name);name=NULL;
	free(tagfile);tagfile=NULL;

	return 1;
}




int dump_tagfile(const char *target){

	char *path,*name,*tagfile;
	if(!extract_paths(target,&path,&name,&tagfile)){return 0;}
	free(path);path=NULL;

	FILE *ftags;
	if((ftags=fopen(tagfile,"rb"))==NULL){
		free(name);name=NULL;
		free(tagfile);tagfile=NULL;
		return 0;
	}

	struct header i={0,0,0,0};
	fread(&i,sizeof(struct header),1,ftags);
	if(i.magic!=TAGFILE_MAGIC){
		printf("bad file\n");
		fclose(ftags);
		free(name);name=NULL;
		free(tagfile);tagfile=NULL;
		return 0;
	}

	printf("target file: %s\n",target);
	printf("tagfile: %s\n",tagfile);
	printf("magic: %x\n",i.magic);
	printf("version: %d.%d.%d\n",i.major,i.minor,i.build);
	printf("\n");

	struct rowinfo ri={0,0,0};
	fread(&ri,sizeof(struct rowinfo),1,ftags);
	printf("name buffer size: %d\n",ri.name_buffer_size);
	printf("tag count: %d\n",ri.tag_count);
	printf("tag buffer size: %d\n",ri.tag_buffer_size);
	printf("\n");

	struct table tdata={0,0};
	fread(&tdata,sizeof(struct table),1,ftags);
	printf("rows virtual: %d\n",tdata.virt);
	printf("rows real: %d\n",tdata.real);
	printf("\n");	

	struct row *rowdata=create_rowdata(&ri);

	for(unsigned int i=0;i<tdata.virt;++i){//for every table entry
		read_row(rowdata,&ri,ftags);//read one row
		if(rowdata->name[0]=='\0')continue;//ignore empty rows
		printf("%s\n",rowdata->name);
		for(int n=0;n<ri.tag_count;++n){
			if(rowdata->tags[n][0]=='\0')continue;
			printf("  %s\n",rowdata->tags[n]);
		}

	}
	
	destroy_rowdata(rowdata,&ri);

	fclose(ftags);
	free(name);name=NULL;
	free(tagfile);tagfile=NULL;

	return 1;
}




int query_tagfile(const char *name,FILE *ftags){//updated

	struct header i={0,0,0,0};
	fread(&i,sizeof(struct header),1,ftags);
	if(i.magic!=TAGFILE_MAGIC){return 0;}

	struct rowinfo ri={0,0,0};
	fread(&ri,sizeof(struct rowinfo),1,ftags);
	
	struct table tdata={0,0};
	fread(&tdata,sizeof(struct table),1,ftags);
	
	struct row *rowdata=create_rowdata(&ri);

	for(unsigned int i=0;i<tdata.virt;++i){
	
		read_row(rowdata,&ri,ftags);
		if(!strcmp(rowdata->name,name)){//if the names match
			for(int n=0;n<ri.tag_count;++n){
				if(rowdata->tags[n][0]!='\0'){
					puts(rowdata->tags[n]);
				}
			}
			break;//print the first result only
		}
	}
	
	destroy_rowdata(rowdata,&ri);

	return 1;
}




int search_tagfile(const char *path,const char **tags,int tagc,FILE *ftags){//updated

	struct header i={0,0,0,0};
	fread(&i,sizeof(struct header),1,ftags);
	if(i.magic!=TAGFILE_MAGIC){return 0;}

	struct rowinfo ri={0,0,0};
	fread(&ri,sizeof(struct rowinfo),1,ftags);
	
	struct table tdata={0,0};
	fread(&tdata,sizeof(struct table),1,ftags);
	
	struct row *rowdata=create_rowdata(&ri);

	for(unsigned int i=0;i<tdata.virt;++i){//for every table entry
		read_row(rowdata,&ri,ftags);//read one row
		if(rowdata->name[0]=='\0')continue;//ignore empty rows
		
		int valid=0,trip=0;
		for(int n=0;n<tagc;++n){//for every tag rule
			
			if(tags[n][0]=='-'){//exact, contain absolutely none
				for(int m=0;m<ri.tag_count;++m){
					if(!strcmp(rowdata->tags[m],tags[n]+1)){//as soon as we find an ilegal tag, stop checking rules
						valid=0;trip=1;break;
					}
				}
				
			}else if(tags[n][0]==':'){//partial, contains at least one
				for(int m=0;m<ri.tag_count;++m){
					if(contains(rowdata->tags[m],tags[n]+1)){//found a match via a partial tag
						valid=1;break;
					}
				}
				
			}else if(tags[n][0]=='.'){//partial, contains absolutely none
				for(int m=0;m<ri.tag_count;++m){
					if(contains(rowdata->tags[m],tags[n]+1)){//found a match via a partial tag
						valid=0;trip=1;break;
					}
				}

			}else{
				int mod=tags[n][0]=='+';//exact, contains at least one
				for(int m=0;m<ri.tag_count;++m){
					if(!strcmp(rowdata->tags[m],tags[n]+mod)){//found a match via an exact tag
						valid=1;break;
					}
				}
				
			}
		
			if(trip)break;//stop looping if an illegal tag was found
		
		
		}
		
		if(valid){//if this row matches the rules
			if(strcmp(rowdata->name,".")){
				printf("%s%s\n",path,rowdata->name);
			}else{
				printf("%s\n",path);//ignore entries with the name of "."
			}
		}

	}

	destroy_rowdata(rowdata,&ri);

return 1;
}




int extract_paths(const char *target,char **path,char **name,char **tagfile){

	struct stat s;
	if(lstat(target,&s)){
		switch(errno){
			case EACCES:
				fprintf(stderr,"tag: `%s\': Permission denied\n",target);
				break;
			default:
				fprintf(stderr,"tag: `%s\': No such file or directory\n",target);
				break;
		}
		return 0;
	}
	
	*path=malloc((strlen(target)+2)*sizeof(**path));
	strcpy(*path,target);
	int pathlen=strlen(*path);

	if(S_ISDIR(s.st_mode)){//if target is a directory...
		if((*path)[pathlen-1]!='/'){
			strcat(*path,"/");
		}
		*name=malloc(2*sizeof(**name));
		strcpy(*name,".");//name will refer to the "." alias for current directory

	}else if(S_ISREG(s.st_mode)||S_ISCHR(s.st_mode)||S_ISBLK(s.st_mode)||S_ISLNK(s.st_mode)){//if file is some other file
		int n=pathlen;
		while(--n){
			if((*path)[n-1]=='/'){
				break;
			}
		}
		int offset=n;

		*name=malloc((pathlen-n+2)*sizeof(**name));
		for(int o=0;n<pathlen;++n,++o){
			(*name)[o]=(*path)[n];
		}
		(*path)[offset]='\0';
			
	}else{
		fprintf(stderr,"tag: `%s\': Not a file or directory\n",target);
		free(*path);*path=NULL;
		return 0;
	}

	*tagfile=malloc((strlen(*path)+2+strlen(TAGFILE_FILENAME))*sizeof(**tagfile));
	sprintf(*tagfile,"%s%s",*path,TAGFILE_FILENAME);

	return 1;

}




int contains(const char *str,const char *sub){
	int len=strlen(sub);
	for(int n=strlen(str)-len+1;n-->0;){
		int trip=1;
		for(int m=0;m<len;++m){
			if(str[n+m]!=sub[m]){trip=0;break;}
		}
		if(trip)
		return 1;
	}
	return 0;
}


#endif

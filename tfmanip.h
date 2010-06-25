#ifndef TFMANIP_H
#define TFMANIP_H

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
int search_tagfile(const char *,char **,int,FILE *);
int query_tagfile(const char *,FILE *);

int extract_paths(const char *,char **,char **,char **);



int contains_str(const char *,const char *);



int contains_str(const char *str,const char *sub){
	int len=strlen(sub);
	for(int n=strlen(str)-len+1;n-->0;){
		//printf("n:%d str:%s\n",n,str+n);
		int trip=1;
		for(int m=0;m<len;++m){
			if(str[n+m]!=sub[m]){trip=0;break;}
		}
		if(trip)
		return 1;
	}
	return 0;
}





int dump(const char *);

int dump(const char *target){

	char *path,*name,*tagfile;
	if(!extract_paths(target,&path,&name,&tagfile)){
		return 0;
	}
	free(path);path=NULL;

	FILE *ftags;
	if((ftags=fopen(tagfile,"rb"))==NULL){
		free(name);name=NULL;
		free(tagfile);tagfile=NULL;
		return 0;
	}


	
	struct info i={0};
	fread(&i,sizeof(struct info),1,ftags);
	if(i.header!=TAGFILE_MAGIC){
		fclose(ftags);
		free(name);name=NULL;
		free(tagfile);tagfile=NULL;
		return 0;
	}

	printf("target file: %s\n",target);
	printf("tagfile: %s\n",tagfile);
	printf("magic: %x\n",i.header);
	printf("version: %d.%d%s%d\n",i.vera,i.verb,(i.verc?"rc":"b"),i.verd);
	printf("\n");

	struct table tdata={0};
	fread(&tdata,sizeof(struct table),1,ftags);
	printf("rows virtual: %d\n",tdata.virt);
	printf("rows real: %d\n",tdata.real);
	printf("\n");	

	struct row rowdata={0};

	for(int i=0;i<tdata.virt;++i){//for every table entry
		read_row(&rowdata,ftags);//read one row
		if(rowdata.name[0]=='\0')continue;//ignore empty rows
		printf("%s\n",rowdata.name);
		for(int n=0;n<TAG_COUNT;++n){
			if(rowdata.tags[n][0]=='\0')continue;
			printf("  %s\n",rowdata.tags[n]);
		}

	}
	
	fclose(ftags);
	free(name);name=NULL;
	free(tagfile);tagfile=NULL;

	return 1;
}










int read_row(struct row *entry,FILE *ftags){
	fread(entry->name,sizeof(char),NAME_BUFFER_SIZE,ftags);
	for(int n=0;n<TAG_COUNT;++n){
		fread(entry->tags[n],sizeof(char),TAG_BUFFER_SIZE,ftags);
	}
	return 1;
}


int search_tagfile(const char *path,char **tags,int tagc,FILE *ftags){

	struct info i={0};
	fread(&i,sizeof(struct info),1,ftags);
	if(i.header!=TAGFILE_MAGIC){return 0;}

	struct table tdata={0};
	fread(&tdata,sizeof(struct table),1,ftags);
	
	struct row rowdata={0};

	for(int i=0;i<tdata.virt;++i){//for every table entry
		read_row(&rowdata,ftags);//read one row
		if(rowdata.name[0]=='\0')continue;//ignore empty rows
		
		int valid=0,trip=0;
		for(int n=0;n<tagc;++n){//for every tag rule
			
			if(tags[n][0]=='-'){//exact, contain absolutely none
				for(int m=0;m<TAG_COUNT;++m){
					if(!strcmp(rowdata.tags[m],tags[n]+1)){//as soon as we find an ilegal tag, stop checking rules
						valid=0;trip=1;break;
					}
				}
				
			}else if(tags[n][0]=='%'){//partial, contains at least one
				for(int m=0;m<TAG_COUNT;++m){
					if(contains_str(rowdata.tags[m],tags[n]+1)){//found a match via a partial tag
						valid=1;break;
					}
				}
				
			}else if(tags[n][0]==':'){//partial, contains absolutely none
				for(int m=0;m<TAG_COUNT;++m){
					if(contains_str(rowdata.tags[m],tags[n]+1)){//found a match via a partial tag
						valid=0;trip=1;break;
					}
				}

			}else{
				int mod=tags[n][0]=='+';//exact, contains at least one
				for(int m=0;m<TAG_COUNT;++m){
					if(!strcmp(rowdata.tags[m],tags[n]+mod)){//found a match via an exact tag
						valid=1;break;
					}
				}
				
			}
		
			if(trip)break;//stop looping if an illegal tag was found
		
		
		}
		
		if(valid){//if this row matches the rules
			if(strcmp(rowdata.name,".")){
				printf("%s%s\n",path,rowdata.name);
			}else{
				printf("%s\n",path);//ignore entries with the name of "."
			}
		}

	}

return 1;
}





int query_tagfile(const char *name,FILE *ftags){

	struct info i={0};
	fread(&i,sizeof(struct info),1,ftags);
	if(i.header!=TAGFILE_MAGIC){return 0;}

	struct table tdata={0};
	fread(&tdata,sizeof(struct table),1,ftags);
	
	struct row rowdata={0};
	
	for(int i=0;i<tdata.virt;++i){
	
		read_row(&rowdata,ftags);
		if(!strcmp(rowdata.name,name)){//if the names match
			for(int n=0;n<TAG_COUNT;++n){
				if(rowdata.tags[n][0]!='\0'){
					printf("%s\n",rowdata.tags[n]);
				}
			}
			break;//prin the first result only
		}
	}

	return 1;
}





#endif

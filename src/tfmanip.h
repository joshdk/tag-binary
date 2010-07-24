#ifndef TFMANIP_H
#define TFMANIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define PROGRAM_NAME			"tag"

#define TAGFILE_FILENAME  ".tags"
#define TAGFILE_MAGIC			0x73676174 //"tags"
//*
//#define NAME_MAX 					255
#define NAME_BUFFER_SIZE 	256

#define TAG_COUNT 				4//16
#define TAG_BUFFER_SIZE 	4//16

#define ROW_BUFFER_SIZE		(NAME_BUFFER_SIZE+TAG_BUFFER_SIZE*TAG_COUNT)
//*/
#define INVALID_OFFSET 		-1
#define INVALID_SIZE			-1

#define DEFRAG_RATIO      0.875

#define TRUE 							1
#define FALSE							0


/*
struct offsets{
	int info;
	int table;
	int data;
};
//*/

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
/*
struct rowold{
	char name[NAME_BUFFER_SIZE+1];
	char tags[TAG_COUNT][TAG_BUFFER_SIZE+1];
};
//*/

struct row{
	char *name;
	char **tags;
};


//int read_row(struct rowold *,FILE *);
int read_row(struct row *,struct rowinfo *,FILE *);

int search_tagfile(const char *,char **,int,FILE *);
int query_tagfile(const char *,FILE *);
int dump_tagfile(const char *);

int extract_paths(const char *,char **,char **,char **);

int defrag_tagfile(FILE *);

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



int defrag_tagfile(FILE *ftags){
	printf("[begin defrag]\n");
	return 1;
}











int dump_tagfile(const char *target){//updated

	char *path,*name,*tagfile;
	if(!extract_paths(target,&path,&name,&tagfile)){return 0;}
	free(path);path=NULL;

	FILE *ftags;
	if((ftags=fopen(tagfile,"rb"))==NULL){
		free(name);name=NULL;
		free(tagfile);tagfile=NULL;
		return 0;
	}


	
	struct header i={0};
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

	struct rowinfo ri={0};
	fread(&ri,sizeof(struct rowinfo),1,ftags);
	printf("name buffer size: %d\n",ri.name_buffer_size);
	printf("tag count: %d\n",ri.tag_count);
	printf("tag buffer size: %d\n",ri.tag_buffer_size);
	printf("\n");

	struct table tdata={0};
	fread(&tdata,sizeof(struct table),1,ftags);
	printf("rows virtual: %d\n",tdata.virt);
	printf("rows real: %d\n",tdata.real);
	printf("\n");	

	//struct rowold rowdata2={{0}};
	
	//allocate all parts of the rowdata structure
	struct row rowdata;
	rowdata.name=malloc((ri.name_buffer_size+1)*sizeof(char));
	rowdata.tags=malloc(ri.tag_count*sizeof(char*));
	for(int n=0;n<ri.tag_count;++n){
		rowdata.tags[n]=malloc((ri.tag_buffer_size+1)*sizeof(char));
	}


	for(int i=0;i<tdata.virt;++i){//for every table entry
		read_row(&rowdata,&ri,ftags);//read one row
		if(rowdata.name[0]=='\0')continue;//ignore empty rows
		printf("%s\n",rowdata.name);
		for(int n=0;n<ri.tag_count;++n){
			if(rowdata.tags[n][0]=='\0')continue;
			printf("  %s\n",rowdata.tags[n]);
		}

	}
	
	//free all parts of the rowdata structure
	for(int n=0;n<ri.tag_count;++n){
		free(rowdata.tags[n]);rowdata.tags[n]=NULL;
	}
	free(rowdata.tags);rowdata.tags=NULL;
	free(rowdata.name);rowdata.name=NULL;


	fclose(ftags);
	free(name);name=NULL;
	free(tagfile);tagfile=NULL;

	return 1;
}






int read_row(struct row *rowdata,struct rowinfo *ri,FILE *ftags){//updated
	//if(rowdata==NULL||ri==NULL)return 0;
	fread(rowdata->name,sizeof(char),ri->name_buffer_size,ftags);
	for(int n=0;n<ri->tag_count;++n){
		fread(rowdata->tags[n],sizeof(char),ri->tag_buffer_size,ftags);
	}
	return 1;
}


/*
int read_row(struct rowold *entry,FILE *ftags){
	fread(entry->name,sizeof(char),NAME_BUFFER_SIZE,ftags);
	for(int n=0;n<TAG_COUNT;++n){
		fread(entry->tags[n],sizeof(char),TAG_BUFFER_SIZE,ftags);
	}
	return 1;
}
//*/





int query_tagfile(const char *name,FILE *ftags){//updated

	struct header i={0};
	fread(&i,sizeof(struct header),1,ftags);
	if(i.magic!=TAGFILE_MAGIC){return 0;}

	struct rowinfo ri={0};
	fread(&ri,sizeof(struct rowinfo),1,ftags);
	

	struct table tdata={0};
	fread(&tdata,sizeof(struct table),1,ftags);

	//allocate all parts of the rowdata structure
	struct row rowdata={0};
	rowdata.name=malloc(ri.name_buffer_size*sizeof(char));
	rowdata.tags=malloc(ri.tag_count*sizeof(char*));
	for(int n=0;n<ri.tag_count;++n){
		rowdata.tags[n]=malloc(ri.tag_buffer_size*sizeof(char));
	}
	
	for(int i=0;i<tdata.virt;++i){
	
		read_row(&rowdata,&ri,ftags);
		if(!strcmp(rowdata.name,name)){//if the names match
			for(int n=0;n<ri.tag_count;++n){
				if(rowdata.tags[n][0]!='\0'){
					printf("(:) %s\n",rowdata.tags[n]);
				}
			}
			break;//print the first result only
		}
	}

	//free all parts of the rowdata structure
	for(int n=0;n<ri.tag_count;++n){
		free(rowdata.tags[n]);rowdata.tags[n]=NULL;
	}
	free(rowdata.tags);rowdata.tags=NULL;
	free(rowdata.name);rowdata.name=NULL;

	return 1;
}











int search_tagfile(const char *path,char **tags,int tagc,FILE *ftags){//updated

	struct header i={0};
	fread(&i,sizeof(struct header),1,ftags);
	if(i.magic!=TAGFILE_MAGIC){return 0;}

	struct rowinfo ri={0};
	fread(&ri,sizeof(struct rowinfo),1,ftags);
	

	struct table tdata={0};
	fread(&tdata,sizeof(struct table),1,ftags);
	
	//struct rowold rowdata={{0}};
	struct row rowdata;
	rowdata.name=malloc((ri.name_buffer_size+1)*sizeof(char));
	rowdata.tags=malloc(ri.tag_count*sizeof(char*));
	for(int n=0;n<ri.tag_count;++n){
		rowdata.tags[n]=malloc((ri.tag_buffer_size+1)*sizeof(char));
	}

	for(int i=0;i<tdata.virt;++i){//for every table entry
		read_row(&rowdata,&ri,ftags);//read one row
		if(rowdata.name[0]=='\0')continue;//ignore empty rows
		
		int valid=0,trip=0;
		for(int n=0;n<tagc;++n){//for every tag rule
			
			if(tags[n][0]=='-'){//exact, contain absolutely none
				for(int m=0;m<ri.tag_count;++m){
					if(!strcmp(rowdata.tags[m],tags[n]+1)){//as soon as we find an ilegal tag, stop checking rules
						valid=0;trip=1;break;
					}
				}
				
			}else if(tags[n][0]==':'){//partial, contains at least one
				for(int m=0;m<ri.tag_count;++m){
					if(contains_str(rowdata.tags[m],tags[n]+1)){//found a match via a partial tag
						valid=1;break;
					}
				}
				
			}else if(tags[n][0]=='.'){//partial, contains absolutely none
				for(int m=0;m<ri.tag_count;++m){
					if(contains_str(rowdata.tags[m],tags[n]+1)){//found a match via a partial tag
						valid=0;trip=1;break;
					}
				}

			}else{
				int mod=tags[n][0]=='+';//exact, contains at least one
				for(int m=0;m<ri.tag_count;++m){
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


	//free all parts of the rowdata structure
	for(int n=0;n<ri.tag_count;++n){
		free(rowdata.tags[n]);rowdata.tags[n]=NULL;
	}
	free(rowdata.tags);rowdata.tags=NULL;
	free(rowdata.name);rowdata.name=NULL;

return 1;
}










#endif

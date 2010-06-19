#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tfmanip.h"
#include "res.h"




int lstat(const char *path, struct stat *buf);
int isalnum(int);
int isprint (int);

void search(const char *,char **,int);


//void printf_r(const char*,int,char**);
//int valid_tag(const char*);

int valid_tag(const char *,const char *);
int contains(const char*,char**,int);
int subset(char**,char**,int,int,int);
char **get_tags(char **,int,const char *,int *);


int contains_char(const char *,char);
int query_file(const char*);
int tag_file(const char*,char**,int);
int append_row(FILE*);
int tag_row(const char*,char**,int,FILE *,int);


char **get_tags(char **argv,int argc,const char *mods,int *count){
	*count=0;
	char **tags=malloc(argc*sizeof(*tags));
	for(int n=2;n<argc-1;++n){
		if(valid_tag(argv[n],mods)){
			tags[(*count)++]=argv[n];
			//printf("    args: %s\n",argv[n]);
		}
	}
	if(*count==0){free(tags);return NULL;}
	return tags;
}


//*
void printf_r(const char *format,char **arrv,int arrc){
	for(int n=0;n<arrc;++n)
		printf(format,arrv[n]);
}
//*/
//*
int contains(const char *str,char **arr,int size){
	for(int n=size;n--;)
		if(!strcmp(str,arr[n]))
			return 1;
	return 0;
}
//*/

int subset(char **sub,char **set,int size,int to,int from){
	int o=0;
	for(int n=to;n<=from&&n<size;++n)
		sub[o++]=set[n];
	return o;
}


/*
int valid_tag(const char *tag){
	for(int n=strlen(tag);--n;)
		if(!isalnum(tag[n]))return 0;
	return isalnum(tag[0])||tag[0]=='+'||tag[0]=='-';
}
//*/

int contains_char(const char *str,char l){
	for(int n=strlen(str);n--;)
		if(str[n]==l)return 1;
	return 0;
}






int valid_tag(const char *tag,const char *mods){
	int len=strlen(tag);
	if(len>17)
		return 0;
	if(len==17 && !contains_char(mods,tag[0]))
		return 0;
	/*
	if(!contains_char(mods,tag[0]) && !isalnum(tag[0]))
		return 0;	
	//*/
	for(int n=len;--n;)
		if(!isprint(tag[n]))
			return 0;

	return 1;
}








int main(int argc,char **argv) {

//printf("result: %d\n",contains_str("hello",""));
//return 0;

//*
	if(argc<2){
		fprintf(stderr,"tag: mising operand\n");
		fprintf(stderr,"Try `tag --help' for more information.\n");
		return 0;
	}
	
	
	if(contains("-h",argv,argc) || contains("--help",argv,argc)){
		printf(helptext,PROGRAM_NAME);
		return 0;
	}
	
	if(contains("-v",argv,argc) || contains("--version",argv,argc)){
		printf(versiontext,VERSION_MAJOR,VERSION_MINOR,(VERSION_BUILD?"rc":"b"),VERSION_REVISION);
		return 0;
	}

	if(argc<3){
		fprintf(stderr,"tag: mising operand\n");
		fprintf(stderr,"Try `tag --help' for more information.\n");
		return 0;
	}


	if(!strcmp(argv[1],"-t") || !strcmp(argv[1],"--tag")){//tag a file
		int tagc=0;
		char **tags;
		if(tags=get_tags(argv,argc,"-+%",&tagc)){
			tag_file(argv[argc-1],tags,tagc);
			free(tags);tags=NULL;
		}else{
			fprintf(stderr,"tag: too many invalid tag(s)\n",argv[argc-1]);
		}

	}else if(!strcmp(argv[1],"-f") || !strcmp(argv[1],"--find")){//find file(s)
		//printf("find mode\n");
		struct stat s;
		if(lstat(argv[argc-1],&s)){
			fprintf(stderr,"tag: `%s\': No such directory\n",argv[argc-1]);
			return 0;
		}

		if(!S_ISDIR(s.st_mode)){
			fprintf(stderr,"tag: `%s\': Not a directory\n",argv[argc-1]);
			return 0;
		}
		//char **tags=(char**)malloc(sizeof(char*)*(argc-3));
		//subset(tags,argv,argc,2,argc-2);

		int tagc=0;
		char **tags;
		if(tags=get_tags(argv,argc,"+-%",&tagc)){

			int len=strlen(argv[argc-1]);
			char *path=malloc((len+4)*sizeof(*path));
			strcpy(path,argv[argc-1]);
			if(path[len-1]!='/'){
				sprintf(path,"%s/",path);
			}
			search(path,tags,tagc);
			free(path);path=NULL;
			free(tags);tags=NULL;


		}else{
			fprintf(stderr,"tag: too many invalid tag(s)\n",argv[argc-1]);
		}


	}else if(!strcmp(argv[1],"-d") || !strcmp(argv[1],"--dump")){//dump file tags
		dump(argv[argc-1]);
		
	}else if(!strcmp(argv[1],"-q") || !strcmp(argv[1],"--query")){//query file tags
		
		//printf("query mode\n");
		query_file(argv[argc-1]);

	}else{
		fprintf(stderr,"tag: invalid operand\n");
		fprintf(stderr,"Try `tag --help' for more information.\n");
	}


	return 0;
}




int append_row(FILE *ftags){
	fseek(ftags,0,SEEK_END);
	int offset=ftell(ftags);
	char buffer[ROW_BUFFER_SIZE];
	memset(buffer,'\0',ROW_BUFFER_SIZE);
	fwrite(buffer,sizeof(char),ROW_BUFFER_SIZE,ftags);
	return offset;
}










int tag_row(const char *filename,char **tags,int tagc,FILE *ftags,int offset){
	
	fseek(ftags,offset,SEEK_SET);
	struct row rowdata={0};
	read_row(&rowdata,ftags);
	fseek(ftags,offset,SEEK_SET);

	for(int n=0;n<tagc;++n){//loop over tags

		if(tags[n][0]=='-'){//remove a tag from the list via exact match
			for(int m=0;m<TAG_COUNT;++m){
				if(!rowdata.tags[m][0])continue;
				if(!strcmp(rowdata.tags[m],tags[n]+1)){//if this tag matches
					printf("(-) %s\n",rowdata.tags[m]);
					memset(rowdata.tags[m],'\0',TAG_BUFFER_SIZE);//zero out the tag
					break;//removed our tag, now on to the next rule
				}
			}		

		}else if(tags[n][0]=='%'){//remove a tag from the list via partial match
			for(int m=0;m<TAG_COUNT;++m){
				if(!rowdata.tags[m][0])continue;
				if(contains_str(rowdata.tags[m],tags[n]+1)){//if this tag matches
					printf("(-) %s\n",rowdata.tags[m]);
					memset(rowdata.tags[m],'\0',TAG_BUFFER_SIZE);//zero out the tag
				}
			}

		}else{//add a tag to the list
			int index=INVALID_OFFSET,mod=tags[n][0]=='+';
			for(int m=0;m<TAG_COUNT;++m){
				if(!strcmp(rowdata.tags[m],tags[n]+mod)){//this tag already exists
					index=INVALID_OFFSET;
					break;//stop caring about it
				}else if(rowdata.tags[m][0]=='\0'){//found an empty tag
					if(index==INVALID_OFFSET){
						index=m;
					}
				}
			}
			if(index!=INVALID_OFFSET){//if we found a place to put the tag
				strcpy(rowdata.tags[index],tags[n]+mod);
				printf("(+) %s\n",rowdata.tags[index]);
			}

		}

	}

	int count=0;
	for(int n=TAG_COUNT;n--;){
		if(strlen(rowdata.tags[n])){
			count=1;
			break;
		}
	}
	//printf("count: [%d]\n",count);

	if(count>0){
		strcpy(rowdata.name,filename);
		fwrite(rowdata.name,sizeof(char),NAME_BUFFER_SIZE,ftags);
		for(int n=0;n<TAG_COUNT;++n){
			fwrite(rowdata.tags[n],sizeof(char),TAG_BUFFER_SIZE,ftags);
		}
	}else{
		char buf[ROW_BUFFER_SIZE];
		memset(buf,'\0',ROW_BUFFER_SIZE);
		fwrite(buf,sizeof(char),ROW_BUFFER_SIZE,ftags);
	}
		
return count>0;
}








int tag_file(const char *target,char **tags,int tagc){
	if(tagc<1){
		fprintf(stderr,"tag: No tags specified\n");
		return 0;
	}

	//printf("!here 1\n");

	char *path,*name,*tagfile;
	if(!extract_paths(target,&path,&name,&tagfile)){
		return 0;
	}
	//printf("!here 2\n");

	FILE *ftags;
	struct info i;
	//struct table t;
	int /*ventries=0,rentries=0,*/existing=0,offset=INVALID_OFFSET,entries_offset=-1;
	struct table tdata={0};

	struct row rowdata={0};
	
	if((ftags=fopen(tagfile,"rb+"))!=NULL){//open an old file
		//printf("opening old file\n");
		fread(&i,sizeof(struct info),1,ftags);
		//printf("magic number: [%x]\n",i.header);
		entries_offset=ftell(ftags);
		fread(&tdata,sizeof(struct table),1,ftags);
		//fread(&ventries,sizeof(int),1,ftags);
		//fread(&rentries,sizeof(int),1,ftags);
		//char filename[NAME_BUFFER_SIZE];
		for(int n=0;n<tdata.virt;++n){
			int pos=ftell(ftags);
			//memset(filename,'\0',NAME_BUFFER_SIZE);
			//fread(filename,sizeof(char),NAME_BUFFER_SIZE,ftags);
			read_row(&rowdata,ftags);
			//printf("current row name: %s\n",filename);
			if(rowdata.name[0]=='\0' && offset==INVALID_OFFSET){//found the first blank row
				offset=pos;
				//printf("found an empty row at offset: %d\n",offset);
			}else if(!strcmp(name,rowdata.name)){//found the target row
				offset=pos;
				existing=1;
				break;
				//printf("found the target's row at offset: %d\n",offset);
			}
			//didn't find a matching, or empty row
			//fseek(ftags,TAG_COUNT*TAG_BUFFER_SIZE,SEEK_CUR);
			
		}
		if(offset==INVALID_OFFSET){
			//printf("appending new row\n");
			offset=append_row(ftags);
			++tdata.virt;
		}
		


	}else if((ftags=fopen(tagfile,"wb+"))!=NULL){//create a new file
		//printf("creating new file\n");
		//struct info i;
		i.header=TAGFILE_MAGIC;
		i.vera=VERSION_MAJOR,i.verb=VERSION_MINOR,i.verc=VERSION_BUILD,i.verd=VERSION_REVISION;
		fwrite(&i,sizeof(struct info),1,ftags);
		
		//int entries_offset=ftell(ftags);

		entries_offset=ftell(ftags);
		fwrite(&tdata,sizeof(struct table),1,ftags);
		//fwrite(&ventries,sizeof(int),1,ftags);
		//fwrite(&rentries,sizeof(int),1,ftags);
		offset=append_row(ftags);
		++tdata.virt;
		//printf("row offset: [%d]\n",offset);

	}else{
		fprintf(stderr,"tag: `%s\': Could not tag\n",target);
		return 0;
	}
	

	//printf("row offset: [%d]\n",offset);
	if(tag_row(name,tags,tagc,ftags,offset)){//if data was added
		if(!existing){++tdata.real;}
	}else{//if no data was added (or removed)
		if(existing){--tdata.real;}
	}

		//update size of virual and real entries
		fseek(ftags,entries_offset,SEEK_SET);
		fwrite(&tdata,sizeof(struct table),1,ftags);
		//fwrite(&ventries,sizeof(int),1,ftags);
		//fwrite(&rentries,sizeof(int),1,ftags);

	fclose(ftags);

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
	//char *name;
	//printf("before: [%s]\n",path);

	//printf("here 3\n");

	if(S_ISDIR(s.st_mode)){//if target is a directory...
		if((*path)[pathlen-1]!='/'){
			sprintf(*path,"%s/",*path);//apend a slash onto the end of the path
		}
		//printf("after: [%s]\n",path);
		*name=malloc(2*sizeof(**name));
		strcpy(*name,".");//name will refer to the "." alias for current directory

	}else if(S_ISREG(s.st_mode)||S_ISCHR(s.st_mode)||S_ISBLK(s.st_mode)||S_ISLNK(s.st_mode)){//if file is some other file
		
		//printf("here 2\n");		
		int n=pathlen;
		while(--n){
			if((*path)[n-1]=='/'){
				break;
			}
		}
		int offset=n;


		//printf("here 3\n");

		*name=malloc((pathlen-n+2)*sizeof(**name));
		for(int o=0;n<pathlen;++n,++o){
			//printf("[%c]\n",path[n]);
			(*name)[o]=(*path)[n];
		}
		//*name[o]='\0';
		(*path)[offset]='\0';
		//printf("after: [%s]\n",path);
		
			
	}else{
		fprintf(stderr,"tag: `%s\': Not a file or directory\n",target);
		free(*path);*path=NULL;
		return 0;
	}



	*tagfile=malloc((strlen(*path)+2+strlen(TAGFILE_FILENAME))*sizeof(**tagfile));
	sprintf(*tagfile,"%s%s",*path,TAGFILE_FILENAME);
	//printf(".tag target: %s\n",path);
	//printf("real target: %s\n",name);

	return 1;

}















int query_file(const char *target){

	char *path,*name,*tagfile;
	//printf("before\n");
	if(!extract_paths(target,&path,&name,&tagfile)){
		return 0;
	}


	FILE *ftags;
	if((ftags=fopen(tagfile,"r"))==NULL){return 0;}
	
	query_tagfile(name,ftags);
	
	free(name);name=NULL;
	free(path);path=NULL;
	free(tagfile);tagfile=NULL;
	return 1;
}











void search(const char *path,char **tags,int tagc){


	struct dirent *dp;
	struct stat s;
	DIR *dir;

	if((dir=opendir(path))==NULL){return;}

	while((dp=readdir(dir))!=NULL){

		char *temp=malloc((strlen(path)+strlen(dp->d_name)+4)*sizeof(*temp));
		sprintf(temp,"%s%s",path,dp->d_name);

		lstat(temp,&s);

		if(S_ISREG(s.st_mode)){//if regular file
			if(!strcmp(dp->d_name,".tags")){
				//printf("parsing: [%s]\n",temp);
				FILE *ftags;
				if((ftags=fopen(temp,"r"))!=NULL){search_tagfile(path,tags,tagc,ftags);}
				fclose(ftags);

			}
		}else if(S_ISDIR(s.st_mode) && !S_ISLNK(s.st_mode)){//if directory
			if(strcmp(dp->d_name,".") && strcmp(dp->d_name,"..")){
				sprintf(temp,"%s/",temp);
				search(temp,tags,tagc);
			}
		}


		free(temp);

	}
	closedir(dir);


}







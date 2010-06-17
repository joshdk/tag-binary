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


void search(const char *,char **,int);


void printf_r(const char*,int,char**);
int valid_tag(const char*);
int contains(const char*,char**,int);
int subset(char**,char**,int,int,int);


int query(char*);
int tag(char*,int,char**);
int append_row(FILE*);
int tag_row(int,FILE*,char*,int,char**);





void printf_r(const char *format,int arrc,char **arrv){
	for(int n=0;n<arrc;++n)
		printf(format,arrv[n]);
}


int contains(const char *str,char **arr,int size){
	for(int n=size;n--;)
		if(!strcmp(str,arr[n]))
			return 1;
	return 0;
}

int subset(char **sub,char **set,int size,int to,int from){
	int o=0;
	for(int n=to;n<=from&&n<size;++n)
		sub[o++]=set[n];
	return o;
}



int valid_tag(const char *tag){
	for(int n=strlen(tag);--n;)
		if(!isalnum(tag[n]))return 0;
	return isalnum(tag[0])||tag[0]=='+'||tag[0]=='-';
}







int main(int argc,char **argv) {


//*
	if(argc<2){
		fprintf(stderr,"tag: mising operand\n");
		fprintf(stderr,"Try `tag --help' for more information.\n");
		return 0;
	}
	
	
	if(contains("-h",argv,argc) || contains("--help",argv,argc)){//query file tags
		printf(helptext);
		return 0;
	}
	
	if(contains("-v",argv,argc) || contains("--version",argv,argc)){///query file tags
		printf(versiontext,VERSION_MAJOR,VERSION_MINOR,(VERSION_BUILD?"rc":"b"),VERSION_REVISION);
		return 0;
	}

	if(argc<3){
		fprintf(stderr,"tag: mising operand\n");
		fprintf(stderr,"Try `tag --help' for more information.\n");
		return 0;
	}


	if(!strcmp(argv[1],"-t") || !strcmp(argv[1],"--tag")){//tag a file
		//printf("tagging mode\n");
		char **tags=(char**)malloc(sizeof(char*)*(argc-3));
		subset(tags,argv,argc,2,argc-2);
		//printf_r("[%s]\n",argc-3,tags);
		tag(argv[argc-1],argc-3,tags);
		free(tags);

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
		char **tags=(char**)malloc(sizeof(char*)*(argc-3));
		subset(tags,argv,argc,2,argc-2);
		int len=strlen(argv[argc-1]);
		char *path=malloc(sizeof(char)*len+4);
		strcpy(path,argv[argc-1]);
		if(path[len-1]!='/'){
			sprintf(path,"%s/",path);
		}
		//printf_r("[%s]\n",argc-3,tags);
		search(path,tags,argc-3);
		//tag(argv[argc-1],argc-3,tags);
		free(path);
		free(tags);
		
	}else if(!strcmp(argv[1],"-q") || !strcmp(argv[1],"--query")){//query file tags
		
		//printf("query mode\n");
		query(argv[argc-1]);

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
	fwrite(buffer,1,ROW_BUFFER_SIZE,ftags);
	return offset;
}




int tag_row(int offset,FILE *ftags,char *filename,int tagc,char **tags){
	struct row rowdata;
	memset(rowdata.name,'\0',NAME_BUFFER_SIZE);
	strcpy(rowdata.name,filename);
	//printf("NAME: [%s]\n",rowdata.name);
	fseek(ftags,offset+NAME_BUFFER_SIZE,SEEK_SET);
	int count=0;
	for(int n=0;n<TAG_COUNT;++n){
		fread(rowdata.tags[n],TAG_BUFFER_SIZE,1,ftags);
		if(rowdata.tags[n][0])++count;		
		//printf("TAG %d: [%s]\n",n+1,rowdata.tags[n]);
	}

	for(int n=0;n<tagc;++n){
		//printf("APPLYING RULE: [%s]\n",tags[n]);
		if(tags[n][0]=='-'){//remove this tag
			if(!strcmp(tags[n],"-*")){//remove ALL tags
				//printf("deleting all tags\n");
				for(int m=0;m<TAG_COUNT;++m){
					memset(rowdata.tags[m],'\0',TAG_BUFFER_SIZE);//zero out the tag
				}
				count=0;
			}else{//remove a single tag
				for(int m=0;m<TAG_COUNT;++m){
					if(!strcmp(rowdata.tags[m],tags[n]+1)){//if this tag matches
						//printf("deleting tag [%s]\n",tags[n]+1);
						memset(rowdata.tags[m],'\0',TAG_BUFFER_SIZE);//zero out the tag
						--count;
						break;
					}
				}

			}			



		}else{//add this tag
			int index=-1,plus=tags[n][0]=='+';
			for(int m=0;m<TAG_COUNT;++m){
				//printf("checking rule: [%s] against tag: [%s]\n",tags[n],rowdata.tags[m]);
				if(!strcmp(rowdata.tags[m],tags[n]+plus)){//this tag already exists
					//printf("tag [%s] already exists\n",tags[n]+plus);
					break;//stop caring about it
				}else if(rowdata.tags[m][0]=='\0'){//found an empty tag
					if(index==-1){
						index=m;
						//break;
					}
				}

			}

			//int plus=tags[n][0]=='+';
			if(index!=-1){//if we found a place to put the tag
				//printf("adding tag [%s]\n",tags[n]+plus);
				strcpy(rowdata.tags[index],tags[n]+plus);
				++count;
			}else{	
				//printf("no room for tag [%s]\n",tags[n]+plus);
			}
		}


	}

	
	fseek(ftags,offset,SEEK_SET);
	if(count>0){
		fwrite(rowdata.name,NAME_BUFFER_SIZE,1,ftags);
		for(int n=0;n<TAG_COUNT;++n){
			fwrite(rowdata.tags[n],TAG_BUFFER_SIZE,1,ftags);
		}
	}else{
		char buf[ROW_BUFFER_SIZE];
		memset(buf,'\0',ROW_BUFFER_SIZE);
		fwrite(buf,ROW_BUFFER_SIZE,1,ftags);
	}
		
return count>1;
}












int tag(char *target,int tagc,char **tags){
	if(tagc<1){
		fprintf(stderr,"tag: No tags specified\n");
		return 0;
	}

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
	
	char *path=(char*)malloc(strlen(target)+8);
	strcpy(path,target);
	int pathlen=strlen(path);
	char *name;
	//printf("before: [%s]\n",path);


	if(S_ISDIR(s.st_mode)){
		//printf("  is's a directory\n");
		if(path[pathlen-1]!='/'){
			path[pathlen]='/';
			path[pathlen+1]='\0';
		}
		//printf("after: [%s]\n",path);
		name=(char*)malloc(2);
		strcpy(name,".");
			//dir=

	}else if(S_ISREG(s.st_mode) || S_ISCHR(s.st_mode) || S_ISBLK(s.st_mode) || S_ISLNK(s.st_mode)){
		//printf("  is's a file\n");
		int n=pathlen;
		while(--n){
			if(path[n-1]=='/'){
				break;
			}
		}
		int offset=n;
		name=(char*)malloc(pathlen-n+2);
		for(int o=0;n<pathlen;++n,++o){
			//printf("[%c]\n",path[n]);
			name[o]=path[n];
		}

				path[offset]='\0';
		//printf("after: [%s]\n",path);
		
			
	}else{
		fprintf(stderr,"tag: `%s\': Not a file or directory\n",target);
		free(path);
		return 0;
	}

	sprintf(path,"%s.tags",path);
	printf(".tag path: %s\n",path);
	printf("filename:  %s\n",name);


	//*
	FILE *ftags;
	struct info i;
	int ventries=0,rentries=0,existing=0,offset=-1,entries_offset=-1;


	if((ftags=fopen(path,"rb+"))!=NULL){
		printf("opening old file\n");
		fread(&i,sizeof(struct info),1,ftags);
		printf("magic number: [%x]\n",i.header);
		entries_offset=ftell(ftags);
		fread(&ventries,4,1,ftags);
		fread(&rentries,4,1,ftags);
		char filename[NAME_BUFFER_SIZE];
		for(int n=0;n<ventries;++n){
			int pos=ftell(ftags);
			memset(filename,'\0',NAME_BUFFER_SIZE);
			fread(filename,1,NAME_BUFFER_SIZE,ftags);
			printf("current row name: %s\n",filename);
			if(filename[0]=='\0' && offset==0){//found the first blank row
				offset=pos;
				printf("found an empty row at offset: %d\n",offset);
			}else if(!strcmp(name,filename)){//found the target row
				offset=pos;
				existing=1;

				printf("found the target's row at offset: %d\n",offset);
				break;
			}
			//didn't find a matching, or empty row
			fseek(ftags,TAG_COUNT*TAG_BUFFER_SIZE,SEEK_CUR);
			
		}
		if(offset==-1){
			printf("appending new row\n");
			offset=append_row(ftags);
			++ventries;
		}
		


	}else if((ftags=fopen(path,"wb+"))!=NULL){
		printf("creating new file\n");
		//struct info i;
		i.header=TAGFILE_MAGIC;
		i.vera=VERSION_MAJOR,i.verb=VERSION_MINOR,i.verc=VERSION_BUILD,i.verd=VERSION_REVISION;
		fwrite(&i,sizeof(struct info),1,ftags);
		
		//int entries_offset=ftell(ftags);

		entries_offset=ftell(ftags);
		fwrite(&ventries,4,1,ftags);
		fwrite(&rentries,4,1,ftags);
		offset=append_row(ftags);
		++ventries;
		printf("row offset: [%d]\n",offset);



	}else{
		fprintf(stderr,"tag: `%s\': Could not tag\n",target);
		return 0;
	}
	

	printf("row offset: [%d]\n",offset);
	if(tag_row(offset,ftags,name,tagc,tags)){//if data was added
		if(!existing){//if this row was just appended
			//--ventries;
			++rentries;
		}
	}else{//if no data was added (or removed)
		if(existing){
			//++ventries;
			--rentries;
		}
		
	}

		fseek(ftags,entries_offset,SEEK_SET);
		
		fwrite(&ventries,4,1,ftags);
		fwrite(&rentries,4,1,ftags);

	fclose(ftags);

}


























int query(char *target){

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
	
	char *path=(char*)malloc(strlen(target)+8);
	strcpy(path,target);
	int pathlen=strlen(path);
	char *name;
	//printf("before: [%s]\n",path);


	if(S_ISDIR(s.st_mode)){
		//printf("  is's a directory\n");
		if(path[pathlen-1]!='/'){
			path[pathlen]='/';
			path[pathlen+1]='\0';
		}
		//printf("after: [%s]\n",path);
		name=(char*)malloc(2);
		strcpy(name,".");
			//dir=

	}else if(S_ISREG(s.st_mode) || S_ISCHR(s.st_mode) || S_ISBLK(s.st_mode) || S_ISLNK(s.st_mode)){
		//printf("  is's a file\n");
		int n=pathlen;
		while(--n){
			if(path[n-1]=='/'){
				break;
			}
		}
		int offset=n;
		name=(char*)malloc(pathlen-n+2);
		for(int o=0;n<pathlen;++n,++o){
			//printf("[%c]\n",path[n]);
			name[o]=path[n];
		}

				path[offset]='\0';
		//printf("after: [%s]\n",path);
		
			
	}else{
		fprintf(stderr,"tag: `%s\': Not a file or directory\n",target);
		free(path);
		return 0;
	}

	sprintf(path,"%s.tags",path);
	//printf(".tag target: %s\n",path);
	//printf("real target: %s\n",name);


	FILE *ftags;
	if((ftags=fopen(path,"r"))==NULL){return 0;}
	
	struct info i;

	fread(&i,1,sizeof(struct info),ftags);
	//printf("Header: [%x]\n",tf->t_info.header);
	if(i.header!=TAGFILE_MAGIC){return 0;}

	int ventries=0,rentries=0;
	fread(&ventries,1,4,ftags);
	fread(&rentries,1,4,ftags);
	
	char rowname[NAME_BUFFER_SIZE+1],rowtag[TAG_BUFFER_SIZE+1];
	rowname[NAME_BUFFER_SIZE]=rowtag[TAG_BUFFER_SIZE]='\0';

	for(int n=0;n<ventries;++n){
		fread(rowname,1,NAME_BUFFER_SIZE,ftags);
		if(strcmp(rowname,name)){
			fseek(ftags,TAG_BUFFER_SIZE*TAG_COUNT,SEEK_CUR);
			continue;
		}
		for(int c=0;c<TAG_COUNT;++c){
			fread(rowtag,1,TAG_BUFFER_SIZE,ftags);
			if(rowtag[0]!='\0'){
				printf("%s\n",rowtag);
			}
		}
		break;
	}
	
	

	free(name);
	free(path);
	return 0;
}







int search_tagfile(const char *,FILE*,int,char**);

int search_tagfile(const char *path,FILE *ftags,int tagc,char **tags){

	struct info i;
	char name[NAME_BUFFER_SIZE+1],tag[TAG_BUFFER_SIZE+1];
	

	fread(&i,1,sizeof(struct info),ftags);
	//printf("Header: [%x]\n",tf->t_info.header);
	if(i.header!=TAGFILE_MAGIC){return 0;}

	int ventries=0,rentries=0;
	fread(&ventries,1,4,ftags);
	fread(&rentries,1,4,ftags);

	for(int n=0;n<ventries;++n){
		int offset=ftell(ftags);
		fread(name,1,NAME_BUFFER_SIZE,ftags);
		for(int m=0;m<TAG_COUNT;++m){
			fread(tag,1,TAG_BUFFER_SIZE,ftags);
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














void search(const char *path,char **tags,int size){


	struct dirent *dp;
	struct stat s;
	DIR *dir;

	if((dir=opendir(path))==NULL){return;}

	while((dp=readdir(dir))!=NULL){

		char *temp=(char*)malloc(strlen(path)+strlen(dp->d_name)+4);
		sprintf(temp,"%s%s",path,dp->d_name);

		lstat(temp,&s);

		if(S_ISREG(s.st_mode)){//if regular file
			if(!strcmp(dp->d_name,".tags")){
				//printf("parsing: [%s]\n",temp);
				FILE *ftags;
				if((ftags=fopen(temp,"r"))!=NULL){search_tagfile(path,ftags,size,tags);}
				fclose(ftags);

			}
		}else if(S_ISDIR(s.st_mode) && !S_ISLNK(s.st_mode)){//if directory
			if(strcmp(dp->d_name,".") && strcmp(dp->d_name,"..")){
				sprintf(temp,"%s/",temp);
				search(temp,tags,size);
			}
		}


		free(temp);

	}
	closedir(dir);


}







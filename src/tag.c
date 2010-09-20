#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "tfmanip.h"
#include "res.h"

void search(const char *,const char **,int);
int query_file(const char*);




int main(int argc,char **argv){

	if(argc<2){
		fprintf(stderr,"tag: mising operand\n");
		fprintf(stderr,"Try `tag --help' for more information.\n");
		return 0;
	}
	
	if(!strcmp(argv[1],"--help")){
		printf(helptext);
		return 0;
	}
	
	if(!strcmp(argv[1],"--version")){
		printf(versiontext,VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD);
		return 0;
	}

	if(argc<3){
		fprintf(stderr,"tag: mising operand\n");
		fprintf(stderr,"Try `tag --help' for more information.\n");
		return 0;
	}

	if(!strcmp(argv[1],"-t")){//tag a file
			tag_tagfile(argv[argc-1],(const char **)argv+2,argc-3);

	}else if(!strcmp(argv[1],"-f")){//find file(s)
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

			int len=strlen(argv[argc-1]);
			char *path=malloc((len+4)*sizeof(*path));
			strcpy(path,argv[argc-1]);
			if(path[len-1]!='/'){
				strcat(path,"/");
			}
			search(path,(const char **)argv+2,argc-3);
			free(path);path=NULL;

	}else if(!strcmp(argv[1],"-d")){//dump file tags
		dump_tagfile(argv[argc-1]);
		

	}else if(!strcmp(argv[1],"-q")){//query file tags
		query_file(argv[argc-1]);

	}else{
		fprintf(stderr,"tag: invalid operand\n");
		fprintf(stderr,"Try `tag --help' for more information.\n");
	}


	return 0;
}




int query_file(const char *target){

	char *path,*name,*tagfile;
	if(!extract_paths(target,&path,&name,&tagfile)){
		return 0;
	}
	
	FILE *ftags;
	if((ftags=fopen(tagfile,"rb"))==NULL){return 0;}

	query_tagfile(name,ftags);

	fclose(ftags);
	free(name);name=NULL;
	free(path);path=NULL;
	free(tagfile);tagfile=NULL;

	return 1;
}




#ifdef _DIRENT_HAVE_D_TYPE
	#ifndef __USE_BSD
		#define DT_UNKNOWN	0
		#define DT_FIFO	    1
		#define DT_CHR		  2
		#define DT_DIR		  4
		#define DT_BLK		  6
		#define DT_REG		  8
		#define DT_LNK		 10
		#define DT_SOCK	   12
		#define DT_WHT		 14
	#endif




void search(const char *path,const char **tags,int tagc){
	struct dirent *dp;
	DIR *dir;

	if((dir=opendir(path))==NULL){return;}

	while((dp=readdir(dir))!=NULL){

		if(dp->d_type==DT_REG){//if it's a regular file
			if(strcmp(dp->d_name,TAGFILE_FILENAME))continue;

			char *temp=malloc((strlen(path)+strlen(dp->d_name)+1)*sizeof(*temp));//lots of wasted cycles
			sprintf(temp,"%s%s",path,dp->d_name);
			FILE *ftags;
			if((ftags=fopen(temp,"rb"))!=NULL){search_tagfile(path,tags,tagc,ftags);}
			fclose(ftags);
			free(temp);

		}else if(dp->d_type==DT_DIR){//if it's a directory
			if(!strcmp(dp->d_name,".") || !strcmp(dp->d_name,".."))continue;

			char *temp=malloc((strlen(path)+strlen(dp->d_name)+2)*sizeof(*temp));//lots of wasted cycles
			sprintf(temp,"%s%s/",path,dp->d_name);
			search(temp,tags,tagc);
			free(temp);

		}

	}
	closedir(dir);
}




#else




void search(const char *path,const char **tags,int tagc){



	struct dirent *dp;
	//struct stat s;
	DIR *dir;

	if((dir=opendir(path))==NULL){return;}

	while((dp=readdir(dir))!=NULL){

		if(!strcmp(dp->d_name,".") || !strcmp(dp->d_name,".."))continue;
	
		char *temp=malloc((strlen(path)+strlen(dp->d_name)+4)*sizeof(*temp));//lots of wasted cycles (;_;)
		sprintf(temp,"%s%s",path,dp->d_name);

		lstat(temp,&s);

		if(S_ISREG(s.st_mode)){//if it's regular file
			if(!strcmp(dp->d_name,TAGFILE_FILENAME)){
				FILE *ftags;
				if((ftags=fopen(temp,"rb"))!=NULL){search_tagfile(path,tags,tagc,ftags);}
				fclose(ftags);

			}
		}else if(S_ISDIR(s.st_mode) && !S_ISLNK(s.st_mode)){//if it's a directory
				strcat(temp,"/");
				search(temp,tags,tagc);
		}

		free(temp);


	}
	closedir(dir);


}

#endif


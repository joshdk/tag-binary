#ifndef FILESYS_C
#define FILESYS_C

#include "filesys.h"

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

	if(tdata.virt>=DEFRAG_THRESHOLD && tdata.virt!=0){
		if(((double)tdata.real)/((double)tdata.virt) <= DEFRAG_RATIO){//too many empty rows in the file, time to defrag it
			fseek(ftags,sizeof(struct header)+sizeof(struct rowinfo)+sizeof(struct table),SEEK_SET);
			defrag_tagfile(&tdata,&ri,ftags);
		}
	}
	//update size of virual and real entries
	fseek(ftags,sizeof(struct header)+sizeof(struct rowinfo),SEEK_SET);
	fwrite(&tdata,sizeof(struct table),1,ftags);

	fclose(ftags);
	free(path);path=NULL;
	free(name);name=NULL;
	free(tagfile);tagfile=NULL;
	return 1;
}





/*
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
//*/








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



/*
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
//*/

#endif 

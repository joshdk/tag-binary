#ifndef TAGDB_C
#define TAGDB_C

#include "tagdb.h"





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
				if(strstr(rowdata->tags[m],tags[n]+1)){//if this tag matches
					//printf("(-) %s\n",rowdata->tags[m]);
					memset(rowdata->tags[m],'\0',ri->tag_buffer_size);//zero out the tag
				}
			}

		}else if(tags[n][0]=='.'){//remove a tag from the list via partial match
			for(int m=0;m<ri->tag_count;++m){
				if(!rowdata->tags[m][0])continue;
				if(!strstr(rowdata->tags[m],tags[n]+1)){//if this tag matches
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







int defrag_tagfile(struct table *tdata,struct rowinfo *ri,FILE *ftags){
	int read_pointer=ftell(ftags);
	int write_pointer=read_pointer;
	int row_buffer_size=ri->name_buffer_size+ri->tag_count*ri->tag_buffer_size;

	//printf("defragging\n");

	struct row *rowdata=create_rowdata(ri);

	for(unsigned int i=0;i<tdata->virt;++i){//for every table entry
		//rowdata->name[0]='\0';//zero it out

		fseek(ftags,read_pointer,SEEK_SET);
		read_row(rowdata,ri,ftags);//read one row
		//printf("read  @ %d - name: [%s]\n",read_pointer,rowdata->name);
		read_pointer+=row_buffer_size;
		fflush(ftags);

		if(rowdata->name[0]!='\0'){
			if(write_pointer!=read_pointer){//no reason to overwrite data with identical data...
				fseek(ftags,write_pointer,SEEK_SET);
				write_row(rowdata,ri,ftags);
				//printf("write @ %d - name: [%s]\n",write_pointer,rowdata->name);
				write_pointer+=row_buffer_size;
				fflush(ftags);				
			}
		}
	}
	
	destroy_rowdata(rowdata,ri);
	//printf("truncating to [%d] bytes\n",write_pointer);
	if(ftruncate(fileno(ftags),write_pointer)!=0){return 0;}

	tdata->virt=tdata->real;
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
					if(strstr(rowdata->tags[m],tags[n]+1)){//found a match via a partial tag
						valid=1;break;
					}
				}
				
			}else if(tags[n][0]=='.'){//partial, contains absolutely none
				for(int m=0;m<ri.tag_count;++m){
					if(strstr(rowdata->tags[m],tags[n]+1)){//found a match via a partial tag
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



#endif

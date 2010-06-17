#ifndef _TAGFILE_H_
#define _TAGFILE_H_
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOWORD(a) ((short)(a))
#define HIWORD(a) ((short)(((int)(a) >> 16) & 0xFFFF))

#define LOBYTE(a) ((char)(a))
#define HIBYTE(a) ((char)(((short)(a) >> 8) & 0xFF))
//*/


/*
struct item{
	int count;
	//char *file;
	char *file,**tags;
};


struct tagfile{
	struct info t_info;
	unsigned int t_size;
	struct item *t_items;
};


int print_tags(struct tagfile *,char *);
void print_item(struct item *);
void print_tagfile(struct tagfile *);

void destroy_item(struct item *);
void destroy_tagfile(struct tagfile *);









int parse(struct tagfile *tf,FILE *f){

	fread(&tf->t_info,1,8,f);
	printf("Header: [%x]\n",tf->t_info.header);
	if(tf->t_info.header^0x73676174)return 0;
	//printf("header: %d\nversion: %d\n",tf->t_info.header,tf->t_info.version);//1936154996, 0.0.0.0
	
	//int count=0;
	int ventries=0;
	fread(&ventries,1,4,f);
	fread(&tf->t_size,1,4,f);
	//printf("entries: %x\n",tf->t_size);
	//tf->t_size=count;	

	//struct item *items=(struct item *)malloc(sizeof(struct item)*count);
	tf->t_items=(struct item *)malloc(sizeof(struct item)*tf->t_size);
	for(int n=0,o=0;n<ventries;++n){

		char *filename=(char*)malloc(256+1);
		filename[256]='\0';
		fread(filename,1,256,f);
		if(filename[0]=='\0'){free(filename);continue;}

		
		
		//unsigned char len=0;
		//fread(&len,1,1,f);
		//items[n].file=(char*)malloc(len+1);
		tf->t_items[o].file=filename;//(char*)malloc(len+1);
		//tf->t_items[n].file[len]='\0';
		//fread(tf->t_items[n].file,1,len,f);
		printf("  file: %s\n",tf->t_items[n].file);		


		//printf("item #%d:\n",n+1);
		//unsigned int c=0;
		//fread(&items[n].count,1,4,f);
		//fread(&tf->t_items[o].count,1,1,f);
		//printf("  tags: %d\n",tf->t_items[n].count);
		//items[n].tags=(char**)malloc(items[n].count);
		tf->t_items[o].tags=(char**)malloc(16);//tf->t_items[n].count);
		int tagcount=0;
		for(int m=0;m<16;++m){
			//unsigned char len=0;
			//fread(&len,1,1,f);
			//items[n].tags[m]=(char*)malloc(len+1);
			char *tag=(char*)malloc(17);
			tag[16]='\0';
			fread(tag,1,16,f);
			if(tag[0]=='\0'){free(tag);continue;}

			tf->t_items[o].tags[tagcount]=tag;//(char*)malloc(17);
			
			//tf->t_items[o].tags[m][16]='\0';
			//printf("  [%s]\n",tf->t_items[n].tags[m]);
			++tagcount;
		}
		tf->t_items[o].count=tagcount;
		printf("  tags: %d\n",tf->t_items[o].count);
		++o;


	}

return 1;
}
















void print_item(struct item *i){
	printf("file: %s\n",i->file);	
	for(int n=0;n<i->count;n++){
		printf("  %s\n",i->tags[n]);
	}
}

void print_tagfile(struct tagfile *tf){
	int h=tf->t_info.header;
	//printf("header: %X\n",tf->t_info.header);
	printf("header: %c%c%c%c\n",LOBYTE(LOWORD(h)),HIBYTE(LOWORD(h)),LOBYTE(HIWORD(h)),HIBYTE(HIWORD(h)));
	//int v=tf->t_info.version;
	//printf("version: %8x\n",v);
	//printf("version: %d.%d.%d.%d\n",LOBYTE(LOWORD(v)),HIBYTE(LOWORD(v)),LOBYTE(HIWORD(v)),HIBYTE(HIWORD(v)));
	printf("version: %d.%d.%d.%d\n",tf->t_info.vera,tf->t_info.verb,tf->t_info.verc,tf->t_info.verd);
	
	for(int n=0;n<tf->t_size;n++){
		print_item(&tf->t_items[n]);
		//printf("  %s\n",i->tags[n]);
	}
}





void destroy_item(struct item *i){
	for(int n=0;n<i->count;++n){
		free(i->tags[n]);
	}
	free(i->tags);
	free(i->file);
}


void destroy_tagfile(struct tagfile *tf){
	for(int n=0;n<tf->t_size;++n){
		destroy_item(&tf->t_items[n]);
	}
	free(tf->t_items);
}


int print_tags(struct tagfile *tf,char *file){
	for(int n=0;n<tf->t_size;++n){
		if(strcmp(tf->t_items[n].file,file))continue;
		for(int m=0;m<tf->t_items[n].count;++m){
			printf("%s\n",tf->t_items[n].tags[m]);
		}
		return 1;
	}
	return 0;
}

//*/











#endif

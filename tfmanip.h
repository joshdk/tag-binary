#ifndef _TFMANIP_H_
#define _TFMANIP_H_


#define TAGFILE_MAGIC			0x73676174 //"tags"

#define NAME_MAX 					255
#define NAME_BUFFER_SIZE 	256

#define TAG_COUNT 				16
#define TAG_BUFFER_SIZE 	16

#define ROW_BUFFER_SIZE		(NAME_BUFFER_SIZE+TAG_BUFFER_SIZE*TAG_COUNT)


struct info{
	int header;
	unsigned char vera,verb,verc,verd;
};

struct row{
	char name[NAME_BUFFER_SIZE+1],tags[TAG_COUNT][TAG_BUFFER_SIZE+1];
};



#endif

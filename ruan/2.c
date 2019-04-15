#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <assert.h>

int duptwo(int filedesc_src,int filedesc_dst)
{
	int index =0;
	assert(filedesc_dst<OPEN_MAX);
	int filedesc_temp[filedesc_dst];
	char tempbuf[100];
	sprintf(tempbuf,"/dev/fd/%d",filedesc_src);

	int ret = -1;
	ret = open(tempbuf,O_RDONLY);
	assert(ret>=0);
	if(ret == filedesc_dst) return ret;

	close(ret);

	while(index<filedesc_dst-1) 
		tempbuf[index++] = -1;

	index = 0;
	while(ret=dup(filedesc_src))
	{
		if(ret==filedesc_dst) break;
		tempbuf[index++] = ret;
	}

	while(--index>0 ) close(tempbuf[index]);
	return ret;
}

int main(int argc, char **argv)
{
	assert(argc>1);
	int fd= open(*++argv,O_RDWR|O_CREAT|O_EXCL,0777);
	assert(fd>=0);
	int fd2 = duptwo(fd,10);
	printf("%d\n",fd2);
	close(fd);

	return 0;
}
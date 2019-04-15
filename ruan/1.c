#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define MAX_PATH 1024

void traverse(const char *dir,int filedesc)
{
	char name[MAX_PATH];
	struct dirent *dp;
	DIR *dfd;

	if ((dfd = opendir(dir)) == NULL) {
		fprintf(stderr, "dirwalk: can't open %s\n", dir);
		return;
	}
	while ((dp = readdir(dfd)) != NULL) {
		if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) continue; 
		if (strlen(dir)+strlen(dp->d_name)+2 > sizeof(name))
			fprintf(stderr, "dirwalk: name %s %s too long\n",
				dir, dp->d_name);
		else {
			sprintf(name, "%s/%s", dir, dp->d_name);
			struct stat stbuf;

			if (stat(name, &stbuf) == -1) {
				fprintf(stderr, "fsize: can't access %s\n", name);
				return;
			}
			if ((stbuf.st_mode & S_IFMT) == S_IFDIR)
				traverse(name,filedesc);
			char tempbuf[MAX_PATH];
			sprintf(tempbuf,"%15ld %s\n",stbuf.st_size,name);
			write(filedesc,tempbuf,strlen(tempbuf));
		}
	}
	closedir(dfd);
}

int main(int argc, char **argv)
{
	assert(argc>1);
	int fd;
	if((fd=open(*++argv,O_RDWR|O_CREAT|O_TRUNC))<0) perror("open error");
	if(argc==2){
		dirwalk(".",fd);
		return 0;
	}
	while (--argc - 1> 0)
		dirwalk(*++argv,fd);
	close(fd);
	return 0;
}
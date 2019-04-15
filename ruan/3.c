#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>

int main(int argc, char **argv)
{
	assert(argc>1);
	++argv;
	int opret = -1;
	if(strcmp(*argv,"CreateDir")==0){
		assert(argc>2);
		opret = mkdir(*++argv,atoi(*++argv));
	}else if(strcmp(*argv,"DeleteDir")==0){
		opret = rmdir(*++argv);
	}else if(strcmp(*argv,"Rename")==0){
		assert(argc>2);
		opret = rename(*++argv,*++argv);
	}else if(strcmp(*argv,"Chomd")==0){
		assert(argc>2);
		++argv;
		char* filename = *argv++;
		char* temp = *argv;
		if(temp[0]>'0' && temp[0]<'9'){
			opret = chmod(filename,atoi(temp));
			printf("%c\n",temp[0]);
			return 0;
		}	
		struct stat buf;
		stat(*++argv,&buf);
		mode_t mode =buf.st_mode;
		while(*temp){
			if(*temp=='u'){
				while(*temp!='_')
				{
					if(*temp=='+'){
						if(*temp++){
							if(*temp=='r')
								mode|=S_IRUSR;
							else if(*temp == 'w')
								mode|=S_IWUSR;
							else if(*temp == 'x')
								mode|=S_IXUSR;
						}
					}else if(*temp=='-'){
						if(*temp++){
							if(*temp=='r')
								mode&=~S_IRUSR;
							else if(*temp=='w')
								mode&=~S_IWUSR;
							else if(*temp=='x'){
								mode&=~S_IXUSR;
							}
						}
					}
					temp++;
				}
			}else if(*temp=='g'){
				while(*temp!='_')
				{
					if(*temp=='+'){
						if(*temp++){
							if(*temp=='r')
								mode|=S_IRGRP;
							else if(*temp == 'w')
								mode|=S_IWGRP;
							else if(*temp == 'x')
								mode|=S_IXGRP;
						}
					}else if(*temp=='-'){
						if(*temp++){
							if(*temp=='r')
								mode&=~S_IRGRP;
							else if(*temp=='w')
								mode&=~S_IWGRP;
							else if(*temp=='x'){
								mode&=~S_IXGRP;
							}
						}
					}
					temp++;
				}
			}else if(*temp=='o'){
				while(*temp!='_')
				{
					if(*temp=='+'){
						if(*temp++){
							if(*temp=='r')
								mode|=S_IROTH;
							else if(*temp == 'w')
								mode|=S_IWOTH;
							else if(*temp == 'x')
								mode|=S_IXOTH;
						}
					}else if(*temp=='-'){
						if(*temp++){
							if(*temp=='r')
								mode&=~S_IROTH;
							else if(*temp=='w')
								mode&=~S_IWOTH;
							else if(*temp=='x'){
								mode&=~S_IXOTH;
							}
						}
					}
					temp++;
				}
			}
			temp++;
		}
		opret = chmod(filename,mode);
	}else if(strcmp(*argv,"Access")==0){
		int r=-1,w=-1,x=-1,e=-1;
		++argv;
		if(!access(*argv,F_OK)) e = 1; 
		if(!access(*argv,W_OK)) w = 1;
		if(!access(*argv,R_OK)) r = 1;
		if(!access(*argv,X_OK)) x = 1;
		if(e<0) {printf("file is not exist!"); return 0;}
		printf("%s asscess: %s%s%s\n",*argv,r>0?"r":"-",w>0?"w":"-",x>0?"x":"-");
	}else if(strcmp(*argv,"State")==0){
		struct stat buf;
		stat(*++argv,&buf);
		char type[100];
		if(S_ISREG(buf.st_mode)){
			strcpy(type,"regular_file\0");
		}else if(S_ISDIR(buf.st_mode)){
			strcpy(type,"directory_file\0");
		}else if(S_ISCHR(buf.st_mode)){
			strcpy(type,"character_spcial_file\0");
		}else if(S_ISBLK(buf.st_mode)){
			strcpy(type,"block_special_file\0");
		}else if(S_ISFIFO(buf.st_mode)){
			strcpy(type,"fifo_file\0");
		}else if(S_ISLNK(buf.st_mode)){
			strcpy(type,"link_file\0");
		}else if(S_ISSOCK(buf.st_mode)){
			strcpy(type,"socket_file\0");
		}else strcpy(type,"unknow_file\0");
		char mtime[64],atime[64],ctime[64];
		struct tm *t = gmtime(&buf.st_mtimespec.tv_sec);
		strftime(mtime,sizeof(time),"%Y-%m-%d %H:%M:%S",t);
		t = gmtime(&buf.st_atimespec.tv_sec);
		strftime(atime,sizeof(atime),"%Y-%m-%d %H:%M:%S",t);
		t = gmtime(&buf.st_ctimespec.tv_sec);
		strftime(ctime,sizeof(ctime),"%Y-%m-%d %H:%M:%S",t);

		printf("size: %ld  type: %s modify_time: %s asscess_time: %s change_time:%s \n",
			buf.st_size,type,
			mtime,atime,ctime);
	}

	return 0;
}
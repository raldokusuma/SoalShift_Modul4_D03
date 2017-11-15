#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
 #include <sys/stat.h>
 #include <sys/types.h>


const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}


static const char *dirpath = "/home/raldo/Documents";


static int xmp_getattr(const char *path, struct stat *stbuf)
{
  	int res;
  	char cb[100];
	char fpath[1000];

	sprintf(fpath,"%s%s",dirpath,path);

	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  	char fpath[1000];
  	char cb[1000];


	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);

	}
	else {
		sprintf(fpath, "%s%s",dirpath,path);

	}
	int res = 0;
    int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);

	return res;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  	char fpath[1000];
	char cb[500];
	char tmp[100];
	int hm2, hm3;
	if(strcmp(path,"/") == 0)
	{

		path=dirpath;
		sprintf(fpath,"%s",path);

	}
	else {
		sprintf(fpath, "%s%s",dirpath,path);

	}

	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;
	FILE *rrah;
	struct stat he;
	mode_t modeku;
	modeku = he.st_mode & 000;
	
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		int hm;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
		
		if(res!=0) break;

		if(strcmp(get_filename_ext(de->d_name),"pdf")==0 || strcmp(get_filename_ext(de->d_name),"txt")==0  || strcmp(get_filename_ext(de->d_name),"doc")==0 ){
			system(cb);

  			hm2 = mkdir("/home/raldo/Documents/rahasia",0777);
  			char oldname[100];
 		    char newname[100];
 		    //char check[1000];
			strcpy(oldname,de->d_name);
			sprintf(oldname,"%s/%s",dirpath,de->d_name);
			sprintf(newname,"%s/rahasia/%s.ditandai",dirpath,de->d_name );   
 		  	//sprintf(check,"notify-send '%s %s %s'",de->d_name, oldname,newname );
 		  	//system(check);
 		  	
 			
 			hm = rename(oldname, newname);
 			hm3 = chmod(newname,0000);
 			//hm3= open(newname, O_RDWR|O_CREAT, 0000);
 			//fclose(rrah);
		}
	}

	closedir(dp);
	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read

};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
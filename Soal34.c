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
#include <sys/types.h>

static const char *dirpath = "/home/raldo/Downloads";
char check1[500];
char check2[500];
char loc[200];
FILE *f1;
FILE *f2;
FILE *cp2;
char cr[]=".copy";
char *hsl;

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char nama[500];
  char fpath[1000];
  int jj;
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

char oldname[100];
char newname[100];

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  	char fpath[1000];
	
	if(strstr(path,cr) != NULL){	
		system("notify-send 'File yang anda buka adalah file hasil salinan. File tidak bisa diubah maupun disalin kembali!'");
		return -errno;
	}

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	
	f1 = fopen (fpath, "r");
	fgets(check1, 500, f1);	

	int res = 0;
 	int fd = 0 ;

	(void) fi;



	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	fclose(f1);
	close(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	char cb[200];

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

	int fd;
	int res;

	(void) fi;
	
	
	fd = open(fpath, O_WRONLY);
	

	if (fd == -1)
		return -errno;

	if(strstr(path,cr) != NULL){
		return -errno;
	}

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;


	f2 = fopen (fpath, "r");

	fgets(check2, 500, f2);
	int hm,ha,he;
	if(strcmp(check1,check2)!=0 && strstr(path,cr) == NULL){
			
		system("notify-send 'terjadi perubahan'");
		hm = mkdir("/home/raldo/Downloads/simpanan",0777);

	    sprintf(oldname,"/home/raldo/Downloads%s",path);
		sprintf(newname,"/home/raldo/Downloads/simpanan%s.copy",path);
		
		cp2 = fopen(newname,"w+");
		fprintf(cp2, "%s", check2);
		fclose(cp2);
		chmod(newname,0000);
	}


	fclose(f2);
	close(fd);

	return res;
}

static int xmp_truncate(const char *path, off_t size)
{

	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	
	int res;

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}


static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.write 		= xmp_write,
	.truncate  	= xmp_truncate
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
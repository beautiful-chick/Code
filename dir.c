#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include <fcntl.h>

#include <unistd.h>


#define TEST_DIR	 "dirmmmmmm"

int main(int argc,char **argv)
{
	int		rv;
	int		fd1;
	int		fd2;
	DIR		*dirp;
	struct dirent	*direntp;

	/*创建文件夹dirent，并设置文件夹权限为755*/
	if(mkdir(TEST_DIR,0755)<0)
	{
		printf("create direnty '%s' failure:%s\n",TEST_DIR,strerror(errno));
		return -1;
	}

	/*更改当前路径到文件夹dir下去*/
	if(chdir(TEST_DIR)<0)
	{
		printf("Change directory to '%s' failure: %s\n",TEST_DIR,strerror(errno));
		rv = -2;
		goto cleanup;
	}

	/*在dir文件夹下，创建普通文本文件file1.txt，设置其文件权限644*/
	if((fd1 = creat("file1.txt",0644))<0)
	{
		printf("Create file1.txt failure : %s\n",strerror(errno));
		rv = -3;
		goto cleanup;

	}

	/*在dir文件下创建普通文本文件file2.txt，设置文件权限为644*/
	if((fd2 = creat("file2.txt",0644))<0)
	{
		printf("Create file2.txt failure :%s\n",strerror(errno));
		rv = -4;
		goto cleanup;
	}	

	/*更改当前目录到父目录*/
	if(chdir("../")<0)
	{
		printf("Change directory to '%s' failure :%s\n",TEST_DIR,strerror(errno));
		rv  = -5;
		goto cleanup;
	}

	/*打开dir文件夹*/
	if((dirp = opendir(TEST_DIR)) == NULL)
	{
		rv = -6;
		printf("Opendir %s error :%s\n",TEST_DIR,strerror(errno));
		goto cleanup;
	}

	/*列出dir里面所有的文件和文件夹*/
	while((direntp = readdir(dirp)) != NULL)
	{
		printf("Find file :%s\n",direntp -> d_name);

	}

	/*关闭所有打开的文件夹*/
	closedir(dirp);
cleanup:
	if(fd1 >= 0)
	{
		close(fd1);
	}
	if(fd2 >= 0)
	{
		close(fd2);
	}

}

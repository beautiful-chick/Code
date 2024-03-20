
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include "ds18b20.h"
#if 1
int ds18b20_get_temperature(float *temp);

int ds18b20_get_temperature(float *temp)
{
	int 		fd = -1;
	int 		found = 0;
	int 		rv = 0;
	char	       *ptr = NULL;
	DIR	       *dirp = NULL;
	struct dirent  *direntp = NULL;
	const char     *w1_path = "/sys/bus/w1/devices/";
char   buf[128];
char    chip[32];
char    ds_path[64];

       if (NULL == temp )
	{
   		return -1;
        }

        if (NULL == (dirp=opendir(w1_path)))
        {
                printf("opendir error: %s\n",strerror(errno));
		return -2;
	}

        while (NULL != (direntp = readdir(dirp)))
	{	
		if (strstr(direntp->d_name,"28-"))

	
	{        strncpy(chip,direntp->d_name,sizeof(chip));
                 found = 1;
       	         break;
																		                }
					        }
        closedir(dirp);

	        if(!found)
	        {	
			printf("can not find DS18B20 in %s\n,w1_path");
			return -3;
		}

        snprintf(ds_path,sizeof(ds_path),"%s/%s/w1_slave",w1_path,chip);
		if ( (fd=open(ds_path, O_RDONLY)) < 0 )
	{
		printf("open %s error: %s\n", ds_path, strerror(errno));
		return -4;
	}

       memset(buf, 0, sizeof(buf));
	        if ( read(fd,buf,sizeof(buf))<0)
	        {
			printf ("read %s error. %s\n", ds_path,strerror(errno));
			rv=-5;
			goto cleanup;
									        }

                if (NULL == (ptr = strstr(buf, "t=")) )
	        {
			printf("ERROR:Can not get temperture\n");
			rv = -6;
			goto cleanup;

		}

		ptr += 2;

		*temp = atof(ptr) / 1000;
		cleanup:
		close(fd);
		return rv;
	}
#endif

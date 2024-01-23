#include <stdio.h>
#include <string.h>
#include <time.h>

#include "get_time.h"


double gettime(char *data_time)
{
	time_t   timep;
	struct   tm* st;
	double   last_time;

	time(&timep);
	st=gmtime(&timep); 
	memset(data_time,0,sizeof(data_time));
	snprintf(data_time,50,"%04d-%02d-%02d %02d:%02d:%02d",(1900+st->tm_year),(1+st->tm_mon),st->tm_mday,(st->tm_hour+8),st->tm_min,st->tm_sec);
						    
	return last_time;
}   
    


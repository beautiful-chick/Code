/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  Project_client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(20/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "20/03/24 12:37:34"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<getopt.h>
#include<libgen.h>
#include<sqlite3.h>
#include<netinet/tcp.h>
//#include<sqlite3ext.h>
int			TIMEOUT;
float		get_temperature();
char 		*get_name();
char		*get_time();
int 		internet_connect();
int			re_connect();
int			internet_write();
int			internet_read();
static inline void print_usage(char *progname);
char		buf2[1024];
char		*now_time;
int			conn_fd = -1;
char		sock_buf[1024];
int			rv = -1;
int			cli_port;
char   		*cli_ip;
struct sockaddr_in		serv_addr;
struct DS18B20_DATA
{
	char            d_time[64];
	float           d_temp;
	char            d_name[64];
}data;
static char                       snd_buf[2048] = {0};
int dev_sqlite3(struct DS18B20_DATA data);
int read_data(int rc,char *snd_buf);
int main(int argc,char **argv)
{
	char				*snd_buf;
	int			opt;
	char		*progname = NULL;
	float 		dev_temp;
	char		*dev_name;
	char		*dev_time;
	progname = (char *)basename(argv[0]);
	struct option		long_options[] = 
	{
		{"port",required_argument,NULL,'p'},
		{"IP Address",required_argument,NULL,'i'},
		{"time",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}

	};
	while( (opt = getopt_long(argc,argv,"p:i:t:h",long_options,NULL)) != -1 )
	{
		switch(opt)
		{
			case 'p':
				cli_port = atoi(optarg);
				break;
			case 'i':
				cli_ip = optarg;
				break;
			case 't':
				TIMEOUT = atoi(optarg);
				break;
			case 'h':
				print_usage(progname);
				return 0;
			default:
				break;
		}
	}
	if( !TIMEOUT || !cli_ip || !cli_port)
	{
		print_usage(progname);
		return 0;
	}
	dev_temp = get_temperature();
	dev_name = get_name();
	dev_time = get_time();
	snprintf(data.d_name,32,dev_name);
	snprintf(data.d_time,32,dev_time);
	data.d_temp = dev_temp;
	internet_connect(data);
	internet_write();
	return 0;


}

float get_temperature(){
	

	int					fd;
	char				buf[1024];
	char				*ptr;
	float				temp;
	DIR					*dirp = NULL;
	char				w1_path[64] = "/sys/bus/w1/devices/";//定义写文件夹的指针

	char				ds18b20_path[164];
	struct dirent		*direntp = NULL;
	char				chip_sn[32];
	int					found = 0;
	/*打开文件夹*/
	
	dirp = opendir(w1_path);
	if( !dirp )
	{
		printf("Open the floder failure : %s\n",strerror(errno));
		return -1;
	}

	/*读取文件夹中内容*/

	while( NULL != (direntp = readdir(dirp)) )
	{
		/*查找存储温度的文件*/
		if( strstr(direntp -> d_name,"28-") )
		{
			strncpy(chip_sn,direntp -> d_name,sizeof(chip_sn));
			found = 1;
		}
	}
	closedir(dirp);

	if( !found )
	{
		printf("can not find ds18b20 chipset\n");
		return -2;
	}

	/*获取全路径到ds18b20_path中去*/
	strncat(w1_path,chip_sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));
	fd = open(w1_path,O_RDONLY);
	if( fd < 0 )
	{
		printf("Open the file failure : %s\n",strerror(errno));
		return -1;
	}

	memset(buf,0,sizeof(buf));

	if(read(fd,buf,sizeof(buf)) < 0 )
	{
		printf("Read data from fd = %d failure : %s\n",fd,strerror(errno));
		return -2;
	}

	ptr = strstr(buf,"t=");

	/*判断buf是否为空*/

	if( NULL == ptr )
	{
		printf("Can not find t = String\n");
		return -1;
	}
	ptr += 2;
	temp = atof(ptr)/1000;
	return temp;
}
char *get_name(){
	int				fd2;
	char			w2_path[64] = "/sys/bus/w1/devices/";
	char			chip_sn1[32];
	DIR				*dirp1 = NULL;
	struct dirent	*direntp1 = NULL;
	int				found1 = 0;
	dirp1 = opendir(w2_path);
	if( !dirp1 )
	{
		printf("Open the floder failure : %s\n",strerror(errno));
		return NULL;
	}

	while( NULL != (direntp1 = readdir(dirp1)))
	{
		if(strstr(direntp1 -> d_name,"28-"))
		{
			strncpy(chip_sn1,direntp1->d_name,sizeof(chip_sn1));
			found1 = 1;
		}
	}

	closedir(dirp1);
	if(!found1)
	{
		printf("Can not find name chipset\n");
	}
	strncat(w2_path,chip_sn1,sizeof(w2_path)-strlen(w2_path));
	strncat(w2_path,"/name",sizeof(w2_path)-strlen(w2_path));
	fd2 = open(w2_path,O_RDONLY);
	if( fd2 < 0 )
	{
		printf("Open the file about name failure : %s\n",strerror(errno));
		return NULL;
	}
	memset(buf2,0,sizeof(buf2));
	if( read(fd2,buf2,sizeof(buf2)) < 0 )
	{
		printf("get devices number failure : %s\n",strerror(errno));
		return NULL;
	}
	//printf("%s\n",buf2);
	return buf2;
}

char *get_time(){
	time_t			timer;
	struct tm		*Now;
	char			*now_time;

	time( &timer );
	Now = localtime( &timer );
	now_time = asctime(Now);
	return now_time;
}

int internet_connect(struct DS18B20_DATA data){
//	int							conn_fd = -1;
//	int							rv = -1;
//	char						sock_buf[1024];
//	struct sockaddr_in			serv_addr;
	char						snd_buf[2048] = {0};
	conn_fd = socket(AF_INET,SOCK_STREAM,0);	
	if( conn_fd < 0 )
	{
		printf("Create socket failure : %s\n",strerror(errno));
		return -1;
	}
	printf("create socket ok\n");
	memset(&serv_addr,0,sizeof(serv_addr));
//	printf("memset ok\n");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(cli_port);
//	printf("Set ok\n");
	inet_aton(cli_ip,&serv_addr.sin_addr);
//	printf("inet_aton() ok\n");
//	printf("okokokok\n");
	if( connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("Connect to server[%s:%d] failure : %s\n",cli_ip,cli_port,strerror(errno));
		printf("Start to reconnect\n");
		re_connect(data);
		printf("reconnect over\n");
		return -1;
	}
	return 0;
}
int internet_write(){
	//static char						snd_buf[2048] = {0};

	snprintf(snd_buf,2048,"%s,%s,%.2f",data.d_time,data.d_name,data.d_temp);
	if( write(conn_fd,snd_buf,strlen(snd_buf)) < 0 )
	{
		printf("Wirte data to server[%s:%d] failure : %s\n",cli_ip,cli_port,strerror(errno));
		goto cleanup;
	}
cleanup:
	close(conn_fd);
	return 0;
	}

int internet_read(){
	char 					sock_buf[1024];
//	int						rv = -1;
	memset(sock_buf,0,sizeof(sock_buf));
	rv = read(conn_fd,sock_buf,sizeof(sock_buf));
	if( rv < 0 )
	{
		printf("Read data from server failure : %s\n",strerror(errno));
		close(conn_fd);
	}
	else if( 0 == rv)
	{
		printf("Client connect to server failure : %s\n",strerror(errno));
	}
	printf("Read %d bytes data from server : '%s'\n",rv,sock_buf);
	return 0;
}


 int re_connect(int row_count)
{

	struct tcp_info				optval;
	socklen_t 					optlen = sizeof(optval);
	static sqlite3				*db;
	static int					rc;
	int							ret;
	ret = getsockopt(conn_fd,IPPROTO_TCP,TCP_INFO,&optval,&optlen);
	printf("ret:%d\n",ret);
	printf("optval:%d\n",optval.tcpi_state);	
	/*连接失败：继续获取数据，将数据存储在数据库中*/
		if(optval.tcpi_state != TCP_ESTABLISHED)
		{
			printf("disconnect\n");
			dev_sqlite3(data);
			printf("store ok\n");
	//		printf("%s %s %.2f\n",data.d_name,data.d_time,data.d_temp);
		}
		
		/*重连成功：将数据库中的数据上传，并删除数据库中的数据*/
		else
		{
			internet_write();
			/*删除表中内容*/
			for(int i = 0;i<=row_count;i++)
			{
				
			}

		}
		return 0;
}


int read_data(int rc,char *snd_buf)
{
	static int			row_count; 
	sqlite3				*db;
	char				*errmsg;
	sqlite3_stmt		*stmt;
	rc = sqlite3_open("dev_database.db",&db);
	if( rc != SQLITE_OK)
	{
		fprintf(stderr,"Can't open database : %s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	const char *sql = "SELECT * FROM dev_mesg;";
	rc = sqlite3_prepare_v2(db,sql,-1,&stmt,0);
	if( rc != SQLITE_OK)
	{
		fprintf(stderr,"Failed to prepare statement : %s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	while( (rc = sqlite3_step(stmt)) == SQLITE_ROW )
	{
		const char *de_name = (const char *)sqlite3_column_text(stmt,0);
		const char *de_time = (const char *)sqlite3_column_text(stmt,0);
		double de_temp = sqlite3_column_double(stmt,2);
		printf("de_name: %s, de_time: %s, de_temp: %.2f\n", de_name, de_time, de_temp);
		snprintf(snd_buf,2048,"%s,%s,%.2f",de_time,de_name,de_temp);
	}
	if (rc != SQLITE_DONE)
	{
		fprintf(stderr, "Error reading data: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return -1;
	}
	const char *sqll = "SELECT COUNT(*) FROM dev_mesg;";
	rc = sqlite3_prepare_v2(db,sqll,-1,&stmt,NULL);
	if (rc != SQLITE_OK)
	{
       fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
	   sqlite3_close(db);
	   return -1;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) 
	{
	   row_count = sqlite3_column_int(stmt, 0);
	   printf("Total rows in table: %d\n", row_count);
	}
	else 
	{
	   fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
	}



	
	 //snprintf(snd_buf,2048,"%s,%s,%.2f",de_time,de_name,de_temp);	
	 printf("%s\n",snd_buf);
	 sqlite3_finalize(stmt);
	 sqlite3_close(db);
	 return 0;
	

}
int dev_sqlite3(struct DS18B20_DATA data,int row_count)
{

	printf("start sqlite\n");
	int 		rc;
	sqlite3		*db;
	char 		*err_msg = 0;
	char		*sql = 0;
	rc = sqlite3_open("dev_database.db",&db);
	if( rc != SQLITE_OK)
	{
		fprintf(stderr,"Can not open database : %s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	printf("Open database ok\n");
	const char *sql_stmt = 
		"CREATE TABLE IF NOT EXISTS dev_mesg ("
		"de_name TEXT NOT NULL,"
		"de_time TEXT NOT NULL,"
		"de_temp REAL NOT NULL);";
	rc = sqlite3_exec(db,sql_stmt,0,0,&err_msg);
	if( rc != SQLITE_OK )
	{
		fprintf(stderr,"create table error:%s\n",err_msg);
		sqlite3_free(err_msg);
		return -1;
	}
	printf("Table create successfully\n");
	
	printf("%s %s %.2f\n",data.d_name,data.d_time,data.d_temp);
	//const char *sql = "INSERT INTO dev_mesg (de_name,de_time,de_temp) VALUES ('%s', '%s', '%.2f');";
	sql = sqlite3_mprintf("INSERT INTO dev_mesg (de_name,de_time,de_temp) VALUES ('%s','%s',%.2f);",data.d_name,data.d_time,data.d_temp);
	//const char *sql = "INSERT INTO dev_mesg (de_name, de_time, de_temp) VALUES ('%s', '%s', %.2f);";
	//sprintf(sql, "INSERT INTO dev_mesg (de_name, de_time, de_temp) VALUES ('%s', '%s', %.2f);", data.d_name, data.d_time, data.d_temp);


	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	 if (rc != SQLITE_OK) 
	 {
		fprintf(stderr, "give values error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return -1;
	}
	read_data(rc,snd_buf);
	printf("Record inserted successfully\n");
	sqlite3_close(db);

	return 0;
}

static inline void print_usage(char *progname)
{
	printf("Usage: %s [OPTION]...\n",progname);
	printf("%s is a socket server program,which used to verify client and echo back string from it\n",progname);
	printf("\nMandatory arguments to long options are mandatory for short options too:\n");
	printf("-i[IP Address]\n");
	printf("-p[Port]\n");
	printf("-t[time]\n");
	return ;
}

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "./common/mavlink.h"
#define BUFFER_SIZE 1024
#define BUFFER_SIZE_MAVLINK 270
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;
    switch( nSpeed )
        {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        }
    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch( nEvent )
    {
    case 'O':                     //奇校验
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':                     //偶校验
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':                    //无校验
        newtio.c_cflag &= ~PARENB;
        break;
    }


    if( nStop == 1 )
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if ( nStop == 2 )
    {
        newtio.c_cflag |=  CSTOPB;
    }
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
    printf("set done!\n");
    return 0;
}
int main()
{
	mavlink_status_t*  r_mavlink_status;
	 mavlink_message_t* msg;
	 r_mavlink_status=(mavlink_status_t*)malloc(BUFFER_SIZE_MAVLINK);
	 	 if(r_mavlink_status==NULL)
	 	 {
	 		 perror("malloc error");
	 		 exit(-1);
	 	 }
	 	msg=(mavlink_message_t*)malloc(BUFFER_SIZE_MAVLINK);
	 	 if(msg==NULL)
	 	 {
	 		 perror("malloc error");
	 		 exit(-1);
	 	 }
	int fd;
	fd = open( "/dev/ttyUL0" ,O_RDWR|O_NOCTTY|O_NDELAY);
	if (-1 == fd)
	   {
	        perror("Can't Open Serial Port");
	        exit(-1);
	    }
	 printf("open success\n");
	 if(isatty(STDIN_FILENO)==0)
	     {
	         printf("standard input is not a terminal device\n");
	     }
	     else
	     {
	         printf("isatty success!\n");
	     }
	     if(set_opt(fd,57600,8,'N',1)<0)
	     {
	         perror("set_opt error");
	         return 0;
	     }
	     char buf[BUFFER_SIZE_MAVLINK]={0};
	     		mavlink_user_rtcm_t cmd_mavlink;
	     		mavlink_rc_channels_override_t override_mavlink;
	     		mavlink_heartbeat_t heartbeat_mavlink;
	     		int nread=0,index=0;
	     		while(1){
	     	 nread=read(fd,buf,BUFFER_SIZE_MAVLINK);
	     //    	    printf("%s\n",buff);
	     	 for( index=0;index<nread;index++)
	     	 {
	     		 //把一个完整的mavlink包放到msg中
	     	 	  mavlink_parse_char(0,buf[index],msg,r_mavlink_status);
	     	  }
	     	 printf("recieve id:%d\n",msg->msgid);
	     	 if(msg->msgid==13)
	     	 {
	     		 //从mavlink数据包解析数据段
	     		 mavlink_msg_user_rtcm_decode(msg, &cmd_mavlink);
	     //		 if(write(new_sockfd,cmd_mavlink.rtcm_data, (msg->len-6)) < 0)
	     //		 {
	     //			 perror("Server Recieve Data Failed\n");
	     //			 exit(-1);
	     //		 }
	     //
	     		 bzero(buf,BUFFER_SIZE_MAVLINK);
	     		 printf("%d ",cmd_mavlink.rtcm_seq_total);
	     	  }
	     	 else if(msg->msgid==70)
	     	 {
	     		 mavlink_msg_rc_channels_override_decode(msg,&override_mavlink);
	     		 printf("%d %d %d %d ",override_mavlink.chan1_raw,override_mavlink.chan2_raw,override_mavlink.chan3_raw,override_mavlink.chan4_raw);
	     		bzero(buf,BUFFER_SIZE_MAVLINK);
	     	 }
	     	 else if(msg->msgid==0)
	     	 {
	     		 mavlink_msg_heartbeat_decode(msg,&heartbeat_mavlink);
	     		 printf("%d ",heartbeat_mavlink.type);
	     		bzero(buf,BUFFER_SIZE_MAVLINK);
	     	 }


	     	}
}

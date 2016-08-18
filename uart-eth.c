#include "uart-eth.h"
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
QUEUE  msg_quene1={0,0,0};
void quene_push(QUEUE *p, char data)
{
	if((p->tail+1)%BUFFER_SIZE_MAVLINK==p->head){
		printf("the queue is full\n");
		exit(-1);
	}
	p->data[p->tail]=data;
	p->tail=(p->tail+1)%BUFFER_SIZE_MAVLINK;
}
char quene_pop(QUEUE *p)
{
	if(p->tail==p->head){
		printf("the queue is empty\n");
		exit(-1);
	}
	p->head++;
	return p->data[p->head-1];
}
int quene_size(QUEUE *p)  // 队列中有效的元素
{
	if(p->tail<p->head)
	{
		return BUFFER_SIZE_MAVLINK+p->tail-p->head;
	}
	else
		return (p->tail-p->head);
}

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
        case 57600:
        	cfsetispeed(&newtio, B57600);
        	cfsetospeed(&newtio, B57600);
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
int set_eth(char *ip,int portnumber)
{
	int sockfd;
	struct sockaddr_in server_addr;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("Socket Error:");
		exit(-1);
	}
	printf("Socket Success\n");
	/*填充服务器端sockaddr地址结构*/
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(portnumber);
	// 设置套接字选项避免地址使用错误
	int on = 1;
	if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
		perror("setsockopt failed\n");
		exit(EXIT_FAILURE);
	}
	/*绑定*/
	if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))< 0)
	{
		perror("Server Bind Failed\n");
		exit(-1);
	}
	printf("Server Bind Success\r\n");
	/*监听*/
	if (listen(sockfd, LENGTH_OF_LISTEN_QUEUE) == -1) {
		perror("Server Listen Failed\n");
		exit(-1);
	}
	printf("Server Listen Success\r\n");
	return sockfd;
}
int eth_success(int fd){
	int i=0;
	struct sockaddr_in client_addr;
	socklen_t client_addr_length = sizeof(client_addr);
	i=accept(fd, (struct sockaddr*)&client_addr, &client_addr_length);
	if(i < 0)
	    {
	    	perror("Server Accept Failed\n");
	    	exit(-1);
	    }
	    printf("Server Accept Success\r\n");
	return i;
}
void src_to_tar(int src_fd,int tar1_fd,int tar2_fd)
{
	char buf[BUFFER_SIZE_MAVLINK] = { 0 };
	mavlink_user_rtcm_t cmd_mavlink;
	mavlink_rc_channels_override_t override_mavlink;
	mavlink_heartbeat_t heartbeat_mavlink;
	int nread = 0, index = 0;
	nread = read(src_fd, buf, BUFFER_SIZE_MAVLINK);

	for (index = 0; index < nread; index++) {
		//把一个完整的mavlink包放到msg中
		if (mavlink_parse_char(0, buf[index], msg, r_mavlink_status)) {
			printf("recieve id:%d\n", msg->msgid);

				if (msg->msgid == 13) {
					//			//从mavlink数据包解析数据段
					mavlink_msg_user_rtcm_decode(msg, &cmd_mavlink);
					if (write(tar1_fd, cmd_mavlink.rtcm_data, (msg->len - 6))
							< 0) {
						// perror(" send Data Failed\n");
						// exit(-1);
					}
					bzero(msg, BUFFER_SIZE_MAVLINK);

				} else{
					char *px = (char*) msg;
					write(tar2_fd, &px[2], msg->len + 6);
					write(tar2_fd, px, 2);
					bzero(msg, BUFFER_SIZE_MAVLINK);
				}
		}
	}
}
void transfer_msg_mavlink(int src_fd,int tar_fd){//传输一个完整的mavlink包
	char buf[BUFFER_SIZE_MAVLINK]={0};
	int nread=0,index=0;
	nread=read(src_fd,buf,BUFFER_SIZE_MAVLINK);
//	for( index=0;index<nread;index++)
//		 {
//			 //把一个完整的mavlink包放到msg中
//		 	  if(mavlink_parse_char(0,buf[index],msg,r_mavlink_status_1)){
//	//	 		 printf("recieve id:%d\n",msg->msgid);
//		 	  }
//		  }
	write(tar_fd,buf,nread);
	bzero(buf,BUFFER_SIZE_MAVLINK);
	bzero(msg_1,BUFFER_SIZE);
}
void transfer_msg(int src_fd,int tar_fd){
	char buf[BUFFER_SIZE_MAVLINK]={0};
	int nread=0;
	nread=read(src_fd,buf,BUFFER_SIZE_MAVLINK);
	if(nread>0){
	write(tar_fd,buf,nread);
	}
	bzero(buf,BUFFER_SIZE_MAVLINK);
}

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
int main(void)
{
//	 mavlink_status_t*  r_mavlink_status=malloc(BUFFER_SIZE_MAVLINK);
//	 mavlink_message_t* msg=malloc(BUFFER_SIZE_MAVLINK);
	r_mavlink_status = (mavlink_status_t*) malloc(BUFFER_SIZE_MAVLINK);
	if (r_mavlink_status == NULL) {
		perror("malloc error");
		exit(-1);
	}
	r_mavlink_status_1 = (mavlink_status_t*) malloc(BUFFER_SIZE_MAVLINK);
	if (r_mavlink_status_1 == NULL) {
		perror("malloc error");
		exit(-1);
	}
	msg = (mavlink_message_t*) malloc(BUFFER_SIZE_MAVLINK);
	if (msg == NULL) {
		perror("malloc error");
		exit(-1);
	}
	msg_1 = (mavlink_message_t*) malloc(BUFFER_SIZE_MAVLINK);
	if (msg_1 == NULL) {
		perror("malloc error");
		exit(-1);
	}
	int nread, i;
	int fd_nano;
	int fd_237_1, fd_237_2,fd_237_3;
	int fd_uart1, fd_uart4, fd_uart5;
	int sockfd, new_sockfd;
	struct sockaddr_in client_addr;
	char buff[BUFFER_SIZE] = { 0 };
	char buff1[BUFFER_SIZE] = { 0 };
	fd_nano = open( NANO_UART, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd_nano) {
		perror("Can't Open Serial Port");
		exit(-1);
	}
	printf("open nano success\n");
	if (isatty(STDIN_FILENO) == 0) {
		printf("standard input is not a terminal device\n");
	} else {
		printf("nano isatty success!\n");
	}
	fd_237_1 = open( UART0_237, O_RDWR | O_NOCTTY | O_NDELAY); //com3,rtc
	if (-1 == fd_237_1) {
		perror("Can't Open Serial Port");
		exit(-1);
	}
	printf("open 237_1 success\n");
	if (isatty(STDIN_FILENO) == 0) {
		printf("standard input is not a terminal device\n");
	} else {
		printf("237_1 isatty success!\n");
	}
	fd_237_2 = open( UART1_237, O_RDWR | O_NOCTTY | O_NDELAY); //com1,gps
	if (-1 == fd_237_2) {
		perror("Can't Open Serial Port");
		exit(-1);
	}
	printf("open 237_2 success\n");
	if (isatty(STDIN_FILENO) == 0) {
		printf("standard input is not a terminal device\n");
	} else {
		printf("237_2 isatty success!\n");
	}
	fd_237_3 = open( UART2_237, O_RDWR | O_NOCTTY | O_NDELAY); //com1,gps
	if (-1 == fd_237_3) {
		perror("Can't Open Serial Port");
		exit(-1);
	}
	printf("open 237_3 success\n");
	if (isatty(STDIN_FILENO) == 0) {
		printf("standard input is not a terminal device\n");
	} else {
		printf("237_ isatty success!\n");
	}
	fd_uart1 = open( UART1, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd_uart1) {
		perror("Can't Open Serial Port");
		exit(-1);
	}
	printf("open uart1 success\n");
	if (isatty(STDIN_FILENO) == 0) {
		printf("standard input is not a terminal device\n");
	} else {
		printf("uart1 isatty success!\n");
	}
	fd_uart4= open( UART4, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd_uart4) {
		perror("Can't Open Serial Port");
		exit(-1);
	}
	printf("open uart4 success\n");
	if (isatty(STDIN_FILENO) == 0) {
		printf("standard input is not a terminal device\n");
	} else {
		printf("uart4 isatty success!\n");
	}
	fd_uart5 = open( UART5, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd_uart5) {
		perror("Can't Open Serial Port");
		exit(-1);
	}
	printf("open uart5 success\n");
	if (isatty(STDIN_FILENO) == 0) {
		printf("standard input is not a terminal device\n");
	} else {
		printf("uart5 isatty success!\n");
	}
	if ((i = set_opt(fd_nano, 57600, 8, 'N', 1)) < 0) {
		perror("set_opt error");
		return 0;
	}
	if ((i = set_opt(fd_237_1, 57600, 8, 'N', 1)) < 0) {
		perror("set_opt error");
		return 0;
	}
	if ((i = set_opt(fd_237_2, 57600, 8, 'N', 1)) < 0) {
		perror("set_opt error");
		return 0;
	}
	if ((i = set_opt(fd_237_3, 9600, 8, 'N', 1)) < 0) {
			perror("set_opt error");
			return 0;
		}
	if ((i = set_opt(fd_uart1, 57600, 8, 'N', 1)) < 0) {
		perror("set_opt error");
		return 0;
	}
	if ((i = set_opt(fd_uart4, 57600, 8, 'N', 1)) < 0) {
		perror("set_opt error");
		return 0;
	}
	if ((i = set_opt(fd_uart5, 57600, 8, 'N', 1)) < 0) {
		perror("set_opt error");
		return 0;
	}
//    sockfd=set_eth("192.168.1.222",8888);//设置网络通信的ip和端口号
//    new_sockfd=eth_success(sockfd);//网络成功通信
	while (1) {
		src_to_tar(fd_nano, fd_237_3, fd_uart1); //电台数据--->板卡rtcm和飞控的控制命令
//		transfer_msg_mavlink(fd_uart1, fd_nano); //飞控的回复（mavlink数据）-->电台数据
		transfer_msg( fd_uart1,fd_nano);
		transfer_msg( fd_237_2,fd_uart4); //转发数据，板卡gps数据--->飞控串口
		transfer_msg( fd_uart4,fd_237_2);
	}
	free(r_mavlink_status);
	free(r_mavlink_status_1);
	r_mavlink_status = NULL;
	r_mavlink_status_1 = NULL;
	free(msg);
	free(msg_1);
	msg = NULL;
	msg_1 = NULL;
	close(sockfd);
	close(new_sockfd);
	close(fd_nano);
	close(fd_237_1);
	close(fd_237_2);
	close(fd_237_3);
	close(fd_uart1);
	close(fd_uart4);
	close(fd_uart5);
	return 0;
}


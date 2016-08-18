#ifndef __UART_ETH_H
#define __UART_ETH_H
#define BUFFER_SIZE 1024
#define BUFFER_SIZE_MAVLINK 512
#define LENGTH_OF_LISTEN_QUEUE 20
#define NANO_UART "/dev/ttyS0"//电台串口，内部直连
#define UART1 "/dev/ttyS1"//待定义   j20  9600
#define UART2 "/dev/ttyS2"//待定义   j22  9600
#define UART3 "/dev/ttyS3"//待定义   j24  9600
#define UART4 "/dev/ttyS4"//待定义   j26(2,3)  9600
#define UART5 "/dev/ttyS5"//待定义   j26(4,5)  9600
#define UART0_237 "/dev/ttyS6"//GPS串口，内部直连
#define UART1_237 "/dev/ttyS7"//GPS串口，内部直连
#define UART2_237 "/dev/ttyS8"//GPS串口，内部直连
#include "./common/mavlink.h"
int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop);
int set_eth(char *ip,int portnumber);
int eth_success(int fd);
int flag1;
void src_to_tar(int src_fd,int tar1_fd,int tar2_fd);
//void eth_to_uart(int fd,int new_sockfd);
void transfer_msg_mavlink(int src_fd,int tar_fd);
void transfer_msg(int src_fd,int tar_fd);
mavlink_status_t*  r_mavlink_status;
mavlink_status_t*  r_mavlink_status_1;
mavlink_message_t* msg;
mavlink_message_t* msg_1;
typedef struct _queue_mavlink
{
	int  head;
	int tail;
	char data[BUFFER_SIZE_MAVLINK];
}QUEUE;
void quene_push(QUEUE *p, char data);
char quene_pop(QUEUE *p);
int quene_size(QUEUE *p);
#endif

//头文件包含
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <pthread.h>
#include <stddef.h>


#define SERV_PORT 1234
#define SERV_IP "47.98.63.64"


char buff[100] = {0};
int sockfd;//服务器描述符



//客户端已连接和处理函数
void service_deal()
{
      char msg[100];
      char sz[10];
      int i,rc;
	 char buf[100] = {};
      char ss;
	  int stm32_fd = 10;
		int app_fd = 9;
      int maxfd = sockfd;

	  
      fd_set set;
   //多路复用集合的使用处理
	    FD_ZERO(&set);
      while(1)
      {
	 
	    FD_SET(sockfd,&set);
		FD_SET(stm32_fd,&set);
		FD_SET(app_fd,&set);
		
		
	
	    //多路复用函数
	    select(maxfd+1,&set,NULL,NULL,NULL);
	//	  continue;
		//监听文件描述符
	    if(FD_ISSET(sockfd,&set))
	    {
		  //和连接上来的客户端建立连接
		  struct sockaddr_in sc;
		  socklen_t len = sizeof(sc);
		  //等待
		  int s = accept(sockfd,(struct sockaddr *)&sc,&len);
		  if(s<0)
			continue;
		  
		  if(stm32_fd == 10){
				stm32_fd = s;
				printf("STM32连接上服务器！\n");	
			}else{
				app_fd = s;
				printf("APP连接上服务器！\n");	
			}
			maxfd = s;	

		  char ip[100];
		  inet_ntop(PF_INET,&sc.sin_addr,ip,sizeof(ip));
		  printf("%s连接成功!\n",ip);
	 
	    }


		if(FD_ISSET(app_fd,&set)){	//等待app的信号
			
		   bzero(buf,sizeof(buf));
		   //接收APP的数据
		  read(app_fd,buf,sizeof(buf));
	//	  printf("buf = %s\n",buf);
		  write(stm32_fd,buf,sizeof(buf));
		  		
		}
	#if 1	

		if(FD_ISSET(stm32_fd,&set)){	//接收STM 32反馈信息
			//清空client_buf
			bzero(buf,sizeof(buf));
			//接收板子的数据
			read(stm32_fd,buf,sizeof(buf));
	
			printf("buf1 = %s\n",buf);

			write(app_fd,buf,sizeof(buf));
			
		
		}
#endif



      }
	  
	  close(sockfd);
	  close(stm32_fd);
	  close(app_fd);
  
}


//信号的处理函数
void sig_fa(int sig)
{
      close(sockfd);
      printf("服务器关闭!\n");
      exit(0);
}

//主函数入口处理
int main()
{
      //ctrl+c关闭服务器
      signal(SIGINT,sig_fa);
      
	  sockfd = socket(PF_INET,SOCK_STREAM,0);
      if(sockfd==-1)
      {
	    perror("socket");
	    printf("网络编程失败!\n");
	    exit(-1);
      }

      //2.快速重启
      long val = 1;
      setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));//允许重用本地地址和端口

      struct sockaddr_in addr;
      bzero(&addr,sizeof(addr));
      addr.sin_family = PF_INET;
      addr.sin_port = htons(SERV_PORT);
      addr.sin_addr.s_addr =htonl(INADDR_ANY);

      if(bind(sockfd,(struct sockaddr *)&addr,sizeof(addr))==-1)
      {
	    perror("bind");
	    printf("Bind服务器IP失败!\n");
	    exit(-1);
      }

      //3.
      if(listen(sockfd,5)==-1)
      {
	    perror("listen");
	    printf("监听文件描述符失败!\n");
	    exit(-1);
      }




printf("/********************************欢迎大家进入 《芯知识学堂》 *******************************************\n");
printf("* **												    ** *\n");
printf("* **												    ** *\n");
printf("* **												    ** *\n");
printf("* **	 ﹎  ●  ○ .﹎                               	   ★  作者：四叶草               	    ** *\n");
printf("* **  **┈ ┈ /█\\/▓\\       ﹎ ┈**                          ★单片机&工控技术QQ群： 82558344 ★          ** *\n");
printf("* **   	    ☆°★ ∴°°∴ ☆°．·★°°Oooo          ★             ★物联网&嵌入式群QQ群: 544101253 ★   	    ** *\n");
printf("* **	 ▅▆▇█████▇▆▅▃▂┈﹎▂▃▅▆    __▁▂▃▁__ ●              ★微信群向群主提交密语:  获取密钥★	    ** *\n");
printf("* **												    ** *\n");
printf("* **												    ** *\n");
printf("* **					    密钥 :《好看的皮囊千篇一律，有料的大脑万里挑一  	    ** *\n");
printf(" * **					        	研发的梦想带你入门，无尽的学识助你成长》    ** *\n");
printf("* ** 												    ** *\n");
printf(" * **  本学堂走的是开源免费路线、里面所有的原创优质文章、书籍、视频就是免费获取！！！共同学习进步！ ** *\n");
printf("* **                                                                				    ** *\n");
printf("********************************************************************************************************\n");



      //启动服务
      service_deal();

      return 0;
}

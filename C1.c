// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include<stdlib.h>
#include <string.h> 
#include<sys/wait.h>
#define PORT 8089
#define PSIZE 10
int main(int argv,char *argc[])
{
	int sock;
	char buf[1024];
	struct sockaddr_in serv_addr;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(atoi(argc[1])); 
	if(inet_aton("127.0.0.2",&serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	int ptr;
	int kl=0;
	for(int i=0;i<=1;i++) //2
	{
		bzero(buf,1024);
		sprintf(buf,"11Advantage%d:Lucky",i);	
		kl++;
		int k=send(sock,buf,strlen(buf),0);
		bzero(buf,1024);
		printf("waiting for rec...%d\n",kl);
		recv(sock,buf,1024,0);
		printf("data : %s\n",buf);
	}
	for(int i=0;i<=1;i++) //4
	{
		bzero(buf,1024);
		sprintf(buf,"11Advantage%d:Lucky2",i);	
		kl++;
		int k=send(sock,buf,strlen(buf),0);
		bzero(buf,1024);
		printf("waiting for rec...%d\n",kl);
		recv(sock,buf,1024,0);
		printf("data : %s\n",buf);
	}
	for(int i=0;i<4;i++) //8
	{
		bzero(buf,1024);
		sprintf(buf,"12Advantage1:Lucky2:Hello%d",i);
		kl++;	
		int k=send(sock,buf,strlen(buf),0);
		bzero(buf,1024);
		printf("waiting for rec...%d\n",kl);
		recv(sock,buf,1024,0);
		printf("data : %s\n",buf);
	}
	for(int i=0;i<4;i++) //12
	{
		bzero(buf,1024);
		sprintf(buf,"12Advantage0:Lucky2:Hello%d",i);
		kl++;	
		int k=send(sock,buf,strlen(buf),0);
		bzero(buf,1024);
		printf("waiting for rec...%d\n",kl);
		recv(sock,buf,1024,0);
		printf("data : %s\n",buf);
	}
	printf("requesing subscribiot\n");

	// FILE *fp = fopen("heh.txt","r");
	// char chr = fgetc(fp);
	// int i=0;
	// char topic[1000];
	// bzero(topic,1000);
	// while(chr != -1)
	// {	
	// 	if(chr == '\n')
	// 	{
	// 		if(i==0)
	// 		{
	// 			i=0;
	// 			bzero(topic,1000);
	// 			chr = fgetc(fp);
	// 			continue;
	// 		}
	// 		else
	// 		{
	// 			i=0;
	// 			sprintf(buf,"12Advantage0:Lucky2:%s",topic);
	// 			printf("sending:%s\n",buf);
	// 			int k=send(sock,buf,strlen(buf),0);
	// 			bzero(buf,1024);
	// 			printf("waiting for rec...%d\n",kl);
	// 			recv(sock,buf,1024,0);
	// 			printf("data : %s\n",buf);
	// 			chr = fgetc(fp);
	// 			bzero(topic,1000);
	// 			continue;
	// 		}
			
	// 	}
	// 	else
	// 	{
	// 		topic[i++]=chr;
	// 		chr = fgetc(fp);
	// 	}
	// }


	// if(i!=0)
	// {
	// 	sprintf(buf,"12Advantage0:Lucky2:%s",topic);
	// 	int k=send(sock,buf,strlen(buf),0);
	// 	bzero(buf,1024);
	// 	printf("waiting for rec...%d\n",kl);
	// 	recv(sock,buf,1024,0);
	// 	printf("data : %s\n",buf);
	// }


	// for(int i=0;i<1;i++) //15
	// {
	// 	bzero(buf,1024);
	// 	sprintf(buf,"04Eyanshu:Advantage%d",i);
	// 	kl++;	
	// 	int k=send(sock,buf,strlen(buf),0);
	// 	bzero(buf,1024);
	// 	printf("waiting for rec...%d\n",kl);
	// 	recv(sock,buf,1024,0);
	// 	printf("data : %s\n",buf);
	// }

	printf("requesing subscribiot\n");
	// for(int i=0;i<=5;i++) //21
	// {
	// 	bzero(buf,1024);
	// 	sprintf(buf,"02Eyanshu2:Advantage0");
	// 	int k=send(sock,buf,strlen(buf),0);
	// 	kl++;
	// 	bzero(buf,1024);
	// 	printf("waiting for rec...%d\n",kl);
	// 	recv(sock,buf,1024,0);
	// 	printf("data : %s\n",buf);
	// 	bzero(buf,1024);
	// }
	close(sock);
}
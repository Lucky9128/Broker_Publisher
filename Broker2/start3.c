#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 
#include<sys/wait.h>
#include "ds.h"

struct Topic *TopicList;
struct Publisher *PublisherList;
struct Subscriber *SubscriberList;

/***************************************************
1/0<TopicName>:<Publisher Name>:<Msg>
*******************************************************/

void send_to_Next(char *mydata,char *IP,int port,char *NewResult)
{

	int sock;
	char buf[1024];
	// char NewResult[1000];
	struct sockaddr_in serv_addr;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(port); 
	if(inet_aton(IP,&serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
    char NewQuery[1000];
    bzero(NewQuery,1000);
    printf("new Query %s\n",NewQuery);
    sprintf(NewQuery,"2127.0.0.1:%s",mydata);
    int k=send(sock,NewQuery,strlen(NewQuery),0);
    bzero(NewResult,1000);
    recv(sock,NewResult,1000,0);
    printf("data : %s\n",NewResult);
    bzero(buf,1024);
    strcpy(buf,"Ack");
    printf("Return New result\n");
    k=send(sock,buf,strlen(buf)+1,0);
    close(sock);
    printf("Return New result %d\n",k);
    // return NewResult;
}

void Publisher(char *Query,int sock)
{
    printf("Publisher\n");
    char TopicName[1000];
    char PubName[1000];
    char Buffer[1000];
    if(Query[0]=='1')   //Publisher wants to create a topic
    {
        printf("Query1\n");
        bzero(TopicName,1000);
        bzero(PubName,1000);
        char *c = strchr(Query,':'); 
        printf("No:%s\n",c);
        *c = '\0';
        c++;
        strcpy(TopicName,&Query[1]);
        strcpy(PubName,c);
        // printf("Publisher Name:%s\t Topic :%s\n",PubName,TopicName);
        PublisherList = addPublisher(PublisherList,TopicName,PubName);
        TopicList = addTopic(TopicList,TopicName,PubName);
        // printf("new Topic is created:\nSending Ack\n");
        bzero(Buffer,1000);
        sprintf(Buffer,"Acked");
        if(send(sock,Buffer,strlen(Buffer),0)<0)
        {
            perror("Sending...");
        }
        printf("Send Ack\n");
    }
    else if(Query[0]=='2')   //Publisher wants to write msg to topic
    {
        char msg[1000];
        printf("Query2\n");
        char *c = strchr(Query,':');
        *c = '\0';
        bzero(TopicName,1000);
        bzero(PubName,1000);
        bzero(msg,1000);
        strcpy(TopicName,&Query[1]);
        c++;
        char *t = strchr(c,':');
        *t = '\0';
        t++;
        strcpy(PubName,c);
        strcpy(msg,t);
        addMsg(TopicList,msg,PubName,TopicName);
        bzero(Buffer,1000);
        sprintf(Buffer,"Acked");
        if(send(sock,Buffer,strlen(Buffer),0)<0)
        {
            perror("Sending...");
        }
        printf("Send Ack\n");
    }
}
void Subscriber(char *Query, int sock, int type_exe, char *result,char *IP)
{
    printf("Subscirber\n");
    char Sname[1000];
    char Buffer[1000];
    char TopicName[1000]; 
    bzero(TopicName,1000);

    bzero(Sname,1000);
    char *c = strchr(Query,':'); 
    *c = '\0';
    c++;
    strcpy(Sname,&Query[1]);
    strcpy(TopicName,c);
    c--;
    *c=':';
    if(Query[0]=='1')
    {
        // printf("Creating subscriber:\n");
       
        // printf("Adding %s to %s:\n",TopicName,Sname);
        int k=0;
        char err[100];
        if(TopicExists(TopicList,TopicName))
        {
            SubscriberList = addSbscr(SubscriberList,TopicList,TopicName,Sname,&k,&err[0]);
            // printf("%d \t :%s:\n",k,err);
            if(type_exe==1)
            {
                strcpy(result,"Subscribed on other broker");
                if(strncmp(IP,"127.0.0.1",9)==0)
                    return;
            }
            bzero(Buffer,1000);
            sprintf(Buffer,"exit");
            if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
            {
                perror("Sending...");
            }
            printf("Send Ack\n");
        }
        else
        {
            printf("Calling sub\n");
            bzero(Buffer,1000);
            send_to_Next(Query,"127.0.0.2",8089,Buffer);
            if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
            {
                perror("Sending...");
            }
            printf("Send Msg\n");
            bzero(Buffer,1000);
            sprintf(Buffer,"exit");
            if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
            {
                perror("Sending...");
            }
            printf("Send Ack\n");            
        }
        
    }
    else if(Query[0]=='2')
    {
        bzero(Buffer,1000);
        if(!isSubscbr(SubscriberList,Sname)||!isTopicSubscbr(SubscriberList,Sname,TopicName))
        {
            sprintf(Buffer,"You have not subscribe  %s\n",TopicName);
        }
        else
        {
            printf("%s Subscribes %s\n",Sname,TopicName);
            strcpy(Buffer,GetMsg(TopicList,SubscriberList,TopicName,Sname));
        }

        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        printf("Send Msg\n");
        recv(sock,Buffer,1024,0);
        bzero(Buffer,1000);
        sprintf(Buffer,"exit");
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        printf("Send Ack\n");
    }
    else if(Query[0]=='3')
    {
        printf("type3\n");
        if(!isSubscbr(SubscriberList,Sname)||!isTopicSubscbr(SubscriberList,Sname,TopicName))
        {
            sprintf(Buffer,"You have not subscribe  %s\n",TopicName);
            if(send(sock,Buffer,strlen(Buffer),0)<0)
            {
                perror("Sending...");
            }
            printf("Send Msg\n");
        }
        else
        {
            printf("loop\n");
            while(1)
            {
                bzero(Buffer,1000);
                strcpy(Buffer,GetMsg(TopicList,SubscriberList,TopicName,Sname));
                if(Buffer[0]==':'&&Buffer[1]==':')
                {
                    if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
                    {
                        perror("Sending...");
                    }
                    printf("Send Msg %s\n",Buffer);
                    if(recv(sock,Buffer,1024,0)<0)
                    {
                        perror("Sending...");
                    }
                    printf("Recv Msg %s\n",Buffer);
                    break;
                }
                else
                {
                    if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
                    {
                        perror("Sending...");
                    }
                    printf("Send Msg %s\n",Buffer);
                    bzero(Buffer,1000);
                    if(recv(sock,Buffer,1024,0)<0)
                    {
                        perror("Sending...");
                    }
                    printf("Rec Msg %s\n",Buffer);
                }
            } 
            bzero(Buffer,1000);
            sprintf(Buffer,"exit");
            if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
            {
                perror("Sending...");
            }
            printf("Send Msg %s\n",Buffer);
        }
        
    }
    else if(Query[0]=='4')
    {
        Listoftopic(TopicList,&Buffer[0]);
        if(type_exe==1)
        {
            strcpy(result,Buffer);
            if(strncmp(IP,"127.0.0.1",9)==0)
                return;
        }
        else
        {   
            char tempdata[1000];
            bzero(tempdata,1000);
            printf("2\n");
            send_to_Next(Query,"127.0.0.2",8089,tempdata);
            printf("Data::%s",tempdata);
            strcat(Buffer,tempdata);
            printf("final Rsult:%s\n",Buffer);
            if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
            {
                perror("Sending...");
            }
            printf("Send Msg\n");
            bzero(Buffer,1000);
            recv(sock,Buffer,1024,0);
            bzero(Buffer,1000);
            sprintf(Buffer,"exit");
            if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
            {
                perror("Sending...");
            }
            printf("Send Ack\n");
        }
        
    }
}
void CircularCall(char *Query,int sock)
{
    printf("Ciruclar %s\n",Query);
    char *c = strchr(Query,':');
    *c='\0';
    char IP[20];
    bzero(IP,20);
    strcpy(IP,Query);
    c++;
    char PreQuery[1000];
    bzero(PreQuery,1000);
    strcpy(PreQuery,c);
    printf("Ip is %s \t Query is %s\n",IP,PreQuery);
    char myResult[1000];
    bzero(myResult,1000);
    Subscriber(PreQuery,0,1,&myResult[0],IP);
    printf("final Result : %s\n",myResult);
    if(send(sock,myResult,strlen(myResult)+1,0)<0)
    {
        perror("Sending...");
    }
    printf("Send Msg\n");
    bzero(PreQuery,1000);
    recv(sock,myResult,1024,0);
    bzero(myResult,1000);
    printf("Send Ack %s\n",myResult);
}

int main(int argv, char* argc[]) 
{ 
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli; 
    char SendBuff[1024];
    char RecBuff[1024];

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	inet_aton(argc[2],&servaddr.sin_addr); 
	servaddr.sin_port = htons(atoi(argc[1])); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
    int res;

    while(1)
    {
	    int len = sizeof(cli); 
        // Accept the data packet from client and verification 
        connfd = accept(sockfd, (struct sockaddr*)&cli, &len); 
        if (connfd < 0) { 
            printf("server acccept failed...\n"); 
            exit(0); 
        } 
        else
            printf("server acccept the client...\n"); 

        for(int i=0;i<1;i++)
        {
            bzero(RecBuff,1024);
            int k = recv(connfd,RecBuff,1024,0);
            printf("Recv : %s\n",RecBuff);
            if(RecBuff[0]=='\0')
                continue;
            if(strncmp(RecBuff,"exit",4)==0)
                break;
            if(RecBuff[0]=='1')
                Publisher(&RecBuff[1],connfd);
            else if(RecBuff[0]=='0')
                Subscriber(&RecBuff[1],connfd,0,NULL,NULL);
            else if(RecBuff[0]=='2')
                CircularCall(&RecBuff[1],connfd);
        }
        close(connfd);
        printf("Printing data\n");
        printAllTopic(TopicList);
        printAllPublisher(PublisherList);
        printAllSubscriber(SubscriberList);
    }
	close(sockfd); 
} 
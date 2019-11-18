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
struct SockAddr *mySock;
struct SockAddr *Socks;

/***************************************************
1/0<TopicName>:<Publisher Name>:<Msg>
*******************************************************/
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
        bzero(Buffer,1000);
        sprintf(Buffer,"%s Topic is Added:",TopicName);
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        bzero(Buffer,1000);
        recv(sock,Buffer,1000,0);
        bzero(Buffer,1000);
        sprintf(Buffer,"exit");
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
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
        bzero(Buffer,1000);
        if(!TopicExists(TopicList,TopicName))
        {
            sprintf(Buffer,"No such topic exists");
        }
        else
        {
            addMsg(TopicList,msg,PubName,TopicName);
            sprintf(Buffer,"Message Added in %s",TopicName);
        }
        
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        bzero(Buffer,1000);
        recv(sock,Buffer,1000,0);
        bzero(Buffer,1000);
        sprintf(Buffer,"exit");
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        printf("Send Ack\n");
        return;
    }

}
void Subscriber(char *Query, int sock)
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

    if(Query[0]=='1')
    {
        // printf("Creating subscriber:\n");
       
        // printf("Adding %s to %s:\n",TopicName,Sname);
        int k=0;
        char err[100];
        bzero(Buffer,1000);
        if(!TopicExists(TopicList,TopicName))
        {
            sprintf(Buffer,"No such Topic Exists");
        }
        else
        {
            SubscriberList = addSbscr(SubscriberList,TopicList,TopicName,Sname,&k,&err[0]);
            sprintf(Buffer,"You have Subscribed %s",TopicName);
        }
        
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
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
    else if(Query[0]=='2')
    {
        bzero(Buffer,1000);
        if(!isSubscbr(SubscriberList,Sname)||!isTopicSubscbr(SubscriberList,Sname,TopicName))
        {
            sprintf(Buffer,"You have not subscribe  %s",TopicName);
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
        bzero(Buffer,1000);
        if(!isSubscbr(SubscriberList,Sname)||!isTopicSubscbr(SubscriberList,Sname,TopicName))
        {
            sprintf(Buffer,"You have not subscribe  %s",TopicName);
            if(send(sock,Buffer,strlen(Buffer),0)<0)
            {
                perror("Sending...");
            }
            recv(sock,Buffer,1024,0);
            bzero(Buffer,1000);
            bzero(Buffer,1000);
            sprintf(Buffer,"exit");        
            if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
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
                    bzero(Buffer,1000);
                    strcpy(Buffer,"exit");
                    if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
                    {
                        perror("Sending...");
                    }
                    printf("Send Msg %s\n",Buffer);
                    break;
                }
                else
                {
                    strcat(Buffer,"\n");
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
        }
        
    }
    
    else if(Query[0]=='4')
    {  
        bzero(Buffer,1000);
        Listoftopic(TopicList,&Buffer[0]);
        if(strlen(Buffer)==0)
        {
            sprintf(Buffer,"Empty:");
        }
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


int main(int argv, char* argc[]) 
{ 

    Socks=NULL;
    mySock = NULL;

    Socks = MakeAddress(Socks);
    mySock = GetMySock(Socks,argc[1]);

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
	inet_aton(mySock->IP,&servaddr.sin_addr); 
	servaddr.sin_port = htons(mySock->port); 

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

        // for(int i=0;i<15;i++)
        // {
            bzero(RecBuff,1024);
            int k = recv(connfd,RecBuff,1024,0);
            printf("Recv : %s\n",RecBuff);
            if(RecBuff[0]=='\0')
                continue;
            if(strncmp(RecBuff,"exit",4)==0)
                break;
            if(RecBuff[0]=='1')
                Publisher(&RecBuff[1],connfd);
            else
                Subscriber(&RecBuff[1],connfd);
        // }
        close(connfd);
        printf("Printing data\n");
        printAllTopic(TopicList);
        printAllPublisher(PublisherList);
        printAllSubscriber(SubscriberList);
    }
	close(sockfd); 
} 
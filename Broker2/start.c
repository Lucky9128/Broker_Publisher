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

/***************************************************
1/0<TopicName>:<Publisher Name>:<Msg>
*******************************************************/

int send_to_Next(char *mydata,char *NewResult,char *QueryIP)
{

	int sock;
	char buf[1000];
    char curResult[1000];
	// char NewResult[1000];
	struct sockaddr_in serv_addr;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(mySock->next->port); 
	if(inet_aton(mySock->next->IP,&serv_addr.sin_addr)<=0) 
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
    sprintf(NewQuery,"2%s:%s",QueryIP,mydata);
    // printf("new Query %s\n",NewQuery);
    int k=send(sock,NewQuery,strlen(NewQuery)+1,0);
    // printf("Sending data back:\n");
    bzero(buf,1000);
    bzero(curResult,1000);
    while(1)
    {
        bzero(buf,1000);
        recv(sock,buf,1000,0);
        if(strncmp(buf,"exit",4)==0)
        {
            break;
        }
        strcat(curResult,buf);
        bzero(buf,1000);
        strcpy(buf,"Ack");
        k=send(sock,buf,strlen(buf)+1,0);
    }
    strcpy(NewResult,curResult);
    // printf("NewResult:%s\nCurResult:%s\n",NewResult,curResult);
    close(sock);
}

void Publisher(char *Query,int sock)
{
    printf("Publisher\n");
    char TopicName[1000];
    char PubName[1000];
    char Buffer[1000];
    if(Query[0]=='1')   //Publisher wants to create a topic
    {
        // printf("Query1\n");
        bzero(TopicName,1000);
        bzero(PubName,1000);
        char *c = strchr(Query,':'); 
        // printf("No:%s\n",c);
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
        // printf("Send Ack\n");
    }
    else if(Query[0]=='2')   //Publisher wants to write msg to topic
    {
        char msg[1000];
        // printf("Query2\n");
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
        // printf("Send Ack\n");
    }
}
void Subscriber(char *Query, int sock, int type_exe,char *IP)
{
    printf("Subscirber\n");
    char Sname[1000];
    char Buffer[10000];
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
       char tempdata[1000];
        bzero(tempdata,1000);
        bzero(Buffer,1000);
        if(!TopicExists(TopicList,TopicName))
        {
            strcpy(Buffer,"::No such Topic Exists");
            if(type_exe==1)
            {
                if(strcmp(mySock->next->IP,IP)!=0)
                {
                    send_to_Next(Query,tempdata,IP);
                    if(tempdata[0]!=':'&&tempdata[1]!=':')
                    {
                        bzero(Buffer,1000);
                        strcpy(Buffer,tempdata);
                    }
                }
            }
            else
            {
                // printf("Initial %s\n",IP);
                send_to_Next(Query,tempdata,mySock->IP);
                if(tempdata[0]!=':'&&tempdata[1]!=':')
                {
                    bzero(Buffer,1000);
                    strcpy(Buffer,tempdata);
                }
            }
        }
        else
        {
            SubscriberList = addSbscr(SubscriberList,TopicList,TopicName,Sname);
            if(type_exe==1)
            {
                if(strcmp(mySock->next->IP,IP)!=0)
                {
                    send_to_Next(Query,tempdata,IP);
                }
            }
            else
            {
                send_to_Next(Query,tempdata,mySock->IP);
            }
            bzero(Buffer,1000);
            sprintf(Buffer,"You subscribed %s at %s",TopicName,mySock->IP);
        }
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        // printf("Send Msg\n");
        bzero(Buffer,1000);
        recv(sock,Buffer,1024,0);
        bzero(Buffer,1000);
        sprintf(Buffer,"exit");
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        // printf("Send Ack\n");  
        
    }
    else if(Query[0]=='2')
    {
        char tempdata[1000];
        bzero(tempdata,1000);
        bzero(Buffer,1000);
        printf("Topic name:%s\n",TopicName);
        printf("Sub name:%s\n",Sname);
        printf("Topic name:%d\n",TopicExists(TopicList,TopicName));
        printf("Sub name:%d\n",isTopicSubscbr(SubscriberList,Sname,TopicName));
        if(!TopicExists(TopicList,TopicName))
        {
            strcpy(Buffer,"::No such Topic Exists");
            // printf("If condition 1\n");
            if(type_exe==1)
            {
            // printf("If condition 11\n");
                if(strcmp(mySock->next->IP,IP)!=0)
                {
                    send_to_Next(Query,tempdata,IP);
                    if(tempdata[0]!=':'&&tempdata[1]!=':')
                    {
                        bzero(Buffer,1000);
                        strcpy(Buffer,tempdata);
                    }
                    else
                    {
                        bzero(Buffer,1000);
                        strcpy(Buffer,tempdata);                        
                    }
                }
            }
            else
            {
            // printf("If condition 12\n");
                // printf("Initial %s\n",IP);
                send_to_Next(Query,tempdata,mySock->IP);
                    if(tempdata[0]!=':'&&tempdata[1]!=':')
                    {
                        bzero(Buffer,1000);
                        strcpy(Buffer,tempdata);
                    }
                    else
                    {
                        bzero(Buffer,1000);
                        strcpy(Buffer,tempdata);                        
                    }
            }
        }
        else if(!isTopicSubscbr(SubscriberList,Sname,TopicName))
        {
            // printf("If condition 2\n");
            strcpy(Buffer,"::You Haven't subscribed");
            if(type_exe==1)
            {
            // printf("If condition 21\n");
                if(strcmp(mySock->next->IP,IP)!=0)
                {
                    send_to_Next(Query,tempdata,IP);
                    if(tempdata[0]!=':'&&tempdata[1]!=':')
                    {
                        bzero(Buffer,1000);
                        strcpy(Buffer,tempdata);
                    }
                    else
                    {
                        bzero(Buffer,1000);
                        strcpy(Buffer,tempdata);                        
                    }
                }
            }
            else
            {
            printf("If condition 22\n");
                printf("Initial %s\n",IP);
                send_to_Next(Query,tempdata,mySock->IP);
                    if(tempdata[0]!=':'&&tempdata[1]!=':')
                    {
                        bzero(Buffer,1000);
                        strcpy(Buffer,tempdata);
                    }
                    else
                    {
                        bzero(Buffer,1000);
                        strcpy(Buffer,tempdata);                        
                    }
            }
        }
        else
        {
            strcpy(Buffer,GetMsg(TopicList,SubscriberList,TopicName,Sname));
            // printf("Mydata:%s\n",Buffer);

            if(Buffer[0]==':'&&Buffer[1]==':')
            {
                if(type_exe==1)
                {
                    if(strcmp(mySock->next->IP,IP)!=0)
                    {
                        send_to_Next(Query,tempdata,IP);
                        if(tempdata[0]!=':'&&tempdata[1]!=':')
                        {
                            bzero(Buffer,1000);
                            strcpy(Buffer,tempdata); 
                        }
                    }
                    
                }
                else
                {
                    send_to_Next(Query,tempdata,mySock->IP);
                    if(tempdata[0]!=':'&&tempdata[1]!=':')
                    {
                            bzero(Buffer,1000);
                            strcpy(Buffer,tempdata);
                    }
                }
            }
        }
        // printf("Sending:%s\n",Buffer);
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        // printf("Send Msg\n");
        recv(sock,Buffer,1024,0);
        bzero(Buffer,1000);
        sprintf(Buffer,"exit");
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        // printf("Send Ack\n");
        

    }
    else if(Query[0]=='4')
    {
        char tempdata[1000];
        bzero(tempdata,1000);
        bzero(Buffer,1000);
        Listoftopic(TopicList,&Buffer[0]);
        if(type_exe==1)
        {
            // printf("Query with data Query Ip %s sending to %s\n",IP,mySock->next->IP);
            if(strcmp(mySock->next->IP,IP)!=0)
            {
                send_to_Next(Query,tempdata,IP);
                strcat(Buffer,tempdata);
            }
        }
        else
        {
            // printf("Initial %s\n",IP);
            send_to_Next(Query,tempdata,mySock->IP);
            strcat(Buffer,tempdata);
        }
        // printf("Sending..%s\n",Buffer);
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        // printf("Send Msg\n");
        bzero(Buffer,1000);
        recv(sock,Buffer,1024,0);
        bzero(Buffer,1000);
        sprintf(Buffer,"exit");
        if(send(sock,Buffer,strlen(Buffer)+1,0)<0)
        {
            perror("Sending...");
        }
        // printf("Send Ack\n");
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
    printf("Initial Ip is %s \t Query is %s\n",IP,PreQuery);
    char myResult[1000];
    bzero(myResult,1000);
    Subscriber(PreQuery,sock,1,IP);
}

int main(int argv, char* argc[]) 
{ 
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli; 
    char SendBuff[1024];
    char RecBuff[1024];

    struct SockAddr *sc = NULL;
    sc = MakeAddress(sc);
    mySock = GetMySock(sc,argc[1]);

    printAddr(sc);

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
		printf("Server listening..\n"); 
	    int len = sizeof(cli); 
        // Accept the data packet from client and verification 
        connfd = accept(sockfd, (struct sockaddr*)&cli, &len); 
        if (connfd < 0) { 
            printf("server acccept failed...\n"); 
            exit(0); 
        } 
        else
            printf("server acccept the client...\n"); 

        // for(int i=0;i<1;i++)
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
            else if(RecBuff[0]=='0')
                Subscriber(&RecBuff[1],connfd,0,NULL);
            else if(RecBuff[0]=='2')
                CircularCall(&RecBuff[1],connfd);
        // }
        close(connfd);
        printf("Printing data\n");
        printAllTopic(TopicList);
        printAllPublisher(PublisherList);
        printAllSubscriber(SubscriberList);
    }
	close(sockfd); 
} 
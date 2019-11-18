#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include<stdlib.h>
#include <string.h> 
#include<sys/wait.h>
int PORT;


int sendQuery(char *Query)
{
    int sock;
    char buf[1024];
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
    bzero(&serv_addr,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
    if(inet_aton("127.0.0.1",&serv_addr.sin_addr)<=0) 
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    int k=send(sock,Query,strlen(Query),0);

    while(1)
    {
        bzero(buf,1024);
        recv(sock,buf,1024,0);
        printf("data : %s\n",buf);
        if(strncmp(buf,"exit",4)==0)
        {
            break;
        }
        bzero(buf,1024);
        strcpy(buf,"Ack");
        k=send(sock,buf,strlen(buf)+1,0);
    }
    close(sock);
    return 1;
}

int main(int argv,char *argc[])
{
    PORT = atoi(argc[1]);
    char Pname[100];
    char Tname[100];
    char Msg[1000];
    char filename[1000];
    int choose;
    int sock;
    char buf[1024];
    struct sockaddr_in serv_addr;
    bzero(Pname,100);
    strcpy(Pname,argc[2]);
    char *c=NULL;
    while(1)
    {
        choose = 0;
        printf("1:Subscirbe\n2:Get Next Msg\n3Get All New Messages\n4Get List of Topics\n5:Exit\nEnter your choice:");
        scanf("%d",&choose);
        switch(choose)
        {
            case 1:
                printf("Enter Topic Name:");
                bzero(Tname,100);
                scanf("%s",Tname);	
                bzero(buf,1024);
                sprintf(buf,"01%s:%s",Pname,Tname);
                sendQuery(buf);
                break;
            case 2:
                printf("Enter Topic Name:\n");
                bzero(Tname,100);
                read(0,Tname,100);
                printf("1 : %s",Tname);
                c = strchr(Tname,'\n');
                *c='\0';
                bzero(buf,1000);
                sprintf(buf,"02%s:%s",Pname,Tname);	
                sendQuery(buf);
                break;
            case 3:
                printf("Enter Topic Name:\n");
                bzero(Tname,100);
                read(0,Tname,100);
                printf("1 : %s",Tname);
                char *c = strchr(Tname,'\n');
                *c='\0';
                sprintf(buf,"03%s:%s",Pname,Tname);	
                sendQuery(buf);
                break;            
            case 5:
                exit(0);
        }
    }
}
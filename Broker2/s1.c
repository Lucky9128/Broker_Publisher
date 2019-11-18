#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include<stdlib.h>
#include <string.h> 
#include<sys/wait.h>
int PORT;
char IP[20];

int sendQuery(char *Query,char *DataRecv)
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
    // printf("Query Send:%s\n",Query);
    while(1)
    {
        bzero(buf,1024);
        recv(sock,buf,1024,0);
        // printf("REcV:%s\n",buf);
        if(strncmp(buf,"exit",4)==0)
        {
            break;
        }
        strcat(DataRecv,buf);
        bzero(buf,1024);
        strcpy(buf,"Ack");
        k=send(sock,buf,strlen(buf)+1,0);
    }
    // printf("Rec:%s\n",DataRecv);
    close(sock);
    return 1;
}

int main(int argv,char *argc[])
{
    if(argv<4)
    {
        printf("Argument format  ./program Port_of_Broker IP_of_Broker name_of_subscriber\n");
        exit(0);
    }

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
    strcpy(IP,argc[2]);
    strcpy(Pname,argc[3]);
    char *c=NULL;
    char DataRec[10000];
    while(1)
    {
        bzero(DataRec,10000);
        choose = 0;
        printf("1:Subscirbe\n2:Get Next Msg\n3:Get All New Messages\n4:Get List of Topics\n5:Exit\nEnter your choice:");
        scanf("%d",&choose);
        switch(choose)
        {
            case 1:
                printf("Enter Topic Name:");
                bzero(Tname,100);
                scanf("%s",Tname);	
                bzero(buf,1024);
                sprintf(buf,"01%s:%s",Pname,Tname);
                sendQuery(buf,DataRec);
                printf("%s\n\n",DataRec);
                break;
            case 2:
                printf("Enter Topic Name:\n");
                bzero(Tname,100);
                read(0,Tname,100);
                c = strchr(Tname,'\n');
                *c='\0';
                bzero(buf,1000);
                sprintf(buf,"02%s:%s",Pname,Tname);	
                sendQuery(buf,DataRec);
                printf("%s\n\n",DataRec);
                break;
            case 3:
                printf("Enter Topic Name:\n");
                bzero(Tname,100);
                read(0,Tname,100);
                printf("1 : %s",Tname);
                char *c = strchr(Tname,'\n');
                *c='\0';
                sprintf(buf,"02%s:%s",Pname,Tname);	
                for(int i=0;i<10;i++)
                {
                    bzero(DataRec,10000);
                    sendQuery(buf,DataRec);
                    // printf("%s\n\n",DataRec);
                    if(DataRec[0]==':'&&DataRec[1]==':')
                    {
                        break;
                    }
                    printf("%s\n\n",DataRec);
                }
                break;     
            case 4:
                sprintf(buf,"04%s:%s",Pname,Pname);	
                sendQuery(buf,DataRec);
                printf("%s\n\n",DataRec);
                break;

            case 5:
                exit(0);
                break;
            default:
            printf("valid input\n");
                continue;
                break;
        }
    }
}
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include<stdlib.h>
#include <string.h> 
#include<sys/wait.h>
int PORT;
char IP[20];

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
    int k=send(sock,Query,strlen(Query),0);
    bzero(buf,1024);
    recv(sock,buf,1024,0);
    printf("data : %s\n",buf);
    close(sock);
        return 1;
}

int main(int argv,char *argc[])
{
    bzero(IP,20);
    PORT = atoi(argc[1]);
    strcpy(IP,argc[2]);
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
        printf("1:Create\n2:Msg\n3File\n4:Exit\nEnter your choice:");
        scanf("%d",&choose);
        switch(choose)
        {
            case 1:
                printf("Enter Topic Name:");
                bzero(Tname,100);
                scanf("%s",Tname);	
                bzero(buf,1024);
                sprintf(buf,"11%s:%s",Tname,Pname);
                sendQuery(buf);
                break;
            case 2:
                printf("Enter Topic Name:\n");
                bzero(Tname,100);
                read(0,Tname,100);
                printf("1 : %s",Tname);
                c = strchr(Tname,'\n');
                *c='\0';
                printf("Enter Message:\n");
                bzero(Msg,1000);
                read(0,Msg,1000);
                c = strchr(Msg,'\n');
                *c='\0';
                bzero(buf,1000);
                sprintf(buf,"12%s:%s:%s",Tname,Pname,Msg);	
                sendQuery(buf);
                break;
            case 3:
                printf("Enter Topic Name:\n");
                bzero(Tname,100);
                read(0,Tname,100);
                printf("1 : %s",Tname);
                char *c = strchr(Tname,'\n');
                *c='\0';
                printf("Enter filename:\n");
                bzero(filename,1000);
                read(0,filename,1000);
                c = strchr(filename,'\n');
                *c='\0';
                FILE *fp = fopen(filename,"r");
                char chr = fgetc(fp);
                int filecount=0;
                char lines[1000];
                bzero(lines,1000);
                while(chr != -1)
                    {	
                        if(chr == '\n')
                        {
                            if(filecount==0)
                            {
                                filecount=0;
                                bzero(lines,1000);
                                chr = fgetc(fp);
                                continue;
                            }
                            else
                            {
                                filecount=0;
                                bzero(buf,1000);
                                sprintf(buf,"12%s:%s:%s",Tname,Pname,lines);
                                printf("sending:%s\n",buf);
                                sendQuery(buf);
                                chr = fgetc(fp);
                                bzero(lines,1000);
                                continue;
                            }
                            
                        }
                        else
                        {
                            lines[filecount++]=chr;
                            chr = fgetc(fp);
                        }
                    }
                    if(filecount!=0)
                    {
                        bzero(buf,1024);
                        sprintf(buf,"12Advantage0:Lucky2:%s",lines);
                        sendQuery(buf);
                    }
                break;            
            case 4:
                exit(0);
        }
    }
}
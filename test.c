#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>

int main()
{
        system("chmod -c 666 ./t");
        system("./t");
}
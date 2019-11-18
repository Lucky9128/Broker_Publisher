#include<stdio.h>
#include<sys/signal.h>
#include<stdlib.h>
#include<unistd.h>
void sigHan()
{
    printf("Hello\n");
}


int main()
{
    
    signal(SIGALRM, sigHan);
    alarm(10);
    while(1)
    {
        printf("");
    }

}
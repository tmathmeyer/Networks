#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 


int get_socket(int port_number, char* ip);

int main(int argc, char *argv[])
{
    if(argc <= 2)
    {
        perror("ERROR, NO IP PROVIDED");
        return 1;
    } 
    int n = 0;
    char recvBuff[1024];
    memset(recvBuff, '0',sizeof(recvBuff));

    int sockfd = get_socket(8090, "127.0.0.1");


    write(sockfd, "GET /index.html HTTP/1.1", 24);


    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    } 

    if(n < 0)
    {
        printf("\n Read error \n");
    } 

    return 0;
}


// messy nonsense to connect open a socket
int get_socket(int port_number, char* ip)
{
    int sockfd = 0, n = 0;
    struct sockaddr_in serv_addr; 

    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number); 

    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    }


    return sockfd;
}
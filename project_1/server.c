//  ///////////////////////////////////////////////////////////////////////
//  This is a simple program to test out the pthread package on Linux.
//  It can be built on a Linux machine with the command line 
//
//   gcc  -lpthread  -g -o threads server.c
//
//  ///////////////////////////////////////////////////////////////////////
//

// Ted Meyer
// 03/11/2013

#include <stdio.h>
#include <pthread.h>
#include <sys/resource.h>
#include <asm/errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include "server.h"
#include <sys/stat.h>


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        perror("ERROR, no port provided\n");
        return 9;
    }

    struct sockaddr_in serv_addr, cli_addr;
    int sockfd,
        portno, 
        clilen,
        broken_sockets = 0;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket\n");
        return 9;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        return 9;
    }
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    if (argc < 3)
    {
        while(broken_sockets < 10)
        {
            int newsockfd;

            while(newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))
            {
                if (newsockfd < 0)
                {
                    broken_sockets++;
                } else
                {
                    make_thread((void *)(*accept_socket), newsockfd);
                }
            }
        }
    } else {
        close(sockfd);
    }
}


void* accept_socket(void *data)
{
    int buffer_size = 2048;
    int n;
    char buffer[buffer_size];
    memset(buffer,0,buffer_size);
    int newsockfd = (int *)data;

    n = read(newsockfd,buffer,buffer_size-1);
    printf("\n\n\n%s\n\n\n\n",buffer);
    
    char method[5];
    char filename[100];
    char* req_path[2];
    req_path[0] = method;
    req_path[1] = filename;

    parse_header(buffer, req_path);

    if (starts_with(method, "GET"))
    {
        write_file(filename, newsockfd);
    }
    
    close(newsockfd);
    fprintf(stderr, "socket closed after writing bytes\n");
}

//req_path is a tuple of strings
//  [0] = 5 chars long, http method (GET / POST / OTHER)
//  [1] = 100 chars long, holds path to get (opitting the opening '/')
void parse_header(char* header, char** req_path)
{
    int req_path_pos = 0;
    int header_pos   = 0;

    // if the header is broken, well, thats not my problem now is it :D
    while(header_pos < 3) {
        req_path[0][req_path_pos] = header[header_pos];
        header_pos++;
        req_path_pos++;
    }
    header_pos+=2;
    req_path_pos = 0;

    while((header[header_pos] != ' ') && req_path_pos < 99)
    {
        req_path[1][req_path_pos] = header[header_pos];
        header_pos   ++;
        req_path_pos ++;
    }
    req_path[1][req_path_pos] = '\0';
}

void write_file(char* filename, int newsockfd)
{
    char* temp_file = "index.html";
    if(filename[0] == 0)
    {
        puts("index");
        filename = temp_file;
    }
    if (!file_exists(filename))
    {
        write(newsockfd, "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nDate: Wed, 06 Nov 2013 06:00:26 GMT\nConnection: keep-alive\nTransfer-Encoding: chunked", 133);
        fprintf(stderr, "there was a 404 from: \"%s\"\n", filename);
        return;
    }

    //collect file information for header
    char type[20]; // give some space to allow really long file names
    char snum[20]; // allocate space to hold the decimal representation of the files size
    int type_length = get_file_type(filename, type); // set the name and get the length
    int file_size = get_file_size(filename); // get the size of the file (in bytes)
    int result_length = write_int_to_string(snum, file_size); // set the filesize and get it's length

    // write the header
    write(newsockfd, "HTTP/1.0 200 OK\nDate: Sun, 99 Dec 1492 23:59:59 GMT\nContent-Type: ", 66);
    write(newsockfd, type, type_length);
    write(newsockfd, "\nContent-Length: ", 17);
    write(newsockfd,snum,result_length);


    // read the file write to buffer
    int size  = 0,
        total = 0;
    char f_buffer[256];
    FILE* requested_file;
    requested_file = fopen(filename, "r");
    while((size = read_file_256(requested_file, f_buffer)) != 0)
    {
        total += size;
        write(newsockfd, f_buffer, size);
        //gotta sleep to prevent the socket from getting smashed upside the head
        usleep(10);
    }
    fclose(requested_file);
}

int get_file_type(char* filename, char* type)
{
    int file_pos = 0;
    int alloc_pos = 0;
    int allocing = 0;
    char allocation[100]; // this will hold the type

    while(filename[file_pos] != '\0')
    {
        file_pos ++;
        if (filename[file_pos] == '.')
        {
            allocing = 1;
            alloc_pos = 0;
        }
        else if (filename[file_pos] == '/')
        {
            alloc_pos = 0;
        }
        else if (allocing)
        {
            allocation[alloc_pos] = filename[file_pos];
            alloc_pos ++;
        }
    }

    allocation[alloc_pos] = '\0';

    if (starts_with(allocation, "jpg"))
    {
        write_type(type, "image/jpg");
        return 10;
    } else if (starts_with(allocation, "html"))
    {
        write_type(type, "text/html");
        return 10;
    } else if (starts_with(allocation, "mp3"))
    {
        write_type(type, "audio/mpeg");
        return 10;
    } else 
    {
        write_type(type, "text/plain");
        return 10;
    }
}

void write_type(char* final, char* temp)
{
    int t_pos = 0;
    while(temp[t_pos] != '\0' && t_pos < 19)
    {
        final[t_pos] = temp[t_pos];
        t_pos++;
    }
    final[t_pos] = '\0';
}

// a.starts_with(b)
int starts_with (char* a, char* b)
{
    int b_pos = 0;
    while(b[b_pos] != '\0')
    {
        if (a[b_pos] == '\0' || a[b_pos] != b[b_pos])
        {
            return 0;
        }
        b_pos ++;
    }
    return 1;
}

int write_int_to_string(char* string, int val)
{
    val = reverse_int_digits(val);
    char* vals = "0123456789";
    int string_position = 0;
    while(val > 0)
    {
        int i = (val/10)*10;
        i = val-i;
        string[string_position] = vals[i];
        val /= 10;
        string_position++;
    }
    string[string_position] = string[string_position+1] = '\n';
    return string_position+2;
}

int reverse_int_digits(int i)
{
    int result = 0;
    while(i > 0)
    {
        result *= 10;
        int j = i%10;
        i /= 10;
        result += j;
    }
    return result;
}

int get_file_size(char* filename)
{
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}


pthread_t make_thread(void *init_proc, void *data)
{
    pthread_t t;
    pthread_create(&t,NULL,init_proc,data);
    return t;
}


int read_file_256(FILE* file, char* buffer)
{
    char temp;
    int buff_pos = 0;
    while((! feof(file)) && (buff_pos < 256))
    {
        temp = fgetc(file);
        buffer[buff_pos] = temp;
        buff_pos += 1;
    }
    return buff_pos;
}

int file_exists(char *filename)
{
    struct stat   buffer;   
    return (stat (filename, &buffer) == 0);
}

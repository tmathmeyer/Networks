// Ted Meyer
// 03/11/2013

#ifndef SERVER_H
#define SERVER_H

pthread_t    make_thread( void *, void *);


void*        accept_socket(void *data);


int          read_file_256(FILE* file, char* buffer);
int          get_file_size(char* filename);
int          write_int_to_string(char* string, int val);
int          reverse_int_digits(int i);
int          starts_with (char* a, char* b);
int          write_int_to_string(char* string, int val);
int          reverse_int_digits(int i);
int          get_file_type(char* filename, char* type);
int          file_exists(char *filename);


void         parse_header(char* header, char** req_path);
void         write_file(char* filename, int newsockfd);
void         write_type(char* final, char* temp);



#endif
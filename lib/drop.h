#ifndef DROP_H
#define DROP_H

#define PORT 8081
#define ADDR "localhost"
#define TIME_FORMAT "%F %T"
#define h_addr h_addr_list[0] 
#define _GNU_SOURCE
#define MAX_BACKUPS 5
#define OLD_FILE ".old"
#define BUFF_SIZE 1024

#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <time.h>
#include <unistd.h>

enum msg_type{NEW_FILE, DELETE_FILE, NEW_DIR, PULL_REQUEST, DISCONNECT, E};

struct file_info
{
	char file_name[256];
	off_t file_size;
	mode_t permissions;
};

struct message
{
	enum msg_type type;
	struct file_info file;
	time_t time;	
};


int send_file_to_socket(int, const char *, const char *, const struct stat *);
int send_dir_to_socket(int , const char *, const char *, const struct stat *);
int receive_file_from_socket(int, const char *, struct file_info);
int receive_dir_from_socket(int, const char *, struct file_info);
int deal_with_old_file(const char*);
int check_updates(const char*, const struct stat *, int);
void match_val(char *, char *);
void before_exit();
void exit_signal_handler(int);
int error(char *);
void configure_data();

#endif
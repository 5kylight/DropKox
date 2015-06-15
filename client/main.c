#define _GNU_SOURCE
#include <ctype.h>
#include <pthread.h>

#include "../lib/drop.h"

void configure_data(int, char **);
void save_data();
void send_pull_request();
void *receiver_thread(void *);
void update_sync_time();

time_t last_update_time = 0; // default 1970.. 
time_t last_server_synchronization = 0;
char start_path[256] = "./";
int is_working;
char confing_file_path[100] = "./config";

int server_socket;
int delay_time = 10;
char address[50] = ADDR;
int port = PORT;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


int
main(int argc, char *argv[])
{	
	/* Read configuration from file */
	configure_data(argc, argv);

	//  AteXIT fun!
	atexit(before_exit);


	// Maybe two proceses
	signal(SIGINT, exit_signal_handler);
	
 
	// Opening socket etc.

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket < 0)
		error("socket");

	struct sockaddr_in sa_server;
	sa_server.sin_family = AF_INET;
	sa_server.sin_port = htons(port);
	struct hostent *server;
	server = gethostbyname(address);

	bcopy(server -> h_addr, &sa_server.sin_addr.s_addr, server -> h_length);

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(server_socket, 
				(struct sockaddr *) &sa_server, 
				sizeof(sa_server)) < 0)
		error("connect");


	/* Create thread which recives updates files */
	pthread_t receiver;
	pthread_create(&receiver, NULL, &receiver_thread, NULL);


	is_working = 1;
	while(is_working) {	
		send_pull_request();
		ftw(start_path, check_updates, 16);
		time(&last_update_time);
		
		sleep(delay_time); // now a few seconds but it  will be about five minutes!		
	}
}

void send_pull_request()
{
	struct message message;
	message.type = PULL_REQUEST;
	
	pthread_mutex_lock(&lock);
	message.last_update_time = last_server_synchronization;
	pthread_mutex_unlock(&lock);

	if (send(server_socket, 
					&message, 
					sizeof(message), 
					0) < 0)
				error("sendmsg");
}

void configure_data(int argc, char *argv[])
{	
	printf("\n== Starting configuration ==\n\n");
	FILE *config_file = fopen(confing_file_path, "r");
	if (config_file == NULL)
		error("fopen config file");

	char *line;
	size_t len = 0;
	ssize_t read;
	char what[256];
	char value[256];

	while ((read = getline(&line, &len, config_file)) > 0){
		char *eq = strstr(line, "=");
		if (eq == NULL)
			continue;

		*eq = 0;
		*(line + read -1) = 0;
		strcpy(what, line);
		strcpy(value, eq + 1);
		match_val(what, value);
	}

	fclose(config_file);
	FILE *client_data = fopen("client.data", "r+");
	if (client_data != NULL) {
		while ((read = getline(&line, &len, client_data)) > 0) {
			char *eq = strstr(line, "=");
			if (eq == NULL)
				continue;

			*eq = 0;
			*(line + read - 1) = 0;
			strcpy(what, line);
			strcpy(value, eq + 1);
			match_val(what, value);
		}
		
		fclose(client_data);
	}

	printf("\n== Configuration finished ==\n\n");
}

void update_sync_time()
{
	pthread_mutex_unlock(&lock);
	time(&last_server_synchronization);
	pthread_mutex_unlock(&lock);	
}


void match_val(char *what, char* val)
{	
	printf("Setting %s with %s\n", what, val);
	if (!strcmp(what, "start path")) {
		strcpy(start_path, val);
		if (create_backup_dir(val))
			error("Bad start path");
	}
	else if (!strcmp(what, "delay time"))
		delay_time = atoi(val);
	else if (!strcmp(what, "address"))
		strcpy(address, val);
	else if (!strcmp(what, "port"))
		port = atoi(val);
	else if (!strcmp(what, "last update time")) {
		struct tm tm;
		strptime(val, TIME_FORMAT, &tm);
		last_update_time = mktime(&tm); 
	} else if (!strcmp(what, "last server sync")) {
		struct tm tm;
		strptime(val, TIME_FORMAT, &tm);
		last_server_synchronization = mktime(&tm); 
	}
}


int check_updates(const char *path, const struct stat *info, int type)
{	
	if(strcmp(path, ".") == 0 
		|| strcmp(path, "..") == 0 
		|| strcmp(path, start_path) == 0)
		return 0;

	if(strstr(path, OLD_FILE) != NULL)
		return 0;

	/* Check and send file if it was modified after last update */
	if(type == FTW_F && difftime(info -> st_mtime, last_update_time) > FILE_DIFF) {		
		if(send_file_to_socket(server_socket, path, start_path, info) < 0) {
			error("Error sending file!");
		}
	} else if(type == FTW_D && difftime(info -> st_mtime, last_update_time) > FILE_DIFF) {
		if(send_dir_to_socket(server_socket, path, start_path, info) < 0) {
			error("Error sending file!");
		}
	}

	return 0;	
}

void *receiver_thread(void *unused)
{
	struct pollfd ufd;
	struct message message;
	int rv;
	while(is_working) {
		ufd.fd = server_socket;
		ufd.events = POLLIN;

		if ((rv = poll(&ufd, 1, 3000))== -1) {
		    perror("poll"); 
		} else {
			if (recv(server_socket, &message, sizeof(message), 0) < 0)
				error("recvform");

			/* Normalize name */
			char *file_name = message.file.file_name;
			char *normalized_name = calloc(strlen(file_name) + strlen(start_path),
				 sizeof(char));
			strcpy(normalized_name, start_path);
			strcat(normalized_name, file_name);

			if (message.type == NEW_FILE) {
				receive_file_from_socket(server_socket, normalized_name, message.file);
				update_sync_time();
			} else if(message.type == NEW_DIR) {
				receive_dir_from_socket(server_socket, normalized_name, message.file);
				update_sync_time();
			} else if(message.type == DISCONNECT) {
				is_working = 0;
				exit(EXIT_SUCCESS);
			} else if(message.type == CONF) {
				update_sync_time();
			}
		}
	}
	return NULL;
}

void disconnect()
{
	struct message message;
	message.type = DISCONNECT;
	if (send(server_socket, 
					&message, 
					sizeof(message), 
					0) < 0)
				error("sendmsg");		
}

void save_data()
{	
	FILE *client_data = fopen("client.data", "w");
	if (client_data != NULL) {
		char buf[256];
		struct tm *ptm = localtime(&last_update_time);
		strftime(buf, sizeof buf, TIME_FORMAT, ptm);
		fprintf(client_data, "last update time=%s\n", buf);
		
		ptm = localtime(&last_server_synchronization);
		strftime(buf, sizeof buf, TIME_FORMAT, ptm);
		fprintf(client_data, "last server sync=%s\n", buf);
		fclose(client_data);
	}
}


void before_exit()
{
	save_data();
	disconnect();
	// close(server_socket);
}

void exit_signal_handler(int signum)
{
	printf("\n\nExiting\n");
	exit(EXIT_SUCCESS);
}
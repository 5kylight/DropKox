#define _GNU_SOURCE
#include <ctype.h>
#include <ftw.h>
#include "../lib/drop.h"

void configure_data(int, char **);
void save_data();

time_t last_update_time = 0; // default 1970.. 
char start_path[256] = "./";
int is_working;
char confing_file_path[100] = "./config";

int server_socket;
int delay_time = 10;
char address[50] = ADDR;
int port = PORT;	

int
main(int argc, char *argv[])
{	
	/* Read configuration from file */
	configure_data(argc, argv);
	/* Read from file last_update_time */


	/* Mayby will be exported to library*/
	// TODO: if this is not first start read last_update_time from file!

	/*Prepare to exit */
	
	// TODO: AteXIT fun!
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



	// int i = 0;
	is_working = 1;
	while(is_working) {	
		// printf("\n\n Iteration: %d\n", i++);
		ftw(start_path, check_updates, 16);
		
		/* Update time*/
		time(&last_update_time);
		
		sleep(delay_time); // now a few seconds but it  will be about five minutes!		
	}

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

void match_val(char *what, char* val)
{	
	printf("Setting %s with %s\n", what, val);
	if (!strcmp(what, "start path"))
		strcpy(start_path, val);
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
	}
}


int check_updates(const char *path, const struct stat *info, int type)
{	
	if(strcmp(path, ".") == 0 || strcmp(path, "..") == 0 || strcmp(path, start_path) == 0)
			return 0;

	/* Check and send file if it was modified after last update */
	if(type == FTW_F && difftime(info -> st_mtime, last_update_time) >= 0) {		
		if(send_file_to_socket(server_socket, path, start_path, info) < 0) {
			error("Error sending file!");
		}
	} else if(type == FTW_D && difftime(info -> st_mtime, last_update_time) > 0) {
		if(send_dir_to_socket(server_socket, path, start_path, info) < 0) {
			error("Error sending file!");
		}
	}

	return 0;	
}


void exit_signal_handler(int signum)
{
	printf("\n\nExiting\n");
	exit(EXIT_SUCCESS);
}

void save_data()
{	
	FILE *client_data = fopen("client.data", "w");
	if (client_data != NULL) {
		struct tm *ptm = localtime(&last_update_time);
		char buf[256];
		strftime(buf, sizeof buf, TIME_FORMAT, ptm);
		fprintf(client_data, "last update time=%s\n", buf);
		fclose(client_data);
	}
}


void before_exit()
{
	save_data();
	// close(server_socket);
}


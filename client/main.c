#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

int check_updates(const char*, const struct stat *);
int send_file_to_socket(const char*);
void exit_signal_handler(int);
int error(char *);

int check_updates;

time_t last_update_time;
char *start_path = ".";
int is_working;

time_t 

int
main(int argc, char *argv[])
{	
	/* Read from file last_update_time*/
	last_update_time = 0;  // it should be from .data file
	/*Prepare to exit */
	
	// TODO: AteXIT fun!


	// Maybe two proceses
	signal(SIGINT, exit_signal_handler);
	

	// Opening socket etc.



	is_working = 1;
	while(is_working)
	{

		ftw(path, visit_ftw, );
		last_update_time = 
		sleep(5); // now a few seconds but it  will be about five minutes!		
	}

}


int check_updates(const char *path, const stuct stat *info, int type)
{	
	/* Check and send file if it was modified after last update */
	if(type == FTW_F && difftime(info.st_mtime, last_update_time) >= 0))
	{		
		if(send_file_to_socket(path) < 0)
		{
			error("Error sending file!");
		}
	}			
	return 1;	
}

/** Sends file to socket opened in main! */
int send_file_to_socket(const char* path)
{
	printf("Sending file: \t %s\n", path);
	return 1;
}



void exit_signal_handler(int signum)
{
	printf("Exiting\n");
	exit(EXIT_SUCCESS):
}



int error(char *error_info)
{
	perror(error_info);
	return -1;
}
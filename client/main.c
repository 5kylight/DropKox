#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include <ftw.h>

int check_updates(const char*, const struct stat *, int);
int send_file_to_socket(const char*);
int send_dir_to_socket(const char*);
void exit_signal_handler(int);
int error(char *);


time_t last_update_time; // default 1970.. 
char *start_path = "./";
int is_working;



int
main(int argc, char *argv[])
{	
	/* Read from file last_update_time */
	/*xif this was a first start! */


	/* Mayby will be exported to library*/
	// TODO: if this is not first start read last_update_time from file!

	/*Prepare to exit */
	
	// TODO: AteXIT fun!


	// Maybe two proceses
	signal(SIGINT, exit_signal_handler);
	

	// Opening socket etc.


	int i =0;
	is_working = 1;
	while(is_working)
	{	
		printf("\n\n Iteration: %d\n", i++);
		ftw(start_path, check_updates, 16);
		
		/* Update time*/
		time(&last_update_time);
		
		sleep(10); // now a few seconds but it  will be about five minutes!		
	}

}


int check_updates(const char *path, const struct stat *info, int type)
{	
	if(strcmp(path, ".") == 0 || strcmp(path, "..")== 0)
			return 0;

	/* Check and send file if it was modified after last update */
	if(type == FTW_F && difftime(info->st_mtime, last_update_time) >= 0)
	{		
		if(send_file_to_socket(path) < 0)
		{
			error("Error sending file!");
		}
	} else if(type == FTW_D)
		if(send_dir_to_socket(path) < 0)
		{
			error("Error sending file!");
		}
	
	return 0;	
}

/** Sends file to socket opened in main!
	Returns 1 if ok else -1 */
int send_file_to_socket(const char* path)
{
	printf("Sending file: \t %s\n", path);
	return 1;
}

int send_dir_to_socket(const char* path)
{
	printf("Sending dir: \t %s\n", path);
	return 1;
}


void exit_signal_handler(int signum)
{
	printf("\n\nExiting\n");
	exit(EXIT_SUCCESS);
}



int error(char *error_info)
{
	perror(error_info);
	return -1;
}
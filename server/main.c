#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int check_updates(const char*, const struct stat *);
int send_file_to_socket(const char*);
void exit_signal_handler(int);
int error(char *);


time_t last_update_time;
char *main_path = ".";
int is_working;

int
main(int argc, char *argv[])
{	
	/*Prepare to exit */
	
	// TODO: AteXIT fun!


	// Maybe two proceses
	signal(SIGINT, exit_signal_handler);
	is_working = 1;
	while(is_working)

}

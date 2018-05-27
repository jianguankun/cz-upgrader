#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include "daemon.h"

using namespace std;

void init_daemon()
{
	pid_t pid = fork();

	if(pid < 0) //1. fork , father process exit
	{
		perror("error fork");
		exit(1);
	}
	else if(pid == 0)
	{
		;
	}
	else 
	{
		//cout << "daemon create,pid = " << pid << endl;
		exit(0);
	}

	// setsid();

	// chdir("/");
	// umask(0);

	// int i = 0;
	// for(i = 0; i < getdtablesize(); i++)
	// {
	// 	close(i);
	// }
}

void sigterm_handler(int arg)
{
	arg = arg;
	//_running = 0;
} 

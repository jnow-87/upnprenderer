/* 
 *  Copyright (C) 2006 by:
 *  Da He <da.he@uni-dortmund.de>
 *  Julian Flake <julian.flake@uni-dortmund.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cRenderer.h"
#include "log.h"

bool quit;
cRenderer* renderer;


#define HELP(){ \
	cout << "usage: " << endl; \
	cout << "\trenderer [options]" << endl; \
	cout << endl; \
	cout << "options:" << endl; \
	cout << "\t-h              : \thelp" << endl; \
	cout << "\t-a <ip_addr>    : \tset the ip address" << endl; \
	cout << "\t-f              : \tenable fullscreen" << endl; \
	cout << "\t-d              : \texecute as daemon" << endl; \
	cout << "\t-l <log_file>    : \tuse <log_file> for logging" << endl; \
	cout << "\t-ll 0x<loglvl>  : \tuse <loglvl> for logging" << endl; \
	cout << "\t-p <pidfile>    : \tuse <pidfile> to export pid" << endl;\
	cout << "\t-v              : \tshow the version" << endl; \
	exit(0); \
}

#define DEFAULT_LOG "/var/run/renderer.log"
#define DEFAULT_PID "/var/run/renderer.pid"

void shutdown() 
{
	delete(renderer);
}

void onSignal(int in)
{
	quit = true;
}

int main(int argc, char *argv[])
{
	// Test, whether directory "$HOME/.renderer" exists
	char* home=getenv("HOME");
	char *log_file = 0, *pid_file = 0;
	char pfad_to_renderer[100];
	FILE* file;


	sprintf(pfad_to_renderer,"%s/.renderer",home);
	DIR* dir=opendir(pfad_to_renderer);
	if(dir==NULL)
	{
		mkdir(pfad_to_renderer,0100755);
		LOG(STATUS, cout << "Generating directory: " << pfad_to_renderer << endl); 
	}else
	{
		closedir(dir);
		LOG(STATUS, cout << "Directory(" << pfad_to_renderer << ") exists." << endl);
	}
	
	bool isdaemon=false;
	char* optionName;
	if(argc>1)
	{
		for(int i=1;i<argc;i++)
		{
			optionName=argv[i];
			if(!strcmp(optionName,"-a"))
			{
				i++;
			}else if(!strcmp(optionName,"-f"))
			{
			}else if(!strcmp(optionName,"-d"))
			{
				isdaemon=true;
			}else if(!strcmp(optionName,"-v"))
			{
				LOG(STATUS, cout << "Media Renderer 0.1.1" << endl);
				exit(0);
			}
			else if(strcmp(optionName, "-ll") == 0){
				sscanf(argv[i + 1], "%x", &log_level);
				i++;
			}
			else if(strcmp(optionName, "-l") == 0){
				log_file = argv[i + 1];
				i++;
			}
			else if(strcmp(optionName, "-p") == 0){
				pid_file = argv[i + 1];
				i++;
			}
			else
				HELP()
		}
	}

	if(log_file  == 0)
		log_file = DEFAULT_LOG;

	if(pid_file == 0)
		pid_file = DEFAULT_PID;

	printf("[%s] log_file \"%s\" loglvl 0x%x pidfile \"%s\"\n", argv[0], log_file, log_level, pid_file);

	if(isdaemon)
	{
		signal(SIGCHLD,SIG_IGN);
		pid_t daemonpid;
		if ( (daemonpid = fork()) < 0)
		{
			// FATAL: cannot fork child 
			LOG(STATUS, cout << "cannot fork child" << endl);
		}else if(daemonpid==0)
		{
			int daemonfd;
			file=fopen(log_file,"w");
			fclose(file);
			daemonfd=open(log_file,O_WRONLY,O_CREAT);
			if(dup2(daemonfd, 1) == -1  &&  close(daemonfd) == -1)
			{
				perror("dup2");
				exit(1);
			}
		}else
		{
			/* in case of parent process exit */
			exit(0);
		}
	}
	
	/* in case of child process or normal execution */
	file = fopen(pid_file, "w");
	fprintf(file, "%d", getpid());
	fclose(file);

	signal(SIGTERM, onSignal);
	signal(SIGINT, onSignal);
	signal(SIGKILL, onSignal);
	quit = false;
	renderer = new cRenderer(argc,argv);

	
	//	daemon(0,0);
	while (!quit) {
		usleep(100000);
	}
	shutdown();
	LOG(STATUS, cout << "normal beendet." << endl);
	return 0;
}

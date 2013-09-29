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

#define DEFAULT_LOG "log"

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
	char* logfile = 0;
	char pfad_to_renderer[100];
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
			}else if(!strcmp(optionName,"-h"))
			{
				// print usage
				LOG(STATUS, cout << "usage: " << endl);
				LOG(STATUS, cout << "\trenderer [options]" << endl);
				LOG(STATUS, cout << endl);
				LOG(STATUS, cout << "options:" << endl);
				LOG(STATUS, cout << "\t-h              : \thelp" << endl);
				LOG(STATUS, cout << "\t-a <ip_addr>    : \tset the ip address" << endl);
				LOG(STATUS, cout << "\t-f              : \tenable fullscreen" << endl);
				LOG(STATUS, cout << "\t-d              : \texecute as daemon" << endl);
				LOG(STATUS, cout << "\t-l <logfile>    : \tuse <logfile> for logging" << endl);
				LOG(STATUS, cout << "\t-ll 0x<loglvl>  : \tuse <loglvl> for logging" << endl);
				LOG(STATUS, cout << "\t-v              : \tshow the version" << endl);
				exit(0);
			}
			else if(strcmp(optionName, "-ll") == 0){
				sscanf(argv[i + 1], "%x", &loglevel);
				i++;
			}
			else if(strcmp(optionName, "-l") == 0){
				logfile = argv[i + 1];
				i++;
			}
			else
			{
				// print usage
				LOG(STATUS, cout << "usage: " << endl);
				LOG(STATUS, cout << "\trenderer [options]" << endl);
				LOG(STATUS, cout << "options:" << endl);
				LOG(STATUS, cout << "\t-h              : \thelp" << endl);
				LOG(STATUS, cout << "\t-a <ip_addr>    : \tset the ip address" << endl);
				LOG(STATUS, cout << "\t-f              : \tenable fullscreen" << endl);
				LOG(STATUS, cout << "\t-d              : \texecute as daemon" << endl);
				LOG(STATUS, cout << "\t-l 0x<logfile>  : \tuse <logfile> for logging" << endl);
				LOG(STATUS, cout << "\t-ll <loglvl>    : \tuse <loglvl> for logging" << endl);
				LOG(STATUS, cout << "\t-v              : \tshow the version" << endl);
				exit(0);
			}
		}
	}

	if(logfile  == 0){
		logfile = new char[strlen(DEFAULT_LOG) + strlen(home) + strlen(".renderer/") + 3];
		sprintf(logfile, "%s/.renderer/%s", home, DEFAULT_LOG);
	}

	printf("use \"%s\" for logging with loglevel 0x%x\n", logfile, loglevel);

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
			FILE* file=fopen(logfile,"w");
			fclose(file);
			daemonfd=open(logfile,O_WRONLY,O_CREAT);
			if(dup2(daemonfd, 1) == -1  &&  close(daemonfd) == -1)
			{
				perror("dup2");
				exit(1);
			}
			signal(SIGTERM, onSignal);
			signal(SIGINT, onSignal);
			signal(SIGKILL, onSignal);
			quit = false;
			renderer = new cRenderer(argc,argv);

			while (!quit) {
				usleep(100000);
			}
			shutdown();
			LOG(STATUS, cout << "normal beendet." << endl);
			return 0;
		}else
		{
			exit(0);
		}
	}else
	{
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
}

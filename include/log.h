#ifndef _LOG_H_
#define _LOG_H_


enum LogLevel{
	STATUS = 1,
	RENDERER = 2,
	AVTRANSPORT = 4,
	CONNECTION = 8,
	CONTROL = 16,
	SERVICE = 32
};


extern unsigned int loglevel;


#define LOG(lvl, cmd) { \
	if(lvl & loglevel){ \
		cmd; \
	} \
}


#endif

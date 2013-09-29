#ifndef _LOG_H_
#define _LOG_H_


enum log_level{
	STATUS = 1,
	RENDERER = 2,
	AVTRANSPORT = 4,
	CONNECTION = 8,
	CONTROL = 16,
	SERVICE = 32
};


extern unsigned int log_level;


#define LOG(lvl, cmd) { \
	if(lvl & log_level){ \
		cmd; \
	} \
}


#endif

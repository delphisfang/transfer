#ifndef _MY_DEFINE_H_
#define _MY_DEFINE_H_

#define LogDebug(fmt, ...)	do{			\
		DEBUG_P(LOG_DEBUG, "[DEBUG] [%s:%s:%d] " fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__);	\
	}while(0)

#define LogError(fmt, ...)	do{			\
		DEBUG_P(LOG_ERROR, "[ERROR] [%s:%s:%d] " fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__);	\
	}while(0)

#define LogTrace(fmt, ...)	do{			\
		DEBUG_P(LOG_TRACE, "[TRACE] [%s:%s:%d] " fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__);	\
	}while(0)

#define LogFatal(fmt, ...)	do{			\
		DEBUG_P(LOG_FATAL, "[FATAL] [%s:%s:%d] " fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__);	\
	}while(0)

#define LogErrPrint(fmt, ...)   do{         \
        DEBUG_P(LOG_ERROR, "[ERROR] [%s:%s:%d]" fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__);    \
        printf("[ERROR] [%s:%s:%d]" fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
    }while(0)

#define MAJOR_VERSION  "1"
#define MIDDLE_VERSION "0"
#define MINOR_VERSION  "0"

#define ON_ERROR_PARSE_PACKET() do{\
		LogError("Error parse http data");\
		on_error_parse_packet("Error unknown packet");\
	}while(0)

#endif


#ifndef _MY_DEBUG_H_
#define _MY_DEBUG_H_

#define TF_OK (0)
#define TF_ERROR (-1)

#define LogDebug(fmt, ...)	do{			\
		DEBUG_P(LOG_DEBUG, "[DEBUG] [%s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);	\
	}while(0)

#define LogWarn(fmt, ...)	do{			\
		DEBUG_P(LOG_ERROR, "\033[1m\033[33;40m[ERROR] [%s:%d] " fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__);	\
	}while(0)

#define LogError(fmt, ...)	do{			\
		DEBUG_P(LOG_ERROR, "\033[1m\033[31;40m[ERROR] [%s:%d] " fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__);	\
	}while(0)

#define LogTrace(fmt, ...)	do{			\
		DEBUG_P(LOG_TRACE, "\033[1m\033[32;40m[TRACE] [%s:%d] " fmt "\033[0m\n", __func__, __LINE__, ##__VA_ARGS__);	\
	}while(0)

#define LogFatal(fmt, ...)	do{			\
		DEBUG_P(LOG_FATAL, "[FATAL] [%s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);	\
	}while(0)

#define LogErrPrint(fmt, ...)   do{         \
        LogError("[ERROR] [%s:%d]" fmt "\n", __func__, __LINE__, ##__VA_ARGS__);    \
        printf("[ERROR] [%s:%d]" fmt "\n", __func__, __LINE__, ##__VA_ARGS__); \
    }while(0)

#define MAJOR_VERSION  "1"
#define MIDDLE_VERSION "0"
#define MINOR_VERSION  "0"

#define DO_FAIL(expr) do{\
			if (TF_OK != expr)\
			{\
				LogError("[ERROR] Failed to call %s", #expr);\
				return TF_ERROR;\
			}\
		}while(0)

#define ON_ERROR_PARSE_PACKET() do{\
		LogError("Error parse http data");\
		on_error_parse_packet("Error unknown packet");\
	}while(0)


#endif


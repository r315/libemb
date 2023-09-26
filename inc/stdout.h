#ifndef _stdout_h_
#define _stdout_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stdout_s {
    int (*available)(void);
    char (*readchar)(void);
    int (*read)(const char* str, int len);
    void (*writechar)(char c);
    int (*write)(const char* str, int len);
}stdout_t;
	

#ifdef __cplusplus
}
#endif

#endif /* _stdout_h_ */

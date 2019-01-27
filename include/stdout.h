#ifndef _stdout_h_
#define _stdout_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _StdOut {


		void(*init)(void);
		/**
		 * std function
		 * */
		char(*getchar)(void);
		void(*putchar)(char c);
		void(*puts)(const char* str);

		/**
		 * Checks if any char was received and return it on parameter c,
		 * this function is non blocking
		 *
		 * \param  c  - Pointer to put the received char
		 * \returns   - 1 if a char was received and placed on parameter c,
		 *              0 no char received and param c is anchanged
		 * */
		uint8_t(*getCharNonBlocking)(char *c);

		/**
		 * Test if a char is available for reading
		 * */
		uint8_t(*kbhit)(void);

	}StdOut;

#ifdef __cplusplus
}
#endif

#endif /* _stdout_h_ */
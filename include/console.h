#ifndef _console_h_
#define _console_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stdout.h"
#include "console_command.h"

#define NO        0
#define YES       1

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

#define CONSOLE_MAX_COMMANDS 64
#define COMMAND_MAX_LEN		 64


	class Console {

		ConsoleCommand *cmdList[CONSOLE_MAX_COMMANDS];
		uint8_t cmdListSize;
		uint8_t executing;
		char line[COMMAND_MAX_LEN];
		uint8_t lineLen;
		const char *prompt;
		StdOut *out;

	public:
		Console(void);
		Console(StdOut *sp, const char *prt);

		void init(StdOut *sp, const char *prt);

		char getLine(char *line, uint8_t max);
		char getLineNonBlocking(char *line, uint8_t *curLen, uint8_t max);
		void process(void);

		void addCommand(ConsoleCommand *cmd);
		char parseCommand(char *line, uint8_t len);
		char executeCommand(void *ptr);
		void putc(char c);
		void puts(const char* str);
		char getchar(void);
		void gets(char* str);
		char getline(char *line, uint8_t max);
		void print(const char* str, ...);
		uint8_t kbhit(void);

		uint8_t getCmdListSize(void) { return cmdListSize; }
		ConsoleCommand *getCmdIndexed(uint8_t idx) { return cmdList[idx]; } // security issues??
	};

#ifdef __cplusplus
}
#endif

#endif
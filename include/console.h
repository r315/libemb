#ifndef _console_h_
#define _console_h_


#ifdef __cplusplus
extern "C" {

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
#define HISTORY_SIZE		 5


	class Console {

		ConsoleCommand *cmdList[CONSOLE_MAX_COMMANDS];
		uint8_t cmdListSize;
		uint8_t executing;
		char line[COMMAND_MAX_LEN];
		uint8_t line_len;
		const char *prompt;
		StdOut *out;
		
		char history[HISTORY_SIZE][COMMAND_MAX_LEN];
		void historyDump(void);
		void historyAdd(char *entry);
		char *historyBack(void);
		char *historyForward(void);
		char *historyGet(void);
		void historyClear(void);
		uint8_t changeLine(char *old_line, char *new_line, uint8_t old_line_len);
		uint8_t hist_idx;
		uint8_t hist_cur;
		uint8_t hist_size;

	public:
		Console(void);
		Console(StdOut *sp, const char *prt);

		void init(StdOut *sp, const char *prt);

		char getLine(char *line, uint8_t max);
		char getLineNonBlocking(char *line, uint8_t *cur_len, uint8_t max);
		void process(void);

		void addCommand(ConsoleCommand *cmd);
		char parseCommand(char *line);
		char executeCommand(void *ptr);
		int xputchar(int c);
		int xputs(const char* str);
		int xgetchar(void);
		char *xgets(char* str);
		char getline(char *line, uint8_t max);
		void print(const char* str, ...);
		uint8_t kbhit(void);

		uint8_t getCmdListSize(void) { return cmdListSize; }
		ConsoleCommand *getCmdIndexed(uint8_t idx) { return cmdList[idx]; } // security issues??
	};


}
#endif

#endif
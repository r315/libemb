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

#define CONSOLE_MAX_COMMANDS 	64
#ifndef COMMAND_MAX_LEN
#define COMMAND_MAX_LEN		 	64
#endif
#define HISTORY_SIZE		 	5

#ifndef CONSOLE_PRINT_MAX_LEN
#define CONSOLE_PRINT_MAX_LEN	64
#endif


	class Console {

		ConsoleCommand *cmdList[CONSOLE_MAX_COMMANDS];
		uint8_t cmdListSize;
		uint8_t processing;
		char line[COMMAND_MAX_LEN];
		uint8_t line_len;
		const char *prt;
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
		void cls(void);
		void setOutput(StdOut *sp);

		void addCommand(ConsoleCommand *cmd);
		void registerCommandList(ConsoleCommand **list);
		char parseCommand(char *line);
		char executeCommand(void *ptr);
		int putChar(int c);
		int putString(const char* str);
		int getChar(void);
		uint8_t getCharNonBlocking(char *c);
		char *getString(char* str);
		void print(const char* str, ...);
		uint8_t kbhit(void);

		uint8_t getCmdListSize(void) { return cmdListSize; }
		ConsoleCommand *getCmdIndexed(uint8_t idx) { return cmdList[idx]; } // security issues??
	};
}
#endif

#endif

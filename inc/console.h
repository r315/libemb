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
#ifndef CONSOLE_COMMAND_MAX_LEN
#define CONSOLE_COMMAND_MAX_LEN	64
#endif

#define CONSOLE_HISTORY_SIZE 	5
#define CONSOLE_MAX_PARAMS		10

#ifndef CONSOLE_PRINT_MAX_LEN
#define CONSOLE_PRINT_MAX_LEN	64
#endif


	class Console {

		ConsoleCommand *m_cmdList[CONSOLE_MAX_COMMANDS];
		uint8_t m_cmdListSize;
		uint8_t m_active;
		char m_line[CONSOLE_COMMAND_MAX_LEN];
		char m_buf[CONSOLE_PRINT_MAX_LEN];
		uint8_t m_line_len;
		const char *m_prompt;
		char *m_argv[CONSOLE_MAX_PARAMS];
    	int m_argc;
		
		char m_history[CONSOLE_HISTORY_SIZE][CONSOLE_COMMAND_MAX_LEN];
		uint8_t m_hist_idx;
		uint8_t m_hist_cur;
		uint8_t m_hist_size;
		stdout_t *m_out;

		void historyDump(void);
		void historyAdd(char *entry);
		char *historyBack(void);
		char *historyForward(void);
		char *historyGet(void);
		void historyClear(void);
		uint8_t changeLine(char *old_line, char *new_line, uint8_t old_line_len);

	public:
		Console(void);
		Console(stdout_t *sp, const char *prt);

		void init(stdout_t *sp, const char *prt);

		char getLine(char *line, uint8_t max);
		char getLineNonBlocking(char *line, uint8_t *cur_len, uint8_t max);
		void process(void);
		void cls(void);
		void setOutput(stdout_t *sp);

		void addCommand(ConsoleCommand *cmd);
		void registerCommandList(ConsoleCommand **list);
		char parseCommand(char *line);
		char executeCommand(void *ptr);

		int getChar(void);
        char getch(void);
		char *getString(char* str);
		int print(const char* str);
        int println(const char* str);
		int printf(const char* str, ...);
        int printchar(int c);
		uint8_t available(void);
        uint8_t getchNonBlocking(char *);

		uint8_t getCmdListSize(void) { return m_cmdListSize; }
		ConsoleCommand *getCmdIndexed(uint8_t idx) { return m_cmdList[idx]; } // security issues??
	};
}
#endif

#endif

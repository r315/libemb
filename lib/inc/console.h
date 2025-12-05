#ifndef _console_h_
#define _console_h_


#ifdef __cplusplus
extern "C" {

#include <stdint.h>
#include "stdinout.h"
#include "console_command.h"


#define NO      0
#define YES     1

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif


#ifndef CONSOLE_WIDTH
#define CONSOLE_WIDTH 	                32      // Width in chars
#endif

#ifndef CONSOLE_MAX_COMMANDS
#define CONSOLE_MAX_COMMANDS 	        16
#endif

#ifndef CONSOLE_COMMAND_PARAMS
#define CONSOLE_COMMAND_PARAMS		    10
#endif

#ifndef HISTORY_MAX_SIZE
#define HISTORY_MAX_SIZE 	            5
#endif

typedef enum con_res{
    CON_IDLE = 0,
    CON_LINE
}con_res_t;

class History {
    public:
        History(void);
		void push(const char *);
		char *pop(void);
		char *back(void);
		char *forward(void);
		void clear(void);
    private:
        char m_history[HISTORY_MAX_SIZE][CONSOLE_WIDTH];
		uint8_t m_top;
		uint8_t m_idx;
		uint8_t m_size;
};

class Console {
	public:
		Console(void);
		Console(const stdinout_t *, const char *);
		void init(const stdinout_t *, const char *);
		char getLine(char *, uint8_t);
		void process(void);
		void cls(void);
		void setOutput(const stdinout_t *);

		void addCommand(ConsoleCommand *);
        void addCommandArray(ConsoleCommand *, int count);
		void registerCommandList(ConsoleCommand **);
		char parseCommand(char *);
		char executeCommand(void *);

		int getChar(void);
        char getch(void);
		char *getString(char*);
		int print(const char* );
        int println(const char*);
		int printf(const char*, ...);
        int printchar(int c);
        void asciiprint(const uint8_t *mem, uint32_t len);
        void hexprint(const uint8_t *addr, uint32_t len, uint8_t ascii);
        void hexdump(const uint8_t *addr, uint32_t len, uint8_t ncols, uint8_t ascii);
		uint8_t available(void);
        uint8_t getchNonBlocking(char *c);

		uint8_t getCmdListSize(void) { return m_cmdListSize; }
		ConsoleCommand *getCmdIndexed(uint8_t idx) { return m_cmdList[idx]; } // security issues??

    private:
        ConsoleCommand *m_cmdList[CONSOLE_MAX_COMMANDS];
        char readchar(void);
        void writechar(char c);
		char m_line[CONSOLE_WIDTH];
		char m_buf[CONSOLE_WIDTH];
		char *m_argv[CONSOLE_COMMAND_PARAMS];
    	int m_argc;
		const char *m_prompt;
        const stdinout_t *m_out;
		uint8_t m_cmdListSize;
		uint8_t m_active;
		uint8_t m_line_len;
        uint8_t m_line_edit;
        History m_hist;
		void replaceLine(char *);
		con_res_t scanForLine(void);
};


}
#endif

#endif

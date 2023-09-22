
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "console.h"

Console::Console(void) {

}

Console::Console(StdOut *sp, const char *prompt) {
	init(sp, prompt);
}

void Console::init(StdOut *sp, const char *prompt) {
	memset(m_cmdList, '#', CONSOLE_MAX_COMMANDS * sizeof(ConsoleCommand*));
	memset(m_line, '\0', CONSOLE_COMMAND_MAX_LEN);
	m_cmdListSize = 0;
	m_active = NO;
	m_out = sp;
	m_prompt = prompt;
	historyClear();	
	m_line_len = 0;
	//addCommand(&help); // since all cpp support is disabled, classes on .data section are not initialized
}

void Console::addCommand(ConsoleCommand *cmd) {
	if (cmd == NULL || m_cmdListSize == CONSOLE_MAX_COMMANDS)
	{
		print("Invalid command or command list full!\n");
		return;
	}

	cmd->init(this);
	m_cmdList[m_cmdListSize++] = cmd;	
}

void Console::registerCommandList(ConsoleCommand **list){
	while(*list != NULL){
		addCommand(*(list++));
	}
}

char Console::parseCommand(char *line) {
	char res = CMD_NOT_FOUND, *cmdname;
	ConsoleCommand **cmd = m_cmdList;

	// check if empty line
	if (*line == '\n' || *line == '\r' || *line == '\0')
		return CMD_OK;

	// Convert command line into string array
	m_argc = strToArray((char*)line, m_argv);

	if(m_argc < CONSOLE_MAX_PARAMS && m_argv[0] != NULL){
		cmdname = m_argv[0];
		for (uint8_t i = 0; i < m_cmdListSize; i++, cmd++) {
			if (*cmd == NULL) {
				break;
			}
			if ((*cmd)->isNameEqual(cmdname) != 0) {
				res = (*cmd)->execute(m_argc, m_argv);
				break;
			}	
		}
	}
	
	// Clear command line
	memset(line, '\0', CONSOLE_COMMAND_MAX_LEN);

	if (res == CMD_NOT_FOUND) {
		print("Command not found\n");
	}else if (res == CMD_BAD_PARAM) {
		print("Bad parameter\n");
	}else if(res == CMD_OK_LF){
		printchar('\n');
	}

	return res;
}

void Console::process(void) {
	if(m_active == NO){
		m_active = YES;
		print(m_prompt);
		return;
	}
#if defined(CONSOLE_BLOCKING)
	m_line_len = 0;
	m_line_len = getLine(m_line, CONSOLE_COMMAND_MAX_LEN);	
#else
	if (getLineNonBlocking(m_line, &m_line_len, CONSOLE_COMMAND_MAX_LEN)) 
#endif
	{
		historyAdd(m_line);
		if(parseCommand(m_line) != CMD_OK_NO_PRT){
			print(m_prompt);
		}
	}
}

void Console::cls(void){
	print("\e[2J\r");	
}

/**
 * */
void Console::setOutput(StdOut *sp){
	m_out = sp;	
}

/**
 * 
 * */
int Console::print(const char* str)
{
    int len = 0;

    while(*str){
	    m_out->xputchar(*str++);
        len++;
    }
	
    return len;
}

int Console::println(const char* str)
{
    return m_out->xputs(str);
}

char *Console::getString(char* str)
{
	uint8_t i = 0;
	char c;

	c = m_out->xgetchar();

	while ((c != '\n') && (c != '\r'))
	{
		*(str + i++) = c;		
		c = m_out->xgetchar();
	}
	*(str + i) = '\0';

	return str;
}

int Console::printchar(int c) {
	m_out->xputchar(c);
	return (int)c;
}

int Console::getChar(void)
{
	char c = m_out->xgetchar();
	m_out->xputchar(c);
	return (int)c;
}

uint8_t Console::getCharNonBlocking(char *c)
{
	return m_out->getCharNonBlocking(c);
}

/**
 * Read a line ended by \n or \r from serial port.
 * Ending char is not added to line read *
 * */
char Console::getLineNonBlocking(char *dst, uint8_t *cur_len, uint8_t maxLen) {
	char c;
	uint8_t len;

	while (m_out->getCharNonBlocking(&c)) {
		len = *cur_len;
		
		if ((c == '\n') || (c == '\r')) {			
			//Remove all extra text from previous commands
			memset(dst + len, '\0', maxLen - len);
			m_out->xputchar('\n');
			*cur_len = 0;
			return len + 1;
		}
		else if (c == '\b') {
			if (len > 0) {
				print("\b \b");
				(*cur_len)--;
			}
		}
		else if (c == 0x1b) {
			uint16_t count = 1000; // counter to ensure that escape sequences are received
			do{
				//print("%X ", c);				
			}while (m_out->getCharNonBlocking(&c) || count--);
			

			switch (c) {
				case 0x41:  // UP arrow
					*cur_len = changeLine(dst, historyBack(), *cur_len);
					break;
				case 0x42:  // Down arrow
					*cur_len = changeLine(dst, historyForward(), *cur_len);
					break;
			}
#if CONSOLE_ENABLE_HISTORY_DUMP
		}else if (c == CONSOLE_HISTORY_LIST_KEY) {
			historyDump();
#endif
		}else if (len < maxLen) {
			m_out->xputchar(c);
			*(dst + len) = c;
			(*cur_len)++;
		}	
	}
	return 0;
}

char Console::getLine(char *dst, uint8_t maxLen)
{
	uint8_t len = 0, hasLine = 0;
	char c;

	while (!hasLine) {
		c = m_out->xgetchar();
		switch (c) {
		case '\b':
			if (len != 0) {
				m_out->xputchar(c);
				m_out->xputchar(' ');
				m_out->xputchar(c);
				len--;
			}
			break;

		case '\n':
		case '\r':
			hasLine = 1;
			m_out->xputchar('\n');
			break;

		case 0x1b:
			c = m_out->xgetchar();
			c = m_out->xgetchar();
			//print("%X ", c);
			switch (c) {
			case 0x41:  // [1B, 5B, 41] UP arrow
				len = changeLine(dst, historyBack(), len);
				break;
			case 0x42:  // [1B, 5B, 42] Down arrow
				len = changeLine(dst, historyForward(), len);
			default:
				break;
			}
			break;

		case '<':
			historyDump();
			hasLine = 1;
			break;

		default:
			if (len < maxLen) {
				m_out->xputchar(c);
				dst[len] = c;
				len++;
			}
			break;
		}
	}
	//Remove all extra text from previous commands
	memset(dst + len, '\0', maxLen - len);
	return len;
}

int Console::printf(const char* fmt, ...){
	va_list arp;
	va_start(arp, fmt);
	strformater(m_buf, fmt, arp);
	va_end(arp);
	return print(m_buf);
}

uint8_t Console::kbhit(void) {
	return m_out->kbhit();
}

char *Console::historyGet(void) {
	return &m_history[m_hist_cur][0];
}

void Console::historyDump(void) {
	for (uint8_t i = 0; i < CONSOLE_HISTORY_SIZE; i++)
	{
		printf("\n %u %s", i, m_history[i]);
	}
	m_out->xputchar('\n');
}

void Console::historyAdd(char *entry) {
	if (*m_line != '\n' && *m_line != '\r' && *m_line != '\0') {
		
		memcpy(m_history[m_hist_cur], entry, CONSOLE_COMMAND_MAX_LEN);

		if (++m_hist_cur == CONSOLE_HISTORY_SIZE)
			m_hist_cur = 0;		
		m_hist_idx = m_hist_cur;

		if (m_hist_size < CONSOLE_HISTORY_SIZE) {
			m_hist_size++;
		}
	}
}

char *Console::historyBack(void) {
	uint8_t new_idx = m_hist_idx;

	if (m_hist_size == CONSOLE_HISTORY_SIZE) {
		// History is full, wrap arround is allowed
		if (--new_idx > CONSOLE_HISTORY_SIZE)
			new_idx = CONSOLE_HISTORY_SIZE - 1;
		if (new_idx != m_hist_cur) {
			m_hist_idx = new_idx;
		}
	}
	else if(m_hist_idx > 0){
		m_hist_idx--;
	}

	return &m_history[m_hist_idx][0];
}

char *Console::historyForward(void) {
	if (m_hist_idx != m_hist_cur) {
		if (++m_hist_idx == CONSOLE_HISTORY_SIZE)
			m_hist_idx = 0;
	}
	
	if(m_hist_idx == m_hist_cur){
		// Clear current line to avoid duplicating m_history navigation
		memset(m_history[m_hist_cur], '\0', CONSOLE_COMMAND_MAX_LEN);
	}
	return &m_history[m_hist_idx][0];
}

void Console::historyClear(void) {
	for (uint8_t i = 0; i < CONSOLE_HISTORY_SIZE; i++)
	{
		memset(m_history[i], '\0', CONSOLE_COMMAND_MAX_LEN);
	}
	m_hist_idx = m_hist_cur = m_hist_size = 0;
}

uint8_t Console::changeLine(char *old_line, char *new_line, uint8_t old_line_len) {
	uint8_t new_line_len;

	while (old_line_len--) {
		print("\b \b");
	}
	
	new_line_len = print(new_line);

	memcpy(old_line, new_line, new_line_len);

	return new_line_len;
}


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
	memset(cmdList, '#', CONSOLE_MAX_COMMANDS * sizeof(ConsoleCommand*));
	memset(line, '\0', COMMAND_MAX_LEN);
	cmdListSize = 0;
	processing = NO;
	out = sp;
	prt = prompt;
	historyClear();	
	line_len = 0;
	//addCommand(&help); // since all cpp support is disabled, classes on .data section are not initialized
}

void Console::addCommand(ConsoleCommand *cmd) {
	if (cmd == NULL || cmdListSize == CONSOLE_MAX_COMMANDS)
	{
		xputs("Invalid command or command list full!");
		return;
	}

	cmd->init(this);
	cmdList[cmdListSize++] = cmd;	
}

void Console::registerCommandList(ConsoleCommand **list){
	while(*list != NULL){
		addCommand(*(list++));
	}
}

char Console::parseCommand(char *line) {
	char res = CMD_NOT_FOUND, *cmdname, *param;
	ConsoleCommand **cmd = cmdList;

	if (*line == '\n' || *line == '\r' || *line == '\0')
		return CMD_OK;

	cmdname = strsub(line, ' ', COMMAND_MAX_LEN, &param);

	for (uint8_t i = 0; i < cmdListSize; i++, cmd++) {
		if (*cmd == NULL) {
			break;
		}
		if ((*cmd)->isNameEqual(cmdname) != 0) {
			res = (*cmd)->execute((void*)param);
			break;
		}	
	}

	memset(line, '\0', COMMAND_MAX_LEN);

	if (res == CMD_NOT_FOUND) {
		xputs("Command not found\r");
	}else if (res == CMD_BAD_PARAM) {
		xputs("Bad parameter ");
	}else if(res == CMD_OK_LF){
		xputchar('\n');
	}

	return res;
}

void Console::process(void) {
	if(processing == NO){
		processing = YES;
		out->xputs(prt);
		return;
	}
#if defined(CONSOLE_BLOCKING)
	line_len = 0;
	line_len = getLine(line, COMMAND_MAX_LEN);	
#else
	if (getLineNonBlocking(line, &line_len, COMMAND_MAX_LEN)) 
#endif
	{
		historyAdd(line);
		if(parseCommand(line) != CMD_OK_NO_PRT){
			out->xputs(prt);
		}
	}
}

void Console::cls(void){
	out->xputs("\e[2J\r");	
}

/**
 * */
void Console::setOutput(StdOut *sp){
	out = sp;	
}

/**
 * libc compatible functions *
 * */
int Console::xputs(const char* str)
{
	out->xputs(str);
	out->xputchar('\n');
	return 1;
}

char *Console::xgets(char* str)
{
	uint8_t i = 0;
	char c;

	c = out->xgetchar();

	while ((c != '\n') && (c != '\r'))
	{
		*(str + i++) = c;		
		c = out->xgetchar();
	}
	*(str + i) = '\0';

	return str;
}

int Console::xputchar(int c) {
	out->xputchar(c);
	return (int)c;
}

int Console::xgetchar(void)
{
	char c = out->xgetchar();
	out->xputchar(c);
	return (int)c;
}

/**
 * Read a line ended by \n or \r from serial port.
 * Ending char is not added to line read *
 * */
char Console::getLineNonBlocking(char *dst, uint8_t *cur_len, uint8_t maxLen) {
	char c;
	uint8_t len;

	while (out->getCharNonBlocking(&c)) {
		len = *cur_len;
		
		if ((c == '\n') || (c == '\r')) {			
			//Remove all extra text from previous commands
			memset(dst + len, '\0', maxLen - len);
			out->xputchar(c);
			*cur_len = 0;
			return len + 1;
		}
		else if (c == '\b') {
			if (len > 0) {
				out->xputs("\b \b");
				(*cur_len)--;
			}
		}
		else if (c == 0x1b) {
			uint16_t count = 1000; // counter to ensure that escape sequences are received
			do{
				//print("%X ", c);				
			}while (out->getCharNonBlocking(&c) || count--);
			

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
			out->xputchar(c);
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
		c = out->xgetchar();
		switch (c) {
		case '\b':
			if (len != 0) {
				out->xputchar(c);
				out->xputchar(' ');
				out->xputchar(c);
				len--;
			}
			break;

		case '\n':
		case '\r':
			hasLine = 1;
			out->xputchar(c);
			break;

		case 0x1b:
			c = out->xgetchar();
			c = out->xgetchar();
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
				out->xputchar(c);
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

void Console::print(const char* fmt, ...){
	char buf[CONSOLE_PRINT_MAX_LEN];
	va_list arp;
	va_start(arp, fmt);
	strformater(buf, fmt, arp);
	va_end(arp);
	out->xputs(buf);
}

uint8_t Console::kbhit(void) {
	return out->kbhit();
}

char *Console::historyGet(void) {
	return &history[hist_cur][0];
}

void Console::historyDump(void) {
	for (uint8_t i = 0; i < HISTORY_SIZE; i++)
	{
		print("\n %u %s", i, history[i]);
	}
	out->xputchar('\n');
}

void Console::historyAdd(char *entry) {
	if (*line != '\n' && *line != '\r' && *line != '\0') {
		
		memcpy(history[hist_cur], entry, COMMAND_MAX_LEN);

		if (++hist_cur == HISTORY_SIZE)
			hist_cur = 0;		
		hist_idx = hist_cur;

		if (hist_size < HISTORY_SIZE) {
			hist_size++;
		}
	}
}

char *Console::historyBack(void) {
	uint8_t new_idx = hist_idx;

	if (hist_size == HISTORY_SIZE) {
		// History is full, wrap arround is allowed
		if (--new_idx > HISTORY_SIZE)
			new_idx = HISTORY_SIZE - 1;
		if (new_idx != hist_cur) {
			hist_idx = new_idx;
		}
	}
	else if(hist_idx > 0){
		hist_idx--;
	}

	return &history[hist_idx][0];
}

char *Console::historyForward(void) {
	if (hist_idx != hist_cur) {
		if (++hist_idx == HISTORY_SIZE)
			hist_idx = 0;
	}
	
	if(hist_idx == hist_cur){
		// Clear current line to avoid duplicating history navigation
		memset(history[hist_cur], '\0', COMMAND_MAX_LEN);
	}
	return &history[hist_idx][0];
}

void Console::historyClear(void) {
	for (uint8_t i = 0; i < HISTORY_SIZE; i++)
	{
		memset(history[i], '\0', COMMAND_MAX_LEN);
	}
	hist_idx = hist_cur = hist_size = 0;
}

uint8_t Console::changeLine(char *old_line, char *new_line, uint8_t old_line_len) {
	uint8_t new_line_len;
	while (old_line_len--) {
		out->xputs("\b \b");
	}

	out->xputs(new_line);
	new_line_len = strlen(new_line);

	memcpy(old_line, new_line, new_line_len);

	return new_line_len;
}

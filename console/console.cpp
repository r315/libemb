
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "console.h"

Console::Console(void) {

}

Console::Console(StdOut *sp, const char *prt) {
	init(sp, prt);
}

void Console::init(StdOut *sp, const char *prt) {
	memset(cmdList, '#', CONSOLE_MAX_COMMANDS * sizeof(ConsoleCommand*));
	memset(line, '\0', COMMAND_MAX_LEN);
	cmdListSize = 0;
	executing = NO;
	out = sp;
	prompt = prt;
	historyClear();	
	line_len = 0;
	//addCommand(&help); // since all cpp support is disabled, classes on .data section are not initialized
	print("\e[2J\r%s" ,prompt);
}

void Console::addCommand(ConsoleCommand *cmd) {
	if (cmd == NULL || cmdListSize == CONSOLE_MAX_COMMANDS)
	{
		puts("Invalid command or command list full!");
		return;
	}

	cmd->init(this);
	cmdList[cmdListSize++] = cmd;	
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
		puts("Command not found\r");
	}
	else if (res == CMD_BAD_PARAM) {
		puts("Bad parameter ");
	}

	return res;
}

void Console::process(void) {
	if (getLineNonBlocking(line, &line_len, COMMAND_MAX_LEN)) {
		historyAdd(line);
		parseCommand(line);
		print(prompt);
	}
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

char Console::getLineNonBlocking(char *dst, uint8_t *cur_len, uint8_t maxLen) {
	char c;
	uint8_t len;

	if (out->getCharNonBlocking(&c)) {
		len = *cur_len;
		
		if ((c == '\n') || (c == '\r')) {
			*(dst + (len++)) = '\0';
			out->xputchar(c);
			*cur_len = 0;
			return len;
		}
		else if (c == '\b') {
			if (len > 0) {
				out->xputs("\b \b");
				(*cur_len)--;
			}
		}
		else if (c == 0x1b) {
			while (out->getCharNonBlocking(&c)) {
				//print("%X ", c);				
			}

			switch (c) {
				case 0x41:  // UP arrow
					changeLine(historyBack());
					break;
				case 0x42:  // Down arrow
					changeLine(historyForward());
					break;
			}
		}
		else if (c == '_') {
			historyDump();		
		}else if (len < maxLen) {
			out->xputchar(c);
			*(dst + len) = c;
			(*cur_len)++;
		}	
	}
	return 0;
}

char Console::getline(char *dst, uint8_t max)
{
	uint8_t len = 0;
	char c;

	do {
		c = out->getchar();
		if (c == '\b') {
			if (len != 0) {
				out->xputchar(c);
				out->xputchar(' ');
				out->xputchar(c);
				len--;
			}
		}
		else {
			if (len < max) {
				*dst++ = c;
				out->xputchar(c);
				len++;
			}
		}
	} while ((c != '\n') && (c != '\r'));
	*dst = '\0';
	return len;
}

void Console::print(const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l, f;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			out->xputchar(d); continue;
		}
		d = *str++; w = r = s = l = 0;
		if (d == '.') {
			d = *str++; f = 1;
		}
		if (d == '0') {
			d = *str++; s = 1;
		}
		while ((d >= '0') && (d <= '9')) {
			w += w * 10 + (d - '0');
			d = *str++;
		}
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		if (!d) break;
		if (d == 's') {
			out->xputs(va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			out->xputchar((char)va_arg(arp, int));
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
		if (d == 'b') r = 2;
		if (d == 'f') {
			if (!f)
				w = 6;						// dafault 6 decimal places
			out->xputs(pftoa(va_arg(arp, double), w));
			continue;
		}
		if (!r) break;
		if (s) w = -w;
		if (l) {
			out->xputs(pitoa((long)va_arg(arp, long), r, w));
		}
		else {
			if (r > 0)
				out->xputs(pitoa((unsigned long)va_arg(arp, int), r, w));
			else
				out->xputs(pitoa((long)va_arg(arp, int), r, w));
		}
	}

	va_end(arp);
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
		xstrcpy(history[hist_cur], entry, COMMAND_MAX_LEN);

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

uint8_t Console::changeLine(char *new_line) {
	uint8_t i;
	while (line_len--) {
		out->xputs("\b \b");
	}

	out->xputs(new_line);
	line_len = strlen(new_line);

	for (i = 0; i < line_len; i++) {
		*(line + i) = *new_line++;
	}

	return line_len;
}

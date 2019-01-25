
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "console.h"

Console::Console(void) {

}

Console::Console(ConsoleOut *sp, const char *prt) {
	init(sp, prt);
}

void Console::init(ConsoleOut *sp, const char *prt) {
	memset(cmdList, '#', CONSOLE_MAX_COMMANDS * sizeof(ConsoleCommand*));
	memset(line, '\0', COMMAND_MAX_LEN);
	lineLen = 0;
	cmdListSize = 0;
	executing = NO;
	out = sp;
	prompt = prt;	
	//addCommand(&help); // since all cpp support is disabled, classes on .data section are not initialized
	print(prompt);
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

char Console::parseCommand(char *line, uint8_t len) {
	char res = CMD_NOT_FOUND, *cmdname, *param;
	ConsoleCommand **cmd = cmdList;

	if (len == 1) /* the should not exists commands with only one char */
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

	if (res == CMD_NOT_FOUND) {
		puts("Command not found\r");
	}
	else if (res == CMD_BAD_PARAM) {
		puts("Bad parameter ");
	}
	memset(line, '\0', COMMAND_MAX_LEN);
	return res;
}

void Console::process(void) {
	if (getLineNonBlocking(line, &lineLen, COMMAND_MAX_LEN)) {
		parseCommand(line, lineLen);
		print(prompt);
		lineLen = 0;
	}
}

/**
 * libc functions *
 * */
void Console::puts(const char* str)
{
	out->puts(str);
	out->putchar('\n');
}

void Console::gets(char* str)
{
	char c;
	c = out->getchar();
	while ((c != '\n') && (c != '\r'))
	{
		*str++ = c;
		c = out->getchar();
	}
	*str = '\0';
}

char Console::getchar(void)
{
	char c = out->getchar();
	out->putchar(c);
	return c;
}

char Console::getLineNonBlocking(char *line, uint8_t *curLen, uint8_t max) {
	char c;
	uint8_t len = 0;

	if (out->getCharNonBlocking(&c)) {
		len = *curLen;
		if (c == '\b') {
			if (len > 0) {
				out->putchar(c);
				out->putchar(' ');
				out->putchar(c);
				len--;
			}
		}
		else {
			if (len < max) {
				out->putchar(c);
				*(line + len) = c;
				len++;
			}
		}

		*curLen = len;

		if ((c == '\n') || (c == '\r')) {
			*(line + len) = '\0';
		}
		else {
			len = 0;
		}
	}
	return len;
}

char Console::getline(char *line, uint8_t max)
{
	uint8_t len = 0;
	char c;

	do {
		c = out->getchar();
		if (c == '\b') {
			if (len != 0) {
				out->putchar(c);
				out->putchar(' ');
				out->putchar(c);
				line--;
				len--;
			}
		}
		else {
			if (len < max) {
				out->putchar(c);
				*line++ = c;
				len++;
			}
		}
	} while ((c != '\n') && (c != '\r'));
	*line = '\0';
	return len;
}

void Console::print(const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l, f;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			out->putchar(d); continue;
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
			out->puts(va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			out->putchar((char)va_arg(arp, int));
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
		if (d == 'b') r = 2;
		if (d == 'f') {
			if (!f)
				w = 6;						// dafault 6 decimal places
			out->puts(pftoa(va_arg(arp, double), w));
			continue;
		}
		if (!r) break;
		if (s) w = -w;
		if (l) {
			out->puts(pitoa((long)va_arg(arp, long), r, w));
		}
		else {
			if (r > 0)
				out->puts(pitoa((unsigned long)va_arg(arp, int), r, w));
			else
				out->puts(pitoa((long)va_arg(arp, int), r, w));
		}
	}

	va_end(arp);
}


uint8_t Console::kbhit(void) {
	return out->kbhit();
}

void Console::putc(char c) {
	out->putchar(c);
}

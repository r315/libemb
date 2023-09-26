
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "console.h"

#define CONSOLE_ENABLE_HISTORY_DUMP 1
#define CONSOLE_HISTORY_LIST_KEY    '>'

Console::Console(void) {

}

Console::Console(stdout_t *out, const char *prompt) {
	init(out, prompt);
}

void Console::init(stdout_t *out, const char *prompt) {
	memset(m_cmdList, '#', CONSOLE_MAX_COMMANDS * sizeof(ConsoleCommand*));
	memset(m_line, '\0', CONSOLE_WIDTH);
	m_cmdListSize = 0;
	m_active = NO;
	m_out = out;
	m_prompt = prompt;
	m_line_len = 0;

    m_hist.init(out);
	m_hist.clear();	
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

	if(m_argc < CONSOLE_COMMAND_PARAMS && m_argv[0] != NULL){
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
	memset(line, '\0', CONSOLE_WIDTH);

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
	m_line_len = getLine(m_line, CONSOLE_WIDTH);	
#else
	if (getLineNonBlocking(m_line, &m_line_len, CONSOLE_WIDTH)) 
#endif
	{
		m_hist.push(m_line);
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
void Console::setOutput(stdout_t *sp){
	m_out = sp;	
}

/**
 * 
 * */
int Console::print(const char* str)
{
    int len = 0;

    while(*str){
        m_buf[len] = *str++;
        len++;
    }

    return m_out->write(m_buf, len);
}

int Console::println(const char* str)
{
    int len = 0;

    while(*str){
        m_buf[len] = *str++;
        len++;
    }

    m_buf[len++] = '\n';

    return m_out->write(m_buf, len);
}

char *Console::getString(char* str)
{
	uint8_t i = 0;
	char c;

	c = m_out->readchar();

	while ((c != '\n') && (c != '\r'))
	{
		*(str + i++) = c;		
		c = m_out->readchar();
	}
	*(str + i) = '\0';

	return str;
}

int Console::printchar(int c) {
	m_out->writechar(c);
	return (int)c;
}

int Console::getChar(void)
{
	char c = m_out->readchar();
	m_out->writechar(c);
	return (int)c;
}

char Console::getch(void)
{
	return m_out->readchar();
}

uint8_t Console::getchNonBlocking(char *c)
{
    if(m_out->available()){
        *c = m_out->readchar();
        return 1;
    }
    return 0;
}

/**
 * Read a line ended by \n or \r from serial port.
 * Ending char is not added to line read *
 * */
char Console::getLineNonBlocking(char *dst, uint8_t *cur_len, uint8_t maxLen) {
	char c;
	uint8_t len;

	while (m_out->available()) {
        c = m_out->readchar();
		len = *cur_len;
		
		if ((c == '\n') || (c == '\r')) {			
			//Remove all extra text from previous commands
			memset(dst + len, '\0', maxLen - len);
			m_out->writechar('\n');
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
				if(m_out->available())
                    c = m_out->readchar();
			}while (count--);
			

			switch (c) {
				case 0x41:  // UP arrow
					*cur_len = replaceCommandLine(dst, m_hist.back(), *cur_len);
					break;
				case 0x42:  // Down arrow
					*cur_len = replaceCommandLine(dst, m_hist.forward(), *cur_len);
					break;
			}
#if CONSOLE_ENABLE_HISTORY_DUMP
		}else if (c == CONSOLE_HISTORY_LIST_KEY && *cur_len == 0) {
            *dst = '\0';
			m_hist.dump();
            return 1;
#endif
		}else if (len < (maxLen - 1)) {
			m_out->writechar(c);
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
		c = m_out->readchar();
		switch (c) {
		case '\b':
			if (len != 0) {
				m_out->write("\b \b", 3);				
				len--;
			}
			break;

		case '\n':
		case '\r':
			hasLine = 1;
			m_out->writechar('\n');
			break;

		case 0x1b:
			c = m_out->readchar();
			c = m_out->readchar();
			//print("%X ", c);
			switch (c) {
			case 0x41:  // [1B, 5B, 41] UP arrow
				len = replaceCommandLine(dst, m_hist.back(), len);
				break;
			case 0x42:  // [1B, 5B, 42] Down arrow
				len = replaceCommandLine(dst, m_hist.forward(), len);
			default:
				break;
			}
			break;

		case '<':
			m_hist.dump();
			hasLine = 1;
			break;

		default:
			if (len < maxLen) {
				m_out->writechar(c);
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
    int len;
	va_start(arp, fmt);
	len = strformater(m_buf, fmt, arp);
	va_end(arp);

	return m_out->write(m_buf, len);
}

uint8_t Console::available(void) {
	return m_out->available();
}


History::History(void)
{
}

History::History(stdout_t *out)
{
    init(out);
}

void History::init(stdout_t *out)
{
    m_out = out;
}

char *History::pop(void) {
	return &m_history[m_top][0];
}

void History::push(const char *entry) {
	if (*entry != '\n' && *entry != '\r' && *entry != '\0') {
		
		memcpy(m_history[m_top], entry, CONSOLE_WIDTH);

		if (++m_top == HISTORY_MAX_SIZE)
			m_top = 0;

		m_idx = m_top;

		if (m_size < HISTORY_MAX_SIZE) {
			m_size++;
		}
	}
}

char *History::back(void) {
	uint8_t new_idx = m_idx;

	if (m_size == HISTORY_MAX_SIZE) {
		// History is full, wrap arround is allowed
		if (--new_idx > HISTORY_MAX_SIZE)
			new_idx = HISTORY_MAX_SIZE - 1;
		if (new_idx != m_top) {
			m_idx = new_idx;
		}
	}
	else if(m_idx > 0){
		m_idx--;
	}

	return &m_history[m_idx][0];
}

char *History::forward(void) {
	if (m_idx != m_top) {
		if (++m_idx == HISTORY_MAX_SIZE)
			m_idx = 0;
	}
	
	if(m_idx == m_top){
		// Clear current line to avoid duplicating m_history navigation
		memset(m_history[m_top], '\0', CONSOLE_WIDTH);
	}
	return &m_history[m_idx][0];
}

void History::clear(void) {
	for (uint8_t i = 0; i < HISTORY_MAX_SIZE; i++)
	{
		memset(m_history[i], '\0', CONSOLE_WIDTH);
	}
	m_idx = m_top = m_size = 0;
}

void History::dump(void) {
    m_out->writechar('\n');

	for (uint8_t i = 0; i < HISTORY_MAX_SIZE; i++)
	{
        //printf("\n%c %u %s", (i == m_top) ? '>' : ' ', i, m_history[i]);
        m_out->writechar('\n');
        m_out->writechar((i == m_top) ? '>' : ' ');
        m_out->writechar(' ');
        m_out->writechar('0' + i);
        m_out->writechar(' ');
        char *ptr = m_history[i];
        while(*ptr){
            m_out->writechar(*ptr++);
        }
	}
	
    m_out->writechar('\n');
    m_out->writechar('\n');
}

uint8_t Console::replaceCommandLine(char *old_line, char *new_line, uint8_t old_line_len) {
	uint8_t new_line_len;

	while (old_line_len--) {
		print("\b \b");
	}
	
	new_line_len = print(new_line);

	memcpy(old_line, new_line, new_line_len);

	return new_line_len;
}

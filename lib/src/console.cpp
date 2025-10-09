
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "console.h"

#define VT100_BOLD   "\e[1m"
#define VT100_NORMAL "\e[m"

#define CONSOLE_ENABLE_HISTORY_DUMP 0
#define CONSOLE_HISTORY_LIST_KEY    '>'

Console::Console(void) {

}

Console::Console(stdinout_t *out, const char *prompt) {
	init(out, prompt);
}

void Console::init(stdinout_t *out, const char *prompt) {
	memset(m_cmdList, '#', CONSOLE_MAX_COMMANDS * sizeof(ConsoleCommand*));
	memset(m_line, '\0', CONSOLE_WIDTH);
	m_cmdListSize = 0;
	m_active = NO;
	m_out = out;
	m_prompt = prompt;
	m_line_len = 0;
    m_line_edit = 0;
	m_hist.clear();
	//addCommand(&help); // since all cpp support is disabled, classes on .data section are not initialized
}

void Console::addCommand(ConsoleCommand *cmd) {
	if (cmd == NULL || m_cmdListSize == CONSOLE_MAX_COMMANDS)
	{
		this->print("Invalid command or command list full!\n");
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

	m_argc = strToArray((char*)line, m_argv, CONSOLE_COMMAND_PARAMS);

	if(m_argv[0] != NULL){
		cmdname = m_argv[0];
		for (uint8_t i = 0; i < m_cmdListSize; i++, cmd++) {
			if (*cmd == NULL) {
				break;
			}
            #ifdef CONSOLE_SIMPLE_COMMAND
            if(xstrcmp((*cmd)->name, cmdname)){
                res = (*cmd)->run(m_argc, m_argv);
            }
            #else
			if ((*cmd)->isNameEqual(cmdname) != 0) {
                if(m_argc > 1){
                    if(m_argv[1][0] == '?'){
                        (*cmd)->help();
                        res = CMD_OK;
                        break;
                    }
                }
				res = (*cmd)->execute(m_argc, m_argv);
				break;
			}
            #endif
		}
	}

	// Clear command line
	memset(line, '\0', CONSOLE_WIDTH);

	if (res == CMD_NOT_FOUND) {
		this->print("Command not found\n");
	}else if (res == CMD_BAD_PARAM) {
		this->print("Bad parameter\n");
	}else if(res == CMD_OK_LF){
		writechar('\n');
	}

	return res;
}

void Console::process(void) {
	if(m_active == NO){
		m_active = YES;
		this->printf(
            VT100_BOLD
            "%s"
            VT100_NORMAL
            ,m_prompt
        );
		return;
	}
#if defined(CONSOLE_BLOCKING)
	m_line_len = 0;
	m_line_len = getLine(m_line, CONSOLE_WIDTH);
#else
	if (scanForLine() == CON_LINE)
#endif
	{
		m_hist.push(m_line);
		if(parseCommand(m_line) != CMD_OK_NO_PRT){
			this->printf(
                VT100_BOLD
                "%s"
                VT100_NORMAL
                ,m_prompt
            );
		}
        // clear line after reading line and parsing it
		memset(m_line, '\0', CONSOLE_WIDTH);
        m_line_len = 0;
        m_line_edit = 0;
	}
}

void Console::cls(void){
	print("\e[2J\r");
}

/**
 * */
void Console::setOutput(stdinout_t *sp){
	m_out = sp;
}

/**
 * @brief
 *
 * @param addr
 * @param len
 */
void Console::asciiprint(const uint8_t *addr, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++){
        if(*addr > (' '-1) && *addr < 0x7F){
            this->printf("%c", *addr);
        }else{
            writechar('.');
        }
        addr++;
    }
}

/**
 * @brief Prints memory raw bytes in a single line
 * @param addr Pointer to first byte
 * @param len   Number of bytes to print
 * @param ascii  Print ascci after byes
 */
void Console::hexprint(const uint8_t *addr, uint32_t len, uint8_t ascii)
{
    uint32_t i;

    for(i= 0 ; i < len; i++){
		this->printf("%02X ",*(addr + i));
	}

    if(ascii){
        asciiprint(addr, len);
	}

	writechar('\n');
}

void Console::hexdump(const uint8_t *mem, uint32_t len, uint8_t ncols, uint8_t ascii)
{
	for(uint32_t i = 0; i < len; i += ncols){
		this->printf("%02X: ",i);
        uint32_t count = len - i;
        if(count > ncols){
            count = ncols;
            hexprint(mem, count, ascii);
        }else{
            for(uint32_t k= 0 ; k < count; k++){
                this->printf("%02X ", *(mem + k));
            }

            for(uint32_t k= 0 ; k < ncols - count; k++){
                this->printf("   ");
            }

            if(ascii){
                asciiprint(mem, count);
            }

            writechar('\n');
        }
		mem += ncols;
	}
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

int Console::printf(const char* fmt, ...)
{
	va_list arp;
    int len;
	va_start(arp, fmt);
	len = strformater(m_buf, fmt, CONSOLE_WIDTH, arp);
	va_end(arp);

	return m_out->write(m_buf, len);
}

int Console::printchar(int c) {
	writechar(c);
	return (int)c;
}

char *Console::getString(char* str)
{
	uint8_t i = 0;
	char c;

	c = readchar();

	while ((c != '\n') && (c != '\r'))
	{
		*(str + i++) = c;
		c = readchar();
	}
	*(str + i) = '\0';

	return str;
}

int Console::getChar(void)
{
	char c = readchar();
	writechar(c);
	return (int)c;
}

char Console::getch(void)
{
	return readchar();
}

uint8_t Console::getchNonBlocking(char *c)
{
    if(m_out->available()){
        *c = readchar();
        return 1;
    }
    return 0;
}

/**
 * Read a line ended by \n or \r from serial port.
 * Ending char is not added to line read *
 * */

/**
 * @brief Checks if a line has been entered by user
 * A line is valid if \n character is entered by user.
 * Ending line character is not included in line
 *
 * @return line scan state
 */
con_res_t Console::scanForLine(void) {
	char c;
	static uint8_t EscSeq = 0;

	while (m_out->available()) {
        c = readchar();

        // Handle escape sequences
        if(c == '\e'){
            EscSeq = 1;
            continue;
        }

        if(EscSeq == 1){
            if(c == '['){
                EscSeq = 2;
                continue;
            }else{
                EscSeq = 0;
            }
        }

        if(EscSeq == 2){
            switch (c) {
				case 'A':  // UP arrow
					replaceLine(m_hist.back());
					break;

				case 'B':  // Down arrow
					replaceLine(m_hist.forward());
					break;

                case 'C':
                    //puts ("RIGTH");
                    if(m_line_edit > 0){
                        this->printf("\e[1C");
                        m_line_edit--;
                    }
                    break;

                case 'D':
                    //puts ("LEFT");
                    if(m_line_edit < m_line_len){
                        this->printf("\e[1D");
                        m_line_edit++;
                    }
                    break;
			}
            EscSeq = 0;
            continue;
        }

        // Handle line ending
		if ((c == '\n') || (c == '\r')) {
            m_line[m_line_len] = '\0';
			writechar('\n');
            return CON_LINE;
		}

        // Handle backspace
		if (c == '\b') {
			if (m_line_len > 0) {
                if(!m_line_edit){
    				m_out->write("\b \b", 3);
                }else{
                    uint8_t offset = m_line_len - m_line_edit;
                    // move cursor one character to left
                    writechar('\b');
                    // Move and print remaning string
                    for(uint8_t i = 0; i < m_line_edit; i++){
                        m_line[offset + i - 1] = m_line[offset + i];
                        writechar(m_line[offset + i]);
                    }
                    // Erase character at the end of line
                    writechar(' ');
                    // Move cursor back to edit position
                    this->printf("\e[%uD", m_line_edit + 1);
                }
                m_line_len--;
			}
            continue;
		}

#if CONSOLE_ENABLE_HISTORY_DUMP
		if (c == CONSOLE_HISTORY_LIST_KEY && m_line_len == 0) {
            *m_line = '\0';
			m_hist.dump();
            return CON_LINE;
        }
#endif
        // Ignore TAB, implement tab completion??
        if (c == '\t'){
            continue;
        }
        // Handle normal character
		if (m_line_len < (CONSOLE_WIDTH - 1)) {
            if(!m_line_edit){
                // append character to end of line
			    writechar(c);
			    m_line[m_line_len++] = c;
            }else{
                // Insert character on index given by m_line_edit
                uint8_t offset = m_line_len - m_line_edit;
                // Shift all characters right by one,
                // starting from end to m_line_edit
                for(uint8_t i = 0; i < m_line_edit; i++){
                    m_line[m_line_len - i] = m_line[m_line_len - 1 - i];
                }
                // Insert entered character
                m_line[offset] = c;
                // Only needed to print line from edit position to end
                // plus inserted character
                m_out->write(m_line + offset, m_line_edit + 1);
                // Move cursor back to edit position
                this->printf("\e[%uD", m_line_edit);
                m_line_len++;
            }
		}
    }

	return CON_IDLE;
}


char Console::getLine(char *dst, uint8_t maxLen)
{
	char c;
	uint8_t hasLine = 0;
    m_line_len = 0;

	while (!hasLine) {
		c = readchar();
		switch (c) {
		case '\b':
			if (m_line_len != 0) {
				m_out->write("\b \b", 3);
				m_line_len--;
			}
			break;

		case '\n':
		case '\r':
			hasLine = 1;
			writechar('\n');
			break;

		case 0x1b:
			c = readchar();
			c = readchar();
			//print("%X ", c);
			switch (c) {
			case 0x41:  // [1B, 5B, 41] UP arrow
				replaceLine(m_hist.back());
				break;
			case 0x42:  // [1B, 5B, 42] Down arrow
				replaceLine(m_hist.forward());
			default:
				break;
			}
			break;
#if CONSOLE_ENABLE_HISTORY_DUMP
		case '<':
			m_hist.dump();
			hasLine = 1;
			break;
#endif
		default:
			if (m_line_len < maxLen) {
				writechar(c);
				dst[m_line_len] = c;
				m_line_len++;
			}
			break;
		}
	}
	//Remove all extra text from previous commands
	memset(dst + m_line_len, '\0', maxLen - m_line_len);
	return m_line_len;
}

uint8_t Console::available(void)
{
	return m_out->available();
}

char Console::readchar(void)
{
    char c;
    m_out->read(&c, 1);
    return c;
}

void Console::writechar(char c)
{
    m_out->write((const char*)&c, 1);
}

void Console::replaceLine(char *new_line) {
    int new_line_len;

    new_line_len = strlen((const char*)new_line);

    if(new_line_len > 0 && new_line_len < CONSOLE_WIDTH){
    	memcpy(m_line, new_line, new_line_len);

      if(m_line_edit){
        this->printf("\e[%uC", m_line_edit);
         m_line_edit = 0;
      }

      while(m_line_len--){
        this->printf("\b \b");
      }

      m_line_len = new_line_len;

      this->printf("%s", new_line);
    }
}

History::History(void)
{
    memset(m_history, 0, sizeof(m_history));
	m_top = m_idx = m_size = 0;
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
#if 0
void History::dump(void) {
    writechar('\n');

	for (uint8_t i = 0; i < HISTORY_MAX_SIZE; i++)
	{
        //this->printf("\n%c %u %s", (i == m_top) ? '>' : ' ', i, m_history[i]);
        writechar('\n');
        writechar((i == m_top) ? '>' : ' ');
        writechar(' ');
        writechar('0' + i);
        writechar(' ');
        char *ptr = m_history[i];
        while(*ptr){
            writechar(*ptr++);
        }
	}

    m_out->write("\n\n", 2);
}
#endif


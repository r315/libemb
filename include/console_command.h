#ifndef _command_h_
#define _command_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include "strfunc.h"

enum {
	CMD_OK,
	CMD_NOT_FOUND,
	CMD_BAD_PARAM
};

class ConsoleCommand {
private:
	const char *name;

public:
	const char *getName(void) { return this->name; }
	char *toString(void) { return (char*)this->name; }
	char isNameEqual(char *cmd) { return xstrcmp(cmd, (char*)this->name) == 0; }

	virtual void init(void *params) {}
	virtual char execute(void *ptr) { return CMD_OK; }
	virtual void help(void) {}

	ConsoleCommand() {  }
	ConsoleCommand(const char *nm) {
		this->name = nm;
	}
};


#ifdef __cplusplus
}
#endif

#endif
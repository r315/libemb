#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cli_simple.h"

static uint8_t CliLineBuffer[CLI_LINE_MAX_LEN];
static uint16_t CliLineLen;
static uint8_t *CliArgv[CLI_CMD_MAX_ARGS];
static uint32_t CliArgc;

static const char *Prompt;
static const cli_command_t *CliCommands;
static uint16_t CliCommandsCount;

static cli_history_t History;

extern int kbhit (void);

// =============================================================================
// CLI_SkipSpaces
// =============================================================================
/*!
 *
 * Skips spaces until next character of a string.
 * 
 * \param Str    - Pointer to some string
 * \param MaxLen - Input string max length
 * 
 * \return       - uint8_t pointer to next character on string 
 *                 or begin of string if no spaces were found
 *
 */
// =============================================================================
static uint8_t * CLI_SkipSpaces (uint8_t *Str, uint32_t MaxLen)
{
   uint8_t *Start = Str;

   while (*Start == ' ' || *Start == '\t')
   {
      if(Start == Str + MaxLen || *Start == '\0')
      {
         // No spaces found return string start
         return Str;
      }
      Start++;
   }

   return Start;
}

// =============================================================================
// GetCommand
// =============================================================================
/*!
 *
 * Searches the command on the command list
 * 
 * \param - Buffer  String containing command name
 * 
 * \return - cli_command_t*  Pointer to command structure
 *
 */
// =============================================================================
static cli_command_t * CLI_GetCommand(uint8_t *Buffer)
{
   for (uint8_t i = 0; i < CliCommandsCount; i++)
   {
      if(!strcmp(CliCommands[i].name, (const char*) Buffer))
      {
         return (cli_command_t *)&CliCommands[i];
      }
   }

   return NULL;
}

// =============================================================================
// CLI_GetArguments
// =============================================================================
/*!
 *
 * Splits a command line string into arguments
 * 
 * \param - Buffer  String containing command and arguments
 * \param - Argv    Arguments output array 
 * 
 * \return - Number of arguments
 *
 */
// =============================================================================
static uint32_t CLI_GetArguments(uint8_t *Buffer, uint8_t **Argv)
{
    uint8_t *Start, *End, ArgvIndex;

    ArgvIndex = 0;

    // Skip any spaces before command

    Start = CLI_SkipSpaces (Buffer, CLI_LINE_MAX_LEN);

    // Check for empty line

    if(*Start == '\0')
    {
        return 0;
    }
   
    Argv[ArgvIndex++] = End = Start;

    do{
        if(*End == ' ' || *End == '\t')
        {
            // Split argument
            *End = '\0';
            End = CLI_SkipSpaces(End + 1, CLI_LINE_MAX_LEN);
            Argv[ArgvIndex++] = End;
            continue;
        }
     
        End++;
    }while(*End != '\0' && End != Start + CLI_LINE_MAX_LEN);

    return ArgvIndex;
}

// =============================================================================
// CLI_ReplaceLine
// =============================================================================
/*!
 *
 * Replace current line on console
 * 
 * \param - new_line    Replacing line
 * 
 * \return - Replacing line length
 *
 */
// =============================================================================
static uint8_t CLI_ReplaceLine(uint8_t *new_line) {
	uint8_t new_line_len;

    int len = CliLineLen;
	
    while(len--){
        printf("\b \b");
    }
    
	new_line_len = printf("%s", new_line);
	memcpy(CliLineBuffer, new_line, new_line_len);

	return new_line_len;
}

// =============================================================================
// CLI_Prompt
// =============================================================================
/*!
 *
 * Prints cli prompt
 * 
 * \param - None
 * 
 * \return - void
 *
 */
// =============================================================================
static void CLI_Prompt (void)
{
   printf(Prompt);
}

// =============================================================================
// CLI_Commands
// =============================================================================
/*!
 *
 * Prints registered commands alias
 * 
 * 
 * \param - void
 * 
 * 
 * \return - cli_result_t
 *
 */
// =============================================================================
static void CLI_HistoryInit(cli_history_t *Hist)
{
   uint16_t Index;

   Hist->head = 0;
   Hist->index = 0;
   Hist->size = 0;

   for(Index = 0; Index < CLI_HISTORY_SIZE; Index++){
      memset(Hist->history[Index], '\0', CLI_LINE_MAX_LEN);
   }
}

// =============================================================================
// CLI_HistoryDump
// =============================================================================
/*!
 *
 * Prints registered history list
 * 
 * 
 * \param - Hist    history structure
 * 
 * 
 * \return - void
 *
 */
// =============================================================================
static void CLI_HistoryDump(cli_history_t *Hist) {
   uint16_t Index;

	for (Index = 0; Index < CLI_HISTORY_SIZE; Index++)
	{
		printf("\n%c %u %s", (Index == Hist->head) ? '>' : ' ', Index, Hist->history[Index]);
	}

	putchar('\n');
    putchar('\n');
}


// =============================================================================
// CLI_HistoryAdd
// =============================================================================
/*!
 *
 * Adds entry to history
 * 
 * 
 * \param - Hist    history structure
 * \param - line    Line to be added
 * 
 * \return - void
 *
 */
// =============================================================================
static void CLI_HistoryAdd(cli_history_t *Hist, uint8_t *line)
{
   uint16_t Index;

   if (*line != '\n' && *line != '\r' && *line != '\0') {
		
      for(Index = 0; Index < CLI_LINE_MAX_LEN - 1; Index++)
      {
         if(line[Index] == '\0')
         {
            break;
         }

         Hist->history[Hist->head][Index] = line[Index];
      }

      Hist->history[Hist->head][Index] = '\0';

      Hist->head = (Hist->head + 1) % CLI_HISTORY_SIZE;
	
	  Hist->index = Hist->head;

	  if (Hist->size < CLI_HISTORY_SIZE) 
      {
         Hist->size++;
      }
   }
}

// =============================================================================
// CLI_HistoryGet
// =============================================================================
/*!
 *
 * Returns history entry relative to current one
 * 
 * 
 * \param - Hist    history structure
 * \param - Dir     Direction -1 previous entry, 1 next entry, 0 current entry
 * 
 * \return - void   selected entry
 *
 */
// =============================================================================
static uint8_t *CLI_HistoryGet(cli_history_t *Hist, int8_t Dir)
{
   uint16_t CurIndex;
   
   CurIndex = Hist->index;
   
   if(Dir == -1)
   {

      if (Hist->size == CLI_HISTORY_SIZE) 
      {
         // History is full, wrap arround is allowed
         if (--CurIndex > CLI_HISTORY_SIZE)
         {
            CurIndex = CLI_HISTORY_SIZE - 1;
         }

         // Stop going back if we are back on current entry
         if (CurIndex != Hist->head) 
         {
            Hist->index = CurIndex;
         }
      }
      else if(Hist->index > 0)
      {
         Hist->index--;
      }	   
   }
   else if (Dir == 1)
   {
      if (CurIndex != Hist->head) {
         CurIndex = (CurIndex + 1) % CLI_HISTORY_SIZE;
      }
      
      if(CurIndex == Hist->head){
         // Clear current line to avoid duplicating history navigation
         memset(Hist->history[Hist->head], '\0', CLI_LINE_MAX_LEN);
      }

      Hist->index = CurIndex;
   }

   return Hist->history[Hist->index];
}

// =============================================================================
// CLI_History
// =============================================================================
/*!
 *
 * Wrapper for CLI_HistoryDump
 * 
 * 
 * \param - void
 * 
 * 
 * \return - cli_result_t
 *
 */
// =============================================================================
int CLI_History(void)
{
    CLI_HistoryDump(&History);
    return CLI_OK;
}

// =============================================================================
// CLI_Init
// =============================================================================
/*!
 *
 * Initialize command line interface
 * 
 * 
 * \param - Prompt 
 * 
 * \return - void
 *
 */
// =============================================================================
void CLI_Init (const char *prompt)
{
   if (prompt == NULL)
   {
      return;
   }

   memset (CliLineBuffer, 0xAA, sizeof (CliLineBuffer));
   CliLineLen = 0;

   Prompt = prompt;

   setvbuf(stdout, NULL, _IONBF, 0); // make stdout non-buffered, so that printf always calls __io_putchar
   
   CLI_Prompt ();

   CLI_HistoryInit(&History);
}

// =============================================================================
// CLI_RegisterCommand
// =============================================================================
/*!
 *
 * Registers commands that can be executed by cli
 * 
 * 
 * \param - Commands    List of commands 
 * \param - Count       Number of commands in list
 * 
 * \return - void
 *
 */
// =============================================================================
void CLI_RegisterCommand (const cli_command_t *Commands, uint8_t Count)
{
   if(Commands)
   {
      CliCommands = Commands;
      CliCommandsCount = Count;
   }
}

// =============================================================================
// CLI_Commands
// =============================================================================
/*!
 *
 * Prints registered commands alias
 * 
 * 
 * \param - void
 * 
 * 
 * \return - cli_result_t
 *
 */
// =============================================================================
int CLI_Commands (void)
{
   for (int i = 0; i < CliCommandsCount; i++)
   {
      printf("\n %s", CliCommands[i].name);
   }

   putchar('\n');
   putchar('\n');

   return CLI_OK;
}

// =============================================================================
// CLI_HandleLine
// =============================================================================
/*!
 *
 * Handle command input from user by spliting it into arguments and 
 * executing the corresponding command
 * 
 * \param - None
 * 
 * \return - cli_result_t
 *
 */
// =============================================================================
cli_result_t CLI_HandleLine (void)
{
   cli_result_t Res = CLI_CMD_NOT_FOUND;

   CLI_HistoryAdd(&History, CliLineBuffer);

   CliArgc = CLI_GetArguments(CliLineBuffer, CliArgv);

   const cli_command_t *Cmd = CLI_GetCommand(CliArgv[0]);

   if(Cmd != NULL){
      Res = Cmd->exec(CliArgc, (const char**)CliArgv);
   }

   switch(Res){
      case CLI_CMD_NOT_FOUND:
         if(CliLineLen)
         {
            puts("command not found");
         }
         break;

      case CLI_BAD_PARAM:
         puts("Invalid parameter");
         break;
      
      default:
         break;
   }

   // Parse splits initial line, it must be cleared in all lenght
   memset (CliLineBuffer, '\0', CLI_LINE_MAX_LEN);
   CliLineLen = 0;

   CLI_Prompt ();

   return Res;
}

// =============================================================================
// CLI_ReadLine
// =============================================================================
/*!
 *
 * Read input line by user, this call is not blocking
 * 
 * 
 * \param - void
 * 
 * \return - cli_result_t
 *
 */
// =============================================================================
cli_result_t CLI_ReadLine (void)
{
   static uint8_t EscSeq = 0;

   if (kbhit ())
   {
      uint8_t Data = getchar ();

      if (EscSeq == 1)
      {
         if (Data == '[')
         {
            EscSeq++;
            return CLI_OK;
         }
         EscSeq = 0;
      }
      else if (EscSeq == 2)
      {
         switch (Data)
         {
            case 'A':
               //puts ("UP");
               CliLineLen = CLI_ReplaceLine(CLI_HistoryGet(&History, -1));
               break;

            case 'B':
               //puts ("DOWN");
               CliLineLen = CLI_ReplaceLine(CLI_HistoryGet(&History, 1));
               break;

            case 'C':
               //puts ("RIGTH");
               break;
            case 'D':
               //puts ("LEFT");
               break;
            default:
               break;
         }
         EscSeq = 0;
         return CLI_OK;
      }

      switch (Data)
      {
         case 0:
            break;

         case '\e':
         {
            EscSeq++;
            break;
         }

         case 0x7F:
         case '\b':
         {
            if (CliLineLen > 0)
            {
               putchar('\b');
               putchar(' ');
               putchar('\b');
               CliLineLen--;
            }
            break;
         }

         case '\r':
         {
            CliLineBuffer[CliLineLen] = '\0';
            puts ("");
            return CLI_LINE_READ;
         }

         default:
         {
            if (CliLineLen < sizeof (CliLineBuffer))
            {
               CliLineBuffer[CliLineLen++] = Data;
               putchar (Data);
            }
            break;
         }
      }
   }
   return CLI_OK;
}

#ifndef INC_CLI_H_
#define INC_CLI_H_

#include "nmea.h"
#include "main.h"


#include <stdbool.h>


#define CLI_BUF_SIZE 64

typedef struct {
    uint8_t  len;
    char     buf[CLI_BUF_SIZE];
} cli_command_t;


void cli_rx_byte_handler(const uint8_t data);
void cli_process_commands(void);
void cli_send(const char *msg);


extern cli_command_t cli_command;
extern uint8_t cli_raw_enabled;

#endif /* INC_CLI_H_ */

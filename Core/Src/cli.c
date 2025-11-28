#include "cli.h"


cli_command_t cli_command = {0};
uint8_t cli_raw_enabled = 0;



char* cli_skip_start_spaces(char *letter) {

	while(*letter == ' ' || *letter == '\t') {
		letter++;
	}
	return letter;
}


void cli_send(const char *msg) {

    uint8_t old = cli_raw_enabled;
    cli_raw_enabled = 0;

    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);

    cli_raw_enabled = old;
}


void cli_process_commands(void)
{
    char *cmd = cli_command.buf;

    cmd = cli_skip_start_spaces(cmd);

    char buffer[256];

    if (strcasecmp(cmd, "VER") == 0)
        {
            snprintf(buffer, sizeof(buffer),
                     "VER - RESPONSE: built=%s %s uart=%lu\r\n",
                     __DATE__, __TIME__, huart1.Init.BaudRate);
            cli_send(buffer);
            return;
        }

        else if (strcasecmp(cmd, "GET POS") == 0)
        {
            snprintf(buffer, sizeof(buffer),
                     "POS - RESPONSE: Lat=%.6f Lon=%.6f Fix=%d Age_ms=%lu\r\n",
                     gps_position.lat_degree,
                     gps_position.lon_degree,
                     gps_position.status,
                     gps_position.last_update_ms);
            cli_send(buffer);
            return;
        }

        else if (strcasecmp(cmd, "GET TIME") == 0)
        {
            snprintf(buffer, sizeof(buffer),
                     "TIME - RESPONSE: UTC %02d:%02d:%02d Stale=%d\r\n",
                     gps_position.hour,
                     gps_position.minutes,
                     gps_position.second,
                     gps_position.stale);
            cli_send(buffer);
            return;
        }

        else if (strcasecmp(cmd, "RAW ON") == 0)
        {
            cli_raw_enabled = 1;
            cli_send("RAW STREAM: ENABLED\r\n");
            return;
        }

        else if (strcasecmp(cmd, "RAW OFF") == 0)
        {
            cli_raw_enabled = 0;
            cli_send("RAW STREAM: DISABLED\r\n");
            return;
        }
}


void cli_rx_byte_handler(const uint8_t data) {

	 if (data == '\n' || data == '\r') {
		 if (cli_command.len > 0) {
			 cli_command.buf[cli_command.len] = '\0';
			 cli_process_commands();
			 cli_command.len = 0;
		 }
		 return;
	 }

	 if (cli_command.len < CLI_BUF_SIZE - 1) {

		 cli_command.buf[cli_command.len++] = data;

	 } else {
		 cli_command.len = 0;
	 }

	 return;
}

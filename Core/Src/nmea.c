#include "nmea.h"


nmea_line_t nmea_line = {0};
position_gps_t gps_position = {0};


bool check_sum(const char *line) {

	const char* start = strchr(line, '$');
	const char* end = strchr(line, '*');

	if (start == NULL || end == NULL) {
		return false;
	}

	uint8_t cs = 0;
	const char *letter = start + 1;

	while (letter < end) {

		cs ^= (uint8_t)*letter;
		letter++;
	}

	if (*(end + 1) == '\0' || *(end + 2) == '\0')
	    return false;

    char hex_str[3] = { *(end + 1), *(end + 2), '\0' };

    uint8_t expected = (uint8_t)strtol(hex_str, NULL, 16);

    return cs == expected;

}

float nmea_coord_to_degree(float ddmm, char direction) {

	float deg = ddmm / 100.0f;
	float min = ddmm - deg * 100.0f;
	float decimal_deg = deg + min / 60.0f;

	if (direction == 'S' || direction == 'W') {
		decimal_deg = -decimal_deg;
	}

	return decimal_deg;
}

void parse_rmc_data(const char *line) {

	char save_buffer[200];
	memset(save_buffer, 0, sizeof(save_buffer));
	snprintf(save_buffer, sizeof(save_buffer), "%s", line);

	float utc_raw =  0.0f;
	float lat_ddmm = 0.0f;
	char n_or_s = 'N';
	float lon_ddmm = 0.0f;
	char e_or_w = 'E';

	char *p = save_buffer;
	char *token;
	int current_pos = 0;

	while ((token = strsep(&p, ",")) != NULL) {

		switch(current_pos) {
			case 1:
				utc_raw = strtod(token, NULL);
				break;
			case 3:
				lat_ddmm = strtod(token, NULL);
				break;
	        case 4:
	            n_or_s = token[0];
	            break;

	        case 5:
	            lon_ddmm = strtod(token, NULL);
	            break;

	        case 6:
	            e_or_w = token[0];
	            break;
		}
		current_pos++;
	}


	 gps_position.hour = (int)(utc_raw / 10000);
	 gps_position.minutes = (int)((utc_raw / 100) - gps_position.hour * 100);
	 gps_position.second  = (int)(utc_raw - gps_position.hour * 10000 - gps_position.minutes * 100);
	 gps_position.lat_degree = nmea_coord_to_degree(lat_ddmm, n_or_s);
	 gps_position.lon_degree = nmea_coord_to_degree(lon_ddmm, e_or_w);
	 gps_position.hemi_lat = n_or_s;
	 gps_position.hemi_lon = e_or_w;
	 gps_position.status = true;
	 gps_position.last_update_ms = HAL_GetTick();
	 gps_position.stale = true;
}



void process_nmea_line(const char *line) {

	if (strstr(line, "GNRMC") != NULL){

		if (strstr(line, ",A,") != NULL) { // if status Active
				parse_rmc_data(line);
				uint32_t age_ms = HAL_GetTick() - gps_position.last_update_ms;

				if (age_ms > 10000) {
					gps_position.stale = false;

				}

			} else {
				gps_position.stale = false;
				gps_position.status = false;         // status V → no fix
		}
	}
}


void process_bytes_nmea(const uint8_t *data, const uint16_t data_len)
{
    for (uint16_t i = 0; i < data_len; i++)
    {
        uint8_t letter = data[i];

        if (letter == '$')
        {
            nmea_line.len = 0;
            nmea_line.buf[nmea_line.len++] = '$';
            continue;
        }

        if (nmea_line.len == 0)
            continue;

        if (letter == '\r')
            continue;

        if (letter == '\n')
        {
            nmea_line.buf[nmea_line.len] = 0;
            const char *line = (const char*)nmea_line.buf;


           if (cli_raw_enabled) {
               HAL_UART_Transmit(&huart1, (uint8_t*)line, strlen(line), 100);
               HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
           }



            if (check_sum(line)) {
            	process_nmea_line(line);
            }

            nmea_line.len = 0;
            continue;
        }

        if (nmea_line.len < NMEA_LINE_BUF_SIZE - 1)
        {
            nmea_line.buf[nmea_line.len++] = letter;
        }
        else
        {
            nmea_line.len = 0;
        }
    }
}


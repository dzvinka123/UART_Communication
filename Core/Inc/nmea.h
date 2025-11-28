#ifndef INC_NMEA_H_
#define INC_NMEA_H_

#include "gps.h"
#include "cli.h"


#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#define NMEA_LINE_BUF_SIZE 128



typedef struct {
    uint16_t len;
    uint8_t buf[NMEA_LINE_BUF_SIZE];
} nmea_line_t;


typedef struct {
	uint32_t last_update_ms;
	uint8_t hour;
	uint8_t minutes;
	uint8_t second;
	float lat_degree;
	float lon_degree;
	char  hemi_lat;      // 'N' or 'S'
	char  hemi_lon; // 'E' or 'W'
	bool status;
	bool stale;
} position_gps_t;


void process_bytes_nmea(const uint8_t  *data, const uint16_t data_len);
bool check_sum(const char *line) ;
void process_nmea_line(const char *line);
void parse_rmc_data(const char *line);




extern nmea_line_t nmea_line;
extern position_gps_t gps_position;


#endif /* INC_NMEA_H_ */

#ifndef INC_GPS_H_
#define INC_GPS_H_

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define GPS_DMA_BUF_SIZE 1024
#define GPS_CHUNK_QUEUE_LEN 64


typedef struct {
    uint16_t lenght;
    uint8_t  data[GPS_DMA_BUF_SIZE];
} gps_chunk;


typedef struct {
	gps_chunk buffer[GPS_CHUNK_QUEUE_LEN];
    uint8_t head;
    uint8_t tail;
    uint8_t count;

} gps_chunk_queue;

typedef struct {
    uint32_t bytes;
    uint32_t chunks;     // All portion (IDLE)
    uint32_t fe_err;     // Framing Error
    uint32_t ne_err;     // Noise Error
    uint32_t ore_err;    // Overrun Error
    uint32_t pe_err;     // Parity Error
} gps_stats_t;


extern UART_HandleTypeDef *gps_huart;
extern uint8_t gps_dma_buf[GPS_DMA_BUF_SIZE];
extern gps_chunk_queue gps_queue;
extern gps_stats_t gps_stats;




void gps_init(UART_HandleTypeDef *huart);

bool gps_push(gps_chunk_queue *queue, const uint8_t *data, uint16_t len);
bool gps_pop(gps_chunk_queue *queue, gps_chunk *pocessed_chunk);




#endif /* INC_GPS_H_ */

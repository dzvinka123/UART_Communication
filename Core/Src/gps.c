#include "gps.h"




UART_HandleTypeDef *gps_huart = NULL;


uint8_t gps_dma_buf[GPS_DMA_BUF_SIZE];

gps_chunk_queue gps_queue = {0};
gps_stats_t gps_stats = {0};




void gps_init(UART_HandleTypeDef *huart) {

	gps_huart = huart;
	HAL_UART_Receive_DMA(gps_huart, gps_dma_buf, GPS_DMA_BUF_SIZE);
	__HAL_UART_ENABLE_IT(gps_huart, UART_IT_IDLE);

}


bool gps_push(gps_chunk_queue *queue, const uint8_t *data, uint16_t len) {

	if (queue->count < GPS_CHUNK_QUEUE_LEN) {

		gps_chunk *new_chunk = &queue->buffer[queue->tail];

		if (len > GPS_DMA_BUF_SIZE) {
			len = GPS_DMA_BUF_SIZE;
		}

		new_chunk->lenght = len;
		memcpy(new_chunk->data, data, len);

		queue->tail = (queue->tail + 1) % GPS_CHUNK_QUEUE_LEN;
		queue->count++;
		return true;
	}
	else {
		return false;
	}

}

bool gps_pop(gps_chunk_queue *queue, gps_chunk *pocessed_chunk) {

	if (queue->count > 0) {
		gps_chunk *chunk_src = &queue->buffer[queue->head];

		pocessed_chunk->lenght = chunk_src->lenght;
		memcpy(pocessed_chunk->data, chunk_src->data, chunk_src->lenght);

		queue->head = (queue->head + 1) % GPS_CHUNK_QUEUE_LEN;

		queue->count--;

		return true;
	}
	else {
		return false;
	}


}



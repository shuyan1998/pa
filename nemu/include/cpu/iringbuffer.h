#ifndef __CPU_IRINGBUFFER_H

#include <stdint.h>
#define IRINGBUFFER_SIZE 50

typedef struct {
  char data[128];
}logbuf;

typedef struct {
  logbuf buffer[IRINGBUFFER_SIZE];
  uint32_t length;
  uint32_t read;
  uint32_t write;
}ringbuffer_t;

void init_ringbuffer(ringbuffer_t *ringbuffer, uint32_t size);
void write_ringbuffer(ringbuffer_t *ringbuffer, logbuf *logbuffer);
void read_ringbuffer_all(ringbuffer_t *ringbuffer);

#endif
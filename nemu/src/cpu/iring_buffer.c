#include <stdint.h>
#include <string.h>
#include "utils.h"
#include <cpu/iringbuffer.h>

void init_ringbuffer(ringbuffer_t *ringbuffer, uint32_t size) {
  ringbuffer->length = size;
  ringbuffer->read = 0;
  ringbuffer->write = 0;
}

void write_ringbuffer(ringbuffer_t *ringbuffer, logbuf *logbuffer) {
  logbuffer->data[strlen(logbuffer->data)] = '\0';
  char formated_data[128] = {"   "};
  if(nemu_state.state == NEMU_ABORT || (nemu_state.state == NEMU_END && nemu_state.halt_ret != 0)){
    formated_data[0] = '-';
    formated_data[1] = '-';
    formated_data[2] = '>';
  }
  for(int i=0; i<strlen(logbuffer->data)+ 1 ; i++) {
    formated_data[i+3] = logbuffer->data[i];
  }
  memcpy(&ringbuffer->buffer[ringbuffer->write % ringbuffer->length], formated_data, strlen(formated_data)+1);
  ringbuffer->write ++;
}

void read_ringbuffer_all(ringbuffer_t *ringbuffer) {
  for(int i=0; i<IRINGBUFFER_SIZE; i++){
    puts(ringbuffer->buffer[i].data);
  }
}
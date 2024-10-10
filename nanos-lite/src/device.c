#include <common.h>
#include <stdio.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for(int i = 0; i < len; i++) {
    putch(*((char*)buf + i));
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if(ev.keycode == AM_KEY_NONE){
    return 0;
  }
  return snprintf((char*)buf, len, "%s %s", ev.keydown? "kd": "ku", keyname[ev.keycode]);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  return snprintf((char*)buf, len, "WIDTH:%d\nHEIGHT:%d\n", config.width, config.height);
}

/* write len count data from buf to offset of frame buffer
  buf: the source data to write from
  offset: the offset of frame buffer to write to
  len: the length of data to write (It should be a row in ndl - NDL_DrawRect)
*/
size_t fb_write(const void *buf, size_t offset, size_t len) {
  // get screen width and height
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  int width = config.width;
  
  offset /= 4; // convert offset to byte offset
  len /= 4; // convert len to byte len
  int x = offset % width;
  int y = offset / width;
  // AM_GPU_FBDRAW: int x, y; void *pixels; int w, h; bool sync
  io_write(AM_GPU_FBDRAW, x, y, (void *)buf, len, 1, true);
  
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}

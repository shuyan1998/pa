#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#define buf_size 1024

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static int canvas_x = 0, canvas_y = 0;

uint32_t NDL_GetTicks() {
  struct timeval now;
  gettimeofday(&now, NULL);
  uint32_t us = now.tv_sec * 1000000 + now.tv_usec;
  return us;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0, 0);
  int ret = read(fd, buf, len);
  assert(close(fd) == 0);
  
  return ret == 0 ? 0 : 1;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  
  // read from /dev/dispinfo to get screen width and height
  char *buf = (char*)malloc(buf_size * sizeof(char));
  int fd = open("/proc/dispinfo", 0, 0);
  int ret = read(fd, buf, buf_size);
  assert(ret < buf_size);
  assert(close(fd) == 0);
  
  int i = 0;
  int width = 0, height = 0;
  /// check 'WIDTH'
  assert(strncmp(&buf[i], "WIDTH:", 6) == 0);
  i += 6;
  for(;i < buf_size; i++) {
    if(buf[i] >= '0' && buf[i] <= '9') {
      width = width * 10 + buf[i] - '0';
    }
    else {
      break;
    }
  }
  assert(buf[i++] == '\n');
  // check 'HEIGHT'
  assert(strncmp(buf + i, "HEIGHT:", 7) == 0);
  i += 7;
  for(;i < buf_size; i++) {
    if(buf[i] >= '0' && buf[i] <= '9') {
      height = height * 10 + buf[i] - '0';
    }
    else {
      break;
    }
  }

  free(buf);
  screen_w = width; screen_h = height;
  if(*w == 0 && *h == 0) {
    *w = width; *h = height;
  }
  canvas_w = *w;canvas_h = *h;
  // set canvas position to centor of screen
  canvas_x = (screen_w - canvas_w) / 2;
  canvas_y = (screen_h - canvas_h) / 2;
  printf("screen_w: %d, screen_h: %d\n", screen_w, screen_h);
  printf("canvas_x: %d, canvas_y: %d\n", canvas_x, canvas_y);
  
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}

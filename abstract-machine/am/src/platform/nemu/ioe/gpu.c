#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  int i;
  int size = inl(VGACTL_ADDR);
  int w = size >> 16;  // TODO: get the correct width
  int h = size & 0x0000FFFF;  // TODO: get the correct height
  printf("++++++++++++ w is %d, h is %d\n", w, h);
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  int size = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = size >> 16, .height = size & 0x0000FFFF,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // if (ctl->sync) {
  //   outl(SYNC_ADDR, 1);
  // }
  // AM_DEVREG(11, GPU_FBDRAW,   WR, int x, y; void *pixels; int w, h; bool sync);
  int x = ctl->x;
  int y = ctl->y;
  int w = ctl->w;
  int h= ctl->h;
  if (!ctl->sync && (w == 0 || h == 0))
    return;
  int *pixels = ctl->pixels;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;

  int size = inl(VGACTL_ADDR);
  int width = size >> 16;
  int height = size & 0x0000FFFF;
  for(int j = y; j < y + h; j++) {
    for(int i = x; i < x + w; i++) {
      fb[width * j + i] = pixels[w * (j - y) + i - x];
    }
  }
  if(ctl->sync){
    outl(SYNC_ADDR, 1);
  }
}


void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

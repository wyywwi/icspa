#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  int i;
  int w = inw(VGACTL_ADDR + 2);
  int h = inw(VGACTL_ADDR);
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for(i = 0;i<w*h;i++)fb[i] = i;
  outl(SYNC_ADDR,1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = inw(VGACTL_ADDR + 2), .height = inw(VGACTL_ADDR),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t *pixels = ctl->pixels;
  int w = inw(VGACTL_ADDR);
  //int h = inw(VGACTL_ADDR);
  int base = ctl->y * w + ctl->x;
  for(int i = 0;i<ctl->w;i++){
    for(int j = 0;j<ctl->h;j++){
      fb[base + j*w + i] = pixels[j*ctl->w + i];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

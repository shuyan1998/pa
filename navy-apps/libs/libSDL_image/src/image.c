#include "sdl-video.h"
#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  printf("filename is %s\n", filename);
  SDL_Surface *surface = NULL;
  FILE* fd = fopen(filename, "r");
  assert(fd != NULL);

  fseek(fd, 0, SEEK_END);
  size_t size = ftell(fd);
  char* buffer = (char*)SDL_malloc(size * sizeof(char));

  fseek(fd, 0, SEEK_SET);
  fread(buffer, 1, size, fd);

  printf("size is %d\n", size);

  printf("44444444444444444\n");
  surface = STBIMG_LoadFromMemory((const unsigned char*)buffer, size);
  printf("55555555555555555\n");

  fclose(fd);
  SDL_free(buffer);
  
  return surface;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}

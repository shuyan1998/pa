#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <stdint.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

#define KEYNUM sizeof(keyname) / sizeof(keyname[0])
uint8_t key_status[KEYNUM] = {0};
int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];
  if(NDL_PollEvent(buf, sizeof(buf))) {
    char key_type;
    char key_buf[32];
    if(buf[0] == 'k') {
      assert(sscanf(buf, "k%c %s", &key_type, key_buf) == 2);
      switch (key_type) {
        case 'u': ev->type = SDL_KEYUP; break;
        case 'd': ev->type = SDL_KEYDOWN; break;
        default: assert(0);
      }
    }
    for(int i=0;i < KEYNUM; i++) {
      if(strcmp(key_buf, keyname[i]) == 0) {
        ev->key.keysym.sym = i;
        key_status[i] = !ev->type;
        return 1;
      }
    }
  }
  
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  while(true) {
    if(SDL_PollEvent(event)) {
      return 1;
    }
  }
  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}

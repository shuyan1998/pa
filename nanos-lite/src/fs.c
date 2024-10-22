#include "am.h"
#include "amdev.h"
#include "debug.h"
#include "klib-macros.h"
#include "sys/_intsup.h"
#include <fs.h>
#include <memory.h>
#include <stdint.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_DISPINFO, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_EVENT] = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
#include "files.h"
};

#define NR_FILES 32
int fs_open(const char *path, int flags, int mode) {
  for(int i = 3; i < NR_FILES; i++) {
    if(strcmp(path, file_table[i].name) == 0) {
      //printf("Found file %s, index is %d.\n", path, i);
      return i;
    }
  }

  panic("File %s not found.", path);
}

int fs_close(int fd) {
  file_table[fd].open_offset = 0;
  return 0;
}

size_t fs_read(int fd, void *buf, size_t count) {
  // ignore stdin, stdout, stderr for now
  // if(fd <= 2) {
  //   Log("Ignore read from %s.\n", file_table[fd].name);
  //   return 0;
  // }

  size_t read_len = count;
  size_t file_size = file_table[fd].size;
  size_t file_offset = file_table[fd].disk_offset;
  size_t read_offset = file_table[fd].open_offset;

  // read from ramdisk or device
  if(file_table[fd].read == NULL) {
    // some check
    if(read_offset >= file_size) {
      return 0;
    };
    if(read_offset + read_len > file_size) {
      read_len = file_size - read_offset;
    }

    ramdisk_read(buf, file_offset + read_offset, read_len);
    file_table[fd].open_offset += read_len;
  }
  else{
    //printf("read from device\n");
    read_len = file_table[fd].read(buf, file_offset + read_offset, read_len);
  }

  return read_len;
}

size_t fs_write(int fd, void *buf, size_t count) {
  // if(fd == 0) {
  //   Log("Ignore write to %s.\n", file_table[fd].name);
  // }
  // if(fd == 1 || fd == 2) {
  //   for(int i = 0; i < count; i++) {
  //     putch(*((char*)buf + i));
  //   }
  //   return count;
  // }
  
  size_t write_len = count;
  size_t file_size = file_table[fd].size;
  size_t file_offset = file_table[fd].disk_offset;
  size_t write_offset = file_table[fd].open_offset;

  // write to ramdisk or device
  if(file_table[fd].write == NULL) {
    // some check
    if(write_offset >= file_size) {
      return 0;
    };
    if(write_offset + write_len > file_size) {
      write_len = file_size - write_offset;
    }
    ramdisk_write(buf, file_offset + write_offset, write_len);
    file_table[fd].open_offset += write_len;
  }
  else{
    //printf("write to device\n");
    file_table[fd].write(buf, file_offset + write_offset, write_len);
  }

  return write_len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  if(fd <= 2) {
    Log("Ignore lseek on %s.\n", file_table[fd].name);
    return 0;
  }

  Finfo *finfo = &file_table[fd];
  size_t new_offset;
  if(whence == SEEK_SET) {
    new_offset = offset;
  }
  else if(whence == SEEK_CUR) {
    new_offset = finfo->open_offset + offset;
  }
  else if(whence == SEEK_END) {
    new_offset = finfo->size + offset;
  }
  else{
    Log("Invalid whence value %d.", whence);
    return -1;
  }

  if (new_offset < 0 || new_offset > finfo->size) {
    Log("Seek position out of bound.");
    return -1;
  }

  // set new file offset
  finfo->open_offset = new_offset;
  return new_offset;
  
}
void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  int screen_width = config.width;
  int screen_height = config.height;
  int fb_size = screen_width * screen_height * sizeof(uint32_t);
  file_table[FD_FB].size = fb_size;
}

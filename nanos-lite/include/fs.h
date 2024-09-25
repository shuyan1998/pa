#ifndef __FS_H__
#define __FS_H__

#include "am.h"
#include <common.h>

#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif

int fs_open(const char *path, int flags, int mode);
int fs_close(int fd);
size_t fs_read(int fd, void *buf, size_t count);
size_t fs_write(int fd, void *buf, size_t count);
size_t fs_lseek(int fd, size_t offset, int whence);

#endif

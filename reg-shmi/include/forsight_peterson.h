#ifndef PETERSON_Z_
#define PETERSON_Z_

#include <stdint.h>
#include <stddef.h>

struct peterson {
    uint8_t read;
    uint8_t write;
    uint8_t turn;
};

#define READ_PTS 0x01
#define WRITE_PTS 0x02
#define OK_PTS 0x0

void tryLockPts(struct peterson *pts, uint8_t op);
void unlockPts(struct peterson *pts, uint8_t op);

void readPts(void *dest, void *src, size_t size, struct peterson *pts);
void writePts(void *dest, void *src, size_t size, struct peterson *pts);

#endif

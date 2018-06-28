#include <string.h>
#include <stddef.h>
#include "reg-shmi/forsight_peterson.h"

#define OTHER_OPERATION(x) (((x) == READ_PTS) ? WRITE_PTS : READ_PTS)

void tryLockPts(struct peterson *pts, uint8_t op) {
    switch (op) {
    case READ_PTS:
        pts->read = READ_PTS;
        pts->turn = WRITE_PTS;
        while((pts->write == WRITE_PTS) && (pts->turn == WRITE_PTS));
        return;
    case WRITE_PTS:
        pts->write = WRITE_PTS;
        pts->turn = READ_PTS;
        while((pts->read == READ_PTS) && (pts->turn == READ_PTS));
        return;
    }
}

void unlockPts(struct peterson *pts, uint8_t op) {
    switch (op) {
    case READ_PTS:
        pts->read = OK_PTS;
        return;
    case WRITE_PTS:
        pts->write = OK_PTS;
        return;
    }
}

void readPts(void *dest, void *src, size_t size, struct peterson *pts) {
	pts->read = READ_PTS;
	pts->turn = WRITE_PTS;
	while((pts->write == WRITE_PTS) && (pts->turn == WRITE_PTS));
	memcpy(dest, src, size);
	pts->read = OK_PTS;
}

void writePts(void *dest, void *src, size_t size, struct peterson *pts) {
	pts->write = WRITE_PTS;
	pts->turn = READ_PTS;
	while((pts->read == READ_PTS) && (pts->turn == READ_PTS));
	pts->write = OK_PTS;
}
